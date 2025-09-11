#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_system.h"

#include "editor.h"
#include "display.h"
#include "sdcard.h"
#include "usb_host_stub.h"

static const char *TAG = "main";
static editor_t editor;
static TimerHandle_t autosave_timer;
static char current_file[32];

// Forward declaration
static void autosave_callback(TimerHandle_t xTimer);

/**
 * @brief Save editor buffer to the current file.
 */
static void save_current_file(void) {
    if (editor_get_length(&editor) == 0) {
        ESP_LOGW(TAG, "Nothing to save");
        return;
    }
    sdcard_save_text(current_file, editor_get_buffer(&editor));
    ESP_LOGI(TAG, "File saved: %s", current_file);
}

static void autosave_callback(TimerHandle_t xTimer) {
    save_current_file();
}

/**
 * @brief Read a character from available input sources.
 *
 * This function first checks the (stub) USB host keyboard, then falls
 * back to the UART console.
 */
static int read_input_char(void) {
    int c = usb_host_keyboard_getchar();
    if (c >= 0) {
        return c;
    }
    return getchar();
}

/**
 * @brief Main editor loop. Reads characters and updates display.
 */
static void editor_loop(void) {
    while (1) {
        int c = read_input_char();
        if (c < 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (c == 0x13) { // Ctrl+S
            save_current_file();
            continue;
        }
        if (c == 0x05) { // Ctrl+E toggles Hemingway mode
            editor_set_hemingway(&editor, !editor.hemingway_mode);
            ESP_LOGI(TAG, "Hemingway mode: %s", editor.hemingway_mode ? "ON" : "OFF");
            continue;
        }

        editor_insert_char(&editor, (char)c);
        display_show_text(editor_get_buffer(&editor));
    }
}

void app_main(void) {
    esp_log_level_set(TAG, ESP_LOG_INFO);

    editor_init(&editor);
    display_init();
    sdcard_init();
    usb_host_keyboard_init();

    // Start screen
    display_show_text("Hemingway Editor\nN - New File\nO - Open File\n");
    int c;
    while ((c = read_input_char())) {
        if (c == 'n' || c == 'N') {
            break; // create new file
        } else if (c == 'o' || c == 'O') {
            display_show_text("Open file not implemented\nPress N\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    editor_clear(&editor);
    sdcard_generate_filename(current_file, sizeof(current_file));
    ESP_LOGI(TAG, "Editing %s", current_file);
    display_show_text("Start typing...\n(Ctrl+S to save)\n");

    // Create autosave timer (default 30s)
    autosave_timer = xTimerCreate("autosave", pdMS_TO_TICKS(30000), pdTRUE, NULL, autosave_callback);
    if (autosave_timer) {
        xTimerStart(autosave_timer, 0);
    }

    editor_loop();
}

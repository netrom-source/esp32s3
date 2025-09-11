#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "editor.h"
#include "display.h"
#include "sdcard.h"
#include "encoder.h"

#define UART_PORT UART_NUM_0

static const char *TAG = "main";
static editor_t editor;
static TimerHandle_t autosave_timer;
static char current_file[32];

typedef enum {UI_EDITOR, UI_MENU_MAIN, UI_MENU_SETTINGS} ui_state_t;
static ui_state_t ui_state = UI_EDITOR;

typedef enum {NAV_SCROLL, NAV_CURSOR} nav_mode_t;
static nav_mode_t nav_mode = NAV_SCROLL;

static font_size_t font_size = FONT_SMALL;
static size_t top_line = 0;
static int menu_index = 0;

static void autosave_callback(TimerHandle_t xTimer) {
    sdcard_save_text(current_file, editor_get_buffer(&editor));
    editor_mark_saved(&editor);
    ESP_LOGI(TAG, "Autosaved");
}

static size_t compute_total_lines(void) {
    int cols = OLED_WIDTH / display_char_width(font_size);
    size_t lines = 1, col = 0;
    for (size_t i = 0; i < editor_get_length(&editor); ++i) {
        char ch = editor.buffer[i];
        if (ch == '\n') { lines++; col = 0; }
        else {
            col++;
            if (col >= cols) { lines++; col = 0; }
        }
    }
    return lines;
}

static void render_editor(void) {
    display_clear();
    size_t line, col;
    editor_get_cursor(&editor, &line, &col);
    char status[64];
    snprintf(status, sizeof(status), "%s %d:%d%s%s", current_file, (int)line, (int)col,
             editor_is_dirty(&editor) ? "*" : "", editor.hemingway_mode ? " H" : "");
    display_draw_string(0, 0, status, FONT_SMALL);

    int text_y = display_line_height(FONT_SMALL);
    int line_h = display_line_height(font_size);
    int char_w = display_char_width(font_size);
    int max_cols = OLED_WIDTH / char_w;
    int max_lines = (OLED_HEIGHT - 2 * display_line_height(FONT_SMALL)) / line_h;

    size_t idx = 0;
    size_t vis_line = 0;
    while (idx < editor_get_length(&editor) && vis_line < top_line + max_lines) {
        char linebuf[128];
        size_t count = 0;
        size_t start = idx;
        while (idx < editor_get_length(&editor)) {
            char ch = editor.buffer[idx];
            if (ch == '\n') { idx++; break; }
            if (count == max_cols) break;
            count++; idx++;
        }
        size_t len = idx - start;
        memcpy(linebuf, &editor.buffer[start], len);
        linebuf[len] = 0;
        if (vis_line >= top_line) {
            int y = text_y + (vis_line - top_line) * line_h;
            display_draw_string(0, y, linebuf, font_size);
        }
        vis_line++;
    }

    display_draw_string(0, OLED_HEIGHT - display_line_height(FONT_SMALL),
                        "rot=scroll click=mode long=menu", FONT_SMALL);
    display_flush();
}

static void render_menu(const char *const *items, int count, const char *title) {
    display_clear();
    display_draw_string(0, 0, title, FONT_SMALL);
    int y = display_line_height(FONT_SMALL);
    for (int i = 0; i < count; ++i) {
        char linebuf[32];
        snprintf(linebuf, sizeof(linebuf), "%c %s", (i == menu_index) ? '>' : ' ', items[i]);
        display_draw_string(0, y, linebuf, FONT_SMALL);
        y += display_line_height(FONT_SMALL);
    }
    display_flush();
}

static void open_dummy(void) {
    const char *dummy = "Dummy file\nUse serial to edit.";
    editor_clear(&editor);
    strncpy(editor.buffer, dummy, EDITOR_BUFFER_SIZE - 1);
    editor.length = strlen(dummy);
    editor.cursor = editor.length;
    editor.dirty = false;
}

void app_main(void) {
    editor_init(&editor);
    sdcard_init();
    display_init();
    encoder_init(4, 5, 6);

    uart_driver_install(UART_PORT, 256, 0, 0, NULL, 0);

    sdcard_generate_filename(current_file, sizeof(current_file));
    autosave_timer = xTimerCreate("autosave", pdMS_TO_TICKS(30000), pdTRUE, NULL, autosave_callback);
    xTimerStart(autosave_timer, 0);

    while (1) {
        uint8_t ch;
        if (uart_read_bytes(UART_PORT, &ch, 1, 0) > 0 && ui_state == UI_EDITOR) {
            editor_insert_char(&editor, (char)ch);
        }

        int d = encoder_get_delta();
        encoder_button_t btn = encoder_get_button();

        if (ui_state == UI_EDITOR) {
            if (d != 0) {
                if (nav_mode == NAV_SCROLL) {
                    size_t total = compute_total_lines();
                    int newtop = (int)top_line - d;
                    if (newtop < 0) newtop = 0;
                    if (newtop > (int)total) newtop = total;
                    top_line = newtop;
                } else {
                    editor_move_cursor(&editor, d);
                }
            }
            if (btn == ENCODER_BTN_CLICK) {
                nav_mode = (nav_mode == NAV_SCROLL) ? NAV_CURSOR : NAV_SCROLL;
            } else if (btn == ENCODER_BTN_LONG) {
                ui_state = UI_MENU_MAIN;
                menu_index = 0;
            }
            render_editor();
        } else if (ui_state == UI_MENU_MAIN) {
            static const char *items[] = {"New", "Open", "Save", "Settings", "Exit"};
            int count = 5;
            if (d != 0) {
                menu_index = (menu_index + d + count) % count;
            }
            if (btn == ENCODER_BTN_LONG) {
                ui_state = UI_EDITOR;
            } else if (btn == ENCODER_BTN_CLICK) {
                switch (menu_index) {
                    case 0:
                        editor_clear(&editor);
                        top_line = 0;
                        ui_state = UI_EDITOR;
                        break;
                    case 1:
                        open_dummy();
                        ui_state = UI_EDITOR;
                        break;
                    case 2:
                        sdcard_save_text(current_file, editor_get_buffer(&editor));
                        editor_mark_saved(&editor);
                        ui_state = UI_EDITOR;
                        break;
                    case 3:
                        ui_state = UI_MENU_SETTINGS;
                        menu_index = 0;
                        break;
                    case 4:
                        ui_state = UI_EDITOR;
                        break;
                }
            }
            render_menu(items, count, "Menu");
        } else if (ui_state == UI_MENU_SETTINGS) {
            char heming[20];
            snprintf(heming, sizeof(heming), "Hemingway: %s", editor.hemingway_mode ? "On" : "Off");
            const char *items[] = {"Font Small", "Font Medium", "Font Large", heming, "Back"};
            int count = 5;
            if (d != 0) menu_index = (menu_index + d + count) % count;
            if (btn == ENCODER_BTN_LONG) {
                ui_state = UI_MENU_MAIN;
                menu_index = 0;
            } else if (btn == ENCODER_BTN_CLICK) {
                switch (menu_index) {
                    case 0:
                        font_size = FONT_SMALL;
                        break;
                    case 1:
                        font_size = FONT_MEDIUM;
                        break;
                    case 2:
                        font_size = FONT_LARGE;
                        break;
                    case 3:
                        editor_set_hemingway(&editor, !editor.hemingway_mode);
                        break;
                    case 4:
                        ui_state = UI_MENU_MAIN;
                        menu_index = 0;
                        break;
                }
            }
            render_menu(items, count, "Settings");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


#include "display.h"
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "display";

void display_init(void) {
    // This is a minimal stub that only logs initialization.
    // On real hardware you would set up I2C and the SSD1306 driver here.
    ESP_LOGI(TAG, "Display initialized (stub)");
}

void display_clear(void) {
    // Clear serial terminal (ANSI escape). Works in idf.py monitor and Wokwi.
    printf("\x1B[2J\x1B[H");
}

void display_show_text(const char *text) {
    display_clear();
    printf("%s", text);
    fflush(stdout);
}

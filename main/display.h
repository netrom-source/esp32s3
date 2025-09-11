#pragma once

/**
 * @brief Initialize the OLED display (SSD1306).
 *
 * On Wokwi the display is optional. If not present the driver prints
 * the text to the UART console for debugging.
 */
void display_init(void);

/**
 * @brief Clear the display contents.
 */
void display_clear(void);

/**
 * @brief Show text starting at the first line of the display.
 *
 * The function simply prints the text to the screen. Newlines are
 * supported. Long lines are wrapped by the driver.
 */
void display_show_text(const char *text);

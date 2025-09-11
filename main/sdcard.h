#pragma once

#include <stddef.h>
#include "esp_err.h"

/**
 * @brief Initialize and mount the SD card using the FATFS driver.
 *
 * Returns ESP_OK on success.
 */
esp_err_t sdcard_init(void);

/**
 * @brief Generate a filename for a new text file.
 *
 * The name is based on a simple counter and stored in the provided buffer.
 */
void sdcard_generate_filename(char *buffer, size_t buflen);

/**
 * @brief Save a buffer of text to the SD card.
 */
esp_err_t sdcard_save_text(const char *path, const char *text);

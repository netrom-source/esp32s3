#include "sdcard.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "sdcard";
static int file_counter = 0; /**< Used for filename generation */

esp_err_t sdcard_init(void) {
    esp_err_t ret;
    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // This example uses 1-bit mode for compatibility with Wokwi's SD card.
    slot_config.width = 1;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "SD card mounted");
    return ESP_OK;
}

void sdcard_generate_filename(char *buffer, size_t buflen) {
    snprintf(buffer, buflen, "/sdcard/file%03d.txt", file_counter++);
}

esp_err_t sdcard_save_text(const char *path, const char *text) {
    ESP_LOGI(TAG, "Saving to %s", path);
    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fwrite(text, 1, strlen(text), f);
    fclose(f);
    return ESP_OK;
}

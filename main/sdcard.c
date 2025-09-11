#include "sdcard.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "storage";

/**
 * @brief Initialize SPIFFS filesystem used for storing notes.
 */
esp_err_t sdcard_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS: %s", esp_err_to_name(ret));
        return ret;
    }
    size_t total = 0, used = 0;
    esp_spiffs_info(NULL, &total, &used);
    ESP_LOGI(TAG, "SPIFFS mounted, %u/%u bytes used", (unsigned)used, (unsigned)total);
    return ESP_OK;
}

/**
 * @brief Generate filename. For this demo we always use note.txt
 */
void sdcard_generate_filename(char *buffer, size_t buflen) {
    snprintf(buffer, buflen, "/spiffs/note.txt");
}

/**
 * @brief Save text buffer to SPIFFS.
 */
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

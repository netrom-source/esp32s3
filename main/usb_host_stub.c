#include "usb_host_stub.h"
#include "esp_log.h"

static const char *TAG = "usb_stub";

void usb_host_keyboard_init(void) {
    // Stub for USB host initialization. Real implementation would
    // configure the USB host stack and attach to a keyboard.
    ESP_LOGI(TAG, "USB host keyboard stub initialized");
}

int usb_host_keyboard_getchar(void) {
    // No USB host support in Wokwi, so we simply return -1.
    return -1;
}

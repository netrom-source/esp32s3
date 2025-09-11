#include "encoder.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"

static int pinA, pinB, pinSW;
static volatile int8_t delta = 0;
static volatile uint8_t state = 0;
static volatile encoder_button_t button_event = ENCODER_BTN_NONE;
static volatile int64_t press_time = 0;

static void IRAM_ATTR encoder_isr(void *arg) {
    uint8_t a = gpio_get_level(pinA);
    uint8_t b = gpio_get_level(pinB);
    state = ((state << 2) | (a << 1) | b) & 0x0F;
    static const int8_t table[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    delta += table[state];
}

static void IRAM_ATTR button_isr(void *arg) {
    int level = gpio_get_level(pinSW);
    int64_t now = esp_timer_get_time();
    static int64_t last = 0;
    if (now - last < 20000) return; // debounce 20ms
    last = now;
    if (level == 0) {
        press_time = now;
    } else {
        int64_t dur = now - press_time;
        if (dur > 700000) button_event = ENCODER_BTN_LONG;
        else button_event = ENCODER_BTN_CLICK;
    }
}

void encoder_init(int a, int b, int sw) {
    pinA = a; pinB = b; pinSW = sw;
    gpio_config_t io = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<<a)|(1ULL<<b)|(1ULL<<sw),
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io);
    gpio_set_intr_type(pinA, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(pinB, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(pinSW, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(pinA, encoder_isr, NULL);
    gpio_isr_handler_add(pinB, encoder_isr, NULL);
    gpio_isr_handler_add(pinSW, button_isr, NULL);
}

int encoder_get_delta(void) {
    int d = delta;
    delta = 0;
    return d;
}

encoder_button_t encoder_get_button(void) {
    encoder_button_t e = button_event;
    button_event = ENCODER_BTN_NONE;
    return e;
}


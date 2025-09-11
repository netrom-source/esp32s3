#pragma once

#include <stdbool.h>

typedef enum {
    ENCODER_BTN_NONE = 0,
    ENCODER_BTN_CLICK,
    ENCODER_BTN_LONG
} encoder_button_t;

void encoder_init(int pin_a, int pin_b, int pin_sw);
int encoder_get_delta(void);
encoder_button_t encoder_get_button(void);


#pragma once

/**
 * @brief Initialize USB host keyboard subsystem (stub).
 */
void usb_host_keyboard_init(void);

/**
 * @brief Poll USB host keyboard for new characters (stub).
 *
 * Should return -1 if no character is available.
 */
int usb_host_keyboard_getchar(void);

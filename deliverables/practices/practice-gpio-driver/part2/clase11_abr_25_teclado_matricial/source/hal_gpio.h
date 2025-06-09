/*
 * hal_gpio.h
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

void hal_gpio_keypad_init(void);
char hal_gpio_keypad_getkey(void);

void hal_gpio_led_init(void);
void hal_gpio_led_set(int value);

void hal_gpio_init_timer(void);

#endif

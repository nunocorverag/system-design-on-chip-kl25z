/*
 * hal_gpio.h
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

void keypad_init(void);

char keypad_getkey(void);

void LED_init(void);

void LED_set(int value);

void hal_gpio_init_timer(void);

#endif

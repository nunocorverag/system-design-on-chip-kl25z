/*
 * hal_gpio.h
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h> // For uint8_t

// Function prototypes for HAL (device-specific abstraction)
// Keypad_init still here as it calls MCAL_PORTC_init
void HAL_Keypad_init(void);
void HAL_LED_init(void); // Still needed to call MCAL_PORTB_init and MCAL_PORTD_init

#endif // HAL_H

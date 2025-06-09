/*
 * hal_gpio.c
 */

#include "hal_gpio.h"
#include "mcal_gpio.h" // Include MCAL to access low-level functions

/* Initializes PortC that is connected to the keypad.
  Pins as GPIO input pin with pullup enabled.
  This still remains in HAL as it's part of the keypad component's setup. */
void HAL_Keypad_init(void) {
    MCAL_PORTC_init();
}

/* Initialize all three LEDs on the FRDM board.
   This still remains in HAL as it's part of the LED component's setup. */
void HAL_LED_init(void) {
    MCAL_PORTB_init();
    MCAL_PORTD_init();
}

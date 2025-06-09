
//Program (11):  Functions declarations

/* This program scans a 4x4 matrix keypad and returns a unique number or each key pressed.
The number is displayed on the tri-color
 * LEDs using previous code
 * PortC 7-4 are connected to the columns and PortC 3-0 are connected to the rows. */

#include "hal_gpio.h"   // Include HAL for initializations
#include "mcal_gpio.h"  // Include MCAL for all direct hardware interactions

int main(void) {
    /* Init code */
    MCAL_TPM0_init(); // Initialize the timer for delays

    uint8_t key;

    HAL_Keypad_init(); // Initialize the keypad hardware (calls MCAL_PORTC_init)
    HAL_LED_init();    // Initialize the LED hardware (calls MCAL_PORTB_init and MCAL_PORTD_init)

    while (1) {
        key = MCAL_Keypad_getkey(); // Get the pressed key directly from MCAL
        MCAL_LED_set(key);          // Display the key on the LEDs directly via MCAL
    }
}

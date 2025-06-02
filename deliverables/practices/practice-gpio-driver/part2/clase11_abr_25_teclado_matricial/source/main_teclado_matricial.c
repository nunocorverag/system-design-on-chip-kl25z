
//Program (11):  Functions declarations

/* This program scans a 4x4 matrix keypad and returns a unique number or each key pressed.
The number is displayed on the tri-color
 * LEDs using previous code
 * PortC 7-4 are connected to the columns and PortC 3-0 are connected to the rows. */

#include "hal_gpio.h"

int main(void) {
    hal_gpio_init_timer(); // Inicializa el temporizador
    hal_gpio_keypad_init(); // Inicializa el keypad
    hal_gpio_led_init(); // Inicializa los LEDS

    while (1) {
        char key = hal_gpio_keypad_getkey();
        hal_gpio_led_set(key);
    }
}

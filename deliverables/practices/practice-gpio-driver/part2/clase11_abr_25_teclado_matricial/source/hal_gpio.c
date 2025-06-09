/*
 * hal_gpio.c
 */

#include "mcal_gpio.h"
#include "hal_gpio.h"

void hal_gpio_keypad_init(void) {
    gpio_init_port(0x0800); // Enable clock to Port C

    for (int i = 0; i < 8; ++i) {
        gpio_config_pin(&PORTC->PCR[i], 0x103); // GPIO + pull-up
    }

    PTC->PDDR = 0x0F; // PTC0–PTC3 output (rows), PTC4–PTC7 input (columns)
}

char hal_gpio_keypad_getkey(void) {
    return gpio_keypad_getkey(); // Llamada a la capa MCAL
}

void hal_gpio_led_init(void) {
    gpio_init_port(0x400);  // Port B
    gpio_init_port(0x1000); // Port D

    gpio_config_pin(&PORTB->PCR[18], 0x100); // LED rojo
    gpio_set_output(&PTB->PDDR, 0x40000);
    gpio_clear_output(&PTB->PSOR, 0x40000);

    gpio_config_pin(&PORTB->PCR[19], 0x100); // LED verde
    gpio_set_output(&PTB->PDDR, 0x80000);
    gpio_clear_output(&PTB->PSOR, 0x80000);

    gpio_config_pin(&PORTD->PCR[1], 0x100); // LED azul
    gpio_set_output(&PTD->PDDR, 0x02);
    gpio_clear_output(&PTD->PSOR, 0x02);
}

void hal_gpio_led_set(int value) {
    if (value & 1)
        gpio_clear_output(&PTB->PCOR, 0x40000); // Rojo ON
    else
        gpio_clear_output(&PTB->PSOR, 0x40000); // Rojo OFF

    if (value & 2)
        gpio_clear_output(&PTB->PCOR, 0x80000); // Verde ON
    else
        gpio_clear_output(&PTB->PSOR, 0x80000); // Verde OFF

    if (value & 4)
        gpio_clear_output(&PTD->PCOR, 0x02); // Azul ON
    else
        gpio_clear_output(&PTD->PSOR, 0x02); // Azul OFF
}

void hal_gpio_init_timer(void) {
    gpio_init_timer();
}

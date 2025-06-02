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
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};

    PTC->PDDR |= 0x0F;
    PTC->PCOR = 0x0F;
    gpio_delay_us(2);

    col = PTC->PDIR & 0xF0;
    PTC->PDDR = 0;

    if (col == 0xF0) return 0;

    for (row = 0; row < 4; row++) {
        PTC->PDDR = 0;
        PTC->PDDR |= row_select[row];
        PTC->PCOR = row_select[row];

        gpio_delay_us(2);
        col = PTC->PDIR & 0xF0;

        if (col != 0xF0) break;
    }

    PTC->PDDR = 0;

    if (row == 4) return 0;

    if (col == 0xE0) return row * 4 + 1;
    if (col == 0xD0) return row * 4 + 2;
    if (col == 0xB0) return row * 4 + 3;
    if (col == 0x70) return row * 4 + 4;

    return 0;
}

void hal_gpio_led_init(void) {
    gpio_init_port(0x400);  // Port B
    gpio_init_port(0x1000); // Port D

    gpio_config_pin(&PORTB->PCR[18], 0x100);
    gpio_set_output(&PTB->PDDR, 0x40000);
    gpio_clear_output(&PTB->PSOR, 0x40000);

    gpio_config_pin(&PORTB->PCR[19], 0x100);
    gpio_set_output(&PTB->PDDR, 0x80000);
    gpio_clear_output(&PTB->PSOR, 0x80000);

    gpio_config_pin(&PORTD->PCR[1], 0x100);
    gpio_set_output(&PTD->PDDR, 0x02);
    gpio_clear_output(&PTD->PSOR, 0x02);
}

void hal_gpio_led_set(int value) {
    if (value & 1)
        gpio_clear_output(&PTB->PCOR, 0x40000); // Red ON
    else
        gpio_clear_output(&PTB->PSOR, 0x40000); // Red OFF

    if (value & 2)
        gpio_clear_output(&PTB->PCOR, 0x80000); // Green ON
    else
        gpio_clear_output(&PTB->PSOR, 0x80000); // Green OFF

    if (value & 4)
        gpio_clear_output(&PTD->PCOR, 0x02); // Blue ON
    else
        gpio_clear_output(&PTD->PSOR, 0x02); // Blue OFF
}

void hal_gpio_init_timer(void) {
    gpio_init_timer();
}

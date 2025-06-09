/*
 * mcal_gpio.c
 */

#include "mcal_gpio.h"

void gpio_init_port(uint32_t port_mask) {
    SIM->SCGC5 |= port_mask;
}

void gpio_config_pin(volatile uint32_t *pcr, uint32_t value) {
    *pcr = value;
}

void gpio_set_output(volatile uint32_t *pddr, uint32_t mask) {
    *pddr |= mask;
}

void gpio_write_low(volatile uint32_t *port, uint32_t mask) {
    *port &= ~mask;
}

void gpio_write_high(volatile uint32_t *port, uint32_t mask) {
    *port |= mask;
}

uint32_t gpio_read(volatile uint32_t *pdir) {
    return *pdir;
}

void gpio_clear_output(volatile uint32_t *port, uint32_t mask) {
    *port = mask;
}

void gpio_init_timer(void) {
    SIM->SCGC6 |= 0x01000000;    // Enable clock to TPM0
    SIM->SOPT2 |= 0x01000000;    // Use 32.768kHz as clock
    TPM0->SC = 0;                // Disable timer while configuring
    TPM0->SC = 0x02;             // Prescaler /4
    TPM0->MOD = 0x2000;          // Max modulo value
    TPM0->SC |= 0x80;            // Clear TOF
    TPM0->SC |= 0x08;            // Enable free-running mode
}

void gpio_delay_us(int n) {
    while ((TPM0->SC & 0x80) == 0) {}
    TPM0->SC |= 0x80;
}

char gpio_keypad_getkey(void) {
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

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
    while ((TPM0->SC & 0x80) == 0) {} // Wait until TOF is set
    TPM0->SC |= 0x80;                 // Clear TOF
}

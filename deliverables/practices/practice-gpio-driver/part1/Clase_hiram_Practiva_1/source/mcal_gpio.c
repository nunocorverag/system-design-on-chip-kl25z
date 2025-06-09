/*
 * File: mcal_gpio.c
 * Description: MCAL implementation
 */
#include "mcal_gpio.h"

/**
 * Enable clock for given port
 */
void MCAL_GPIO_EnableClock(const MCAL_GPIO_Port_t *port_info) {
    SIM->SCGC5 |= port_info->clk_mask;
}

/**
 * Configure pin as digital output
 */
void MCAL_GPIO_ConfigOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
    port_info->port->PCR[pin] = PORT_PCR_MUX(1);
    port_info->gpio->PDDR |= (1UL << pin);
}

/**
 * Configure pin as digital input (with pull-up)
 */
void MCAL_GPIO_ConfigInput(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
    // Configure pin mux as GPIO and enable pull-up resistor
    port_info->port->PCR[pin] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    // Clear bit in PDDR to configure as input (0 = input, 1 = output)
    port_info->gpio->PDDR &= ~(1UL << pin);
}

/**
 * Write pin: value=1 sets pin, value=0 clears pin
 */
void MCAL_GPIO_WritePin(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint8_t value) {
	if (value) {
		port_info->gpio->PSOR = (1UL << pin);
	}
	else {
		port_info->gpio->PCOR = (1UL << pin);
	}
}

/**
 * Read pin: returns 1 if high, 0 if low
 */
uint8_t MCAL_GPIO_ReadPin(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
    // Read from PDIR (Pin Data Input Register) and check the specific pin
    return (port_info->gpio->PDIR & (1UL << pin)) ? 1 : 0;
}


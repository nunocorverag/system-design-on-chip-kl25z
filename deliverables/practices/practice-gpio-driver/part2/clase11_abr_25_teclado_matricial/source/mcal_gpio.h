/*
 * mcal_gpio.h
 */

#ifndef MCAL_GPIO_H
#define MCAL_GPIO_H

#include <MKL25Z4.h>

// Funciones de bajo nivel (MCAL)
void gpio_init_port(uint32_t port_mask);

void gpio_config_pin(volatile uint32_t *pcr, uint32_t value);

void gpio_set_output(volatile uint32_t *pddr, uint32_t mask);

void gpio_write_low(volatile uint32_t *port, uint32_t mask);

void gpio_write_high(volatile uint32_t *port, uint32_t mask);

uint32_t gpio_read(volatile uint32_t *pdir);

void gpio_clear_output(volatile uint32_t *port, uint32_t mask);

void gpio_delay_us(int n);

void gpio_init_timer(void);

#endif

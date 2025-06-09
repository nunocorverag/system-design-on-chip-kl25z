/*
 * mcal_gpio.h
 */
#ifndef MCAL_GPIO_H
#define MCAL_GPIO_H

#include <MKL25Z4.h>
#include <stdint.h>

// --- MCAL Port Initialization Functions ---
void MCAL_PORTA_init(void);
void MCAL_PORTB_init(void);
void MCAL_PORTC_init(void);
void MCAL_PORTD_init(void);
void MCAL_PORTE_init(void);

// --- MCAL GPIO Operations (Low-level Read/Write/Direction) ---
void MCAL_GPIO_SetPinDirection(PORT_Type *port_base, GPIO_Type *pt_base, uint32_t pin, uint8_t is_output, uint32_t pcr_config);
uint8_t MCAL_GPIO_ReadPin(GPIO_Type *pt_base, uint32_t pin);
void MCAL_GPIO_WritePin(GPIO_Type *pt_base, uint32_t pin, uint8_t value);
void MCAL_GPIO_TogglePin(GPIO_Type *pt_base, uint32_t pin);

// --- MCAL Timer (TPM0) for Delays ---
// The init function will now also take configuration values for TPM0
void MCAL_TPM0_init(uint32_t prescaler_value, uint32_t modulo_value);
void MCAL_TPM0_delayUs(int n);
void MCAL_TPM0_delayMs(int n);

// --- Keypad and LED Logic (now receive configuration as arguments) ---
uint8_t MCAL_Keypad_getkey(uint8_t rows_mask, uint8_t cols_offset, uint8_t cols_nibble_mask);
void MCAL_LED_set(int value, uint32_t red_pin, uint32_t green_pin, uint32_t blue_pin);

#endif // MCAL_H

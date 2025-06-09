/*
 * mcal_gpio.h
 */
#ifndef MCAL_GPIO_H
#define MCAL_GPIO_H

#include <MKL25Z4.h>
#include <stdint.h> // For uint8_t

// Function prototypes for MCAL (low-level hardware access)
void MCAL_PORTC_init(void);
void MCAL_TPM0_init(void);
void MCAL_TPM0_delayUs(int n);
void MCAL_TPM0_delayMs(int n);

void MCAL_PORTB_init(void);
void MCAL_PORTD_init(void);

// Keypad scanning logic now in MCAL
uint8_t MCAL_Keypad_getkey(void);

// New: LED control logic now in MCAL
void MCAL_LED_set(int value);

// Register access macros for Port C
#define MCAL_PORTC_SET_PDDR(value) (PTC->PDDR = (value))
#define MCAL_PORTC_SET_PCOR(value) (PTC->PCOR = (value))
#define MCAL_PORTC_GET_PDIR() (PTC->PDIR)
#define MCAL_PORTC_SET_PCR(pin, value) (PORTC->PCR[(pin)] = (value))

// Register access macros for Port B
#define MCAL_PORTB_SET_PDDR(value) (PTB->PDDR = (value))
#define MCAL_PORTB_SET_PSOR(value) (PTB->PSOR = (value))
#define MCAL_PORTB_SET_PCOR(value) (PTB->PCOR = (value))
#define MCAL_PORTB_SET_PCR(pin, value) (PORTB->PCR[(pin)] = (value))

// Register access macros for Port D
#define MCAL_PORTD_SET_PDDR(value) (PTD->PDDR = (value))
#define MCAL_PORTD_SET_PSOR(value) (PTD->PSOR = (value))
#define MCAL_PORTD_SET_PCOR(value) (PTD->PCOR = (value))
#define MCAL_PORTD_SET_PCR(pin, value) (PORTD->PCR[(pin)] = (value))


#endif // MCAL_H

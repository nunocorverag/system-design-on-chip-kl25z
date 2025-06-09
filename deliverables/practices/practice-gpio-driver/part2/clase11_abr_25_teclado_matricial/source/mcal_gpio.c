/*
 * mcal_gpio.c
 */
#include "mcal_gpio.h"

// --- MCAL Port Initialization Implementations ---
void MCAL_PORTA_init(void) { SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; }
void MCAL_PORTB_init(void) { SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; }
void MCAL_PORTC_init(void) { SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; }
void MCAL_PORTD_init(void) { SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; }
void MCAL_PORTE_init(void) { SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; }

// --- MCAL GPIO Operations Implementations ---
void MCAL_GPIO_SetPinDirection(PORT_Type *port_base, GPIO_Type *pt_base, uint32_t pin, uint8_t is_output, uint32_t pcr_config) {
    port_base->PCR[pin] = pcr_config;

    if (is_output) {
        pt_base->PDDR |= (1u << pin); // Set as output
    } else {
        pt_base->PDDR &= ~(1u << pin); // Set as input
    }
}

uint8_t MCAL_GPIO_ReadPin(GPIO_Type *pt_base, uint32_t pin) {
    return (pt_base->PDIR >> pin) & 1u;
}

void MCAL_GPIO_WritePin(GPIO_Type *pt_base, uint32_t pin, uint8_t value) {
    if (value) {
        pt_base->PSOR = (1u << pin); // Set pin high (turn off for active-low LEDs)
    } else {
        pt_base->PCOR = (1u << pin); // Set pin low (turn on for active-low LEDs)
    }
}

void MCAL_GPIO_TogglePin(GPIO_Type *pt_base, uint32_t pin) {
    pt_base->PTOR = (1u << pin);
}

// --- MCAL Timer (TPM0) for Delays ---
void MCAL_TPM0_init(uint32_t prescaler_value, uint32_t modulo_value) {
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SOPT2 = (SIM->SOPT2 & ~SIM_SOPT2_TPMSRC_MASK) | SIM_SOPT2_TPMSRC(1); // Select OSCERCLK
    TPM0->SC = 0;
    TPM0->SC = prescaler_value;
    TPM0->MOD = modulo_value;
    TPM0->SC |= TPM_SC_TOF_MASK;
    TPM0->SC |= TPM_SC_CMOD(1);
}

void MCAL_TPM0_delayUs(int n) {
    while (n--) {
        while (!((TPM0->SC & TPM_SC_TOF_MASK) == TPM_SC_TOF_MASK)) {
            // wait until the TOF is set
        }
        TPM0->SC |= TPM_SC_TOF_MASK; /* clear TOF */
    }
}

void MCAL_TPM0_delayMs(int n) {
    for (int i = 0; i < n; i++) {
        MCAL_TPM0_delayUs(1000);
    }
}

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

// --- Keypad and LED Logic (now receive configuration as arguments) ---
uint8_t MCAL_Keypad_getkey(uint8_t rows_mask, uint8_t cols_offset, uint8_t cols_nibble_mask) {
    int row, col;
    const uint8_t row_select[] = {0x01, 0x02, 0x04, 0x08}; // This is part of the 4x4 matrix logic itself

    // Temporarily set rows as output
    PTC->PDDR = rows_mask;
    PTC->PCOR = rows_mask; // Drive all rows low
    MCAL_TPM0_delayUs(2);
    col = (PTC->PDIR >> cols_offset) & cols_nibble_mask; // Read column bits only
    PTC->PDDR = 0x00; // Set all as input (high-Z)

    if (col == cols_nibble_mask) // If all columns are high (no key pressed)
        return 0;

    for (row = 0; row < 4; row++) {
        PTC->PDDR = 0x00; // Disable all rows (inputs)
        PTC->PDDR |= row_select[row]; // Enable one row as output
        PTC->PCOR = row_select[row];  // Drive active row low

        MCAL_TPM0_delayUs(2);
        col = (PTC->PDIR >> cols_offset) & cols_nibble_mask; // Read column bits only

        if (col != cols_nibble_mask) break;
    }

    PTC->PDDR = 0x00; // Disable all rows

    if (row == 4) return 0;

    // Decode which column was pressed
    if (col == 0x0E) return row * 4 + 1; // Col 0 (0x0E = 1110 binary)
    if (col == 0x0D) return row * 4 + 2; // Col 1 (0x0D = 1101 binary)
    if (col == 0x0B) return row * 4 + 3; // Col 2 (0x0B = 1011 binary)
    if (col == 0x07) return row * 4 + 4; // Col 3 (0x07 = 0111 binary)
    return 0;
}

void MCAL_LED_set(int value, uint32_t red_pin, uint32_t green_pin, uint32_t blue_pin) {
    // Red LED
    if (value & 1)
        MCAL_GPIO_WritePin(PTB, red_pin, 0); // Active low: 0 to turn ON
    else
        MCAL_GPIO_WritePin(PTB, red_pin, 1); // 1 to turn OFF

    // Green LED
    if (value & 2)
        MCAL_GPIO_WritePin(PTB, green_pin, 0); // Active low: 0 to turn ON
    else
        MCAL_GPIO_WritePin(PTB, green_pin, 1); // 1 to turn OFF

    // Blue LED
    if (value & 4)
        MCAL_GPIO_WritePin(PTD, blue_pin, 0); // Active low: 0 to turn ON
    else
        MCAL_GPIO_WritePin(PTD, blue_pin, 1); // 1 to turn OFF
}

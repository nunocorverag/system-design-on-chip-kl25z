/*
 * mcal_gpio.c
 */

#include "mcal_gpio.h"

// Delays are handled by TPM0
void MCAL_TPM0_init(void) {
    SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000; /* use 32.76khz as clock */
    TPM0->SC = 0;             /* disable timer while configuring */
    TPM0->SC = 0x02;          /* prescaler /4 */
    TPM0->MOD = 0x2000;       /* max modulo value 8192*/
    TPM0->SC |= 0x80;         /* clear TOF */
    TPM0->SC |= 0x08;         /* enable timer free-running mode */
}

void MCAL_TPM0_delayUs(int n) {
    while (n--) {
        while ((TPM0->SC & 0x80) == 0) {
            // wait until the TOF is set
        }
        TPM0->SC |= 0x80; /* clear TOF */
    }
}

void MCAL_TPM0_delayMs(int n) {
    for (int i = 0; i < n; i++) {
        MCAL_TPM0_delayUs(1000); // Rough approximation for 1ms
    }
}

// Port C initialization for Keypad
void MCAL_PORTC_init(void) {
    SIM->SCGC5 |= 0x0800; /* enable clock to Port C */
    // Set PCR for all relevant Port C pins
    MCAL_PORTC_SET_PCR(0, 0x103); // PTD0, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(1, 0x103); // PTD1, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(2, 0x103); // PTD2, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(3, 0x103); // PTD3, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(4, 0x103); // PTD4, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(5, 0x103); // PTD5, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(6, 0x103); // PTD6, GPIO, enable pullup
    MCAL_PORTC_SET_PCR(7, 0x103); // PTD7, GPIO, enable pullup
    // Initial state: PTD7-0 as input pins
    MCAL_PORTC_SET_PDDR(0x00); // All pins as input initially for scanning
}

// Port B initialization for Red/Green LEDs
void MCAL_PORTB_init(void) {
    SIM->SCGC5 |= 0x0400; /* enable clock to Port B */
    MCAL_PORTB_SET_PCR(18, 0x100); /* make PTB18 pin as GPIO */
    MCAL_PORTB_SET_PDDR(PTB->PDDR | 0x40000); /* make PTB18 as output pin */
    MCAL_PORTB_SET_PSOR(0x40000); /* turn off red LED */
    MCAL_PORTB_SET_PCR(19, 0x100); /* make PTB19 pin as GPIO */
    MCAL_PORTB_SET_PDDR(PTB->PDDR | 0x80000); /* make PTB19 as output pin */
    MCAL_PORTB_SET_PSOR(0x80000); /* turn off green LED */
}

// Port D initialization for Blue LED
void MCAL_PORTD_init(void) {
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    MCAL_PORTD_SET_PCR(1, 0x100); /* make PTD1 pin as GPIO */
    MCAL_PORTD_SET_PDDR(PTD->PDDR | 0x02); /* make PTD1 as output pin */
    MCAL_PORTD_SET_PSOR(0x02); /* turn off blue LED */
}

/*
 * Keypad scanning and decoding logic, now in MCAL.
 * If a key is pressed, it returns a key code. Otherwise, a zero is returned.
 */
uint8_t MCAL_Keypad_getkey(void) {
    int row, col;
    const uint8_t row_select[] = {0x01, 0x02, 0x04, 0x08};

    /* Check to see if any key is pressed */
    MCAL_PORTC_SET_PDDR(0x0F);    /* enable all rows as output */
    MCAL_PORTC_SET_PCOR(0x0F);    /* drive all rows low */
    MCAL_TPM0_delayUs(2);         /* wait for signal return */
    col = MCAL_PORTC_GET_PDIR() & 0xF0; /* read all columns */
    MCAL_PORTC_SET_PDDR(0x00);    /* disable all rows (set as input for high-Z) */

    if (col == 0xF0)
        return 0; /* no key pressed */

    /* If a key is pressed, we need to find out which key. */
    for (row = 0; row < 4; row++) {
        MCAL_PORTC_SET_PDDR(0x00);          /* disable all rows (set as input) */
        MCAL_PORTC_SET_PDDR(row_select[row]); /* enable one row as output */
        MCAL_PORTC_SET_PCOR(row_select[row]); /* drive active row low */

        MCAL_TPM0_delayUs(2);                 /* wait for signal to settle */
        col = MCAL_PORTC_GET_PDIR() & 0xF0;     /* read all columns */

        if (col != 0xF0) break; /* if one of the inputs is low, some key is pressed. */
    }

    MCAL_PORTC_SET_PDDR(0x00); /* disable all rows */

    if (row == 4)
        return 0; /* If we get here, no key is pressed */

    /* Gets here when one of the rows has a key pressed */
    /* Check which column it is */
    if (col == 0xE0) return row * 4 + 1; /* key in column 0 */
    if (col == 0xD0) return row * 4 + 2; /* key in column 1 */
    if (col == 0xB0) return row * 4 + 3; /* key in column 2 */
    if (col == 0x70) return row * 4 + 4; /* key in column 3 */
    return 0; /* just to be safe */
}

/*
 * LED control logic, now in MCAL.
 * Turn on or off the LEDs wrt to bit 2-0 of the value.
 */
void MCAL_LED_set(int value) {
    // Use bit 0 of value to control red LED
    if (value & 1)
        MCAL_PORTB_SET_PCOR(0x40000); // turn on red LED
    else
        MCAL_PORTB_SET_PSOR(0x40000); // turn off red LED

    // Use bit 1 of value to control green LED
    if (value & 2)
        MCAL_PORTB_SET_PCOR(0x80000); // turn on green LED
    else
        MCAL_PORTB_SET_PSOR(0x80000); // turn off green LED

    // Use bit 2 of value to control blue LED
    if (value & 4)
        MCAL_PORTD_SET_PCOR(0x02); // turn on blue LED
    else
        MCAL_PORTD_SET_PSOR(0x02); // turn off blue LED
}

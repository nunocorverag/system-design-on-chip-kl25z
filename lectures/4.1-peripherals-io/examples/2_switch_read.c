/*
 * Switch Input Reading Example for FRDM-KL25Z
 *
 * This program reads the state of a switch connected to pin PTA1,
 * turning the green LED (PTB19) on when the switch is pressed and off otherwise.
 * It demonstrates input pin configuration with pull-up resistor,
 * reading pin state, and controlling an output pin accordingly.
 */

#include <MKL25Z4.h>

void delayMs(int n) {
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 7000; j++) {
            __NOP();
        }
}

int main(void) {
    // Enable clock to Port B and Port A
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTA_MASK;

    // Configure PTB19 as GPIO output (LED)
    PORTB->PCR[19] = PORT_PCR_MUX(1);
    PTB->PDDR |= (1 << 19);

    // Configure PTA1 as GPIO input with internal pull-up resistor enabled
    PORTA->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PTA->PDDR &= ~(1 << 1); // Set PTA1 as input

    while(1) {
        // Read PTA1 input pin state
        if (PTA->PDIR & (1 << 1)) {
            // If switch is NOT pressed (input high), turn OFF LED
            PTB->PSOR = (1 << 19);
        } else {
            // If switch is pressed (input low), turn ON LED
            PTB->PCOR = (1 << 19);
        }
        delayMs(100);
    }
}
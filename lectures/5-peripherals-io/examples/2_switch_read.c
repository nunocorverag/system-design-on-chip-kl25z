/*
 * Switch Input Reading Example for FRDM-KL25Z
 *
 * This program reads the state of a switch connected to pin PTA1,
 * turning the green LED (PTB19) on when the switch is pressed and off otherwise.
 * It demonstrates input pin configuration with pull-up resistor,
 * reading pin state, and controlling an output pin accordingly.
 */

#include <MKL25Z4.h> // Includes register definitions for KL25Z MCU

// Simple delay function (~1 ms per iteration)
void delayMs(int n) {
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 7000; j++) {
            __NOP(); // No Operation - prevents compiler optimization from removing the loop
        }
}

int main(void) {
    // Step 1: Enable clock to Port B and Port A using SCGC5 register
    // - Bit 9 (PORTA) and bit 10 (PORTB) must be set to 1 to enable
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTA_MASK;

    // Step 2: Configure PTB19 pin multiplexer for GPIO function (MUX = 1)
    PORTB->PCR[19] = PORT_PCR_MUX(1);

    // Step 3: Set PTB19 as output in Port B Data Direction Register
    PTB->PDDR |= (1 << 19);

    // Step 4: Configure PTA1 as GPIO input with internal pull-up resistor
    // - MUX = 1 to select GPIO
    // - PE (bit 1x) = Pull enable
    // - PS (bit 0x) = Pull select → high = pull-up
    PORTA->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    // Step 5: Set PTA1 as input by clearing the corresponding bit in PDDR
    PTA->PDDR &= ~(1 << 1);

    // Main loop
    while(1) {
        // Step 6: Read PTA1 input pin state using PDIR
        // - If bit 1 is high, switch is NOT pressed
        // - If bit 1 is low, switch is pressed (connected to ground)

        if (PTA->PDIR & (1 << 1)) {
            // Switch NOT pressed → turn OFF LED
            // - Active-low LED: setting bit to 1 turns it OFF
            PTB->PSOR = (1 << 19); // PSOR: Port Set Output Register
        } else {
            // Switch pressed → turn ON LED
            // - Writing 1 to PCOR clears the bit → turns LED ON
            PTB->PCOR = (1 << 19); // PCOR: Port Clear Output Register
        }

        delayMs(100); // Short delay to stabilize LED behavior
    }
}

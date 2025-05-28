/*
 * Simple LED Toggle Example for FRDM-KL25Z
 *
 * This program toggles the green LED connected to pin PTB19 every 500 ms.
 * It demonstrates enabling the clock to a port, configuring a pin as GPIO output,
 * and manipulating output registers to turn the LED on and off.
 */

#include <MKL25Z4.h>  // KL25Z MCU header file with register definitions

// Simple delay function (~1 ms per iteration)
void delayMs(int n) {
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 7000; j++) {
            __NOP(); // No Operation - prevents compiler optimization removing this loop
        }
}

int main(void) {
    // Step 1: Enable clock to Port B (bit 10 in SCGC5)
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Step 2: Configure PTB19 pin multiplexer to GPIO function (MUX = 1)
    PORTB->PCR[19] = PORT_PCR_MUX(1);

    // Step 3: Set PTB19 as output pin in Port B Data Direction Register
    PTB->PDDR |= (1 << 19);

    while (1) {
        // Turn ON LED by clearing output bit (active low LED)
        PTB->PCOR = (1 << 19);

        delayMs(500); // Delay ~500 ms

        // Turn OFF LED by setting output bit
        PTB->PSOR = (1 << 19);

        delayMs(500); // Delay ~500 ms
    }
}
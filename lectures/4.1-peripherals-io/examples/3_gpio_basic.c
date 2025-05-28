/*
 * Basic GPIO Control Example for FRDM-KL25Z
 *
 * This program toggles all three LEDs (red, green, and blue) connected
 * to pins PTB18, PTB19, and PTD1 simultaneously every 500 ms.
 * Demonstrates configuration of multiple output pins and atomic
 * set/clear register operations.
 */

#include <MKL25Z4.h>  // Include microcontroller register definitions

// Delay function to create a visible blink (~1 ms per iteration)
void delayMs(int n);

int main(void) {
    // Step 1: Enable clocks for Ports B and D by setting corresponding bits in SCGC5
    // This allows us to use pins on these ports
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

    // Step 2: Configure pins PTB18 and PTB19 as GPIO outputs for red and green LEDs
    // Set MUX field = 1 in Pin Control Registers to select GPIO function
    PORTB->PCR[18] = PORT_PCR_MUX(1);
    PORTB->PCR[19] = PORT_PCR_MUX(1);

    // Set PTB18 and PTB19 as outputs by setting bits 18 and 19 in Port Data Direction Register (PDDR)
    PTB->PDDR |= (1 << 18) | (1 << 19);

    // Step 3: Configure pin PTD1 as GPIO output for blue LED
    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1 << 1);

    while (1) {
        // Step 4: Turn ON red and green LEDs
        // Writing 1 to PCOR clears the output bit (active low LEDs), turning them ON
        PTB->PCOR = (1 << 18) | (1 << 19);

        // Turn ON blue LED in similar way
        PTD->PCOR = (1 << 1);

        delayMs(500);  // Wait about 500 ms to make blinking visible

        // Step 5: Turn OFF red and green LEDs
        // Writing 1 to PSOR sets the output bit (making output high), turning LEDs OFF
        PTB->PSOR = (1 << 18) | (1 << 19);

        // Turn OFF blue LED
        PTD->PSOR = (1 << 1);

        delayMs(500);  // Wait again for blinking effect
    }
}

void delayMs(int n) {
    int i, j;
    // Nested loops to produce delay (~1 ms per iteration)
    for (i = 0; i < n; i++)
        for (j = 0; j < 7000; j++)
            __NOP(); // No Operation instruction to prevent optimization removing loop
}

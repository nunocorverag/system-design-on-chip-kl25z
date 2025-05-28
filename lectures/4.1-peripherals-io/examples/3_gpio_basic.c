/*
 * Basic GPIO Control Example for FRDM-KL25Z
 *
 * This program toggles all three LEDs (red, green, and blue) connected
 * to pins PTB18, PTB19, and PTD1 simultaneously every 500 ms.
 * Demonstrates configuration of multiple output pins and atomic
 * set/clear register operations.
 */

#include <MKL25Z4.h>

void delayMs(int n);

int main(void) {
    // Enable clocks to Ports B and D
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

    // Configure PTB18 and PTB19 as GPIO outputs (red and green LEDs)
    PORTB->PCR[18] = PORT_PCR_MUX(1);
    PORTB->PCR[19] = PORT_PCR_MUX(1);
    PTB->PDDR |= (1 << 18) | (1 << 19);

    // Configure PTD1 as GPIO output (blue LED)
    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1 << 1);

    while (1) {
        // Turn ON red and green LEDs (active low)
        PTB->PCOR = (1 << 18) | (1 << 19);

        // Turn ON blue LED
        PTD->PCOR = (1 << 1);

        delayMs(500);

        // Turn OFF red and green LEDs
        PTB->PSOR = (1 << 18) | (1 << 19);

        // Turn OFF blue LED
        PTD->PSOR = (1 << 1);

        delayMs(500);
    }
}

void delayMs(int n) {
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 7000; j++)
            __NOP();
}
/*
 * Simple LED Toggle Example for FRDM-KL25Z
 *
 * This program toggles the green LED connected to pin PTB19 every 500 ms.
 * It demonstrates enabling the clock to a port, configuring a pin as GPIO output,
 * and manipulating output registers to turn the LED on and off.
 */

#include <MKL25Z4.h>  // KL25Z MCU header file with register definitions

// Delay function (~1 ms per iteration), used for visualizing the LED toggle
void delayMs(int n) {
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 7000; j++) {
            __NOP(); // No Operation - prevents compiler optimization removing this loop
        }
}

int main(void) {
    // Step 1: Enable the clock for Port B by setting bit 10 in the SCGC5 register
    // This is necessary to configure or use any pin from Port B
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Step 2: Configure PTB19 to act as a GPIO pin by setting MUX = 1
    // Each PORTx_PCRn register has a MUX field to select the pin's function
    PORTB->PCR[19] = PORT_PCR_MUX(1);

    // Step 3: Configure PTB19 as output by setting the corresponding bit in PDDR
    // 1 << 19 sets the 19th bit to 1, configuring PTB19 as output
    PTB->PDDR |= (1 << 19);

    while (1) {
        // Turn ON the LED by writing 1 to bit 19 in the PCOR register
        // This clears the bit in PDOR, making the output LOW (active low LED turns on)
        PTB->PCOR = (1 << 19);

        delayMs(500);  // Wait 500 ms

        // Turn OFF the LED by writing 1 to bit 19 in the PSOR register
        // This sets the bit in PDOR, making the output HIGH (LED turns off)
        PTB->PSOR = (1 << 19);

        delayMs(500);  // Wait 500 ms
    }
}

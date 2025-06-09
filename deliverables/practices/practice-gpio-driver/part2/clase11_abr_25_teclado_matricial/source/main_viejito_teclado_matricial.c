/*
 * main_viejito_teclado_matricial.c
 *
 *  Created on: 9 jun. 2025
 *      Author: alvar
 */


//Program (11):  Functions declarations

/* This program scans a 4x4 matrix keypad and returns a unique number or each key pressed.
The number is displayed on the tri-color
 * LEDs using previous code
 * PortC 7-4 are connected to the columns and PortC 3-0 are connected to the rows. */

#include <MKL25Z4.h>

void delayMs(int n);
void delayUs(int n);
void keypad_init(void);
char keypad_getkey(void);
void LED_init(void);
void LED_set(int value);

int main(void)
{
	/* Init code */
	SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
	SIM->SOPT2 |= 0x01000000; /* use 32.76khz as clock */
	TPM0->SC = 0; /* disable timer while configuring */
	TPM0->SC = 0x02; /* prescaler /4 */
	TPM0->MOD = 0x2000; /* max modulo value 8192*/
	TPM0->SC |= 0x80; /* clear TOF */
	TPM0->SC |= 0x08; /* enable timer free-running mode */

	unsigned char key;

	keypad_init();
	LED_init();

	while(1)
	{

		key = keypad_getkey();
		LED_set(key); /* set LEDs according to the key code */

	}
}

/* Initializes PortC that is connected to the keypad.
  Pins as GPIO input pin with pullup enabled.*/

void keypad_init(void)
{
	SIM->SCGC5 |= 0x0800;  /* enable clock to Port C */
	PORTC->PCR[0] = 0x103; /* PTD0, GPIO, enable pullup*/
	PORTC->PCR[1] = 0x103; /* PTD1, GPIO, enable pullup*/
	PORTC->PCR[2] = 0x103; /* PTD2, GPIO, enable pullup*/
	PORTC->PCR[3] = 0x103; /* PTD3, GPIO, enable pullup*/
	PORTC->PCR[4] = 0x103; /* PTD4, GPIO, enable pullup*/
	PORTC->PCR[5] = 0x103; /* PTD5, GPIO, enable pullup*/
	PORTC->PCR[6] = 0x103; /* PTD6, GPIO, enable pullup*/
	PORTC->PCR[7] = 0x103; /* PTD7, GPIO, enable pullup*/
	PTC->PDDR = 0x0F; /* make PTD7-0 as input pins */
}

/* keypad_getkey()
 * If a key is pressed, it returns a key code. Otherwise, a zero is returned.
The upper nibble of Port C is used as input. Pull-ups are enabled when the keys are not pressed
 * The lower nibble of Port C is used as output that drives the keypad rows.
 * First all rows are driven low and the input pins are read. If no key is pressed, it will read as all ones. Otherwise, some key is pressed.
 * If any key is pressed, the program drives one row low at a time and leave the rest of the rows inactive (float) then read the input pins.
 * Knowing which row is active and which column is active, the program can decide which key is pressed. */

char keypad_getkey(void) {

	int row, col;
	const char row_select[] = {0x01, 0x02, 0x04, 0x08};
	/* one row is active */
	/* check to see any key pressed */

	PTC->PDDR |= 0x0F; /* enable all rows */
	PTC->PCOR = 0x0F;
	delayUs(2); /* wait for signal return */
	col = PTC-> PDIR & 0xF0; /* read all columns */
	PTC->PDDR = 0; /* disable all rows */
	if (col == 0xF0)
		return 0; /* no key pressed */

	/* If a key is pressed, we need find out which key.*/
	for (row = 0; row < 4; row++)
	{ PTC->PDDR = 0; /* disable all rows */

	PTC->PDDR |= row_select[row]; /* enable one row */
	PTC->PCOR = row_select[row]; /* drive active row low*/

	delayUs(2); /* wait for signal to settle */
	col = PTC->PDIR & 0xF0; /* read all columns */

	if (col != 0xF0) break;
	/* if one of the input is low, some key is pressed. */
	}

	PTC->PDDR = 0; /* disable all rows */

	if (row == 4)
		return 0; /* if we get here, no key is pressed */

	/* gets here when one of the rows has key pressed*/
	/check which column it is/

	if (col == 0xE0) return row*4+ 1; /* key in column 0 */
	if (col == 0xD0) return row*4+ 2; /* key in column 1 */
	if (col == 0xB0) return row*4+ 3; /* key in column 2 */
	if (col == 0x70) return row*4+ 4; /* key in column 3 */
	return 0; /* just to be safe */
}

/* initialize all three LEDs on the FRDM board */
void LED_init(void)
{
	SIM->SCGC5 |= 0x400; /* enable clock to Port B */
	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
	PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
	PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
	PTB->PSOR |= 0x40000; /* turn off red LED */
	PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
	PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
	PTB->PSOR |= 0x80000; /* turn off green LED */
	PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
	PTD->PDDR |= 0x02; /* make PTD1 as output pin */
	PTD->PSOR |= 0x02; /* turn off blue LED */
}

/* turn on or off the LEDs wrt to bit 2-0 of the value */
void LED_set(int value)
{
	/* use bit 0 of value to control red LED */
	if (value & 1)
		PTB->PCOR = 0x40000; else /* turn on red LED */
			PTB->PSOR = 0x40000; /* turn off red LED */
	/* use bit 1 of value to control green LED */
	if (value & 2)
		PTB->PCOR = 0x80000; else /* turn on green LED */
			PTB->PSOR = 0x80000; /* turn off green LED */
	/* use bit 2 of value to control blue LED */
	if (value & 4)
		PTD->PCOR = 0x02; else /* turn on blue LED */
			PTD->PSOR = 0x02; /* turn off blue LED */
}

/* Delay function */
void delayUs(int n) {
	while((TPM0->SC & 0x80) == 0) {
	/* wait until the TOF is set */
	TPM0->SC |= 0x80; /* clear TOF */
}

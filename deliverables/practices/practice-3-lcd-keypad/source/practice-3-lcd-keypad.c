#include <MKL25Z4.h>

#define RS 0x04  // PTA2
#define RW 0x10  // PTA4
#define EN 0x20  // PTA5

/* ========= DELAY ========= */
void delayMs(int n) {
	while ((TPM0->SC & 0x80) == 0) {}
	TPM0->SC |= 0x80; // clear TOF
}

/* ========= LCD ========= */
void LCD_command(unsigned char command) {
	PTA->PCOR = RS | RW;       // RS = 0, RW = 0
	PTD->PDOR = command;       // enviar comando
	PTA->PSOR = EN;            // EN = 1
	delayMs(1);
	PTA->PCOR = EN;            // EN = 0
	if (command < 4) delayMs(2); else delayMs(1);
}

void LCD_data(unsigned char data) {
	PTA->PSOR = RS;            // RS = 1
	PTA->PCOR = RW;            // RW = 0
	PTD->PDOR = data;
	PTA->PSOR = EN;            // EN = 1
	delayMs(1);
	PTA->PCOR = EN;            // EN = 0
	delayMs(1);
}

void LCD_init(void) {
	SIM->SCGC5 |= 0x1000; // Puerto D
	for (int i = 0; i < 8; i++)
		PORTD->PCR[i] = 0x100;
	PTD->PDDR = 0xFF;

	SIM->SCGC5 |= 0x0200; // Puerto A
	PORTA->PCR[2] = 0x100;
	PORTA->PCR[4] = 0x100;
	PORTA->PCR[5] = 0x100;
	PTA->PDDR |= (RS | RW | EN);

	delayMs(30);
	LCD_command(0x30); delayMs(10);
	LCD_command(0x30); delayMs(1);
	LCD_command(0x30);

	LCD_command(0x38); // 8 bits, 2 líneas
	LCD_command(0x06); // Modo entrada
	LCD_command(0x01); // Limpiar pantalla
	LCD_command(0x0F); // Encender display y cursor
}

/* ========= TECLADO ========= */
void keypad_init(void) {
	SIM->SCGC5 |= 0x0800;  // Puerto C
	for (int i = 0; i < 8; i++)
		PORTC->PCR[i] = 0x103; // GPIO + pull-up
	PTC->PDDR = 0x0F; // PC0-PC3 como salidas (filas)
}

char keypad_getkey(void) {
	const char row_select[] = {0x01, 0x02, 0x04, 0x08};
	int row, col;
	PTC->PDDR |= 0x0F;
	PTC->PCOR = 0x0F;
	delayMs(1);
	col = PTC->PDIR & 0xF0;
	PTC->PDDR = 0;
	if (col == 0xF0) return 0;

	for (row = 0; row < 4; row++) {
		PTC->PDDR = 0;
		PTC->PDDR |= row_select[row];
		PTC->PCOR = row_select[row];
		delayMs(1);
		col = PTC->PDIR & 0xF0;
		if (col != 0xF0) break;
	}

	PTC->PDDR = 0;
	if (row == 4) return 0;

	if (col == 0xE0) return row * 4 + 1;
	if (col == 0xD0) return row * 4 + 2;
	if (col == 0xB0) return row * 4 + 3;
	if (col == 0x70) return row * 4 + 4;
	return 0;
}

/* ========= MAIN ========= */
int main(void) {
	char key;
	char* bebidas[17] = {
		"",           // 0 (sin uso)
		"MOJITO",     // 1
		"CUBA",       // 2
		"PERLA",      // 3
		"MOSCOW MULE",    // 4
		"AZULITO",     // 5
		"PALOMA",      // 6
		"PINA COLADA",        // 7
		"MARGARITA",        // 8
		"APEROL SPIRITZ",     // 9
		"MARTINI",      // 10
		"MAI TAI",    // 11
		"MANHATTAN",     // 12
		"SANGRIA",// 13
		"LONG ISLAND TEA",     // 14
		"CERVEZA",    // 15
		"CHAMPAGNE"   // 16
	};

	// Inicializar TPM0 para delayMs
	SIM->SCGC6 |= 0x01000000;
	SIM->SOPT2 |= 0x01000000;
	TPM0->SC = 0;
	TPM0->SC = 0x02; // prescaler /4
	TPM0->MOD = 0x2000;
	TPM0->SC |= 0x80;
	TPM0->SC |= 0x08;

	keypad_init();
	LCD_init();

	LCD_command(1); // Limpiar pantalla
	LCD_command(0x80); // Cursor al inicio

	while (1) {
		key = keypad_getkey();
		if (key != 0 && key <= 16) {
			LCD_command(1); // Borrar pantalla
			LCD_command(0x80);
			char* bebida = bebidas[key];
			for (int i = 0; bebida[i] != '\0'; i++) {
				LCD_data(bebida[i]);
			}
			delayMs(500);
		}
	}
}

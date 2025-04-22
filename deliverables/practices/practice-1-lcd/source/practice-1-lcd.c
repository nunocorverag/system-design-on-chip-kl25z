#include <MKL25Z4.H>

#define RS 0x04  // PTA2
#define RW 0x10  // PTA4 (no se usa en este código, RW debe ir a GND)
#define EN 0x20  // PTA5

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_print(char *str);

int main(void) {
    LCD_init();

    while (1) {
        LCD_command(1);         // Clear display
        delayMs(500);
        LCD_command(0x80);      // Cursor at first line
        LCD_print("Hello");     // Show message
        delayMs(2000);
    }
}

void LCD_init(void) {
    SIM->SCGC5 |= 0x1000; // Enable clock to Port D

    // Configura PTD0-PTD7 como GPIO para datos
    for (int i = 0; i <= 7; i++) {
        PORTD->PCR[i] = 0x100; // MUX = 1: GPIO
    }
    PTD->PDDR = 0xFF; // PTD0-7 como salida

    SIM->SCGC5 |= 0x0200; // Enable clock to Port A

    // Configura PTA2 (RS), PTA4 (RW), PTA5 (EN)
    PORTA->PCR[2] = 0x100; // RS
    PORTA->PCR[4] = 0x100; // RW
    PORTA->PCR[5] = 0x100; // EN
    PTA->PDDR |= RS | RW | EN; // Salidas

    PTA->PCOR = RW; // RW = 0 (modo escritura)

    delayMs(30);     // Espera inicial
    LCD_command(0x30); delayMs(10);
    LCD_command(0x30); delayMs(1);
    LCD_command(0x30); delayMs(1);

    LCD_command(0x38); // 8-bit, 2-line, 5x7 font
    LCD_command(0x06); // Increment cursor
    LCD_command(0x01); // Clear display
    LCD_command(0x0F); // Display on, cursor blinking
}

void LCD_command(unsigned char command) {
    PTA->PCOR = RS | RW;        // RS = 0, RW = 0 (comando)
    PTD->PDOR = command;        // Enviar comando
    PTA->PSOR = EN;             // Pulso EN
    delayMs(1);
    PTA->PCOR = EN;

    if (command < 4)
        delayMs(4);  // Comandos que requieren más tiempo
    else
        delayMs(1);
}

void LCD_data(unsigned char data) {
    PTA->PSOR = RS;             // RS = 1 (dato)
    PTA->PCOR = RW;             // RW = 0 (escritura)
    PTD->PDOR = data;           // Enviar datos
    PTA->PSOR = EN;             // Pulso EN
    delayMs(1);
    PTA->PCOR = EN;
    delayMs(1);
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

void delayMs(int n) {
    int i;
    while (n-- > 0)
        for (i = 0; i < 7000; i++) __NOP(); // Ajustado para 48MHz
}

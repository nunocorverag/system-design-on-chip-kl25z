#include <MKL25Z4.H>

#define RS 0x04  // PTA2
#define RW 0x10  // PTA4 (RW debe estar conectado a GND si no se usa lectura)
#define EN 0x20  // PTA5

void delayMs(int n);
void LCD_send_nibble(unsigned char nibble);
void LCD_send_byte(unsigned char byte, int isData);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_print(char *str);

int main(void) {
    LCD_init();

    while (1) {
        LCD_command(0x01);     // Clear display
        delayMs(500);
        LCD_command(0x80);     // Cursor to first line
        LCD_print("Robot Bartender");
        delayMs(2000);
    }
}

void LCD_init(void) {
    SIM->SCGC5 |= 0x1000; // Clock Port D
    for (int i = 4; i <= 7; i++) { // PTD4-7 para datos (4-bit mode)
        PORTD->PCR[i] = 0x100; // GPIO
    }
    PTD->PDDR |= 0xF0; // PTD4-7 como salida

    SIM->SCGC5 |= 0x0200; // Clock Port A
    PORTA->PCR[2] = 0x100; // RS (PTA2)
    PORTA->PCR[4] = 0x100; // RW (PTA4)
    PORTA->PCR[5] = 0x100; // EN (PTA5)
    PTA->PDDR |= RS | RW | EN;

    PTA->PCOR = RW; // RW = 0 (escritura)

    delayMs(40); // Espera inicial

    // Secuencia de arranque especial en modo 4 bits
    LCD_send_nibble(0x30); delayMs(5);
    LCD_send_nibble(0x30); delayMs(1);
    LCD_send_nibble(0x30); delayMs(1);
    LCD_send_nibble(0x20); delayMs(1); // Cambia a modo 4 bits

    LCD_command(0x28); // 4-bit, 2 líneas, 5x7 font
    LCD_command(0x06); // Modo incremento
    LCD_command(0x01); // Limpiar pantalla
    LCD_command(0x0F); // Display ON, cursor ON, parpadeando
}

void LCD_send_nibble(unsigned char nibble) {
    PTD->PDOR &= ~0xF0;               // Limpia bits D4-D7
    PTD->PDOR |= (nibble & 0xF0);     // Coloca nibble en D4-D7
    PTA->PSOR = EN;                   // EN = 1
    delayMs(1);
    PTA->PCOR = EN;                   // EN = 0
}

void LCD_send_byte(unsigned char byte, int isData) {
    if (isData)
        PTA->PSOR = RS; // RS = 1 (dato)
    else
        PTA->PCOR = RS; // RS = 0 (comando)
    PTA->PCOR = RW;     // RW = 0

    LCD_send_nibble(byte & 0xF0);          // nibble alto
    LCD_send_nibble((byte << 4) & 0xF0);   // nibble bajo

    delayMs(2);
}

void LCD_command(unsigned char command) {
    LCD_send_byte(command, 0); // 0 = comando
}

void LCD_data(unsigned char data) {
    LCD_send_byte(data, 1);    // 1 = dato
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

void delayMs(int n) {
    int i;
    while (n-- > 0)
        for (i = 0; i < 7000; i++) __NOP(); // Aproximadamente 1ms a 48 MHz
}

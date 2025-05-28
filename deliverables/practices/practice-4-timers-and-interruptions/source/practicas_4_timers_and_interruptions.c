/*
 * Practica: LCD, Keyboard, Timers and Interrupts (LCD con I2C)
 */

#include "MKL25Z4.h"
#include <stdio.h>

// Variables globales
volatile int paused = 0;
volatile int counter = 0;

// Prototipos
void init_GPIO(void);
void i2c_init(void);
void delay_ms(uint32_t ms);
void LCD_init(void);
void LCD_clear(void);
void LCD_print(char *str);
void LCD_setCursor(uint8_t row, uint8_t col);
void LCD_Write_Byte(uint8_t data, uint8_t mode);
char keypad_getkey(void);

#define I2C_TRAN            I2C1->C1 |= I2C_C1_TX_MASK
#define I2C_REC             I2C1->C1 &= ~I2C_C1_TX_MASK
#define I2C_M_START         I2C1->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP          I2C1->C1 &= ~I2C_C1_MST_MASK
#define I2C_WAIT            while((I2C1->S & I2C_S_IICIF_MASK)==0) {} I2C1->S |= I2C_S_IICIF_MASK

#define LCD_ADDR_W          (0x27 << 1) // Dirección típica del LCD I2C
#define LCD_BACKLIGHT       0x08
#define LCD_EN              0x04
#define LCD_RS              0x01

// ===================== INTERRUPCIONES =====================
void PORTA_IRQHandler(void) {
    if (PORTA->ISFR & (1 << 1)) {
        paused = 1;
        LCD_clear();
        LCD_print("PAUSED");
        PORTA->ISFR |= (1 << 1);
    }
    if (PORTA->ISFR & (1 << 2)) {
        counter = 0;
        PORTA->ISFR |= (1 << 2);
    }
}

// ===================== RETARDO =====================
void delay_ms(uint32_t ms) {
    SysTick->LOAD = 48000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
    for (uint32_t i = 0; i < ms; i++) {
        while ((SysTick->CTRL & 0x10000) == 0);
    }
    SysTick->CTRL = 0;
}

// ===================== MAIN =====================
int main(void) {
    char buf[16];

    init_GPIO();
    i2c_init();
    LCD_init();
    delay_ms(1000);

    LCD_clear();
    LCD_print("Contador iniciado");
    delay_ms(1000);

    while (1) {
        if (!paused) {
            LCD_clear();
            LCD_setCursor(0, 0);
            LCD_print("Contador:");
            LCD_setCursor(1, 0);
            sprintf(buf, "%d", counter);
            LCD_print(buf);
            counter++;
            delay_ms(1000);
        } else {
            char key = keypad_getkey();
            if (key == '*') {
                paused = 0;
                LCD_clear();
            }
        }
    }
}

// ===================== INIT GPIO =====================
void init_GPIO(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;
    PORTA->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(9);
    PORTA->PCR[2] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(9);
    NVIC_EnableIRQ(PORTA_IRQn);
}

// ===================== INIT I2C =====================
void i2c_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[0] = PORT_PCR_MUX(6);
    PORTE->PCR[1] = PORT_PCR_MUX(6);
    I2C1->F = I2C_F_ICR(0x1F) | I2C_F_MULT(0);
    I2C1->C1 = I2C_C1_IICEN_MASK;
}

// ===================== LCD I2C =====================
void LCD_Write_Byte(uint8_t data, uint8_t mode) {
    uint8_t highnib = data & 0xF0;
    uint8_t lownib = (data << 4) & 0xF0;
    uint8_t rs = mode ? LCD_RS : 0;

    I2C_TRAN; I2C_M_START;
    I2C1->D = LCD_ADDR_W; I2C_WAIT;

    I2C1->D = highnib | LCD_BACKLIGHT | rs; I2C_WAIT;
    I2C1->D = highnib | LCD_BACKLIGHT | LCD_EN | rs; I2C_WAIT; delay_ms(1);
    I2C1->D = highnib | LCD_BACKLIGHT | rs; I2C_WAIT;

    I2C1->D = lownib | LCD_BACKLIGHT | rs; I2C_WAIT;
    I2C1->D = lownib | LCD_BACKLIGHT | LCD_EN | rs; I2C_WAIT; delay_ms(1);
    I2C1->D = lownib | LCD_BACKLIGHT | rs; I2C_WAIT;

    I2C_M_STOP;
    delay_ms(1);
}

void LCD_command(uint8_t cmd) {
    LCD_Write_Byte(cmd, 0);
}

void LCD_data(uint8_t data) {
    LCD_Write_Byte(data, 1);
}

void LCD_init(void) {
    delay_ms(50);
    LCD_command(0x33);
    LCD_command(0x32);
    LCD_command(0x28);
    LCD_command(0x0C);
    LCD_command(0x06);
    LCD_command(0x01);
    delay_ms(2);
}

void LCD_clear(void) {
    LCD_command(0x01);
    delay_ms(2);
}

void LCD_setCursor(uint8_t row, uint8_t col) {
    uint8_t address[] = {0x00, 0x40};
    LCD_command(0x80 | (address[row] + col));
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

// ===================== Keypad 4x4 (igual) =====================
const uint8_t row_pins[4] = {0, 1, 2, 3};
const uint8_t col_pins[4] = {4, 5, 6, 7};

const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char keypad_getkey(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    for (int r = 0; r < 4; r++) {
        PORTD->PCR[row_pins[r]] = PORT_PCR_MUX(1);
        PTD->PDDR |= (1 << row_pins[r]);
    }
    for (int c = 0; c < 4; c++) {
        PORTD->PCR[col_pins[c]] = PORT_PCR_MUX(1);
        PTD->PDDR &= ~(1 << col_pins[c]);
    }

    while (1) {
        for (int r = 0; r < 4; r++) {
            PTD->PSOR = 0x0F;
            PTD->PCOR = (1 << row_pins[r]);
            delay_ms(5);
            for (int c = 0; c < 4; c++) {
                if (!(PTD->PDIR & (1 << col_pins[c]))) {
                    while (!(PTD->PDIR & (1 << col_pins[c])));
                    return keymap[r][c];
                }
            }
        }
    }
}

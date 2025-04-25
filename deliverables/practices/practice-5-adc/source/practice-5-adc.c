#include <MKL25Z4.h>
#include <stdio.h>

// ========== CONFIG I2C PARA LCD ==========
#define I2C_TRAN            I2C1->C1 |= I2C_C1_TX_MASK
#define I2C_REC             I2C1->C1 &= ~I2C_C1_TX_MASK
#define I2C_M_START         I2C1->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP          I2C1->C1 &= ~I2C_C1_MST_MASK
#define I2C_WAIT            while((I2C1->S & I2C_S_IICIF_MASK)==0) { } I2C1->S |= I2C_S_IICIF_MASK

#define LCD_ADDR_W          (0x27 << 1) // Dirección 0x27 (o 0x3F si tu módulo es diferente)
#define LCD_BACKLIGHT       0x08
#define LCD_EN              0x04
#define LCD_RS              0x01

// ========== DELAY ==========
void delayMs(int n) {
    int j;
    for(j = 0; j < n; j++) {
        TPM0->CNT = 0;
        while((TPM0->SC & 0x80) == 0) {}
        TPM0->SC |= 0x80; // clear TOF
    }
}

// ========== I2C ==========
void i2c_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    PORTE->PCR[0] = PORT_PCR_MUX(6); // PTE0 = SCL
    PORTE->PCR[1] = PORT_PCR_MUX(6); // PTE1 = SDA

    I2C1->F = I2C_F_ICR(0x1F) | I2C_F_MULT(0); // 100kHz
    I2C1->C1 = I2C_C1_IICEN_MASK;
}

// ========== LCD sobre I2C ==========
void LCD_Write_Byte(uint8_t data, uint8_t mode) {
    uint8_t highnib = data & 0xF0;
    uint8_t lownib = (data << 4) & 0xF0;
    uint8_t rs = mode ? LCD_RS : 0;

    I2C_TRAN;
    I2C_M_START;
    I2C1->D = LCD_ADDR_W;
    I2C_WAIT;

    I2C1->D = highnib | LCD_BACKLIGHT | rs;
    I2C_WAIT;
    I2C1->D = highnib | LCD_BACKLIGHT | LCD_EN | rs;
    I2C_WAIT;
    delayMs(1);
    I2C1->D = highnib | LCD_BACKLIGHT | rs;
    I2C_WAIT;

    I2C1->D = lownib | LCD_BACKLIGHT | rs;
    I2C_WAIT;
    I2C1->D = lownib | LCD_BACKLIGHT | LCD_EN | rs;
    I2C_WAIT;
    delayMs(1);
    I2C1->D = lownib | LCD_BACKLIGHT | rs;
    I2C_WAIT;

    I2C_M_STOP;
    delayMs(1);
}

void LCD_Command(uint8_t cmd) {
    LCD_Write_Byte(cmd, 0);
}

void LCD_Data(uint8_t data) {
    LCD_Write_Byte(data, 1);
}

void LCD_init(void) {
    delayMs(50);
    LCD_Command(0x33);
    LCD_Command(0x32);
    LCD_Command(0x28); // 4 bits, 2 líneas
    LCD_Command(0x0C); // Display ON
    LCD_Command(0x06); // Entry mode
    LCD_Command(0x01); // Clear
    delayMs(2);
}

void LCD_clear(void) {
    LCD_Command(0x01);
    delayMs(2);
}

void LCD_setCursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    LCD_Command(0x80 | (col + row_offsets[row]));
}

void LCD_print(const char *str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

// ========== ADC ==========
void ADC0_init(void) {
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    PORTB->PCR[0] = PORT_PCR_MUX(0); // PTB0 como entrada analógica

    ADC0->CFG1 = ADC_CFG1_MODE(3)     // 16 bits
               | ADC_CFG1_ADICLK(0)    // Bus clock
               | ADC_CFG1_ADIV(3);     // Dividir clk/8
    ADC0->CFG2 = ADC_CFG2_ADLSTS(0);  // Default longest sample time
    ADC0->SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3); // Promediado 32 muestras
}

uint16_t ADC0_read(uint8_t channel) {
    ADC0->SC1[0] = channel & ADC_SC1_ADCH_MASK;
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    return ADC0->R[0];
}

float convertToTemperature(uint16_t adcValue) {
    // 10 mV/°C, referencia 3.3V, resolución 16 bits
    return (adcValue * 330.0f) / 65535.0f;
}

// ========== MAIN ==========
int main(void) {
    uint16_t adcValue;
    float temperature;

    // Inicializar TPM0 para delayMs
    SIM->SCGC6 |= 0x01000000;
    SIM->SOPT2 |= 0x01000000;
    TPM0->SC = 0;
    TPM0->SC = 0x02;
    TPM0->MOD = 0x2000;
    TPM0->SC |= 0x80;
    TPM0->SC |= 0x08;

    // Inicializar sistemas
    i2c_init();
    ADC0_init();
    LCD_init();

    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_print("Sensor LM35");
    LCD_setCursor(1, 0);
    LCD_print("Iniciando...");
    delayMs(1000);

    while (1) {
        adcValue = ADC0_read(8); // PTB0 = canal 8
        temperature = convertToTemperature(adcValue);

        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("Temperatura:");
        LCD_setCursor(1, 0);

        int temp_int = (int)temperature;
        int temp_dec = (int)((temperature - temp_int) * 10);

        if (temp_int < 10) {
            LCD_Data('0' + temp_int);
        } else if (temp_int < 100) {
            LCD_Data('0' + (temp_int / 10));
            LCD_Data('0' + (temp_int % 10));
        } else {
            LCD_Data('0' + (temp_int / 100));
            LCD_Data('0' + ((temp_int / 10) % 10));
            LCD_Data('0' + (temp_int % 10));
        }

        LCD_Data('.');
        LCD_Data('0' + temp_dec);
        LCD_print(" C");

        delayMs(1000);
    }
}
#include "MKL25Z4.h"
#include <stdio.h>
#include <stdint.h>

// ------------------ Macros ------------------
#define PWM_MOD 20833
#define SERVO_MIN_PULSE 1041
#define SERVO_CENTER_PULSE 1562
#define SERVO_MAX_PULSE 2083

#define I2C_LCD_ADDR 0x27
#define LCD_BACKLIGHT 0x08
#define LCD_EN 0x04
#define LCD_RS 0x01
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_SET 0x28
#define LCD_LINE1 0x80
#define LCD_LINE2 0xC0

#define ADC_CHANNEL 14
#define EMERGENCY_BTN_PIN 12
#define MENU_BTN_PIN 13

#define ROW_START_PIN 4
#define COL_START_PIN 0

// ------------------ Utilidades ------------------
void delay_ms(int ms) {
    for (volatile int i = 0; i < ms * 4000; i++);
}

// ------------------ LCD I2C ------------------
void i2c_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[1] = PORT_PCR_MUX(6);
    PORTE->PCR[0] = PORT_PCR_MUX(6);
    I2C1->C1 = 0;
    I2C1->F = I2C_F_ICR(0x12) | I2C_F_MULT(0);
    I2C1->C1 |= I2C_C1_IICEN_MASK;
}

void i2c_write(uint8_t data) {
    I2C1->C1 |= I2C_C1_TX_MASK | I2C_C1_MST_MASK;
    I2C1->D = (I2C_LCD_ADDR << 1);
    while (!(I2C1->S & I2C_S_IICIF_MASK));
    I2C1->S |= I2C_S_IICIF_MASK;
    I2C1->D = data;
    while (!(I2C1->S & I2C_S_IICIF_MASK));
    I2C1->S |= I2C_S_IICIF_MASK;
    I2C1->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TX_MASK);
}

void lcd_enable_pulse(uint8_t data) {
    i2c_write(data | LCD_EN);
    delay_ms(1);
    i2c_write(data & ~LCD_EN);
    delay_ms(1);
}

void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4) | LCD_BACKLIGHT | rs;
    lcd_enable_pulse(data);
}

void lcd_send_command(uint8_t command) {
    lcd_send_nibble(command >> 4, 0);
    lcd_send_nibble(command & 0x0F, 0);
    delay_ms(2);
}

void lcd_send_data(uint8_t data) {
    lcd_send_nibble(data >> 4, LCD_RS);
    lcd_send_nibble(data & 0x0F, LCD_RS);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    lcd_send_command((row == 0 ? LCD_LINE1 : LCD_LINE2) + col);
}

void lcd_print(const char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_init(void) {
    i2c_init();
    delay_ms(50);
    lcd_send_nibble(0x03, 0); delay_ms(5);
    lcd_send_nibble(0x03, 0); delay_ms(1);
    lcd_send_nibble(0x03, 0); delay_ms(1);
    lcd_send_nibble(0x02, 0); delay_ms(1);
    lcd_send_command(LCD_FUNCTION_SET);
    lcd_send_command(LCD_DISPLAY_ON);
    lcd_send_command(LCD_CLEAR);
    lcd_send_command(LCD_ENTRY_MODE);
}

// ------------------ Teclado 4x4 ------------------
void keypad_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    for (int i = ROW_START_PIN; i < ROW_START_PIN + 4; i++) {
        PORTD->PCR[i] = PORT_PCR_MUX(1);
        PTD->PDDR |= (1 << i);
        PTD->PSOR = (1 << i);
    }
    for (int i = COL_START_PIN; i < COL_START_PIN + 4; i++) {
        PORTD->PCR[i] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
        PTD->PDDR &= ~(1 << i);
    }
}

char keypad_getkey(void) {
    const char keymap[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };
    while (1) {
        for (int row = 0; row < 4; row++) {
            for (int i = 0; i < 4; i++) {
                if (i == row) PTD->PCOR = (1 << (ROW_START_PIN + i));
                else PTD->PSOR = (1 << (ROW_START_PIN + i));
            }
            delay_ms(5);
            for (int col = 0; col < 4; col++) {
                if (!(PTD->PDIR & (1 << (COL_START_PIN + col)))) {
                    delay_ms(20);
                    if (!(PTD->PDIR & (1 << (COL_START_PIN + col)))) {
                        while (!(PTD->PDIR & (1 << (COL_START_PIN + col))));
                        delay_ms(20);
                        return keymap[row][col];
                    }
                }
            }
        }
    }
}

// ------------------ PWM Servo ------------------
void pwm_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
    PORTB->PCR[0] = PORT_PCR_MUX(3);
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    TPM1->SC = 0;
    TPM1->MOD = PWM_MOD;
    TPM1->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM1->CONTROLS[0].CnV = SERVO_CENTER_PULSE;
    TPM1->SC = TPM_SC_PS(4) | TPM_SC_CMOD(1);
}

void pwm_set_duty(uint16_t duty) {
    TPM1->CONTROLS[0].CnV = duty;
}

// ------------------ Botones ------------------
void gpio_buttons_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[EMERGENCY_BTN_PIN] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTA->PCR[MENU_BTN_PIN] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PTA->PDDR &= ~((1 << EMERGENCY_BTN_PIN) | (1 << MENU_BTN_PIN));
}

int button_pressed(int pin) {
    return (PTA->PDIR & (1 << pin)); // activo en alto (pull-down físico)
}

// ---------------- ADC ---------------------
void adc_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;         // Habilita reloj para PORTC
    PORTC->PCR[0] = PORT_PCR_MUX(0);            // PTC0 (ADC0_SE14) como entrada analógica
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;          // Habilita ADC0
    ADC0->CFG1 = ADC_CFG1_ADIV(3) | ADC_CFG1_MODE(1); // Divide reloj, 12 bits
    ADC0->SC3 = 0;
}

uint16_t adc_read(void) {
    ADC0->SC1[0] = ADC_CHANNEL;
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    return ADC0->R[0];
}

// ------------------ Modos ------------------
void manual_mode(void) {

    lcd_send_command(0x01); // Limpiar pantalla
    lcd_set_cursor(0, 0);
    lcd_print("Manual Mode");
    delay_ms(500);

    uint16_t duty_array[4] = {
        SERVO_MIN_PULSE,
        SERVO_MIN_PULSE + 350,   // 45°
        SERVO_CENTER_PULSE,      // 90°
        SERVO_MAX_PULSE          // 180°
    };

    while (1) {
        // Mostrar instrucciones
        lcd_send_command(LCD_CLEAR);
        lcd_set_cursor(0, 0);
        lcd_print("1:0° 2:45° ");
        lcd_set_cursor(1, 0);
        lcd_print("3:90° 4:180°");

        char key = keypad_getkey();

        if (key >= '1' && key <= '4') {
            pwm_set_duty(duty_array[key - '1']);

            // Mostrar confirmación
            lcd_send_command(LCD_CLEAR);
            lcd_set_cursor(0, 0);
            lcd_print("Grado aplicado:");
            lcd_set_cursor(1, 0);
            lcd_send_data(key);

            // Esperar botón físico
            while (1) {
                if (button_pressed(MENU_BTN_PIN)) {
                    while (button_pressed(MENU_BTN_PIN));
                    return;  // salir de manual_mode()
                }
                if (button_pressed(EMERGENCY_BTN_PIN)) {
                    lcd_send_command(LCD_CLEAR);
                    lcd_set_cursor(0, 0);
                    lcd_print("EMERGENCY");
                    pwm_set_duty(SERVO_MIN_PULSE);
                    delay_ms(1000);
                    while (button_pressed(EMERGENCY_BTN_PIN));
                    return;
                }
            }
        }
    }
}

void automatic_mode(void) {
    lcd_send_command(0x01); // Limpiar pantalla
    lcd_set_cursor(0, 0);
    lcd_print("Automatic Mode");
    delay_ms(500);

    while (1) {
        // Leer ADC
        uint16_t adc_val = adc_read();

        // Calcular voltaje (en mV)
        uint32_t mv = (3300 * adc_val) / 4095;
        uint16_t v_ent = mv / 1000;        // parte entera
        uint16_t v_dec = (mv % 1000) / 10; // parte decimal (2 cifras)

        // Calcular posición del servo
        uint16_t pulse = SERVO_MIN_PULSE +
                         ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * adc_val / 4095);
        pwm_set_duty(pulse);

        // Mostrar en LCD
        char buffer[17];

        lcd_send_command(0x01);  // Limpiar
        lcd_set_cursor(0, 0);
        sprintf(buffer, "V=%d.%02dV", v_ent, v_dec);
        lcd_print(buffer);

        lcd_set_cursor(1, 0);
        sprintf(buffer, "ADC=%d    ", adc_val);
        lcd_print(buffer);

        // Verificar botón MENU
        if (button_pressed(MENU_BTN_PIN)) {
            while (button_pressed(MENU_BTN_PIN));
            break;
        }

        // Verificar botón EMERGENCY
        if (button_pressed(EMERGENCY_BTN_PIN)) {
            lcd_send_command(0x01);
            lcd_set_cursor(0, 0);
            lcd_print("EMERGENCY");
            pwm_set_duty(SERVO_MIN_PULSE);
            delay_ms(1000);
            while (button_pressed(EMERGENCY_BTN_PIN));
            break;
        }

        delay_ms(300);
    }
}


// ------------------ Main ------------------
int main(void) {
    // Inicialización de módulos
    lcd_init();             // LCD I2C
    pwm_init();             // PWM para servo
    keypad_init();          // Teclado 4x4
    gpio_buttons_init();    // Botones PTA12 y PTA13
    adc_init();             // ADC para modo automático

    // Mensaje de bienvenida
    lcd_send_command(LCD_CLEAR);
    lcd_set_cursor(0, 0);
    lcd_print("Servo Control");
    delay_ms(1000);

    // Posición inicial del servo: centro
    pwm_set_duty(SERVO_MIN_PULSE);

    while (1) {
        // Mostrar menú principal
        lcd_send_command(LCD_CLEAR);
        lcd_set_cursor(0, 0);
        lcd_print("1: Manual");
        lcd_set_cursor(1, 0);
        lcd_print("2: Automatic");

        // Leer selección del teclado
        char opt = keypad_getkey();

        if (opt == '1') {
            manual_mode();      // Entra en modo manual hasta botón físico
        } else if (opt == '2') {
            automatic_mode();   // Entra en modo automático hasta botón físico
        }
    }
}

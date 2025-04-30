#include <MKL25Z4.h>
#include <stdio.h>
#include <string.h>

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

// Definición de pines para LEDs
#define LED_ORANGE    (1 << 8)   // PTB8
#define LED_YELLOW    (1 << 9)   // PTB9
#define LED_GREEN     (1 << 10)  // PTB10

/* ========= DELAY ========= */
void delayMs(int n) {
    int i;
    TPM0->MOD = 41940 - 1;  // 1ms @ 48MHz con prescaler /4

    for(i = 0; i < n; i++) {
        TPM0->CNT = 0;
        TPM0->SC |= TPM_SC_CMOD(1);
        while((TPM0->SC & TPM_SC_TOF_MASK) == 0) {}
        TPM0->SC |= TPM_SC_TOF_MASK;  // clear TOF
    }
}

/* ========= I2C FUNCTIONS ========= */
void i2c_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    PORTE->PCR[0] = PORT_PCR_MUX(6); // PTE0 = SCL
    PORTE->PCR[1] = PORT_PCR_MUX(6); // PTE1 = SDA

    I2C1->F = I2C_F_ICR(0x1F) | I2C_F_MULT(0); // 100kHz
    I2C1->C1 = I2C_C1_IICEN_MASK;
}

/* ========= LCD I2C FUNCTIONS ========= */
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

void LCD_command(uint8_t cmd) {
    LCD_Write_Byte(cmd, 0);
}

void LCD_data(uint8_t data) {
    LCD_Write_Byte(data, 1);
}

void LCD_init(void) {
    delayMs(50);
    LCD_command(0x33);
    LCD_command(0x32);
    LCD_command(0x28); // 4 bits, 2 líneas
    LCD_command(0x0C); // Display ON
    LCD_command(0x06); // Entry mode
    LCD_command(0x01); // Clear
    delayMs(2);
}

void LCD_clear(void) {
    LCD_command(0x01);
    delayMs(2);
}

void LCD_setCursor(int row, int col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    LCD_command(0x80 | (col + row_offsets[row]));
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

/* ========= TECLADO ========= */
void keypad_init(void) {
    SIM->SCGC5 |= 0x0800;      // Habilitar Puerto C
    for (int i = 0; i < 8; i++)
        PORTC->PCR[i] = 0x103; // GPIO + pull-up
    PTC->PDDR = 0x0F;          // PC0-PC3 como salidas (filas)
}

int keypad_getkey(void) {
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};  // Máscara de filas
    int row, col;
    int key = -1;  // Valor por defecto: no hay tecla presionada

    // Configuración para leer las filas
    PTC->PDDR |= 0x0F;           // Establecer las primeras 4 bits como salida (filas)
    PTC->PCOR = 0x0F;            // Poner las filas a bajo
    delayMs(1);                  // Espera
    col = PTC->PDIR & 0xF0;      // Leer columnas
    PTC->PDDR = 0;               // Establecer las columnas como entradas
    if (col == 0xF0) return -1;  // Si no se presionó ninguna tecla, retornar -1

    // Escaneo de filas
    for (row = 0; row < 4; row++) {
        PTC->PDDR = 0;
        PTC->PDDR |= row_select[row];   // Activar una fila a la vez
        PTC->PCOR = row_select[row];    // Poner esta fila a bajo
        delayMs(1);                     // Esperar a que se estabilice
        col = PTC->PDIR & 0xF0;         // Leer columnas
        if (col != 0xF0) break;         // Si se detecta una tecla, salir
    }

    PTC->PDDR = 0;  // Restaurar filas como entrada
    if (row == 4) return -1;  // No se detectó tecla

    // Mapear teclas
    if (col == 0xE0) {        // Primera columna
        if (row == 0) key = 1;
        else if (row == 1) key = 4;
        else if (row == 2) key = 7;
        else if (row == 3) key = 10; // *
    }
    else if (col == 0xD0) {   // Segunda columna
        if (row == 0) key = 2;
        else if (row == 1) key = 5;
        else if (row == 2) key = 8;
        else if (row == 3) key = 0; // 0
    }
    else if (col == 0xB0) {   // Tercera columna
        if (row == 0) key = 3;
        else if (row == 1) key = 6;
        else if (row == 2) key = 9;
        else if (row == 3) key = 11; // #
    }
    else if (col == 0x70) {   // Cuarta columna
        if (row == 0) key = 12; // A
        else if (row == 1) key = 13; // B
        else if (row == 2) key = 14; // C
        else if (row == 3) key = 15; // D
    }

    return key;
}

/* ========= LEDS ========= */
void leds_init(void) {
    // Habilitar reloj para puerto B
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Configurar pines como GPIO (PTB8, PTB9, PTB10)
    PORTB->PCR[8] = PORT_PCR_MUX(1);   // LED_ORANGE (PTB8)
    PORTB->PCR[9] = PORT_PCR_MUX(1);   // LED_YELLOW (PTB9)
    PORTB->PCR[10] = PORT_PCR_MUX(1);  // LED_GREEN (PTB10)

    // Configurar como salidas
    PTB->PDDR |= (LED_ORANGE | LED_GREEN | LED_YELLOW);

    // Inicializar LEDs apagados (activo en alto)
    PTB->PCOR = (LED_ORANGE | LED_GREEN | LED_YELLOW);  // Pines en bajo (LEDs apagados)
}

void led_on(int led) {
    PTB->PSOR = led;  // Encender: Nivel ALTO
}

void led_off(int led) {
    PTB->PCOR = led;  // Apagar: Nivel BAJO
}

/* ========= TIMER (PARTE 2) ========= */

// Variables globales para el temporizador
volatile int seconds_count = 0;
volatile int timer_elapsed = 0;

// Inicializar TPM1 para interrupción cada segundo
void timer_init(void) {
    // Habilitar reloj para TPM1
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

    // Configurar fuente de reloj para TPM1
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // Fuente de reloj MCGFLLCLK o MCGPLLCLK/2

    // Configurar TPM1
    TPM1->SC = 0; // Detener timer y limpiar configuración
    TPM1->MOD = 48000000 / 128 - 1; // Para 1 segundo con prescaler /128 @ 48MHz

    // Configurar para interrupción en overflow
    TPM1->SC = TPM_SC_TOIE_MASK | TPM_SC_PS(7); // Prescaler /128, interrupción habilitada

    // Habilitar interrupción para TPM1 en NVIC
    NVIC_EnableIRQ(TPM1_IRQn);
}

// Handler de interrupción para TPM1
void TPM1_IRQHandler(void) {
    // Limpiar la bandera de interrupción
    TPM1->SC |= TPM_SC_TOF_MASK;

    seconds_count++;

    // Mostrar contador en LCD
    LCD_setCursor(1, 6);

    char time_str[6];
    sprintf(time_str, "%2d s", seconds_count);
    LCD_print(time_str);

    // Verificar si el temporizador ha finalizado
    if (seconds_count >= timer_elapsed) {
        // Detener temporizador
        TPM1->SC &= ~TPM_SC_CMOD_MASK;

        // Alerta visual (secuencia de LEDs)
        for (int i = 0; i < 3; i++) {
            led_on(LED_ORANGE);
            delayMs(50);
            led_off(LED_ORANGE);
            led_on(LED_YELLOW);
            delayMs(50);
            led_off(LED_YELLOW);
            led_on(LED_GREEN);
            delayMs(50);
            led_off(LED_GREEN);
        }

        // Asegurarse de que todos los LEDs queden apagados
        led_off(LED_ORANGE);
        led_off(LED_YELLOW);
        led_off(LED_GREEN);

        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("TIME ELAPSED!");
        delayMs(200);  // Mostrar mensaje por 2 segundos

        // Restablecer contador para la próxima vez
        seconds_count = 0;
        timer_elapsed = 0;
    }
}

// Función principal para el temporizador ascendente
// Función principal para el temporizador ascendente
void run_timer(void) {
    while (1) {
        int key;
        int entered_time = 0;
        int digits = 0;
        char time_str[16] = "";

        // 1. Mensaje de bienvenida
        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("TIMER MODE:");
        LCD_setCursor(1, 0);
        LCD_print("* Enter # Exit");
        
        // Esperar a que el usuario presione * para continuar o # para salir
        while (1) {
            key = keypad_getkey();
            if (key == 10) {  // Tecla * para continuar
                break;
            } else if (key == 11) {  // Tecla # para salir
                return;  // Salir del modo Timer
            }
            delayMs(100);
        }

        // 2. Solicitar tiempo al usuario
        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("Enter seconds:");

        // Bucle para capturar dígitos
        while (1) {
             key = keypad_getkey();

             if (key != -1) {  // Se presionó una tecla
                 if (key >= 0 && key <= 9) {  // Dígito 0-9
                     if (digits == 0 && key == 0) {
                         // No permitir 0 como primer dígito
                     } else {
                         entered_time = entered_time * 10 + key;
                         LCD_clear();
                         LCD_setCursor(0, 0);
                         LCD_print("Seconds: ");
                         sprintf(time_str, "%d", entered_time);
                         LCD_print(time_str);
                         LCD_setCursor(1, 0);
                         LCD_print("* to confirm");
                         digits++;
                         delayMs(10);
                     }
                 } else if (key == 10) {  // Tecla *
                     if (digits > 0) {
                         break;
                     }
                 }
             }
             delayMs(10);
        }

        // Configurar temporizador
        timer_elapsed = entered_time;
        seconds_count = 0;
        
        // Iniciar el contador
        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("Counting:");
        LCD_setCursor(1, 0);
        LCD_print("Time:");
        
        // Iniciar temporizador
        TPM1->CNT = 0;
        TPM1->SC |= TPM_SC_CMOD(1); // Iniciar contador
        
        // Esperar hasta que termine el temporizador
        while (seconds_count < timer_elapsed) {
            delayMs(100);
        }
        
        // Esperar después de mostrar "TIME ELAPSED!" (el IRQHandler muestra este mensaje)
        delayMs(100);
    }
}
/* ========= MENÚ (PARTE 1) ========= */
void display_menu(void) {
    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_print("1:O 2:Y 3:G");
    LCD_setCursor(1, 0);
    LCD_print("# Exit");
}

void control_led(int key) {
    LCD_clear();
    
    switch(key) {
        case 1:
            LCD_setCursor(0, 0);
            LCD_print("Orange LED On");
            led_on(LED_ORANGE);
            led_off(LED_YELLOW);
            led_off(LED_GREEN);
            break;
        case 2:
            LCD_setCursor(0, 0);
            LCD_print("Yellow LED On");
            led_off(LED_ORANGE);
            led_on(LED_YELLOW);
            led_off(LED_GREEN);
            break;
        case 3:
            LCD_setCursor(0, 0);
            LCD_print("Green LED On");
            led_off(LED_ORANGE);
            led_off(LED_YELLOW);
            led_on(LED_GREEN);
            break;
        default:
            break;
    }
    
    delayMs(1000);
    
    // Apagar el LED después de mostrar el mensaje
    led_off(LED_ORANGE);
    led_off(LED_YELLOW);
    led_off(LED_GREEN);
}

// Función para ejecutar el modo LED (Parte 1)
void run_led_mode(void) {
    char key;
    
    while (1) {
        display_menu();
        
        while (1) {
            key = keypad_getkey();
            
            if (key == 1 || key == 2 || key == 3) {
                control_led(key);
                break;
            } else if (key == 11) {  // Tecla # para salir
                return;  // Salir del modo LED
            }
            
            delayMs(100);
        }
    }
}

/* ========= MAIN ========= */
int main(void) {
    // Inicializar TPM0 para delayMs
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    TPM0->SC = 0;
    TPM0->SC = TPM_SC_PS(2);  // prescaler /4

    // Inicializar periféricos
    i2c_init();       // Inicializar I2C antes del LCD
    LCD_init();       // Inicializar LCD por I2C
    keypad_init();    // Inicializar teclado
    leds_init();      // Inicializar LEDs
    timer_init();     // Inicializar TPM1 para interrupciones

    int mode_key;
    
    // Bucle principal del programa
    while(1) {
        // Mostrar menú principal
        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("Welcome!");
        LCD_setCursor(1, 0);
        LCD_print("1:LED 2:Timer");
        
        // Esperar selección de modo
        while (1) {
            mode_key = keypad_getkey();
            
            if (mode_key == 1) {
                // Modo LED
                run_led_mode();
                break;
            } else if (mode_key == 2) {
                // Modo Timer
                run_timer();
                break;
            }
            
            delayMs(100);
        }
    }
}
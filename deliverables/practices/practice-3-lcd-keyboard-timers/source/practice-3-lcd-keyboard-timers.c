/* ========= LEDS (Parte 1) ========= */
//#include <MKL25Z4.h>
//#define RS 0x04  // PTA2
//#define RW 0x10  // PTA4
//#define EN 0x20  // PTA5
//
//// Definición de pines para LEDs
//#define LED_ORANGE    (1 << 8)   // PTB8 (bit 8)
//#define LED_YELLOW   (1 << 9)   // PTB9 (bit 9)
//#define LED_GREEN  (1 << 10)  // PTB10 (bit 10)
//
///* ========= DELAY ========= */
//void delayMs(int n) {
//    int i;
//    TPM0->MOD = 41940 - 1;  // 1ms @ 48MHz con prescaler /4
//
//    for(i = 0; i < n; i++) {
//        TPM0->CNT = 0;
//        TPM0->SC |= TPM_SC_CMOD(1);
//        while((TPM0->SC & TPM_SC_TOF_MASK) == 0) {}
//        TPM0->SC |= TPM_SC_TOF_MASK;  // clear TOF
//    }
//}
//
///* ========= LCD ========= */
//void LCD_command(unsigned char command) {
//    PTA->PCOR = RS | RW;       // RS = 0, RW = 0
//    PTD->PDOR = command;       // enviar comando
//    PTA->PSOR = EN;            // EN = 1
//    delayMs(1);
//    PTA->PCOR = EN;            // EN = 0
//    if (command < 4) delayMs(2); else delayMs(1);
//}
//
//void LCD_data(unsigned char data) {
//    PTA->PSOR = RS;            // RS = 1
//    PTA->PCOR = RW;            // RW = 0
//    PTD->PDOR = data;
//    PTA->PSOR = EN;            // EN = 1
//    delayMs(1);
//    PTA->PCOR = EN;            // EN = 0
//    delayMs(1);
//}
//
//void LCD_init(void) {
//    SIM->SCGC5 |= 0x1000;      // Habilitar Puerto D
//    for (int i = 0; i < 8; i++)
//        PORTD->PCR[i] = 0x100; // Configurar como GPIO
//    PTD->PDDR = 0xFF;          // Configurar como salidas
//
//    SIM->SCGC5 |= 0x0200;      // Habilitar Puerto A
//    PORTA->PCR[2] = 0x100;     // Configurar como GPIO
//    PORTA->PCR[4] = 0x100;
//    PORTA->PCR[5] = 0x100;
//    PTA->PDDR |= (RS | RW | EN); // Configurar como salidas
//
//    delayMs(30);
//    LCD_command(0x30); delayMs(10);
//    LCD_command(0x30); delayMs(1);
//    LCD_command(0x30);
//    LCD_command(0x38);         // 8 bits, 2 líneas
//    LCD_command(0x06);         // Modo entrada
//    LCD_command(0x01);         // Limpiar pantalla
//    LCD_command(0x0F);         // Encender display y cursor
//}
//
//void LCD_print(char *str) {
//    while (*str) {
//        LCD_data(*str++);
//    }
//}
//
//void LCD_clear(void) {
//    LCD_command(0x01);         // Limpiar pantalla
//    delayMs(2);                // Esperar a que se complete
//}
//
//void LCD_setCursor(int row, int col) {
//    int address;
//    if (row == 0)
//        address = 0x80 + col;  // Primera línea
//    else
//        address = 0xC0 + col;  // Segunda línea
//    LCD_command(address);
//}
//
///* ========= TECLADO ========= */
//void keypad_init(void) {
//    SIM->SCGC5 |= 0x0800;      // Habilitar Puerto C
//    for (int i = 0; i < 8; i++)
//        PORTC->PCR[i] = 0x103; // GPIO + pull-up
//    PTC->PDDR = 0x0F;          // PC0-PC3 como salidas (filas)
//}
//
//char keypad_getkey(void) {
//    const char row_select[] = {0x01, 0x02, 0x04, 0x08};
//    int row, col;
//
//    PTC->PDDR |= 0x0F;
//    PTC->PCOR = 0x0F;
//    delayMs(1);
//    col = PTC->PDIR & 0xF0;
//    PTC->PDDR = 0;
//    if (col == 0xF0) return 0;
//
//    for (row = 0; row < 4; row++) {
//        PTC->PDDR = 0;
//        PTC->PDDR |= row_select[row];
//        PTC->PCOR = row_select[row];
//        delayMs(1);
//        col = PTC->PDIR & 0xF0;
//        if (col != 0xF0) break;
//    }
//
//    PTC->PDDR = 0;
//    if (row == 4) return 0;
//
//    if (col == 0xE0) return row * 4 + 1;
//    if (col == 0xD0) return row * 4 + 2;
//    if (col == 0xB0) return row * 4 + 3;
//    if (col == 0x70) return row * 4 + 4;
//
//    return 0;
//}
//
///* ========= LEDS ========= */
//void leds_init(void) {
//    // Habilitar reloj para puerto B
//    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
//
//    // Configurar pines como GPIO (PTB8, PTB9, PTB10)
//    PORTB->PCR[8] = PORT_PCR_MUX(1);   // LED_ORANGE (PTB8)
//    PORTB->PCR[9] = PORT_PCR_MUX(1);   // LED_YELLOW (PTB9)
//    PORTB->PCR[10] = PORT_PCR_MUX(1);  // LED_GREEN (PTB10)
//
//    // Configurar como salidas
//    PTB->PDDR |= (LED_ORANGE | LED_GREEN | LED_YELLOW);
//
//    // **Corrección clave:** Inicializar LEDs apagados (activo en alto)
//    PTB->PCOR = (LED_ORANGE | LED_GREEN | LED_YELLOW);  // Pines en bajo (LEDs apagados)
//}
//
//void led_on(int led) {
//    PTB->PSOR = led;  // Encender: Nivel ALTO
//}
//void led_off(int led) {
//    PTB->PCOR = led;  // Apagar: Nivel BAJO
//}
//
///* ========= FUNCIONES DE MENÚ Y CONTROL DE LEDS ========= */
//void display_menu() {
//    LCD_clear();
//    LCD_setCursor(0, 0);
//    LCD_print("PRESS BUTTON");
//    LCD_setCursor(1, 0);
//    LCD_print("O: 1 Y: 2 G: 3");
//}
//
//void control_led(int led_choice) {
//    LCD_clear();
//    LCD_setCursor(0, 0);
//
//    switch(led_choice) {
//        case 1: // LED ROJO
//            LCD_print("ORANGE LED IS ON!");
//            led_on(LED_ORANGE);
//            delayMs(200);
//            led_off(LED_ORANGE);
//            break;
//
//        case 2: // LED AZUL
//            LCD_print("YELLOW LED IS ON!");
//            led_on(LED_YELLOW);
//            delayMs(200);
//            led_off(LED_YELLOW);
//            break;
//
//        case 3: // LED VERDE
//            LCD_print("GREEN LED IS ON!");
//            led_on(LED_GREEN);
//            delayMs(200);
//            led_off(LED_GREEN);
//            break;
//
//        default:
//            LCD_print("INVALID CHOICE");
//            delayMs(200);
//            break;
//    }
//}
//
///* ========= MAIN ========= */
//int main(void) {
//    char key;
//
//    // Inicializar TPM0 para delayMs
//    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
//    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
//    TPM0->SC = 0;
//    TPM0->SC = TPM_SC_PS(2);  // prescaler /4
//
//    // Inicializar periféricos
//    keypad_init();
//    LCD_init();
//    leds_init();  // Ahora inicializa los LEDs apagados
//
//    // Bucle principal
//    while (1) {
//        display_menu();
//
//        while (1) {
//            key = keypad_getkey();
//
//            if (key == 1 || key == 2 || key == 3) {
//                control_led(key);
//                break;
//            }
//
//            delayMs(100);
//        }
//    }
//}

/* ========= TIMER (PARTE 2) ========= */

#include <MKL25Z4.h>
#include <stdio.h>
#include <string.h>

// Definición de los pines para LCD
#define RS 0x04  // PTA2
#define RW 0x10  // PTA4
#define EN 0x20  // PTA5

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
    SIM->SCGC5 |= 0x1000;      // Habilitar Puerto D
    for (int i = 0; i < 8; i++)
        PORTD->PCR[i] = 0x100; // Configurar como GPIO
    PTD->PDDR = 0xFF;          // Configurar como salidas

    SIM->SCGC5 |= 0x0200;      // Habilitar Puerto A
    PORTA->PCR[2] = 0x100;     // Configurar como GPIO
    PORTA->PCR[4] = 0x100;
    PORTA->PCR[5] = 0x100;
    PTA->PDDR |= (RS | RW | EN); // Configurar como salidas

    delayMs(30);
    LCD_command(0x30); delayMs(10);
    LCD_command(0x30); delayMs(1);
    LCD_command(0x30);
    LCD_command(0x38);         // 8 bits, 2 líneas
    LCD_command(0x06);         // Modo entrada
    LCD_command(0x01);         // Limpiar pantalla
    LCD_command(0x0F);         // Encender display y cursor
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

void LCD_clear(void) {
    LCD_command(0x01);         // Limpiar pantalla
    delayMs(2);                // Esperar a que se complete
}

void LCD_setCursor(int row, int col) {
    int address;
    if (row == 0)
        address = 0x80 + col;  // Primera línea
    else
        address = 0xC0 + col;  // Segunda línea
    LCD_command(address);
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

        LCD_clear();
        LCD_setCursor(0, 0);
        LCD_print("TIME ELAPSED!");
        delayMs(200);

        // Restablecer contador para la próxima vez
        seconds_count = 0;
        timer_elapsed = 0;
    }
}

// Función principal para el temporizador ascendente
void run_timer(void) {
    int key;
    int entered_time = 0;
    int digits = 0;
    char time_str[16] = "";

    // 1. Mensaje de bienvenida y espera
    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_print("TIMER MODE");
    LCD_setCursor(1, 0);
    LCD_print("Please wait...");
    delayMs(500); // Esperar 5 segundos

    // 2. Solicitar tiempo al usuario
    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_print("Enter seconds:");
    LCD_setCursor(1, 0);

    // Bucle para capturar dígitos
    while (1) {
         key = keypad_getkey();

         if (key != -1) {  // Se presionó una tecla
             if (key >= 0 && key <= 9) {  // Dígito 0-9
                 if (digits == 0 && key == 0) {
                     // No permitir 0 como primer dígito
                 } else {
                     entered_time = entered_time * 10 + key;
                     sprintf(time_str, "%d", entered_time);
                     LCD_setCursor(1, 0);
                     LCD_print(time_str);
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

    // 4. Mostrar mensaje de conteo
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
}

// Main para la Parte 2
int main(void) {
    // Inicializar TPM0 para delayMs
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    TPM0->SC = 0;
    TPM0->SC = TPM_SC_PS(2);  // prescaler /4

    // Inicializar periféricos
    keypad_init();
    LCD_init();
    leds_init();
    timer_init(); // Inicializar TPM1 para interrupciones

    // Ejecutar el temporizador
    run_timer();

    // Bucle infinito
    while(1) {
        // Puedes añadir código para volver a ejecutar el timer si lo deseas
        delayMs(1000);
    }
}

#include <MKL25Z4.h>
#define RS 0x04  // PTA2
#define RW 0x10  // PTA4
#define EN 0x20  // PTA5

// Definición de pines para LEDs
#define LED_ORANGE    (1 << 8)   // PTB8 (bit 8)
#define LED_YELLOW   (1 << 9)   // PTB9 (bit 9)
#define LED_GREEN  (1 << 10)  // PTB10 (bit 10)

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

    // **Corrección clave:** Inicializar LEDs apagados (activo en alto)
    PTB->PCOR = (LED_ORANGE | LED_GREEN | LED_YELLOW);  // Pines en bajo (LEDs apagados)
}

void led_on(int led) {
    PTB->PSOR = led;  // Encender: Nivel ALTO
}
void led_off(int led) {
    PTB->PCOR = led;  // Apagar: Nivel BAJO
}

/* ========= FUNCIONES DE MENÚ Y CONTROL DE LEDS ========= */
void display_menu() {
    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_print("PRESS BUTTON");
    LCD_setCursor(1, 0);
    LCD_print("O: 1 Y: 2 G: 3");
}

void control_led(int led_choice) {
    LCD_clear();
    LCD_setCursor(0, 0);

    switch(led_choice) {
        case 1: // LED ROJO
            LCD_print("ORANGE LED IS ON!");
            led_on(LED_ORANGE);
            delayMs(1000);
            led_off(LED_ORANGE);
            break;

        case 2: // LED AZUL
            LCD_print("YELLOW LED IS ON!");
            led_on(LED_YELLOW);
            delayMs(1000);
            led_off(LED_YELLOW);
            break;

        case 3: // LED VERDE
            LCD_print("GREEN LED IS ON!");
            led_on(LED_GREEN);
            delayMs(1000);
            led_off(LED_GREEN);
            break;

        default:
            LCD_print("INVALID CHOICE");
            delayMs(1000);
            break;
    }
}

/* ========= MAIN ========= */
int main(void) {
    char key;

    // Inicializar TPM0 para delayMs
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    TPM0->SC = 0;
    TPM0->SC = TPM_SC_PS(2);  // prescaler /4

    // Inicializar periféricos
    keypad_init();
    LCD_init();
    leds_init();  // Ahora inicializa los LEDs apagados

    // Bucle principal
    while (1) {
        display_menu();

        while (1) {
            key = keypad_getkey();

            if (key == 1 || key == 2 || key == 3) {
                control_led(key);
                break;
            }

            delayMs(100);
        }
    }
}

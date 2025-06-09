//Program (11):  Functions declarations

/* This program scans a 4x4 matrix keypad and returns a unique number or each key pressed.
The number is displayed on the tri-color
 * LEDs using previous code
 * PortC 7-4 are connected to the columns and PortC 3-0 are connected to the rows. */

#include "hal_gpio.h"
#include <MKL25Z4.h>

// --- Configuration Defines ---

// Keypad Pins (Port C)
#define KEYPAD_ROW0_PIN     0
#define KEYPAD_ROW1_PIN     1
#define KEYPAD_ROW2_PIN     2
#define KEYPAD_ROW3_PIN     3
#define KEYPAD_COL0_PIN     4
#define KEYPAD_COL1_PIN     5
#define KEYPAD_COL2_PIN     6
#define KEYPAD_COL3_PIN     7

// Tri-Color LED Pins
#define LED_RED_PIN         18 // Port B
#define LED_GREEN_PIN       19 // Port B
#define LED_BLUE_PIN        1  // Port D

// PCR Configurations
#define GPIO_PCR_PULLUP_CFG 0x103 // GPIO, enable pullup (for keypad inputs)
#define GPIO_PCR_BASIC_CFG  0x100 // Basic GPIO configuration

// TPM0 Timer Configuration
#define TPM0_PRESCALER_VALUE        0x02 // Prescaler /4
#define TPM0_MODULO_VALUE           0x2000 // Max modulo value 8192

// --- GPIO Port Information (for HAL_GPIO) ---
static const MCAL_GPIO_Port_t PORTB_INFO = {PORTB, PTB, MCAL_PORTB_init};
static const MCAL_GPIO_Port_t PORTC_INFO = {PORTC, PTC, MCAL_PORTC_init};
static const MCAL_GPIO_Port_t PORTD_INFO = {PORTD, PTD, MCAL_PORTD_init};

int main(void) {
    // Initialize timer for delays
    HAL_Timer_init(TPM0_PRESCALER_VALUE, TPM0_MODULO_VALUE);

    // Initialize Keypad using HAL
    HAL_Keypad_init(&PORTC_INFO, KEYPAD_ROW0_PIN, KEYPAD_ROW1_PIN, KEYPAD_ROW2_PIN, KEYPAD_ROW3_PIN,
                    KEYPAD_COL0_PIN, KEYPAD_COL1_PIN, KEYPAD_COL2_PIN, KEYPAD_COL3_PIN,
                    GPIO_PCR_BASIC_CFG, GPIO_PCR_PULLUP_CFG);

    // Initialize LEDs using HAL
    HAL_LED_init(&PORTB_INFO, &PORTB_INFO, &PORTD_INFO, LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, GPIO_PCR_BASIC_CFG);

    uint8_t key_pressed;

    while (1) {
        // Get key using HAL function
        key_pressed = HAL_Keypad_getkey();
        // Set LEDs using HAL function
        HAL_LED_set(key_pressed);
    }
}

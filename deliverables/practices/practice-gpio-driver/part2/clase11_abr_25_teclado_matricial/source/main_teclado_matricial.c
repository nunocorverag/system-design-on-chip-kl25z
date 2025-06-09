
//Program (11):  Functions declarations

/* This program scans a 4x4 matrix keypad and returns a unique number or each key pressed.
The number is displayed on the tri-color
 * LEDs using previous code
 * PortC 7-4 are connected to the columns and PortC 3-0 are connected to the rows. */

#include "hal_gpio.h"
#include "mcal_gpio.h" // Include MCAL for timer, and the specific keypad/LED logic
#include <MKL25Z4.h> // Ensure this is included for peripheral definitions

// --- Configuration Defines (All moved here from mcal_config.h and mcal.c) ---

// Keypad Pins (Port C)
#define KEYPAD_ROW0_PIN     0
#define KEYPAD_ROW1_PIN     1
#define KEYPAD_ROW2_PIN     2
#define KEYPAD_ROW3_PIN     3
#define KEYPAD_COL0_PIN     4
#define KEYPAD_COL1_PIN     5
#define KEYPAD_COL2_PIN     6
#define KEYPAD_COL3_PIN     7

// Keypad operational masks/offsets
#define KEYPAD_ROWS_PDDR_MASK       0x0F // (1U << 0) | (1U << 1) | (1U << 2) | (1U << 3)
#define KEYPAD_COLS_PDIR_OFFSET     4    // Columns start at pin 4
#define KEYPAD_COLS_PDIR_NIBBLE_MASK 0x0F // Mask for the 4 column bits once shifted (0b1111)

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
    // Initialize timer for delays, passing configuration values
    MCAL_TPM0_init(TPM0_PRESCALER_VALUE, TPM0_MODULO_VALUE);

    // Initialize Keypad pins using HAL_GPIO
    // Rows (outputs when driven low, inputs when floating)
    HAL_GPIO_InitOutput(&PORTC_INFO, KEYPAD_ROW0_PIN, GPIO_PCR_BASIC_CFG);
    HAL_GPIO_InitOutput(&PORTC_INFO, KEYPAD_ROW1_PIN, GPIO_PCR_BASIC_CFG);
    HAL_GPIO_InitOutput(&PORTC_INFO, KEYPAD_ROW2_PIN, GPIO_PCR_BASIC_CFG);
    HAL_GPIO_InitOutput(&PORTC_INFO, KEYPAD_ROW3_PIN, GPIO_PCR_BASIC_CFG);
    // Columns (inputs with pull-ups)
    HAL_GPIO_InitInput(&PORTC_INFO, KEYPAD_COL0_PIN, GPIO_PCR_PULLUP_CFG);
    HAL_GPIO_InitInput(&PORTC_INFO, KEYPAD_COL1_PIN, GPIO_PCR_PULLUP_CFG);
    HAL_GPIO_InitInput(&PORTC_INFO, KEYPAD_COL2_PIN, GPIO_PCR_PULLUP_CFG);
    HAL_GPIO_InitInput(&PORTC_INFO, KEYPAD_COL3_PIN, GPIO_PCR_PULLUP_CFG);

    // Initialize LED pins using HAL_GPIO
    HAL_GPIO_InitOutput(&PORTB_INFO, LED_RED_PIN, GPIO_PCR_BASIC_CFG);
    HAL_GPIO_InitOutput(&PORTB_INFO, LED_GREEN_PIN, GPIO_PCR_BASIC_CFG);
    HAL_GPIO_InitOutput(&PORTD_INFO, LED_BLUE_PIN, GPIO_PCR_BASIC_CFG);

    uint8_t key_pressed;

    while (1) {
        // Get key from MCAL directly, passing keypad configuration
        key_pressed = MCAL_Keypad_getkey(KEYPAD_ROWS_PDDR_MASK, KEYPAD_COLS_PDIR_OFFSET, KEYPAD_COLS_PDIR_NIBBLE_MASK);
        // Set LEDs via MCAL directly, passing LED pin configuration
        MCAL_LED_set(key_pressed, LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN);
    }
}

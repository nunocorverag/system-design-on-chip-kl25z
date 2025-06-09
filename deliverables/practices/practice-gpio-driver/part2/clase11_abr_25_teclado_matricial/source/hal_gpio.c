/*
 * hal_gpio.c
 */

#include "hal_gpio.h"
#include "mcal_gpio.h" // Include MCAL to access low-level GPIO functions

// Static variables to store keypad and LED configuration
static const MCAL_GPIO_Port_t *keypad_port_info;
static uint32_t keypad_row_pins[4];
static uint32_t keypad_col_pins[4];
static const MCAL_GPIO_Port_t *led_red_port_info;
static const MCAL_GPIO_Port_t *led_green_port_info;
static const MCAL_GPIO_Port_t *led_blue_port_info;
static uint32_t led_red_pin, led_green_pin, led_blue_pin;

// --- HAL GPIO Basic Functions ---
void HAL_GPIO_InitInput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint32_t pcr_config) {
    if (port_info && port_info->mcal_init_func) {
        port_info->mcal_init_func(); // Call the specific MCAL port clock enable
        MCAL_GPIO_SetPinDirection(port_info->port_base, port_info->pt_base, pin, 0, pcr_config); // 0 for input
    }
}

void HAL_GPIO_InitOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint32_t pcr_config) {
    if (port_info && port_info->mcal_init_func) {
        port_info->mcal_init_func(); // Call the specific MCAL port clock enable
        MCAL_GPIO_SetPinDirection(port_info->port_base, port_info->pt_base, pin, 1, pcr_config); // 1 for output
    }
}

uint8_t HAL_GPIO_Read(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
    if (port_info) {
        return MCAL_GPIO_ReadPin(port_info->pt_base, pin);
    }
    return 0xFF; // Error value
}

void HAL_GPIO_Write(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint8_t value) {
    if (port_info) {
        MCAL_GPIO_WritePin(port_info->pt_base, pin, value);
    }
}

void HAL_GPIO_Toggle(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
    if (port_info) {
        MCAL_GPIO_TogglePin(port_info->pt_base, pin);
    }
}

// --- HAL Timer Functions ---
void HAL_Timer_init(uint32_t prescaler_value, uint32_t modulo_value) {
    MCAL_TPM0_init(prescaler_value, modulo_value);
}

// --- HAL Keypad Functions ---
void HAL_Keypad_init(const MCAL_GPIO_Port_t *port_info,
                     uint32_t row0_pin, uint32_t row1_pin, uint32_t row2_pin, uint32_t row3_pin,
                     uint32_t col0_pin, uint32_t col1_pin, uint32_t col2_pin, uint32_t col3_pin,
                     uint32_t row_pcr_config, uint32_t col_pcr_config) {

    // Store configuration
    keypad_port_info = port_info;
    keypad_row_pins[0] = row0_pin;
    keypad_row_pins[1] = row1_pin;
    keypad_row_pins[2] = row2_pin;
    keypad_row_pins[3] = row3_pin;
    keypad_col_pins[0] = col0_pin;
    keypad_col_pins[1] = col1_pin;
    keypad_col_pins[2] = col2_pin;
    keypad_col_pins[3] = col3_pin;

    // Initialize rows as outputs
    HAL_GPIO_InitOutput(port_info, row0_pin, row_pcr_config);
    HAL_GPIO_InitOutput(port_info, row1_pin, row_pcr_config);
    HAL_GPIO_InitOutput(port_info, row2_pin, row_pcr_config);
    HAL_GPIO_InitOutput(port_info, row3_pin, row_pcr_config);

    // Initialize columns as inputs with pull-ups
    HAL_GPIO_InitInput(port_info, col0_pin, col_pcr_config);
    HAL_GPIO_InitInput(port_info, col1_pin, col_pcr_config);
    HAL_GPIO_InitInput(port_info, col2_pin, col_pcr_config);
    HAL_GPIO_InitInput(port_info, col3_pin, col_pcr_config);
}

uint8_t HAL_Keypad_getkey(void) {
    int row, col;

    // Drive all rows low initially
    HAL_GPIO_Write(keypad_port_info, keypad_row_pins[0], 0);
    HAL_GPIO_Write(keypad_port_info, keypad_row_pins[1], 0);
    HAL_GPIO_Write(keypad_port_info, keypad_row_pins[2], 0);
    HAL_GPIO_Write(keypad_port_info, keypad_row_pins[3], 0);

    MCAL_TPM0_delayUs(2);

    // Check if any key is pressed by reading all columns
    col = 0;
    col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[0]) << 0);
    col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[1]) << 1);
    col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[2]) << 2);
    col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[3]) << 3);

    if (col == 0x0F) // If all columns are high (no key pressed)
        return 0;

    // Scan each row to find which key is pressed
    for (row = 0; row < 4; row++) {
        // Set all rows as inputs (high-Z)
        HAL_GPIO_InitInput(keypad_port_info, keypad_row_pins[0], 0x100);
        HAL_GPIO_InitInput(keypad_port_info, keypad_row_pins[1], 0x100);
        HAL_GPIO_InitInput(keypad_port_info, keypad_row_pins[2], 0x100);
        HAL_GPIO_InitInput(keypad_port_info, keypad_row_pins[3], 0x100);

        // Set current row as output and drive low
        HAL_GPIO_InitOutput(keypad_port_info, keypad_row_pins[row], 0x100);
        HAL_GPIO_Write(keypad_port_info, keypad_row_pins[row], 0);

        MCAL_TPM0_delayUs(2);

        // Read columns
        col = 0;
        col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[0]) << 0);
        col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[1]) << 1);
        col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[2]) << 2);
        col |= (HAL_GPIO_Read(keypad_port_info, keypad_col_pins[3]) << 3);

        if (col != 0x0F) break;
    }

    // Restore rows as outputs for next scan
    HAL_GPIO_InitOutput(keypad_port_info, keypad_row_pins[0], 0x100);
    HAL_GPIO_InitOutput(keypad_port_info, keypad_row_pins[1], 0x100);
    HAL_GPIO_InitOutput(keypad_port_info, keypad_row_pins[2], 0x100);
    HAL_GPIO_InitOutput(keypad_port_info, keypad_row_pins[3], 0x100);

    if (row == 4) return 0;

    // Decode which column was pressed
    if (col == 0x0E) return row * 4 + 1; // Col 0 (0x0E = 1110 binary)
    if (col == 0x0D) return row * 4 + 2; // Col 1 (0x0D = 1101 binary)
    if (col == 0x0B) return row * 4 + 3; // Col 2 (0x0B = 1011 binary)
    if (col == 0x07) return row * 4 + 4; // Col 3 (0x07 = 0111 binary)
    return 0;
}

// --- HAL LED Functions ---
void HAL_LED_init(const MCAL_GPIO_Port_t *red_port_info, const MCAL_GPIO_Port_t *green_port_info,
                  const MCAL_GPIO_Port_t *blue_port_info, uint32_t red_pin, uint32_t green_pin,
                  uint32_t blue_pin, uint32_t pcr_config) {

    // Store configuration
    led_red_port_info = red_port_info;
    led_green_port_info = green_port_info;
    led_blue_port_info = blue_port_info;
    led_red_pin = red_pin;
    led_green_pin = green_pin;
    led_blue_pin = blue_pin;

    // Initialize LED pins as outputs
    HAL_GPIO_InitOutput(red_port_info, red_pin, pcr_config);
    HAL_GPIO_InitOutput(green_port_info, green_pin, pcr_config);
    HAL_GPIO_InitOutput(blue_port_info, blue_pin, pcr_config);
}

void HAL_LED_set(int value) {
    // Red LED (active low)
    if (value & 1)
        HAL_GPIO_Write(led_red_port_info, led_red_pin, 0); // 0 to turn ON
    else
        HAL_GPIO_Write(led_red_port_info, led_red_pin, 1); // 1 to turn OFF

    // Green LED (active low)
    if (value & 2)
        HAL_GPIO_Write(led_green_port_info, led_green_pin, 0); // 0 to turn ON
    else
        HAL_GPIO_Write(led_green_port_info, led_green_pin, 1); // 1 to turn OFF

    // Blue LED (active low)
    if (value & 4)
        HAL_GPIO_Write(led_blue_port_info, led_blue_pin, 0); // 0 to turn ON
    else
        HAL_GPIO_Write(led_blue_port_info, led_blue_pin, 1); // 1 to turn OFF
}

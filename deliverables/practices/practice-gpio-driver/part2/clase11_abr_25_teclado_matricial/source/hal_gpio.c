/*
 * hal_gpio.c
 */

#include "hal_gpio.h"
#include "mcal_gpio.h" // Include MCAL to access low-level GPIO functions

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

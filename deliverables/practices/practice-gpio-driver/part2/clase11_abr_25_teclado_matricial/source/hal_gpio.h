/*
 * hal_gpio.h
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <MKL25Z4.h> // For PORT_Type, GPIO_Type
#include <stdint.h>  // For uint8_t

// Function pointer type for port initialization
typedef void (*MCAL_PortInitFunc_t)(void);

// Structure to define a GPIO port
typedef struct {
    PORT_Type *port_base;     // Base address of the PORT peripheral (e.g., PORTB)
    GPIO_Type *pt_base;       // Base address of the GPIO peripheral (e.g., PTB)
    MCAL_PortInitFunc_t mcal_init_func; // Pointer to the MCAL port initialization function
} MCAL_GPIO_Port_t;

// --- HAL GPIO API ---
void HAL_GPIO_InitInput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint32_t pcr_config);
void HAL_GPIO_InitOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint32_t pcr_config);
uint8_t HAL_GPIO_Read(const MCAL_GPIO_Port_t *port_info, uint32_t pin);
void HAL_GPIO_Write(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint8_t value);
void HAL_GPIO_Toggle(const MCAL_GPIO_Port_t *port_info, uint32_t pin);

#endif // HAL_GPIO_H

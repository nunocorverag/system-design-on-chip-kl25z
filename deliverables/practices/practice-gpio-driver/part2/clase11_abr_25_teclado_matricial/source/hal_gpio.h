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

// --- HAL GPIO Basic API ---
void HAL_GPIO_InitInput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint32_t pcr_config);
void HAL_GPIO_InitOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint32_t pcr_config);
uint8_t HAL_GPIO_Read(const MCAL_GPIO_Port_t *port_info, uint32_t pin);
void HAL_GPIO_Write(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint8_t value);
void HAL_GPIO_Toggle(const MCAL_GPIO_Port_t *port_info, uint32_t pin);

// --- HAL Timer API ---
void HAL_Timer_init(uint32_t prescaler_value, uint32_t modulo_value);

// --- HAL Keypad API ---
void HAL_Keypad_init(const MCAL_GPIO_Port_t *port_info,
                     uint32_t row0_pin, uint32_t row1_pin, uint32_t row2_pin, uint32_t row3_pin,
                     uint32_t col0_pin, uint32_t col1_pin, uint32_t col2_pin, uint32_t col3_pin,
                     uint32_t row_pcr_config, uint32_t col_pcr_config);
uint8_t HAL_Keypad_getkey(void);

// --- HAL LED API ---
void HAL_LED_init(const MCAL_GPIO_Port_t *red_port_info, const MCAL_GPIO_Port_t *green_port_info,
                  const MCAL_GPIO_Port_t *blue_port_info, uint32_t red_pin, uint32_t green_pin,
                  uint32_t blue_pin, uint32_t pcr_config);
void HAL_LED_set(int value);

// --- MCAL Function Declarations (from mcal_gpio.h) ---
void MCAL_PORTA_init(void);
void MCAL_PORTB_init(void);
void MCAL_PORTC_init(void);
void MCAL_PORTD_init(void);
void MCAL_PORTE_init(void);

#endif // HAL_GPIO_H

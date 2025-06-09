#include "hal_gpio.h"
#include <MKL25Z4.h>

// Configuracion de pin
#define BUTTON_PIN 3
#define LED_PIN    18          // PTB18 para rojo, PTB19 para verde, PTD1 para azul (cambiar ptb a ptd)


static const MCAL_GPIO_Port_t PORTB_INFO = {PORTB, PTB, MCAL_PORTB};


#define LED_ON  1
#define LED_OFF 0

int main(void) {
    // Inicializar el botón como entrada
	HAL_GPIO_InitInput(&PORTB_INFO, BUTTON_PIN);

    // Inicializar el LED como salida
    HAL_GPIO_InitOutput(&PORTB_INFO, LED_PIN);

    while (1) {
        // Leer estado del botón
        uint8_t button_state = HAL_GPIO_Read(&PORTB_INFO, BUTTON_PIN);


        if (button_state == 0) {
            HAL_GPIO_Write(&PORTB_INFO, LED_PIN, LED_ON);
        } else {
            HAL_GPIO_Write(&PORTB_INFO, LED_PIN, LED_OFF);
        }
    }
}

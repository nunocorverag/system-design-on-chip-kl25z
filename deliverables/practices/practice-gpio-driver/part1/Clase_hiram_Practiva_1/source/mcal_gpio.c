#include "mcal_gpio.h"

void MCAL_GPIO_EnableClock(const MCAL_GPIO_Port_t *port_info){
	SIM->SCG5|= port_info->clk_mask;
}

void MCAL_GPIO_ConfigOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin){
	port_info->port->PCR[pin] = PORT_PCR_MUX(1);
}



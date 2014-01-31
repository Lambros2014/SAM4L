/*
 * gpio.c
 *
 * Created: 
 *  Author: kjetibk
 */ 

#include <stdbool.h>

#include "sam4lc4c.h"
#include "gpio.h"

void gpio_set_mux(uint32_t gpio, uint32_t pin, uint32_t mux){

	if ( mux & (1 << 0) ) { 
		GPIO->GPIO_PORT[pin/32].GPIO_PMR0S = gpio;
	} else {
		GPIO->GPIO_PORT[pin/32].GPIO_PMR0C = gpio;
	}
	
	if ( mux & (1 << 1) ) {
		GPIO->GPIO_PORT[pin/32].GPIO_PMR1S = gpio;
		} else {
		GPIO->GPIO_PORT[pin/32].GPIO_PMR1C = gpio;
	}
	
	if ( mux & (1 << 2) ) {
		GPIO->GPIO_PORT[pin/32].GPIO_PMR2S = gpio;
		} else {
		GPIO->GPIO_PORT[pin/32].GPIO_PMR2C = gpio;
	}
	
}

void gpio_disable_register(uint32_t gpio, uint32_t pin)
{
	GPIO->GPIO_PORT[pin/32].GPIO_GPERC = gpio;
}

void gpio_set_input(uint32_t gpio, uint32_t pin){
	
	GPIO->GPIO_PORT[pin/32].GPIO_GPERS = gpio; //Enable GPIO
	GPIO->GPIO_PORT[pin/32].GPIO_STERS = gpio; //Enable schmitt trigger
	GPIO->GPIO_PORT[pin/32].GPIO_ODERC = gpio; //Disable Output Driver
	
}

void gpio_set_output(uint32_t gpio, uint32_t pin){
	
	GPIO->GPIO_PORT[pin/32].GPIO_GPERS = gpio; //Enable GPIO
	GPIO->GPIO_PORT[pin/32].GPIO_STERC = gpio; //Disable schmitt trigger
	GPIO->GPIO_PORT[pin/32].GPIO_ODERS = gpio; //Enable Output Driver
	
}

void gpio_set_high(uint32_t gpio, uint32_t pin)
{
	GPIO->GPIO_PORT[pin/32].GPIO_OVRS  = gpio;
}

void gpio_set_low(uint32_t gpio, uint32_t pin)
{
	GPIO->GPIO_PORT[pin/32].GPIO_OVRC  = gpio;
}

void gpio_toggle(uint32_t gpio, uint32_t pin)
{
	GPIO->GPIO_PORT[pin/32].GPIO_OVRT  = gpio;
}

void gpio_set_level(uint32_t gpio, uint32_t pin, bool level)
{
	if(level)
		gpio_set_high(gpio, pin);
	else
		gpio_set_low(gpio, pin);
}

bool gpio_get_output(uint32_t gpio, uint32_t pin)
{
	return ((GPIO->GPIO_PORT[pin/32].GPIO_PVR & gpio) == 0);
}
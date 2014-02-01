/*
 * gpio.h
 *
 * Created: 29.10.2012 11:22:48
 *  Author: Are.Halvorsen
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#include "io1_xpro.h"

#define MUX_PERIPHERAL_A 0
#define MUX_PERIPHERAL_B 1
#define MUX_PERIPHERAL_C 2
#define MUX_PERIPHERAL_D 3
#define MUX_PERIPHERAL_E 4
#define MUX_PERIPHERAL_F 5
#define MUX_PERIPHERAL_G 6
#define MUX_PERIPHERAL_H 7

void gpio_set_mux(uint32_t gpio, uint32_t pin, uint32_t mux);
void gpio_set_input(uint32_t gpio, uint32_t pin);
void gpio_set_output(uint32_t gpio, uint32_t pin);

uint8_t gpio_read_button(uint8_t number);


#endif /* GPIO_H_ */
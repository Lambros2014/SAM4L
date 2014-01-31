/*
 * gpio.h
 *
 * Created: 
 *  Author: kjetibk
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>

void gpio_set_mux(uint32_t gpio, uint32_t pin, uint32_t mux);
void gpio_set_input(uint32_t gpio, uint32_t pin);
void gpio_set_output(uint32_t gpio, uint32_t pin);

void gpio_set_high(uint32_t gpio, uint32_t pin);
void gpio_set_low(uint32_t gpio, uint32_t pin);
void gpio_toggle(uint32_t gpio, uint32_t pin);
bool gpio_get_output(uint32_t gpio, uint32_t pin);
void gpio_set_level(uint32_t gpio, uint32_t pin, bool level);
void gpio_disable_register(uint32_t gpio, uint32_t pin);


#endif /* GPIO_H_ */
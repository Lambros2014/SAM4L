/*
 * adc.c
 *
 * Created: 06.11.2012 14:07:24
 *  Author: Are.Halvorsen
 */ 

#include <asf.h>
#include "adc.h"
#include "io1_xpro.h"
#include "gpio.h"

void my_adc_init (void)
{
	printf("1");
	// Give pins to adc module.
	//GPIO->GPIO_PORT[IO1_LIGHT_SENSOR_PIN/32].GPIO_GPERC = IO1_LIGHT_SENSOR_GPIO;
	//gpio_set_mux(IO1_LIGHT_SENSOR_GPIO, IO1_LIGHT_SENSOR_PIN, MUX_PERIPHERAL_A);	//AD2
	
	//GPIO->GPIO_PORT[IO1_LP_OUT_PIN/32].GPIO_GPERC = IO1_LP_OUT_GPIO;
	//gpio_set_mux(IO1_LP_OUT_GPIO, IO1_LP_OUT_PIN, MUX_PERIPHERAL_A);				//AD3
	
	printf("2");
	
	// Start ADCIF clock.
	PM->PM_UNLOCK = (0xAA<<24)|0x28;
	PM->PM_PBAMASK |= PM_PBAMASK_ADCIFE;
	
	// Set up generic clock 10 from OSC0 (12MHz)
	SCIF->SCIF_GCCTRL[10].SCIF_GCCTRL = SCIF_GCCTRL_OSCSEL(3);
	SCIF->SCIF_GCCTRL[10].SCIF_GCCTRL |= SCIF_GCCTRL_CEN;
	
	printf("3");
	
	ADCIFE->ADCIFE_TIM |= ADCIFE_TIM_ENSTUP | ADCIFE_TIM_STARTUP(20);
	
	printf("4");
	do {
		ADCIFE->ADCIFE_CR |= ADCIFE_CR_EN;
	} while((ADCIFE->ADCIFE_SR & ADCIFE_SR_EN) == 0);
	
	//ADCIFE->ADCIFE_CR |= ADCIFE_CR_EN;
	//while((ADCIFE->ADCIFE_SR & ADCIFE_SR_EN) == 0);
	
	ADCIFE->ADCIFE_CR |= ADCIFE_CR_BGREQEN | ADCIFE_CR_REFBUFEN;
	
	printf("5");
	while((ADCIFE->ADCIFE_SR & ADCIFE_SR_EN) == 0);
	
	
	printf("6");
	ADCIFE->ADCIFE_CFG |= ADCIFE_CFG_PRESCAL(0b011); // Div 16 = 375KHz.
	ADCIFE->ADCIFE_CFG |= ADCIFE_CFG_SPEED(0b11); // max speed 75 ksps
	ADCIFE->ADCIFE_CFG |= ADCIFE_CFG_REFSEL(0b001); // 0.625*VCC REF
	
	ADCIFE->ADCIFE_SEQCFG &= ~ADCIFE_SEQCFG_RES; //12-bit mode
	ADCIFE->ADCIFE_SEQCFG |= ADCIFE_SEQCFG_TRGSEL(0b000); //Software triggered conversion
	ADCIFE->ADCIFE_SEQCFG &= ~ADCIFE_SEQCFG_BIPOLAR; //Single ended mode
	
	ADCIFE->ADCIFE_SEQCFG |= ADCIFE_SEQCFG_INTERNAL(0b10);
	ADCIFE->ADCIFE_SEQCFG |= ADCIFE_SEQCFG_MUXNEG(0b111); //Negative input to GND pad
	printf("7\n");
	
	
}

uint32_t my_adc_read(uint8_t channel){

	volatile uint32_t reading;

	ADCIFE->ADCIFE_SEQCFG &= ~ADCIFE_SEQCFG_MUXPOS_Msk; // Clear positive mux source
	ADCIFE->ADCIFE_SEQCFG |= ADCIFE_SEQCFG_MUXPOS(0b1111 & channel); // Select new positive source.
	
	ADCIFE->ADCIFE_CR |= ADCIFE_CR_STRIG; // Trigger conversion
	
	while((ADCIFE->ADCIFE_SR & ADCIFE_SR_SEOC) == 0);
	
	reading = (ADCIFE->ADCIFE_LCV & ADCIFE_LCV_LCV_Msk);
	
	ADCIFE->ADCIFE_SCR |= ADCIFE_SCR_SEOC;
	
	return reading;
}
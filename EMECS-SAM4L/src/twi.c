/*
 * TWI.c
 *
 * Created: 06.11.2012 19:19:52
 *  Author: Are.Halvorsen
 */ 
#include <asf.h>
#include "conf_board.h"

#include "sam.h"
#include "io1_xpro.h"
#include "gpio.h"
#include "twi.h"

void twi_high_speed_transfer(){
	
	TWIM3->TWIM_CMDR |= TWIM_CMDR_HSMCODE(0b101);
	TWIM3->TWIM_CMDR |= TWIM_CMDR_HS;
	TWIM3->TWIM_NCMDR |= TWIM_CMDR_HSMCODE(0b101);
	TWIM3->TWIM_NCMDR |= TWIM_CMDR_HS;
	
}

void twi_init(){
	
	//TWIMS0
		
	//Set Peripheral mux to TWIM0
	//gpio_set_mux(IO1_TWI_SCL_GPIO, IO1_TWI_SCL_PIN, MUX_PERIPHERAL_B);
	//gpio_set_mux(IO1_TWI_SDA_GPIO, IO1_TWI_SDA_PIN, MUX_PERIPHERAL_B);

	//Disable GPIO, give control to modules
	GPIO->GPIO_PORT[IO1_TWI_SCL_PIN/32].GPIO_GPERC = IO1_TWI_SCL_GPIO;
	GPIO->GPIO_PORT[IO1_TWI_SDA_PIN/32].GPIO_GPERC = IO1_TWI_SDA_GPIO;
	
	//Set Peripheral mux to TWIM0
	gpio_set_mux(GPIO_PB14, PIN_PB14, MUX_PERIPHERAL_C);
	gpio_set_mux(GPIO_PB15, PIN_PB15, MUX_PERIPHERAL_C);

	//Disable GPIO, give control to modules
	//GPIO->GPIO_PORT[PIN_PB14/32].GPIO_GPERC = GPIO_PB14;
	//GPIO->GPIO_PORT[PIN_PB15/32].GPIO_GPERC = GPIO_PB15;
	
	// Enable TWI clock.
	PM->PM_UNLOCK = PM_UNLOCK_KEY(0xAA) | PM_UNLOCK_ADDR(0x028);
	PM->PM_PBAMASK |= PM_PBAMASK_TWIM3;
	
	// Clock magic
	TWIM3->TWIM_CWGR &= ~TWIM_CWGR_EXP_Msk; // EXP = 0, fprescale = 6MHz
	TWIM3->TWIM_CWGR |= TWIM_CWGR_DATA(12);
	TWIM3->TWIM_CWGR |= TWIM_CWGR_STASTO(24);
	TWIM3->TWIM_CWGR |= TWIM_CWGR_HIGH(113);
	TWIM3->TWIM_CWGR |= TWIM_CWGR_LOW(93);
	
	// High speed clock magic
	TWIM3->TWIM_HSCWGR &= ~TWIM_HSCWGR_EXP_Msk;
	TWIM3->TWIM_HSCWGR |= TWIM_HSCWGR_DATA(1);
	TWIM3->TWIM_HSCWGR |= TWIM_HSCWGR_STASTO(1);
	TWIM3->TWIM_HSCWGR |= TWIM_HSCWGR_HIGH(3);
	TWIM3->TWIM_HSCWGR |= TWIM_HSCWGR_LOW(3);
	
	
	// total low time = tsetup + thold + tlow = 1.0µS + 1.0µS + 3.0µS = 5.0µS
	// total period time = 10.0µS => 100 KHz!
	
	TWIM3->TWIM_CR = TWIM_CR_MEN; // Master enable
	
}

void twi_write_data(twi_data transfer){
	
	uint8_t i = 0;
	
	// Wait for master to be inactive
	while((TWIM3->TWIM_SR & TWIM_SR_CRDY) == 0);
	
	TWIM3->TWIM_CMDR = 0;
	
	twi_high_speed_transfer();
	
	// clear read bit (set write)
	TWIM3->TWIM_CMDR &= ~TWIM_CMDR_READ;
	
	// Configure address and length
	TWIM3->TWIM_CMDR |= TWIM_CMDR_SADR(transfer.address) | TWIM_CMDR_NBYTES(transfer.length) | TWIM_CMDR_START | TWIM_CMDR_STOP;
	
	//Tell the twi module that the command is valid for use.
	TWIM3->TWIM_CMDR |= TWIM_CMDR_VALID;
	
	// Wait for command to be done
	while((TWIM3->TWIM_SR & TWIM_SR_IDLE) == 0){
		if(((TWIM3->TWIM_SR & TWIM_SR_TXRDY) != 0) & (i < transfer.length) ){
			TWIM3->TWIM_THR = transfer.data[i];
			i++;
		}
	}
	
	TWIM3->TWIM_SCR |= TWIM_SCR_CCOMP;
	
}

void twi_read_data(twi_data transfer){
	
	// Wait until TWI module is ready for a new command
	while((TWIM3->TWIM_SR & TWIM_SR_CRDY) == 0);
	
	TWIM3->TWIM_CMDR = 0;
	
	twi_high_speed_transfer();
	
	// Set read bit
	TWIM3->TWIM_CMDR |= TWIM_CMDR_READ;
	
	// Configure address and length
	TWIM3->TWIM_CMDR |= TWIM_CMDR_SADR(transfer.address) | TWIM_CMDR_NBYTES(transfer.length) | TWIM_CMDR_START | TWIM_CMDR_STOP;

	uint8_t active = 1;
	uint8_t i = 0;
	
	//Tell the twi module that the command is valid for use.
	TWIM3->TWIM_CMDR |= TWIM_CMDR_VALID;

	while(active){
		if(TWIM3->TWIM_SR & TWIM_SR_CCOMP){
			active = 0;
		}

		if(TWIM3->TWIM_SR & TWIM_SR_ARBLST){
			//slave unlock procedure!
			active = 0;
			
			TWIM3->TWIM_SCR |= TWIM_SCR_ARBLST;
		}
		
		if(active){
			if((TWIM3->TWIM_SR & TWIM_SR_RXRDY) & (i < transfer.length) ){
				transfer.data[i] = TWIM3->TWIM_RHR;
				i++;
			}
		}	
	}
	
	
	TWIM3->TWIM_SCR |= TWIM_SCR_CCOMP;
	
}

void twi_write_then_read_data(twi_data write_transfer, twi_data read_transfer){
	
	// Wait until TWI module is ready for a new command
	while((TWIM3->TWIM_SR & TWIM_SR_CRDY) == 0);
	
	TWIM3->TWIM_CMDR = 0;
	TWIM3->TWIM_NCMDR = 0;
	
	twi_high_speed_transfer();
	
	// Set up write command
	
	// clear read bit (set write)
	TWIM3->TWIM_CMDR &= ~TWIM_CMDR_READ;
	
	// Configure address and length
	TWIM3->TWIM_CMDR |= TWIM_CMDR_SADR(write_transfer.address) | TWIM_CMDR_NBYTES(write_transfer.length) | TWIM_CMDR_START;
	
	// Set up read command
	
	// Set read bit
	TWIM3->TWIM_NCMDR = TWIM_CMDR_READ;
		
	// Configure address and length
	TWIM3->TWIM_NCMDR |= TWIM_CMDR_SADR(read_transfer.address) | TWIM_CMDR_NBYTES(read_transfer.length) | TWIM_CMDR_START | TWIM_CMDR_STOP;
	
	
	//Tell the twi module that the write command is valid for use.
	TWIM3->TWIM_CMDR |= TWIM_CMDR_VALID;
	
	//Tell the twi module that the read command is valid for use.
	TWIM3->TWIM_NCMDR |= TWIM_CMDR_VALID;
	
	uint8_t i = 0;
	uint8_t j = 0;
	
	// Wait for command to be done
	while((TWIM3->TWIM_SR & TWIM_SR_IDLE) == 0){
		if(((TWIM3->TWIM_SR & TWIM_SR_TXRDY) != 0) & (i < write_transfer.length) ){
			TWIM3->TWIM_THR = write_transfer.data[i];
			i++;
		}
		
		if(((TWIM3->TWIM_SR & TWIM_SR_RXRDY) != 0) & (j < read_transfer.length) ){
			read_transfer.data[j] = TWIM3->TWIM_RHR;
			j++;
		}
	}
	
	TWIM3->TWIM_SCR |= TWIM_SCR_CCOMP;
}

/* This function works, I just didn't need it.

void twi_write_then_write_data(twi_data write_transfer1, twi_data write_transfer2){
	
	// Wait until TWI module is ready for a new command
	while((TWIM3->TWIM_SR & TWIM_SR_CRDY) == 0);
		
	// Set up write command
		
	// clear read bit (set write)
	TWIM3->TWIM_CMDR = 0;
		
	// Configure address and length
	TWIM3->TWIM_CMDR |= TWIM_CMDR_SADR(write_transfer1.address) | TWIM_CMDR_NBYTES(write_transfer1.length) | TWIM_CMDR_START;
		
	// Set up read command
		
	// clear read bit (set write)
	TWIM3->TWIM_NCMDR = 0;
		
	// Configure address and length
	TWIM3->TWIM_NCMDR |= TWIM_CMDR_SADR(write_transfer2.address) | TWIM_CMDR_NBYTES(write_transfer2.length) | TWIM_CMDR_START | TWIM_CMDR_STOP;
		
	//Tell the twi module that the write command is valid for use.
	TWIM3->TWIM_CMDR |= TWIM_CMDR_VALID;
		
	//Tell the twi module that the read command is valid for use.
	TWIM3->TWIM_NCMDR |= TWIM_CMDR_VALID;
		
	uint8_t i = 0;
	uint8_t j = 0;
		
	// Wait for command to be done
	while((TWIM3->TWIM_SR & TWIM_SR_IDLE) == 0){
		if (((TWIM3->TWIM_SR & TWIM_SR_TXRDY) != 0) & (i < write_transfer1.length) ){
			TWIM3->TWIM_THR = write_transfer1.data[i];
			i++;
		} else if (((TWIM3->TWIM_SR & TWIM_SR_TXRDY) != 0) & (j < write_transfer2.length) ){
			TWIM3->TWIM_THR = write_transfer2.data[j];
			j++;
		}
	}
		
	TWIM3->TWIM_SCR |= TWIM_SCR_CCOMP;
	
}

*/

void twi_read_then_read_data(twi_data transfer1, twi_data transfer2);

void twi_read_then_write_data(twi_data transfer1, twi_data transfer2);


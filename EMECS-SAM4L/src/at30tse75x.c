/*
 * at30tse75x.c
 *
 * Created: 07.11.2012 13:12:31
 *  Author: Are.Halvorsen
 */ 
#include <asf.h>
#include "conf_board.h"

#include "at30tse75x.h"
#include "twi.h"
#include "sam.h"


void at30tse_eeprom_write(uint8_t *data, uint8_t length, uint8_t word_addr, uint8_t page){
	
	// ACK polling for consecutive writing not implemented
	
	uint8_t buffer[17];
	buffer[0] = word_addr & 0x0F;	 // Byte addr in page (0-15)
	buffer[0] |= (0x0F & page) << 4; // 4 lower bits of page addr in EEPROM
	
	// Copy data to be sent
	for (uint8_t i=1; i<17; i++){
		buffer[i] = data[i-1];
	}

	// Set up TWI transfer

	twi_data transfer = {
		.data = buffer,
		.length = length+1,
		.address = AT30TSE758_EEPROM_TWI_ADDR | ( (0x30 & page) >> 4 ), //TWI addr + 2 upper bytes of page addr.
	};

	asm("nop");
	// Do the transfer
	twi_write_data(transfer);

};

void at30tse_eeprom_read(uint8_t *data, uint8_t length, uint8_t word_addr, uint8_t page){

	// ACK polling for consecutive reading not implemented!

	uint8_t buffer[1];
	buffer[0] = word_addr & 0x0F;	 // Byte addr in page (0-15)
	buffer[0] |= (0x0F & page) << 4; // 4 lower bits of page addr in EEPROM

	// Set up internal EEPROM addr write
	twi_data addr_transfer = {
		.data = buffer,
		.length = 1,
		.address = AT30TSE758_EEPROM_TWI_ADDR | ( (0x30 & page) >> 4 ), //TWI addr + 2 upper bytes of page addr.
	};
	
	// Reading sequence
	twi_data read_transfer = {
		.data = data,
		.length = length,
		.address = AT30TSE758_EEPROM_TWI_ADDR | ( (0x30 & page) >> 4 ), //TWI addr + 2 upper bytes of page addr.
	};

	asm("nop");
	// Do the transfer
	twi_write_then_read_data(addr_transfer, read_transfer);

};

volatile uint8_t resolution = AT30TSE_CONFIG_RES_9_bit;

void at30tse_set_register_pointer(uint8_t reg, uint8_t reg_type){

	uint8_t buffer[] = {reg | reg_type};
	
	twi_data transfer = {
		.data = buffer,
		.length = 1,
		.address = AT30TSE_TEMPERATURE_TWI_ADDR,
	};

	twi_write_data(transfer);

};

uint16_t at30tse_read_register(uint8_t reg, uint8_t reg_type, uint8_t reg_size){
	uint8_t buffer[2];
	buffer[0] = reg | reg_type;
	buffer[1] = 0;
	
	// Internal register pointer in AT30TSE
	twi_data write_transfer = {
		.data = buffer,
		.length = 1,
		.address = AT30TSE_TEMPERATURE_TWI_ADDR,
	};
	
	// Read data
	twi_data read_transfer = {
		.data = buffer,
		.length = reg_size,
		.address = AT30TSE_TEMPERATURE_TWI_ADDR,
	};
	
	twi_write_then_read_data(write_transfer, read_transfer);
	
	return (buffer[0] << 8) | buffer[1];
}

void at30tse_write_register(uint8_t reg, uint8_t reg_type, uint8_t reg_size, uint16_t reg_value){

	uint8_t data[3];
	data[0] = reg | reg_type;
	data[1] = 0x00FF & (reg_value >> 8);
	data[2] = 0x00FF & reg_value;
	
	// Internal register pointer in AT30TSE
	twi_data transfer = {
		.data = data,
		.length = 1 + reg_size,
		.address = AT30TSE_TEMPERATURE_TWI_ADDR,
	};
	
	twi_write_data(transfer);
	
}

void at30tse_write_config_register(uint16_t value){

	at30tse_write_register(AT30TSE_CONFIG_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_CONFIG_REG_SIZE-1, value);

	resolution = ( value >> AT30TSE_CONFIG_RES_Pos ) & ( AT30TSE_CONFIG_RES_Msk >> AT30TSE_CONFIG_RES_Pos);
	
}

double at30tse_read_temperature(){
	// Read the 16-bit temperature register.
	uint16_t data = at30tse_read_register(AT30TSE_TEMPERATURE_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_TEMPERATURE_REG_SIZE);
	
	double temperature = 0;
	int8_t sign = 1; 
	
	//Check if negative and clear sign bit.
	if (data & (1 << 15)){
		sign *= -1;
		data &= ~(1 << 15);
	}
	
	// Convert to temperature 
	switch (resolution){
		case AT30TSE_CONFIG_RES_9_bit:
			data = (data >> 7);
			temperature = data * sign * 0.5;
			break;
		case AT30TSE_CONFIG_RES_10_bit:
			data = (data >> 6);
			temperature = data * sign * 0.25;
			break;
		case AT30TSE_CONFIG_RES_11_bit:
			data = (data >> 5);
			temperature = data * sign * 0.125;
			break;
		case AT30TSE_CONFIG_RES_12_bit: 
			data = (data >> 4);
			temperature = data * sign * 0.0625;
			break;
		default:
			break;
	}
	return temperature;
}
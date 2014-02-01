/*
 * at30tse75x.h
 *
 * Created: 07.11.2012 12:32:21
 *  Author: Are.Halvorsen
 */ 

#include "sam.h"

#ifndef AT30TSE75X_H_
#define AT30TSE75X_H_

#define AT30TSE_TEMPERATURE_TWI_ADDR	0b1001111
#define AT30TSE758_EEPROM_TWI_ADDR		0b1010100
#define AT30TSE754_EEPROM_TWI_ADDR		0b1010110
#define AT30TSE752_EEPROM_TWI_ADDR		0b1010111

#define AT30TSE_TEMPERATURE_REG			0x00
#define AT30TSE_TEMPERATURE_REG_SIZE	2
#define AT30TSE_NON_VOLATILE_REG		0x00
#define AT30TSE_VOLATILE_REG			0x10

#define AT30TSE_CONFIG_REG				0x01
#define AT30TSE_CONFIG_REG_SIZE			2
#define AT30TSE_TLOW_REG				0x02
#define AT30TSE_TLOW_REG_SIZE			2
#define AT30TSE_THIGH_REG				0x03
#define AT30TSE_THIGH_REG_SIZE			2

#define AT30TSE_CONFIG_RES_Pos 13
#define AT30TSE_CONFIG_RES_Msk (0x03 << AT30TSE_CONFIG_RES_Pos)
#define AT30TSE_CONFIG_RES(value) ((AT30TSE_CONFIG_RES_Msk & ((value) << AT30TSE_CONFIG_RES_Pos)))

#define AT30TSE_CONFIG_RES_9_bit	0
#define AT30TSE_CONFIG_RES_10_bit	1
#define AT30TSE_CONFIG_RES_11_bit	2
#define AT30TSE_CONFIG_RES_12_bit	3

#define AT30TSE_CONFIG_FTQ_Pos 13
#define AT30TSE_CONFIG_FTQ_Msk (0x03 << AT30TSE_CONFIG_FTQ_Pos)
#define AT30TSE_CONFIG_FTQ(value) ((AT30TSE_CONFIG_FTQ_Msk & ((value) << AT30TSE_CONFIG_FTQ_Pos)))

#define AT30TSE_CONFIG_FTQ_1_fault	0
#define AT30TSE_CONFIG_RES_2_fault	1
#define AT30TSE_CONFIG_RES_4_fault	2
#define AT30TSE_CONFIG_RES_6_fault	3

// R/W bits
#define AT30TSE_CONFIG_OS			(1 << 15)
#define AT30TSE_CONFIG_R1			(1 << 14)
#define AT30TSE_CONFIG_R0			(1 << 13)
#define AT30TSE_CONFIG_FT1			(1 << 12)
#define AT30TSE_CONFIG_FT0			(1 << 11)
#define AT30TSE_CONFIG_POL			(1 << 10)
#define AT30TSE_CONFIG_CMP_INT		(1 << 9)
#define AT30TSE_CONFIG_SD			(1 << 8)

// Read only bits
#define AT30TSE_CONFIG_NVRBSY		(1 << 0)

void at30tse_eeprom_write(uint8_t *data, uint8_t length, uint8_t word_addr, uint8_t page);
void at30tse_eeprom_read(uint8_t *data, uint8_t length, uint8_t word_addr, uint8_t page);
double at30tse_read_temperature();
void at30tse_write_config_register(uint16_t value);
uint16_t at30tse_read_register(uint8_t reg, uint8_t reg_type, uint8_t reg_size);
void at30tse_write_register(uint8_t reg, uint8_t reg_type, uint8_t reg_size, uint16_t reg_value);
void at30tse_set_register_pointer(uint8_t reg, uint8_t reg_type);

#endif /* AT30TSE75X_H_ */
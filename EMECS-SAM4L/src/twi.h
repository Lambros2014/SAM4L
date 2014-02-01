/*
 * twi.h
 *
 * Created: 07.11.2012 12:25:03
 *  Author: Are.Halvorsen
 */ 


#ifndef TWI_H_
#define TWI_H_

typedef struct {
	uint8_t *data;
	uint8_t length;
	uint8_t address;
} twi_data;

void twi_init();

void twi_write_data(twi_data transfer);
void twi_read_data(twi_data transfer);

void twi_write_then_write_data(twi_data transfer1, twi_data transfer2);
void twi_read_then_read_data(twi_data transfer1, twi_data transfer2);
void twi_write_then_read_data(twi_data transfer1, twi_data transfer2);
void twi_read_then_write_data(twi_data transfer1, twi_data transfer2);

#endif /* TWI_H_ */
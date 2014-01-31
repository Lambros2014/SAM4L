/*
 * spi.h
 *
 * Created: 01.02.2014 03:12:56
 *  Author: kjetibk
 */ 

#ifndef SPI_H_
#define SPI_H_

#define EXT1_GPIO_SPI_SS_1         GPIO_PB13
#define EXT1_GPIO_SPI_SS_0         GPIO_PC03
#define EXT1_GPIO_SPI_MOSI         GPIO_PA22
#define EXT1_GPIO_SPI_MISO         GPIO_PA21
#define EXT1_GPIO_SPI_SCK          GPIO_PC30

void spi_init (void);
void spi_send_byte (uint8_t data);
void spi_ss_disable (void);
void spi_ss_enable (void);

#endif /* SPI_H_ */
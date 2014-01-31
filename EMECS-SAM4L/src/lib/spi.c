/*
 * spi.c
 *
 * Created: 01.02.2014 03:12:39
 *  Author: kjetibk
 */ 

#include <asf.h>
#include "gpio.h"
#include <stdint.h>
#include "spi.h"

void spi_ss_enable (void)
{
	gpio_set_low (EXT1_GPIO_SPI_SS_0, EXT1_PIN_SPI_SS_0);
}

void spi_ss_disable (void)
{
	gpio_set_high (EXT1_GPIO_SPI_SS_0, EXT1_PIN_SPI_SS_0);
}

void spi_send_byte (uint8_t data)
{	
	SPI->SPI_TDR |= data;
	while (!(SPI->SPI_SR & SPI_SR_TXEMPTY));
}

void spi_init (void)
{
	gpio_disable_register (EXT1_GPIO_SPI_MOSI, EXT1_PIN_SPI_MOSI); // usikker på SPI_MOSI_GPIO
	gpio_disable_register (EXT1_GPIO_SPI_SCK, EXT1_PIN_SPI_SCK); // usikker på SPI_SPCK_GPIO
	
	gpio_set_mux (EXT1_GPIO_SPI_MOSI, EXT1_PIN_SPI_MOSI, EXT1_SPI_SS_MOSI);
	gpio_set_mux (EXT1_GPIO_SPI_SCK, EXT1_PIN_SPI_SCK, EXT1_SPI_SS_SCK);
	
	// CS pin as output
	gpio_set_output (EXT1_GPIO_SPI_SS_0, EXT1_PIN_SPI_SS_0);
	gpio_set_high (EXT1_GPIO_SPI_SS_0, EXT1_PIN_SPI_SS_0);
	
	// Enable SPI clock.
	PM->PM_UNLOCK = PM_UNLOCK_KEY(0xAA) | PM_UNLOCK_ADDR(0x028);
	PM->PM_PBAMASK |= PM_PBAMASK_SPI;
	
	// Set up SPI mode
	SPI->SPI_MR |= SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_MR_PCS(0b1011);
	
	// Configure NPCS2
	SPI->SPI_CSR[2] |= SPI_CSR_SCBR(2) | SPI_CSR_DLYBCT(1) | SPI_CSR_CPOL_0 | SPI_CSR_NCPHA_1;
	
	//Enable the SPI module.
	SPI->SPI_CR |= SPI_CR_SPIEN;
}

/*
 * tempsens.c
 *
 * Created: 01.02.2014 14:17:42
 *  Author: magnealv
 */

#include <asf.h>

#include "tempsens.h"

#include "at30tse75x.h"
#include "twi.h"

#define TEMPSENS_STACK_SIZE            (configMINIMAL_STACK_SIZE)
#define TEMPSENS_STACK_PRIORITY        (tskIDLE_PRIORITY+1)

xTaskHandle tempsens_init (void)
{
	long ret;
	xTaskHandle handle;
	
	ret = xTaskCreate(tempsens_task,
	"Temperature sensor",
	TEMPSENS_STACK_SIZE,
	NULL,
	TEMPSENS_STACK_PRIORITY,
	&handle);
	
	if (ret != pdPASS)
	{
		printf("Failed to create temperature sensor task\r\n");
		return NULL;
	}
	
	return handle;
}

void tempsens_task (void)
{
	double temp;
	
	twi_init();
		
	// Read thigh and tlow
	volatile uint16_t thigh = 0;
	thigh = at30tse_read_register(AT30TSE_THIGH_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_THIGH_REG_SIZE);
	volatile uint16_t tlow = 0;
	tlow = at30tse_read_register(AT30TSE_TLOW_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_TLOW_REG_SIZE);
		
	// Set 12-bit resolution mode.
	at30tse_write_config_register(AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit));
	
	while(1)
	{
		//at30tse_read_temperature();
		printf("Temperature sensor: %i\n", 2);
		printf("temp\n");
		
		vTaskDelay(1000);
	}
}
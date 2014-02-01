/*
 * lightsens.c
 *
 * Created: 01.02.2014 14:14:40
 *  Author: magnealv
 */

#include <asf.h>

#include "lightsens.h"

#define LIGHTSENS_STACK_SIZE            (configMINIMAL_STACK_SIZE)
#define LIGHTSENS_STACK_PRIORITY        (tskIDLE_PRIORITY+1)

struct adc_dev_inst g_adc_inst;

static void adc_cb (void)
{
	uint16_t val;
	
	if ((adc_get_status(&g_adc_inst) & ADCIFE_SR_SEOC) == ADCIFE_SR_SEOC)
	{
		val = adc_get_last_conv_value(&g_adc_inst);
		adc_clear_status(&g_adc_inst,ADCIFE_SCR_SEOC);
	}
}

xTaskHandle lightsens_init (void)
{
	long ret;
	xTaskHandle handle;
	
	ret = xTaskCreate(lightsens_task,
		    		  "Light sensor",
					  LIGHTSENS_STACK_SIZE,
					  NULL,
					  LIGHTSENS_STACK_PRIORITY,
					  &handle);
					  
	if (ret != pdPASS)
	{
		printf("Failed to create light sensor task\r\n");
		return NULL;
	}
	
	return handle;
}

/**
* \brief Light sensor task
*/
void lightsens_task (void)
{
	struct adc_config adc_cfg = {
		.prescal = ADC_PRESCAL_DIV16,
		.clksel = ADC_CLKSEL_APBCLK,
		.speed = ADC_SPEED_75K,
		.refsel = ADC_REFSEL_1,
		.start_up = CONFIG_ADC_STARTUP
	};

	struct adc_seq_config adc_seq_cfg = {
		.zoomrange = ADC_ZOOMRANGE_0,
		.muxneg = ADC_MUXNEG_7,
		.muxpos = ADC_MUXPOS_6,
		.internal = ADC_INTERNAL_2,
		.gcomp = ADC_GCOMP_DIS,
		.hwla = ADC_HWLA_DIS,
		.res = ADC_RES_12_BIT,
		.bipolar = ADC_BIPOLAR_SINGLEENDED
	};
	
	struct adc_ch_config adc_ch_cfg = {
		.seq_cfg = &adc_seq_cfg,
		.internal_timer_max_count = 60,
		.window_mode = 0,
		.low_threshold = 0,
		.high_threshold = 0,
	};
	
	adc_init (&g_adc_inst, ADCIFE, &adc_cfg);
	adc_enable (&g_adc_inst);
	adc_ch_set_config (&g_adc_inst, &adc_ch_cfg);
	adc_set_callback (&g_adc_inst, ADC_SEQ_SEOC, adc_cb, ADCIFE_IRQn, 1);
		
 	while(1)
 	{
 		adc_start_software_conversion(&g_adc_inst);
 		vTaskDelay(1000);
 	}
}
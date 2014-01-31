/*
 * CFile1.c
 *
 * Created: 31.01.2014 14:53:26
 *  Author: student
 */ 


#include <asf.h>
#include "config/conf_led.h"
#include "lib/spi.h"

#define LED_TASK_STACK_SIZE			configMINIMAL_STACK_SIZE 
#define LED_TASK_STACK_PRIORITY		(tskIDLE_PRIORITY)
#define LED_TASK_DELAY		        (1 / portTICK_RATE_MS)
//#define LED_TASK_INIT_DELAY	  ( 10 / portTICK_RATE_MS)

//! Handle for about screen task
static xTaskHandle ui_task_handle;


static int red = 0;
static int green = 0;
static int blue = 0;

static int red_old = 0;
static int green_old = 0;
static int blue_old = 0;

void led_set_red (uint8_t value)
{
	red = value;
}

void led_set_green (uint8_t value)
{
	green = value;
}

void led_set_blue (uint8_t value)
{
	blue = value;
}

void led_start_transmission (void)
{
	spi_ss_enable();
	spi_send_byte(0xaa);
	spi_ss_disable();
		
	spi_ss_enable();
	spi_send_byte(0xbb);
	spi_ss_disable();
	
}

xTaskHandle led_init (void)
{
	xTaskHandle handle;
	long ret;

	ret = xTaskCreate (led_task,
		  		 "LED",
				 LED_TASK_STACK_SIZE,
				 NULL,
				 LED_TASK_STACK_PRIORITY,
				 &handle);
				 
	if (ret != pdPASS)
	{
		return NULL;
	}
	
	return handle;
}

void led_task (void)
{
	portTickType last_wake_time;
	last_wake_time = xTaskGetTickCount();

	while(1)
	{
		if(red != red_old)
		{
			spi_ss_enable();
			spi_send_byte(0x00);
			spi_ss_disable();
			
			spi_ss_enable();
			spi_send_byte(red);
			spi_ss_disable();
		}
		if(green != green_old)
		{
			spi_ss_enable();
			spi_send_byte(0x01);
			spi_ss_disable();
			
			spi_ss_enable();
			spi_send_byte(green);
			spi_ss_disable();
		}
		if(blue != blue_old)
		{
			spi_ss_enable();
			spi_send_byte(0x02);
			spi_ss_disable();
			
			spi_ss_enable();
			spi_send_byte(blue);
			spi_ss_disable();
		}

		vTaskDelayUntil (&last_wake_time, LED_TASK_DELAY);
	}
}

xTaskHandle led_test_init (void)
{
	xTaskHandle handle;
	long ret;

	ret = xTaskCreate (led_task,
	"LED",
	LED_TASK_STACK_SIZE,
	NULL,
	LED_TASK_STACK_PRIORITY,
	&handle);
	
	if (ret != pdPASS)
	{
		return NULL;
	}
	
	return handle;
}

void led_test_task (void)
{
	portTickType last_wake_time;
	last_wake_time = xTaskGetTickCount();
	
	while(1)
	{
		/*
		led_set_red(0xff);	
		led_set_green(0x00);
		led_set_blue(0x00);
		
		vTaskDelay(500);
		
		led_set_red(0x00);
		led_set_green(0xff);
		led_set_blue(0x00);
		
		vTaskDelay(500);
		
		led_set_red(0x00);
		led_set_green(0x00);
		led_set_blue(0xff);
		
		vTaskDelay(500);
		*/
		
		spi_ss_enable();
		spi_send_byte(0b10101010);
		spi_ss_disable();
		
		vTaskDelay(100);
		
	}
}
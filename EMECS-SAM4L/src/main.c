/**
 * \file
 *
 * \brief FreeRTOS Real Time Kernel example.
 *
 * Copyright (c) 2012-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage FreeRTOS Real Time Kernel example
 *
 * \section Purpose
 *
 * The FreeRTOS example will help users how to use FreeRTOS in SAM boards.
 * This basic application shows hwo to create task and get information of
 * created task.
 *
 * \section Requirements
 *
 * This package can be used with SAM boards.
 *
 * \section Description
 *
 * The demonstration program create two task, one is make LED on the board
 * blink at a fixed rate, and another is monitor status of task.
 *
 * \section Usage
 *
 * -# Build the program and download it inside the evaluation board. Please
 *    refer to the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6224.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *    GNU-Based Software Development</a>
 *    application note or to the
 *    <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">
 *    IAR EWARM User Guide</a>,
 *    depending on your chosen solution.
 * -# On the computer, open and configure a terminal application
 *    (e.g. HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# LED should start blinking on the board. In the terminal window, the
 *    following text should appear (values depend on the board and chip used):
 *    \code
 *     -- Freertos Example xxx --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 *
 */

#include <asf.h>
#include <inttypes.h>
#include "conf_board.h"
//#include "adc.h"
#include "twi.h"
#include "at30tse75x.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)
#define TASK_TEST_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_TEST_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);

extern void xPortSysTickHandler(void);

/**
 * \brief Handler for Sytem Tick interrupt.
 */
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName)
{
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}

/**
 * \brief This function is called by FreeRTOS idle task
 */
extern void vApplicationIdleHook(void)
{
}

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void)
{
}

/**
 * \brief This task, when activated, send every ten seconds on debug UART
 * the whole report of free heap and total tasks status
 */
static void task_monitor(void *pvParameters)
{
	static portCHAR szList[256];
	UNUSED(pvParameters);

	for (;;) {
		printf("--- task ## %u", (unsigned int)uxTaskGetNumberOfTasks());
		vTaskList((signed portCHAR *)szList);
		printf(szList);
		vTaskDelay(1000);
	}
}

/**
 * \brief This task, when activated, make LED blink at a fixed rate
 */
static void task_led(void *pvParameters)
{
	UNUSED(pvParameters);
	for (;;) {
		LED_Toggle(LED0);
		vTaskDelay(1000);
	}
}

struct adc_dev_inst g_adc_inst;

static void adc_read (uint8_t chan)
{
	(uint8_t) chan;
	
	adc_start_software_conversion(&g_adc_inst);
}

static void adc_handle (void)
{
	uint16_t val;
	
	if ((adc_get_status(&g_adc_inst) & ADCIFE_SR_SEOC) == ADCIFE_SR_SEOC)
	{
		val = adc_get_last_conv_value(&g_adc_inst);
		adc_clear_status(&g_adc_inst,ADCIFE_SCR_SEOC);
		
		printf("adc value: %d\n", val);
	}
}

/**
* \brief Test task
*/
static void task_test(void *pvParameters)
{
	/* Init stuff here  */
	
	/* temp */
	twi_init();
	
	// Read thigh and tlow
	volatile uint16_t thigh = 0;
	thigh = at30tse_read_register(AT30TSE_THIGH_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_THIGH_REG_SIZE);
	volatile uint16_t tlow = 0;
	tlow = at30tse_read_register(AT30TSE_TLOW_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_TLOW_REG_SIZE);
	
	// Set 12-bit resolution mode.
	at30tse_write_config_register(AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit));
	
	volatile double temperature; 

	/* ADC */	
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
	adc_set_callback (&g_adc_inst, ADC_SEQ_SEOC, adc_handle, ADCIFE_IRQn, 1);

	printf("Init OK\n");
	
	while(1)
	{
		/* Everything else here */
		adc_read (0);
		temperature = at30tse_read_temperature();
		
		printf("temp = %d \n", (int)temperature);
		
		vTaskDelay(1000);
	}
}

/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
#if defined(__GNUC__)
	setbuf(stdout, NULL);
#else
	/* Already the case in IAR's Normal DLIB default configuration: printf()
	 * emits one character at a time.
	 */
#endif
}

/**
 *  \brief FreeRTOS Real Time Kernel example entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initilize the SAM system */
	sysclk_init();
	board_init();
	twi_init();

	/* Initialize the console uart */
	configure_console();
	

	/* Output demo infomation. */
	printf("-- Freertos Example --\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);
	
	if (xTaskCreate(task_test, "Test", TASK_TEST_STACK_SIZE, NULL,
	TASK_TEST_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create Test task\r\n");
	}

	/* Create task to monitor processor activity */
	//if (xTaskCreate(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL,
	//		TASK_MONITOR_STACK_PRIORITY, NULL) != pdPASS) {
	//	printf("Failed to create Monitor task\r\n");
	//}

	/* Create task to make led blink */
	//if (xTaskCreate(task_led, "Led", TASK_LED_STACK_SIZE, NULL,
	//		TASK_LED_STACK_PRIORITY, NULL) != pdPASS) {
	//	printf("Failed to create test led task\r\n");
	//}

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}

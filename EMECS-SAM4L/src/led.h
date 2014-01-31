/*
 * IncFile1.h
 *
 * Created: 31.01.2014 14:53:38
 *  Author: student
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

xTaskHandle led_init (void);
void led_task();

xTaskHandle led_test_init (void);
void led_test_task();

#endif /* INCFILE1_H_ */
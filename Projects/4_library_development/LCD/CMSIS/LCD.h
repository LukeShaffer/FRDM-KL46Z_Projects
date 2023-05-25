/*
 * LCD.h
 *
 *  Created on: Apr 14, 2020
 *      Author: adity
 */

#ifndef LCD_H_
#define LCD_H_


#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"

#define DB7_PIN 9//PTC-9
#define DB6_PIN 8//PTC-8
#define DB5_PIN 5//PTA-5
#define DB4_PIN 4// PTA-4
#define RS_PIN 13//PTA-13
//#define RW_PIN //GND
#define E_PIN 2//PTD-2

#define PIN_INPUT 0x0
#define PIN_OUTPUT 0x1

#define PIN_LOW 0x0
#define PIN_HIGH 0x1

#define CYCLES_TO_MS 3000 //1ms
#define CYCLES_TO_US 3 //1us


void wait_us(int us_to_wait);
void wait_ms(int ms_to_wait);
void set_RS(bool high);
void set_E(bool high);
void toggle_E();
void set_DB(unsigned char data);
void set_DB_direction(bool direction);
void send_data(bool rs, unsigned char data);
void init_clock_gating();
void init_GPIO();
void init_LCD_setup();
char digit_to_char(int digit);
int int_to_str(int x, char str[]);
void print_int(int data);
void print_float(float data, int num_decimals);
void print_string(char *data, int num_chars);

#endif /* LCD_H_ */

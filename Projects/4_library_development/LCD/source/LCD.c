/*
 * LCD.c
 *
 *  Created on: Apr 14, 2020
 *      Author: adity
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "board.h"
#include <stdbool.h>
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
// Include the header file
#include "LCD.h"

void wait_us(int us_to_wait){
	unsigned int i;

	for (i = 0; i < us_to_wait * CYCLES_TO_US; i++){
		__asm__("");
	}
}

void wait_ms(int ms_to_wait){
	unsigned int i;

	for (i = 0; i < ms_to_wait * CYCLES_TO_MS; i++){
		__asm__("");
	}
}

void set_RS(bool high){
	//pad # 36
	if(high){
		GPIOA->PDOR |= (1 << RS_PIN);
	}
	else{
		GPIOA->PDOR &= ~(1 << RS_PIN);
	}
	wait_us(1);
}

void set_E(bool high){
	// pad # 38

	if(high){
		GPIOD->PDOR |= (1 << E_PIN);
	}
	else{
		GPIOD->PDOR &= ~(1 << E_PIN);
	}
	wait_us(1);
}

void toggle_E(){
	// toggle E
	set_E(PIN_HIGH);
	wait_ms(50);
	set_E(PIN_LOW);
}
void set_DB(unsigned char data){
	// only lower 4 bits are used

	//DOUBLE CHECK THAT THE PINS LINE UP PROPERLY
	// pad #42 - #39
	//DB7 DB6 DB5 DB4

	// go through and detect if it's a 1 or a 0 and then set or clear.

	if((data & 0x08) >> 3){
		//set if 1
		GPIOC->PDOR |= (1 << DB7_PIN);
		//GPIOD->PDOR |= (data) << DB7_PIN;
	}
	else{
		GPIOC->PDOR &= ~(1 << DB7_PIN);
	}

	if((data & 0x04) >> 2){
		GPIOC->PDOR |= (1 << DB6_PIN);
		//GPIOA->PDOR |= (data) << DB6_PIN;
	}
	else{
		GPIOC->PDOR &= ~(1 << DB6_PIN);
	}

	if((data & 0x02) >> 1){
		GPIOA->PDOR |= (1 << DB5_PIN);
		//GPIOA->PDOR |= (data) << DB5_PIN;
	}
	else{
		GPIOA->PDOR &= ~(1 << DB5_PIN);
	}

	if((data & 0x01)){
		GPIOA->PDOR |= (1 << DB4_PIN);
		//GPIOA->PDOR |= (data) << DB4_PIN;
	}
	else{
		GPIOA->PDOR &= ~(1 << DB4_PIN);
	}
}
void set_DB_direction(bool direction){
	if(direction){
		GPIOC->PDDR |= (1 << DB7_PIN);
	}
	else{
		GPIOC->PDDR &= ~(1 << DB7_PIN);
	}

	if(direction){
		GPIOC->PDDR |= (1 << DB6_PIN);
	}
	else{
		GPIOC->PDDR &= ~(1 << DB6_PIN);
	}

	if(direction){
		GPIOA->PDDR |= (1 << DB5_PIN);
	}
	else{
		GPIOA->PDDR &= ~(1 << DB5_PIN);
	}

	if(direction){
		GPIOA->PDDR |= (1 << DB4_PIN);
	}
	else{
		GPIOA->PDDR &= ~(1 << DB4_PIN);
	}
}

void send_data(bool rs, unsigned char data){
	// send data
	//send upper bits
	set_RS(rs);
	//set_RW(rw);
	set_DB((data & 0xf0) >> 4);
	toggle_E();

	//send lower bits
	set_RS(rs);
	//set_RW(rw);
	set_DB(data & 0x0f);
	toggle_E();
}

void init_clock_gating(){
	SIM->SCGC5 |= (1 << 9);  //port a
	SIM->SCGC5 |= (1 << 10); // portb
	SIM->SCGC5 |= (1 << 11); // portc
	SIM->SCGC5 |= (1 << 12); // portd
	SIM->SCGC5 |= (1 << 13); // porte
}

void init_GPIO(){
	// RS
	PORTA->PCR[RS_PIN] &= ~(0x700);  			//Clear the MUX
	PORTA->PCR[RS_PIN] |= (0x700) & (1 << 8);  	//set MUX bits to GPIO
	GPIOA->PDDR |= (1 << RS_PIN); 				// set pin to 1 = output
	//GPIOD->PDOR = 1U << (RS_PIN);

	//DB7
	PORTC->PCR[DB7_PIN] &= ~(0x700);  			//Clear the MUX
	PORTC->PCR[DB7_PIN] |= (0x700) & (1 << 8);  //set MUX bits to GPIO
	GPIOC->PDDR |= (1 << DB7_PIN); 				//set pin to output
	//GPIOD->PDOR = 1U << (DB7_PIN);

	//DB6
	PORTC->PCR[DB6_PIN] &= ~(0x700);  			//Clear the MUX
	PORTC->PCR[DB6_PIN] |= (0x700) & (1 << 8);  //set MUX bits to GPIO
	GPIOC->PDDR |= (1 << DB6_PIN);				//set pin to output
	//GPIOD->PDOR = 1U << (DB6_PIN);

	//DB5
	PORTA->PCR[DB5_PIN] &= ~(0x700);  			//Clear the MUX
	PORTA->PCR[DB5_PIN] |= (0x700) & (1 << 8);  //set MUX bits to GPIO
	GPIOA->PDDR |= (1 << DB5_PIN);				//set pin to output

	//DB4
	PORTA->PCR[DB4_PIN] &= ~(0x700);  			//Clear the MUX
	PORTA->PCR[DB4_PIN] |= (0x700) & (1 << 8);  //set MUX bits to GPIO
	GPIOA->PDDR |= (1 << DB4_PIN);				//set pin to output
	//GPIOD->PDOR = 1U << (DB4_PIN);

	//E_PIN
	PORTD->PCR[E_PIN] &= ~(0x700);  			//Clear the MUX
	PORTD->PCR[E_PIN] |= (0x700) & (1 << 8);  //set MUX bits to GPIO
	GPIOD->PDDR |= (1 << E_PIN);				//set pin to output
}

void init_LCD_setup(){

	// Step 1
	// Power on happens automatically.

	// Step 2
	// initial wait after power on.
	wait_ms(50);


	//begin LCD setup
	unsigned char data;

	//Step 3
	//First instruction is 8-bit by default, send only one
	set_RS(PIN_LOW);
	//set_RW(PIN_LOW);
	data = 3;
	set_DB(data);
	toggle_E();//Step 4

	//Step 5
	//Second instruction.
	set_RS(PIN_LOW);
	//set_RW(PIN_LOW);
	data = 3;
	set_DB(data);
	toggle_E();// Step 6

	//Step 7
	//Third instruction.
	set_RS(PIN_LOW);
	//set_RW(PIN_LOW);
	data = 3;
	set_DB(data);
	toggle_E();

	//Step 8
	set_RS(PIN_LOW);
	//set_RW(PIN_LOW);
	data = 2;
	set_DB(data);
	toggle_E();

	//Step 9
	// function set. sets 4-bit operation and selects 1-line display
	// 5x8 dot character font
	// 4bit operation starts from next line.
	data = 0x28;
	send_data(PIN_LOW, data);

	//Step 10
	// turn display off
	data = 0x0C;
	send_data(PIN_LOW, data);

	//Step 11
	// display clear
	data = 1;
	send_data(PIN_LOW, data);

	//Step 12
	// Entry mode set
	data = 6;
	send_data(PIN_LOW, data);

}

char digit_to_char(int digit){
	switch (digit){
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	default:
		return '\0';
	}

}

int int_to_str(int x, char str[])
{
	int i;
	i = 0;
	char tmp[100];

	while (x > 0){
		tmp[i] = digit_to_char(x % 10);
		x /= 10;
		i++;
	}

	int j;

	for (j=0; j<i; j++){
		str[j] = tmp[i-j-1];
	}

	return i;
}


void print_int(int data){
	char buffer[100];

	int length;
	length = int_to_str(data, buffer);
	print_string(buffer, length);
}

void print_float(float data, int num_decimals){
	char buffer[100];

	int front_length;

	front_length = int_to_str((int)data, buffer);
	buffer[front_length] = '.';

	int i;

	char dec_char;
	for (i =1; i< num_decimals+1; i++){
		dec_char = digit_to_char((int)(data * (pow(10,i)) )%10);
		buffer[front_length + i] = dec_char;
	}
	buffer[num_decimals+2] = '\0';

	print_string(buffer, front_length + num_decimals + 1);
}

void print_string(char *data, int num_chars){
	int i;
	for (i=0; i< num_chars; i++){
		printf("%c", *data);
		//send_data(PIN_HIGH, *data);
		data++;
	}
}

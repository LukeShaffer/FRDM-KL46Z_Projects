/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    Project2_Morse.c
 * @brief   Application entry point.
 */
#define CYCLES_PER_Q_SEC 1200000
#define MORSE_MASK 0b10000000

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */


/* TODO: insert other definitions and declarations here. */

//mapping of chars to their morse reps
// starts at A and ends at 0, dots are 0 and dashes are 1
unsigned char morse_map[36] = {
	0b01000000,  //A
	0b10000000,
	0b10100000,
	0b10000000,
	0b00000000,  //E
	0b00100000,
	0b11000000,
	0b00000000,
	0b00000000,
	0b01110000,  //J
	0b10100000,
	0b01000000,
	0b11000000,
	0b10000000,
	0b11100000,  //O
	0b01100000,
	0b11010000,
	0b01000000,
	0b00000000,
	0b10000000,  //T
	0b00100000,
	0b00010000,
	0b01100000,
	0b10010000,
	0b10110000,  //Y
	0b11000000,

	//numbers, 1-0
	0b01111000,
	0b00111000,
	0b00011000,
	0b00001000,
	0b00000000,
	0b10000000,
	0b11000000,
	0b11100000,
	0b11110000,
	0b11111000
};


//number of morse dots or dashes in a character
unsigned char morse_len[36] = {
	2,  //A
	4,
	4,
	3,
	1,  //E
	4,
	3,
	4,
	2,
	4,  //J
	3,
	4,
	2,
	2,
	3,  //O
	4,
	4,
	3,
	3,
	1,  //T
	3,
	4,
	3,
	4,
	4,  //Y
	4,

	//numbers
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5
};


// busy wait for 250 ms
void waitQSec(){
	int i = 0;
	for (i = CYCLES_PER_Q_SEC;i != 0; i--){
		asm("");
	}
}

// shows the LED for 250 ms, then turns it off
void display_dot(){
	// make sure light is off to start
	GPIOD->PDOR |= (1 << 5);

	//turn light on
	GPIOD->PTOR |= (1 << 5);

	waitQSec();

	//turn light off
	GPIOD->PTOR |= (1 <<5);
}

//shows LED for 750 ms, then turns it off
void display_dash(){
	// make sure light is off to start
	GPIOD->PDOR |= (1 << 5);

	//turn light on
	GPIOD->PTOR |= (1 << 5);

	waitQSec();
	waitQSec();
	waitQSec();

	//turn light off
	GPIOD->PTOR |= (1 <<5);
}

void displayChar(char to_display){
	//index char from ascii value to 0-35
	int index;

	//number, range from index 26 to 35
	if (to_display > 47 && to_display < 58){
		index = to_display - 23;
	}

	// Capital letters, range from index 0 to 25
	else if (to_display > 64 && to_display < 91){
		index = to_display - 65;
	}

	//lowercase letters, range from index 0 to 25
	else if (to_display > 96 && to_display < 123){
		index = to_display - 97;
	}

	//spaces, wait one more qsec to pad word gap and return
	else if (to_display == 32){
		waitQSec();
		return;
	}

	//display the symbol
	int num_flash;

	for(num_flash = 0; num_flash < morse_len[index]; num_flash++){
		if(morse_map[index] & (MORSE_MASK >> num_flash) ){
			display_dash();
		}
		else{
			display_dot();
		}
		//wait 250 ms between flashes, even after the last flash
		waitQSec();
	}
}

void displayMorse(char *message, int length){
	// iterate over message and display one char at a time
	int i;
	for (i=0; i < length; i++){
		displayChar(message[i]);
		//only 2 waits because we wait 250 ms after every char flash
		waitQSec();
		waitQSec();
	}
}
/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("Hello World\n");

    // Enable clock to port d for green led
    SIM->SCGC5 |= (1 << 12);

    // set port pin control register to gpio for the led pin
    PORTD->PCR[5] |= (1 << 8);

    //Turn it off to start
    GPIOD->PTOR |= (1 << 5);

    //set data direction for that pin to output
    GPIOD->PDDR |= (1 << 5);

    displayMorse("TEST test", 9);

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
    	i++;
    }
    return 0 ;
}

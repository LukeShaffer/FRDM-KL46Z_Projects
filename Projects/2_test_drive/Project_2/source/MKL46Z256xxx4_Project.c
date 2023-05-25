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
 * @file    Test_Drive.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"

#define LEFT_BIAS 7999 * 0.054


#define WHEELS_FULL_LEFT 7999 * 0.4
#define WHEELS_HALF_LEFT 7999 * 0.3
#define WHEELS_CENTER 7999 * 0.2 + LEFT_BIAS
#define WHEELS_HALF_RIGHT 7999 * 0.15
#define WHEELS_FULL_RIGHT 7999 * 0.007

#define SOFT_GAS  7999 * 0.05
#define HARD_GAS  7999 * 0.1
/*
 * @brief   Application entry point.
 */

void delay_ms(unsigned int n){
	unsigned int i = 0;
	unsigned int j;

	for(i=0; i< n*3500; i++){
		j++;
		asm("");
	}
}

void turn_wheels(unsigned int wheel_pos){
	TPM2->CONTROLS[0].CnV = wheel_pos;
	delay_ms(10);
	}

void press_gas(unsigned int mil_sec, unsigned int amount){
	TPM2->CONTROLS[1].CnV = amount;
	delay_ms(mil_sec);
	TPM2->CONTROLS[1].CnV = 0;
}

void make_right_turn(){
	int corner_time = 950;
	turn_wheels(WHEELS_FULL_RIGHT);
	press_gas(corner_time, SOFT_GAS);
	//delay_ms(200);
}

void drive_square(void){
	int straight_time = 1410;


	//start on top-left corner
 	turn_wheels(WHEELS_CENTER);
 	delay_ms(500);

	//top edge
	press_gas(straight_time, HARD_GAS);

	//top-right turn
	make_right_turn();

	//right edge
	turn_wheels(WHEELS_CENTER);
	press_gas(straight_time, HARD_GAS);

	//bottom-right turn
	make_right_turn();

	//bottom edge
	turn_wheels(WHEELS_CENTER);
	press_gas(straight_time, HARD_GAS);

	//bottom-left turn
	make_right_turn();

	//left edge
	turn_wheels(WHEELS_CENTER);
	press_gas(straight_time, HARD_GAS);

	//top-left turn
	make_right_turn();


	turn_wheels(WHEELS_CENTER);
}

void drive_circle(){
	//slow circle 1 lap, fast circle 1 lap
	turn_wheels(WHEELS_HALF_RIGHT);
	press_gas(17560, SOFT_GAS);

	//larger duty cycle
	turn_wheels(WHEELS_HALF_RIGHT);
	press_gas(5740, HARD_GAS);
}

int main(void) {

  	/* Init board hardware. */

	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();


    SIM->SCGC5 |= (1 << 10); //port B, steering servo & motor speed
    SIM->SCGC5 |= (1 << 12); //port D, motor direction
    SIM->SCGC6 |= (1 << 26); // TPM2
    SIM->SCGC5 |= (1 << 11); // clock gating for SW1

    //Setup SW1
    PORTC->PCR[3] &= ~(0x703); // Clear the MUX and PE/PS bits
    PORTC->PCR[3] |= (0x703) & ((1<<8) | 0x03); // Set MUX bits to GPIO, set pull up and pull enable
    GPIOC->PDDR &= ~(1<<3); //Clear Data Direction to input


    //Clear pin control Mux's
    PORTB->PCR[3] &= ~(0x7 << 8); //motor speed
    PORTB->PCR[2] &= ~(0x7 << 8); //steering servo

    //set PTB to use TPM2, various channels defined per pin, as input
    PORTB->PCR[2] |= (0x300); //steering servo, uses TMP2 CH0
    PORTB->PCR[3] |= (0x300); //motor speed, uses TPM2 CH1

    //set tpm clock source to OSCERCLK (global)
    SIM->SOPT2 |= (0x2 << 24);

    //set prescaler to 2
    TPM2->SC |= (0x1);

    //set the max tick count
    TPM2->MOD = 7999;

    //set clock to Edge-match PWM high
    //TPM2->CONTROLS[0].CnSC |= (0x1 << 2) | (0x2 << 4); // channel 0
    //TPM2->CONTROLS[1].CnSC |= (0x1 << 2) | (0x2 << 4); //channel 1

    //Active high ??
    TPM2->CONTROLS[0].CnSC |= (0x1 << 2) | (0x2 << 4); // channel 0
    TPM2->CONTROLS[1].CnSC |= (0xA << 2); // channel 1


    //start clock
    TPM2->SC |= (0x1 << 3);


    while(1) {

    	turn_wheels(WHEELS_FULL_RIGHT);
    	turn_wheels(WHEELS_CENTER);
    	drive_square();
    	delay_ms(5000);
    	drive_circle();
    	PRINTF("ONE LAP DOWN\n");

    	delay_ms(10000);

    }
    return 0 ;
}

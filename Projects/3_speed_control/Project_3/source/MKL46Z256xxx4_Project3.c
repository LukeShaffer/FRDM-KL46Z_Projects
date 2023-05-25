#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

// The wheels are crooked and don't go straight
#define LEFT_BIAS 7999 * 0.054

#define WHEELS_FULL_LEFT 7999 * 0.4
#define WHEELS_HALF_LEFT 7999 * 0.3
#define WHEELS_CENTER 7999 * 0.2 + LEFT_BIAS
#define WHEELS_HALF_RIGHT 7999 * 0.15
#define WHEELS_FULL_RIGHT 7999 * 0.007

#define SOFT_GAS  7999 * 0.07 //0.05
#define HARD_GAS  7999 * 0.1

// all distances measured in micrometers unlesss otherwise specified
#define DISTANCE_PER_WHEEL 319024 //Micrometers
#define DISTANCE_PER_TICK DISTANCE_PER_WHEEL / 36 //10986//8890 //micrometers, 0.00889 in meters
#define TEN_METER_TICKS  1161 - (2 * 36)//1822(36 seconds) //911(10 seconds) //911

#define KP 0.5
#define KPD 0.005

// distance measurements are taken in ticks of the wheel encoder
unsigned int total_time = 0;
unsigned int total_distance_covered_ticks = 0;
unsigned int last_timestamp_distance_covered_ticks = 0;

int measured_speed = 0;
int set_point = 500; //micrometers per ms
int new_speed = SOFT_GAS;

// Setup a millisecond timer clock using PIT
void init_pits() {
	SIM->SCGC6 |= (1 << 23);
	PIT->MCR = 0x00;
	//Auxillary clock, used for PIT testing and debugging during development
	PIT->CHANNEL[0].LDVAL = 240000000; // setup timer 0 for 120M cycles (10 seconds)
// PIT->CHANNEL[1].LDVAL = 12000000; // Half-second
// Use channel 1 for quarter of a second

	// Wheel speed measurement period
	PIT->CHANNEL[1].LDVAL = 6000000; //Quarter-second

	// PIT IRQ, timer creates interrupt
	NVIC_EnableIRQ(22);
	PIT->CHANNEL[1].TCTRL = 0x3; // enable Timer 0 interrupts and start timer.
}


//sets up servo and motor on the board
void setup_servo_motor() {
// Setup SERVO AND MOTOR
	SIM->SCGC5 |= (1 << 10); //port B, steering servo & motor speed
	SIM->SCGC5 |= (1 << 12); //port D, motor direction
	SIM->SCGC6 |= (1 << 26); // TPM2
	SIM->SCGC5 |= (1 << 11); // clock gating for SW1

//Setup SW1
	PORTC->PCR[3] &= ~(0x703); // Clear the MUX and PE/PS bits
	PORTC->PCR[3] |= (0x703) & ((1 << 8) | 0x03); // Set MUX bits to GPIO, set pull up and pull enable
	GPIOC->PDDR &= ~(1 << 3); //Clear Data Direction to input

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

//Active high
	TPM2->CONTROLS[0].CnSC |= (0x1 << 2) | (0x2 << 4); // channel 0
	TPM2->CONTROLS[1].CnSC |= (0xA << 2); // channel 1
}

void turn_wheels(unsigned int wheel_pos) {
	TPM2->CONTROLS[0].CnV = wheel_pos;
}

// 36 transitions on the encoder
void setup_encoders() {
	SIM->SCGC5 |= (1 << 9);  // Enable Port A
	// Encoder connected to PTA13
	PORTA->PCR[13] |= 0xF0703 & ((0xB << 16) | (1 << 8) | 0x3); // Set MUX bits, enable pullups, interrupt on both edges

	GPIOA->PDDR &= ~(1 << 13); // Setup Pin 13 Port A as input
	//set the encoder value change to initiate a PORTA_IRQHandler interrupt
	NVIC_EnableIRQ(30);
}

//encoder interrupt handler, increments distance traveled
void PORTA_IRQHandler(void) {
	//encoder
	//PRINTF("THE ENCODER TRIGGERED THIS INTERRUPT\n");
	//PRINTF("THIS IS THE ENCODER VALUE: %d\n", GPIOA->PDIR);

	total_distance_covered_ticks++;
	last_timestamp_distance_covered_ticks++;

	//clear pin 13 interrupt status flag
	PORTA->PCR[13] |= (1 << 24);
	return;
}


//Wheel speed check interrupt
void PIT_IRQHandler(void) {
	// debug pit, not used in project
	if (PIT->CHANNEL[0].TFLG) { // Timer 0 Triggered
		PIT->CHANNEL[0].TFLG = 1; // Reset
		PIT->CHANNEL[0].TCTRL = 0; // Disable Timer
		PRINTF("THE LONG TIMER WENT OFF\n");
		//PIT->CHANNEL[1].TCTRL = 0x3; // Enable Timer 1 Interrupts and start timer
	}

	//250 ms timer
	if (PIT->CHANNEL[1].TFLG) { // Timer 1 Triggered
		PIT->CHANNEL[1].TFLG = 1; // Reset
		//PIT->CHANNEL[1].TCTRL = 0; // Disable Timer
		//PRINTF("HEY THIS IS THE SHORT TIMER\n");

		if (set_point != 0) {
			//micrometers per millisecond
			measured_speed = (last_timestamp_distance_covered_ticks * DISTANCE_PER_TICK) / 250; //16323

			new_speed = (KP * (set_point - measured_speed)) + SOFT_GAS;

			// our motor does not do reverse power, so just turn it off if less "oomf" is required
			if (new_speed < 0){
				new_speed = 0;
			}

			//PRINTF("MEASURED_SPEED: %d\n", measured_speed);
			//PRINTF("NEW SPEED %d\n\n", new_speed);

			// period value
			total_time += 250;
			last_timestamp_distance_covered_ticks = 0;
		} else {

			new_speed = 0;
		}
	}
}

/* TODO: insert other definitions and declarations here. */

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

	setup_servo_motor();
	init_pits();
	setup_encoders();

	turn_wheels(WHEELS_CENTER);
	TPM2->CONTROLS[1].CnV = 200;

	//start Motor PWN clock
	TPM2->SC |= (0x1 << 3);


	/* Enter an infinite loop, just incrementing a counter. */
	while (1) {

		//PRINTF("SETTING NEW SPEED: %d\n", new_speed);
		//PRINTF("TICKS TRAVELED: %d\n", total_distance_covered_ticks);

		if (total_distance_covered_ticks>=TEN_METER_TICKS) {
			TPM2->CONTROLS[1].CnV = 0;
			set_point = 0;
			turn_wheels(WHEELS_CENTER);
			PRINTF("GOAL REACHED IN %X ms\n", total_time);
			PRINTF("TOTAL TICKS ELAPSED: %d\n", total_distance_covered_ticks);
		}

		else if (total_distance_covered_ticks >0 && total_distance_covered_ticks < 36 ){
			//turn_wheels(WHEELS_FULL_LEFT);
			TPM2->CONTROLS[1].CnV = new_speed;

		}

		else{
			TPM2->CONTROLS[1].CnV = new_speed;
		}

	}
	return 0;
}










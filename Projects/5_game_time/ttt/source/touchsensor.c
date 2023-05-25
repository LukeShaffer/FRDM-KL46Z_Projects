/*
 * touchsensor.c
 *
 *  Created on: May 7, 2020
 *      Author: adity
 */


#include "MKL46Z4.h"
#include "touchsense.h"

// TSI initialization function

void touch_sensor_init(void)
{
	// clock gating for TSI port b 16 and 17
	SIM->SCGC5 |= 0x20u;
	// using masks
	TSI0->GENCS = ((0x80000000u)| // OUTRGF. check for range. set to 1
			(0u)| // MODE capacitive sensing. don't use nose or threshold
			(0u)| // REFCHRG reference charge. clear
			(0u)| // DVOLT voltage range DV = 1.03 V; VP = 1.33 V; Vm = 0.30 V
			(0u)| // EXTCHRG electrode oscillator charging and discharge val. 500 nA
			(0u)| // PS prescaler of the electrode oscillator output. Electrode Oscillator Frequency divided by 1
			(0x1F00u)| // NSCN scan number for each electrode - 32 times per electrode
			(0x80u)| // TSIEN set the touch sensing enable bit
			(0x20u)| // STPE TSI stop enable
			(0x4u)); // EOSF end of scan flag set
}

// Read touch sensor left(low capacitance) to right(high capacitance)
int low_to_high(void)
{
	TSI0->DATA = ((10u << 28U) & 0xF0000000U);
	TSI0->DATA |= 0x400000U;
	TSI0->GENCS |= 0x4U ;
	int sensor_val = ((TSI0->DATA & 0xFFFF) - OFFSET);
	return sensor_val;
}

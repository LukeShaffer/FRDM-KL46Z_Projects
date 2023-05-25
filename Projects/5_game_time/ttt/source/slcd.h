/*
 * slcd.h
 *
 *  Created on: May 7, 2020
 *      Author: adity
 */

#ifndef SLCD_H_
#define SLCD_H_

#include "MKL46Z4.h"

#define no_of_frontplanes 8
// segment descriptions
#define A			0x88
#define B			0x44
#define C			0x22
#define D			0x11
#define E			0x22
#define G			0x44
#define F			0x88
#define DECIMAL		0x11
#define COLON		0x11
#define EMPTY		0x00

// function definitions
void pin_init(void);
void slcd_set_digit(uint32_t number, uint32_t print_digit);
void slcd_game(uint32_t win, uint32_t total);
void slcd_init(void);
void contrast_control(uint32_t level);
void decimal_point(uint32_t pos);

#endif /* SLCD_H_ */

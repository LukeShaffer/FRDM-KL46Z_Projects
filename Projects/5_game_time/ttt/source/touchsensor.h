/*
 * touchsensor.h
 *
 *  Created on: May 7, 2020
 *      Author: adity
 */

#ifndef TOUCHSENSOR_H_
#define TOUCHSENSOR_H_

#include "MKL46Z4.h"
// scanned data offset
#define OFFSET 544

// Function Declarations
void touch_sensor_init(void);
int low_to_high(void);



#endif /* TOUCHSENSOR_H_ */

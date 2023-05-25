/*
 * slcd.c
 *
 *  Created on: May 7, 2020
 *      Author: adity
 */


/*
 * slcd_1.c
 *
 *  Created on: May 4, 2020
 *      Author: adity
 */

#include "slcd.h"

// global variables
static uint32_t fronplane_pins[4][2] = {  { 37, 17 }, // Digit 1 pins
									 	 { 7, 8 }, // Digit 2 pins
										 { 53, 38 }, // Digit 3 pins
										 { 10, 11 } // Digit 4 pins
};
const uint32_t waveform_reg[] = {
    37,  // LCD81
    17,  // LCD82
    7,   // LCD83
    8,   // LCD84
    53,  // LCD85
    38,  // LCD86
    10,  // LCD87
    11,  // LCD88
    40,  // LCD77
    52,  // LCD78
    19,  // LCD79
    18,  // LCD80
};

// Function Definitions
void slcd_init(void){

	// Clock Gating
    SIM->SCGC5 |= (0x80000U | 0x400U | 0x800U | 0x1000U | 0x2000U);

    // configure pins for LCD operation
    PORTC->PCR[20] = 0x00000000;     //VLL2
    PORTC->PCR[21] = 0x00000000;     //VLL1
    PORTC->PCR[22] = 0x00000000;     //VCAP2
    PORTC->PCR[23] = 0x00000000;     //VCAP1

    // Internal reference clock
    MCG->C1  |= 0x2U | 0x1U;
    MCG->C2  |= 0x1U ; //4 MHz

    // Set general and auxillary registers to zero initial value
    LCD->GCR = 0x0;
    LCD->AR  = 0x0;
    // masks for GCR
    // General control register init
    LCD->GCR =    (  (0x80000000u) // disable voltage regulator clear RVEN
                   | (((8<< 24u)) & 0xF000000u) // set RVTRIM to known state 0
                   | (0x800000u) // CPSEL to use capacitor charge pump
                   | (((3<< 20u)) & 0x300000u) // set LADJ to low power i.e slower clock rate.
                   | (0x20000u) //VSUPPLY - VLL driven externally
                   | (!0x4000u) // fault detection interrupt
                   | ((1<<12u) & 0x3000u) // ALTDIV
                   | (!0x200u) // LCDDOZE in doze mode
                   | (!0x100u) // LCDSTP in stop mode
                   | (!0x80u) // LCDEN will be available in the next step
                   | (0x40u) // SOURCE set clock source to OSCERCLK
                   | (0x0u) //ALTSOURCE set clock source to OSCERCLK
                   | (0x0u) // LCD Clock prescaler is set to 0
                   | (0x3u) // DUTY to 1/4th duty cycle set to 0x3 for 4 backplane pins.
                 );
    	pin_init();
        LCD->GCR |= 0x80u; // Starts LCD
    }


void pin_init(void){
		int i = 0;
	    uint32_t *lcd_PEN;
	    uint8_t PEN_lcd_offset;
	    uint8_t PEN_digit; // 32 total digits
	    LCD->BPEN[0] = 0x0;
		LCD->BPEN[1] = 0x0;
	    LCD->PEN[0] = 0x0;
	    LCD->PEN[1] = 0x0;
	    // set lcd_PEN to the address of first PEN digit to set the planes
	    lcd_PEN = (uint32_t *)&LCD->PEN[0];
	    while(i<12) {
	    	// 32 total bits goes 0 - 31
	        PEN_lcd_offset=waveform_reg[i]/32;
	        PEN_digit=waveform_reg[i]%32;
	        lcd_PEN[PEN_lcd_offset] |= 1 << PEN_digit;
	        // using 8  frontplanes
	        if (i> 7) {
	        	// setting backplanes
	            lcd_PEN[PEN_lcd_offset+2] |= 1 << PEN_digit;
	            LCD->WF8B[(uint8_t)waveform_reg[i]] = 1 << (i - 8);
	        }
	     i++;
	    }
}



void slcd_set_digit(uint32_t number, uint32_t print_digit){
	// limit for print_digit is 4 as no scroll function has been implemented
	if ((print_digit > 0x3) || (number > 0xF))
		return;
	else{
	switch(number) {
		case 0x0:
			// WF8B has type
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

		case 0x1:
			LCD->WF8B[fronplane_pins[print_digit][0]] = EMPTY;
			LCD->WF8B[fronplane_pins[print_digit][1]] = B | C;
			break;

		case 0x2:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | G | E;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B;
			break;

		case 0x3:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

		case 0x4:
			LCD->WF8B[fronplane_pins[print_digit][0]] = F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = B | C;
			break;

		case 0x5:
			LCD->WF8B[fronplane_pins[print_digit][0]] = F | G | D;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | C;
			break;

		case 0x6:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | C;
			break;

		case 0x7:
			LCD->WF8B[fronplane_pins[print_digit][0]] = EMPTY;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

		case 0x8:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

		case 0x9:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

		case 0xA:
			LCD->WF8B[fronplane_pins[print_digit][0]] = E | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

		case 0xB:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = C;
			break;

		case 0xC:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A;
			break;

		case 0xD:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = B | C;
			break;

		case 0xE:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A;
			break;

		case 0xF:
			LCD->WF8B[fronplane_pins[print_digit][0]] = E | F | G;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A;
			break;

		default:
			LCD->WF8B[fronplane_pins[print_digit][0]] = D | E | F;
			LCD->WF8B[fronplane_pins[print_digit][1]] = A | B | C;
			break;

	}
	}
}

// Prints game score
void slcd_game(uint32_t win, uint32_t total){
	if((win > 99) || (total > 99)) {
		return;
	}
	else{
	uint32_t pos = 2;
	slcd_set_digit(win/10, 0);
	slcd_set_digit(win%10, 1);
	// divider
	decimal_point(pos);
	decimal_point(pos-1);
	slcd_set_digit(total/10, 2);
	slcd_set_digit(total%10, 3);
	}
}

// increases from 0 to 15. 15 is highest
void contrast_control(uint32_t level){
    level &= 0x0F;
    LCD->GCR |= ((level << 23u) & 0x800000u);
}


void decimal_point(uint32_t pos){
	if(pos <=2){
		LCD->WF8B[fronplane_pins[pos][1]] |= DECIMAL;
	}
}

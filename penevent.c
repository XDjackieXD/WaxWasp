/*************************************************************
WaxWasp PenEvent
File:    penevent.c
Author:  Jakob
Licensed under GPL v2.
You should have received a copy of the license with this code.
*************************************************************/

#include "penevent.h"

#include <avr/io.h>

void input_pen_event(struct penEvent penevent){
	if(penevent.touch)
		PORTB |= 0b00000010;	//turn on PB1 when pen touching the surface
	else
		PORTB &= 0b11111101;
	return;
}

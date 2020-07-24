/*************************************************************
WaxWasp PenEvent
File:    penevent.h
Author:  Jakob
Licensed under GPL v2.
You should have received a copy of the license with this code.
*************************************************************/

#ifndef __PENEVENT_H_
#define __PENEVENT_H_

#include <stdint.h>
struct penEvent {
	// serial_packet_first_byte 0xE0 is the value used for a standard stylus position update.
	// It's important to use this value for non-Intuos tablets that may not explicitly
	// set serial_packet_first_byte or their penEvents could have problems like being
	// misinterpreted as Intuos Airbrush "second packets".
	//PenEvent() : tool_id(0), tool_serial_num(0), serial_packet_first_byte(0xE0) { };

	unsigned proximity:1;
	unsigned eraser:1;
	unsigned is_mouse:1; // 0=pen, 1=mouse
	unsigned button0:1;
	unsigned button1:1;

	unsigned touch:1; // pen is touching the surface

	uint16_t x;	// "slave" tablet coordinates
	uint16_t y;	// "slave" tablet coordinates
	uint16_t pressure; // "slave" tablet pressure value

	int16_t tilt_x; // this is signed
	int16_t tilt_y; // this is signed

	int16_t rotation_z; // this is signed

	uint32_t tool_id;
	uint32_t tool_serial_num;

	// For mouse support:
	unsigned discard_first;
	uint8_t serial_packet_first_byte;
	int16_t throttle;
	uint8_t buttons;
	int8_t relwheel;

	// For airbrush support:
	int16_t abswheel;
};

void input_pen_event(struct penEvent penevent);

#endif

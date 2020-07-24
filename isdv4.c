/*************************************************************
WaxWasp ISDV4 Interface Code
File:    isdv4.c
Author:  Jakob
Some parts shamelessly stolen from waxbee :P
Licensed under GPL v2.
You should have received a copy of the license with this code.
*************************************************************/

#include "isdv4.h"

#define WACOM_PKGLEN_TPC 	 9
#define WACOM_PKGLEN_TPCCTL     11

#ifndef F_CPU

#endif



#define F_12MHz
//#define F_16MHz

#ifdef F_12MHz
	#ifndef F_CPU
		#define F_CPU 12000000UL
	#endif
	#define UBRR0H_val 0x00
	#define UBRR0L_val 0x26
#endif

#ifdef F_16MHz
	#ifndef F_CPU
		#define F_CPU 16000000UL
	#endif
	#define UBRR0H_val 0x00
	#define UBRR0L_val 0x51
#endif



#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "penevent.h"

#define true 1
#define false 0

#define halted 0
#define query_packet 1
#define packet 2

#define BITV(bit, val)  (val << bit)

unsigned char state = 0;	//0=halted, 1=query_packet, 2=packet
unsigned char datalen = 0;
unsigned char buffer[11];
struct {
   unsigned in_proximity:1;
   unsigned eraser_mode:1;
} tool;

struct penEvent penevent;



void uart_putch(unsigned char c){
	while(!(UCSR0A & (1<<UDRE0))){}	//wait till buffer is empty
	UDR0 = c;			//write char to buffer
}

void uart_puts(char *s){
	while(*s){
		uart_putch(*s);
		s++;
	}
}

void resetToolState(){
	tool.in_proximity = 0;
	tool.eraser_mode = 0;
}

void start_packets(){
	datalen = 0;
	uart_puts("\r\r1\r"); // send a few \r to make sure we are in sync
	state = packet;
}



void gotSerialByte(unsigned char data){
	if(state == 0) return;
	else{	//packet
		if(data & 0x80){
			datalen = 0;
		}else if(datalen == 0){
			return;		// wait for the first valid byte
		}

		if(datalen < WACOM_PKGLEN_TPC){
			buffer[datalen] = data;
			datalen++;
		}

		if(datalen == WACOM_PKGLEN_TPC){
			// event consumed
			datalen = 0;

			penevent.proximity = (buffer[0] & 0x20)?1:0;

			penevent.x =    (((uint16_t)(buffer[6] & 0x60)) >> 5) |
					(((uint16_t) buffer[2]        ) << 2) |
					(((uint16_t) buffer[1]        ) << 9);

			penevent.y =	(((uint16_t)(buffer[6] & 0x18)) >> 3) |
					(((uint16_t) buffer[4]        ) << 2) |
					(((uint16_t) buffer[3]        ) << 9);
			//pressure
			penevent.pressure = (((uint16_t)(buffer[6] & 0x07)) << 7) | buffer[5];

			//not sure if this bit is looked at anyways
			penevent.touch = penevent.pressure > 10;

			penevent.button0 = buffer[0] & 0x02;
			penevent.button1 = buffer[0] & 0x04;

			//check which device has been reported
			unsigned char curEvent_eraser = (buffer[0] & 4)? 1:0;

			//detect tool state
			if(!tool.in_proximity)
			{
				if(penevent.proximity)
				{
					//entering proximity
					tool.in_proximity = true;

					if(curEvent_eraser)
						tool.eraser_mode = true;
					else
						tool.eraser_mode = false;

				}
			}
			else if(!penevent.proximity)
				resetToolState();

			penevent.eraser = tool.eraser_mode;

			if(tool.eraser_mode)
			{
				// no buttons with eraser
				penevent.button0 = 0;
				penevent.button1 = 0;
			}

			penevent.is_mouse = 0;

			// TODO: understand the extra logic from the linux driver (wcmISDV4.c)

			/* check on previous proximity */
			/* we might have been fooled by tip and second
			 * sideswitch when it came into prox */

			// no tilt data?
			penevent.tilt_x = 0;
			penevent.tilt_y = 0;

			penevent.rotation_z = 0;
			input_pen_event(penevent);
		}
	}
}

void init_isdv4(){
	UBRR0H = UBRR0H_val;
	UBRR0L = UBRR0L_val;	//19200 Baud at 12MHz

	UCSR0A = 0;

	UCSR0B =	BITV(RXCIE0, 1) |
			BITV(TXCIE0, 0) |
			BITV(UDRIE0, 0) |
			BITV(RXEN0, 1) |
			BITV(TXEN0, 1) |
			BITV(UCSZ02, 0);   //8 bits

	UCSR0C = 	BITV(UMSEL00, 0) | // Async USART
			BITV(UMSEL01, 0) | // Async USART
			BITV(UPM00, 0) | // No parity
			BITV(UPM01, 0) | // No parity
			BITV(USBS0, 0) |   //1 stop bit
			BITV(UCSZ01, 1) |  //8 bits
			BITV(UCSZ00, 1);   //8 bits

	resetToolState();
	sei();
	start_packets();
}

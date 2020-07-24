/*************************************************************
WaxWasp Main Code
File:    main.c
Author:  Jakob
Licensed under GPL v2.
You should have received a copy of the license with this code.
*************************************************************/

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "isdv4.h"

//-------------------------------------------------------------------------------
// Low level interrupt driven buffer.  Buffer code courtesy of NewSoftSerial :)

#define _SS_MAX_RX_BUFF 800 // RX buffer size

uint8_t _receive_buffer[_SS_MAX_RX_BUFF];
volatile uint16_t _receive_buffer_tail = 0;
volatile uint16_t _receive_buffer_head = 0;

void flush(){
	/** empty the internal RxD FIFO buffer */
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) )
		dummy = UDR0;

	// delete FIFO content (if any)
	_receive_buffer_head = _receive_buffer_tail;
}

/** Read data from buffer */
int read(){
	// Empty buffer?
	if (_receive_buffer_head == _receive_buffer_tail)
		return -1;

	// Read from "head"
	uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte

	_receive_buffer_head++;

	if(_receive_buffer_head >= _SS_MAX_RX_BUFF)
		_receive_buffer_head = 0;

	return d;
}

ISR(USART_RX_vect){
	// Empty the data in the FIFO
	while(UCSR0A & (1<<RXC0)){
		// data overrun test (must be done before reading UDR1)
		// if(UCSR1A & (1<<DOR1))

		// pop byte from FIFO
		uint8_t data = UDR0;

		// if buffer full, set the overflow flag and return
		if ((_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF != _receive_buffer_head){
			// save new data in buffer: tail points to where byte goes
			_receive_buffer[_receive_buffer_tail] = data; // save new byte
			_receive_buffer_tail++;
			if( _receive_buffer_tail >= _SS_MAX_RX_BUFF)
				_receive_buffer_tail = 0;
		}else{
			// dropping bytes here
			//_buffer_overflow = true;
		}
	}
}

//----------------------------------------------------------------------------------------------






int main(){
	DDRB = 0b00000010;	//PB1 as Output for debugging LED
	init_isdv4();
	int data;

	for(;;){
		data = read();
		if(data == -1){}
		else
			gotSerialByte(data); // process the received byte	
	}
	return 0;
}


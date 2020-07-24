A rewrite of waxbee (https://code.google.com/p/waxbee/) in pure C using vUSB.
The goals are to make the code run on cheaper, non USB AVRs and to make the code smaller.
The ISDV4 interface code is heavily based on waxbee.

Current state:
It *should* turn on a LED connected to PB1 when a pen touches the surface of a serial wacom digitizer connected to UART0.

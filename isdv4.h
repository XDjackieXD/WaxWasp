/*************************************************************
WaxWasp ISDV4 Interface Code
File:    isdv4.h
Author:  Jakob
Licensed under GPL v2.
You should have received a copy of the license with this code.
*************************************************************/

#ifndef __ISDV4_H_
#define __ISDV4_H_

void init_isdv4();
void resetToolState();
void gotSerialByte(unsigned char data);

#endif

/***********************************************************************


 Author        : Johnson Jiao            Johnson.Jiao@analog.com

 Date          : 1st April. 2008

 File          : main.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some function about flash erase/write/read

************************************************************************/
#include <ADuC7026.H>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"

void erase_page(unsigned int addr);
void save(unsigned short addr, unsigned short data);
unsigned short load(unsigned int addr);

void erase_page(unsigned int addr) __ram
{	
	FEEADR = addr;				         // set data address
	FEECON = 0x05;				         // erase page command
	while (!(FEESTA & 0x01)){ }
}

void save(unsigned short addr, unsigned short data) __ram
{
	FEEADR = addr;				// set data address
	FEEDAT = data;				// set data value
	FEECON = 0x02;				// single Write command
	while (!(FEESTA&0x01)){ }
}

unsigned short load(unsigned int addr) __ram
{
	FEEADR = addr;
	FEECON = 0x01;				// single read command
	while (!(FEESTA&0x01)){	}
	return (FEEDAT);		
}






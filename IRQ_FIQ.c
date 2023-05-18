/***********************************************************************
 File          : IRQ_FIQ.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some Functions about IRQ and FIQ

************************************************************************/
#include <ADuC7026.H>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include <stdio.h>

void IRQ_Handler() __irq;
void FIQ_Handler() __fiq;



void IRQ_Handler() __irq __ram
{
	
	unsigned char RX_temp1,temp = 0;
	int i,j = 0;
//	unsigned char CRC_LO, CRC_HI = 0x22;
//	unsigned char buffer0[RS485_BUFFER_SIZE] = {0};

			
	if( COMIID0 & 0x04 )                         // RX IRQ
    {
		if (ascii_to_hex(COMRX) == ':')			// new record
		{
//			delay_ms(1);
			
			Content_start = 0;
			for (i = 0; i < 256; i++) 			// rs485_buffer_size= 256
			{ 
				Package[i] = 0x00;
			}			

			linecount++;
			
			if(lineindex<HEX_LINES) 		{ lineindex++; }
			else 							{ lineindex = lineindex - HEX_LINES; }		
																																				
		}
		else 
		{
			if ((COMRX == '\r') || (COMRX == '\n'))
			{
				temp = COMRX;
				newlineflag = 1;
				GP1DAT ^= 0x00020000;
				
//				delay_ms(50);		 // short delay after each line
			} else {
			RX_temp1 = ascii_to_hex(COMRX)<<4;
			while((COMIID0 & 0x04) != 0x04) {}
			RX_temp1 +=	ascii_to_hex(COMRX);
			Package[Content_start] = RX_temp1;
			newlineflag = 0;
			Content_start++;
//			WDT;
			}
//			if(Content_start>0) { 
				record_length = Package[0]; 
//			}
		} 																	
	}	 
}

void FIQ_Handler() __fiq 
{
 	if( COMIID0 & 0x04 )                         // RX
    {
		RX_Flag = ascii_to_hex(COMRX);
	}

	if((FIQSTA & GP_TIMER_BIT)!= 0)	 		    // Timer1 FIQ?
	{
		GP1DAT ^= 0x00020000;					// Complement P4.2
		T1CLRI = 0;
		timeout = 0xAA;  						// Timeout
		T1CON &= 0xFFFFFF7F;				    // Disable
	} 				
}
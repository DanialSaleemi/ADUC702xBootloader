/***********************************************************************
 File          : IRQ_FIQ.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some Functions about IRQ and FIQ

************************************************************************/
#include <ADuC7026.H>
#include "initCPU.h"
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include "spi.h"
#include "flashSST25VF.h"
#include <stdio.h>

void IRQ_Handler() __irq;
void FIQ_Handler() __fiq;



void IRQ_Handler() __irq __ram
{
	unsigned char RX_temp1, temp, charcount = 0;
	unsigned long offset_addr= 0;

	offset_addr = BOOT_EXT_FLASH_OFFSET;		
														  	
	if( (COMIID0 & 0x04) )                      // RX IRQ
	{	
		while(COMRX != '#')
		{
			while((COMIID0 & 0x04) != 0x04) {}
			if (COMRX == '#') 	{				
				temp = COMRX;	
				break;

			} else if (COMRX == ':') 	{
				temp = COMRX;	
				temp &= 0xFF;			
				write_byte((offset_addr++),temp);
				// while((COMIID0 & 0x04) != 0x04) {}

 			} else if (COMRX == '\n') 	{
				temp = COMRX;
				temp &= 0x0F;	
				write_byte((offset_addr++),temp);
				// while((COMIID0 & 0x04) != 0x04) {}

			} else if (COMRX == '\r') 	{
				temp = COMRX;
				temp &= 0x0F;			
				write_byte((offset_addr++),temp);
				// while((COMIID0 & 0x04) != 0x04) {}	
			
			} else if (
//				(	(COMRX != '#') && (COMRX != ':')	) && 
//				(	(COMRX != '\r') && (COMRX != '\n') 	) &&
				( 	(COMRX >= '0') && (COMRX <= '9')	) ||		  	// check if a valid hex character (0-F) is received.
				(	(COMRX >= 'A') && (COMRX <= 'F')	) || 			// others should be ignored (garbage)
				(	(COMRX >= 'a') && (COMRX <= 'f') 	)				
				){
				
				RX_temp1 = ascii_to_hex(COMRX)<<4;			// receive a character and shift left => 0xDX
				while((COMIID0 & 0x04) != 0x04) {}			// wait for next character
				RX_temp1 +=	ascii_to_hex(COMRX);			// Concatenate next character to the previous one, to form a byte 0xDD
				temp = RX_temp1;
				write_byte(offset_addr, temp); 				//write the byte to external flash
				offset_addr++;
				RX_temp1 &= 0x00;
				temp &= 0x00;
			} else {
				charcount++;
				SendString("Invalid character count: ");
				senddata(charcount);				
				while((COMIID0 & 0x04) != 0x04) {}
			}
		}		
	}
	
}
											

void FIQ_Handler() __fiq 
{
 	if( COMIID0 & 0x04 )                         // RX
    {
		COMIEN0 = 0x05;
		RX_Flag = ascii_to_hex(COMRX);
	}

	if((FIQSTA & GP_TIMER_BIT)!= 0)	 		    // Timer1 FIQ?
	{
		GP1DAT ^= 0x00020000;					// Complement P1.1 To Toggle Red Test LED
		T1CLRI = 0;
		timeout = 0xAA;  						// Timeout
		T1CON &= 0xFFFFFF7F;				    // Disable
	} 				
}
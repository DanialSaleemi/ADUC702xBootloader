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

	
//	return;		
}


			
		
													
		
						

	/*
		if ((COMRX == '\r') || (COMRX == '\n'))			// new line
		{
				
				temp = COMRX;
				delay_ms(10);
				newlineflag = 1;
		}
		else if (ascii_to_hex(COMRX) == ':')			// start of new record
		{
//			delay_ms(1);

			Content_start = 0;
			for (i = 0; i < 256; i++) 			// rs485_buffer_size= 256
			{ 
				Package[i] = 0x00;
			}

//			GP1DAT ^= 0x00020000;										

			linecount++;

																																							
		}
		else 
		{
			RX_temp1 = ascii_to_hex(COMRX)<<4;
			while((COMIID0 & 0x04) != 0x04) {}
			RX_temp1 +=	ascii_to_hex(COMRX);
			Package[Content_start] = RX_temp1;
			newlineflag = 0;
			Content_start++;
//			GP1DAT ^= 0x00020000;
//			WDT;
		}
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//	each hex record is composed of															//
//	:RLOAOARTDDDDDD....DDCSCCCC																//
// 	: = Start character, RL = 1 byte for Record_Length, OA = 2 bytes for Offset_Address,	//
//	DD = Data bytes equal to the number in Record_Length, CS = 1 byte for Checksum,			//
//	CC = 2 bytes of CRC Value (Does not exist by default. Appended in the file to check CRC)//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
 
			record_length = Package[0];
			offset_address = Package[1]<<8 & 0xFF00;
			offset_address += Package[2] & 0xFF;
			record_type = Package[3];
*/			
									

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
/***********************************************************************

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : It is bootloader function, also called In-Application Programming (IAP).
 				 After power on, program wait for upgrading command
				 from host-computer software for 5 seconds
				 If receiving this flag from UART, it will execute
				 upgrade steps. If not, jump 0x00081000 user application
				 area to execute application.

				 Upgrade command: 0x2A


				 Host-Computer Software ----> UART ----> SRAM ----> Flash(0x00081000 ------ 0x0008F7FF)

				 Host-computer Software (any serial console terminal that allows sending full file e.g Hercules, HyperTerminal etc)

************************************************************************/
#include <ADuC7026.h>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include <stdio.h>


#define RX_Flag_UPGRADE 0x2A		// if receive (* or 0x2A), then start upgrade
#define HEX_LINES 0x10		// max number of hex lines to read.
//#define MAX_BUFFER_SIZE 32
//#define RS485_BUFFER_SIZE 512
#define MAX_BUFFER_SIZE 256		// size of buffer to parse data characters from hex file. mostly 32 characters for data
#define RS485_BUFFER_SIZE 256
unsigned int UserAppPTR = 0x00081040;
unsigned short AppStartAddr = 0x1000;				// equal to 0x00081000

unsigned char 	RX_Flag = 0x0; 	 // RX_Flag = 0x0 means no receive rx flag
								 // RX_Flag = 0x2A means receive rx flag

unsigned char 	RX_End=0x0;			//	RX_end=0: continue to receive
									// 	RX_end=1: end

unsigned char 	timeout = 0x0;			// 0 means no timeout of 5 sec
									// 0xAA mean timeout 
unsigned char 	Content_start = 0x0;
unsigned char 	record_type =  0x02;
unsigned char 	record_length= 0xAB;
 
unsigned char 	hex_chksum = 0x10;
unsigned char 	checksum = 0;
unsigned int 	lineindex = 0;				//used in 2D array to store hex data line-wise
unsigned char 	end_of_record_found = 0;	//end_of_record: (record_type=0x01 in Intel hex file)
unsigned char 	line_written = 1;			// flag to indicate when a hex line is written to the flash
unsigned char 	newlineflag = 0;			// flag to indicate a new line character has been received	   
unsigned short 	offset_address = 0;
		 short 	linecount = 0;
unsigned short 	calculated_crc;
unsigned short 	CRC = 0x1234;
int errorcode = 0;

unsigned char 	Package[RS485_BUFFER_SIZE]={0};		// receive package content
unsigned char 	buffer[MAX_BUFFER_SIZE] = {0};		// Data copy from RX receive buffer
unsigned char 	data_array[HEX_LINES][MAX_BUFFER_SIZE] = {0};				//HEX_LINES will be refreshed each time after reaching the set value
        

void InitTimer(void);				// init timer for 5 sec
void Build_INT_Vector_Table(void);
void (*UserAPP)(void);
void UpgradeApp(void);
                   


void main(void)
{
	int i = 5;
	int j = 0;
	// configures GPIO to flash LED P1.1
	GP1DAT |= 0x02000000;			// P1.1 configured as an output. LED is turned on




	InitUART();						// Baudrate of 115200
	InitTimer();					// Init timer1 for 5 second timeout
	FIQEN = UART_BIT + GP_TIMER_BIT;  // Enable UART and Timer1 interrupt in FIQ mode		 
	FEEMOD = 0x8;				// bit 3 should be set to allow erase/write command
	
	// wait for RX flag to upgrade or timeout to jump app

			SendString("\r\n*********************************\r\n\nReady @115200 baud n,8,1 \r\n");
			SendString("\r\nInsert Update Command to enter boot mode or wait 5 seconds for Application\r\n");
    
	while(1)
	{
		if(timeout == 0x00)
		{																
			if(RX_Flag==RX_Flag_UPGRADE)
			{
				FIQEN = 0x0;			// disable all IRQs                          	
				Build_INT_Vector_Table();                    
				REMAP = 0x1;           
				UpgradeApp();			// send a byte of 0x89 to let host-computer to start
										// After upgrade, use software reset to reset ADuC702x
			}
			switch (errorcode)			// Error checking in UpgradeApp function
			{
				case 1:
				SendString("\n\rError: Writing failed due to skipped records, Restarting!\r\n");
				wdt();
				return;
				break;
				case 2:
				SendString("\n\rError: Checksum mismatch, Restarting!\r\n");
				wdt();
				return;
				break;
				case 3:
				SendString("\n\rError: CRC mismatch, Restarting!\r\n");
				wdt();
				return;
				break;
			}

		}
		else
		{
		// Jump to user application code area 0x00081000
			FIQEN = 0x0;					// disable all IRQs
			SendString("\n\r***Timeout! Starting user app***");
			UserAPP = (void (*)(void))UserAppPTR;
			UserAPP();
		}
	}                                      

}

/****************************************************************************/
//
//	 Some functions as below
//
/****************************************************************************/
void InitTimer()
{
	//	40960000 / 32768 * 0x186A =5sec
	T1LD = 0x186A;
	T1CON = 0xCF;						// Enable, Periodic, Binary and CLK/32768

}

void Build_INT_Vector_Table()
{
	volatile unsigned long *Des_VEC, *SOU_VEC;
	int i;
  // Interrupt Vector Table in SRAM
	Des_VEC	= (unsigned long * )0x10000;
  // Interrupt Vector Table in Flash, please refer to Startup.s
  //     AREA   STARTUPCODE, CODE, AT 0x00080000
	SOU_VEC	= (unsigned long * )0x80000;

  // Copy the interrupt Vector Table from Flash to SRAM for 16 instructions
	for (i = 0; i < 16; i++)
	{
		*Des_VEC = *SOU_VEC;
		*Des_VEC++;
		*SOU_VEC++;
	}
}


void UpgradeApp() __ram
{
	unsigned int i,j, data_length = 0; 
//	unsigned int checksumerror, CRCError = 0; 
	unsigned short temp2, temp, errorline = 0;


	SendString("\r\n'Boot mode' Erasing flash...! \r\nPlease wait!");	
	// erase flash from 0x00081000 to 0x0008F7FF

	for(i = 0x1000; i < 0xF800; i += 0x200)
	{
		erase_page(i);
	}
	SendString("\r\nSuccess...! Load hex file\r\n");	// send to host-computer to  handshake

	IRQEN = UART_BIT;
	AppStartAddr = 0x1000;
	errorcode = 0;
	while(RX_End!=0x1)
	{
		while(newlineflag != 1) {}								// wait for newline to start

		if(Content_start>record_length+6)			   			// check for enough data to start parsing
		{
			newlineflag = 0;
//	parsing:

			
//			while(line_written != 0) {}							// wait for line_written flag to go high before start processing new hex line.			 			

			offset_address = Package[1]<<8 & 0xFF00;
			offset_address += Package[2] & 0xFF;
			record_type = Package[3];								
			
//			line_written = 0;									// reset the flag

		if (record_type == 0x04)								// Record start. Start of hex file
		{		 
			linecount = 0;
			lineindex = 0;										// reset the linecount if start of record found
//			checksum *= 0xFF;
			AppStartAddr = 0x1000;
		}								
		else if (record_type == 0x00)							// Record type =0x00: Significant data lines
		{
				checksum=0x00;
				checksum = (record_length + (offset_address >> 8) + (offset_address & 0xFF) + record_type);				
				for (i = 0, j=0; i < record_length; i++, j++)
////////////////////////////////////////////////////////////////////////
//																	  //
//				{													  //
//					data_array[lineindex][i] = Package[j+4];		  //
//					checksum +=	Package[j+4];						  //
//				}													  //
////////////////////////////////////////////////////////////////////////
				{
					buffer[i] = Package[j+4];					
					checksum += buffer[i];						
				}									
///////////////////////////////////////////////////	/												/


				checksum = ~checksum + 1;						// Two's complement
				
				hex_chksum  = Package[record_length+4];

				CRC = Package[(record_length+5)]<<8 & 0xFF00;		// CRC is appended to last two bytes of each record
				CRC += Package[(record_length+6)] & 0xFF;

				data_length = (record_length+1+2+1);			// number of bytes in record_length + 1 byte of record_length value + 2 bytes of offset address value + 1 byte of checksum value
				
				calculated_crc = calculate_crc16(Package, data_length);
						
				if (CRC != calculated_crc)
				{
					errorcode = 3;
					errorline = linecount;
														SendString("\n\n\r Linecount: ");
				senddata(linecount>>12);
				senddata(linecount>>8);
				senddata(linecount>>4);
				senddata(linecount);
				delay_ms(10);
					return;			
								
				} else {
				
					if (hex_chksum != checksum)
					{
						errorcode = 2;
						errorline = linecount;
															SendString("\n\n\r Linecount: ");
				senddata(linecount>>12);
				senddata(linecount>>8);
				senddata(linecount>>4);
				senddata(linecount);

				delay_ms(10);

						return;	

					}
					else {							
							temp2 = linecount;										

							for (i = 0; i < record_length; i++)
							{
								data_array[lineindex][i] = buffer[i];		   	// 2d array
							}
							 
						while((ascii_to_hex(COMRX)!= ':') ){};					// wait for ':' character to indicate new line									
//						if (ascii_to_hex(COMRX)== ':') { temp2++; }
						temp2++;	

							for (i = 0; i < record_length; i +=2)
							{
								if(linecount != temp2) 								
								{
									errorcode = 1;
									errorline = linecount;
									delay_ms(1);
									return;
//									SendString("\n\n\r Linecount: ");
//				senddata(linecount>>12);
//				senddata(linecount>>8);
//				senddata(linecount>>4);
//				senddata(linecount);
//									return;
//									//goto parsing;									
//									SendString("Error code 1");
//									return;
//									if (lineindex>0x00) { 
//										lineindex--; 
//}
//									else { lineindex = 0x00; }
//									linecount--;
//									AppStartAddr -= 2*record_length;
	//								goto parsing;
								} else
								{
										temp = (data_array[lineindex-1][i]) + ((data_array[lineindex-1][i+1])<<8);			// lineindex has already been incremented for next record								
										save(AppStartAddr,temp);						// write to flash memory from the starting address (offset) of user application 
										AppStartAddr += 2;
									   /*
										senddata(temp>>4);
										senddata(temp);
										senddata(temp>>12);
										senddata(temp>>8);
										*/
								}
							}
//							delay_ms(50);

							
//							line_written = 1;				// flag set to indicate the current hex line is written to the memory							
						}									
				}
/*
				SendString("\n\n\r Linecount: ");
				senddata(linecount>>12);
				senddata(linecount>>8);
				senddata(linecount>>4);
				senddata(linecount);

				SendString("\n\n\r Checksum Calculated: ");
				senddata(checksum>>4);
				senddata(checksum);

				SendString("\n\r Checksum Received: ");
				senddata(hex_chksum>>4);
				senddata(hex_chksum);

				SendString("\n\n\r Calculated CRC: ");
				senddata(calculated_crc>>12);
				senddata(calculated_crc>>8);
				senddata(calculated_crc>>4);
				senddata(calculated_crc);
				
				SendString("\n\r Temp2: ");
				senddata(temp2>>12);
				senddata(temp2>>8);
				senddata(temp2>>4);
				senddata(temp2);
				
				SendString("\n\r Received CRC: ");
				senddata(CRC>>12);
				senddata(CRC>>8);
				senddata(CRC>>4);
				senddata(CRC);


				SendString("\n\r Line Index: ");
				senddata(lineindex>>4);
				senddata(lineindex);
				SendString("\n\r");
/*
				SendString("\n\rApp Start Addr: ");
				senddata(AppStartAddr>>12);
				senddata(AppStartAddr>>8);
				senddata(AppStartAddr>>4);
				senddata(AppStartAddr);
*/				
		}																				 	
							
		else if (record_type == 0x01)									// Record end. End of hex file
		{
			if (errorcode != 0)
			{
							SendString("\r\nError at line: ");					
			senddata(errorline>>12);
			senddata(errorline>>8);
			senddata(errorline>>4);
			senddata(errorline);

//			SetRs485RxDir;
				return;			
			} 
			else {
						SendString("\r\nNumber of Lines Written Succesfully: ");					
			senddata(linecount>>12);
			senddata(linecount>>8);
			senddata(linecount>>4);
			senddata(linecount);

			SetRs485RxDir;

			
//			end_of_record_found = 1;
//			linecount = 0;
			RX_End = 0x1;								// Exit while loop	
			}
		}				
		else {
				SendString("\n\rInvalid record type\n\r");
			}
	} 
}
	SendString("\r\nUpdate finished \r\nRestarting...!");	
	SET_LED_TEST_OFF;				// LED is turned off, indicate Upgrade Finish!!!
	i=0x1000;
	while(i>0)i--;					// short delay!
	RSTSTA = 0x04;					// software reset
	RSTSTA = 0x04;					// software reset
	while(1);		
}
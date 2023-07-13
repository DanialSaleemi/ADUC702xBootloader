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
#include "spi.h"
#include "flashSST25VF.h"
#include "initCPU.h"
#include <stdio.h>


#define RX_Flag_UPGRADE 0x2A			// if receive (* or 0x2A), then start upgrade
#define MAX_BUFFER_SIZE 80				// size of buffer to parse hex record
unsigned int UserAppPTR = 0x00082040;
unsigned short AppStartAddr = 0x2000;				// equal to 0x00082000

unsigned char 	RX_Flag = 0x0; 	 // RX_Flag = 0x0 means no receive rx flag
								 // RX_Flag = 0x2A means receive rx flag

unsigned char 	RX_End=0x0;			//	RX_end=0: continue to receive
									// 	RX_end=1: end

unsigned char 	timeout = 0x0;			// 0 means no timeout of 5 sec
									// 0xAA mean timeout 

int hexchar = 0;

unsigned long 	addr_offset;
unsigned char 	record_type =  0x03; 			// initialized with non-zero value
unsigned char 	record_length= 0xAB;			// initialized with non-zero value
unsigned short 	hex_offset_address = 0xF000;	// initialized with non-zero value
 
unsigned char 	hex_chksum = 0x10;
unsigned char 	checksum = 0;

unsigned short 	offset_address = 0;
unsigned short 	calculated_crc;
unsigned short 	CRC = 0x1234;

unsigned char 	Package[MAX_BUFFER_SIZE]={0};		// receive package content
//unsigned char	RS485_BUFFER[RS485_BUFFER_SIZE]={0};
//unsigned char 	buffer[MAX_BUFFER_SIZE] = {0};		// Data copy from RX receive buffer
//unsigned char 	data_array[HEX_LINES][MAX_BUFFER_SIZE] = {0};				//HEX_LINES will be refreshed each time after reaching the set value
        

void InitTimer(void);				// init timer for 5 sec
void Build_INT_Vector_Table(void);
void InitCPU(void);
void (*UserAPP)(void);
void UpgradeApp(void);
                   


void main(void)
{
 	InitCPU();
	
	// wait for RX flag to upgrade or timeout to jump app

			SendString("\r\n*********************************\r\n\nReady @115200 baud n,8,1 \r\n");
			SendString("\r\nInsert Update Command to enter boot mode or wait 5 seconds for User Application\r\n");
    
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
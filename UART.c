/***********************************************************************

 File          : UART.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some function about UART

************************************************************************/
#include <ADuC7026.H>
#include "initCPU.h"
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include "flashSST25VF.h"

void InitUART(void);				// Baudrate of 115200,n,8,1
char hex2ascii(unsigned char toconv);        // HEX to ASCII
char ascii_to_hex(char c);			// ASCII to HEX
void senddata(unsigned char to_send);	    // send out one CHAR
void sendbyte(unsigned char to_send);		// send out one byte
void SendString (char *string);		// Send out a string
short calculate_crc16(unsigned char *data, unsigned int length); //crc calculation
// void RS485_BUFF_FLUSH(void);



void InitUART()				 // RS485
{
	// Setup Rx & Tx pins on SPM 8(P0.7) and SPM 9(P2.0) respectively
	GP0CON |= 0x20000000;	 	// RS485_RX
	GP0DAT |= 0x7E780000;		// Setting P0.1 & P0.2 for RE'_RS485 & DE_RS485
	GP2CON |= 0x00000002;		// RS485_TX
	GP2DAT |= 0x05000000;

   	COMCON0 = 0x080;			// Setting DLAB
	COMDIV0=0x0B;           	// COMDIV0 -> 2, Set baud rate to 115206 (use of fractionnal baud rate mode) (from aduc/hardware.c)
   	COMDIV1 = 0x00;		    
	COMDIV2=0x883E;         	// M=1  N=62      FBEN=1    => 0x883E	(from aduc/hardware.c)
	COMCON0=0x03;				// (from aduc/hardware.c) Clearing DLAB, 8 bits, 1 Stop bit, Parity None

	COMIEN0=0x05;				// Enable Rx Status Interrupt
								// Enable Receive Buffer Full Interrupt
	SetRs485RxDir;
}

char hex2ascii(unsigned char toconv) __ram
{
								// 0-F character
	
	if (toconv<0x0A) {
		toconv += 0x30;
		hexchar = 1;
		return (toconv);
	} else if ( (toconv<0x10) && (toconv>0x09) ) {		
		toconv += 0x37;
		hexchar = 1;
		return (toconv);
	} else if ( (toconv == ':') || (toconv == '\n') || (toconv == '\r')) {
		hexchar = 0;
		return (toconv);
	} else {
		hexchar = 0;
		return;
	}	
}

char ascii_to_hex(char toconv) __ram 
{
    if (toconv >= '0' && toconv <= '9') {
//		hexchar = 1;
        return (toconv - '0');

    } else if (toconv >= 'A' && toconv <= 'F') {
//      hexchar = 1;
		return (toconv - 'A' + 10);

    } else if (toconv >= 'a' && toconv <= 'f') {
//        hexchar = 1;
		return (toconv - 'a' + 10);

    } else if (toconv == '*') {
//		hexchar = 0;
		return (toconv);
		
	} else if (toconv == ':') {
//		hexchar = 0;
		return (toconv);
	}  
	else {
		return 0;
	}
}

void senddata(unsigned char to_send) __ram 	   // send out one CHAR
{	
	COMIEN0 = 0x2;
	SetRs485TxDir;
	delay_ms(5);
	while(!(0x020==(COMSTA0 & 0x020))){}
//	if (!hexchar)  { COMTX = hex2ascii(to_send); }
//	else {
		COMTX = hex2ascii(to_send & 0x0F);
//	} 
	delay_ms(1);						// short delay		
	SetRs485RxDir;						// Set Rx Direction for RS485	
	COMIEN0 = 0x05;						// UART Rx Interrupt
	delay_ms(1);						// short delay		
}

void sendbyte(unsigned char to_send) __ram
{
	senddata(to_send>>4);
	senddata(to_send);	
}

void SendString(char *string) __ram
{
	
    delay_ms(10);
	SetRs485TxDir;							// Set Rx Direction for RS485
	COMIEN0 = 0x2;							// UART Tx Interrupt

	while (*string != '\0')
    {
        while ((COMSTA0 & 0x20) == 0);
        COMTX = *string;
        string++;
    }
		delay_ms(1);						// short delay		
		SetRs485RxDir;						// Set Rx Direction for RS485	
		COMIEN0 = 0x05;						// UART Rx Interrupt

}

// CRC-16 calculation function

short calculate_crc16(unsigned char *data, unsigned int length) __ram
{
	int i,j = 0;
    unsigned int crc = 0xFFFF;
    for (i = 0; i < length; i++) {
        crc ^= (unsigned int)data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/*
void wdt (void)
{
		// Timer3 WDT configuration
	T3LD  = 0x2000;			  			// (1/32768)*8192 			= 250ms
//	T3LD  = 0x4000;			  			// (1/32768)*16384 			= 500ms
	T3CON = 0xE0;    	  				// ->WDT
	T3CLRI = 0xFF;	
}
*/

// Flush the RS485 Buffer to receive new data
/*
void RS485_BUFF_FLUSH(void)							 
{
	int i=0;
//	for(i=0;i<RS485_BUFFER_SIZE; i++)		{ RS485_BUFFER[i] = 0; }
//	Content_start = 0;
	SetRs485RxDir;
	COMIEN0=0x05;				// enable ELSI,ERBFI
//	toggle_WLED;
	delay_ms(20);
//	toggle_WLED;
//	for (i=0; i<5; i++) {	SET_GLED_ON_OFF();	}
}
*/




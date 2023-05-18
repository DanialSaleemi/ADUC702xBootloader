/***********************************************************************

 File          : UART.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some function about UART

************************************************************************/
#include <ADuC7026.H>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"

void InitUART(void);				// Baudrate of 115200,n,8,1
char hex2ascii(char toconv);        // HEX to ASCII
char ascii_to_hex(char c);			// ASCII to HEX
void senddata(char to_send);	    // send out one CHAR
void SendString (char *string);		// Send out a string
void delay(int i);					// not being used
void delay_ms(unsigned int count);	// delay in ms
short calculate_crc16(unsigned char *data, unsigned int length); //crc calculation




void InitUART()				 // RS485
{
	// Setup Rx & Tx pins on SPM 8(P0.7) and SPM 9(P2.0) respectively
	GP0CON |= 0x20000000;	 	// RS485_RX
	GP0DAT |= 0x7E780000;		// Setting P0.1 & P0.2 for RE'_RS485 & DE_RS485
	GP2CON |= 0x00000002;		// RS485_TX
	GP2DAT |= 0x05000000;

   	COMCON0 = 0x080;			// Setting DLAB
	COMDIV0=0x0B;           // COMDIV0 -> 2, Set baud rate to 115206 (use of fractionnal baud rate mode) (from aduc/hardware.c)
   	COMDIV1 = 0x00;		    
	COMDIV2=0x883E;         // M=1  N=62      FBEN=1    => 0x883E	(from aduc/hardware.c)
	COMCON0=0x03;			// (from aduc/hardware.c) Clearing DLAB, 8 bits, 1 Stop bit, Parity None


	COMIEN0=0x05;				// Enable Rx Status Interrupt
								// Enable Receive Buffer Full Interrupt
	SetRs485RxDir;
}

char hex2ascii(char toconv) __ram
{
	
	if (toconv<0x0A) {
		toconv += 0x30;
		return (toconv);
	} else if ( (toconv<0x10) && (toconv>0x09) ){		
		toconv += 0x37;
		return (toconv);
	} else {
		return -1;
	}	
}

char ascii_to_hex(char c) __ram 
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c == '*') {
		return c;
	} else if (c == ':') {
		return c;
	}  
	else {
		return -1;
	}
}

void senddata(char to_send) __ram 	   // send out one CHAR
{
	delay_ms(5);
	SetRs485TxDir;
	while(!(0x020==(COMSTA0 & 0x020))){}
	COMTX = hex2ascii(to_send & 0x0F);
	delay_ms(5);						// short delay		
	SetRs485RxDir;						// Set Rx Direction for RS485	
}

void SendString(char *string) __ram
{
	delay_ms(5);
	SetRs485TxDir;							// Set Rx Direction for RS485
    while (*string != '\0')
    {
        while ((COMSTA0 & 0x20) == 0);
        COMTX = *string;
        string++;
    }
		delay_ms(5);						// short delay		
		SetRs485RxDir;						// Set Rx Direction for RS485	
}


void delay (int i) __ram
{
// 	int i=10000;
	while(i>0)i--;					// short delay!
}
void delay_ms(unsigned int count) __ram
{
    unsigned int i;
    for (i = 0; i < count; i++)
    {
        unsigned int j;
//		GP1DAT ^= 0x00020000;

        for (j = 0; j < (32768 / 1152) - 1; j++)
        {
            __asm nop; // A no-operation instruction to waste a cycle
        }
    }
}

void wdt (void)
{
		// Timer3 WDT configuration
	T3LD  = 0x2000;			  			// (1/32768)*8192 			= 250ms
//	T3LD  = 0x4000;			  			// (1/32768)*16384 			= 500ms
	T3CON = 0xE0;    	  				// ->WDT
	T3CLRI = 0xFF;	
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




#include "spi.h" 

void InitSPI(void)
{
	GP1DAT |= 0xDFFC0000;   				// !!! SPI in common exept cs !!!	      // aduc/hardware.c
	GP1CON |= 0x02220000;					// SPI pin configuration (P1.4 to P1.7)	  // aduc/hardware.c
	SPICON = 0x134F;						// enable SPI master mode				  // aduc/hardware.c
	SPIDIV = 0x05;							// 3.482 MHz							  // aduc/hardware.c


    write_Status_Register_EXT_FLASH(0);
}						   

unsigned char SPI(unsigned char data) __ram		// send character over SPI
{
	SPITX = data;								// send character						SSPBUF = d; 
	do {} while ((SPISTA & 0x01) == 0x01) ;		// wait until sent	   					(!BF)
	do {} while ((SPISTA & 0x08) != 0x08) ;
	return (SPIRX);								// and return the received character  	SSPBUF
}
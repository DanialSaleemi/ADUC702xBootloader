/***********************************************************************


 Author        : Johnson Jiao            Johnson.Jiao@analog.com

 Date          : 1st April. 2008

 File          : main.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about UART functions
************************************************************************/

#define SetRs485RxDir		GP0CLR = 0x00040000;GP0CLR = 0x00020000;
#define SetRs485TxDir		GP0SET = 0x00020000;GP0SET = 0x00040000;


extern void InitUART(void);					// Baudrate of 115200, N,8,1
extern char hex2ascii(unsigned char toconv);        	// HEX to ASCII
extern void senddata(unsigned char to_send);	    	// send out one CHAR
extern void sendbyte(unsigned char to_send);            // send out one Byte
extern char ascii_to_hex(char c);			// ASCII to HEX
extern void SendString (char *string);		// send out string
extern short calculate_crc16(unsigned char *data, unsigned int length);
//extern void delay(int i);					// short delay
//extern void	wdt(void);						// watch dog timer
// extern void RS485_BUFF_FLUSH(void);

/***********************************************************************
 File          : main.h

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about global variables
************************************************************************/

#define HEX_LINES 0x10
//#define MAX_BUFFER_SIZE 32
#define MAX_BUFFER_SIZE 256
//#define RS485_BUFFER_SIZE 512
#define RS485_BUFFER_SIZE 256

#define SET_LED_TEST_ON		GP1SET |= 0x00020000;		// Set P1.1
#define SET_LED_TEST_OFF	GP1CLR |= 0x00020000;		// Clear P1.1

extern unsigned int UserAppPTR;
extern unsigned short AppStartAddr;				// equal to 0x00081000

extern unsigned char RX_Flag; 	 	// RX_Flag = 0x0 means no receive rx flag
								 	// RX_Flag = 0xD means receive rx flag

extern unsigned char RX_End;		//	RX_end=0: continue to receive
									// 	RX_end=1: end

extern unsigned char timeout;		// 0 means no timeout of 5 sec
									// 0xAA mean timeout 

extern unsigned char 	Content_start;
extern unsigned char 	record_type;
extern unsigned char 	record_length;
extern unsigned char 	hex_chksum;					// checksum value received from the serial
extern unsigned char 	line_written;				// flag to indicate when a hex line is written to the flash
extern unsigned int 	lineindex;					// lineindex to count number of lines, read from hex file; used in 2d array
extern unsigned char 	newlineflag;			// flag to indicate a new line character has been received	   


extern unsigned short 	offset_address;
extern 			short 	linecount;
extern unsigned short 	calculated_crc;
extern unsigned short 	CRC;

extern unsigned char 	Package[];		// receive package content
extern unsigned char 	buffer[];
extern unsigned char 	data_array[HEX_LINES][MAX_BUFFER_SIZE];


extern void InitTimer(void);				// init timer for 5 sec
extern void Build_INT_Vector_Table(void);
extern void (*UserAPP)(void);
extern void UpgradeApp(void);
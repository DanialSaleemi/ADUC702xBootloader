/***********************************************************************
 File          : main.h

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about global variables
************************************************************************/

//#define HEX_LINES 0x10
//#define MAX_BUFFER_SIZE 32
#define MAX_BUFFER_SIZE 80
//#define RS485_BUFFER_SIZE 512
//#define RS485_BUFFER_SIZE 512
								  
#define SET_LED_TEST_ON		GP1SET |= 0x00020000;		// Set P1.1
#define SET_LED_TEST_OFF	GP1CLR |= 0x00020000;		// Clear P1.1
#define SET_GLED_ON			GP4SET |= 0x00040000;		// Set P4.2
#define SET_GLED_OFF		GP4CLR |= 0x00040000;		// Clear P4.2
#define SET_WLED_ON			GP4SET |= 0x00080000;		// Set P4.3
#define SET_WLED_OFF		GP4CLR |= 0x00080000;		// Clear P4.3
#define toggle_WLED			GP4DAT ^= 0x00080000;		// Complement P4.3
#define toggle_GLED			GP4DAT ^= 0x00040000;		// Complement P4.2
#define toggle_RLED			GP1DAT ^= 0x00020000;		// Complement P1.1


#define WDT		T3CLRI = 0xFF;

extern unsigned int UserAppPTR;
extern unsigned short AppStartAddr;				// equal to 0x00082000

extern unsigned char RX_Flag; 	 	// RX_Flag = 0x0 means no receive rx flag
								 	// RX_Flag = 0xD means receive rx flag

extern unsigned char RX_End;		//	RX_end=0: continue to receive
									// 	RX_end=1: end

extern unsigned char timeout;		// 0 means no timeout of 5 sec
									// 0xAA mean timeout 
extern int hexchar;
//extern unsigned int 	Content_start;
extern unsigned char 	record_type;
extern unsigned char 	record_length;
extern unsigned char 	checksum;
extern unsigned char 	hex_chksum;					// checksum value received from the serial
//extern unsigned char 	line_written;				// flag to indicate when a hex line is written to the flash
//extern unsigned int 	lineindex;					// lineindex to count number of lines, read from hex file; used in 2d array
//extern unsigned char 	newlineflag;			// flag to indicate a new line character has been received	   


extern unsigned short 	hex_offset_address;
//extern 			short 	linecount;
extern unsigned short 	calculated_crc;
extern unsigned short 	CRC;

//extern unsigned char 	RS485_BUFFER[];
extern unsigned char 	Package[];		// receive package content
//extern unsigned char 	data_array[HEX_LINES][MAX_BUFFER_SIZE];

//extern unsigned char RX_temp1;
extern unsigned long addr_offset;


//extern void InitTimer(void);				// init timer for 5 sec
//extern void Build_INT_Vector_Table(void);
extern void (*UserAPP)(void);
extern void UpgradeApp(void);
//extern void SET_TEST_LED_ON_OFF(void);
//extern void SET_GLED_ON_OFF(void);
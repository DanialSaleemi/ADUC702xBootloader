/***********************************************************************
 File          : main.h

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about global variables
************************************************************************/

#define MAX_BUFFER_SIZE 80

								  
#define SET_LED_TEST_ON		GP1SET |= 0x00020000;		// Set P1.1
#define SET_LED_TEST_OFF	GP1CLR |= 0x00020000;		// Clear P1.1
#define SET_GLED_ON			GP4SET |= 0x00040000;		// Set P4.2			(Eval board)
#define SET_GLED_OFF		GP4CLR |= 0x00040000;		// Clear P4.2		(Eval board)
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
extern int hexchar;					// used to check if a received character is a hexadecimal number or not

extern unsigned char 	record_type;				// type of each hex record (00 for 'data', 04 for 'start', 01 for 'end')
extern unsigned char 	record_length;				// length of each hex line/record
extern unsigned char 	checksum;					// checksum value computed for each record
extern unsigned char 	hex_chksum;					// checksum value received for each record


extern unsigned short 	hex_offset_address;

extern unsigned short 	calculated_crc;
extern unsigned short 	CRC;

extern unsigned char 	Package[];		// receive hex package content from external flash



extern unsigned long addr_offset;


extern void (*UserAPP)(void);
extern void UpgradeApp(void);

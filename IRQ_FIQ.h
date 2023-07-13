/***********************************************************************


 Author        : Johnson Jiao            Johnson.Jiao@analog.com

 Date          : 1st April. 2008

 File          : main.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about IRQ and FIQ function
************************************************************************/
//#define RS485_BUFFER_SIZE 512
//#define MAX_BUFFER_SIZE 2048

//extern unsigned char Package[MAX_BUFFER_SIZE];
//extern unsigned char RX_temp1;
//extern unsigned long offset_addr;

extern void IRQ_Handler() __irq;
extern void FIQ_Handler() __fiq;

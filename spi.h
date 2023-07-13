

#ifndef SPI_H
#define SPI_H			  

#include <ADuC7026.h>

void InitSPI(void);
unsigned char SPI(unsigned char);
void write_Status_Register_EXT_FLASH(unsigned char);

#define FLASH_DESELECT		GP1SET = 0x00800000;	
#define FLASH_SELECT		GP1CLR = 0x00800000;
#define FPGA_DESELECT		GP2SET = 0x00040000;
#define FPGA_SELECT			GP2CLR = 0x00040000;

#endif
/*
    @author Sebastien De Dijcker
    @author Marc Belenger
    @version 0.1 20/07/2015
    @version 0.71 16/03/2016
    @version 0.72 29/03/2016
    @version 1.0 12/04/2016 
    @version 1.1 18/04/2016
    @version 1.2 20/04/2016
    @version 1.3 22/04/2016
    @version 1.4 28/04/2016
    @version 1.5 27/05/2016
    @further versions: tracked with revision control
*/

#ifndef FLASHSST25VF_H
#define FLASHSST25VF_H

#include <ADuC7026.h>
#include "main.h"
#include "spi.h" 

#define BOOT_EXT_FLASH_OFFSET		0x1000
#define BUSY						1


unsigned char read_byte(unsigned long );
void high_speed_read(unsigned long, unsigned char *, unsigned short int);
void write_byte(unsigned long, unsigned char );
unsigned char read_Busy_EXT_FLASH(void);
void erase_4K_Page(unsigned long);
extern void write_Status_Register_EXT_FLASH(unsigned char);
//unsigned char read_Status_Register_EXT_FLASH();
//unsigned char read_Status_Register1_EXT_FLASH();
void writeEnable();
void writeRegisterEnable();
void chipErase(void);


#endif
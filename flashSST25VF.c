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

#include "flashSST25VF.h"
#include "UART.h"
#include "main.h"
#include "initCPU.h"

unsigned char read_byte(unsigned long add) __ram
{
	unsigned char ret;
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;

	SPI(0x03); 					//HIGH SPEED INSTRUCTION						
	SPI((add&0x00FF0000)>>16); 	//ADDRESS			 
	SPI((add&0x0000FF00)>> 8); 	//ADDRESS		
	SPI((add&0x000000FF)>> 0); 	//ADDRESS		
	ret = SPI(0x00);
	FLASH_DESELECT;
//	toggle_GLED;
	return ret;
}

void high_speed_read(unsigned long add, unsigned char *data, unsigned short int size)
{
	unsigned int i = 0;
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;

	SPI(0x0B); 					//HIGH SPEED INSTRUCTION						
	SPI((add&0x00FF0000)>>16); 	//ADDRESS			 
	SPI((add&0x0000FF00)>> 8); 	//ADDRESS		
	SPI((add&0x000000FF)>> 0); 	//ADDRESS		
	SPI(0x00); 					//DUMMY BYTE

	for(i = 0; i < size; i++)
	{
		data[i] = SPI(0x00);
		WDT;
//		toggle_GLED;
	}
	FLASH_DESELECT;

}

void write_byte(unsigned long add, unsigned char value) __ram
{
	writeEnable();	
	FLASH_DESELECT;		
	FPGA_DESELECT;
	FLASH_SELECT;
	SPI(0x02); 					
	SPI((add&0x00FF0000)>>16); 	
	SPI((add&0x0000FF00)>> 8); 	
	SPI((add&0x000000FF)>> 0); 
	SPI(value);	 
	FLASH_DESELECT;		
	while (read_Busy_EXT_FLASH()==BUSY);
	toggle_RLED;							// Toggles Red led on p1.1
}


unsigned char read_Busy_EXT_FLASH(void) __ram	  	// 1=busy
{
	unsigned char value;
	
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;
	SPI(0x05) ;
	value=SPI(0x00);									
	FLASH_DESELECT;						
	return(value&0x01);
}

void erase_4K_Page(unsigned long start_add_page)
{
//	int i;	
	writeEnable();							
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;
	SPI(0x20); 	
	SPI((start_add_page&0x00FF0000)>>16); 	
	SPI((start_add_page&0x0000F000)>> 8); 	
	SPI(0); 
		   
	while (read_Busy_EXT_FLASH()==BUSY);
	FLASH_DESELECT;
//	WDT;	  
} 

void chipErase(void) __ram  //WRITE
{
//	int i;	
	writeEnable();
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;
	SPI(0x60); 
	FLASH_DESELECT;
	while (read_Busy_EXT_FLASH()==BUSY){WDT;};
}


void writeEnable() __ram
{
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;
	SPI(0x06); 
	FLASH_DESELECT;
//	WDT;
}

void writeRegisterEnable() __ram
{
	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;
	SPI(0x50); 
	FLASH_DESELECT;
	WDT;
}

void write_Status_Register_EXT_FLASH(unsigned char value)
{
	writeRegisterEnable();	

	FPGA_DESELECT;
	FLASH_DESELECT;
	FLASH_SELECT;
	SPI(0x01); 
	SPI(value);
	FLASH_DESELECT;	  
	while (read_Busy_EXT_FLASH()==BUSY);
}
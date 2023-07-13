#include <ADuC7026.H>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include "spi.h"
#include "flashSST25VF.h"
#include "initCPU.h"
#include <stdio.h>

int parse_hex(void);
void read_hex_line(unsigned long, unsigned char *);
void write_mcu_flash (unsigned char *, unsigned char, unsigned short);

void read_hex_line_from_ext_flash(unsigned long off_addr, unsigned char *data) __ram
{
	int i = 0;
	unsigned char temp1, temp2, temp3;
	
	while(1)
	{
		temp1 = read_byte(off_addr);
		temp2 = read_byte(off_addr+1);
		temp3 = read_byte(off_addr+2);
		if( (temp1 == '\r') && (temp2 == '\n') && (temp3 == ':') )			// sequence to verify to end/start of hex record, that is; CRLF(\r\n) and ':' for new record
		{
			off_addr += 2;
			break;
		} else {
			data[i] = temp1;
			delay_ms(1);
			i++;
			off_addr++;
		}
	}
}	


void write_mcu_flash (unsigned char *hex_data, unsigned char length, unsigned short address) __ram
{
	int i = 0;
	unsigned short data_word = 0;
	unsigned short memread = 0;
	AppStartAddr = address;
	for(i=0; i<length; i+=2)
	{
		data_word = hex_data[i] + (hex_data[i+1]<<8);
		save(AppStartAddr,data_word);						// write to flash memory from the starting address (offset) of user application 
		AppStartAddr += 2;


		/* debug print */
/*
		memread = load(AppStartAddr);
		sendbyte(memread);
		sendbyte(memread>>8);
		SendString("\n\rOffset Address");
		sendbyte(AppStartAddr>>8);
		sendbyte(AppStartAddr);
*/		
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//	parse_hex;																				//
//	each hex record is composed as following												//
//	:RLOAOARTDDDDDD....DDCSCCCC																//
// 	: = Start character, RL = 1 byte for Record_Length, OA = 2 bytes for Offset_Address,	//
//	RT = 1 byte for Record_Type, DD = Data bytes equal to the number in Record_Length,		//
//	CS = 1 byte for Checksum,																//
//	CC = 2 bytes of CRC Value (Does not exist by default. Appended in the file to check CRC)//
//	Only record type = 0x00 is implemented													//
//////////////////////////////////////////////////////////////////////////////////////////////


int parse_hex(void) __ram
{
	unsigned char temp = 0;
	unsigned char buffer[16] = {0};
	int i,data_length = 0;
	
	
	if ((temp = read_byte(addr_offset)) == ':')
	{
		read_hex_line_from_ext_flash(addr_offset, Package);
	} else {
		while((temp = read_byte(addr_offset++)) != '\r');
		while((temp = read_byte(addr_offset++)) != '\n');
		while((temp = read_byte(addr_offset++)) != ':');
		read_hex_line_from_ext_flash(addr_offset, Package);
	}

	
	record_length = Package[0];

	hex_offset_address = Package[1]<<8 & 0xFF00;
	hex_offset_address += Package[2]	& 0xFF;

	record_type = Package[3];

	checksum = (record_length + (hex_offset_address >> 8) + (hex_offset_address & 0xFF) + record_type);

	/*	debug print	*/
/*
	SendString("\n\r Record Length: ");
	sendbyte(record_length);
	
	SendString("\n\r Offset Address: ");
	sendbyte(hex_offset_address>>8);
	sendbyte(hex_offset_address);
	
	SendString("\n\r Record Type: ");
	sendbyte(record_type);
	
	SendString("\n\rRecord data: ");
*/
	for(i = 0; i<record_length; i++)
	{
		buffer[i] = Package[i+4];
		checksum += buffer[i];
		/* debug print */
		/* sendbyte(buffer[i]); */
	}
	checksum = ~checksum + 1;						// two's complement

	hex_chksum = Package[record_length+4];

	CRC = Package[(record_length+5)]<<8 & 0xFF00;	// CRC is appended to last two bytes of each hex record
	CRC += Package[(record_length+6)] & 0xFF;		// CRC received from hex file

	data_length = (record_length+1+2+1);			// number of bytes (record_length) + 1 byte of record_length + 2 bytes of offset address + 1 byte of checksum				
	calculated_crc = calculate_crc16(Package, data_length); // CRC calculated on received bytes


	/*	debug Print	*/
/*	
	SendString("\n\rReceived CRC: ");
	sendbyte(CRC>>8);
	sendbyte(CRC);	

	SendString("\n\rCalculated CRC: ");
	sendbyte(calculated_crc>>8);
	sendbyte(calculated_crc);

	SendString("\n\rError: Computed Checksum: ");
	sendbyte(checksum);

	SendString("\n\rError: Received Checksum: ");
	sendbyte(hex_chksum);
*/

	if (checksum == hex_chksum)
	{
		if (CRC == calculated_crc)
		{	
			write_mcu_flash(buffer, record_length, hex_offset_address);
			return 1;
		} 
		else 
		{	
			SendString("\n\rCRC Error");		
			return 0;
		}
	} else {
			SendString("\n\rChecksum Error");
			return 0;
	}
}

void UpgradeApp() __ram
{
	int i, ret;
	unsigned char temp =0;	

	SendString("\r\n'Boot mode' Erasing flash...! \r\nPlease wait!");	
	// erase flash from 0x00081000 to 0x0008F7FF

	chipErase();							// external flash, complete chip erase
	for(i = 0x2000; i < 0xF800; i += 0x200)
	{
		erase_page(i);						// ADuC's flash -> erase pages from 0x2000 to 0xF800, this space shall be used for bootloader
	}
	SendString("\r\nSuccess...! Load hex file\r\n");	// send to host-computer to  handshake

	IRQEN = UART_BIT;									// UART Interrupt
	while(COMRX != '#') {}
	addr_offset = BOOT_EXT_FLASH_OFFSET;
	IRQEN = UART_BIT;
	COMIEN0 = 0x00;	 									// clear UART interrupts
while(record_type != 0x01)
	{
		if(record_type != 0x01)
		{
			ret = parse_hex();							// parses hex records and calls write function to write ADuC flash line by line
			if(ret)
			{
				toggle_RLED;
			} else {
				SendString("\n\rTarget not updated!");
				RSTSTA = 0x04;					// software reset
				RSTSTA = 0x04;					// software reset
			}
		} else {break;}
	} 
 
			
	SendString("\r\nSuccess: Update finished...! \n\rRestarting...!");	
	i=0x1000;
	while(i>0)i--;					// short delay!
	RSTSTA = 0x04;					// software reset
	RSTSTA = 0x04;					// software reset
	while(1);		
}
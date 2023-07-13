#include <ADuC7026.H>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include "spi.h"
#include "flashSST25VF.h"
#include "initCPU.h"
#include <stdio.h>

void parse_hex(void);
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
		if( (temp1 == '\r') && (temp2 == '\n') && (temp3 == ':') )
		{
			off_addr += 2;
			break;
		} else {
			data[i] = temp1;
//			sendbyte(data[i]);
			// senddata(data[i]>>4);
			// senddata(data[i]);
			delay_ms(1);
			i++;
			off_addr++;
		}
	}
}	

//////////////////////////////////////////////////////////////////////////////////////////////
//	parse_hex;																				//
//	each hex record is composed of															//
//	:RLOAOARTDDDDDD....DDCSCCCC																//
// 	: = Start character, RL = 1 byte for Record_Length, OA = 2 bytes for Offset_Address,	//
//	RT = 1 byte for Record_Type, DD = Data bytes equal to the number in Record_Length,		//
//	CS = 1 byte for Checksum,																//
//	CC = 2 bytes of CRC Value (Does not exist by default. Appended in the file to check CRC)//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

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
//		delay_ms(100);
//		memread = load(AppStartAddr);
//		sendbyte(memread);
//		sendbyte(memread>>8);
//		SendString("\n\rOffset Address");
//		sendbyte(AppStartAddr>>8);
//		sendbyte(AppStartAddr);
		AppStartAddr += 2;
	}
}

void parse_hex(void) __ram
{
	unsigned char temp = 0;
	unsigned char buffer[16] = {0};
	int i,data_length = 0;
	
	
 //SET_WLED_ON_OFF();
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
//	SendString("\n\r RL: ");
//	sendbyte(record_length);

	hex_offset_address = Package[1]<<8 & 0xFF00;
	hex_offset_address += Package[2]	& 0xFF;
	// SendString("\n\r OA: ");
	// sendbyte(hex_offset_address>>8);
	// sendbyte(hex_offset_address);

	record_type = Package[3];
	// SendString("\n\r Record Type: ");
	//  sendbyte(record_type);

	 checksum = (record_length + (hex_offset_address >> 8) + (hex_offset_address & 0xFF) + record_type);

	// SendString("\n\rR: ");
	for(i = 0; i<record_length; i++)
	{
		buffer[i] = Package[i+4];
		checksum += buffer[i];
		// sendbyte(buffer[i]);
	}
	checksum = ~checksum + 1;

	hex_chksum = Package[record_length+4];

	CRC = Package[(record_length+5)]<<8 & 0xFF00;	// CRC is appended to last two bytes of each hex record
	CRC += Package[(record_length+6)] & 0xFF;		// CRC received from hex file

	// SendString("\n\rReceived CRC: ");
	// senddata(CRC>>12);
	// senddata(CRC>>8);
	// senddata(CRC>>4);
	// senddata(CRC);	

	data_length = (record_length+1+2+1);			// number of bytes (record_length) + 1 byte of record_length + 2 bytes of offset address + 1 byte of checksum				
	calculated_crc = calculate_crc16(Package, data_length); // CRC calculated on received bytes
	// SendString("\n\rCalculated CRC: ");
	// senddata(calculated_crc>>12);
	// senddata(calculated_crc>>8);
	// senddata(calculated_crc>>4);
	// senddata(calculated_crc);

	if (checksum == hex_chksum)
	{
		if (CRC == calculated_crc)
		{	
			write_mcu_flash(buffer, record_length, hex_offset_address);
		} 
		else 
		{	
			SendString("\n\rCRC Error");	
		}
	} else {
			SendString("\n\rChecksum Error");
//			sendbyte(checksum);
//			SendString("\n\rError: Received Checksum: ");
//			sendbyte(hex_chksum);
	}
}

void UpgradeApp() __ram
{
	int i, index = 0;
	unsigned char temp =0;	
	// unsigned long addr_offset;


	SendString("\r\n'Boot mode' Erasing flash...! \r\nPlease wait!");	
	// erase flash from 0x00081000 to 0x0008F7FF

	chipErase();							// external flash, complete chip erase
	for(i = 0x2000; i < 0xF800; i += 0x200)
	{
		erase_page(i);						// ADuC's flash -> erase pages from 0x2000 to 0xF800, this space shall be used for bootloader
	}
	SendString("\r\nSuccess...! Load hex file\r\n");	// send to host-computer to  handshake

	IRQEN = UART_BIT;
	while(COMRX != '#') {}
	index = 0;
	addr_offset = BOOT_EXT_FLASH_OFFSET;
	
while(record_type != 0x01)
	{
		if(record_type != 0x01)
		{
		parse_hex();
		toggle_WLED;		
		} else {break;}
	} 
 
  

/*	


		else if (record_type == 0x00)							// Record type =0x00: Significant data lines
		{


//										temp = (data_array[lineindex-1][i]) + ((data_array[lineindex-1][i+1])<<8);			// lineindex has already been incremented for next record								
//										save(AppStartAddr,temp);						// write to flash memory from the starting address (offset) of user application 
//										AppStartAddr += 2;
													
/*										SendString("\n\rData ");
										senddata(temp>>4);
										senddata(temp);
										senddata(temp>>12);
										senddata(temp>>8);
										
										memread = load(AppStartAddr-2);
											
//							}
//							delay_ms(1);														
//							line_written = 1;				// flag set to indicate the current hex line is written to the memory							
						}
						
				}

/*
				SendString("\n\n\r Checksum Calculated: ");
				senddata(checksum>>4);
				senddata(checksum);

				SendString("\n\r Checksum Received: ");
				senddata(hex_chksum>>4);
				senddata(hex_chksum);

				SendString("\n\n\r Calculated CRC: ");
				senddata(calculated_crc>>12);
				senddata(calculated_crc>>8);
				senddata(calculated_crc>>4);
				senddata(calculated_crc);
				SendString("\n\r");
				
				SendString("\n\r Temp2: ");
				senddata(temp2>>12);
				senddata(temp2>>8);
				senddata(temp2>>4);
				senddata(temp2);
				
				SendString("\n\r Received CRC: ");
				senddata(CRC>>12);
				senddata(CRC>>8);
				senddata(CRC>>4);
				senddata(CRC);

				SendString("\n\r Line Index: ");
				senddata(lineindex>>4);
				senddata(lineindex);
				SendString("\n\r");				
		}																				 	
				
		else {
				SendString("\n\rInvalid record type\n\r");
			}
			*/
	toggle_WLED;				// LED is turned off, indicate Upgrade Finish!!!
			
	SendString("\r\nRestarting...!");	
	i=0x1000;
	while(i>0)i--;					// short delay!
	RSTSTA = 0x04;					// software reset
	RSTSTA = 0x04;					// software reset
	while(1);		
}
/***********************************************************************


 Author        : Johnson Jiao            Johnson.Jiao@analog.com

 Date          : 1st April. 2008

 File          : main.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about flash erase/write/read
************************************************************************/

extern void erase_page(unsigned int addr); 
extern void save(unsigned short addr, unsigned short data);
extern unsigned short load(unsigned int addr);


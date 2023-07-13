#include <ADuC7026.h>
#include "initCPU.h"
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"
#include "flashSST25VF.h"

void InitCPU(void);
void InitUART(void);
void InitTimer(void);
void InitSPI(void);
void Build_INT_Vector_Table(void);
void delay_ms(unsigned int);
void SET_TEST_LED_ON_OFF(void);
void SET_GLED_ON_OFF(void);
void SET_WLED_ON_OFF(void);


void InitCPU (void)
{

    // configures GPIO to flash LED P1.1
	GP1DAT |= 0x02000000;			        // P1.1 configured as an output. LED is turned on
	
    // configures GPIO to flash LED P4.2, P4.3 (for use in Eval board only)
	GP4DAT |= 0x0C000000;					// P4.2 & P4.3 configured as an output. LED is turned off

    InitUART();				                // Baudrate of 115200,n,8,1
    InitTimer();
    InitSPI();
    FIQEN = UART_BIT + GP_TIMER_BIT;        // Enable UART and Timer1 interrupt in FIQ mode		 
	FEEMOD = 0x8;				            // bit 3 should be set to allow erase/write command (ADUC's Flash)
}
//////////////////////////////////////////////////////////////////////////////
//  Some functions to be used throughout the program                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void Build_INT_Vector_Table()
{
	volatile unsigned long *Des_VEC, *SOU_VEC;
	int i;
  // Interrupt Vector Table in SRAM
	Des_VEC	= (unsigned long * )0x10000;
  // Interrupt Vector Table in Flash, please refer to Startup.s
  //     AREA   STARTUPCODE, CODE, AT 0x00080000
	SOU_VEC	= (unsigned long * )0x80000;

  // Copy the interrupt Vector Table from Flash to SRAM for 16 instructions
	for (i = 0; i < 16; i++)
	{
		*Des_VEC = *SOU_VEC;
		*Des_VEC++;
		*SOU_VEC++;
	}
}

void InitTimer()
{
	//	40960000 / 32768 * 0x186A =5sec
	T1LD = 0x186A;
	T1CON = 0xCF;						// Enable, Periodic, Binary and CLK/32768

	// Timer 3 configuration for WDT
//	T3LD  = 0x2000;			  			// (1/32768)*8192 			= 250ms
//	T3CON = 0xE0;    	  				// ->WDT

}

void delay_ms(unsigned int count) __ram
{
    unsigned int i;
    for (i = 0; i < count; i++)
    {
        unsigned int j;
        for (j = 0; j < (3276800 / 1152) - 1; j++)
        {
            __asm nop; // A no-operation instruction to waste a cycle
        }
    }
}

void SET_TEST_LED_ON_OFF(void) __ram                          // Toggles LED Connected to P1.1 
{
  SET_LED_TEST_ON ;delay_ms(30);
  SET_LED_TEST_OFF;delay_ms(30); 
}

void SET_GLED_ON_OFF(void) __ram                              // Toggles LED Connected to P4.2 (Eval board only) 
{
  SET_GLED_ON ;delay_ms(30);   
  SET_GLED_OFF;delay_ms(30);  
}

void SET_WLED_ON_OFF(void) __ram                          // Toggles LED Connected to P4.3 (Eval board only - test specific) 
{
  SET_WLED_ON ;delay_ms(30);  
  SET_WLED_OFF;delay_ms(30);  
}
/***********************************************************************


 Author        : Johnson Jiao            Johnson.Jiao@analog.com

 Date          : 1st April. 2008

 File          : main.c

 Hardware      : Applicable to ADuC702x rev H or I silicon
                 Currently targetting ADuC7026.

 Description   : some head about IRQ and FIQ function
************************************************************************/


extern void IRQ_Handler() __irq;
extern void FIQ_Handler() __fiq;

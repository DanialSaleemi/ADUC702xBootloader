#include <ADuC7026.h>

void InitCPU (void);
void Build_INT_Vector_Table();
void InitTimer();
void SET_TEST_LED_ON_OFF(void);
void SET_GLED_ON_OFF(void);
void SET_WLED_ON_OFF(void);
void delay_ms(unsigned int count);

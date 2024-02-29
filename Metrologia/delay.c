
#include "delay.h"

#include "driver\driver_delay.h"





void init_delay(void)
{
	init_driver_delay();
}



void delay_us(uint16_t value)
{
	driver_delay_us(value);
}




void delay_test(void)
{
	driver_delay_test();
}



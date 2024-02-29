
#include "watchdog.h"

#include "driver\driver_watchdog.h"


void init_watchdog(void)
{
	init_driver_watchdog();
}



void wdtr(void)
{
	driver_watchdog_reset();
}


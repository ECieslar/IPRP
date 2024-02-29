

#include "reset.h"

#include "driver\driver_reset.h"



bool reset_source_power(void)
{
	return driver_reset_source_power();
}






#include "lcd.h"

#include "keys.h"

#include "timers.h"

#include "ultrasonic_meter.h"

#include "pressure_meter.h"

#include "power.h"

#include "irda.h"

#include "clock.h"

#include "inttypes.h"

#include "extras.h"

#include "watchdog.h"

#include "input_voltage.h"

#include "hub_communication.h"

#include "radio_communication.h"

#include "keys.h"




void scheduler(void)
{
	

	for(;;)
	{
		wdtr();
			
		check_date_time();
		
		check_timers();
		
		check_irda();
		
		check_voltage_measure();
		
		check_hub_communication();
		
		check_radio_communication();
		
		check_key_activation();
		
		power_save();
	}

	
}


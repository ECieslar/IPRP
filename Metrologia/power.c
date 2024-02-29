

#include "power.h"

#include "keys.h"

#include "clock.h"

#include "menu.h"

#include "pressure_meter.h"

#include "irda.h"

#include "timers.h"

#include "debug.h"

#include "driver/driver_power.h"




enum power_modes
{
	power_mode_run							=	0,
	power_mode_low_power_run		=	1,
	power_mode_sleep						=	2,
	power_mode_low_power_sleep	=	3,
	power_mode_stop							=	4,
	power_mode_standby					=	5,
} power_mode;




void init_power_mode(void)
{
	power_mode = power_mode_low_power_sleep;
	
}



static void prepare_deep_sleep_entry(void)
{
	keys_prepare_deep_sleep_entry();
}



static void prepare_deep_sleep_exit(void)
{
	keys_prepare_deep_sleep_exit();
}


static void determine_power_mode(void)
{
	if(is_key_activation() || is_irda_on() || ultrasonic_meter_transmitting() || time_calibration_calibrating() || debug_is_transmitting())
	{
		power_mode = power_mode_low_power_sleep;
	}
	else
	{
		if(irda_power_request())
		{
			power_mode = power_mode_low_power_sleep;
		}
		else
		{
			//power_mode = power_mode_low_power_sleep;
			power_mode = power_mode_stop;
		}
	}
}



void power_save(void)
{
	determine_power_mode();
		
	switch(power_mode)
	{
		case power_mode_run:
		{
			break;
		}
		
		case power_mode_low_power_run:
		{
			driver_enter_low_power_run();
			break;
		}
		
		case power_mode_sleep:
		{
			driver_enter_sleep();
			break;
		}
		
		case power_mode_low_power_sleep:
		{
			driver_enter_low_power_sleep();
			break;
		}
		
		case power_mode_stop:
		{
			prepare_deep_sleep_entry();
			driver_enter_stop();
			prepare_deep_sleep_exit();
			break;
		}
		
		case power_mode_standby:
		{
			driver_enter_standby();
			break;
		}
	}
}



bool is_low_power_mode(void)
{
	if(power_mode == power_mode_stop)
	{
		return true;
	}
	
	return false;
}



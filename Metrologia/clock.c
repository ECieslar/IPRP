

#include "clock.h"

#include "date_time.h"

#include "keys.h"

#include "pressure_meter.h"

#include "input_voltage.h"

#include "irda.h"

#include "battery.h"

#include "ultrasonic_meter.h"

#include "counters_and_consumption.h"

#include "latches.h"

#include "prepaid.h"

#include "menu.h"

#include "schedule.h"

#include "driver\driver_clock.h"

#include "driver\driver_rtc.h"

void init_clock(void)
{
	driver_init_clock();
	
	driver_rtc_init();
}



void check_date_time(void)
{
	if(driver_rtc_tick())
	{
		extras_pin1_toggle();
		
		driver_rtc_reset_tick();
		
		update_date_time();
		
		keys_tick();
		
		input_voltage_tick();
		
		irda_tick();
		
		ultrasonic_meter_tick();
		
		battery_tick();
		
		menu_invoke_slow_timer();
		
		update_archive();
		
		update_latches();
		
		check_prepaid();
		
		update_volume_energy();
		
		check_schedule();
	}
}



uint64_t get_time_ms(void)
{
	return get_driver_clock_time_ms();
}


bool time_calibration_calibrating(void)
{
	return driver_time_calibration_calibrating();
}





#include "clock.h"

#include "keys.h"

#include "lcd.h"

#include "timers.h"

#include "date_time.h"

#include "menu.h"

#include "power.h"

#include "flash.h"

#include "ultrasonic_meter.h"

#include "pressure_meter.h"

#include "irda.h"

#include "valve.h"

#include "bellows.h"

#include "watchdog.h"

#include "extras.h"

#include "battery.h"

#include "crc.h"

#include "information.h"

#include "location.h"

#include "counters_and_consumption.h"

#include "flow.h"

#include "latches.h"

#include "calibration.h"

#include "driver/driver_power.h"

#include "hub_communication.h"

#include "radio_communication.h"

#include "events_configuration.h"

#include "modem.h"

#include "mkg_module.h"

#include "prepaid.h"

#include "protocol_settings.h"

#include "delay.h"

#include "smart_gas.h"

#include "valve.h"

#include "schedule.h"

#include "debug.h"

#include "preparation.h"




static uint32_t preparation_signature;

#define PREPARATION_SIGNATURE_VALUE	0xAC4A7DF1


void check_preparation(void)
{
	eeprom_read_parameter((uint8_t*)&preparation_signature, PREPARATION_SIGNATURE);
	
	if(preparation_signature != PREPARATION_SIGNATURE_VALUE)
	{
		preparation_signature = PREPARATION_SIGNATURE_VALUE;
		
		eeprom_write_parameter((uint8_t*)&preparation_signature, PREPARATION_SIGNATURE);
		
		prepare_parameters(1);
	}
}



bool prepare_parameters(uint8_t value)
{
	if(value != 0 )
	{
		return false;
	}
	
	prepare_calibration();
	
	prepare_date_time();
	
	prepare_information();
	
	prepare_location();
	
	prepare_latches();
	
	prepare_events_configuration();
	
	prepare_modem();
	
	prepare_prepaid();
	
	prepare_protocol_settings();
	
	prepare_counters_and_consumption();
	
	prepare_flow();
	
	prepare_valve();
	
	prepare_mkg_module();
	
	prepare_schedule();
	
	return true;
}



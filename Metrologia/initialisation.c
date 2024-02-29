
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

#include "debug.h"

#include "preparation.h"

#include "logs.h"

#include "status.h"

#include "schedule.h"



void init(void)
{
	//driver_enter_standby();
	
	init_extras();
		
	init_clock();
	
	init_debug();
	
	init_power_mode();
	
	init_calibration();
	
	init_delay();
	
	init_flash();
		
	init_keys();
	
	init_lcd();
	
	init_timers();
	
	init_date_time();
	
	
	
	init_menu();
	
	init_hub_communication();
	
	init_radio_communication();
	
	init_ultrasonic_meter();
	
	init_pressure_meter();
	
	init_bellows();
	
	init_battery();
	
	//init_bellows_emulator();
	
	init_crc();
	
	init_information();
	
	init_irda();
	
	init_input_voltage();
	
	init_valve();
	
	init_location();
	
	init_counters_and_consumption();
	
	init_flow();
	
	init_latches();
	
	init_calibration();
	
	init_events_configuration();
	
	init_modem();
	
	init_mkg_module();
	
	init_prepaid();
	
	init_protocol_settings();
	
	init_smart_gas();
	
	init_status();
	
	init_logs();
	
	init_schedule();
	
	check_preparation();
	
	init_watchdog();
}






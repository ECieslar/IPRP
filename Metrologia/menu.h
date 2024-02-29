
#ifndef MENU_H_
#define MENU_H_



#include "extras.h"

#include "date_time.h"

#include "timers.h"

#include "lcd.h"

#include "keys.h"

#include "ultrasonic_meter.h"

#include "pressure_meter.h"

#include "input_voltage.h"

#include "irda.h"

#include "valve.h"

#include "power.h"

#include "battery.h"

#include "version.h"

#include "stdbool.h"

#include "inttypes.h"

#include "flow.h"

#include "bellows.h"

#include "eeprom.h"

#include "radio_communication.h"

#include "watchdog.h"

#include "debug.h"

#include <string.h>


enum menu_events
{
	menu_event_redraw										= 0,
	menu_event_timer										= 1,
	menu_event_slow_timer								= 2,
	menu_event_time_change							= 3,
	menu_event_key_down									= 4,
	menu_event_key_press								= 5,
	menu_event_key_up										= 6,
	menu_event_flow											= 7,
	menu_event_valve_current						= 8,
	menu_event_bellows_tick							= 9,
};



enum menu_states
{
	menu_state_init							=	1,
	menu_state_essential_info		=	2,
	menu_state_values						=	3,
	menu_state_pressure_meter		=	4,
	menu_state_input_voltage		=	5,
	menu_state_irda							=	6,
	menu_state_valve						=	7,
	menu_state_battery					=	8,
	menu_state_bellows					=	9,
	menu_state_radio						=	10,
};


void set_menu_state(enum menu_states state);

void init_menu(void);

void menu_event(enum menu_events event, void* parameters);

void menu_invoke_slow_timer(void);



#endif /*MENU_H_*/

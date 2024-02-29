

#include "menu.h"

#include "menu\menu_init.h"

#include "menu\menu_essential_info.h"

#include "menu\menu_values.h"

#include "menu\menu_pressure_meter.h"

#include "menu\menu_input_voltage.h"

#include "menu\menu_irda.h"

#include "menu\menu_valve.h"

#include "menu\menu_battery.h"

#include "menu\menu_bellows.h"

#include "menu\menu_radio.h"



static enum menu_states menu_state;



static void timer_event()
{
	menu_event(menu_event_timer, NULL);
}


void init_menu(void)
{
	register_time_event(timer_event, 100, false);
	
	set_menu_init();
}



void menu_invoke_slow_timer(void)
{
	menu_event(menu_event_slow_timer, NULL);
}



void set_menu_state(enum menu_states state)
{
	menu_state = state;
}


void menu_event(enum menu_events event, void* parameters)
{
	switch(event)
	{
		case menu_event_time_change:
		{
			
		}
		
		default:
		{
		}
	}
	
	switch(menu_state)
	{
		case menu_state_init:
		{
			menu_event_init(event, parameters);
			break;
		}
		
		case menu_state_essential_info:
		{
			menu_event_essential_info(event, parameters);
			break;
		}
		
		case menu_state_values:
		{
			menu_event_values(event, parameters);
			break;
		}
		
		case menu_state_pressure_meter:
		{
			menu_event_pressure_meter(event, parameters);
			break;
		}
		
		case menu_state_input_voltage:
		{
			menu_event_input_voltage(event, parameters);
			break;
		}
		
		case menu_state_irda:
		{
			menu_event_irda(event, parameters);
			break;
		}
		
		case menu_state_valve:
		{
			menu_event_valve(event, parameters);
			break;
		}
		
		case menu_state_battery:
		{
			menu_event_battery(event, parameters);
			break;
		}
		
		case menu_state_bellows:
		{
			menu_event_bellows(event, parameters);
			break;
		}
		
		case menu_state_radio:
		{
			menu_event_radio(event, parameters);
			break;
		}
	}
}




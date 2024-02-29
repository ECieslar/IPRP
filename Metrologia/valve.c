
#include "valve.h"


#include "driver/driver_input_voltage.h"

#include "driver/driver_adc.h"

#include "timers.h"

#include "hub_communication.h"

#include "menu.h"

#include "counters_and_consumption.h"

#include <string.h>

#include "status.h"



static bool is_left;

static bool is_right;

static uint16_t current = 0;

static uint8_t valve_operation;

static uint16_t valve_activation_timeout;

static uint16_t valve_leak_test_duration;

static uint16_t valve_leak_test_volume;

static uint32_t valve_volume_units;

static uint8_t valve_error_code;

static uint16_t valve_open_count;

static uint16_t valve_close_count;

static int valve_activation_timeout_function_idx;

static int valve_leak_test_duration_function_idx;

enum valve_states
{
	valve_state_open_no_operation 				= 0,
	valve_state_closed_no_operation				= 1,
	valve_state_closed_waiting_activation	= 2,
	valve_state_open_tightness_test				= 3,
	valve_state_closed_opening_failure		= 4,
} valve_state;





static void init_valve_error_code(void)
{
	valve_error_code = 0;
}



static void init_valve_leak_test_duration(void)
{
	eeprom_read_parameter((uint8_t*)&valve_leak_test_duration, VALVE_LEAK_TEST_DURATION);
}



static void prepare_valve_leak_test_duration(void)
{
	set_valve_leak_test_duration(300);
}



static void init_valve_leak_test_volume(void)
{
	eeprom_read_parameter((uint8_t*)&valve_leak_test_volume, VALVE_LEAK_TEST_VOLUME);
}



static void prepare_valve_leak_test_volume(void)
{
	set_valve_leak_test_volume(2);
}



static void init_valve_open_count(void)
{
	eeprom_read_parameter((uint8_t*)&valve_open_count, VALVE_OPEN_COUNT);
}



static void prepare_valve_open_count(void)
{
	set_valve_open_count(0);
}



static void init_valve_close_count(void)
{
	eeprom_read_parameter((uint8_t*)&valve_close_count, VALVE_CLOSE_COUNT);
}



static void prepare_valve_close_count(void)
{
	set_valve_close_count(0);
}



static void set_valve_state(enum valve_states state)
{
	valve_state = state;
	
	uint8_t tmp_state = valve_state;
	
	eeprom_write_parameter((uint8_t*)&tmp_state, VALVE_STATE);
}



static void prepare_valve_state(void)
{
	valve_off();
	
	set_valve_state(valve_state_closed_no_operation);
}



static void init_valve_state(void)
{
	uint8_t state;
	
	eeprom_read_parameter((uint8_t*)&state, VALVE_STATE);
	
	valve_state = (enum valve_states)state;
	
	valve_state = valve_state_open_no_operation;
	
	switch(valve_state)
	{
		case valve_state_open_no_operation:
		{
			valve_on();
			break;
		}
		
		case valve_state_closed_no_operation:
		{
			valve_off();
			break;
		}
		
		case valve_state_closed_waiting_activation:
		{
			valve_off();
			break;
		}
		
		case valve_state_open_tightness_test:
		{
			valve_on();
			break;
		}
		
		case valve_state_closed_opening_failure:
		{
			valve_off();
			break;
		}
		
		default:
		{
			prepare_valve_state();
			break;
		}
	}
}


static void init_valve_activation_timeout(void)
{
	valve_activation_timeout = 0;
}



static void prepare_valve_activation_timeout(void)
{
	valve_activation_timeout = 0;
}




static void valve_activation_timeout_function(void)
{
	valve_activation_timeout--;
	
	if(valve_activation_timeout == 0)
	{
		stop_time_event(valve_activation_timeout_function_idx);
		
		set_valve_state(valve_state_closed_opening_failure);
	}
}



static void valve_leak_test_duration_function(void)
{
	valve_leak_test_duration--;
	
	if(valve_leak_test_duration == 0)
	{
		stop_time_event(valve_leak_test_duration_function_idx);
		
		valve_off();
		
		set_valve_state(valve_state_closed_no_operation);
		
		if(get_volume_units() - valve_volume_units > valve_leak_test_volume)
		{
			//nieudana próba szczelnosci
			set_status_flag(flag_valve_leak_test_failed);
		}
		else
		{
			//udana próba szczelnosci
			clr_status_flag(flag_valve_leak_test_failed);
		}
	}
}



void init_valve(void)
{
	is_left = false;
	
	is_right = false;
	
	init_valve_error_code();
	
	init_valve_leak_test_duration();
	
	init_valve_leak_test_volume();
	
	init_valve_open_count();
	
	init_valve_close_count();
	
	init_valve_state();
	
	init_valve_activation_timeout();
	
	valve_activation_timeout_function_idx = register_time_event(valve_activation_timeout_function, 60000, true);
	
	valve_leak_test_duration_function_idx = register_time_event(valve_leak_test_duration_function, 60000, true);
}


void prepare_valve(void)
{
	valve_off();
	
	prepare_valve_leak_test_duration();
	
	prepare_valve_leak_test_volume();
	
	prepare_valve_open_count();
	
	prepare_valve_close_count();
	
	prepare_valve_state();
	
	prepare_valve_activation_timeout();
}



void valve_turn_left(void)
{
	send_hub_data(hcf_valve_turn_left, NULL, 0);
}




void valve_turn_right(void)
{
	send_hub_data(hcf_valve_turn_right, NULL, 0);
}




void valve_turn_off(void)
{
	send_hub_data(hcf_valve_turn_off, NULL, 0);
}



void valve_invoke_get_current(void)
{
	send_hub_data(hcf_valve_invoke_get_current, NULL, 0);
}



void valve_current(uint8_t* data)
{
	current = data[0];
	current <<= 8;
	current |= data[1];
	
	menu_event(menu_event_valve_current, NULL);
}



uint16_t valve_get_current(void)
{
	return current;
}



bool is_valve_turn_left(void)
{
	return is_left;
}


bool is_valve_turn_right(void)
{
	return is_right;
}


void valve_off(void)
{
	set_valve_close_count(valve_close_count + 1);
	
	valve_turn_right();
}



void valve_on(void)
{
	set_valve_open_count(valve_open_count + 1);
	
	valve_turn_left();
}


bool set_valve_operation(uint8_t vo)
{
	switch(vo)
	{
		case 0x01:
		{
			if(valve_state == valve_state_closed_waiting_activation)
			{
				valve_on();
				
				set_valve_state(valve_state_open_no_operation);
			}
			
			break;
		}
		
		case 0x02:
		{
			valve_off();
			
			set_valve_state(valve_state_closed_no_operation);
			
			break;
		}
		
		default:
		{
			return false;
		}
	}
	
	valve_operation = vo;
	
	eeprom_write_parameter(&valve_operation, VALVE_OPERATION);
	
	return true;
}



uint16_t get_valve_activation_timeout(void)
{
	return valve_activation_timeout;
}


void set_valve_activation_timeout(uint16_t vat)
{
	if((valve_state == valve_state_closed_no_operation) || 
		 (valve_state == valve_state_closed_opening_failure) ||
		 (valve_state == valve_state_closed_waiting_activation))
	{
		valve_activation_timeout = vat;
		
		eeprom_write_parameter((uint8_t*)&valve_activation_timeout, VALVE_ACTIVATION_TIMEOUT);
		
		start_time_event(valve_activation_timeout_function_idx);
		
		set_valve_state(valve_state_closed_waiting_activation);
	}
}



uint8_t get_valve_state(void)
{
	return valve_state;
}


uint16_t get_valve_leak_test_duration(void)
{
	return valve_leak_test_duration;
}


void set_valve_leak_test_duration(uint16_t vltd)
{
	valve_leak_test_duration = vltd;
	
	eeprom_write_parameter((uint8_t*)&valve_leak_test_duration, VALVE_LEAK_TEST_DURATION);
	
	start_time_event(valve_leak_test_duration_function_idx);
	
	valve_volume_units = get_volume_units();
	
	valve_on();
	
	set_valve_state(valve_state_open_tightness_test);
}



uint16_t get_valve_leak_test_volume(void)
{
	switch(get_volume_units())
	{
		case 0x01:
		{
			return valve_leak_test_volume / 10000;
		}
		
		case 0x00:
		{
			return valve_leak_test_volume / 1000;
		}
		
		case 0x0F:
		{
			return valve_leak_test_volume / 100;
		}
		
		case 0x0E:
		{
			return valve_leak_test_volume / 10;
		}
	}
	
	return 0;
}


void set_valve_leak_test_volume(uint16_t vltv)
{
	switch(get_volume_units())
	{
		case 0x01:
		{
			valve_leak_test_volume = 10000 * vltv;
			break;
		}
		
		case 0x00:
		{
			valve_leak_test_volume = 1000 * vltv;
			break;
		}
		
		case 0x0F:
		{
			valve_leak_test_volume = 100 * vltv;
			break;
		}
		
		case 0x0E:
		{
			valve_leak_test_volume = 10 * vltv;
			break;
		}
	}
	
	eeprom_write_parameter((uint8_t*)&valve_leak_test_volume, VALVE_LEAK_TEST_VOLUME);
}



uint8_t get_valve_error_code(void)
{
	return valve_error_code;
}


uint16_t get_valve_open_count(void)
{
	return valve_open_count;
}


void set_valve_open_count(uint16_t voc)
{
	valve_open_count = voc;
	
	eeprom_write_parameter((uint8_t*)&valve_open_count, VALVE_OPEN_COUNT);
}


uint16_t get_valve_close_count(void)
{
	return valve_close_count;
}


void set_valve_close_count(uint16_t vcc)
{
	valve_close_count = vcc;
	
	eeprom_write_parameter((uint8_t*)&valve_close_count, VALVE_CLOSE_COUNT);
}

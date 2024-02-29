
#include "battery.h"

#include "power.h"

#include "flow.h"

#include "status.h"

#include "ultrasonic_meter.h"


static uint64_t max_capacity;

static uint64_t capacity;

static uint32_t low_power_mode_no_flow_consumption;

static uint32_t low_power_mode_flow_consumption;

static uint32_t high_power_mode_consumption;








void init_battery(void)
{
	max_capacity = (uint64_t)MAX_CAPACITY_UAH * (uint64_t)3600;
	
	capacity = max_capacity;
	
	low_power_mode_no_flow_consumption = 60;

	low_power_mode_flow_consumption = 70;

	high_power_mode_consumption = 450;

}



void battery_tick(void)
{
	uint32_t consumption = 0;
	
	if(is_low_power_mode())
	{
		if(get_temporary_flow() == 0)
		{
			consumption = low_power_mode_no_flow_consumption;
		}
		else
		{
			consumption = low_power_mode_flow_consumption;
		}
	}
	else
	{
		consumption = high_power_mode_consumption;
	}
	
	if(consumption > capacity)
	{
		capacity = 0;
	}
	else
	{
		capacity -= consumption;
	}
	
	
	if((uint64_t)100 * capacity / max_capacity < 10)
	{
		set_status_flag(flag_battery_low);
	}
	else
	{
		clr_status_flag(flag_battery_low);
	}
}


uint8_t get_battery_level(void)
{
	return 100 * capacity / max_capacity;
}



uint32_t get_battery_capacity(void)
{
	return capacity / 3600;
}


bool set_batter_level(uint8_t level)
{
	if(level > 100)
	{
		return false;
	}
	
	capacity = (uint64_t)level * max_capacity / 100;
	
	return true;
}





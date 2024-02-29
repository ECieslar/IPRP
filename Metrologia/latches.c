

#include "latches.h"

#include "eeprom.h"

#include "date_time.h"

#include "counters_and_consumption.h"

#include "status.h"


static uint8_t monthly_data_latch_day;

static uint8_t monthly_and_daily_data_latch_hour;

static uint8_t monthly_and_daily_data_latch_local_time;

static uint32_t last_daily_latch_time;

static uint32_t last_daily_latch_volume;

static uint32_t last_daily_latch_energy;




static void init_monthly_data_latch_day(void)
{
	eeprom_read_parameter(&monthly_data_latch_day, MONTHLY_DATA_LATCH_DAY);
	
	if((monthly_data_latch_day == 0) || (monthly_data_latch_day > 31))
	{
		set_monthly_data_latch_day(1);
	}
}



static void prepare_monthly_data_latch_day(void)
{
	set_monthly_data_latch_day(1);
}



static void init_monthly_and_daily_data_latch_hour(void)
{
	eeprom_read_parameter(&monthly_and_daily_data_latch_hour, MONTHLY_AND_DAILY_DATA_LATCH_HOUR);
	
	if(monthly_and_daily_data_latch_hour > 23)
	{
		set_monthly_and_daily_data_latch_hour(6);
	}
}



static void prepare_monthly_and_daily_data_latch_hour(void)
{
	set_monthly_and_daily_data_latch_hour(6);
}



static void init_monthly_and_daily_data_latch_local_time(void)
{
	eeprom_read_parameter(&monthly_and_daily_data_latch_local_time, MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME);
	
	if(monthly_and_daily_data_latch_local_time > 1)
	{
		set_monthly_and_daily_data_latch_local_time(0);
	}
}



static void prepare_monthly_and_daily_data_latch_local_time(void)
{
	set_monthly_and_daily_data_latch_local_time(0);
}



void init_latches(void)
{
	init_monthly_data_latch_day();
	
	init_monthly_and_daily_data_latch_hour();
	
	init_monthly_and_daily_data_latch_local_time();
	
	last_daily_latch_time = 0;
}



void prepare_latches(void)
{
	prepare_monthly_data_latch_day();
	
	prepare_monthly_and_daily_data_latch_hour();
	
	prepare_monthly_and_daily_data_latch_local_time();
}



uint8_t get_monthly_data_latch_day(void)
{
	return monthly_data_latch_day;
}



bool set_monthly_data_latch_day(uint8_t mdld)
{
	if((mdld == 0) || (mdld > 31))
	{
		return false;
	}
	
	monthly_data_latch_day = mdld;
	
	eeprom_write_parameter(&monthly_data_latch_day, MONTHLY_DATA_LATCH_DAY);
	
	return true;
}




uint8_t get_monthly_and_daily_data_latch_hour(void)
{
	return monthly_and_daily_data_latch_hour;
}



bool set_monthly_and_daily_data_latch_hour(uint8_t maddlh)
{
	if(maddlh > 31)
	{
		return false;
	}
	
	monthly_and_daily_data_latch_hour = maddlh;
	
	eeprom_write_parameter(&monthly_and_daily_data_latch_hour, MONTHLY_AND_DAILY_DATA_LATCH_HOUR);
	
	return true;
}





uint8_t get_monthly_and_daily_data_latch_local_time(void)
{
	return monthly_and_daily_data_latch_local_time;
}



bool set_monthly_and_daily_data_latch_local_time(uint8_t maddllt)
{
	if(maddllt > 1)
	{
		return false;
	}
	
	monthly_and_daily_data_latch_local_time = maddllt;
	
	eeprom_write_parameter(&monthly_and_daily_data_latch_local_time, MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME);
	
	return true;
}




void update_latches(void)
{
	static bool is_latch_hour = false;
	
	uint32_t unix_time;
	
	if(monthly_and_daily_data_latch_local_time == 0)
	{
		unix_time = get_unix_time_utc();
	}
	else
	{
		unix_time = get_unix_time_local();
	}
	
	uint8_t current_hour = (unix_time / 3600) % 24;
	
	if(is_latch_hour)
	{
		if(current_hour != monthly_and_daily_data_latch_hour)
		{
			is_latch_hour = false;
		}
	}
	else
	{
		if(current_hour == monthly_and_daily_data_latch_hour)
		{
			is_latch_hour = true;
			
			last_daily_latch_time = unix_time;
			
			last_daily_latch_volume = get_volume_units();
		
			last_daily_latch_energy = get_energy_units();
			
			update_status_periodic_latched();
		}
	}
}




uint32_t get_last_daily_latch_time(void)
{
	return last_daily_latch_time;
}



uint32_t get_last_daily_latch_volume(void)
{
	return last_daily_latch_volume;
}




uint32_t get_last_daily_latch_energy(void)
{
	return last_daily_latch_energy;
}




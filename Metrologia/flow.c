
#include "flow.h"

#include "counters_and_consumption.h"

#include "clock.h"

#include "menu.h"

#include "eeprom.h"

#include "latches.h"

#include "date_time.h"

#include "reset.h"

#include "driver\driver_rtc.h"

#include "stdio.h"

#include "crc.h"



static int32_t temporary_flow;

static int64_t volume;
//static double volume;

static int64_t volume_max_value;

static uint16_t calorific_value;

static int64_t energy;

static int64_t current_hour_volume;

static int32_t temporary_flow_period_latched;

static int32_t temporary_flow_period;

static int64_t temporary_flow_period_latched_time_compressed;

static int64_t temporary_flow_period_time_compressed;

static int32_t temporary_flow_monthly_latched;

static int32_t temporary_flow_monthly;

static int64_t temporary_flow_monthly_latched_time_compressed;

static int64_t temporary_flow_monthly_time_compressed;

static int64_t hourly_flow_period_latched;

static int64_t hourly_flow_period;

static uint8_t hourly_flow_period_latched_time_compressed;

static uint8_t hourly_flow_period_time_compressed;

static int64_t hourly_flow_monthly_latched;

static int64_t hourly_flow_monthly;

static uint16_t hourly_flow_monthly_latched_time_compressed;

static uint16_t hourly_flow_monthly_time_compressed;

static int64_t last_hourly_volume;

static int update_volume_timeout_idx;

static uint16_t gas_meter_q_max;

static uint32_t ev_tab_idx;

#define PHYSICAL_GAS_METER_Q_MAX	6000




static void update_volume_timeout(void)
{
	temporary_flow = 0;
}



static uint32_t get_unix_time(void)
{
	if(get_monthly_and_daily_data_latch_local_time())
	{
		return get_unix_time_local();
	}
	else
	{
		return get_unix_time_utc();
	}
}



static void init_volume_energy_from_eeprom()
{
	ev_tab_idx = 0;
	
	volume = 0;
		
	energy = 0;
	
	for(int i = 0; i < 64; i++)
	{
		uint8_t tab[24];
		
		eeprom_read(tab, EA(ENERGY_VOLUME) + i * 24, 24);
		
		uint32_t crc32 = get_crc32(tab, 20);
		
		if(memcmp((uint8_t*)&crc32, tab + 20, 4) == 0)
		{
			uint32_t idx;
			
			memcpy((uint8_t*)&idx, tab + 16, 4);
			
			if(idx > ev_tab_idx)
			{
				ev_tab_idx = idx;
				
				memcpy((uint8_t*)&volume, tab, 4);
				
				memcpy((uint8_t*)&energy, tab + 8, 4);
			}
		}
	}
}



static void prepare_volume_energy_from_eeprom(void)
{
	uint8_t tab[24] = {	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
											0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
											0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	
	for(int i = 0; i < 64; i++)
	{
		eeprom_write(tab, EA(ENERGY_VOLUME) + i * 24, 24);
	}
	
	ev_tab_idx = 0;
}



static void init_volume(void)
{
	/*
	uint32_t tmp = get_backup_register(0);
	
	uint8_t* tab = (uint8_t*)&volume;
	
	memcpy(tab, (uint32_t*)&tmp, 4);
	
	tmp = get_backup_register(1);
	
	memcpy(tab + 4, (uint32_t*)&tmp, 4);
	*/
	
	if(reset_source_power())
	{
		init_volume_energy_from_eeprom();
	}
	else
	{
		volume = get_backup_register(1);
		
		volume <<= 32;
		
		volume |= get_backup_register(0);
		
		ev_tab_idx = get_backup_register(4);
	}
}



static void prepare_volume(void)
{
	volume = 0;
}





static void init_energy(void)
{
	if(reset_source_power())
	{
		init_volume_energy_from_eeprom();
	}
	else
	{
		energy = get_backup_register(3);
		
		energy <<= 32;
		
		energy |= get_backup_register(2);
		
		ev_tab_idx = get_backup_register(4);
	}
}



static void prepare_energy(void)
{
	energy = 0;
}



static bool set_gas_meter_q_max(uint16_t value)
{
	if((gas_meter_q_max == 0) || (gas_meter_q_max > PHYSICAL_GAS_METER_Q_MAX))
	{
		return false;
	}
	
	gas_meter_q_max = value;
	
	eeprom_write_parameter((uint8_t*)&gas_meter_q_max, GAS_METER_Q_MAX);
	
	return true;
}




static void read_gas_meter_q_max(void)
{
	eeprom_read_parameter((uint8_t*)&gas_meter_q_max, GAS_METER_Q_MAX);
	
	if((gas_meter_q_max == 0) || (gas_meter_q_max > PHYSICAL_GAS_METER_Q_MAX))
	{
		gas_meter_q_max = PHYSICAL_GAS_METER_Q_MAX;
		
		eeprom_write_parameter((uint8_t*)&gas_meter_q_max, GAS_METER_Q_MAX);
	}
}



void init_flow(void)
{
	temporary_flow = 0;
	
	init_volume();
	
	eeprom_read_parameter((uint8_t*)&calorific_value, CALORIFIC_VALUE);
	
	read_gas_meter_q_max();
	
	init_energy();
	
	current_hour_volume = 0;
		
	temporary_flow_period_latched = 0;
	
	temporary_flow_period = 0;
	
	temporary_flow_period_latched_time_compressed = 0;

	temporary_flow_period_time_compressed = 0;
	
	temporary_flow_monthly_latched = 0;

	temporary_flow_monthly = 0;
	
	temporary_flow_monthly_latched_time_compressed = 0;

	temporary_flow_monthly_time_compressed = 0;

	hourly_flow_period_latched = 0;

	hourly_flow_period = 0;
	
	hourly_flow_period_latched_time_compressed = 0;

	hourly_flow_period_time_compressed = 0;
	
	hourly_flow_monthly_latched = 0;

	hourly_flow_monthly = 0;
	
	hourly_flow_monthly_latched_time_compressed = 0;

	hourly_flow_monthly_time_compressed = 0;
	
	last_hourly_volume = volume;
	
	update_volume_timeout_idx = register_time_event(update_volume_timeout, 30000, false);
}



void prepare_flow(void)
{
	prepare_volume();
	
	prepare_energy();
	
	prepare_volume_energy_from_eeprom();
}


void backup_volume(void)
{
	/*
	uint8_t* tab = (uint8_t*)&volume;
	
	uint32_t tmp;
	
	memcpy((uint32_t*)&tmp, tab, 4);
	
	set_backup_register(0, tmp);
	
	memcpy((uint32_t*)&tmp, tab + 4, 4);
	
	set_backup_register(1, tmp);
	*/
	
	set_backup_register(1, volume >> 32);
	
	set_backup_register(0, volume & 0xffffffff);
	
	set_backup_register(4, ev_tab_idx);
}



void backup_energy(void)
{
	set_backup_register(3, energy >> 32);
	
	set_backup_register(2, energy & 0xffffffff);
	
	set_backup_register(4, ev_tab_idx);
}



uint16_t get_gas_meter_q_max_units(void)
{
	switch(get_units())
	{
		case 0x01:
		{
			return gas_meter_q_max / 10000;
		}
		
		case 0x00:
		{
			return gas_meter_q_max / 1000;
		}
		
		case 0x0F:
		{
			return gas_meter_q_max / 100;
		}
		
		case 0x0E:
		{
			return gas_meter_q_max / 10;
		}
	}
	
	return 0;
}




bool set_gas_meter_q_max_units(uint16_t value)
{
	uint16_t gmqm = 0;
	
	switch(get_units())
	{
		case 0x01:
		{
			gmqm = value * 10000;
		}
		
		case 0x00:
		{
			gmqm = value * 1000;
		}
		
		case 0x0F:
		{
			gmqm = value * 100;
		}
		
		case 0x0E:
		{
			gmqm = value * 10;
		}
	}
	
	if(set_gas_meter_q_max(gmqm))
	{
		return true;
	}
	
	return false;
}




void update_flow_time_compressed(void)
{
	temporary_flow_period_latched_time_compressed = temporary_flow_period_time_compressed;
	
	temporary_flow_monthly_latched_time_compressed = temporary_flow_monthly_time_compressed;
	
	hourly_flow_period_latched_time_compressed = hourly_flow_period_time_compressed;
	
	hourly_flow_monthly_latched_time_compressed = hourly_flow_monthly_time_compressed;
}



static void update_flow_latches(int32_t temporary_flow, int64_t delta_time_ms)
{
	if(temporary_flow >= 1000 * gas_meter_q_max)
	{
		temporary_flow_period_time_compressed += delta_time_ms;
		
		temporary_flow_monthly_time_compressed += delta_time_ms;
	}
	
	uint32_t unix_time = get_unix_time();
		
	if(temporary_flow > temporary_flow_period)
	{
		temporary_flow_period = temporary_flow;
	}
	
	if(temporary_flow > temporary_flow_monthly)
	{
		temporary_flow_monthly = temporary_flow;
	}
	
	unix_time = get_unix_time();
	
	rtc_t dt;
	
	convert_unix_time_to_date_time(unix_time, &dt);
	
	static uint8_t last_latched_hour = 255;
	
	if(dt.hour != last_latched_hour)
	{
		int64_t delta_volume = volume - last_hourly_volume;
		
		int64_t hourly_flow = delta_volume / 1000000;
		
		if(hourly_flow > hourly_flow_period)
		{
			hourly_flow_period = hourly_flow;
		}
		
		if(hourly_flow > hourly_flow_monthly)
		{
			hourly_flow_monthly = hourly_flow;
		}
		
		if(hourly_flow >= 1000 * gas_meter_q_max)
		{
			hourly_flow_period_time_compressed++;
			
			hourly_flow_monthly_time_compressed++;
		}
		
		last_hourly_volume = volume;
		
		last_latched_hour = dt.hour;
	}
	
	static uint8_t last_latched_day = 255;
	
	if((dt.day != last_latched_day) && (dt.hour == get_monthly_and_daily_data_latch_hour()))
	{
		temporary_flow_period_time_compressed = 0;
		
		hourly_flow_period_time_compressed = 0;
		
		temporary_flow_period_latched = temporary_flow_period;
		
		temporary_flow_period = 0;
		
		hourly_flow_period_latched = hourly_flow_period;
		
		hourly_flow_period = 0;
		
		last_latched_day = dt.day;
		
		static uint8_t last_latched_month = 255;
		
		if((dt.month != last_latched_month) && (dt.day == get_monthly_data_latch_day()))
		{
			temporary_flow_monthly_time_compressed = 0;
			
			hourly_flow_monthly_time_compressed = 0;
			
			temporary_flow_monthly_latched = temporary_flow_monthly;
			
			temporary_flow_monthly = 0;
			
			hourly_flow_monthly_latched = hourly_flow_monthly;

			hourly_flow_monthly = 0;
			
			last_latched_month = dt.month;
		}
	}
}


static int64_t get_standardized_dV(int64_t dV)
{
	return dV * 
	((int64_t)get_standard_temperature_additional() + (int64_t)27315) * 
	(int64_t)get_pressure() / 
	((int64_t)get_temperature_additional() + (int64_t)27315) / 
	(int64_t)get_standard_pressure();
}



void update_flow(int32_t flow)
{
	temporary_flow = flow;
	
	static uint64_t last_time_ms = 0;
	
	uint64_t time_ms = get_time_ms();
	
	int64_t delta_time_ms = time_ms - last_time_ms;
	
	if(delta_time_ms >= 2010)
	{
		delta_time_ms = 2000;
	}
	
	if(last_time_ms > 0)
	{
		int64_t dV = (int64_t)temporary_flow * delta_time_ms * (int64_t)5 / (int64_t)18; // [nL]
		
		dV = get_standardized_dV(dV);
		
		set_volume(volume + dV); //[nL]
		
		set_energy(energy + (int64_t)calorific_value * dV / (int64_t)1000); //[nWh]
		
		static uint32_t current_hour = 0;
		
		uint32_t tmp_current_hour = get_unix_time_utc() / 3600;
		
		if(current_hour != tmp_current_hour)
		{
			current_hour_volume = 0;
			
			current_hour = tmp_current_hour;
		}
		else
		{
			current_hour_volume += dV;
		}
	}
	
	last_time_ms = time_ms;
	
	update_flow_latches(temporary_flow, delta_time_ms);
	
	menu_event(menu_event_flow, NULL);
	
	start_time_event(update_volume_timeout_idx);
}



void update_volume(int32_t dV_mL)
{
	static uint64_t last_time_ms = 0;
				
	uint64_t time_ms = get_time_ms();
	
	int64_t delta_time_ms = time_ms - last_time_ms;
	
	int64_t dV = (int64_t)dV_mL * (int64_t)1000000;
	
	dV = get_standardized_dV(dV);
	
	if(delta_time_ms < 1)
	{
		delta_time_ms = 1;
	}
	
	temporary_flow = dV * (int64_t)18 / (int64_t)delta_time_ms / (int64_t)5;
	
	set_volume(volume + dV); //[nL]
	
	set_energy(energy + (int64_t)calorific_value * (int64_t)dV / (int64_t)1000); //[nWh]
	
	static uint32_t current_hour = 0;
	
	uint32_t tmp_current_hour = get_unix_time_utc() / 3600;
	
	if(current_hour != tmp_current_hour)
	{
		current_hour_volume = 0;
		
		current_hour = tmp_current_hour;
	}
	else
	{
		current_hour_volume += dV;
	}
	
	last_time_ms = time_ms;
	
	update_flow_latches(temporary_flow, delta_time_ms);
	
	menu_event(menu_event_flow, NULL);
	
	start_time_event(update_volume_timeout_idx);
}



uint16_t get_temporary_flow_units(void)
{
	int32_t tf = temporary_flow;
	
	if(tf < 0)
	{
		tf = 0;
	}
	
	switch(get_units())
	{
		case 0x01:
		{
			return tf / 10000000;
		}
		
		case 0x00:
		{
			return tf / 1000000;
		}
		
		case 0x0F:
		{
			return tf / 100000;
		}
		
		case 0x0E:
		{
			return tf / 10000;
		}
	}
	
	return 0;
}



int64_t get_energy(void)
{
	return energy;
}



void set_energy(int64_t e)
{
	energy = e;
	
	backup_energy();
}



void set_energy_offset(int64_t energy_offset)
{
	set_energy(energy + energy_offset);
}



uint16_t get_calorific_value(void)
{
	return calorific_value;
}



void set_calorific_value(uint16_t cv)
{
	calorific_value = cv;
	
	eeprom_write_parameter((uint8_t*)&calorific_value, CALORIFIC_VALUE);
}




void set_volume_offset(int64_t volume_offset)
{
	set_volume(volume + volume_offset);
}




int32_t get_temporary_flow(void)
{
	return temporary_flow;
}


int64_t get_volume(void)
{
	return volume;
}



void set_volume(int64_t value)
{
	volume = value;
	
	if(volume >= volume_max_value)
	{
		volume -= volume_max_value;
	}
	
	backup_volume();
}



void reset_volume(void)
{
	set_volume(0);
}



uint16_t get_hourly_usage_current_units(void)
{
	int32_t volume_L = current_hour_volume / 1000000000;
	
	if(volume_L < 0)
	{
		volume_L = 0;
	}
	
	switch(get_units())
	{
		case 0x01:
		{
			return volume_L / 10000;
		}
		
		case 0x00:
		{
			return volume_L / 1000;
		}
		
		case 0x0F:
		{
			return volume_L / 100;
		}
		
		case 0x0E:
		{
			return volume_L / 10;
		}
	}
	
	return 0;
}





uint16_t get_temporary_flow_period_latched(void)
{
	int32_t tfpl = temporary_flow_period_latched;
	
	if(tfpl < 0)
	{
		tfpl = 0;
	}
	
	switch(get_units())
	{
		case 0x01:
		{
			return tfpl / 10000000;
		}
		
		case 0x00:
		{
			return tfpl / 1000000;
		}
		
		case 0x0F:
		{
			return tfpl / 100000;
		}
		
		case 0x0E:
		{
			return tfpl / 10000;
		}
	}
	
	return 0;
}




uint16_t get_temporary_flow_period_latched_time_compressed(void)
{
	return temporary_flow_period_latched_time_compressed / (int64_t)60000;
}




uint16_t get_hourly_flow_period_latched(void)
{
	int32_t volume_L = hourly_flow_period_latched / 1000000000;
	
	if(volume_L < 0)
	{
		volume_L = 0;
	}
	
	switch(get_units())
	{
		case 0x01:
		{
			return volume_L / 10000;
		}
		
		case 0x00:
		{
			return volume_L / 1000;
		}
		
		case 0x0F:
		{
			return volume_L / 100;
		}
		
		case 0x0E:
		{
			return volume_L / 10;
		}
	}
	
	return 0;
}




uint8_t get_hourly_flow_period_latched_time_compressed(void)
{
	return hourly_flow_period_latched_time_compressed;
}




uint16_t get_temporary_flow_monthly_latched(void)
{
	int32_t tfml = temporary_flow_monthly_latched;
	
	if(tfml < 0)
	{
		tfml = 0;
	}
	
	switch(get_units())
	{
		case 0x01:
		{
			return tfml / 10000000;
		}
		
		case 0x00:
		{
			return tfml / 1000000;
		}
		
		case 0x0F:
		{
			return tfml / 100000;
		}
		
		case 0x0E:
		{
			return tfml / 10000;
		}
	}
	
	return 0;

}




uint16_t get_temporary_flow_monthly_latched_time_compressed(void)
{
	return temporary_flow_monthly_latched_time_compressed / (int64_t)60000;
}




uint16_t get_hourly_flow_monthly_latched(void)
{
	int32_t volume_L = hourly_flow_monthly_latched / 1000000000;
	
	if(volume_L < 0)
	{
		volume_L = 0;
	}
	
	switch(get_units())
	{
		case 0x01:
		{
			return volume_L / 10000;
		}
		
		case 0x00:
		{
			return volume_L / 1000;
		}
		
		case 0x0F:
		{
			return volume_L / 100;
		}
		
		case 0x0E:
		{
			return volume_L / 10;
		}
	}
	
	return 0;
}




uint16_t get_hourly_flow_monthly_latched_time_compressed(void)
{
	return hourly_flow_monthly_latched_time_compressed;
}



bool is_over_volume(uint32_t v)
{
	int32_t volume_L = hourly_flow_period_latched / 1000000000;
	
	if(volume_L < 0)
	{
		volume_L = 0;
	}
	
	int32_t volume_units = volume_L;
	
	switch(get_units())
	{
		case 0x01:
		{
			volume_units /= 10000;
		}
		
		case 0x00:
		{
			volume_units /= 1000;
		}
		
		case 0x0F:
		{
			volume_units /= 100;
		}
		
		case 0x0E:
		{
			volume_units /= 10;
		}
	}
	
	if(v > volume_units)
	{
		return true;
	}
	
	return false;
}


bool is_over_energy(uint32_t e)
{
	if(e > energy)
	{
		return true;
	}
	
	return false;
}



void set_volume_max_value_units(int64_t value)
{
	volume_max_value = value;
}



void update_volume_energy(void)
{
	static int counter = 0;
	
	if(counter >= 5 * 60)
	{
		counter = 0;
		
		static int64_t last_volume = 0;
		
		if((last_volume / 1000000000) != (volume / 1000000000))
		{
			last_volume = volume;
			
			uint8_t tab[24];
			
			memcpy(tab, (uint8_t*)&volume, 8);
			
			memcpy(tab + 8, (uint8_t*)&energy, 8);
			
			memcpy(tab + 16, (uint8_t*)&ev_tab_idx, 4);
			
			uint32_t crc32 = get_crc32(tab, 20);
			
			memcpy(tab + 20, (uint8_t*)&crc32, 4);
			
			eeprom_write(tab, EA(ENERGY_VOLUME) + (ev_tab_idx & 0x3f) * 24, 24);
		}
	}
	else
	{
		counter++;
	}
}



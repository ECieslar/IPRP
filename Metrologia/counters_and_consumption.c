
#include "counters_and_consumption.h"

#include "eeprom.h"

#include "date_time.h"

#include "flow.h"

#include "flash.h"

#include "crc.h"

#include "delay.h"

#include "status.h"

#include <string.h>



static uint8_t units;

static uint8_t counters_registration_period;

static uint16_t current_archive_tick;

static uint32_t counters_archive_last_time;

static uint32_t counters_archive_last_volume;

static uint32_t counters_archive_last_energy;

static uint32_t counters_archive_page_nr;

static const uint32_t max_counters_archive_page_nr = 1024;

static int counters_archive_flash_idx;

static int counters_archive_eeprom_idx;

static int counters_archive_idx;

static uint32_t volume_max_value;





static void prepare_archive_page_nr(void)
{
	counters_archive_page_nr = 0;
		
	eeprom_write_parameter((uint8_t*)&counters_archive_page_nr, COUNTERS_ARCHIVE_PAGE_NR);
}


static void init_archive_page_nr(void)
{
	eeprom_read_parameter((uint8_t*)&counters_archive_page_nr, COUNTERS_ARCHIVE_PAGE_NR);
	
	if(counters_archive_page_nr >= max_counters_archive_page_nr)
	{
		prepare_archive_page_nr();
	}
}



static void prepare_units(void)
{
	set_units(0x0E);
}




static void init_units(void)
{
	eeprom_read_parameter(&units, UNITS);
	
	switch(units)
	{
		case 0x01:
		case 0x00:
		case 0x0F:
		case 0x0E:
		{
			break;
		}
		
		default:
		{
			set_units(0x0E);
		}
	}
}



static void prepare_counters_registration_period(void)
{
	set_counters_registration_period(60);
}



static void init_counters_registration_period(void)
{
	eeprom_read_parameter(&counters_registration_period, COUNTERS_REGISTRATION_PERIOD);
	
	switch(counters_registration_period)
	{
		case 60:
		case 30:
		case 20:
		case 15:
		case 12:
		case 10:
		{
			break;
		}
		
		default:
		{
			set_counters_registration_period(60);
		}
	}
}




static void clear_eeprom_counters_archive()
{
	uint8_t tab[1024];
	
	for(int i = 0; i < 1024; i++)
	{
		tab[i] = 0xff;
	}
	
	eeprom_write(tab, EA(COUNTERS_ARCHIVE), EL(COUNTERS_ARCHIVE));

	counters_archive_eeprom_idx = 0;
}



static void clear_flash_counters_archive()
{
	
	for(int i = 0; i < 1024; i++)
	{
		flash_page_erase(i);
	}
	
	
	for(int i = 0; i < 256; i++)
	{
		flash_sector_erase(i);
	}
	
	//delay_test();
	
}




static void prepare_counters_archive(void)
{
	clear_eeprom_counters_archive();
	
	clear_flash_counters_archive();
	
	counters_archive_idx = 0;
}



static void init_counters_archive(void)
{
	uint8_t tab_eeprom[1024];
	
	eeprom_write(tab_eeprom, EA(COUNTERS_ARCHIVE), EL(COUNTERS_ARCHIVE));
	
	counters_archive_eeprom_idx = 0;
	
	counters_archive_flash_idx = 0;
	
	counters_archive_idx = 0;
	
	/*
	for(int i = 0; i < 1024; i += 32)
	{
		uint32_t* tab;
		
		tab = (uint32_t*)(tab_eeprom + i);
		
		uint32_t crc32 = get_crc32((uint8_t*)tab, 56);
		
		if(crc32 == tab[7])
		{
			if(tab[0] != 0xffffffff)
			{
				if(tab[0] > counters_archive_idx)
				{
					counters_archive_idx = tab[0];
					
					counters_archive_eeprom_idx = i >> 5;
				}
			}
		}
	}
	
	uint32_t max_flash_counters_archive = 0;
	
	for(int i = 0; i < 1024; i += 32)
	{
		uint32_t tab[8];
		
		flash_page_read((uint8_t*)tab, i, 0, 32);
		
		uint32_t crc32 = get_crc32((uint8_t*)tab, 56);
		
		if(crc32 == tab[7])
		{
			if(tab[0] != 0xffffffff)
			{
				if(tab[0] > max_flash_counters_archive)
				{
					max_flash_counters_archive = tab[0];
					
					counters_archive_flash_idx = i + 1;
				}
			}
		}
	}
	
	if(counters_archive_flash_idx >= 1024)
	{
		counters_archive_flash_idx = 0;
	}
	*/
}




void init_counters_and_consumption(void)
{
	init_eeprom();
	
	init_units();
	
	init_counters_registration_period();
	
	current_archive_tick = 60 * counters_registration_period;
	
	init_archive_page_nr();
	
	init_counters_archive();
}



void prepare_counters_and_consumption(void)
{
	prepare_archive_page_nr();
	
	prepare_units();
	
	prepare_counters_registration_period();
	
	prepare_counters_archive();
}



uint8_t get_units(void)
{
	return units;
}



bool set_units(uint8_t u)
{
	switch(u)
	{
		case 0x01:
		case 0x00:
		case 0x0F:
		case 0x0E:
		{
			break;
		}
		
		default:
		{
			return false;
		}
	}
	
	units = u;
	
	eeprom_write_parameter(&units, UNITS);
	
	return true;
}



static uint32_t convert_volume_units(int64_t volume)
{
	int32_t volume_L = volume / 1000000000;
	
	if(volume_L < 0)
	{
		volume_L = 0;
	}
	
	switch(units)
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




uint32_t get_volume_units(void)
{
	return convert_volume_units(get_volume());
}




void set_volume_units(uint32_t volume)
{
	set_status_flag(flag_volume_set);
	
	switch(units)
	{
		case 0x01:
		{
			set_volume(volume * 10000000000000);
			break;
		}
		
		case 0x00:
		{
			set_volume(volume * 1000000000000);
			break;
		}
		
		case 0x0F:
		{
			set_volume(volume * 100000000000);
			break;
		}
		
		case 0x0E:
		{
			set_volume(volume * 10000000000);
			break;
		}
	}
}



void set_volume_offset_units(int32_t volume_offset)
{
	set_status_flag(flag_volume_set);
	
	int64_t volume_offset_nL = (int64_t)volume_offset * (int64_t) 1000000000;
	
	switch(units)
	{
		case 0x01:
		{
			set_volume_offset(volume_offset_nL * 10000);
			break;
		}
		
		case 0x00:
		{
			set_volume_offset(volume_offset_nL * 1000);
			break;
		}
		
		case 0x0F:
		{
			set_volume_offset(volume_offset_nL * 100);
			break;
		}
		
		case 0x0E:
		{
			set_volume_offset(volume_offset_nL * 10);
			break;
		}
	}
}




static uint32_t convert_energy_units(int64_t energy)
{
	int32_t energy_Wh = energy / 1000000000;
	
	if(energy_Wh < 0)
	{
		energy_Wh = 0;
	}
	
	switch(units)
	{
		case 0x01:
		{
			return energy_Wh / 100000;
		}
		
		case 0x00:
		{
			return energy_Wh / 10000;
		}
		
		case 0x0F:
		{
			return energy_Wh / 1000;
		}
		
		case 0x0E:
		{
			return energy_Wh / 100;
		}
	}
	
	return 0;
}



uint32_t get_energy_units(void)
{
	return convert_energy_units(get_energy());
}




void set_energy_units(uint32_t energy)
{
	set_status_flag(flag_energy_set);
	
	switch(units)
	{
		case 0x01:
		{
			set_energy(energy * 100000000000000);
			break;
		}
		
		case 0x00:
		{
			set_energy(energy * 10000000000000);
			break;
		}
		
		case 0x0F:
		{
			set_energy(energy * 1000000000000);
			break;
		}
		
		case 0x0E:
		{
			set_energy(energy * 100000000000);
			break;
		}
	}
}



void set_energy_offset_units(int32_t energy_offset)
{
	set_status_flag(flag_energy_set);
	
	int64_t energy_offset_nWh = (int64_t)energy_offset * (int64_t) 1000000000;
	
	switch(units)
	{
		case 0x01:
		{
			set_energy_offset(energy_offset_nWh * 100000);
			break;
		}
		
		case 0x00:
		{
			set_energy_offset(energy_offset_nWh * 10000);
			break;
		}
		
		case 0x0F:
		{
			set_energy_offset(energy_offset_nWh * 1000);
			break;
		}
		
		case 0x0E:
		{
			set_energy_offset(energy_offset_nWh * 100);
			break;
		}
	}
}


uint8_t get_counters_registration_period(void)
{
	return counters_registration_period;
}



bool set_counters_registration_period(uint8_t crp)
{
	switch(crp)
	{
		case 60:
		case 30:
		case 20:
		case 15:
		case 12:
		case 10:
		{
			break;
		}
		
		default:
		{
			return false;
		}
	}
	
	counters_registration_period = crp;
	
	eeprom_write_parameter(&counters_registration_period, COUNTERS_REGISTRATION_PERIOD);
	
	return true;
}


/*
static void update_counters_archive(void)
{
	static uint32_t tab[256];
	
	tab[counters_archive_tab_idx++] = counters_archive_last_time;
	
	tab[counters_archive_tab_idx++] = get_volume_units();
	
	tab[counters_archive_tab_idx++] = get_energy_units();
	
	if(counters_archive_tab_idx >= 256)
	{
		flash_page_write((uint8_t*)tab, counters_archive_page_nr);
		
		counters_archive_page_nr++;
		
		if(counters_archive_page_nr >= 1024)
		{
			counters_archive_page_nr = 0;
		}
		
		counters_archive_tab_idx = 0;
	}
}
*/




static void update_counters_archive(void)
{
	static uint32_t tab[8];
	
	tab[0] = counters_archive_idx++;
	
	tab[1] = counters_archive_last_time;
	
	int64_t volume = get_volume();
	
	tab[2] = volume & 0xffffffff;
	
	tab[3] = (volume >> 32) & 0xffffffff;
	
	int64_t energy = get_energy();
	
	tab[4] = energy & 0xffffffff;
	
	tab[5] = (energy >> 32) & 0xffffffff;
	
	tab[6] = 0xffffffff;
	
	tab[7] = get_crc32((uint8_t*)tab, 28);
	
	eeprom_write((uint8_t*)tab, EA(COUNTERS_ARCHIVE) + counters_archive_eeprom_idx, 32);
	
	counters_archive_eeprom_idx += 32;
	
	if(counters_archive_eeprom_idx >= 1024)
	{
		uint8_t tab_eeprom[1024];
		
		eeprom_read(tab_eeprom, EA(COUNTERS_ARCHIVE), EL(COUNTERS_ARCHIVE));
		
		flash_page_write(tab_eeprom, counters_archive_flash_idx++);
		
		if(counters_archive_flash_idx >= 1024)
		{
			counters_archive_flash_idx = 0;
		}
		
		clear_eeprom_counters_archive();
	}
}



void update_archive(void)
{
	if(current_archive_tick >= 60 * counters_registration_period)
	{
		current_archive_tick = 0;
		
		counters_archive_last_time = get_unix_time_utc();
		
		counters_archive_last_volume = get_volume_units();
		
		counters_archive_last_energy = get_energy_units();
		
		update_flow_time_compressed();
		
		update_counters_archive();
	}
	else
	{
		current_archive_tick++;
	}
}



uint32_t get_counters_archive_last_time(void)
{
	return counters_archive_last_time;
}




uint32_t get_counters_archive_last_volume(void)
{
	return counters_archive_last_volume;
}





uint32_t get_counters_archive_last_energy(void)
{
	return counters_archive_last_energy;
}



static bool get_counters_archive_tab_data(uint32_t* tab, uint32_t* time, uint32_t* volume, uint32_t* energy)
{
	if(tab[7] == get_crc32((uint8_t*)tab, 28))
	{
		*time = tab[1];
		
		int64_t i64volume = tab[3];
		
		i64volume <<= 32;
		
		i64volume |= tab[2];
		
		*volume = convert_volume_units(i64volume);
		
		int64_t i64energy = tab[5];
		
		i64energy <<= 32;
		
		i64energy |= tab[4];
		
		*energy = convert_energy_units(i64energy);
		
		return true;
	}
	
	return false;
}



static bool get_counters_archive_eeprom_data(int idx, uint32_t* time, uint32_t* volume, uint32_t* energy)
{
	if(idx >= 32)
	{
		return false;
	}
	
	static uint32_t tab[8];
	
	eeprom_read((uint8_t*)tab, EA(COUNTERS_ARCHIVE) + (idx << 5), 32);
	
	return get_counters_archive_tab_data(tab, time, volume, energy);
}




static bool get_counters_archive_flash_data(int page_idx, int idx, uint32_t* time, uint32_t* volume, uint32_t* energy)
{
	if(idx >= 32)
	{
		return false;
	}
	
	static uint32_t tab[8];
	
	flash_page_read((uint8_t*)tab, page_idx, idx << 5, 32);
	
	return get_counters_archive_tab_data(tab, time, volume, energy);
}



int get_counters_archive_acc_time(uint32_t start_datetime, uint32_t end_datetime, uint8_t* output_data, uint16_t max_output_length)
{
	int length = 0;
	
	uint32_t time, volume, energy;
	
	uint32_t left = 0, right = 1024 * 32 - 1;
	
	uint32_t middle = (left + right) << 1;
	
	while(left < right)
	{
		uint32_t middle_address = (counters_archive_page_nr << 5) + middle;
		
		if(middle_address >= 1024 * 32)
		{
			middle_address -= 1024 * 32;
		}
		
		if(get_counters_archive_flash_data(middle_address >> 5, middle_address & 0x1f, &time, &volume, &energy))
		{
			if(time < start_datetime)
			{
				left = middle;
			}
			else
			{
				right = middle;
			}
		}
		else
		{
			left = middle;
		}
		
		middle = (left + right) << 1;
	}
	
	uint32_t flash_idx = middle;
	
	while((length + 12 < max_output_length) && (flash_idx < 1024 * 32))
	{
		uint32_t flash_idx_address = (counters_archive_page_nr << 5) + flash_idx;
		
		if(flash_idx_address >= 1024 * 32)
		{
			flash_idx_address -= 1024 * 32;
		}
		
		if(get_counters_archive_flash_data(flash_idx_address >> 5, flash_idx_address & 0x1f, &time, &volume, &energy))
		{
			if((time >= start_datetime) && (time <= end_datetime))
			{
				memcpy(output_data + length, &time, 4);
				
				memcpy_to_buffer_uint32_t_littleendian(output_data + length + 4, volume);
				
				memcpy_to_buffer_uint32_t_littleendian(output_data + length + 8, energy);
				
				length += 12;
			}
		}
		else
		{
			break;
		}
	}
	
	int idx = 0;
	
	while((length + 12 < max_output_length) && (idx < 32))
	{
		if(get_counters_archive_eeprom_data(idx++, &time, &volume, &energy))
		{
			if((time >= start_datetime) && (time <= end_datetime))
			{
				memcpy(output_data + length, &time, 4);
				
				memcpy_to_buffer_uint32_t_littleendian(output_data + length + 4, volume);
				
				memcpy_to_buffer_uint32_t_littleendian(output_data + length + 8, energy);
				
				length += 12;
			}
		}
		else
		{
			break;
		}
	}
	
	return length;
}




uint32_t get_volume_max_value(void)
{
	return volume_max_value;
}




void set_volume_max_value(uint32_t volume)
{
	switch(units)
	{
		case 0x01:
		{
			set_volume_max_value_units(volume_max_value * 10000000000000);
			break;
		}
		
		case 0x00:
		{
			set_volume_max_value_units(volume_max_value * 1000000000000);
			break;
		}
		
		case 0x0F:
		{
			set_volume_max_value_units(volume_max_value * 100000000000);
			break;
		}
		
		case 0x0E:
		{
			set_volume_max_value_units(volume_max_value * 10000000000);
			break;
		}
	}
}


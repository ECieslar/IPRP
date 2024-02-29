
#include "prepaid.h"

#include "eeprom.h"

#include "date_time.h"

#include "valve.h"

#include "flow.h"


static uint8_t prepaid_topup_id;

static uint32_t prepaid_topup_end_time;

static uint32_t prepaid_topup_end_volume;

static uint32_t prepaid_topup_end_energy;

static uint8_t prepaid_valve_control;



static void init_prepaid_topup_id(void)
{
	eeprom_read_parameter((uint8_t*)&prepaid_topup_id, PREPAID_TOPUP_ID);
}



static void prepare_prepaid_topup_id(void)
{
	set_prepaid_topup_id(0);
}



static void init_prepaid_topup_end_time(void)
{
	eeprom_read_parameter((uint8_t*)&prepaid_topup_end_time, PREPAID_TOPUP_END_TIME);
}



static void prepare_prepaid_topup_end_time(void)
{
	set_prepaid_topup_end_time(0);
}



static void init_prepaid_topup_end_volume(void)
{
	eeprom_read_parameter((uint8_t*)&prepaid_topup_end_volume, PREPAID_TOPUP_END_VOLUME);
}



static void prepare_prepaid_topup_end_volume(void)
{
	set_prepaid_topup_end_volume(0);
}



static void init_prepaid_topup_end_energy(void)
{
	eeprom_read_parameter((uint8_t*)&prepaid_topup_end_energy, PREPAID_TOPUP_END_ENERGY);
}



static void prepare_prepaid_topup_end_energy(void)
{
	set_prepaid_topup_end_energy(0);
}



static void init_prepaid_valve_control(void)
{
	eeprom_read_parameter((uint8_t*)&prepaid_valve_control, PREPAID_VALVE_CONTROL);
	
	if((prepaid_valve_control != 0) && (prepaid_valve_control != 1))
	{
		set_prepaid_valve_control(1);
	}
}



static void prepare_prepaid_valve_control(void)
{
	set_prepaid_valve_control(1);
}



void init_prepaid(void)
{
	init_eeprom();
	
	init_prepaid_topup_id();
	
	init_prepaid_topup_end_time();
	
	init_prepaid_topup_end_volume();
	
	init_prepaid_topup_end_energy();
	
	init_prepaid_valve_control();
}



void prepare_prepaid(void)
{
	prepare_prepaid_topup_id();
	
	prepare_prepaid_topup_end_time();
	
	prepare_prepaid_topup_end_volume();
	
	prepare_prepaid_topup_end_energy();
	
	prepare_prepaid_valve_control();
}


void check_prepaid(void)
{
	if(get_unix_time_local() >= prepaid_topup_end_time)
	{
		//valve_off();
	}
	
	if((prepaid_topup_end_volume != 0xffffffff) && is_over_volume(prepaid_topup_end_volume))
	{
		//valve_off();
	}

	if((prepaid_topup_end_energy != 0xffffffff) && is_over_energy(prepaid_topup_end_energy))
	{
		//valve_off();
	}

}



uint8_t get_prepaid_topup_id(void)
{
	return prepaid_topup_id;
}



void set_prepaid_topup_id(uint8_t pti)
{
	prepaid_topup_id = pti;
	
	eeprom_write_parameter((uint8_t*)&prepaid_topup_id, PREPAID_TOPUP_ID);
}



uint32_t get_prepaid_topup_end_time(void)
{
	return prepaid_topup_end_time;
}



void set_prepaid_topup_end_time(uint32_t ptet)
{
	prepaid_topup_end_time = ptet;
	
	eeprom_write_parameter((uint8_t*)&prepaid_topup_end_time, PREPAID_TOPUP_END_TIME);
}



uint32_t get_prepaid_topup_end_volume(void)
{
	return prepaid_topup_end_volume;
}



void set_prepaid_topup_end_volume(uint32_t ptev)
{
	prepaid_topup_end_volume = ptev;
	
	eeprom_write_parameter((uint8_t*)&prepaid_topup_end_volume, PREPAID_TOPUP_END_VOLUME);
}



uint32_t get_prepaid_topup_end_energy(void)
{
	return prepaid_topup_end_energy;
}



void set_prepaid_topup_end_energy(uint32_t ptee)
{
	prepaid_topup_end_energy = ptee;
	
	eeprom_write_parameter((uint8_t*)&prepaid_topup_end_energy, PREPAID_TOPUP_END_ENERGY);
}




uint8_t get_prepaid_valve_control(void)
{
	return prepaid_valve_control;
}



bool set_prepaid_valve_control(uint8_t pvc)
{
	if((pvc != 0) && (pvc != 1))
	{
		return false;
	}
	
	prepaid_valve_control = pvc;
	
	eeprom_write_parameter((uint8_t*)&prepaid_valve_control, PREPAID_VALVE_CONTROL);
	
	return true;
}

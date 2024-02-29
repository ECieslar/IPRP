

#include "mkg_module.h"

#include "eeprom.h"

#include "date_time.h"



static uint32_t mkg_module_type;

static uint32_t mkg_hardware_version;

static uint32_t mkg_software_version;

static uint32_t mkg_serial_number;



static void init_mkg_module_type(void)
{
	eeprom_read_parameter((uint8_t*)&mkg_module_type, MKG_MODULE_TYPE);
}



static void prepare_mkg_module_type(void)
{
	
}



static void init_mkg_hardware_version(void)
{
	eeprom_read_parameter((uint8_t*)&mkg_hardware_version, MKG_HARDWARE_VERSION);
}



static void prepare_mkg_hardware_version(void)
{
	
}



static void init_mkg_software_version(void)
{
	eeprom_read_parameter((uint8_t*)&mkg_software_version, MKG_SOFTWARE_VERSION);
}



static void prepare_mkg_software_version(void)
{
	
}



static void init_mkg_serial_number(void)
{
	eeprom_read_parameter((uint8_t*)&mkg_serial_number, MKG_SERIAL_NUMBER);
}



static void prepare_mkg_serial_number(void)
{
	
}



void init_mkg_module(void)
{
	init_eeprom();
	
	init_mkg_module_type();
	
	init_mkg_hardware_version();
	
	init_mkg_software_version();
	
	init_mkg_serial_number();
}



void prepare_mkg_module(void)
{
	prepare_mkg_module_type();
	
	prepare_mkg_hardware_version();
	
	prepare_mkg_software_version();
	
	prepare_mkg_serial_number();
}



uint32_t get_mkg_module_type(void)
{
	return mkg_module_type;
}



void set_mkg_module_type(uint32_t mmt)
{
	if(mkg_module_type != mmt)
	{
		mkg_module_type = mmt;
		
		eeprom_write_parameter((uint8_t*)&mkg_module_type, MKG_MODULE_TYPE);
	}
}



uint32_t get_mkg_hardware_version(void)
{
	return mkg_hardware_version;
}



void set_mkg_hardware_version(uint32_t mhv)
{
	if(mkg_hardware_version != mhv)
	{
		mkg_hardware_version = mhv;
		
		eeprom_write_parameter((uint8_t*)&mkg_hardware_version, MKG_HARDWARE_VERSION);
	}
}



uint32_t get_mkg_software_version(void)
{
	return mkg_software_version;
}



void set_mkg_software_version(uint32_t msv)
{
	if(mkg_software_version != msv)
	{
		mkg_software_version = msv;
		
		eeprom_write_parameter((uint8_t*)&mkg_software_version, MKG_SOFTWARE_VERSION);
	}
}




uint32_t get_mkg_serial_number(void)
{
	return mkg_serial_number;
}



void set_mkg_serial_number(uint32_t mmt)
{
	if(mkg_serial_number != mmt)
	{
		mkg_serial_number = mmt;
		
		eeprom_write_parameter((uint8_t*)&mkg_serial_number, MKG_SERIAL_NUMBER);
	}
}



static uint8_t get_octet(int8_t byte)
{
	if(byte >= 0)
	{
		return ((byte % 10) << 4) | (byte / 10);
	}
	
	byte = -byte;
	
	return ((byte % 10) << 4) | (byte / 10) | 8;
}



static int8_t get_byte(uint8_t octet)
{
	int8_t byte = 10 * (octet & 7);
	
	byte += octet >> 4;
	
	if(octet & 8)
	{
		return -byte;
	}
	
	return byte;
}



uint8_t get_mkg_gsm_clock(uint8_t* data)
{
	rtc_t dt;
	
	get_date_time_utc(&dt);
	
	data[0] = get_octet(dt.year - 2000);
	
	data[1] = get_octet(dt.month);
	
	data[2] = get_octet(dt.day);
	
	data[3] = get_octet(dt.hour);
	
	data[4] = get_octet(dt.minute);
	
	data[5] = get_octet(dt.second);
	
	data[6] = get_octet(get_time_zone_offset());
	
	return 7;
}


bool set_mkg_gsm_clock(uint8_t length, uint8_t* data)
{
	if(length != 7)
	{
		return false;
	}
	
	rtc_t dt;
	
	dt.year = 2000 + get_byte(data[0]);
	
	if(dt.year < 2000) return false;
	
	dt.month = get_byte(data[1]);
	
	if((dt.month == 0) || (dt.month > 12)) return false;
	
	dt.day = get_byte(data[2]);
	
	if((dt.day == 0) || (dt.day > 31)) return false;
	
	dt.hour = get_byte(data[3]);
	
	if((dt.hour == 0) || (dt.hour > 24)) return false;
	
	dt.minute = get_byte(data[4]);
	
	if((dt.minute == 0) || (dt.minute > 60)) return false;
	
	dt.second = get_byte(data[5]);
	
	if((dt.second == 0) || (dt.second > 60)) return false;
	
	set_date_time_utc(&dt);
	
	set_time_zone_offset(get_byte(data[6]));
	
	return true;
}




#include "location.h"

#include "eeprom.h"


static int32_t location_latitude;

static int32_t location_longitude;

static const int32_t min_location_latitude = -90000000;

static const int32_t max_location_latitude = 90000000;

static const int32_t min_location_longitude = -180000000;

static const int32_t max_location_longitude = 180000000;



static void init_location_latitude(void)
{
	eeprom_read_parameter((uint8_t*)&location_latitude, LOCATION_LATITUDE);
	
	if(location_latitude < min_location_latitude)
	{
		set_location_latitude(min_location_latitude);
	}
	else
	{
		if(location_latitude > max_location_latitude)
		{
			set_location_latitude(max_location_latitude);
		}
	}
}



static void prepare_location_latitude(void)
{
	set_location_latitude(0);
}



static void init_location_longitude(void)
{
	eeprom_read_parameter((uint8_t*)&location_longitude, LOCATION_LONGITUDE);
	
	if(location_longitude < min_location_longitude)
	{
		set_location_longitude(min_location_longitude);
	}
	else
	{
		if(location_longitude > max_location_longitude)
		{
			set_location_longitude(max_location_longitude);
		}
	}
}



static void prepare_location_longitude(void)
{
	set_location_longitude(0);
}


void init_location(void)
{
	init_eeprom();
	
	init_location_latitude();
	
	init_location_longitude();
}


void prepare_location(void)
{
	prepare_location_latitude();
	
	prepare_location_longitude();
}


int32_t get_location_latitude(void)
{
	return location_latitude;
}



int32_t get_location_longitude(void)
{
	return location_longitude;
}



bool set_location_latitude(int32_t latitude)
{
	if((latitude < min_location_latitude) || (latitude > max_location_latitude))
	{
		return false;
	}
		
	location_latitude = latitude;
	
	eeprom_write_parameter((uint8_t*)&location_latitude, LOCATION_LATITUDE);

	return true;
}



bool set_location_longitude(int32_t longitude)
{
	if((longitude < min_location_longitude) || (longitude > max_location_longitude))
	{
		return false;
	}
	
	location_longitude = longitude;
	
	eeprom_write_parameter((uint8_t*)&location_longitude, LOCATION_LONGITUDE);

	return true;
}




#include "information.h"

#include "eeprom.h"

#include <string.h>


static uint32_t serial_number;

static uint8_t client_id[33];

static uint8_t gas_meter_serial_nbr[21];

static uint8_t device_type_name[21];



static void init_serial_number(void)
{
	eeprom_read_parameter((uint8_t*)&serial_number, SERIAL_NUMBER);
}



static void prepare_serial_number(void)
{
	set_serial_number(0);
}



static void init_client_id(void)
{
	eeprom_read_parameter(client_id, CLIENT_ID);
	
	if(client_id[0] > 32)
	{
		client_id[0] = 32;
		
		eeprom_write(client_id, EA(CLIENT_ID), 1);
	}
}



static void prepare_client_id(void)
{
	set_client_id(0, 0);
}



static void init_gas_meter_serial_nbr(void)
{
	eeprom_read_parameter(gas_meter_serial_nbr, GAS_METER_SERIAL_NBR);
	
	if(gas_meter_serial_nbr[0] > 20)
	{
		gas_meter_serial_nbr[0] = 20;
		
		eeprom_write(gas_meter_serial_nbr, EA(GAS_METER_SERIAL_NBR), 1);
	}
}



static void prepare_gas_meter_serial_nbr(void)
{
	uint8_t value[] = {'0', 0};
	
	set_gas_meter_serial_nbr(1, value);
}



static void init_device_type_name(void)
{
	eeprom_read_parameter(device_type_name, DEVICE_TYPE_NAME);
	
	if(device_type_name[0] > 20)
	{
		device_type_name[0] = 20;
		
		eeprom_write(device_type_name, EA(DEVICE_TYPE_NAME), 1);
	}
}



static void prepare_device_type_name(void)
{
	char name[] = "GASMETER";
	
	set_device_type_name(strlen(name), (uint8_t*)name);
}



void init_information(void)
{
	init_eeprom();
	
	init_serial_number();
	
	init_client_id();
	
	init_gas_meter_serial_nbr();
	
	init_device_type_name();
}


void prepare_information(void)
{
	prepare_serial_number();
	
	prepare_client_id();
	
	prepare_gas_meter_serial_nbr();
	
	prepare_device_type_name();
}


uint32_t get_serial_number(void)
{
	return serial_number;
}



void set_serial_number(uint32_t sn)
{
	serial_number = sn;
	
	eeprom_write_parameter((uint8_t*)&serial_number, SERIAL_NUMBER);
}



uint8_t get_client_id(uint8_t* ci)
{
	for(int i = 0; i < client_id[0]; i++)
	{
		ci[i] = client_id[i + 1];
	}
	
	return client_id[0];
}



bool set_client_id(uint8_t length, uint8_t* ci)
{
	if(length > 32)
	{
		return false;
	}
	
	client_id[0] = length;
	
	for(int i = 0; i < client_id[0]; i++)
	{
		client_id[i + 1] = ci[i];
	}
	
	eeprom_write_parameter(client_id, CLIENT_ID);
	
	return true;
}



uint8_t get_gas_meter_serial_nbr(uint8_t* gms_nbr)
{
	for(int i = 0; i < gas_meter_serial_nbr[0]; i++)
	{
		gms_nbr[i] = gas_meter_serial_nbr[i + 1];
	}
	
	return gas_meter_serial_nbr[0];
}



bool set_gas_meter_serial_nbr(uint8_t length, uint8_t* gms_nbr)
{
	if(length > 20)
	{
		return false;
	}
	
	gas_meter_serial_nbr[0] = length;
	
	for(int i = 0; i < gas_meter_serial_nbr[0]; i++)
	{
		gas_meter_serial_nbr[i + 1] = gms_nbr[i];
	}
	
	eeprom_write_parameter(gas_meter_serial_nbr, GAS_METER_SERIAL_NBR);
	
	return true;
}



uint8_t get_device_type_name(uint8_t* dtn)
{
	for(int i = 0; i < device_type_name[0]; i++)
	{
		dtn[i] = device_type_name[i + 1];
	}
	
	return device_type_name[0];
}



bool set_device_type_name(uint8_t length, uint8_t* dtn)
{
	if(length > 20)
	{
		return false;
	}
	
	device_type_name[0] = length;
	
	for(int i = 0; i < device_type_name[0]; i++)
	{
		device_type_name[i + 1] = dtn[i];
	}
	
	eeprom_write_parameter(device_type_name, DEVICE_TYPE_NAME);
	
	return true;
}


#include "calorimeter.h"

#include "hub_communication.h"

#include "smart_gas.h"

#include <string.h>



static uint32_t modbus_start_address_length;

static uint16_t calorimeter_calorific_value;


void init_calorimeter(void)
{
	modbus_start_address_length = 0;
	
	calorimeter_calorific_value = 11611;
}



uint32_t get_modbus_start_address_length(void)
{
	return modbus_start_address_length;
}



void set_modbus_start_address_length(uint32_t msal)
{
	modbus_start_address_length = msal;
}



void get_cal_modbus_holding_registers(uint8_t* data)
{
	send_hub_data(hcf_calorimeter_read_holding_registers, (uint8_t*)&modbus_start_address_length, 4);
}


void set_cal_modbus_holding_registers(uint8_t length, uint8_t* data)
{
	send_hub_data(hcf_calorimeter_write_holding_registers, (uint8_t*)&modbus_start_address_length, 4);
}


void get_cal_modbus_input_registers(uint8_t* data)
{
	send_hub_data(hcf_calorimeter_read_input_registers, (uint8_t*)&modbus_start_address_length, 4);
}




void calorimeter_modbus_holding_registers(uint8_t* data, uint16_t length)
{
	smart_gas_calorimeter_response(cp_modbus_holding_registers, data, length);
}



void calorimeter_modbus_input_registers(uint8_t* data, uint16_t length)
{
	smart_gas_calorimeter_response(cp_modbus_input_registers, data, length);
}



void set_calorimeter_calorific_value(uint8_t* data)
{
	float value;
	
	memcpy(&value, data, 4);
	
	calorimeter_calorific_value = value;
}


uint16_t get_calorimeter_calorific_value(void)
{
	return calorimeter_calorific_value;
}



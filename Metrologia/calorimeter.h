#ifndef CALORIMETER_H_
#define CALORIMETER_H_


#include "stdbool.h"

#include "stdint.h"


enum calorimeter_parameters
{
	cp_modbus_holding_registers	= 0x01,
	cp_modbus_input_registers		= 0x02,
};



void init_calorimeter(void);

uint32_t get_modbus_start_address_length(void);

void set_modbus_start_address_length(uint32_t msal);

void get_cal_modbus_holding_registers(uint8_t* data);

void set_cal_modbus_holding_registers(uint8_t length, uint8_t* data);

void get_cal_modbus_input_registers(uint8_t* data);

void get_calorimeter_parameter_response(enum calorimeter_parameters cp, uint8_t* data, uint16_t length);

void set_calorimeter_calorific_value(uint8_t* value);

uint16_t get_calorimeter_calorific_value(void);

void calorimeter_modbus_holding_registers(uint8_t* data, uint16_t length);

void calorimeter_modbus_input_registers(uint8_t* data, uint16_t length);




#endif /*CALORIMETER_H_*/


#include "calibration.h"

#include "eeprom.h"



static uint8_t temperatures_count;

static uint8_t flows_count;

static int8_t temperatures[8];

static uint16_t flows[8];

static int16_t coefficients[8][8];



static void init_temperatures_count()
{
	eeprom_read_parameter(&temperatures_count, CALIBRATION_TEMPERATURES_COUNT);
	
	if((temperatures_count == 0) || (temperatures_count > 8))
	{
		temperatures_count = 1;
		
		eeprom_write_parameter(&temperatures_count, CALIBRATION_TEMPERATURES_COUNT);
	}
}



static void prepare_temperatures_count()
{
	temperatures_count = 1;
		
	eeprom_write_parameter(&temperatures_count, CALIBRATION_TEMPERATURES_COUNT);
}



static void init_flows_count()
{
	eeprom_read_parameter(&flows_count, CALIBRATION_FLOWS_COUNT);
	
	if((flows_count == 0) || (flows_count > 8))
	{
		flows_count = 1;
		
		eeprom_write_parameter(&flows_count, CALIBRATION_FLOWS_COUNT);
	}
}



static void prepare_flows_count()
{
	flows_count = 1;
		
	eeprom_write_parameter(&flows_count, CALIBRATION_FLOWS_COUNT);
}



static void init_temperatures()
{
	eeprom_read_parameter((uint8_t*)temperatures, CALIBRATION_TEMPERATURES);
}



static void prepare_temperatures()
{
	for(int i = 0; i < 8; i++)
	{
		temperatures[i] = 25;
	}
	
	eeprom_write_parameter((uint8_t*)temperatures, CALIBRATION_TEMPERATURES);
}



static void init_flows()
{
	eeprom_read_parameter((uint8_t*)flows, CALIBRATION_FLOWS);
}



static void prepare_flows()
{
	for(int i = 0; i < 8; i++)
	{
		flows[i] = 1000;
	}
	
	eeprom_write_parameter((uint8_t*)temperatures, CALIBRATION_FLOWS);
}



static void init_coefficients()
{
	eeprom_read_parameter((uint8_t*)coefficients, CALIBRATION_COEFFICIENTS);
}



static void prepare_coefficients()
{
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			coefficients[i][j] = 0;
		}
	}
	
	eeprom_write_parameter((uint8_t*)coefficients, CALIBRATION_COEFFICIENTS);
}



void init_calibration(void)
{
	init_temperatures_count();
	
	init_flows_count();
	
	init_temperatures();
	
	init_flows();
	
	init_coefficients();
}



void prepare_calibration(void)
{
	prepare_temperatures_count();
	
	prepare_flows_count();
	
	prepare_temperatures();
	
	prepare_flows();
	
	prepare_coefficients();
}




bool set_calibration(uint8_t* data, int length)
{
	if(length < 7)
	{
		return false;
	}
	
	uint8_t t_count = data[0];

	uint8_t f_count = data[1];
	
	if((t_count == 0) || (t_count > 8))
	{
		return false;
	}

	if((f_count == 0) || (f_count > 8))
	{
		return false;
	}
	
	if(2 + t_count + f_count * 2 + t_count * f_count * 2 != length)
	{
		return false;
	}
	
	temperatures_count = t_count;
	
	flows_count = f_count;
	
	int idx = 2;
	
	for(int i = 0; i < temperatures_count; i++)
	{
		temperatures[i] = data[idx++];
	}
	
	for(int i = 0; i < flows_count; i++)
	{
		flows[i] = data[idx + 1];
		
		flows[i] <<= 8;
		
		flows[i] |= data[idx];
		
		idx += 2;
	}
	
	for(int i = 0; i < temperatures_count; i++)
	{
		for(int j = 0; j < flows_count; j++)
		{
			coefficients[i][j] = data[idx + 1];
			
			coefficients[i][j] <<= 8;
			
			coefficients[i][j] |= data[idx];
			
			idx += 2;
		}
	}
	
	eeprom_write_parameter(&temperatures_count, CALIBRATION_TEMPERATURES_COUNT);
	
	eeprom_write_parameter(&flows_count, CALIBRATION_FLOWS_COUNT);
	
	eeprom_write_parameter((uint8_t*)temperatures, CALIBRATION_TEMPERATURES);
	
	eeprom_write_parameter((uint8_t*)flows, CALIBRATION_FLOWS);
	
	eeprom_write_parameter((uint8_t*)coefficients, CALIBRATION_COEFFICIENTS);
 	
	return true;
}




int get_calibration(uint8_t* data)
{
	int idx = 0;
	
	data[idx++] = temperatures_count;
	
	data[idx++] = flows_count;
	
	for(int i = 0; i < temperatures_count; i++)
	{
		data[idx++] = temperatures[i];
	}
	
	for(int i = 0; i < flows_count; i++)
	{
		data[idx++] = flows[i] & 0xff;
		
		data[idx++] = flows[i] >> 8;
	}
	
	for(int i = 0; i < temperatures_count; i++)
	{
		for(int j = 0; j < flows_count; j++)
		{
			data[idx++] = coefficients[i][j] & 0xff;
			
			data[idx++] = coefficients[i][j] >> 8;
		}
	}
	
	return 2 + temperatures_count + flows_count * 2 + temperatures_count * flows_count * 2;
}




#include "pressure_meter.h"

#include "timers.h"


static uint32_t pressure;

static int32_t temperature;

static uint32_t humidity;

const uint32_t standard_pressure = 256 * 101325;//101325 hPa, rozdzielczosc 1/256 Pa

const int32_t standard_temperature = 0 * 100;//0 st. C, rozdzielczosc 0,01 st. C

const uint32_t standard_humidity = 0 * 1024;//0 %, rozdzielczosc 1/1024 %




void init_pressure_meter(void)
{
	pressure = standard_pressure;
	
	temperature = standard_temperature;
	
	humidity = standard_humidity;
}



uint32_t get_pressure(void)
{
	return pressure;
}


int16_t get_temperature_additional(void)
{
	return temperature;
}


uint32_t get_humidity(void)
{
	return humidity;
}


void set_pressure_meter_values(uint8_t* data, int length)
{
	if(length >= 8)
	{
		pressure = data[0];
		pressure <<= 8;
		pressure |= data[1];
		pressure <<= 8;
		pressure |= data[2];
		pressure <<= 8;
		pressure |= data[3];
		
		temperature = data[4];
		temperature <<= 8;
		temperature |= data[5];
		temperature <<= 8;
		temperature |= data[6];
		temperature <<= 8;
		temperature |= data[7];
		
		if(length >= 12)
		{
			humidity = data[8];
			humidity <<= 8;
			humidity |= data[9];
			humidity <<= 8;
			humidity |= data[10];
			humidity <<= 8;
			humidity |= data[11];
		}
	}
}
	


uint32_t get_standard_pressure(void)
{
	return standard_pressure;
}


int16_t get_standard_temperature_additional(void)
{
	return standard_temperature;
}


uint32_t get_standard_humidity(void)
{
	return standard_humidity;
}


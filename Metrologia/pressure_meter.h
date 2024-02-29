#ifndef PRESSURE_METER_H_
#define PRESSURE_METER_H_


#include "stdbool.h"

#include "extras.h"



void init_pressure_meter(void);

void check_pressure_meter(void);

uint32_t get_pressure(void);

int16_t get_temperature_additional(void);

void pressure_meter_tick(void);

bool is_pressure_meter_measuring(void);

void set_pressure_meter_values(uint8_t* data, int length);

uint32_t get_humidity(void);

uint32_t get_standard_pressure(void);

int16_t get_standard_temperature_additional(void);

uint32_t get_standard_humidity(void);





#endif /*PRESSURE_METER_H_*/

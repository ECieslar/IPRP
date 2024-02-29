#ifndef INPUT_VOLTAGE_H_
#define INPUT_VOLTAGE_H_


#include "stdbool.h"

#include "extras.h"

void init_input_voltage(void);

uint16_t get_input_voltage(void);

void input_voltage_tick(void);

void check_voltage_measure(void);
	

#endif /*INPUT_VOLTAGE_H_*/

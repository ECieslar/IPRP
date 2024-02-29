#ifndef POWER_H_
#define POWER_H_


#include "stdbool.h"




void power_save(void);

void init_power_mode(void);

bool is_low_power_mode(void);

void invoke_low_power_mode(void);

//void power_set_low_mode(void);


#endif /*POWER_H_*/

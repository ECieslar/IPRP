#ifndef CLOCK_H_
#define CLOCK_H_


#include "stdbool.h"

#include "stdint.h"

bool time_calibration_calibrating(void);



void init_clock(void);

void check_date_time(void);

uint64_t get_time_ms(void);


#endif /*CLOCK_H_*/

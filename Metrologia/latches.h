#ifndef LATCHES_H_
#define LATCHES_H_


#include "inttypes.h"

#include "stdbool.h"



void init_latches(void);

void prepare_latches(void);

uint8_t get_monthly_data_latch_day(void);

bool set_monthly_data_latch_day(uint8_t mdld);

uint8_t get_monthly_and_daily_data_latch_hour(void);

bool set_monthly_and_daily_data_latch_hour(uint8_t maddlh);

uint8_t get_monthly_and_daily_data_latch_local_time(void);

bool set_monthly_and_daily_data_latch_local_time(uint8_t maddllt);

void update_latches(void);

uint32_t get_last_daily_latch_time(void);

uint32_t get_last_daily_latch_volume(void);

uint32_t get_last_daily_latch_energy(void);




#endif /*LATCHES_H_*/

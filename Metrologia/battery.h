#ifndef BATTERY_H_
#define BATTERY_H_


#include "inttypes.h"

#include "stdbool.h"


#define MAX_CAPACITY_UAH	19000000

void init_battery(void);

uint32_t get_battery_capacity(void);

void battery_tick(void);

uint8_t get_battery_level(void);

bool set_batter_level(uint8_t level);


#endif /*BATTERY_H_*/

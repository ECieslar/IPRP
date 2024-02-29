
#ifndef KEYS_H_
#define KEYS_H_

#include "stdbool.h"

#include "extras.h"



#define KM_SW1			1
#define KM_SW2			2
#define KM_SW3			3


void init_keys(void);

bool is_sw_1(void);

bool is_sw_2(void);

bool is_sw_3(void);

void keys_tick(void);

uint32_t get_keys_tick(void);

void keys_prepare_deep_sleep_entry(void);

void keys_prepare_deep_sleep_exit(void);

bool is_key_activation(void);

void check_key_activation(void);


#endif /*KEYS_H_*/

#ifndef PREPAID_H_
#define PREPAID_H_


#include "extras.h"

#include "stdbool.h"



void init_prepaid(void);

void prepare_prepaid(void);

void check_prepaid(void);

uint8_t get_prepaid_topup_id(void);

void set_prepaid_topup_id(uint8_t pti);

uint32_t get_prepaid_topup_end_time(void);

void set_prepaid_topup_end_time(uint32_t ptet);

uint32_t get_prepaid_topup_end_volume(void);

void set_prepaid_topup_end_volume(uint32_t ptev);

uint32_t get_prepaid_topup_end_energy(void);

void set_prepaid_topup_end_energy(uint32_t ptee);

uint8_t get_prepaid_valve_control(void);

bool set_prepaid_valve_control(uint8_t pvc);


	

#endif /*PREPAID_H_*/

#ifndef IRDA_H_
#define IRDA_H_


#include "stdbool.h"

#include "extras.h"

#include "smart_gas.h"

void init_irda(void);

void check_irda(void);

void irda_send(uint8_t* data, uint8_t length);

void check_irda(void);

uint8_t get_irda_last_byte(void);

void irda_tick(void);

bool irda_is_switch(void);

bool irda_power_request(void);

bool is_irda_on(void);

void invoke_irda_receive(void);

void irda_send_smart_gas(enum smart_gas_object_ids obj_id, uint8_t* data, uint16_t length);
	

#endif /*IRDA_H_*/

#ifndef BELLOWS_H_
#define BELLOWS_H_


#include "stdbool.h"

#include "stdint.h"



void init_bellows(void);

int32_t get_bellows_ticks_counter(void);

void hallotron_tick(uint8_t* data);



#endif /*BELLOWS_H_*/

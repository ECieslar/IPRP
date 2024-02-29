#ifndef LOGS_H_
#define LOGS_H_


#include "stdbool.h"

#include "stdint.h"



enum log_types
{
	lt_reset				= 1,
	lt_irda_start		= 2,
	lt_irda_stop		= 3,
	lt_radio_start	= 4,
	lt_radio_stop		= 5,
	
};



void init_logs(void);

void add_log(enum log_types lt, uint8_t* tab);

int get_log(uint8_t* value, int idx);




#endif /*LOGS_H_*/

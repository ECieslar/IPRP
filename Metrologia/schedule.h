#ifndef SCHEDULE_H_
#define SCHEDULE_H_


#include "stdbool.h"

#include "extras.h"

void init_schedule(void);

void prepare_schedule(void);

void check_schedule(void);

bool add_schedule(uint8_t* tab, uint8_t length);

bool get_schedule(uint8_t* input, uint8_t input_length, uint8_t* output, int* output_length);

	

#endif /*SCHEDULE_H_*/

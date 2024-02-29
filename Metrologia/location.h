
#ifndef LOCATION_H_
#define LOCATION_H_

#include "stdbool.h"

#include "extras.h"


void init_location(void);

void prepare_location(void);

int32_t get_location_latitude(void);

int32_t get_location_longitude(void);

bool set_location_latitude(int32_t latitude);

bool set_location_longitude(int32_t longitude);
	

#endif /*LOCATION_H_*/

#ifndef CALIBRATION_H_
#define CALIBRATION_H_


#include "stdbool.h"

#include "stdint.h"



void init_calibration(void);

void prepare_calibration(void);

bool set_calibration(uint8_t* data, int length);

int get_calibration(uint8_t* data);


#endif /*CALIBRATION_H_*/

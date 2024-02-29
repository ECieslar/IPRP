#ifndef INFORMATION_H_
#define INFORMATION_H_


#include "stdbool.h"

#include "extras.h"



void init_information(void);

void prepare_information(void);

uint32_t get_serial_number(void);

void set_serial_number(uint32_t sn);

uint8_t get_client_id(uint8_t* ci);

bool set_client_id(uint8_t length, uint8_t* ci);

uint8_t get_gas_meter_serial_nbr(uint8_t* gms_nbr);

bool set_gas_meter_serial_nbr(uint8_t length, uint8_t* gms_nbr);

uint8_t get_device_type_name(uint8_t* dtn);

bool set_device_type_name(uint8_t length, uint8_t* dtn);


#endif /*INFORMATION_H_*/

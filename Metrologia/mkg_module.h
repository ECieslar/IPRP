
#ifndef MKG_MODULE_H_
#define MKG_MODULE_H_



#include "extras.h"


#include "stdbool.h"



void init_mkg_module(void);

void prepare_mkg_module(void);

uint32_t get_mkg_module_type(void);

void set_mkg_module_type(uint32_t mmt);

uint32_t get_mkg_hardware_version(void);

void set_mkg_hardware_version(uint32_t mhv);

uint32_t get_mkg_software_version(void);

void set_mkg_software_version(uint32_t msv);

uint32_t get_mkg_serial_number(void);

void set_mkg_serial_number(uint32_t mmt);

uint8_t get_mkg_gsm_clock(uint8_t* data);

bool set_mkg_gsm_clock(uint8_t length, uint8_t* data);



#endif /*MKG_MODULE_H_*/

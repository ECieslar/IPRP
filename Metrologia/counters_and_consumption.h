#ifndef COUNTERS_AND_CONSUMPTION_H_
#define COUNTERS_AND_CONSUMPTION_H_


#include "inttypes.h"

#include "stdbool.h"




void init_counters_and_consumption(void);

void prepare_counters_and_consumption(void);

uint8_t get_units(void);

bool set_units(uint8_t u);

uint32_t get_volume_units(void);

void set_volume_units(uint32_t volume);

void set_volume_offset_units(int32_t volume_offset);

uint32_t get_energy_units(void);

void set_energy_units(uint32_t energy);

void set_energy_offset_units(int32_t energy_offset);

uint8_t get_counters_registration_period(void);

bool set_counters_registration_period(uint8_t crp);

void update_archive(void);

uint32_t get_counters_archive_last_time(void);

uint32_t get_counters_archive_last_volume(void);

uint32_t get_counters_archive_last_energy(void);

uint16_t get_gas_meter_q_max(void);

int get_counters_archive_acc_time(uint32_t start_datetime, uint32_t end_datetime, uint8_t* output_data, uint16_t max_output_length);

uint32_t get_volume_max_value(void);

void set_volume_max_value(uint32_t volume);




#endif /*COUNTERS_AND_CONSUMPTION_H_*/

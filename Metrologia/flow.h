#ifndef FLOW_H_
#define FLOW_H_


#include "inttypes.h"

#include "stdbool.h"



void init_flow(void);

void prepare_flow(void);

uint16_t get_gas_meter_q_max_units(void);

bool set_gas_meter_q_max_units(uint16_t value);

void update_flow(int32_t flow);

uint16_t get_temporary_flow_units(void);

int64_t get_energy(void);

void set_energy(int64_t energy);

void set_energy_offset(int64_t energy_offset);

uint16_t get_calorific_value(void);

void set_calorific_value(uint16_t cv);

void set_volume_offset(int64_t volume_offset);

int32_t get_temporary_flow(void);

int64_t get_volume(void);

void set_volume(int64_t value);

void reset_volume(void);

uint16_t get_hourly_usage_current_units(void);

uint16_t get_temporary_flow_period_latched(void);

uint16_t get_temporary_flow_period_latched_time_compressed(void);

uint16_t get_hourly_flow_period_latched(void);

uint8_t get_hourly_flow_period_latched_time_compressed(void);

uint16_t get_temporary_flow_monthly_latched(void);

uint16_t get_temporary_flow_monthly_latched_time_compressed(void);

uint16_t get_hourly_flow_monthly_latched(void);

uint16_t get_hourly_flow_monthly_latched_time_compressed(void);

void update_volume(int32_t dV_mL);

bool is_over_volume(uint32_t volume);

bool is_over_energy(uint32_t volume);

void update_flow_time_compressed(void);

void set_volume_max_value_units(int64_t value);

void update_volume_energy(void);




#endif /*FLOW_H_*/

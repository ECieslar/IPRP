#ifndef STATUS_H_
#define STATUS_H_


#include "inttypes.h"

#include "stdbool.h"




enum status_flags
{
	flag_error_sim_card					= 0,
	flag_error_modem						= 1,
	flag_max_hour_flow					= 2,
	flag_max_temporal_flow			= 3,
	flag_tamper_magnetic				= 4,
	flag_tamper_remove					= 5,
	flag_tamper_open						= 6,
	flag_ir_flag								= 7,
	flag_battery_low						= 8,
	flag_low_voltage						= 9,
	flag_button									= 10,
	flag_valve_error						= 11,
	flag_valve_open_activation	= 12,
	flag_valve_leak_test_failed	= 13,
	flag_valve_close_failed			= 14,
	flag_topup_end							= 15,
	flag_large_clock_set				= 16,
	flag_volume_set							= 17,
	flag_energy_set							= 18,
	flag_installation						= 19,
	flag_discontinuity					= 27,
};


void init_status(void);

uint32_t get_status(void);

void set_status_flag(enum status_flags flag);

void clr_status_flag(enum status_flags flag);

uint32_t get_status_periodic_latched(void);

void set_status_periodic_latched(uint32_t spl);

void update_status_periodic_latched(void);
	
uint32_t get_status_monthly_latched(void);

void set_status_monthly_latched(uint32_t sml);

void update_status_monthly_latched(void);

uint32_t get_status_up_event_match(void);

void set_status_up_event_match(uint32_t suem);

uint32_t get_status_down_event_match(void);

void set_status_down_event_match(uint32_t sdem);





#endif /*STATUS_H_*/

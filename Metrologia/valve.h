#ifndef VALVE_H_
#define VALVE_H_


#include "stdbool.h"

#include "extras.h"

void init_valve(void);

void prepare_valve(void);

void valve_turn_left(void);

void valve_turn_right(void);

void valve_turn_off(void);

void valve_invoke_get_current(void);

void valve_current(uint8_t* data);

uint16_t valve_get_current(void);

bool is_valve_turn_left(void);

bool is_valve_turn_right(void);

void valve_off(void);

void valve_on(void);

bool set_valve_operation(uint8_t vo);

uint16_t get_valve_activation_timeout(void);

void set_valve_activation_timeout(uint16_t vat);

uint8_t get_valve_state(void);

uint16_t get_valve_leak_test_duration(void);

void set_valve_leak_test_duration(uint16_t vltd);

uint16_t get_valve_leak_test_volume(void);

void set_valve_leak_test_volume(uint16_t vltv);

uint8_t get_valve_error_code(void);

uint16_t get_valve_open_count(void);

void set_valve_open_count(uint16_t voc);

uint16_t get_valve_close_count(void);

void set_valve_close_count(uint16_t vcc);




	

#endif /*VALVE_H_*/

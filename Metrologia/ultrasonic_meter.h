
#ifndef ULTRASONIC_METER_H_
#define ULTRASONIC_METER_H_



#include "extras.h"


#include "stdbool.h"






void init_ultrasonic_meter(void);

int get_ultrasonic_meter_value(void);

uint32_t get_frame_error_counter(void);

uint32_t get_overrun_error_counter(void);

uint64_t get_bytes_counter(void);

void set_ultrasonic_meter_normal_mode(bool mode);

int16_t get_temperature_main(void);

uint32_t get_correct_frame_counter(void);

uint32_t get_unreceived_frames_counter(void);

void ultrasonic_meter_tick(void);

uint32_t get_bcc_incorrect_frames_counter(void);

uint32_t get_abnormal_measurement_counter(void);

bool ultrasonic_meter_transmitting(void);

uint16_t ultrasonic_gas_meter_q_max(void);

void ultrasonic_meter_value(uint8_t* data);

void ultrasonic_meter_temperature(uint8_t* data);

	


#endif /*ULTRASONIC_METER_H_*/

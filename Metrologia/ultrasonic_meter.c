

#include "ultrasonic_meter.h"

#include "menu.h"

#include "clock.h"

#include "timers.h"

#include "eeprom.h"

#include "flow.h"


#define UM_BUFFER_LENGTH	80

#define TEMPERATURE_FRAME_INTERVAL	30



//static uint8_t buffer[UM_BUFFER_LENGTH];

//static int start_buffer;

//static int stop_buffer;

//static int32_t ultrasonic_flow;

//static int64_t ultrasonic_value;

//static bool ultrasonic_meter_normal_mode;

static int16_t temperature;

static uint32_t correct_frames_counter;

static uint32_t unreceived_frames_counter;

static uint32_t bcc_incorrect_frames_counter;

static uint32_t unreceived_frames_ticks;

static uint32_t abnormal_measurement_counter;

//static uint16_t calorific_value;

//static int64_t ultrasonic_energy;









void init_ultrasonic_meter(void)
{
	//start_buffer = 0;
	
	//stop_buffer = 0;
	
	//ultrasonic_meter_normal_mode = true;
	
	correct_frames_counter = 0;
	
	unreceived_frames_counter = 0;
	
	unreceived_frames_ticks = 0;
	
	bcc_incorrect_frames_counter = 0;
	
	temperature = 2500;
	
	abnormal_measurement_counter = 0;
}






void ultrasonic_meter_value(uint8_t* data)
{
	correct_frames_counter++;
	
	unreceived_frames_ticks = 0;
	
	uint32_t value;
				
	value = data[0];
	value <<= 8;
	value |= data[1];
	value <<= 8;
	value |= data[2];
	
	if(value == 0xffffff)
	{
		abnormal_measurement_counter++;
	}
	else
	{
	
		if(value & 0x800000)
		{
			value &= ~0x800000;
			
			value = -value;
		}
		
		int32_t ultrasonic_flow = 10 * value; // [mL/h]
		
		update_flow(ultrasonic_flow);
	}
}



void ultrasonic_meter_temperature(uint8_t* data)
{
	temperature = data[0];
	temperature <<= 8;
	temperature |= data[1];
}



uint32_t get_frame_error_counter(void)
{
	//return driver_get_frame_error_counter();
	return 0;
}


uint32_t get_overrun_error_counter(void)
{
	//return driver_get_overrun_error_counter();
	return 0;
}


uint64_t get_bytes_counter(void)
{
	//return driver_get_bytes_counter();
	return 0;
}



void set_ultrasonic_meter_normal_mode(bool mode)
{
	//ultrasonic_meter_normal_mode = mode;
}



int16_t get_temperature_main(void)
{
	return temperature;
}


uint32_t get_correct_frame_counter(void)
{
	return correct_frames_counter;
}


uint32_t get_unreceived_frames_counter(void)
{
	return unreceived_frames_counter;
}


void ultrasonic_meter_tick(void)
{
	unreceived_frames_ticks++;
	
	if(unreceived_frames_ticks >= 3)
	{
		if(unreceived_frames_ticks & 1)
		{
			unreceived_frames_counter++;
		}
	}
}


uint32_t get_bcc_incorrect_frames_counter(void)
{
	return bcc_incorrect_frames_counter;
}


uint32_t get_abnormal_measurement_counter(void)
{
	return abnormal_measurement_counter;
}

bool ultrasonic_meter_transmitting(void)
{
	//return driver_ultrasonic_meter_transmitting();
	return false;
}





uint16_t ultrasonic_gas_meter_q_max(void)
{
	return 6000;
}




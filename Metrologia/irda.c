

#include "driver/driver_irda.h"

#include "timers.h"

#include "irda.h"

#include "menu.h"

#include "crc.h"

#include "date_time.h"

#include "extras.h"

#include "status.h"

#include <string.h>




#define IRDA_TRANSMISSION_BUFFER_LENGTH	512

static uint8_t transmission_buffer[IRDA_TRANSMISSION_BUFFER_LENGTH];

#define IRDA_RECEIVE_BUFFER_LENGTH	512

static uint8_t receive_buffer[IRDA_RECEIVE_BUFFER_LENGTH];

//static uint8_t receive_bytes_count;

static uint8_t receive_idx;

static int irda_count_tick;

static bool irda_on;

static int irda_receive_timeout_idx;

static uint64_t received_bytes;

static uint32_t received_frames;

static uint32_t received_frames_error;

static uint32_t received_frames_bad_crc;

static uint64_t sent_bytes;

static uint32_t sent_frames;



const uint8_t start_byte = 0x55;

const uint8_t stop_byte = 0xff;







static uint16_t finish_frame(uint8_t protocol, uint16_t length)
{
	transmission_buffer[0] = start_byte;
	
	transmission_buffer[1] = protocol;
	
	transmission_buffer[2] = length >> 8;
	
	transmission_buffer[3] = length & 0xff;
	
	uint32_t crc32 = get_crc32(transmission_buffer + 1, length + 3);
	
	transmission_buffer[length + 4] = (crc32 >> 24) & 0xff;
	
	transmission_buffer[length + 5] = (crc32 >> 16) & 0xff;
	
	transmission_buffer[length + 6] = (crc32 >> 8) & 0xff;
	
	transmission_buffer[length + 7] = crc32 & 0xff;
	
	transmission_buffer[length + 8] = stop_byte;
	
	return length + 9;
}






/*
static uint16_t empty_response(void)
{
	return finish_frame(irda_frame_empty, 0);
}



static uint16_t get_ultrasonic_meter_data_response(void)
{
	int idx = 4;
	
	idx += memcpy_int32_t_bigendian(transmission_buffer + idx, get_ultrasonic_flow());
	
	idx += memcpy_int64_t_bigendian(transmission_buffer + idx, get_ultrasonic_value());
	
	idx += memcpy_int16_t_bigendian(transmission_buffer + idx, get_temperature_main());	
	
	return finish_frame(irda_frame_get_ultrasonic_meter_data, idx - 4);
}





static uint16_t get_ultrasonic_meter_transmission_statistics_response(void)
{
	int idx = 4;
		
	idx += memcpy_uint64_t_bigendian(transmission_buffer + idx, get_bytes_counter());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_frame_error_counter());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_overrun_error_counter());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_correct_frame_counter());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_bcc_incorrect_frames_counter());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_unreceived_frames_counter());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_abnormal_measurement_counter());
	
	return finish_frame(irda_frame_get_ultrasonic_meter_transmission_statistics, idx - 4);
}



static uint16_t get_pressure_sensor_data_response(void)
{
	int idx = 4;
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_pressure());
	
	idx += memcpy_int16_t_bigendian(transmission_buffer + idx, get_temperature_additional());
	
	return finish_frame(irda_frame_get_pressure_sensor_data, idx - 4);
}




static uint16_t get_battery_voltage_response(void)
{
	int idx = 4;
	
	idx += memcpy_uint16_t_bigendian(transmission_buffer + idx, get_input_voltage());
	
	return finish_frame(irda_frame_get_battery_voltage, idx - 4);
}




static uint16_t open_valve_response(void)
{
	valve_invoke_left();
	
	return finish_frame(irda_frame_open_valve, 0);
}



static uint16_t close_valve_response(void)
{
	valve_invoke_right();
	
	return finish_frame(irda_frame_close_valve, 0);
}




static uint16_t get_battery_capacity_response(void)
{
	int idx = 4;
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, get_battery_capacity());
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, MAX_CAPACITY_UAH);
	
	return finish_frame(irda_frame_get_battery_capacity, idx - 4);
}



static uint16_t get_irda_transmission_statistics_response(void)
{
	int idx = 4;
		
	idx += memcpy_uint64_t_bigendian(transmission_buffer + idx, received_bytes);
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, received_frames);
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, received_frames_error);
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, received_frames_bad_crc);
	
	idx += memcpy_uint64_t_bigendian(transmission_buffer + idx, sent_bytes);
	
	idx += memcpy_uint32_t_bigendian(transmission_buffer + idx, sent_frames);
	
	return finish_frame(irda_frame_get_irda_transmission_statistics, idx - 4);
}




static uint16_t clear_meter_response(void)
{
	reset_ultrasonic_value();
	
	return finish_frame(irda_frame_clear_meter, 0);
}




static uint16_t ultrasonic_meter_mode_response(uint8_t* buffer)
{
	if(buffer[0] == 0)
	{
		set_ultrasonic_meter_normal_mode(true);
	}
	
	if(buffer[0] == 1)
	{
		set_ultrasonic_meter_normal_mode(false);
	}
	
	return finish_frame(irda_frame_ultrasonic_meter_mode, 0);
}



static uint16_t get_archive_period_response(void)
{
	int idx = 4;
	
	idx += memcpy_uint16_t_bigendian(transmission_buffer + idx, get_archive_period());
	
	return finish_frame(irda_frame_get_archive_period, idx - 4);
}




static uint16_t set_archive_period_response(uint8_t* buffer)
{
	uint32_t archive_period = buffer[0];
	archive_period <<= 8;
	
	archive_period |= buffer[1];
	
	set_archive_period(archive_period);
	
	return finish_frame(irda_frame_set_archive_period, 0);
}
*/






bool call_back(uint8_t* buffer, uint16_t length)
{
	start_time_event(irda_receive_timeout_idx);
		
	if(buffer[0] != start_byte)
	{
		received_bytes += length;
		
		received_frames_error++;
		
		return false;
	}
	
	//enum irda_functions function = (enum irda_functions)buffer[1];
	
	uint16_t data_length = buffer[2];
	
	data_length <<= 8;
	
	data_length |= buffer[3];
	
	if((data_length != 0) && (length == 9))
	{
		if(data_length > IRDA_MAX_RECEIVE_BUFFER - 9)
		{
			received_bytes += length;
			
			received_frames_error++;
			
			return false;
		}
		
		driver_irda_continue_reception(data_length);
		
		return true;
	}
	
	
	if(data_length + 9 != length)
	{
		received_bytes += length;
		
		received_frames_error++;
		
		return false;
	}
	
	
	
	if(buffer[8 + data_length] != stop_byte)
	{
		received_bytes += length;
		
		received_frames_error++;
		
		return false;
	}
	
	
	
	uint32_t crc32 = buffer[4 + data_length];
	crc32 <<= 8;
	
	crc32 |= buffer[5 + data_length];
	crc32 <<= 8;
	
	crc32 |= buffer[6 + data_length];
	crc32 <<= 8;
	
	crc32 |= buffer[7 + data_length];
	
	
	//extras_pin2_on();
	static uint32_t crc32_count = 0;
	crc32_count = get_crc32(buffer + 1, data_length + 3);
	//extras_pin2_off();
	
	if(crc32 != crc32_count)
	{
		received_bytes += length;
		
		received_frames_bad_crc++;
		
		return false;
	}
	
	received_bytes += length;
	
	received_frames++;
	
	int transmission_length = 0;
	
	if(buffer[1] == 0)
	{
		transmission_length = smart_gas_object(buffer + 4, data_length, transmission_buffer + 4, IRDA_TRANSMISSION_BUFFER_LENGTH - 4, sgos_irda);
	}
	
	if(transmission_length != -1)
	{
		if(transmission_length == 0)
		{
			driver_irda_receive();
		}
		else
		{
			sent_bytes += transmission_length;

			sent_frames++;
			
			driver_irda_send(transmission_buffer, finish_frame(0, transmission_length));
		}
	}
	
	return true;
}



void invoke_irda_receive(void)
{
	driver_irda_receive();
}


void irda_send_smart_gas(enum smart_gas_object_ids obj_id, uint8_t* data, uint16_t length)
{
	sent_bytes += length;

	sent_frames++;
	
	transmission_buffer[4] = obj_id >> 8;
				
	transmission_buffer[5] = obj_id & 0xff;
	
	memcpy(transmission_buffer + 6, data, length);
	
	driver_irda_send(transmission_buffer, finish_frame(0, length + 2));
}



static void irda_receive_timeout(void)
{
	if(!(driver_irda_is_transmitting() || driver_irda_is_receive_initialized()))
	{
		driver_irda_receive();
	}
}


void init_irda()
{
	received_bytes = 0;

	received_frames = 0;
	
	received_frames_error = 0;

	received_frames_bad_crc = 0;

	sent_bytes = 0;

	sent_frames = 0;
	
	receive_idx = 0;
	
	init_driver_irda(call_back);
	
	irda_receive_timeout_idx = register_time_event(irda_receive_timeout, 200, false);
	
	irda_on = true;
}



void irda_send(uint8_t* data, uint8_t length)
{
	memcpy(transmission_buffer, data, length);
	
	driver_irda_send(transmission_buffer, length);
}




void check_irda(void)
{
	if(irda_power_request() && !irda_on)
	{
		driver_irda_vcc_on();

		irda_on = true;
		
		set_status_flag(flag_ir_flag);
		
		start_time_event(irda_receive_timeout_idx);
	}
	
	
	/*
	int byte = get_driver_irda_byte();
	
	if(byte != -1)
	{
		receive_buffer[receive_idx++] = byte;
		
		if(receive_idx >= IRDA_RECEIVE_BUFFER_LENGTH)
		{
			receive_idx = 0;
			
			receive_bytes_count++;
		}
	}
	*/
}



uint8_t get_irda_last_byte(void)
{
	if(receive_idx == 0)
	{
		return receive_buffer[IRDA_RECEIVE_BUFFER_LENGTH - 1];
	}
	
	return receive_buffer[receive_idx - 1];
}



void irda_tick(void)
{
	if(driver_irda_is_switch())
	{
		irda_count_tick++;
	}
	else
	{
		irda_count_tick = 0;
		
		if(irda_on)
		{
			driver_irda_vcc_off();

			irda_on = false;
			
			clr_status_flag(flag_ir_flag);
			
			stop_time_event(irda_receive_timeout_idx);
		}
	}
}




bool irda_power_request(void)
{
	if(irda_count_tick >= 2)
	{
		return true;
	}
	
	return false;
}



bool is_irda_on(void)
{
	return irda_on;
}




#include "smart_gas.h"


#include "battery.h"

#include "information.h"

#include "location.h"

#include "date_time.h"

#include "ultrasonic_meter.h"

#include "version.h"

#include "counters_and_consumption.h"

#include "events_configuration.h"

#include "modem.h"

#include "flow.h"

#include "latches.h"

#include "calibration.h"

#include "mkg_module.h"

#include "prepaid.h"

#include "protocol_settings.h"

#include "initialisation.h"

#include "timers.h"

#include "irda.h"

#include "radio_communication.h"

#include "preparation.h"

#include "valve.h"

#include "status.h"

#include "schedule.h"

#include <string.h>








enum smart_gas_errors
{
	sg_objd_err_not_found					=	0x8001,
	sg_objd_err_read_denied				=	0x8002,
	sg_objd_err_write_denied			=	0x8003,
	sg_objd_err_wrong_size				=	0x8004,
	sg_objd_err_wrong_value				=	0x8005,
	sg_objd_err_unexpected_end		=	0x8006,
	sg_objd_err_too_large					=	0x8007,
	sg_objd_err_tbl_out_of_range	=	0x8008,
	sg_objd_err_param_unsupported	=	0x8009,
	sg_objd_err_param_error				=	0x800A,
	sg_objd_err_tbl_col_error			=	0x800B,
	sg_objd_err_acclvl_error			=	0x800C,
	sg_objd_err_accpass_error			=	0x800D,
	sg_objd_err_not_initialised		=	0x800E,
	sg_objd_err_hardware					=	0x8100,
	sg_objd_err_firmware					=	0x8200,
	sg_objd_err_unknown						=	0x8FFF,
};



enum smart_gas_tbl_acc
{
	sg_tbl_acc_idx						= 0x00,
	sg_tbl_acc_time						= 0x01,
	sg_tbl_acc_time_back			= 0x02,
	sg_tbl_acc_cols						= 0x03,
	sg_tbl_acc_total_idx			= 0x04,
	sg_tbl_acc_cols_info			= 0x05,
	sg_tbl_acc_time_back_rel	= 0x06,
};



static int max_output_length;

static int smart_gas_timeout_idx;

static enum smart_gas_object_sources smart_gas_object_source;

enum smart_gas_object_ids delayed_response_obj_id;

static uint16_t (*delayed_response_smart_gas_response)(uint8_t* input_data, uint16_t length, uint8_t* response_buffer);




int get_smart_gas_max_output_length(void)
{
	return max_output_length;
}


static uint16_t finish_frame_smart_gas_error(uint16_t error, uint8_t* output_data)
{	
	output_data[-2] |= 0x80;
	
	output_data[0] = error & 0xff;
	
	output_data[1] = error >> 8;
	
	return 2;
}



static bool smart_gas_wrong_size(enum smart_gas_object_ids obj_id, uint8_t* input_data, uint16_t length)
{
	if((obj_id & sg_put) == sg_put)
	{
		switch(obj_id & 0xf00)
		{
			case 0x100:
			{
				if(length == 1)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			
			case 0x200:
			{
				if(length == 2)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			
			case 0x400:
			{
				if(length == 4)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			
			case 0x500:
			{
				if((input_data[0] + 1) == length)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			
			
			
			default:
			{
				return true;
			}
		}
	}
	else
	{
		switch(obj_id & 0xf00)
		{
			case 0x100:
			case 0x200:
			case 0x400:
			case 0x500:
			{
				if(length == 0)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			
			case 0x600:
			{
				if(length < 6)
				{
					return true;
				}
				else
				{
					if((input_data[0] + 1) == length)
					{
						uint8_t tbl_acc_length;
						
						switch(input_data[1])
						{
							case sg_tbl_acc_idx:tbl_acc_length = 6;break;
							case sg_tbl_acc_time:tbl_acc_length = 10;break;
							case sg_tbl_acc_time_back:tbl_acc_length = 8;break;
							case sg_tbl_acc_total_idx:tbl_acc_length = 8;break;
							case sg_tbl_acc_time_back_rel:tbl_acc_length = 10;break;
							
							default:
							{
								return true;
							}
						}
						
						if(length == tbl_acc_length)
						{
							return false;
						}
						else
						{
							if(length > tbl_acc_length)
							{
								switch(input_data[tbl_acc_length])
								{
									case sg_tbl_acc_cols:
									{
										if(length == (tbl_acc_length + 1))
										{
											return false;
										}
									}
									
									case sg_tbl_acc_cols_info:
									{
										if(length >= (tbl_acc_length + 2))
										{
											if(length == (tbl_acc_length + 2 + 2 * input_data[tbl_acc_length + 1]))
											{
												return true;
											}
											else
											{
												return false;
											}
										}
										else
										{
											return true;
										}
									}
									
									default:
									{
										return true;
									}
								}
							}
							
							return true;
						}
					}
					else
					{
						return true;
					}
				}
			}
			
			default:
			{
				return true;
			}
		}
	}
}




static uint8_t get_tbl_acc(uint8_t* input_data, uint32_t* p1, uint32_t* p2)
{
	switch(input_data[1])
	{
		case sg_tbl_acc_idx:
		{
			*p1 = get_from_buffer_uint16_t_littleendian(input_data + 2);
			*p2 = get_from_buffer_uint16_t_littleendian(input_data + 4);
			break;
		}
		
		case sg_tbl_acc_time:
		case sg_tbl_acc_time_back_rel:
		{
			*p1 = get_from_buffer_uint32_t_littleendian(input_data + 2);
			*p2 = get_from_buffer_uint32_t_littleendian(input_data + 6);
			break;
		}
				
		case sg_tbl_acc_time_back:
		case sg_tbl_acc_total_idx:
		{
			*p1 = get_from_buffer_uint32_t_littleendian(input_data + 2);
			*p2 = get_from_buffer_uint16_t_littleendian(input_data + 6);
			break;
		}
	}
	
	return input_data[1];
}



static uint16_t set_tbl_acc(uint8_t* input_data, uint8_t* output_data, int input_length, int output_length)
{
	memcpy(output_data, input_data, input_length);
	
	output_data[input_length] = output_length & 0xff;
	
	output_data[input_length + 1] = (output_length >> 8) & 0xff;
	
	return input_length + 2;
}




static uint16_t sg_get_serial_nbr_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_serial_number());
}



static uint16_t sg_put_serial_nbr_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_serial_number(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_latitude_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_int32_t_littleendian(output_data, get_location_latitude());
}



static uint16_t sg_put_latitude_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_location_latitude(get_from_buffer_int32_t_littleendian(input_data)))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}


static uint16_t sg_get_longitude_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_int32_t_littleendian(output_data, get_location_longitude());
}


static uint16_t sg_put_longitude_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_location_longitude(get_from_buffer_int32_t_littleendian(input_data)))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_hardware_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_int32_t_littleendian(output_data, HARDWARE_VERSION);
}



static uint16_t sg_get_software_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[3] = 0;
	output_data[2] = FIRMWARE_VERSION_MAJOR;
	output_data[1] = FIRMWARE_VERSION_MINOR;
	output_data[0] = FIRMWARE_VERSION_EXTRA;
	
	return 4;
}



static uint16_t sg_get_standard_parameter_list_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = 0;
	output_data[1] = 0;
	
	return 2;
}



static uint16_t sg_get_client_id_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_client_id(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_client_id_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_client_id(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_gas_meter_serial_nbr_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_gas_meter_serial_nbr(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_gas_meter_serial_nbr_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gas_meter_serial_nbr(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_device_type_name_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_device_type_name(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_device_type_name_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_device_type_name(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_clock_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_unix_time_utc());
}



static uint16_t sg_put_clock_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_unix_time_utc(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_clock_synchronisation_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_int16_t_littleendian(output_data, get_clock_synchronisation_offset());
}


static uint16_t sg_put_clock_synchronisation_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_clock_synchronisation_offset(get_from_buffer_int16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_time_zone_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_time_zone_offset();
	
	return 1;
}


static uint16_t sg_put_time_zone_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_time_zone_offset(input_data[0]);
	
	return 0;
}




static uint16_t sg_get_time_zone_offset_winter_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_time_zone_offset_winter();
	
	return 1;
}


static uint16_t sg_put_time_zone_offset_winter_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_time_zone_offset_winter(input_data[0]);
	
	return 0;
}



static uint16_t sg_get_time_zone_offset_summer_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_time_zone_offset_summer();
	
	return 1;
}


static uint16_t sg_put_time_zone_offset_summer_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_time_zone_offset_summer(input_data[0]);
	
	return 0;
}




static uint16_t sg_get_clock_synchronisation_from_gsm_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_clock_synchronisation_from_gsm();
	
	return 1;
}


static uint16_t sg_put_clock_synchronisation_from_gsm_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_clock_synchronisation_from_gsm(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_battery_level_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_battery_level();
	
	return 1;
}


static uint16_t sg_put_battery_level_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_batter_level(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_temperature_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_temperature_main() / 100;
	
	return 1;
}



static uint16_t sg_get_units_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_units();
	
	return 1;
}


static uint16_t sg_put_units_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_units(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}


static uint16_t sg_get_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_volume_units());
}




static uint16_t sg_put_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_volume_units(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_put_command_volume_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_volume_offset_units(get_from_buffer_int32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_calorific_value_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_calorific_value());
}


static uint16_t sg_put_calorific_value_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_calorific_value(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_energy_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_energy_units());
}




static uint16_t sg_put_energy_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_energy_units(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_put_command_energy_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_energy_offset_units(get_from_buffer_int32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_counters_registration_period_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_counters_registration_period();
	
	return 1;
}


static uint16_t sg_put_counters_registration_period_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_counters_registration_period(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_counters_archive_last_time_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_counters_archive_last_time());
}



static uint16_t sg_get_counters_archive_last_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_counters_archive_last_volume());
}



static uint16_t sg_get_counters_archive_last_energy_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_counters_archive_last_energy());
}




static uint16_t sg_get_gas_meter_q_max_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_gas_meter_q_max_units());
}




static uint16_t sg_put_gas_meter_q_max_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gas_meter_q_max_units(get_from_buffer_uint16_t_littleendian(input_data)))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_temporary_flow_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_temporary_flow_units());
}


static uint16_t sg_get_hourly_usage_current_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_hourly_usage_current_units());
}



static uint16_t sg_get_monthly_data_latch_day_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_monthly_data_latch_day();
	
	return 1;
}


static uint16_t sg_put_monthly_data_latch_day_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_monthly_data_latch_day(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_monthly_and_daily_data_latch_hour_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_monthly_and_daily_data_latch_hour();
	
	return 1;
}


static uint16_t sg_put_monthly_and_daily_data_latch_hour_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_monthly_and_daily_data_latch_hour(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_monthly_and_daily_data_latch_local_time_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_monthly_and_daily_data_latch_local_time();
	
	return 1;
}


static uint16_t sg_put_monthly_and_daily_data_latch_local_time_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_monthly_and_daily_data_latch_local_time(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_last_daily_latch_time_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_last_daily_latch_time());
}




static uint16_t sg_get_last_daily_latch_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_last_daily_latch_volume());
}



static uint16_t sg_get_last_daily_latch_energy_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_last_daily_latch_energy());
}




static uint16_t sg_get_calibration_parameters_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_calibration(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_calibration_parameters_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_calibration(input_data + 1, input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}


static uint16_t sg_get_event_link_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_event_link();
	
	return 1;
}


static uint16_t sg_put_event_link_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_event_link(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_event_udp_port_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_event_udp_port());
}


static uint16_t sg_put_event_udp_port_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_event_udp_port(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_event_tcp_port_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_event_tcp_port());
}


static uint16_t sg_put_event_tcp_port_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_event_tcp_port(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_event_phone_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_event_phone(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_event_phone_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_event_phone(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_size, output_data);
	}
}



static uint16_t sg_get_event_ip_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_event_ip());
}


static uint16_t sg_put_event_ip_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_event_ip(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_sim_pin_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_sim_pin(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_sim_pin_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_sim_pin(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_sim_iccid_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_sim_iccid(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_sim_iccid_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_sim_iccid(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_sim_phone_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_sim_phone(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_sim_phone_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_sim_phone(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_sim_ip_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_sim_ip());
}



static uint16_t sg_put_sim_ip_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_sim_ip(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_sim_smsc_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_sim_smsc(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_sim_smsc_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_sim_smsc(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_gprs_apn_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_gprs_apn(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_gprs_apn_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gprs_apn(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_sim_sms_validity_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_sim_sms_validity();
	
	return 1;
}


static uint16_t sg_put_sim_sms_validity_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_sim_sms_validity(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}


static uint16_t sg_get_gprs_user_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_gprs_user(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_gprs_user_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gprs_user(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_gprs_password_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_gprs_password(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_gprs_password_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gprs_password(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_gsm_quality_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_gsm_quality();
	
	return 1;
}


static uint16_t sg_put_gsm_quality_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gsm_quality(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_gsm_network_code_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_gsm_network_code(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_gsm_network_code_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_gsm_network_code(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_gsm_bts_lac_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_gsm_bts_lac());
}



static uint16_t sg_put_gsm_bts_lac_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_gsm_bts_lac(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_gsm_bts_cell_id_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_gsm_bts_cell_id());
}



static uint16_t sg_put_gsm_bts_cell_id_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_gsm_bts_cell_id(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_modem_state_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_modem_state();
	
	return 1;
}


static uint16_t sg_put_modem_state_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_modem_state(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_modem_sms_received_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_modem_sms_received());
}


static uint16_t sg_put_modem_sms_received_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modem_sms_received(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_modem_sms_sent_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_modem_sms_sent());
}


static uint16_t sg_put_modem_sms_sent_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modem_sms_sent(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_modem_gprs_packets_received_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_modem_gprs_packets_received());
}


static uint16_t sg_put_modem_gprs_packets_received_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modem_gprs_packets_received(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_modem_gprs_packets_sent_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_modem_gprs_packets_sent());
}


static uint16_t sg_put_modem_gprs_packets_sent_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modem_gprs_packets_sent(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_modem_wakeup_count_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_modem_wakeup_count());
}


static uint16_t sg_put_modem_wakeup_count_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modem_wakeup_count(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}


static uint16_t sg_get_modem_wakeup_duration_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_modem_wakeup_duration());
}



static uint16_t sg_put_modem_wakeup_duration_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modem_wakeup_duration(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_mkg_module_type_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_mkg_module_type());
}


static uint16_t sg_put_mkg_module_type_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_mkg_module_type(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_mkg_hardware_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_mkg_hardware_version());
}


static uint16_t sg_put_mkg_hardware_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_mkg_hardware_version(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_mkg_software_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_mkg_software_version());
}


static uint16_t sg_put_mkg_software_version_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_mkg_software_version(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_mkg_serial_number_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_mkg_serial_number());
}


static uint16_t sg_put_mkg_serial_number_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_mkg_serial_number(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_temporary_flow_period_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_temporary_flow_period_latched());
}



static uint16_t sg_get_temporary_flow_period_latched_time_compressed_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_temporary_flow_period_latched_time_compressed());
}




static uint16_t sg_get_hourly_flow_period_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_hourly_flow_period_latched());
}




static uint16_t sg_get_hourly_flow_period_latched_time_compressed_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_hourly_flow_period_latched_time_compressed();
	
	return 1;
}




static uint16_t sg_get_temporary_flow_monthly_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_temporary_flow_monthly_latched());
}




static uint16_t sg_get_temporary_flow_monthly_latched_time_compressed_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_temporary_flow_monthly_latched_time_compressed());
}



static uint16_t sg_get_hourly_flow_monthly_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_hourly_flow_monthly_latched());
}




static uint16_t sg_get_hourly_flow_monthly_latched_time_compressed_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_hourly_flow_monthly_latched_time_compressed());
}



static uint16_t sg_get_prepaid_topup_id_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_prepaid_topup_id();
	
	return 1;
}


static uint16_t sg_put_prepaid_topup_id_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_prepaid_topup_id(input_data[0]);
	
	return 0;
}



static uint16_t sg_get_prepaid_topup_end_time_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_prepaid_topup_end_time());
}


static uint16_t sg_put_prepaid_topup_end_time_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_prepaid_topup_end_time(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_prepaid_topup_end_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_prepaid_topup_end_volume());
}


static uint16_t sg_put_prepaid_topup_end_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_prepaid_topup_end_volume(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}


static uint16_t sg_get_prepaid_topup_end_energy_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_prepaid_topup_end_energy());
}


static uint16_t sg_put_prepaid_topup_end_energy_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_prepaid_topup_end_energy(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_prepaid_valve_control_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_prepaid_valve_control();
	
	return 1;
}


static uint16_t sg_put_prepaid_valve_control_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_prepaid_valve_control(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_wdp_arp_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wdp_arp();
	
	return 1;
}


static uint16_t sg_put_wdp_arp_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_wdp_arp(input_data[0]);
	
	return 0;
}



static uint16_t sg_get_wdp_at_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wdp_at();
	
	return 1;
}


static uint16_t sg_put_wdp_at_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_wdp_at(input_data[0]);
	
	return 0;
}



static uint16_t sg_get_wdp_mr_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wdp_mr();
	
	return 1;
}


static uint16_t sg_put_wdp_mr_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_wdp_mr(input_data[0]);
	
	return 0;
}



static uint16_t sg_get_wpp_event_send_sn_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wpp_event_send_sn();
	
	return 1;
}


static uint16_t sg_put_wpp_event_send_sn_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_wpp_event_send_sn(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_wpp_iv_in_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_wpp_iv_in());
}


static uint16_t sg_put_wpp_iv_in_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_wpp_iv_in(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_wpp_key_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wpp_key(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_wpp_key_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_wpp_key(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}





static uint16_t sg_get_counters_archive_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	uint32_t p1, p2;
	
	switch(get_tbl_acc(input_data, &p1, &p2))
	{
		case sg_tbl_acc_time:
		{
			int output_length = get_counters_archive_acc_time(p1, p2, output_data + length + 2, max_output_length);
			
			if(output_length == -1)
			{
				return finish_frame_smart_gas_error(sg_objd_err_param_unsupported, output_data);
			}
			else
			{
				return set_tbl_acc(input_data, output_data, length, output_length) + output_length;
			}
		}
		
		case sg_tbl_acc_time_back:
		{
			break;
		}
			
		case sg_tbl_acc_idx:
		case sg_tbl_acc_total_idx:
		case sg_tbl_acc_time_back_rel:
		{
			return finish_frame_smart_gas_error(sg_objd_err_param_unsupported, output_data);
		}
		
		default:
		{
			return finish_frame_smart_gas_error(sg_objd_err_param_error, output_data);
		}
	}
	
	return 0;
}



static uint16_t sg_put_prepare_parameters_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(prepare_parameters(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}





static uint16_t sg_get_volume_max_value_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_volume_max_value());
}


static uint16_t sg_put_volume_max_value_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_volume_max_value(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_mkg_gsm_clock_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_mkg_gsm_clock(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_mkg_gsm_clock_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_mkg_gsm_clock(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_time_zone_auto_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_time_zone_auto_offset();
	
	return 1;
}


static uint16_t sg_put_time_zone_auto_offset_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_time_zone_auto_offset(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_cal_modbus_start_addr_len_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_modbus_start_address_length());
}



static uint16_t sg_put_cal_modbus_start_addr_len_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_modbus_start_address_length(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_cal_modbus_holding_registers_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	get_cal_modbus_holding_registers(output_data + 1);
	
	return 0;
}




static uint16_t sg_put_cal_modbus_holding_registers_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_cal_modbus_holding_registers(input_data[0], input_data + 1);
	
	return 0;
}



static uint16_t sg_get_cal_modbus_input_registers_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	get_cal_modbus_input_registers(output_data + 1);
	
	return 0;
}



static uint16_t sg_get_wpp_cryptography_obligatory_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wpp_cryptography_obligatory();
	
	return 1;
}


static uint16_t sg_put_wpp_cryptography_obligatory_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_wpp_cryptography_obligatory(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_wpp_access_control_obligatory_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_wpp_access_control_obligatory();
	
	return 1;
}


static uint16_t sg_put_wpp_access_control_obligatory_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_wpp_access_control_obligatory(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}




static uint16_t sg_get_sms_phone_numbers_restriction_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_sms_phone_numbers_restriction();
	
	return 1;
}


static uint16_t sg_put_sms_phone_numbers_restriction_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_sms_phone_numbers_restriction(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_permitted_phone_number_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_permitted_phone_number(output_data + 1);
	
	return output_data[0] + 1;
}




static uint16_t sg_put_permitted_phone_number_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_permitted_phone_number(input_data[0], input_data + 1))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_size, output_data);
	}
}


static uint16_t sg_put_valve_operation_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(set_valve_operation(input_data[0]))
	{
		return 0;
	}
	else
	{
		return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
	}
}



static uint16_t sg_get_valve_activation_timeout_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_valve_activation_timeout());
}



static uint16_t sg_put_valve_activation_timeout_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_valve_activation_timeout(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_valve_state_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_valve_state();
	
	return 1;
}



static uint16_t sg_get_valve_leak_test_duration_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_valve_leak_test_duration());
}



static uint16_t sg_put_valve_leak_test_duration_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_valve_leak_test_duration(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_valve_leak_test_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_valve_leak_test_volume());
}



static uint16_t sg_put_valve_leak_test_volume_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_valve_leak_test_volume(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_valve_error_code_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	output_data[0] = get_valve_error_code();
	
	return 1;
}



static uint16_t sg_get_valve_open_count_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_valve_open_count());
}



static uint16_t sg_put_valve_open_count_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_valve_open_count(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_valve_close_count_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint16_t_littleendian(output_data, get_valve_close_count());
}



static uint16_t sg_put_valve_close_count_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_valve_close_count(get_from_buffer_uint16_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_status_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_status());
}


static uint16_t sg_get_status_periodic_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_status_periodic_latched());
}


static uint16_t sg_put_status_periodic_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_status_periodic_latched(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_status_monthly_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_status_monthly_latched());
}


static uint16_t sg_put_status_monthly_latched_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_status_monthly_latched(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_status_up_event_match_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_status_up_event_match());
}


static uint16_t sg_put_status_up_event_match_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_status_up_event_match(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}



static uint16_t sg_get_status_down_event_match_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	return memcpy_to_buffer_uint32_t_littleendian(output_data, get_status_down_event_match());
}


static uint16_t sg_put_status_down_event_match_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	set_status_down_event_match(get_from_buffer_uint32_t_littleendian(input_data));
	
	return 0;
}




static uint16_t sg_get_schedule_table_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	int output_length;
	
	if(get_schedule(input_data, length, output_data, &output_length))
	{
		return output_length;
	}
	
	return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
}




static uint16_t sg_put_schedule_table_response(uint8_t* input_data, uint16_t length, uint8_t* output_data)
{
	if(add_schedule(input_data, length))
	{
		return 0;
	}
	
	return finish_frame_smart_gas_error(sg_objd_err_wrong_value, output_data);
}




int smart_gas_object(uint8_t* input_data, uint16_t length, uint8_t* output_data, uint16_t max_output_data_length, enum smart_gas_object_sources source)
{
	if(length < 2)
	{
		return 0;
	}
	
	enum smart_gas_object_ids obj_id = (enum smart_gas_object_ids)input_data[0];
	
	obj_id <<= 8;
	
	obj_id |= input_data[1];
	
	max_output_length = max_output_data_length - 2;
	
	if((obj_id & 0xf00) == 0x600)
	{
		max_output_length -= length;
	}
	
	int timeout = 0;
	
	uint16_t (*smart_gas_response)(uint8_t* input_data, uint16_t length, uint8_t* response_buffer) = NULL;
	
	switch(obj_id)
	{
		case sg_get_standard_parameter_list_version:smart_gas_response = sg_get_standard_parameter_list_version_response;break;
		case sg_get_serial_nbr:smart_gas_response = sg_get_serial_nbr_response;break;
		case sg_put_serial_nbr:smart_gas_response = sg_put_serial_nbr_response;break;
		case sg_get_latitude:smart_gas_response = sg_get_latitude_response;break;
		case sg_put_latitude:smart_gas_response = sg_put_latitude_response;break;
		case sg_get_longitude:smart_gas_response = sg_get_longitude_response;break;
		case sg_put_longitude:smart_gas_response = sg_put_longitude_response;break;
		case sg_get_hardware_version:smart_gas_response = sg_get_hardware_version_response;break;
		case sg_get_software_version:smart_gas_response = sg_get_software_version_response;break;
		case sg_get_client_id:smart_gas_response = sg_get_client_id_response;break;
		case sg_put_client_id:smart_gas_response = sg_put_client_id_response;break;
		case sg_get_gas_meter_serial_nbr:smart_gas_response = sg_get_gas_meter_serial_nbr_response;break;
		case sg_put_gas_meter_serial_nbr:smart_gas_response = sg_put_gas_meter_serial_nbr_response;break;
		case sg_get_device_type_name:smart_gas_response = sg_get_device_type_name_response;break;
		case sg_put_device_type_name:smart_gas_response = sg_put_device_type_name_response;break;
		case sg_get_clock:smart_gas_response = sg_get_clock_response;break;
		case sg_put_clock:smart_gas_response = sg_put_clock_response;break;
		case sg_get_clock_synchronisation_offset:smart_gas_response = sg_get_clock_synchronisation_offset_response;break;
		case sg_put_clock_synchronisation_offset:smart_gas_response = sg_put_clock_synchronisation_offset_response;break;
		case sg_get_time_zone_offset:smart_gas_response = sg_get_time_zone_offset_response;break;
		case sg_put_time_zone_offset:smart_gas_response = sg_put_time_zone_offset_response;break;
		case sg_get_time_zone_offset_winter:smart_gas_response = sg_get_time_zone_offset_winter_response;break;
		case sg_put_time_zone_offset_winter:smart_gas_response = sg_put_time_zone_offset_winter_response;break;
		case sg_get_time_zone_offset_summer:smart_gas_response = sg_get_time_zone_offset_summer_response;break;
		case sg_put_time_zone_offset_summer:smart_gas_response = sg_put_time_zone_offset_summer_response;break;
		case sg_get_clock_synchronisation_from_gsm:smart_gas_response = sg_get_clock_synchronisation_from_gsm_response;break;
		case sg_put_clock_synchronisation_from_gsm:smart_gas_response = sg_put_clock_synchronisation_from_gsm_response;break;
		case sg_get_battery_level:smart_gas_response = sg_get_battery_level_response;break;
		case sg_put_battery_level:smart_gas_response = sg_put_battery_level_response;break;
		case sg_get_temperature:smart_gas_response = sg_get_temperature_response;break;
		case sg_get_units:smart_gas_response = sg_get_units_response;break;
		case sg_put_units:smart_gas_response = sg_put_units_response;break;
		case sg_get_volume:smart_gas_response = sg_get_volume_response;break;
		case sg_put_volume:smart_gas_response = sg_put_volume_response;break;
		case sg_put_command_volume_offset:smart_gas_response = sg_put_command_volume_offset_response;break;
		case sg_get_calorific_value:smart_gas_response = sg_get_calorific_value_response;break;
		case sg_put_calorific_value:smart_gas_response = sg_put_calorific_value_response;break;
		case sg_get_energy:smart_gas_response = sg_get_energy_response;break;
		case sg_put_energy:smart_gas_response = sg_put_energy_response;break;
		case sg_put_command_energy_offset:smart_gas_response = sg_put_command_energy_offset_response;break;
		case sg_get_counters_registration_period:smart_gas_response = sg_get_counters_registration_period_response;break;
		case sg_put_counters_registration_period:smart_gas_response = sg_put_counters_registration_period_response;break;
		case sg_get_counters_archive_last_time:smart_gas_response = sg_get_counters_archive_last_time_response;break;
		case sg_get_counters_archive_last_volume:smart_gas_response = sg_get_counters_archive_last_volume_response;break;
		case sg_get_counters_archive_last_energy:smart_gas_response = sg_get_counters_archive_last_energy_response;break;
		case sg_get_gas_meter_q_max:smart_gas_response = sg_get_gas_meter_q_max_response;break;
		case sg_put_gas_meter_q_max:smart_gas_response = sg_put_gas_meter_q_max_response;break;
		case sg_get_temporary_flow:smart_gas_response = sg_get_temporary_flow_response;break;
		case sg_get_hourly_usage_current:smart_gas_response = sg_get_hourly_usage_current_response;break;
		case sg_get_monthly_data_latch_day:smart_gas_response = sg_get_monthly_data_latch_day_response;break;
		case sg_put_monthly_data_latch_day:smart_gas_response = sg_put_monthly_data_latch_day_response;break;
		case sg_get_monthly_and_daily_data_latch_hour:smart_gas_response = sg_get_monthly_and_daily_data_latch_hour_response;break;
		case sg_put_monthly_and_daily_data_latch_hour:smart_gas_response = sg_put_monthly_and_daily_data_latch_hour_response;break;
		case sg_get_monthly_and_daily_data_latch_local_time:smart_gas_response = sg_get_monthly_and_daily_data_latch_local_time_response;break;
		case sg_put_monthly_and_daily_data_latch_local_time:smart_gas_response = sg_put_monthly_and_daily_data_latch_local_time_response;break;
		case sg_get_last_daily_latch_time:smart_gas_response = sg_get_last_daily_latch_time_response;break;
		case sg_get_last_daily_latch_volume:smart_gas_response = sg_get_last_daily_latch_volume_response;break;
		case sg_get_last_daily_latch_energy:smart_gas_response = sg_get_last_daily_latch_energy_response;break;
		case sg_get_calibration_parameters:smart_gas_response = sg_get_calibration_parameters_response;break;
		case sg_put_calibration_parameters:smart_gas_response = sg_put_calibration_parameters_response;break;
		case sg_get_event_link:smart_gas_response = sg_get_event_link_response;break;
		case sg_put_event_link:smart_gas_response = sg_put_event_link_response;break;
		case sg_get_event_udp_port:smart_gas_response = sg_get_event_udp_port_response;break;
		case sg_put_event_udp_port:smart_gas_response = sg_put_event_udp_port_response;break;
		case sg_get_event_tcp_port:smart_gas_response = sg_get_event_tcp_port_response;break;
		case sg_put_event_tcp_port:smart_gas_response = sg_put_event_tcp_port_response;break;
		case sg_get_event_phone:smart_gas_response = sg_get_event_phone_response;break;
		case sg_put_event_phone:smart_gas_response = sg_put_event_phone_response;break;
		case sg_get_event_ip:smart_gas_response = sg_get_event_ip_response;break;
		case sg_put_event_ip:smart_gas_response = sg_put_event_ip_response;break;
		case sg_get_sim_pin:smart_gas_response = sg_get_sim_pin_response;break;
		case sg_put_sim_pin:smart_gas_response = sg_put_sim_pin_response;break;
		case sg_get_sim_iccid:smart_gas_response = sg_get_sim_iccid_response;break;
		case sg_put_sim_iccid:smart_gas_response = sg_put_sim_iccid_response;break;
		case sg_get_sim_phone:smart_gas_response = sg_get_sim_phone_response;break;
		case sg_put_sim_phone:smart_gas_response = sg_put_sim_phone_response;break;
		case sg_get_sim_ip:smart_gas_response = sg_get_sim_ip_response;break;
		case sg_put_sim_ip:smart_gas_response = sg_put_sim_ip_response;break;
		case sg_get_sim_smsc:smart_gas_response = sg_get_sim_smsc_response;break;
		case sg_put_sim_smsc:smart_gas_response = sg_put_sim_smsc_response;break;
		case sg_get_sim_sms_validity:smart_gas_response = sg_get_sim_sms_validity_response;break;
		case sg_put_sim_sms_validity:smart_gas_response = sg_put_sim_sms_validity_response;break;
		case sg_get_gprs_apn:smart_gas_response = sg_get_gprs_apn_response;break;
		case sg_put_gprs_apn:smart_gas_response = sg_put_gprs_apn_response;break;
		case sg_get_gprs_user:smart_gas_response = sg_get_gprs_user_response;break;
		case sg_put_gprs_user:smart_gas_response = sg_put_gprs_user_response;break;
		case sg_get_gprs_password:smart_gas_response = sg_get_gprs_password_response;break;
		case sg_put_gprs_password:smart_gas_response = sg_put_gprs_password_response;break;
		case sg_get_gsm_quality:smart_gas_response = sg_get_gsm_quality_response;break;
		case sg_put_gsm_quality:smart_gas_response = sg_put_gsm_quality_response;break;
		case sg_get_gsm_network_code:smart_gas_response = sg_get_gsm_network_code_response;break;
		case sg_put_gsm_network_code:smart_gas_response = sg_put_gsm_network_code_response;break;
		case sg_get_gsm_bts_lac:smart_gas_response = sg_get_gsm_bts_lac_response;break;
		case sg_put_gsm_bts_lac:smart_gas_response = sg_put_gsm_bts_lac_response;break;
		case sg_get_gsm_bts_cell_id:smart_gas_response = sg_get_gsm_bts_cell_id_response;break;
		case sg_put_gsm_bts_cell_id:smart_gas_response = sg_put_gsm_bts_cell_id_response;break;
		case sg_get_modem_state:smart_gas_response = sg_get_modem_state_response;break;
		case sg_put_modem_state:smart_gas_response = sg_put_modem_state_response;break;
		case sg_get_modem_sms_received:smart_gas_response = sg_get_modem_sms_received_response;break;
		case sg_put_modem_sms_received:smart_gas_response = sg_put_modem_sms_received_response;break;
		case sg_get_modem_sms_sent:smart_gas_response = sg_get_modem_sms_sent_response;break;
		case sg_put_modem_sms_sent:smart_gas_response = sg_put_modem_sms_sent_response;break;
		case sg_get_modem_gprs_packets_received:smart_gas_response = sg_get_modem_gprs_packets_received_response;break;
		case sg_put_modem_gprs_packets_received:smart_gas_response = sg_put_modem_gprs_packets_received_response;break;
		case sg_get_modem_gprs_packets_sent:smart_gas_response = sg_get_modem_gprs_packets_sent_response;break;
		case sg_put_modem_gprs_packets_sent:smart_gas_response = sg_put_modem_gprs_packets_sent_response;break;
		case sg_get_modem_wakeup_count:smart_gas_response = sg_get_modem_wakeup_count_response;break;
		case sg_put_modem_wakeup_count:smart_gas_response = sg_put_modem_wakeup_count_response;break;
		case sg_get_modem_wakeup_duration:smart_gas_response = sg_get_modem_wakeup_duration_response;break;
		case sg_put_modem_wakeup_duration:smart_gas_response = sg_put_modem_wakeup_duration_response;break;
		case sg_get_mkg_module_id:smart_gas_response = sg_get_mkg_module_type_response;break;
		case sg_put_mkg_module_id:smart_gas_response = sg_put_mkg_module_type_response;break;
		case sg_get_mkg_module_hw_ver:smart_gas_response = sg_get_mkg_hardware_version_response;break;
		case sg_put_mkg_module_hw_ver:smart_gas_response = sg_put_mkg_hardware_version_response;break;
		case sg_get_mkg_module_sw_ver:smart_gas_response = sg_get_mkg_software_version_response;break;
		case sg_put_mkg_module_sw_ver:smart_gas_response = sg_put_mkg_software_version_response;break;
		case sg_get_mkg_module_sn:smart_gas_response = sg_get_mkg_serial_number_response;break;
		case sg_put_mkg_module_sn:smart_gas_response = sg_put_mkg_serial_number_response;break;
		case sg_get_temporary_flow_period_latched:smart_gas_response = sg_get_temporary_flow_period_latched_response;break;
		case sg_get_temporary_flow_period_latched_time_compressed:smart_gas_response = sg_get_temporary_flow_period_latched_time_compressed_response;break;
		case sg_get_hourly_flow_period_latched:smart_gas_response = sg_get_hourly_flow_period_latched_response;break;
		case sg_get_hourly_flow_period_latched_time_compressed:smart_gas_response = sg_get_hourly_flow_period_latched_time_compressed_response;break;
		case sg_get_temporary_flow_monthly_latched:smart_gas_response = sg_get_temporary_flow_monthly_latched_response;break;
		case sg_get_temporary_flow_monthly_latched_time_compressed:smart_gas_response = sg_get_temporary_flow_monthly_latched_time_compressed_response;break;
		case sg_get_hourly_flow_monthly_latched:smart_gas_response = sg_get_hourly_flow_monthly_latched_response;break;
		case sg_get_hourly_flow_monthly_latched_time_compressed:smart_gas_response = sg_get_hourly_flow_monthly_latched_time_compressed_response;break;
		case sg_get_prepaid_topup_id:smart_gas_response = sg_get_prepaid_topup_id_response;break;
		case sg_put_prepaid_topup_id:smart_gas_response = sg_put_prepaid_topup_id_response;break;
		case sg_get_prepaid_topup_end_time:smart_gas_response = sg_get_prepaid_topup_end_time_response;break;
		case sg_put_prepaid_topup_end_time:smart_gas_response = sg_put_prepaid_topup_end_time_response;break;
		case sg_get_prepaid_topup_end_volume:smart_gas_response = sg_get_prepaid_topup_end_volume_response;break;
		case sg_put_prepaid_topup_end_volume:smart_gas_response = sg_put_prepaid_topup_end_volume_response;break;
		case sg_get_prepaid_topup_end_energy:smart_gas_response = sg_get_prepaid_topup_end_energy_response;break;
		case sg_put_prepaid_topup_end_energy:smart_gas_response = sg_put_prepaid_topup_end_energy_response;break;
		case sg_get_prepaid_valve_control:smart_gas_response = sg_get_prepaid_valve_control_response;break;
		case sg_put_prepaid_valve_control:smart_gas_response = sg_put_prepaid_valve_control_response;break;
		case sg_get_wdp_arp:smart_gas_response = sg_get_wdp_arp_response;break;
		case sg_put_wdp_arp:smart_gas_response = sg_put_wdp_arp_response;break;
		case sg_get_wdp_at:smart_gas_response = sg_get_wdp_at_response;break;
		case sg_put_wdp_at:smart_gas_response = sg_put_wdp_at_response;break;
		case sg_get_wdp_mr:smart_gas_response = sg_get_wdp_mr_response;break;
		case sg_put_wdp_mr:smart_gas_response = sg_put_wdp_mr_response;break;
		case sg_get_wpp_event_send_sn:smart_gas_response = sg_get_wpp_event_send_sn_response;break;
		case sg_put_wpp_event_send_sn:smart_gas_response = sg_put_wpp_event_send_sn_response;break;
		case sg_get_wpp_iv_in:smart_gas_response = sg_get_wpp_iv_in_response;break;
		case sg_put_wpp_iv_in:smart_gas_response = sg_put_wpp_iv_in_response;break;
		case sg_get_wpp_key:smart_gas_response = sg_get_wpp_key_response;break;
		case sg_put_wpp_key:smart_gas_response = sg_put_wpp_key_response;break;
		case sg_get_counters_archive:smart_gas_response = sg_get_counters_archive_response;break;
		case sg_put_prepare_parameters:smart_gas_response = sg_put_prepare_parameters_response;break;
		case sg_get_volume_max_value:smart_gas_response = sg_get_volume_max_value_response;break;
		case sg_put_volume_max_value:smart_gas_response = sg_put_volume_max_value_response;break;
		case sg_get_mkg_gsm_clock:smart_gas_response = sg_get_mkg_gsm_clock_response;break;
		case sg_put_mkg_gsm_clock:smart_gas_response = sg_put_mkg_gsm_clock_response;break;
		case sg_get_time_zone_auto_offset:smart_gas_response = sg_get_time_zone_auto_offset_response;break;
		case sg_put_time_zone_auto_offset:smart_gas_response = sg_put_time_zone_auto_offset_response;break;
		case sg_get_cal_modbus_start_addr_len:smart_gas_response = sg_get_cal_modbus_start_addr_len_response;break;
		case sg_put_cal_modbus_start_addr_len:smart_gas_response = sg_put_cal_modbus_start_addr_len_response;break;
		case sg_get_cal_modbus_holding_registers:smart_gas_response = sg_get_cal_modbus_holding_registers_response;timeout = 100;break;
		case sg_put_cal_modbus_holding_registers:smart_gas_response = sg_put_cal_modbus_holding_registers_response;break;
		case sg_get_cal_modbus_input_registers:smart_gas_response = sg_get_cal_modbus_input_registers_response;timeout = 100;break;
		case sg_get_wpp_cryptography_obligatory:smart_gas_response = sg_get_wpp_cryptography_obligatory_response;break;
		case sg_put_wpp_cryptography_obligatory:smart_gas_response = sg_put_wpp_cryptography_obligatory_response;break;
		case sg_get_wpp_access_control_obligatory:smart_gas_response = sg_get_wpp_access_control_obligatory_response;break;
		case sg_put_wpp_access_control_obligatory:smart_gas_response = sg_put_wpp_access_control_obligatory_response;break;
		case sg_get_sms_phone_numbers_restriction:smart_gas_response = sg_get_sms_phone_numbers_restriction_response;break;
		case sg_put_sms_phone_numbers_restriction:smart_gas_response = sg_put_sms_phone_numbers_restriction_response;break;
		case sg_get_permitted_phone_number:smart_gas_response = sg_get_permitted_phone_number_response;break;
		case sg_put_permitted_phone_number:smart_gas_response = sg_put_permitted_phone_number_response;break;
		case sg_put_valve_operation:smart_gas_response = sg_put_valve_operation_response;break;
		case sg_get_valve_activation_timeout:smart_gas_response = sg_get_valve_activation_timeout_response;break;
		case sg_put_valve_activation_timeout:smart_gas_response = sg_put_valve_activation_timeout_response;break;
		case sg_get_valve_state:smart_gas_response = sg_get_valve_state_response;break;
		case sg_get_valve_leak_test_duration:smart_gas_response = sg_get_valve_leak_test_duration_response;break;
		case sg_put_valve_leak_test_duration:smart_gas_response = sg_put_valve_leak_test_duration_response;break;
		case sg_get_valve_leak_test_volume:smart_gas_response = sg_get_valve_leak_test_volume_response;break;
		case sg_put_valve_leak_test_volume:smart_gas_response = sg_put_valve_leak_test_volume_response;break;
		case sg_get_valve_error_code:smart_gas_response = sg_get_valve_error_code_response;break;
		case sg_get_valve_open_count:smart_gas_response = sg_get_valve_open_count_response;break;
		case sg_put_valve_open_count:smart_gas_response = sg_put_valve_open_count_response;break;
		case sg_get_valve_close_count:smart_gas_response = sg_get_valve_close_count_response;break;
		case sg_put_valve_close_count:smart_gas_response = sg_put_valve_close_count_response;break;
		case sg_get_status:smart_gas_response = sg_get_status_response;break;
		case sg_get_status_periodic_latched:smart_gas_response = sg_get_status_periodic_latched_response;break;
		case sg_put_status_periodic_latched:smart_gas_response = sg_put_status_periodic_latched_response;break;
		case sg_get_status_monthly_latched:smart_gas_response = sg_get_status_monthly_latched_response;break;
		case sg_put_status_monthly_latched:smart_gas_response = sg_put_status_monthly_latched_response;break;
		case sg_get_status_up_event_match:smart_gas_response = sg_get_status_up_event_match_response;break;
		case sg_put_status_up_event_match:smart_gas_response = sg_put_status_up_event_match_response;break;
		case sg_get_status_down_event_match:smart_gas_response = sg_get_status_down_event_match_response;break;
		case sg_put_status_down_event_match:smart_gas_response = sg_put_status_down_event_match_response;break;
		case sg_schedule_year:break;
		case sg_schedule_month:break;
		case sg_schedule_day_of_month:break;
		case sg_schedule_day_of_week:break;
		case sg_schedule_hour:break;
		case sg_schedule_minute:break;
		case sg_schedule_local_time:break;
		case sg_schedule_command:break;
		case sg_get_schedule_table:smart_gas_response = sg_get_schedule_table_response;break;
		case sg_put_schedule_table:smart_gas_response = sg_put_schedule_table_response;break;
	}
	
	output_data[0] = obj_id >> 8;
	
	output_data[1] = obj_id & 0xff;
	
	if(smart_gas_response != NULL)
	{
		if(smart_gas_wrong_size(obj_id, input_data + 2, length - 2))
		{
			return finish_frame_smart_gas_error(sg_objd_err_wrong_size, output_data + 2) + 2;
		}
		else
		{
			if(timeout == 0)
			{
				return smart_gas_response(input_data + 2, length - 2, output_data + 2) + 2;
			}
			else
			{
				smart_gas_response(input_data + 2, length - 2, output_data + 2);
				
				delayed_response_obj_id = obj_id;
	
				smart_gas_object_source = source;
				
				delayed_response_smart_gas_response = smart_gas_response;
				
				start_time_event(smart_gas_timeout_idx);
				
				return -1;
			}
		}
	}
	
	return finish_frame_smart_gas_error(sg_objd_err_not_found, output_data + 2) + 2;
}





static bool delayed_response_cp_cal_modbus_holding_registers(uint8_t* data, uint16_t length)
{
	if(delayed_response_smart_gas_response == sg_get_cal_modbus_holding_registers_response)
	{
		return true;
	}
	
	return false;
}



static bool delayed_response_cp_cal_modbus_input_registers(uint8_t* data, uint16_t length)
{
	if(delayed_response_smart_gas_response == sg_get_cal_modbus_input_registers_response)
	{
		return true;
	}
	
	return false;
}



void smart_gas_calorimeter_response(enum calorimeter_parameters cp, uint8_t* data, uint16_t length)
{
	bool (*delayed_response)(uint8_t* data, uint16_t length) = NULL;
	
	switch(cp)
	{
		case cp_modbus_holding_registers:delayed_response = delayed_response_cp_cal_modbus_holding_registers;break;
		case cp_modbus_input_registers:delayed_response = delayed_response_cp_cal_modbus_input_registers;break;
	}
	
	if(delayed_response(data, length))
	{
		switch(smart_gas_object_source)
		{
			case sgos_irda:
			{
				irda_send_smart_gas(delayed_response_obj_id, data, length);
				break;
			}
			
			case sgos_radio:
			{
				radio_communication_send_smart_gas(delayed_response_obj_id, data, length);
				break;
			}
			
			case sgos_schedule:
			{
				break;
			}
		}
	}
	else
	{
		if(smart_gas_object_source == sgos_irda)
		{
			invoke_irda_receive();
		}
	}
}



static void smart_gas_timeout(void)
{
	if(smart_gas_object_source == sgos_irda)
	{
		invoke_irda_receive();
	}
	
	stop_time_event(smart_gas_timeout_idx);
}



void init_smart_gas(void)
{
	smart_gas_timeout_idx = register_time_event(smart_gas_timeout, 100, true);
}



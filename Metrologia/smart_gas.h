
#ifndef SMART_GAS_H_
#define SMART_GAS_H_


#include "stdbool.h"

#include "extras.h"

#include "calorimeter.h"


static const int sg_put = 0x1000;

//504,505,506,600,124,10D,10E,10F,217,218,219,21A,21B,421,422,423,424,425,42C,520,601,21C,511,512,513,514,515,516,517,518,519,51A,521,523,525,527,528,529,52A,605,606,607,60D,60E,60F
//?PUT 402,100,101,42B,106,109,508,509,10C,50F,211,123,120,121,122

enum smart_gas_object_ids
{
	sg_get_units																					= 0x100,
	sg_put_units																					= sg_get_units + sg_put,
	
	sg_get_counters_registration_period										= 0x101,
	sg_put_counters_registration_period										= sg_get_counters_registration_period + sg_put,
	
	sg_get_hourly_flow_period_latched_time_compressed			= 0x102,
	
	sg_get_time_zone_offset																= 0x103,
	sg_put_time_zone_offset																= sg_get_time_zone_offset + sg_put,
	
	sg_get_time_zone_offset_winter												= 0x104,
	sg_put_time_zone_offset_winter												= sg_get_time_zone_offset_winter + sg_put,
	
	sg_get_time_zone_offset_summer												= 0x105,
	sg_put_time_zone_offset_summer												= sg_get_time_zone_offset_summer + sg_put,
	
	sg_get_clock_synchronisation_from_gsm									= 0x106,
	sg_put_clock_synchronisation_from_gsm									= sg_get_clock_synchronisation_from_gsm + sg_put,
	
	sg_get_battery_level																	= 0x107,
	sg_put_battery_level																	= sg_get_battery_level + sg_put,
	
	sg_get_temperature																		= 0x108,
	
	sg_get_sim_sms_validity																= 0x109,
	sg_put_sim_sms_validity																= sg_get_sim_sms_validity + sg_put,
	
	sg_get_gsm_quality																		= 0x10A,
	sg_put_gsm_quality																		= sg_get_gsm_quality + sg_put,
	
	sg_get_modem_state																		= 0x10B,
	sg_put_modem_state																		= sg_get_modem_state + sg_put,
	
	sg_get_event_link																			= 0x10C,
	sg_put_event_link																			= sg_get_event_link + sg_put,
	
	sg_put_valve_operation																= 0x10D + sg_put,
	
	sg_get_valve_state																		= 0x10E + sg_put,
	
	sg_get_valve_error_code																= 0x10F + sg_put,
	
	sg_get_prepaid_topup_id																= 0x110,
	sg_put_prepaid_topup_id																= sg_get_prepaid_topup_id + sg_put,
	
	sg_get_prepaid_valve_control													= 0x111,
	sg_put_prepaid_valve_control													= sg_get_prepaid_valve_control + sg_put,
	
	sg_schedule_year																			= 0x112,
	
	sg_schedule_month																			= 0x113,
	
	sg_schedule_day_of_month															= 0x114,
	
	sg_schedule_day_of_week																= 0x115,
	
	sg_schedule_hour																			= 0x116,
	
	sg_schedule_minute																		= 0x117,
	
	sg_schedule_local_time																= 0x118,
	
	sg_get_wdp_arp																				= 0x119,
	sg_put_wdp_arp																				= sg_get_wdp_arp + sg_put,
	
	sg_get_wdp_at																					= 0x11a,
	sg_put_wdp_at																					= sg_get_wdp_at + sg_put,
	
	sg_get_wdp_mr																					= 0x11b,
	sg_put_wdp_mr																					= sg_get_wdp_mr + sg_put,
	
	sg_get_wpp_cryptography_obligatory										= 0x11c,
	sg_put_wpp_cryptography_obligatory										= sg_get_wpp_cryptography_obligatory + sg_put,
	
	sg_get_wpp_access_control_obligatory									= 0x11d,
	sg_put_wpp_access_control_obligatory									= sg_get_wpp_access_control_obligatory + sg_put,
	
	sg_get_monthly_data_latch_day													= 0x120,
	sg_put_monthly_data_latch_day													= sg_get_monthly_data_latch_day + sg_put,
	
	sg_get_monthly_and_daily_data_latch_hour							= 0x121,
	sg_put_monthly_and_daily_data_latch_hour							= sg_get_monthly_and_daily_data_latch_hour + sg_put,
	
	sg_get_monthly_and_daily_data_latch_local_time				= 0x122,
	sg_put_monthly_and_daily_data_latch_local_time				= sg_get_monthly_and_daily_data_latch_local_time + sg_put,
	
	sg_get_wpp_event_send_sn															= 0x123,
	sg_put_wpp_event_send_sn															= sg_get_wpp_event_send_sn + sg_put,
	
	sg_get_time_zone_auto_offset													= 0x124,
	sg_put_time_zone_auto_offset													= sg_get_time_zone_auto_offset + sg_put,
	
	sg_get_sms_phone_numbers_restriction									= 0x125,
	sg_put_sms_phone_numbers_restriction									= sg_get_sms_phone_numbers_restriction + sg_put,
	
	sg_put_prepare_parameters															= 0x180 + sg_put,
	
	sg_get_standard_parameter_list_version								= 0x200,
	
	sg_get_calorific_value																= 0x201,
	sg_put_calorific_value																= sg_get_calorific_value + sg_put,
	
	sg_get_gas_meter_q_max																= 0x202,
	sg_put_gas_meter_q_max																= sg_get_gas_meter_q_max + sg_put,
	
	sg_get_temporary_flow																	= 0x203,
	
	sg_get_hourly_usage_current														= 0x204,
	
	sg_get_temporary_flow_period_latched									= 0x205,
	
	sg_get_temporary_flow_period_latched_time_compressed	= 0x206,
	
	sg_get_hourly_flow_period_latched											= 0x207,
	
	sg_get_temporary_flow_monthly_latched									= 0x208,
	
	sg_get_temporary_flow_monthly_latched_time_compressed	= 0x209,
	
	sg_get_hourly_flow_monthly_latched										= 0x20a,
	
	sg_get_hourly_flow_monthly_latched_time_compressed		= 0x20b,
	
	sg_get_clock_synchronisation_offset										= 0x20C,
	sg_put_clock_synchronisation_offset										= sg_get_clock_synchronisation_offset + sg_put,
	
	sg_get_modem_sms_received															= 0x210,
	sg_put_modem_sms_received															= sg_get_modem_sms_received + sg_put,
	
	sg_get_modem_sms_sent																	= 0x211,
	sg_put_modem_sms_sent																	= sg_get_modem_sms_sent + sg_put,
	
	sg_get_modem_gprs_packets_received										= 0x212,
	sg_put_modem_gprs_packets_received										= sg_get_modem_gprs_packets_received + sg_put,
	
	sg_get_modem_gprs_packets_sent												= 0x213,
	sg_put_modem_gprs_packets_sent												= sg_get_modem_gprs_packets_sent + sg_put,
	
	sg_get_modem_wakeup_count															= 0x214,
	sg_put_modem_wakeup_count															= sg_get_modem_wakeup_count + sg_put,
	
	sg_get_event_udp_port																	= 0x215,
	sg_put_event_udp_port																	= sg_get_event_udp_port + sg_put,
	
	sg_get_event_tcp_port																	= 0x216,
	sg_put_event_tcp_port																	= sg_get_event_tcp_port + sg_put,
	
	sg_get_valve_activation_timeout												= 0x217,
	sg_put_valve_activation_timeout												= sg_get_valve_activation_timeout + sg_put,
	
	sg_get_valve_leak_test_duration												= 0x218,
	sg_put_valve_leak_test_duration												= sg_get_valve_leak_test_duration + sg_put,
	
	sg_get_valve_leak_test_volume													= 0x219,
	sg_put_valve_leak_test_volume													= sg_get_valve_leak_test_volume + sg_put,
	
	sg_get_valve_open_count																= 0x21A,
	sg_put_valve_open_count																= sg_get_valve_open_count + sg_put,
	
	sg_get_valve_close_count															= 0x21B,
	sg_put_valve_close_count															= sg_get_valve_close_count + sg_put,
		
	sg_get_serial_nbr																			= 0x400,
	sg_put_serial_nbr																			= sg_get_serial_nbr + sg_put,
	
	sg_get_latitude																				= 0x401,
	sg_put_latitude																				= sg_get_latitude + sg_put,
	
	sg_get_longitude																			= 0x402,
	sg_put_longitude																			= sg_get_longitude + sg_put,
	
	sg_get_hardware_version																= 0x403,
	
	sg_get_software_version																= 0x404,
	
	sg_get_volume																					= 0x405,
	sg_put_volume																					= sg_get_volume + sg_put,
	
	sg_put_command_volume_offset													= 0x406 + sg_put,
	
	sg_get_energy																					= 0x407,
	sg_put_energy																					= sg_get_energy + sg_put,
	
	sg_put_command_energy_offset													= 0x408 + sg_put,
	
	sg_get_counters_archive_last_time											= 0x409,
	
	sg_get_counters_archive_last_volume										= 0x40A,
	
	sg_get_counters_archive_last_energy										= 0x40B,
	
	sg_get_clock																					= 0x40C,
	sg_put_clock																					= sg_get_clock + sg_put,
	
	sg_get_sim_ip																					= 0x419,
	sg_put_sim_ip																					= sg_get_sim_ip + sg_put,
	
	sg_get_gsm_bts_lac																		= 0x41A,
	sg_put_gsm_bts_lac																		= sg_get_gsm_bts_lac + sg_put,
	
	sg_get_gsm_bts_cell_id																= 0x41B,
	sg_put_gsm_bts_cell_id																= sg_get_gsm_bts_cell_id + sg_put,
	
	sg_get_modem_wakeup_duration													= 0x41C,
	sg_put_modem_wakeup_duration													= sg_get_modem_wakeup_duration + sg_put,
	
	sg_get_event_ip																				= 0x41D,
	sg_put_event_ip																				= sg_get_event_ip + sg_put,
	
	sg_get_prepaid_topup_end_time													= 0x41E,
	sg_put_prepaid_topup_end_time													= sg_get_prepaid_topup_end_time + sg_put,
	
	sg_get_prepaid_topup_end_volume												= 0x41F,
	sg_put_prepaid_topup_end_volume												= sg_get_prepaid_topup_end_volume + sg_put,
	
	sg_get_prepaid_topup_end_energy												= 0x420,
	sg_put_prepaid_topup_end_energy												= sg_get_prepaid_topup_end_energy + sg_put,
	
	sg_get_status																					= 0x421,
	
	sg_get_status_periodic_latched												= 0x422,
	sg_put_status_periodic_latched												= sg_get_status_periodic_latched + sg_put,
	
	sg_get_status_monthly_latched													= 0x423,
	sg_put_status_monthly_latched													= sg_get_status_monthly_latched + sg_put,
	
	sg_get_status_up_event_match													= 0x424,
	sg_put_status_up_event_match													= sg_get_status_up_event_match + sg_put,
	
	sg_get_status_down_event_match												= 0x425,
	sg_put_status_down_event_match												= sg_get_status_down_event_match + sg_put,
	
	sg_get_wpp_iv_in																			= 0x426,
	sg_put_wpp_iv_in																			= sg_get_wpp_iv_in + sg_put,
	
	sg_get_last_daily_latch_time													= 0x428,
	
	sg_get_last_daily_latch_volume												= 0x429,
	
	sg_get_last_daily_latch_energy												= 0x42A,
	
	sg_get_volume_max_value																= 0x42B,
	sg_put_volume_max_value																= sg_get_volume_max_value + sg_put,
	
	sg_get_mkg_module_id																	= 0x480,
	sg_put_mkg_module_id																	= sg_get_mkg_module_id + sg_put,
	
	sg_get_mkg_module_hw_ver															= 0x481,
	sg_put_mkg_module_hw_ver															= sg_get_mkg_module_hw_ver + sg_put,
	
	sg_get_mkg_module_sw_ver															= 0x482,
	sg_put_mkg_module_sw_ver															= sg_get_mkg_module_sw_ver + sg_put,
	
	sg_get_mkg_module_sn																	= 0x483,
	sg_put_mkg_module_sn																	= sg_get_mkg_module_sn + sg_put,
	
	sg_get_cal_modbus_start_addr_len											= 0x484,
	sg_put_cal_modbus_start_addr_len											= sg_get_cal_modbus_start_addr_len + sg_put,
	
	sg_get_client_id																			= 0x500,
	sg_put_client_id																			= sg_get_client_id + sg_put,
	
	sg_get_gas_meter_serial_nbr														= 0x501,
	sg_put_gas_meter_serial_nbr														= sg_get_gas_meter_serial_nbr + sg_put,
	
	sg_get_device_type_name																= 0x502,
	sg_put_device_type_name																= sg_get_device_type_name + sg_put,
	
	sg_get_sim_pin																				= 0x507,
	sg_put_sim_pin																				= sg_get_sim_pin + sg_put,
	
	sg_get_sim_iccid																			= 0x508,
	sg_put_sim_iccid																			= sg_get_sim_iccid + sg_put,
	
	sg_get_sim_phone																			= 0x509,
	sg_put_sim_phone																			= sg_get_sim_phone + sg_put,
	
	sg_get_sim_smsc																				= 0x50A,
	sg_put_sim_smsc																				= sg_get_sim_smsc + sg_put,
	
	sg_get_gprs_apn																				= 0x50B,
	sg_put_gprs_apn																				= sg_get_gprs_apn + sg_put,
	
	sg_get_gprs_user																			= 0x50C,
	sg_put_gprs_user																			= sg_get_gprs_user + sg_put,
	
	sg_get_gprs_password																	= 0x50D,
	sg_put_gprs_password																	= sg_get_gprs_password + sg_put,
	
	sg_get_gsm_network_code																= 0x50E,
	sg_put_gsm_network_code																= sg_get_gsm_network_code + sg_put,
	
	sg_get_event_phone																		= 0x50F,
	sg_put_event_phone																		= sg_get_event_phone + sg_put,
	
	sg_schedule_command																		= 0x510,
	
	sg_get_wpp_key																				= 0x51B,
	sg_put_wpp_key																				= sg_get_wpp_key + sg_put,
	
	sg_get_permitted_phone_number													= 0x51C,
	sg_put_permitted_phone_number													= sg_get_permitted_phone_number + sg_put,
	
	sg_get_mkg_gsm_clock																	= 0x580,
	sg_put_mkg_gsm_clock																	= sg_get_mkg_gsm_clock + sg_put,
	
	sg_get_cal_modbus_holding_registers										= 0x581,
	sg_put_cal_modbus_holding_registers										= sg_get_cal_modbus_holding_registers + sg_put,
	
	sg_get_cal_modbus_input_registers											= 0x582,
	
	sg_get_calibration_parameters													= 0x5A0,
	sg_put_calibration_parameters													= sg_get_calibration_parameters + sg_put,
	
	sg_get_counters_archive																= 0x600,
	
	sg_get_schedule_table																	= 0x601,
	sg_put_schedule_table																	= sg_get_schedule_table + sg_put,
};


enum smart_gas_object_sources
{
	sgos_irda			= 1,
	sgos_radio		= 2,
	sgos_schedule	= 3,
};



int smart_gas_object(uint8_t* buffer, uint16_t length, uint8_t* response_buffer, uint16_t max_output_data_length, enum smart_gas_object_sources source);

int get_smart_gas_max_output_length(void);

void init_smart_gas(void);

void smart_gas_calorimeter_response(enum calorimeter_parameters cp, uint8_t* data, uint16_t length);


#endif /*SMART_GAS_H_*/

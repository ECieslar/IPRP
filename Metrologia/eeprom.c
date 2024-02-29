
#include "eeprom.h"

#include "driver\driver_eeprom.h"



static bool parameter_error;




static bool eeprom_parameter_error(int address1, int length1, int address2)
{
	return address1 + length1 > address2;
}



#define parameter_error(name1, name2) eeprom_parameter_error(EA(name1), EL(name1), EA(name2))



void init_eeprom(void)
{
	static bool initialized = false;
	
	if(initialized)
	{
		return;
	}
	
	init_driver_eeprom();
	
	initialized = true;
	
	parameter_error = false;
	
	if(parameter_error(SERIAL_NUMBER, LOCATION_LATITUDE)) parameter_error = true;
	if(parameter_error(LOCATION_LONGITUDE, CLIENT_ID)) parameter_error = true;
	if(parameter_error(CLIENT_ID, GAS_METER_SERIAL_NBR)) parameter_error = true;
	if(parameter_error(GAS_METER_SERIAL_NBR, DEVICE_TYPE_NAME)) parameter_error = true;
	if(parameter_error(DEVICE_TYPE_NAME, CLOCK_SYNCHRONISATION_OFFSET)) parameter_error = true;
	if(parameter_error(CLOCK_SYNCHRONISATION_OFFSET, TIME_ZONE_OFFSET)) parameter_error = true;
	if(parameter_error(TIME_ZONE_OFFSET, TIME_ZONE_OFFSET_WINTER)) parameter_error = true;
	if(parameter_error(TIME_ZONE_OFFSET_WINTER, TIME_ZONE_OFFSET_SUMMER)) parameter_error = true;
	if(parameter_error(TIME_ZONE_OFFSET_SUMMER, CLOCK_SYNCHRONISATION_FROM_GSM)) parameter_error = true;
	if(parameter_error(CLOCK_SYNCHRONISATION_FROM_GSM, UNITS)) parameter_error = true;
	if(parameter_error(UNITS, CALORIFIC_VALUE)) parameter_error = true;
	if(parameter_error(CALORIFIC_VALUE, COUNTERS_REGISTRATION_PERIOD)) parameter_error = true;
	if(parameter_error(COUNTERS_REGISTRATION_PERIOD, MONTHLY_DATA_LATCH_DAY)) parameter_error = true;
	if(parameter_error(MONTHLY_DATA_LATCH_DAY, MONTHLY_AND_DAILY_DATA_LATCH_HOUR)) parameter_error = true;
	if(parameter_error(MONTHLY_AND_DAILY_DATA_LATCH_HOUR, MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME)) parameter_error = true;
	if(parameter_error(MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME, CALIBRATION_TEMPERATURES_COUNT)) parameter_error = true;
	if(parameter_error(CALIBRATION_TEMPERATURES_COUNT, CALIBRATION_FLOWS_COUNT)) parameter_error = true;
	if(parameter_error(CALIBRATION_FLOWS_COUNT, CALIBRATION_TEMPERATURES)) parameter_error = true;
	if(parameter_error(CALIBRATION_TEMPERATURES, CALIBRATION_FLOWS)) parameter_error = true;
	if(parameter_error(CALIBRATION_FLOWS, CALIBRATION_COEFFICIENTS)) parameter_error = true;
	if(parameter_error(CALIBRATION_COEFFICIENTS, EVENT_LINK)) parameter_error = true;
	if(parameter_error(EVENT_LINK, EVENT_UDP_PORT)) parameter_error = true;
	if(parameter_error(EVENT_UDP_PORT, EVENT_TCP_PORT)) parameter_error = true;
	if(parameter_error(EVENT_TCP_PORT, EVENT_PHONE)) parameter_error = true;
	if(parameter_error(EVENT_PHONE, EVENT_IP)) parameter_error = true;
	if(parameter_error(EVENT_IP, SIM_PIN)) parameter_error = true;
	if(parameter_error(SIM_PIN, SIM_ICCID)) parameter_error = true;
	if(parameter_error(SIM_ICCID, SIM_PHONE)) parameter_error = true;
	if(parameter_error(SIM_PHONE, SIM_IP)) parameter_error = true;
	if(parameter_error(SIM_IP, SIM_SMSC)) parameter_error = true;
	if(parameter_error(SIM_SMSC, SIM_SMS_VALIDITY)) parameter_error = true;
	if(parameter_error(SIM_SMS_VALIDITY, GPRS_APN)) parameter_error = true;
	if(parameter_error(GPRS_APN, GPRS_USER)) parameter_error = true;
	if(parameter_error(GPRS_USER, GPRS_PASSWORD)) parameter_error = true;
	if(parameter_error(GPRS_PASSWORD, GSM_QUALITY)) parameter_error = true;
	if(parameter_error(GSM_QUALITY, GSM_NETWORK_CODE)) parameter_error = true;
	if(parameter_error(GSM_NETWORK_CODE, GSM_BTS_LAC)) parameter_error = true;
	if(parameter_error(GSM_BTS_LAC, GSM_BTS_CELL_ID)) parameter_error = true;
	if(parameter_error(GSM_BTS_CELL_ID, MKG_MODULE_TYPE)) parameter_error = true;
	if(parameter_error(MKG_MODULE_TYPE, MKG_HARDWARE_VERSION)) parameter_error = true;
	if(parameter_error(MKG_HARDWARE_VERSION, MKG_SOFTWARE_VERSION)) parameter_error = true;
	if(parameter_error(MKG_SOFTWARE_VERSION, MKG_SERIAL_NUMBER)) parameter_error = true;
	if(parameter_error(MKG_SERIAL_NUMBER, PREPAID_TOPUP_ID)) parameter_error = true;
	if(parameter_error(PREPAID_TOPUP_ID, PREPAID_TOPUP_END_TIME)) parameter_error = true;
	if(parameter_error(PREPAID_TOPUP_END_TIME, PREPAID_TOPUP_END_VOLUME)) parameter_error = true;
	if(parameter_error(PREPAID_TOPUP_END_VOLUME, PREPAID_TOPUP_END_ENERGY)) parameter_error = true;
	if(parameter_error(PREPAID_TOPUP_END_ENERGY, PREPAID_VALVE_CONTROL)) parameter_error = true;
	if(parameter_error(PREPAID_VALVE_CONTROL, WDP_ARP)) parameter_error = true;
	if(parameter_error(WDP_ARP, WDP_AT)) parameter_error = true;
	if(parameter_error(WDP_AT, WDP_MR)) parameter_error = true;
	if(parameter_error(WDP_MR, WPP_EVENT_SEND_SN)) parameter_error = true;
	if(parameter_error(WPP_EVENT_SEND_SN, WPP_IV_IN)) parameter_error = true;
	if(parameter_error(WPP_IV_IN, WPP_KEY)) parameter_error = true;
	if(parameter_error(WPP_KEY, COUNTERS_ARCHIVE_PAGE_NR)) parameter_error = true;
	if(parameter_error(COUNTERS_ARCHIVE_PAGE_NR, GAS_METER_Q_MAX)) parameter_error = true;
	if(parameter_error(GAS_METER_Q_MAX, TIME_ZONE_AUTO_OFFSET)) parameter_error = true;
	if(parameter_error(TIME_ZONE_AUTO_OFFSET, WPP_CRYPTOGRAPHY_OBLIGATORY)) parameter_error = true;
	if(parameter_error(WPP_CRYPTOGRAPHY_OBLIGATORY, WPP_ACCESS_CONTROL_OBLIGATORY)) parameter_error = true;
	if(parameter_error(WPP_ACCESS_CONTROL_OBLIGATORY, SMS_PHONE_NUMBERS_RESTRICTION)) parameter_error = true;
	if(parameter_error(SMS_PHONE_NUMBERS_RESTRICTION, PERMITTED_PHONE_NUMBER)) parameter_error = true;
	if(parameter_error(PERMITTED_PHONE_NUMBER, PREPARATION_SIGNATURE)) parameter_error = true;
	if(parameter_error(PREPARATION_SIGNATURE, VALVE_OPERATION)) parameter_error = true;
	if(parameter_error(VALVE_OPERATION, VALVE_ACTIVATION_TIMEOUT)) parameter_error = true;
	if(parameter_error(VALVE_ACTIVATION_TIMEOUT, VALVE_LEAK_TEST_DURATION)) parameter_error = true;
	if(parameter_error(VALVE_LEAK_TEST_DURATION, VALVE_LEAK_TEST_VOLUME)) parameter_error = true;
	if(parameter_error(VALVE_LEAK_TEST_VOLUME, VALVE_OPEN_COUNT)) parameter_error = true;
	if(parameter_error(VALVE_OPEN_COUNT, VALVE_CLOSE_COUNT)) parameter_error = true;
	if(parameter_error(VALVE_CLOSE_COUNT, VALVE_STATE)) parameter_error = true;
	if(parameter_error(VALVE_STATE, COUNTERS_ARCHIVE)) parameter_error = true;
	if(parameter_error(COUNTERS_ARCHIVE, ENERGY_VOLUME)) parameter_error = true;
}


void eeprom_write(uint8_t* buffer, int address, int length)
{
	driver_eeprom_write(buffer, address, length);
}


void eeprom_read(uint8_t* buffer, int address, int length)
{
	driver_eeprom_read(buffer, address, length);
}


bool get_eeprom_parameter_error(void)
{
	return parameter_error;
}




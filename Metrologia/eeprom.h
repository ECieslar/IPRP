#ifndef EEPROM_H_
#define EEPROM_H_


#include "stdbool.h"

#include "extras.h"

void init_eeprom(void);

void eeprom_write(uint8_t* buffer, int address, int length);

void eeprom_read(uint8_t* buffer, int address, int length);

#define eeprom_write_parameter(buffer, name) eeprom_write(buffer, EA(name), EL(name))

#define eeprom_read_parameter(buffer, name) eeprom_read(buffer, EA(name), EL(name))

#define EA(name) EEPROM_ADDRESS_##name

#define EL(name) EEPROM_LENGTH_##name

#define EP(name, address, length)	static const int EA(name) = address; \
																	static const int EL(name) = length;
																	
bool get_eeprom_parameter_error(void);




EP(SERIAL_NUMBER, 0, 4)
EP(LOCATION_LATITUDE, 4, 4)
EP(LOCATION_LONGITUDE, 8, 4)
EP(CLIENT_ID, 12, 33)
EP(GAS_METER_SERIAL_NBR, 45, 21)
EP(DEVICE_TYPE_NAME, 66, 11)
EP(CLOCK_SYNCHRONISATION_OFFSET, 87, 2)
EP(TIME_ZONE_OFFSET, 89, 1)
EP(TIME_ZONE_OFFSET_WINTER, 90, 1)
EP(TIME_ZONE_OFFSET_SUMMER, 91, 1)
EP(CLOCK_SYNCHRONISATION_FROM_GSM, 92, 1)
EP(UNITS, 93, 1)
EP(CALORIFIC_VALUE, 94, 2)
EP(COUNTERS_REGISTRATION_PERIOD, 96, 1)
EP(MONTHLY_DATA_LATCH_DAY, 97, 1)
EP(MONTHLY_AND_DAILY_DATA_LATCH_HOUR, 98, 1)
EP(MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME, 99, 1)
EP(CALIBRATION_TEMPERATURES_COUNT, 100, 1)
EP(CALIBRATION_FLOWS_COUNT, 101, 1)
EP(CALIBRATION_TEMPERATURES, 102, 8)
EP(CALIBRATION_FLOWS, 110, 16)
EP(CALIBRATION_COEFFICIENTS, 126, 128)
EP(EVENT_LINK, 254, 1)
EP(EVENT_UDP_PORT, 255, 2)
EP(EVENT_TCP_PORT, 257, 2)
EP(EVENT_PHONE, 259, 8)
EP(EVENT_IP, 267, 4)
EP(SIM_PIN, 271, 9)
EP(SIM_ICCID, 280, 11)
EP(SIM_PHONE, 291, 8)
EP(SIM_IP, 299, 4)
EP(SIM_SMSC, 303, 17)
EP(SIM_SMS_VALIDITY, 320, 1)
EP(GPRS_APN, 321, 33)
EP(GPRS_USER, 354, 17)
EP(GPRS_PASSWORD, 371, 17)
EP(GSM_QUALITY, 388, 1)
EP(GSM_NETWORK_CODE, 389, 9)
EP(GSM_BTS_LAC, 398, 4)
EP(GSM_BTS_CELL_ID, 402, 4)
EP(MKG_MODULE_TYPE, 406, 4)
EP(MKG_HARDWARE_VERSION, 410, 4)
EP(MKG_SOFTWARE_VERSION, 414, 4)
EP(MKG_SERIAL_NUMBER, 418, 4)
EP(PREPAID_TOPUP_ID, 422, 1)
EP(PREPAID_TOPUP_END_TIME, 423, 4)
EP(PREPAID_TOPUP_END_VOLUME, 427, 4)
EP(PREPAID_TOPUP_END_ENERGY, 431, 4)
EP(PREPAID_VALVE_CONTROL, 435, 1)
EP(WDP_ARP, 436, 1)
EP(WDP_AT, 437, 1)
EP(WDP_MR, 438, 1)
EP(WPP_EVENT_SEND_SN, 439, 1)
EP(WPP_IV_IN, 440, 4)
EP(WPP_KEY, 444, 17)
EP(COUNTERS_ARCHIVE_PAGE_NR, 461, 4)
EP(GAS_METER_Q_MAX, 465, 2)
EP(TIME_ZONE_AUTO_OFFSET, 467, 1)
EP(WPP_CRYPTOGRAPHY_OBLIGATORY, 468, 1)
EP(WPP_ACCESS_CONTROL_OBLIGATORY, 469, 1)
EP(SMS_PHONE_NUMBERS_RESTRICTION, 470, 1)
EP(PERMITTED_PHONE_NUMBER, 471, 8)
EP(PREPARATION_SIGNATURE, 479, 4)
EP(VALVE_OPERATION, 483, 1)
EP(VALVE_ACTIVATION_TIMEOUT, 484, 2)
EP(VALVE_LEAK_TEST_DURATION, 486, 2)
EP(VALVE_LEAK_TEST_VOLUME, 488, 2)
EP(VALVE_OPEN_COUNT, 490, 2)
EP(VALVE_CLOSE_COUNT, 492, 2)
EP(VALVE_STATE, 494, 1)
EP(COUNTERS_ARCHIVE, 1024, 1024)
EP(ENERGY_VOLUME, 2048, 1536)




#endif /*EEPROM_H_*/

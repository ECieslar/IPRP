
#ifndef DATE_TIME_H_
#define DATE_TIME_H_


#include "inttypes.h"

#include "stdbool.h"


typedef struct
{
	int second;
	int minute;
	int hour;
	
	int day;
	int day_week;
	int month;
	int year;
} rtc_t;



void init_date_time(void);

void prepare_date_time(void);

void update_date_time(void);

void get_date_time_utc(rtc_t* dt);

void set_date_time_utc(rtc_t* dt);

void get_date_time_local(rtc_t* dt);

void set_date_time_local(rtc_t* dt);

uint32_t get_unix_time_utc(void);

void set_unix_time_utc(uint32_t ut);

uint32_t get_unix_time_local(void);

void set_unix_time_local(uint32_t ut);

int16_t get_clock_synchronisation_offset(void);

void set_clock_synchronisation_offset(int16_t cso);

int8_t get_time_zone_offset(void);

void set_time_zone_offset(int8_t tzo);

int8_t get_time_zone_offset_winter(void);

void set_time_zone_offset_winter(int8_t tzow);

int8_t get_time_zone_offset_summer(void);

void set_time_zone_offset_summer(int8_t tzos);

int8_t get_clock_synchronisation_from_gsm(void);

bool set_clock_synchronisation_from_gsm(int8_t csfg);

void convert_unix_time_to_date_time(uint32_t unix_time, rtc_t* dt);

uint8_t get_time_zone_auto_offset(void);

bool set_time_zone_auto_offset(uint8_t tzao);

int get_weekday(rtc_t* dt);

#endif /*DATE_TIME_H_*/

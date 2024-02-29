

#include "date_time.h"

#include "lcd.h"

#include "extras.h"

#include "menu.h"

#include "eeprom.h"

#include "driver\driver_timers.h"

#include "driver\driver_rtc.h"


#include <string.h>

static int16_t clock_synchronisation_offset;

static int8_t time_zone_offset;

static int8_t time_zone_offset_winter;

static int8_t time_zone_offset_summer;

static int8_t clock_synchronisation_from_gsm;

static uint8_t time_zone_auto_offset;




void update_date_time(void)
{
	menu_event(menu_event_time_change, NULL);
}



static void init_clock_synchronisation_offset(void)
{
	eeprom_read_parameter((uint8_t*)&clock_synchronisation_offset, CLOCK_SYNCHRONISATION_OFFSET);
}


static void prepare_clock_synchronisation_offset(void)
{
	set_clock_synchronisation_offset(0);
}



static void init_time_zone_offset(void)
{
	eeprom_read_parameter((uint8_t*)&time_zone_offset, TIME_ZONE_OFFSET);
}



static void prepare_time_zone_offset(void)
{
	set_time_zone_offset(4);
}



static void init_time_zone_offset_winter(void)
{
	eeprom_read_parameter((uint8_t*)&time_zone_offset_winter, TIME_ZONE_OFFSET_WINTER);
}



static void prepare_time_zone_offset_winter(void)
{
	set_time_zone_offset_winter(4);
}



static void init_time_zone_offset_summer(void)
{
	eeprom_read_parameter((uint8_t*)&time_zone_offset_summer, TIME_ZONE_OFFSET_SUMMER);
}



static void prepare_time_zone_offset_summer(void)
{
	set_time_zone_offset_summer(8);
}



static void init_clock_synchronisation_from_gsm(void)
{
	eeprom_read_parameter((uint8_t*)&clock_synchronisation_from_gsm, CLOCK_SYNCHRONISATION_FROM_GSM);
	
	if((clock_synchronisation_from_gsm != 0) && (clock_synchronisation_from_gsm != 1))
	{
		set_clock_synchronisation_from_gsm(1);
	}
}



static void prepare_clock_synchronisation_from_gsm(void)
{
	set_clock_synchronisation_from_gsm(1);
}



static void init_time_zone_auto_offset(void)
{
	eeprom_read_parameter((uint8_t*)&time_zone_auto_offset, TIME_ZONE_AUTO_OFFSET);
	
	if((time_zone_auto_offset != 0) && (time_zone_auto_offset != 1))
	{
		set_time_zone_auto_offset(0);
	}
}



static void prepare_time_zone_auto_offset(void)
{
	set_time_zone_auto_offset(0);
}



void init_date_time(void)
{	
	driver_rtc_init();
		
	init_clock_synchronisation_offset();
	
	init_time_zone_offset();
	
	init_time_zone_offset_winter();
	
	init_time_zone_offset_summer();
	
	init_clock_synchronisation_from_gsm();
	
	init_time_zone_auto_offset();
}



void prepare_date_time(void)
{
	prepare_clock_synchronisation_offset();
	
	prepare_time_zone_offset();
	
	prepare_time_zone_offset_winter();
	
	prepare_time_zone_offset_summer();
	
	prepare_clock_synchronisation_from_gsm();
	
	prepare_time_zone_auto_offset();
}




static uint8_t months_normal_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8_t months_leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};




void convert_unix_time_to_date_time(uint32_t unix_time, rtc_t* dt)
{
	uint32_t ut = unix_time;
	
	dt->second = ut % 60;
	
	ut /= 60;
	
	dt->minute = ut % 60;
	
	ut /= 60;
	
	dt->hour = ut % 24;
	
	ut /= 24;
	
	
	
	dt->year = 1970 + ((ut / (366 + 3 * 365)) << 2);
	
	ut -= ((dt->year - 1970) >> 2) * (366 + 3 * 365);
	
	if(ut >= 365)
	{
		dt->year++;
		ut -= 365;
	}
	
	if(ut >= 365)
	{
		dt->year++;
		ut -= 365;
	}
	
	if(ut >= 366)
	{
		dt->year++;
		ut -= 366;
	}
	
	
	
	uint8_t* months;
	
	if((dt->year & 3) == 0)
	{
		months = months_leap_year;
	}
	else
	{
		months = months_normal_year;
	}
	
	dt->month = 1;
	
	for(int i = 0; i < 12; i++)
	{
		if(ut >= months[i])
		{
			dt->month++;
			
			ut -= months[i];
		}
		else
		{
			break;
		}
	}
	
	dt->day = ut + 1;
}


uint32_t convert_date_time_to_unix_time(rtc_t* dt)
{
	uint32_t unix_time = ((dt->year - 1970) >> 2) * (366 + 3 * 365);

	switch (dt->year & 3)
	{
		case 1: unix_time += 366;
		case 0: unix_time += 365;
		case 3: unix_time += 365;
	}

	uint8_t months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if ((dt->year & 3) == 0)
	{
		months[1] = 29;
	}

	for (int i = 1; i < dt->month; i++)
	{
		unix_time += months[i - 1];
	}

	unix_time += dt->day - 1;

	unix_time *= 24;

	unix_time += dt->hour;

	unix_time *= 60;

	unix_time += dt->minute;

	unix_time *= 60;

	unix_time += dt->second;

	return unix_time;
}


void set_date_time_utc(rtc_t* dt)
{
	driver_rtc_set_date_time(dt->year, dt->month, dt->day, dt->hour, dt->minute, dt->second);
}


void get_date_time_utc(rtc_t* dt)
{
	uint16_t year;
	uint8_t month, day, day_week, hour, minute, second;
	
	driver_rtc_get_date_time(&year, &month, &day, &day_week, &hour, &minute, &second);
	
	dt->year = year;
	dt->month = month;
	dt->day = day;
	dt->day_week = day_week;
	dt->hour = hour;
	dt->minute = minute;
	dt->second = second;
}



static int32_t get_offset(void)
{
	int32_t offset = time_zone_offset;
	
	if(time_zone_auto_offset == 0)
	{
		if(time_zone_offset == 127)
		{
			offset = time_zone_offset_winter;
		}
		else if(time_zone_offset == -128)
		{
			offset = time_zone_offset_summer;
		}
	}
	else
	{
		rtc_t dt;
		
		get_date_time_utc(&dt);
		
		switch(dt.month)
		{
			case 1:case 2:case 11:case 12:
			{
				offset = time_zone_offset_winter;
				
				break;
			}
			
			case 4:case 5:case 6:case 7:case 8:case 9:
			{
				offset = time_zone_offset_summer;
				
				break;
			}
			
			case 3:
			{
				if(dt.day < 25)
				{
					offset = time_zone_offset_winter;
				}
				else
				{
					int weekday = get_weekday(&dt);
					
					if(weekday == 7)
					{
						if(dt.hour < 1)
						{
							offset = time_zone_offset_winter;
						}
						else
						{
							offset = time_zone_offset_summer;
						}
					}
					else
					{
						if((dt.day - weekday) <= 26)
						{
							offset = time_zone_offset_winter;
						}
						else
						{
							offset = time_zone_offset_summer;
						}
					}
				}
				
				break;
			}
			
			case 10:
			{
				if(dt.day < 25)
				{
					offset = time_zone_offset_summer;
				}
				else
				{
					int weekday = get_weekday(&dt);
					
					if(weekday == 7)
					{
						if(dt.hour < 1)
						{
							offset = time_zone_offset_summer;
						}
						else
						{
							offset = time_zone_offset_winter;
						}
					}
					else
					{
						if((dt.day - weekday) <= 26)
						{
							offset = time_zone_offset_summer;
						}
						else
						{
							offset = time_zone_offset_winter;
						}
					}
				}
				
				break;
			}
		}
	}
	
	return offset;
}




void set_date_time_local(rtc_t* dt)
{
	set_unix_time_local(convert_date_time_to_unix_time(dt));
}


void get_date_time_local(rtc_t* dt)
{
	convert_unix_time_to_date_time(get_unix_time_local(), dt);
	
	dt->day_week = get_day_week(dt->year, dt->month, dt->day);
} 




uint32_t get_unix_time_utc(void)
{
	rtc_t dt;
	
	get_date_time_utc(&dt);
	
	return convert_date_time_to_unix_time(&dt);
}



void set_unix_time_utc(uint32_t ut)
{
	rtc_t dt;
	
	convert_unix_time_to_date_time(ut, &dt);
	
	set_date_time_utc(&dt);
}



int get_weekday(rtc_t* dt)
{
	uint32_t ut = convert_date_time_to_unix_time(dt);
	
	return (((ut / (3600 * 24)) + 3) % 7) + 1;
}





uint32_t get_unix_time_local(void)
{
	return get_unix_time_utc() + get_offset() * 15 * 60 + clock_synchronisation_offset;
}







void set_unix_time_local(uint32_t ut)
{
	set_unix_time_utc(ut - get_offset() * 15 * 60 - clock_synchronisation_offset);
}








int16_t get_clock_synchronisation_offset(void)
{
	return clock_synchronisation_offset;
}




void set_clock_synchronisation_offset(int16_t cso)
{
	clock_synchronisation_offset = cso;
	
	eeprom_write_parameter((uint8_t*)&clock_synchronisation_offset, CLOCK_SYNCHRONISATION_OFFSET);
}



int8_t get_time_zone_offset(void)
{
	return time_zone_offset;
}




void set_time_zone_offset(int8_t tzo)
{
	time_zone_offset = tzo;
	
	eeprom_write_parameter((uint8_t*)&time_zone_offset, TIME_ZONE_OFFSET);
}



int8_t get_time_zone_offset_winter(void)
{
	return time_zone_offset_winter;
}




void set_time_zone_offset_winter(int8_t tzow)
{
	time_zone_offset_winter = tzow;
	
	eeprom_write_parameter((uint8_t*)&time_zone_offset_winter, TIME_ZONE_OFFSET_WINTER);
}




int8_t get_time_zone_offset_summer(void)
{
	return time_zone_offset_summer;
}




void set_time_zone_offset_summer(int8_t tzos)
{
	time_zone_offset_summer = tzos;
	
	eeprom_write_parameter((uint8_t*)&time_zone_offset_summer, TIME_ZONE_OFFSET_SUMMER);
}




int8_t get_clock_synchronisation_from_gsm(void)
{
	return clock_synchronisation_from_gsm;
}




bool set_clock_synchronisation_from_gsm(int8_t csfg)
{
	if((csfg != 0) && (csfg != 1))
	{
		return false;
	}
	
	clock_synchronisation_from_gsm = csfg;
	
	eeprom_write_parameter((uint8_t*)&time_zone_offset_summer, CLOCK_SYNCHRONISATION_FROM_GSM);
	
	return true;
}



uint8_t get_time_zone_auto_offset(void)
{
	return time_zone_auto_offset;
}



bool set_time_zone_auto_offset(uint8_t tzao)
{
	if(time_zone_auto_offset != tzao)
	{
		if((tzao != 0) && (tzao != 1))
		{
			return false;
		}
	
		time_zone_auto_offset = tzao;
		
		eeprom_write_parameter((uint8_t*)&time_zone_auto_offset, TIME_ZONE_AUTO_OFFSET);
	}
	
	return true;
}



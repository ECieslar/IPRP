
#include "schedule.h"

#include "timers.h"

#include "date_time.h"

#include "smart_gas.h"

#include <string.h>


static const int max_schedules = 16;

//maksymalna dlugosc komendy zgodnie z opisem harmonogramów protokolu SMART-GAS
static const int max_command_length = 25;

//struktura opisujaca zdarzenie
struct schedule
{
	uint8_t year;
	uint8_t month;
	uint8_t day_of_month;
	uint8_t day_of_week;
	uint8_t hour;
	uint8_t minute;
	uint8_t local_time;
	uint8_t command[max_command_length];//komenda w formacie WO_DATA do wykonania
	uint8_t command_length;
} schedules[max_schedules];



void init_schedule(void)
{
}


void prepare_schedule(void)
{
	//dla wszystkich niezainicjalizowanych harmonogramów
	for(int i = 0; i < max_schedules; i++)
	{
		//ustaw wartosci harmonogramu w ten sposób, aby nie bylo mozliwe wykonanie harmonogramu
		schedules[i].year = 0xff;
		schedules[i].month = 0xff;
		schedules[i].day_of_month = 0xff;
		schedules[i].day_of_week = 0xff;
		schedules[i].hour = 0xff;
		schedules[i].minute = 0xff;
		schedules[i].local_time = 0;
		schedules[i].command_length = 0;
	}
}


static void send_wo_data(int idx)
{
	uint8_t response_data[266];
	
	//wywolaj harmonogram
	smart_gas_object(schedules[idx].command, schedules[idx].command_length, response_data, 266, sgos_schedule);
}


void check_schedule(void)
{
	rtc_t dt_utc;
	
	rtc_t dt_local;
	
	get_date_time_utc(&dt_utc);
	
	get_date_time_local(&dt_local);
	
	rtc_t* dt;
	
	for(int i = 0; i < max_schedules; i++)
	{
		//sprawdz, czy wartosci parametrów harmonogramu umozliwiaja jego wykonanie
		if((schedules[i].year == 0xff) && (schedules[i].month == 0xff) && (schedules[i].day_of_month == 0xff) && 
			 (schedules[i].day_of_week == 0xff) && (schedules[i].hour == 0xff) && (schedules[i].minute == 0xff) &&
		   (schedules[i].local_time == 0) && (schedules[i].command_length == 0))
		{
			continue;
		}
		
		//okresl odpowiedni czas harmonogramu
		if(schedules[i].local_time == 0)
		{
			dt = &dt_utc;
		}
		else
		{
			dt = &dt_local;
		}
		
		//sprawdz, czy czas harmonogramu jest zbiezny z aktualnym czasem
		if((schedules[i].year != (dt->year - 2000)) && (schedules[i].year != 0xff))
		{
			continue;
		}
		
		if((schedules[i].month != dt->month) && (schedules[i].month != 0xff))
		{
			continue;
		}
		
		if((schedules[i].day_of_month != dt->day) && (schedules[i].day_of_month != 0xff))
		{
			continue;
		}
		
		if((schedules[i].day_of_week != (1 << (dt->day_week % 7))) && (schedules[i].day_of_week != 0xff))
		{
			continue;
		}
		
		if((schedules[i].hour != dt->day) && (schedules[i].hour != 0xff))
		{
			continue;
		}
		
		if((schedules[i].minute != dt->day) && (schedules[i].minute != 0xff))
		{
			continue;
		}
		
		//wykonaj harmonogram, bo aktualny czas zgadza sie z czasem harmonogramu
		send_wo_data(i);
	}
}



bool add_schedule(uint8_t* tab, uint8_t length)
{
	if(length < 6)
	{
		return false;
	}
	
	uint16_t start_index = 0xffff;
	
	uint16_t stop_index = 0xffff;
	
	int idx = 0;
	
	uint8_t length1 = tab[idx++];
	
	if(length1 > length)
	{
		return false;
	}
	
	if(tab[idx] == 0)
	{
		start_index = get_from_buffer_uint16_t_littleendian(tab + idx);
		
		idx += 2;
		
		stop_index = get_from_buffer_uint16_t_littleendian(tab + idx);
		
		idx += 2;
		
		if((start_index > max_schedules) || (stop_index > max_schedules))
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	int col_count = 0;
	
	uint16_t columns[10];
	
	if(tab[idx++] == 3)
	{
		col_count = get_from_buffer_uint16_t_littleendian(tab + idx++);
		
		if(col_count > 8)
		{
			return false;
		}
		
		for(int i = 0; i < col_count; i++, idx += 2)
		{
			columns[i] = get_from_buffer_uint16_t_littleendian(tab + idx);
			
			switch(columns[i])
			{
				case sg_schedule_year:
				case sg_schedule_month:
				case sg_schedule_day_of_month:
				case sg_schedule_day_of_week:
				case sg_schedule_hour:
				case sg_schedule_minute:
				case sg_schedule_local_time:
				case sg_schedule_command:
				{
					break;
				}
				
				default:
				{
					return false;
				}
			}
		}
		
		uint16_t length2 = get_from_buffer_uint16_t_littleendian(tab + idx);
		
		if(length1 + length2 + 3 != length)
		{
			return false;
		}
		
		idx += 2;
		
		for(int i = start_index; i <= stop_index; i++)
		{
			for(int j = 0; j < col_count; j++)
			{
				switch(columns[j])
				{
					case sg_schedule_year:
					{
						schedules[i].year = tab[idx++];
						break;
					}
					
					case sg_schedule_month:
					{
						schedules[i].month = tab[idx++];
						break;
					}
						
					case sg_schedule_day_of_month:
					{
						schedules[i].day_of_month = tab[idx++];
						break;
					}
						
					case sg_schedule_day_of_week:
					{
						schedules[i].day_of_week = tab[idx++];
						break;
					}
						
					case sg_schedule_hour:
					{
						schedules[i].hour = tab[idx++];
						break;
					}
						
					case sg_schedule_minute:
					{
						schedules[i].minute = tab[idx++];
						break;
					}
						
					case sg_schedule_local_time:
					{
						schedules[i].local_time = tab[idx++];
						break;
					}
						
					case sg_schedule_command:
					{
						schedules[i].command_length = tab[idx++];
						
						if(schedules[i].command_length > 25)
						{
							schedules[i].command_length = 25;
						}
						
						memcpy(schedules[i].command, tab + idx, schedules[i].command_length);
						
						idx += schedules[i].command_length;
						
						break;
					}
				}
			}
		}
	}
	
	
	
	return true;
}


bool get_schedule(uint8_t* input, uint8_t input_length, uint8_t* output, int* output_length)
{
	if(input_length < 6)
	{
		return false;
	}
	
	uint16_t start_index = 0xffff;
	
	uint16_t stop_index = 0xffff;
	
	int idx = 0;
	
	uint8_t length1 = input[idx++];
	
	if(length1 > input_length)
	{
		return false;
	}
	
	if(input[idx] == 0)
	{
		start_index = get_from_buffer_uint16_t_littleendian(input + idx);
		
		idx += 2;
		
		stop_index = get_from_buffer_uint16_t_littleendian(input + idx);
		
		idx += 2;
		
		if((start_index > max_schedules) || (stop_index > max_schedules))
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	int col_count = 0;
	
	uint16_t columns[10];
	
	if(input[idx++] == 3)
	{
		col_count = get_from_buffer_uint16_t_littleendian(input + idx++);
		
		if(col_count > 8)
		{
			return false;
		}
		
		for(int i = 0; i < col_count; i++, idx += 2)
		{
			columns[i] = get_from_buffer_uint16_t_littleendian(input + idx);
			
			switch(columns[i])
			{
				case sg_schedule_year:
				case sg_schedule_month:
				case sg_schedule_day_of_month:
				case sg_schedule_day_of_week:
				case sg_schedule_hour:
				case sg_schedule_minute:
				case sg_schedule_local_time:
				case sg_schedule_command:
				{
					break;
				}
				
				default:
				{
					return false;
				}
			}
		}
		
		memcpy(output, input, idx);
			
		for(int i = start_index; i <= stop_index; i++)
		{
			for(int j = 0; j < col_count; j++)
			{
				switch(columns[j])
				{
					case sg_schedule_year:
					{
						output[idx++] = schedules[i].year;
						break;
					}
					
					case sg_schedule_month:
					{
						output[idx++] = schedules[i].month;
						break;
					}
						
					case sg_schedule_day_of_month:
					{
						output[idx++] = schedules[i].day_of_month;
						break;
					}
						
					case sg_schedule_day_of_week:
					{
						output[idx++] = schedules[i].day_of_week;
						break;
					}
						
					case sg_schedule_hour:
					{
						output[idx++] = schedules[i].hour;
						break;
					}
						
					case sg_schedule_minute:
					{
						output[idx++] = schedules[i].minute;
						break;
					}
						
					case sg_schedule_local_time:
					{
						output[idx++] = schedules[i].local_time;
						break;
					}
						
					case sg_schedule_command:
					{
						output[idx++] = schedules[i].command_length;
						
						memcpy(output + idx, schedules[i].command, schedules[i].command_length);
						
						idx += schedules[i].command_length;
						
						break;
					}
				}
			}
		}
	}
	
	
	
	return true;
}




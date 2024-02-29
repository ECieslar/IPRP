

#include "bellows.h"

#include "menu.h"

#include <string.h>


static int64_t ticks_counter;

static const int32_t c_update_volume_value = 225;
//static const int32_t c_update_volume_value = 300;

static uint32_t hub_seconds;


void init_bellows(void)
{
	ticks_counter = 0;
	
	hub_seconds = 0;
}


int32_t get_bellows_ticks_counter(void)
{
	return ticks_counter;
}



void hallotron_tick(uint8_t* data)
{
	uint32_t tmp_hub_seconds;
	
	memcpy((uint8_t*)&tmp_hub_seconds, data, 4);
	
	int64_t tmp_ticks_counter = 0;
	
	for(int i = 0, idx = 9; i < 6; i++, idx--)
	{
		tmp_ticks_counter <<= 8;
		
		tmp_ticks_counter |= data[idx];
	}
	
	if(tmp_ticks_counter & 0x800000000000)
	{
		tmp_ticks_counter |= 0xffff000000000000;
	}
	
	if((ticks_counter == 0) && (hub_seconds == 0))
	{
		ticks_counter = tmp_ticks_counter;
	}
	else
	{
		if(tmp_hub_seconds < hub_seconds)
		{
			update_volume(tmp_ticks_counter * c_update_volume_value);
			
			ticks_counter = tmp_ticks_counter;
		}
		else
		{
			if(tmp_ticks_counter > ticks_counter)
			{
				update_volume((tmp_ticks_counter - ticks_counter) * c_update_volume_value);
				
				ticks_counter = tmp_ticks_counter;
			}
		}
	}
	
	hub_seconds = tmp_hub_seconds;
	
	/*
	
	static uint32_t idx = (uint32_t)-10;
	
	uint32_t sent_idx;
	
	memcpy((uint8_t*)&sent_idx, data + 1, 4);
	
	if(*data == 1)
	{
		ticks_counter++;
		
		if(sent_idx == idx)
		{
			update_volume(c_update_volume_value);
		}
		else
		{
			if(sent_idx < idx)
			{
				update_volume(c_update_volume_value);
			}
			else
			{
				if(idx - sent_idx < 10)
				{
					update_volume((idx - sent_idx) * c_update_volume_value);
				}
				else
				{
					if(idx - sent_idx < 100)
					{
						update_volume(10 * c_update_volume_value);
					}
				}
			}
		}
	}
	else if(*data == 255)
	{
		//ticks_counter--;
		
		//update_volume(-c_update_volume_value);
	}
	
	idx = sent_idx + 1;
	*/
	
	menu_event(menu_event_bellows_tick, NULL);
}





#include "timers.h"

#include "driver\driver_timers.h"

#include "time.h"




#define MAX_TIME_EVENTS 34



struct s_time_event
{
	int max_interval;
	int current_tick;
	bool stopped;
	bool running;
	void (*func)(void);
};

static struct s_time_event time_events[MAX_TIME_EVENTS];


static int time_events_count = 0;



void init_timers(void)
{
	init_driver_timers();
	
	for(int i = 0; i < MAX_TIME_EVENTS; i++)
	{
		struct s_time_event *se_ptr = time_events + i;
		se_ptr->running = false;
	}
}




static void check_time_events(void)
{
	int i;
	for(i = 0; i < time_events_count; i++)
	{
		struct s_time_event *se_ptr = time_events + i;

		//cli();
		if(!se_ptr->stopped)
		{
			se_ptr->current_tick--;
			if(se_ptr->current_tick == 0)
			{
				//sei();
				se_ptr->current_tick = se_ptr->max_interval;
				
				se_ptr->func();
			}
			else
			{
				//sei();
			}
		}
		else
		{
			//sei();
		}
	}
}




void check_timers(void)
{
	if(get_timer_tick())
	{
		clr_timer_tick();
		
		check_time_events();
	}
}


int register_time_event(void (*func)(void), int interval, bool stopped)
{
	if(time_events_count < MAX_TIME_EVENTS)
	{
		struct s_time_event *se_ptr = time_events + time_events_count;
		se_ptr->current_tick = interval;
		se_ptr->max_interval = interval;
		se_ptr->stopped = stopped;
		se_ptr->func = func;

		int idx = time_events_count;

		//cli();
		time_events_count++;
		//sei();

		return idx;
	}
	else
	{
		//set_system_error(system_error_timer_register);
	}

	return -1;
}




void stop_time_event(int nr)
{
	if((nr >= 0) && (nr < MAX_TIME_EVENTS))
	{
		struct s_time_event *se_ptr = time_events + nr;
		se_ptr->stopped = true;
	}
	else
	{
		//set_system_error(system_error_timer_stop);
	}
}



void start_time_event(int nr)
{
	if((nr >= 0) && (nr < MAX_TIME_EVENTS))
	{
		struct s_time_event *se_ptr = time_events + nr;
		//se_ptr->current_tick = 0;
		se_ptr->current_tick = se_ptr->max_interval;
		se_ptr->stopped = false;
	}
	else
	{
		//set_system_error(system_error_timer_start);
	}
}



void new_interval_time_event(int nr, int interval)
{
	if((nr >= 0) && (nr < MAX_TIME_EVENTS))
	{
		struct s_time_event *se_ptr = time_events + nr;
		se_ptr->max_interval = interval;
		//se_ptr->current_tick = 0;
		se_ptr->current_tick = interval;
	}
	else
	{
		//set_system_error(system_error_timer_interval);
	}
}



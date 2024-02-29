
#ifndef TIMERS_H_
#define TIMERS_H_


#include "stdbool.h"

void init_timers(void);

void check_timers(void);

int register_time_event(void (*func)(void), int interval, bool stopped);

void stop_time_event(int nr);

void start_time_event(int nr);

void new_interval_time_event(int nr, int interval);

#endif /*TIMERS_H_*/

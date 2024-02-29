

#include "status.h"

#include "eeprom.h"



static uint32_t status;

static uint32_t status_periodic_latched;

static uint32_t status_monthly_latched;

static uint32_t status_up_event_match;

static uint32_t status_down_event_match;




void init_status(void)
{
	status = 0;
	
	status_periodic_latched = 0;
	
	status_monthly_latched = 0;
	
	status_up_event_match = 0;
	
	status_down_event_match = 0;
}



uint32_t get_status(void)
{
	return status;
}



void set_status_flag(enum status_flags flag)
{
	status |= (1 << flag);
}



void clr_status_flag(enum status_flags flag)
{
	status &= ~(1 << flag);
}



uint32_t get_status_periodic_latched(void)
{
	return status_periodic_latched;
}



void set_status_periodic_latched(uint32_t spl)
{
	status_periodic_latched = spl;
}


void update_status_periodic_latched(void)
{
	status_periodic_latched = status;
}


uint32_t get_status_monthly_latched(void)
{
	return status_periodic_latched;
}



void set_status_monthly_latched(uint32_t sml)
{
	status_monthly_latched = sml;
}


void update_status_monthly_latched(void)
{
	status_monthly_latched = status;
}



uint32_t get_status_up_event_match(void)
{
	return status_up_event_match;
}



void set_status_up_event_match(uint32_t suem)
{
	status_up_event_match = suem;
}



uint32_t get_status_down_event_match(void)
{
	return status_down_event_match;
}



void set_status_down_event_match(uint32_t sdem)
{
	status_down_event_match = sdem;
}



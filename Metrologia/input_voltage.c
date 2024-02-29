
#include "input_voltage.h"

#include "driver/driver_adc.h"

#include "driver/driver_input_voltage.h"

#include "timers.h"


#define INPUT_VOLTAGE_TIME 10


static uint16_t vref;

static bool do_measure;



static void voltage_measure(void)
{
	vref = driver_vref_get();
}



void check_voltage_measure(void)
{
	if(do_measure)
	{
		voltage_measure();
		
		do_measure = false;
	}
}




void init_input_voltage(void)
{
	init_driver_input_voltage();
	
	do_measure = false;
	
	voltage_measure();
}



uint16_t get_input_voltage(void)
{
	return (uint64_t)3000 * (uint64_t)(driver_adc_get_vrefint_cal()) / (uint64_t) vref;
}


void input_voltage_tick(void)
{
	static int tick_counter = 0;
	
	tick_counter++;
	
	if(tick_counter >= INPUT_VOLTAGE_TIME)
	{
		do_measure = true;
		
		tick_counter = 0;
	}
}


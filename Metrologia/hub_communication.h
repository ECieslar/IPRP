
#ifndef HUB_COMMUNICATION_H_
#define HUB_COMMUNICATION_H_



#include "extras.h"


#include "stdbool.h"


enum hub_communication_functions
{
	hcf_ultrasonic_meter										=	0x01,
	hcf_pressure_meter											= 0x02,
	hcf_valve_current												= 0x03,
	hcf_valve_hallotron_tick								= 0x04,
	hcf_calorimeter_holding_registers				= 0x05,
	hcf_calorimeter_input_registers					= 0x06,
	hcf_calorimeter_caloric_value						= 0x07,
	
	
	hcf_valve_turn_left											= 0x81,
	hcf_valve_turn_right										= 0x82,
	hcf_valve_turn_off											= 0x83,
	hcf_valve_invoke_get_current						= 0x84,
	hcf_calorimeter_read_holding_registers	= 0x85,
	hcf_calorimeter_read_input_registers		= 0x86,
	hcf_calorimeter_write_holding_registers	= 0x87,
	hcf_calorimeter_get_caloric_value				= 0x88,
};


void init_hub_communication(void);

void check_hub_communication(void);

void send_hub_data(enum hub_communication_functions function, uint8_t* data, uint16_t length);


#endif /*HUB_COMMUNICATION_H_*/

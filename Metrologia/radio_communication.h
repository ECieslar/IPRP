
#ifndef RADIO_COMMUNICATION_H_
#define RADIO_COMMUNICATION_H_



#include "extras.h"

#include "stdbool.h"

#include "smart_gas.h"


void init_radio_communication(void);

void check_radio_communication(void);

uint32_t get_radio_send_frames_counter(void);

uint32_t get_radio_receive_frames_counter(void);

void radio_communication_send_smart_gas(enum smart_gas_object_ids obj_id, uint8_t* data, uint16_t length);




enum radio_communication_functions
{
	rcf_server				=	0x01,
	rcf_baudrate			= 0x02,
	rcf_repeat_demand	= 0x03,
	rcf_radio					= 0x04,
};


#endif /*HUB_COMMUNICATION_H_*/

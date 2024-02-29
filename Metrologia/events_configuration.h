
#ifndef EVENTS_CONFIGURATION_H_
#define EVENTS_CONFIGURATION_H_

#include "inttypes.h"

#include "stdbool.h"



void init_events_configuration(void);

void prepare_events_configuration(void);

uint8_t get_event_link(void);

bool set_event_link(uint8_t el);

uint8_t get_event_phone(uint8_t* ep);

bool set_event_phone(uint8_t length, uint8_t* ep);

uint32_t get_event_ip(void);

void set_event_ip(uint32_t ei);

uint16_t get_event_udp_port(void);

void set_event_udp_port(uint16_t eup);

uint16_t get_event_tcp_port(void);

void set_event_tcp_port(uint16_t etp);




#endif /*EVENTS_CONFIGURATION_H_*/

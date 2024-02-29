
#include "events_configuration.h"

#include "eeprom.h"

#include <string.h>


static uint8_t event_link;

static uint8_t event_phone[8];

static uint32_t event_ip;

static uint16_t event_udp_port;

static uint16_t event_tcp_port;



static void init_event_link(void)
{
	eeprom_read_parameter(&event_link, EVENT_LINK);
	
	switch(event_link)
	{
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		{
			break;
		}
		
		default:
		{
			set_event_link(0x00);
		}
	}
}



static void prepare_event_link(void)
{
	set_event_link(0x00);
}



static void init_event_phone(void)
{
	eeprom_read_parameter(event_phone, EVENT_PHONE);
}



static void prepare_event_phone(void)
{
	uint8_t phone[8] = {0x48, 0x60, 0x12, 0x34, 0x56, 0x7F, 0xFF, 0xFF};
	
	set_event_phone(8, phone);
}



static void init_event_ip(void)
{
	eeprom_read_parameter((uint8_t*)&event_ip, EVENT_IP);
}



static void prepare_event_ip(void)
{
	uint32_t ip = 0x7F000001;
	
	set_event_ip(ip);
}



static void init_event_udp_port(void)
{
	eeprom_read_parameter((uint8_t*)&event_udp_port, EVENT_UDP_PORT);
}



static void prepare_event_udp_port(void)
{
	set_event_udp_port(1234);
}



static void init_event_tcp_port(void)
{
	eeprom_read_parameter((uint8_t*)&event_tcp_port, EVENT_TCP_PORT);
}



static void prepare_event_tcp_port(void)
{
	set_event_udp_port(5678);
}


void init_events_configuration(void)
{
	init_eeprom();
	
	init_event_link();
	
	init_event_phone();
	
	init_event_ip();
	
	init_event_udp_port();
	
	init_event_tcp_port();
}



void prepare_events_configuration(void)
{
	prepare_event_link();
	
	prepare_event_phone();
	
	prepare_event_ip();
	
	prepare_event_udp_port();
	
	prepare_event_tcp_port();
}


uint8_t get_event_link(void)
{
	return event_link;
}


bool set_event_link(uint8_t el)
{
	switch(el)
	{
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		{
			break;
		}
		
		default:
		{
			return false;
		}
	}
	
	event_link = el;
	
	eeprom_write_parameter(&event_link, EVENT_LINK);
	
	return true;
}



uint8_t get_event_phone(uint8_t* ep)
{
	memcpy(ep, event_phone, 8);
	
	return 8;
}



bool set_event_phone(uint8_t length, uint8_t* ep)
{
	if(length != 8)
	{
		return false;
	}
	
	memcpy(event_phone, ep, 8);
	
	eeprom_write_parameter(event_phone, EVENT_PHONE);
	
	return true;
}



uint32_t get_event_ip(void)
{
	return event_ip;
}


void set_event_ip(uint32_t ei)
{
	event_ip = ei;
	
	eeprom_write_parameter((uint8_t*)&event_ip, EVENT_IP);
}


uint16_t get_event_udp_port(void)
{
	return event_udp_port;
}


void set_event_udp_port(uint16_t eup)
{
	event_udp_port = eup;
	
	eeprom_write_parameter((uint8_t*)&event_udp_port, EVENT_UDP_PORT);
}


uint16_t get_event_tcp_port(void)
{
	return event_tcp_port;
}


void set_event_tcp_port(uint16_t etp)
{
	event_tcp_port = etp;
	
	eeprom_write_parameter((uint8_t*)&event_tcp_port, EVENT_TCP_PORT);
}



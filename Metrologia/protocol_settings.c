
#include "protocol_settings.h"

#include "eeprom.h"

#include <string.h>



static uint8_t wdp_arp;

static uint8_t wdp_at;

static uint8_t wdp_mr;

static uint8_t wpp_event_send_sn;

static uint32_t wpp_iv_in;

static uint8_t wpp_key[17];

static uint8_t wpp_cryptography_obligatory;

static uint8_t wpp_access_control_obligatory;

static uint8_t sms_phone_numbers_restriction;

static uint8_t permitted_phone_number[8];




static void init_wdp_arp(void)
{
	eeprom_read_parameter((uint8_t*)&wdp_arp, WDP_ARP);
}



static void prepare_wdp_arp(void)
{
	set_wdp_arp(0);
}



static void init_wdp_at(void)
{
	eeprom_read_parameter((uint8_t*)&wdp_at, WDP_AT);
}



static void prepare_wdp_at(void)
{
	set_wdp_at(0);
}



static void init_wdp_mr(void)
{
	eeprom_read_parameter((uint8_t*)&wdp_mr, WDP_MR);
}



static void prepare_wdp_mr(void)
{
	set_wdp_mr(1);
}



static void init_wpp_event_send_sn(void)
{
	eeprom_read_parameter((uint8_t*)&wpp_event_send_sn, WPP_EVENT_SEND_SN);
	
	if((wpp_event_send_sn != 0) && (wpp_event_send_sn != 1))
	{
		set_wpp_event_send_sn(1);
	}
}



static void prepare_wpp_event_send_sn(void)
{
	set_wpp_event_send_sn(1);
}



static void init_wpp_iv_in(void)
{
	eeprom_read_parameter((uint8_t*)&wpp_iv_in, WPP_IV_IN);
}



static void prepare_wpp_iv_in(void)
{
	set_wpp_iv_in(0);
}



static void init_wpp_key(void)
{
	eeprom_read_parameter(wpp_key, WPP_KEY);
	wpp_key[16] = 0;
}



static void prepare_wpp_key(void)
{
	uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	
	set_wpp_key(16, key);
}



static void init_wpp_cryptography_obligatory(void)
{
	eeprom_read_parameter((uint8_t*)&wpp_cryptography_obligatory, WPP_CRYPTOGRAPHY_OBLIGATORY);
	
	if((wpp_cryptography_obligatory != 0) && (wpp_cryptography_obligatory != 1))
	{
		set_wpp_cryptography_obligatory(0);
	}
}



static void prepare_wpp_cryptography_obligatory(void)
{
	set_wpp_cryptography_obligatory(0);
}



static void init_wpp_access_control_obligatory(void)
{
	eeprom_read_parameter((uint8_t*)&wpp_access_control_obligatory, WPP_ACCESS_CONTROL_OBLIGATORY);
	
	if((wpp_access_control_obligatory != 0) && (wpp_access_control_obligatory != 1))
	{
		set_wpp_access_control_obligatory(0);
	}
}



static void prepare_wpp_access_control_obligatory(void)
{
	set_wpp_access_control_obligatory(0);
}



static void init_sms_phone_numbers_restriction(void)
{
	eeprom_read_parameter((uint8_t*)&sms_phone_numbers_restriction, SMS_PHONE_NUMBERS_RESTRICTION);
	
	if((sms_phone_numbers_restriction != 0) && (sms_phone_numbers_restriction != 1))
	{
		set_sms_phone_numbers_restriction(0);
	}
}



static void prepare_sms_phone_numbers_restriction(void)
{
	set_sms_phone_numbers_restriction(0);
}



static void init_permitted_phone_number(void)
{
	eeprom_read_parameter(permitted_phone_number, PERMITTED_PHONE_NUMBER);
}



static void prepare_permitted_phone_number(void)
{
	uint8_t phone[8] = {0x48, 0x60, 0x12, 0x34, 0x56, 0x7F, 0xFF, 0xFF};
	
	set_permitted_phone_number(8, phone);
}




void init_protocol_settings(void)
{
	init_eeprom();
	
	init_wdp_arp();
	
	init_wdp_at();
	
	init_wdp_mr();
	
	init_wpp_event_send_sn();
	
	init_wpp_iv_in();
	
	init_wpp_key();
	
	init_wpp_cryptography_obligatory();
	
	init_wpp_access_control_obligatory();
	
	init_sms_phone_numbers_restriction();
	
	init_permitted_phone_number();
}


void prepare_protocol_settings(void)
{
	prepare_wdp_arp();
	
	prepare_wdp_at();
	
	prepare_wdp_mr();
	
	prepare_wpp_event_send_sn();
	
	prepare_wpp_iv_in();
	
	prepare_wpp_key();
	
	prepare_wpp_cryptography_obligatory();
	
	prepare_wpp_access_control_obligatory();
	
	prepare_sms_phone_numbers_restriction();
	
	prepare_permitted_phone_number();
}



uint8_t get_wdp_arp(void)
{
	return wdp_arp;
}



void set_wdp_arp(uint8_t wa)
{
	if(wdp_arp != wa)
	{
		wdp_arp = wa;
		
		eeprom_write_parameter((uint8_t*)&wdp_arp, WDP_ARP);
	}
}



uint8_t get_wdp_at(void)
{
	return wdp_at;
}



void set_wdp_at(uint8_t wa)
{
	if(wdp_at != wa)
	{
		wdp_at = wa;
		
		eeprom_write_parameter((uint8_t*)&wdp_at, WDP_AT);
	}
}



uint8_t get_wdp_mr(void)
{
	return wdp_mr;
}



void set_wdp_mr(uint8_t wm)
{
	if(wdp_mr != wm)
	{
		wdp_mr = wm;
		
		eeprom_write_parameter((uint8_t*)&wdp_mr, WDP_MR);
	}
}



uint8_t get_wpp_event_send_sn(void)
{
	return wpp_event_send_sn;
}



bool set_wpp_event_send_sn(uint8_t wess)
{
	if(wpp_event_send_sn != wess)
	{
		if((wess != 0) && (wess != 1))
		{
			return false;
		}
	
		wpp_event_send_sn = wess;
		
		eeprom_write_parameter((uint8_t*)&wpp_event_send_sn, WPP_EVENT_SEND_SN);
	}
	
	return true;
}



uint32_t get_wpp_iv_in(void)
{
	return wpp_iv_in;
}



void set_wpp_iv_in(uint32_t wii)
{
	if(wpp_iv_in != wii)
	{
		wpp_iv_in = wii;
		
		eeprom_write_parameter((uint8_t*)&wpp_iv_in, WPP_IV_IN);
	}
}



uint8_t get_wpp_key(uint8_t* wk)
{
	uint8_t length = strlen((char*)wpp_key);
	
	memcpy(wk, wpp_key, length);
	
	return length;
}



bool set_wpp_key(uint8_t length, uint8_t* wk)
{
	if(length >= EL(WPP_KEY))
	{
		return false;
	}
	
	memcpy(wpp_key, wk, length);
	
	wpp_key[length] = 0;
	
	eeprom_write(wpp_key, EA(WPP_KEY), length + 1);
	
	return true;
}



uint8_t get_wpp_cryptography_obligatory(void)
{
	return wpp_cryptography_obligatory;
}



bool set_wpp_cryptography_obligatory(uint8_t wco)
{
	if((wco == 0) || (wco == 1))
	{
		if(wpp_cryptography_obligatory != wco)
		{
			wpp_cryptography_obligatory = wco;
			
			eeprom_write_parameter((uint8_t*)&wpp_cryptography_obligatory, WPP_CRYPTOGRAPHY_OBLIGATORY);
		}
	}
	
	return false;
}



uint8_t get_wpp_access_control_obligatory(void)
{
	return wpp_access_control_obligatory;
}



bool set_wpp_access_control_obligatory(uint8_t waco)
{
	if((waco == 0) || (waco == 1))
	{
		if(wpp_access_control_obligatory != waco)
		{
			wpp_access_control_obligatory = waco;
			
			eeprom_write_parameter((uint8_t*)&wpp_access_control_obligatory, WPP_ACCESS_CONTROL_OBLIGATORY);
		}
	}
	
	return false;
}



uint8_t get_sms_phone_numbers_restriction(void)
{
	return sms_phone_numbers_restriction;
}



bool set_sms_phone_numbers_restriction(uint8_t spnr)
{
	if((spnr == 0) || (spnr == 1))
	{
		if(sms_phone_numbers_restriction != spnr)
		{
			sms_phone_numbers_restriction = spnr;
			
			eeprom_write_parameter((uint8_t*)&sms_phone_numbers_restriction, SMS_PHONE_NUMBERS_RESTRICTION);
		}
	}
	
	return false;
}



uint8_t get_permitted_phone_number(uint8_t* ppn)
{
	memcpy(ppn, permitted_phone_number, 8);
	
	return 8;
}



bool set_permitted_phone_number(uint8_t length, uint8_t* ppn)
{
	if(length != 8)
	{
		return false;
	}
	
	memcpy(permitted_phone_number, ppn, 8);
	
	eeprom_write_parameter(permitted_phone_number, PERMITTED_PHONE_NUMBER);
	
	return true;
}



#ifndef PROTOCOL_SETTINGS_H_
#define PROTOCOL_SETTINGS_H_



#include "extras.h"


#include "stdbool.h"



void init_protocol_settings(void);

void prepare_protocol_settings(void);

uint8_t get_wdp_arp(void);

void set_wdp_arp(uint8_t wa);

uint8_t get_wdp_at(void);

void set_wdp_at(uint8_t wa);

uint8_t get_wdp_mr(void);

void set_wdp_mr(uint8_t wm);

uint8_t get_wpp_event_send_sn(void);

bool set_wpp_event_send_sn(uint8_t wess);

uint32_t get_wpp_iv_in(void);

void set_wpp_iv_in(uint32_t wii);

uint8_t get_wpp_key(uint8_t* wk);

bool set_wpp_key(uint8_t length, uint8_t* wk);

uint8_t get_wpp_cryptography_obligatory(void);

bool set_wpp_cryptography_obligatory(uint8_t wco);

uint8_t get_wpp_access_control_obligatory(void);

bool set_wpp_access_control_obligatory(uint8_t waco);

uint8_t get_sms_phone_numbers_restriction(void);

bool set_sms_phone_numbers_restriction(uint8_t spnr);

uint8_t get_permitted_phone_number(uint8_t* ppn);

bool set_permitted_phone_number(uint8_t length, uint8_t* ppn);






#endif /*PROTOCOL_SETTINGS_H_*/

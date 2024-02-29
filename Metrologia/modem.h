
#ifndef MODEM_H_
#define MODEM_H_

#include "inttypes.h"

#include "stdbool.h"



void init_modem(void);

void prepare_modem(void);

uint8_t get_sim_pin(uint8_t* sp);

bool set_sim_pin(uint8_t length, uint8_t* sp);

uint8_t get_sim_iccid(uint8_t* si);

bool set_sim_iccid(uint8_t length, uint8_t* si);

uint8_t get_sim_phone(uint8_t* sp);

bool set_sim_phone(uint8_t length, uint8_t* sp);

uint32_t get_sim_ip(void);

void set_sim_ip(uint32_t si);

uint8_t get_sim_smsc(uint8_t* ss);

bool set_sim_smsc(uint8_t length, uint8_t* ss);

uint8_t get_sim_sms_validity(void);

bool set_sim_sms_validity(uint8_t sv);

uint8_t get_gprs_apn(uint8_t* ga);

bool set_gprs_apn(uint8_t length, uint8_t* ga);

uint8_t get_gprs_user(uint8_t* gu);

bool set_gprs_user(uint8_t length, uint8_t* gu);

uint8_t get_gprs_password(uint8_t* gp);

bool set_gprs_password(uint8_t length, uint8_t* gp);

uint8_t get_gsm_quality(void);

bool set_gsm_quality(uint8_t gq);

uint8_t get_gsm_network_code(uint8_t* gnc);

bool set_gsm_network_code(uint8_t length, uint8_t* gnc);

uint32_t get_gsm_bts_lac(void);

void set_gsm_bts_lac(uint32_t gbl);

uint32_t get_gsm_bts_cell_id(void);

void set_gsm_bts_cell_id(uint32_t gbci);

uint8_t get_modem_state(void);

bool set_modem_state(uint8_t ms);

uint16_t get_modem_sms_received(void);

void set_modem_sms_received(uint16_t msr);

uint16_t get_modem_sms_sent(void);

void set_modem_sms_sent(uint16_t mss);

uint16_t get_modem_gprs_packets_received(void);

void set_modem_gprs_packets_received(uint16_t mgpr);

uint16_t get_modem_gprs_packets_sent(void);

void set_modem_gprs_packets_sent(uint16_t mgps);

uint16_t get_modem_wakeup_count(void);

void set_modem_wakeup_count(uint16_t mwc);

uint32_t get_modem_wakeup_duration(void);

void set_modem_wakeup_duration(uint32_t mwd);







#endif /*MODEM_H_*/

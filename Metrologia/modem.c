
#include "modem.h"

#include "eeprom.h"

#include <string.h>



static uint8_t sim_pin[9];

static uint8_t sim_iccid[11];

static uint8_t sim_phone[8];

static uint32_t sim_ip;

static uint8_t sim_smsc[17];

static uint8_t sim_sms_validity;

static uint8_t gprs_apn[33];

static uint8_t gprs_user[17];

static uint8_t gprs_password[17];

static uint8_t gsm_quality;

static uint8_t gsm_network_code[9];

static uint32_t gsm_bts_lac;

static uint32_t gsm_bts_cell_id;

static uint8_t modem_state;

static uint8_t modem_sms_received;

static uint8_t modem_sms_sent;

static uint16_t modem_gprs_packets_received;

static uint16_t modem_gprs_packets_sent;

static uint16_t modem_wakeup_count;

static uint32_t modem_wakeup_duration;



static void init_sim_pin(void)
{
	eeprom_read_parameter(sim_pin, SIM_PIN);
	
	if(sim_pin[0] > 8)
	{
		sim_pin[0] = 8;
		
		eeprom_write(sim_pin, EA(SIM_PIN), 1);
	}
}



static void prepare_sim_pin(void)
{
	char pin[] = "1111";
	
	set_sim_pin(strlen(pin), (uint8_t*)pin);
}



static void init_sim_iccid(void)
{
	eeprom_read_parameter(sim_iccid, SIM_ICCID);
}



static void prepare_sim_iccid(void)
{
	uint8_t iccid[1] = {1};
	
	set_sim_pin(1, iccid);
}



static void init_sim_phone(void)
{
	eeprom_read_parameter(sim_phone, SIM_PHONE);
}



static void prepare_sim_phone(void)
{
	uint8_t phone[8] = {0x48, 0x60, 0x12, 0x34, 0x56, 0x7F, 0xFF, 0xFF};
	
	set_sim_phone(8, phone);
}



static void init_sim_ip(void)
{
	eeprom_read_parameter((uint8_t*)&sim_ip, SIM_IP);
}



static void prepare_sim_ip(void)
{
	uint32_t ip = 0x7F000001;
	
	set_sim_ip(ip);
}



static void init_sim_smsc(void)
{
	eeprom_read_parameter(sim_smsc, SIM_SMSC);
	sim_smsc[16] = 0;
}



static void prepare_sim_smsc(void)
{
	char pin[] = "+48601234567";
	
	set_sim_pin(strlen(pin), (uint8_t*)pin);
}



static void init_sim_sms_validity(void)
{
	eeprom_read_parameter(&sim_sms_validity, SIM_SMS_VALIDITY);
	
	switch(sim_sms_validity)
	{
		case 0x0B:case 0x47:case 0x8F:case 0xA7:
		case 0xA8:case 0xA9:case 0xAD:case 0xFF:
		{
			break;
		}
		
		default:
		{
			set_sim_sms_validity(0xFF);
		}
	}
}



static void prepare_sim_sms_validity(void)
{
	set_sim_sms_validity(0xFF);
}



static void init_gprs_apn(void)
{
	eeprom_read_parameter(gprs_apn, GPRS_APN);
	gprs_apn[32] = 0;
}



static void prepare_gprs_apn(void)
{
	char apn[] = "192.168.1.1";
	
	set_gprs_apn(strlen(apn), (uint8_t*)apn);
}



static void init_gprs_user(void)
{
	eeprom_read_parameter(gprs_user, GPRS_USER);
	gprs_user[16] = 0;
}



static void prepare_gprs_user(void)
{
	char user[] = "admin";
	
	set_gprs_user(strlen(user), (uint8_t*)user);
}



static void init_gprs_password(void)
{
	eeprom_read_parameter(gprs_password, GPRS_PASSWORD);
	gprs_password[16] = 0;
}



static void prepare_gprs_password(void)
{
	char password[] = "admin";
	
	set_gprs_password(strlen(password), (uint8_t*)password);
}



static void init_gsm_quality(void)
{
	eeprom_read_parameter(&gsm_quality, GSM_QUALITY);
	
	if(!((gsm_quality <= 31) || (gsm_quality == 99)))
	{
		set_gsm_quality(99);
	}
}



static void prepare_gsm_quality(void)
{
	set_gsm_quality(99);
}



static void init_gsm_network_code(void)
{
	eeprom_read_parameter(gsm_network_code, GSM_NETWORK_CODE);
	gsm_network_code[8] = 0;
}



static void prepare_gsm_network_code(void)
{
	char code[] = "GSM";
	
	set_gsm_network_code(strlen(code), (uint8_t*)code);
}



static void init_gsm_bts_lac(void)
{
	eeprom_read_parameter((uint8_t*)&gsm_bts_lac, GSM_BTS_LAC);
}



static void prepare_gsm_bts_lac(void)
{
	set_gsm_bts_lac(1234);
}



static void init_gsm_bts_cell_id(void)
{
	eeprom_read_parameter((uint8_t*)&gsm_bts_cell_id, GSM_BTS_CELL_ID);
}



static void prepare_gsm_bts_cell_id(void)
{
	set_gsm_bts_cell_id(1234);
}




void init_modem(void)
{
	init_eeprom();
	
	init_sim_pin();
	
	init_sim_iccid();
	
	init_sim_phone();
	
	init_sim_ip();
	
	init_sim_smsc();
	
	init_sim_sms_validity();
	
	init_gprs_apn();
	
	init_gprs_user();
	
	init_gprs_password();
	
	init_gsm_quality();
	
	init_gsm_network_code();
	
	init_gsm_bts_lac();
	
	init_gsm_bts_cell_id();
	
	modem_state = 0;

	modem_sms_received = 0;

	modem_sms_sent = 0;

	modem_gprs_packets_received = 0;

	modem_gprs_packets_sent = 0;

	modem_wakeup_count = 0;

	modem_wakeup_duration = 0;

}


void prepare_modem(void)
{
	prepare_sim_pin();
	
	prepare_sim_iccid();
	
	prepare_sim_phone();
	
	prepare_sim_ip();
	
	prepare_sim_smsc();
	
	prepare_sim_sms_validity();
	
	prepare_gprs_apn();
	
	prepare_gprs_user();
	
	prepare_gprs_password();
	
	prepare_gsm_quality();
	
	prepare_gsm_network_code();
	
	prepare_gsm_bts_lac();
	
	prepare_gsm_bts_cell_id();
}


uint8_t get_sim_pin(uint8_t* sp)
{
	for(int i = 0; i < sim_pin[0]; i++)
	{
		sp[i] = sim_pin[i + 1];
	}
	
	return sim_pin[0];
}



bool set_sim_pin(uint8_t length, uint8_t* sp)
{
	if(length > 8)
	{
		return false;
	}
	
	sim_pin[0] = length;
	
	for(int i = 0; i < sim_pin[0]; i++)
	{
		sim_pin[i + 1] = sp[i];
	}
	
	eeprom_write_parameter(sim_pin, SIM_PIN);
	
	return true;
}


uint8_t get_sim_iccid(uint8_t* si)
{
	memcpy(si, sim_iccid, 11);
	
	return 11;
}



bool set_sim_iccid(uint8_t length, uint8_t* si)
{
	if(length != 11)
	{
		return false;
	}
	
	memcpy(sim_iccid, si, 11);
	
	eeprom_write_parameter(sim_iccid, SIM_ICCID);
	
	return true;
}


uint8_t get_sim_phone(uint8_t* sp)
{
	memcpy(sp, sim_phone, 8);
	
	return 8;
}



bool set_sim_phone(uint8_t length, uint8_t* sp)
{
	if(length != 8)
	{
		return false;
	}
	
	memcpy(sim_phone, sp, 8);
	
	eeprom_write_parameter(sim_phone, SIM_PHONE);
	
	return true;
}



uint32_t get_sim_ip(void)
{
	return sim_ip;
}



void set_sim_ip(uint32_t si)
{
	sim_ip = si;
	
	eeprom_write_parameter((uint8_t*)&sim_ip, SIM_IP);
}


uint8_t get_sim_smsc(uint8_t* ss)
{
	uint8_t length = strlen((char*)sim_smsc);
	
	memcpy(ss, sim_smsc, length);
	
	return length;
}



bool set_sim_smsc(uint8_t length, uint8_t* ss)
{
	if(length >= EL(SIM_SMSC))
	{
		return false;
	}
	
	memcpy(sim_smsc, ss, length);
	
	sim_smsc[length] = 0;
	
	eeprom_write(sim_smsc, EA(SIM_SMSC), length + 1);
	
	return true;
}



uint8_t get_sim_sms_validity(void)
{
	return sim_sms_validity;
}



bool set_sim_sms_validity(uint8_t sv)
{
	switch(sv)
	{
		case 0x0B:case 0x47:case 0x8F:case 0xA7:
		case 0xA8:case 0xA9:case 0xAD:case 0xFF:
		{
			break;
		}
		
		default:
		{
			return false;
		}
	}
	
	sim_sms_validity = sv;
	
	eeprom_write_parameter((uint8_t*)&sim_sms_validity, SIM_SMS_VALIDITY);
	
	return true;
}




uint8_t get_gprs_apn(uint8_t* ga)
{
	uint8_t length = strlen((char*)gprs_apn);
	
	memcpy(ga, gprs_apn, length);
	
	return length;
}



bool set_gprs_apn(uint8_t length, uint8_t* ga)
{
	if(length >= EL(GPRS_APN))
	{
		return false;
	}
	
	memcpy(gprs_apn, ga, length);
	
	gprs_apn[length] = 0;
	
	eeprom_write(gprs_apn, EA(GPRS_APN), length + 1);
	
	return true;
}




uint8_t get_gprs_user(uint8_t* gu)
{
	uint8_t length = strlen((char*)gprs_user);
	
	memcpy(gu, gprs_user, length);
	
	return length;
}



bool set_gprs_user(uint8_t length, uint8_t* gu)
{
	if(length >= EL(GPRS_USER))
	{
		return false;
	}
	
	memcpy(gprs_user, gu, length);
	
	gprs_user[length] = 0;
	
	eeprom_write(gprs_user, EA(GPRS_USER), length + 1);
	
	return true;
}



uint8_t get_gprs_password(uint8_t* gp)
{
	uint8_t length = strlen((char*)gprs_password);
	
	memcpy(gp, gprs_password, length);
	
	return length;
}



bool set_gprs_password(uint8_t length, uint8_t* gp)
{
	if(length >= EL(GPRS_PASSWORD))
	{
		return false;
	}
	
	memcpy(gprs_password, gp, length);
	
	gprs_password[length] = 0;
	
	eeprom_write(gprs_password, EA(GPRS_PASSWORD), length + 1);
	
	return true;
}



uint8_t get_gsm_quality(void)
{
	return gsm_quality;
}



bool set_gsm_quality(uint8_t gq)
{
	if(!((gsm_quality <= 31) || (gsm_quality == 99)))
	{
		return false;
	}
	
	gsm_quality = gq;
	
	eeprom_write_parameter((uint8_t*)&gsm_quality, GSM_QUALITY);
	
	return true;
}



uint8_t get_gsm_network_code(uint8_t* gnc)
{
	uint8_t length = strlen((char*)gsm_network_code);
	
	memcpy(gnc, gsm_network_code, length);
	
	return length;
}



bool set_gsm_network_code(uint8_t length, uint8_t* gnc)
{
	if(length >= EL(GSM_NETWORK_CODE))
	{
		return false;
	}
	
	memcpy(gsm_network_code, gnc, length);
	
	gsm_network_code[length] = 0;
	
	eeprom_write(gsm_network_code, EA(GSM_NETWORK_CODE), length + 1);
	
	return true;
}



uint32_t get_gsm_bts_lac(void)
{
	return gsm_bts_lac;
}



void set_gsm_bts_lac(uint32_t gbl)
{
	gsm_bts_lac = gbl;
	
	eeprom_write_parameter((uint8_t*)&gsm_bts_lac, GSM_BTS_LAC);
}



uint32_t get_gsm_bts_cell_id(void)
{
	return gsm_bts_cell_id;
}



void set_gsm_bts_cell_id(uint32_t gbci)
{
	gsm_bts_cell_id = gbci;
	
	eeprom_write_parameter((uint8_t*)&gsm_bts_cell_id, GSM_BTS_CELL_ID);
}



uint8_t get_modem_state(void)
{
	return modem_state;
}



bool set_modem_state(uint8_t ms)
{
	modem_state = ms;
	
	return true;
}


uint16_t get_modem_sms_received(void)
{
	return modem_sms_received;
}



void set_modem_sms_received(uint16_t msr)
{
	modem_sms_received = msr;
}



uint16_t get_modem_sms_sent(void)
{
	return modem_sms_sent;
}



void set_modem_sms_sent(uint16_t mss)
{
	modem_sms_sent = mss;
}




uint16_t get_modem_gprs_packets_received(void)
{
	return modem_gprs_packets_received;
}



void set_modem_gprs_packets_received(uint16_t mgpr)
{
	modem_gprs_packets_received = mgpr;
}



uint16_t get_modem_gprs_packets_sent(void)
{
	return modem_gprs_packets_sent;
}



void set_modem_gprs_packets_sent(uint16_t mgps)
{
	modem_gprs_packets_sent = mgps;
}



uint16_t get_modem_wakeup_count(void)
{
	return modem_wakeup_count;
}



void set_modem_wakeup_count(uint16_t mwc)
{
	modem_wakeup_count = mwc;
}


uint32_t get_modem_wakeup_duration(void)
{
	return modem_wakeup_duration;
}



void set_modem_wakeup_duration(uint32_t mwd)
{
	modem_wakeup_duration = mwd;
}

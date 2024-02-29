
#ifndef EXTRAS_H_
#define EXTRAS_H_

#include "inttypes.h"


void udelay(uint32_t ms);

char* itoa(unsigned int value, char *str, int base);

char* itoa64(uint64_t value, char *str, int base);

void init_extras(void);

void extras_pin1_off(void);

void extras_pin1_on(void);

void extras_pin1_toggle(void);

void extras_pin2_off(void);

void extras_pin2_on(void);

void extras_pin2_toggle(void);

int memcpy_to_buffer_uint16_t_bigendian(uint8_t* buf, uint16_t value);

int memcpy_to_buffer_int16_t_bigendian(uint8_t* buf, int16_t value);

int memcpy_to_buffer_uint32_t_bigendian(uint8_t* buf, uint32_t value);

int memcpy_to_buffer_int32_t_bigendian(uint8_t* buf, int32_t value);

int memcpy_to_buffer_uint64_t_bigendian(uint8_t* buf, uint64_t value);

int memcpy_to_buffer_int64_t_bigendian(uint8_t* buf, int64_t value);

int memcpy_to_buffer_int64_t_bigendian(uint8_t* buf, int64_t value);

int memcpy_to_buffer_uint16_t_littleendian(uint8_t* buf, uint16_t value);

int memcpy_to_buffer_int16_t_littleendian(uint8_t* buf, int16_t value);

int memcpy_to_buffer_uint32_t_littleendian(uint8_t* buf, uint32_t value);

int memcpy_to_buffer_int32_t_littleendian(uint8_t* buf, int32_t value);

int memcpy_to_buffer_uint64_t_littleendian(uint8_t* buf, uint64_t value);

int memcpy_to_buffer_int64_t_littleendian(uint8_t* buf, int64_t value);

uint16_t get_from_buffer_uint16_t_littleendian(uint8_t* buf);

int16_t get_from_buffer_int16_t_littleendian(uint8_t* buf);

uint32_t get_from_buffer_uint32_t_littleendian(uint8_t* buf);

int32_t get_from_buffer_int32_t_littleendian(uint8_t* buf);

uint64_t get_from_buffer_uint64_t_littleendian(uint8_t* buf);

int64_t get_from_buffer_int64_t_littleendian(uint8_t* buf);



#endif /*EXTRAS_H_*/

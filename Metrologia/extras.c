

#include "extras.h"

#include "inttypes.h"

#include "driver\driver_extras.h"

void udelay(uint32_t ms)
{
	uint32_t value = ms >> 2;
	
	if(value == 0)
	{
		value = 1;
	}
	
	for(volatile uint32_t i = 0; i < value; i++)
	{
	}
}



char* itoa(unsigned int value, char *str, int base)
{
	if((base > 36) || (base < 2))
	{
		*str = 0;
		return str;
	}


	if(value == 0)
	{
		*str = '0';
		*(str + 1) = 0;
		return str;
	}
	else
	{
		char *ptr = str;

		do
		{
			int tmp = (value % base);
			if(tmp >= 10)
				*ptr++ = 'A' + tmp - 10;
			else
				*ptr++ = '0' + tmp;
			value /= base;
		}
		while(value);

		*ptr = 0;

		int n = ptr - str;
		ptr--;
		int max_i = n >> 1;
		int i;
		for(i = 0; i < max_i; i++)
		{
			char tmp = str[i];
			str[i] = ptr[-i];
			ptr[-i] = tmp;
		}
	}

	return str;
}



char* itoa64(uint64_t value, char *str, int base)
{
	if((base > 36) || (base < 2))
	{
		*str = 0;
		return str;
	}


	if(value == 0)
	{
		*str = '0';
		*(str + 1) = 0;
		return str;
	}
	else
	{
		char *ptr = str;

		do
		{
			int tmp = (value % base);
			if(tmp >= 10)
				*ptr++ = 'A' + tmp - 10;
			else
				*ptr++ = '0' + tmp;
			value /= base;
		}
		while(value);

		*ptr = 0;

		int n = ptr - str;
		ptr--;
		int max_i = n >> 1;
		int i;
		for(i = 0; i < max_i; i++)
		{
			char tmp = str[i];
			str[i] = ptr[-i];
			ptr[-i] = tmp;
		}
	}

	return str;
}




void init_extras(void)
{
	init_driver_extras();
}



void extras_pin1_off(void)
{
	driver_extras_pin1_off();
}



void extras_pin1_on(void)
{
	driver_extras_pin1_on();
}


void extras_pin1_toggle(void)
{
	driver_extras_pin1_toggle();
}



void extras_pin2_off(void)
{
	driver_extras_pin2_off();
}



void extras_pin2_on(void)
{
	driver_extras_pin2_on();
}



void extras_pin2_toggle(void)
{
	driver_extras_pin2_toggle();
}



int memcpy_to_buffer_uint16_t_bigendian(uint8_t* buf, uint16_t value)
{
	buf[0] = (value >> 8) & 0xff;
	
	buf[1] = value & 0xff;
	
	return 2;
}



int memcpy_to_buffer_int16_t_bigendian(uint8_t* buf, int16_t value)
{
	buf[0] = (value >> 8) & 0xff;
	
	buf[1] = value & 0xff;
	
	return 2;
}




int memcpy_to_buffer_uint32_t_bigendian(uint8_t* buf, uint32_t value)
{
	buf[0] = (value >> 24) & 0xff;
	
	buf[1] = (value >> 16) & 0xff;
	
	buf[2] = (value >> 8) & 0xff;
	
	buf[3] = value & 0xff;
	
	return 4;
}



int memcpy_to_buffer_int32_t_bigendian(uint8_t* buf, int32_t value)
{
	buf[0] = (value >> 24) & 0xff;
	
	buf[1] = (value >> 16) & 0xff;
	
	buf[2] = (value >> 8) & 0xff;
	
	buf[3] = value & 0xff;
	
	return 4;
}



int memcpy_to_buffer_uint64_t_bigendian(uint8_t* buf, uint64_t value)
{
	buf[0] = (value >> 56) & 0xff;
	
	buf[1] = (value >> 48) & 0xff;
	
	buf[2] = (value >> 40) & 0xff;
	
	buf[3] = (value >> 32) & 0xff;
	
	buf[4] = (value >> 24) & 0xff;
	
	buf[5] = (value >> 16) & 0xff;
	
	buf[6] = (value >> 8) & 0xff;
	
	buf[7] = value & 0xff;
	
	return 8;
}



int memcpy_to_buffer_int64_t_bigendian(uint8_t* buf, int64_t value)
{
	buf[0] = (value >> 56) & 0xff;
	
	buf[1] = (value >> 48) & 0xff;
	
	buf[2] = (value >> 40) & 0xff;
	
	buf[3] = (value >> 32) & 0xff;
	
	buf[4] = (value >> 24) & 0xff;
	
	buf[5] = (value >> 16) & 0xff;
	
	buf[6] = (value >> 8) & 0xff;
	
	buf[7] = value & 0xff;
	
	return 8;
}



int memcpy_to_buffer_uint16_t_littleendian(uint8_t* buf, uint16_t value)
{
	buf[1] = (value >> 8) & 0xff;
	
	buf[0] = value & 0xff;
	
	return 2;
}



int memcpy_to_buffer_int16_t_littleendian(uint8_t* buf, int16_t value)
{
	buf[1] = (value >> 8) & 0xff;
	
	buf[0] = value & 0xff;
	
	return 2;
}




int memcpy_to_buffer_uint32_t_littleendian(uint8_t* buf, uint32_t value)
{
	buf[3] = (value >> 24) & 0xff;
	
	buf[2] = (value >> 16) & 0xff;
	
	buf[1] = (value >> 8) & 0xff;
	
	buf[0] = value & 0xff;
	
	return 4;
}



int memcpy_to_buffer_int32_t_littleendian(uint8_t* buf, int32_t value)
{
	buf[3] = (value >> 24) & 0xff;
	
	buf[2] = (value >> 16) & 0xff;
	
	buf[1] = (value >> 8) & 0xff;
	
	buf[0] = value & 0xff;
	
	return 4;
}



int memcpy_to_buffer_uint64_t_littleendian(uint8_t* buf, uint64_t value)
{
	buf[7] = (value >> 56) & 0xff;
	
	buf[6] = (value >> 48) & 0xff;
	
	buf[5] = (value >> 40) & 0xff;
	
	buf[4] = (value >> 32) & 0xff;
	
	buf[3] = (value >> 24) & 0xff;
	
	buf[2] = (value >> 16) & 0xff;
	
	buf[1] = (value >> 8) & 0xff;
	
	buf[0] = value & 0xff;
	
	return 8;
}



int memcpy_to_buffer_int64_t_littleendian(uint8_t* buf, int64_t value)
{
	buf[7] = (value >> 56) & 0xff;
	
	buf[6] = (value >> 48) & 0xff;
	
	buf[5] = (value >> 40) & 0xff;
	
	buf[4] = (value >> 32) & 0xff;
	
	buf[3] = (value >> 24) & 0xff;
	
	buf[2] = (value >> 16) & 0xff;
	
	buf[1] = (value >> 8) & 0xff;
	
	buf[0] = value & 0xff;
	
	return 8;
}




uint16_t get_from_buffer_uint16_t_littleendian(uint8_t* buf)
{
	uint16_t value = buf[1];
	value <<= 8;
	
	value |= buf[0];
	
	return value;
}



int16_t get_from_buffer_int16_t_littleendian(uint8_t* buf)
{
	int16_t value = buf[1];
	value <<= 8;
	
	value |= buf[0];
	
	return value;
}



uint32_t get_from_buffer_uint32_t_littleendian(uint8_t* buf)
{
	uint32_t value = buf[3];
	value <<= 8;
	
	value |= buf[2];
	value <<= 8;
	
	value |= buf[1];
	value <<= 8;
	
	value |= buf[0];
	
	return value;
}



int32_t get_from_buffer_int32_t_littleendian(uint8_t* buf)
{
	int32_t value = buf[3];
	value <<= 8;
	
	value |= buf[2];
	value <<= 8;
	
	value |= buf[1];
	value <<= 8;
	
	value |= buf[0];
	
	return value;
}




uint64_t get_from_buffer_uint64_t_littleendian(uint8_t* buf)
{
	uint64_t value = buf[7];
	value <<= 8;
	
	value |= buf[6];
	value <<= 8;
	
	value |= buf[5];
	value <<= 8;
	
	value |= buf[4];
	value <<= 8;
	
	value |= buf[3];
	value <<= 8;
	
	value |= buf[2];
	value <<= 8;
	
	value |= buf[1];
	value <<= 8;
	
	value |= buf[0];
	
	return value;
}



int64_t get_from_buffer_int64_t_littleendian(uint8_t* buf)
{
	int64_t value = buf[7];
	value <<= 8;
	
	value |= buf[6];
	value <<= 8;
	
	value |= buf[5];
	value <<= 8;
	
	value |= buf[4];
	value <<= 8;
	
	value |= buf[3];
	value <<= 8;
	
	value |= buf[2];
	value <<= 8;
	
	value |= buf[1];
	value <<= 8;
	
	value |= buf[0];
	
	return value;
}



#include "crc.h"

#include "driver\driver_crc.h"



									
uint32_t get_crc32(uint8_t *buf, uint32_t length)
{
	return driver_crc_get_crc32(buf, length);
}


void init_crc(void)
{
	init_driver_crc();
}




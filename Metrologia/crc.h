#ifndef CRC_H_
#define CRC_H_


#include "inttypes.h"

void init_crc(void);

uint32_t get_crc32(uint8_t *buf, uint32_t length);


#endif /*CRC_H_*/

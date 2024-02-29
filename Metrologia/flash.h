#ifndef FLASH_H_
#define FLASH_H_


#include "stdbool.h"

#include "stdint.h"


void init_flash(void);

unsigned char flash_page_read(uint8_t *buf, unsigned int page, unsigned int offset, unsigned int bytes);

unsigned char flash_page_write(uint8_t *buf, unsigned int page);

unsigned char flash_page_write_no_erase(uint8_t *buf, unsigned int page);

unsigned char flash_page_erase(unsigned int page);

void flash_sector_erase(unsigned int sector);



#endif /*FLASH_H_*/

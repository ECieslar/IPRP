
#include "flash.h"

#include "driver/driver_flash.h"

void init_flash(void)
{
	driver_init_flash();
}


unsigned char flash_page_read(uint8_t *buf, unsigned int page, unsigned int offset, unsigned int bytes)
{
	return driver_flash_page_read(buf, page, offset, bytes);
}



unsigned char flash_page_write(uint8_t *buf, unsigned int page)
{
	return driver_flash_page_write(buf, page);
}


unsigned char flash_page_write_no_erase(uint8_t *buf, unsigned int page)
{
	return driver_flash_page_write_no_erase(buf, page);
}




unsigned char flash_page_erase(unsigned int page)
{
	return driver_flash_page_erase(page);
}



void flash_sector_erase(unsigned int sector)
{
	unsigned int sector_address = sector << 14;

	unsigned char address2, address1, address0;

	address2 = sector_address >> 6;
	address1 = (sector_address & 0x3F) << 2;
	address0 = 0;

	driver_flash_sector_erase(address2, address1, address0);
}




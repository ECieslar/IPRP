
#include "logs.h"

#include "date_time.h"

#include "flash.h"

#include <string.h>



static uint8_t logs[1024];

static int logs_idx;

static int logs_page_nr;

static const int logs_page_start = 0;

static const int logs_max_pages = 1024;






void init_logs(void)
{
	logs_idx = 0;
	
	logs_page_nr = 0;
}



void add_log(enum log_types lt, uint8_t* tab)
{
	uint32_t utl = get_unix_time_local();
	
	memcpy(logs + logs_idx, (uint8_t*)&utl, 4);
	
	logs_idx += 4;
	
	uint8_t log_value[4];
	
	log_value[0] = lt;
	
	memcpy(log_value + 1, tab, 3);
	
	memcpy(logs, log_value, 4);
	
	logs_idx += 4;
	
	if(logs_idx >= 1024)
	{
		logs_idx = 0;
		
		flash_page_write(logs, logs_page_start + logs_page_nr++);
		
		if(logs_page_nr >= logs_max_pages)
		{
			logs_page_nr = 0;
		}
	}
}



int get_log(uint8_t* value, int idx)
{
	if(idx >= (logs_max_pages << 7))
	{
		return -1;
	}
	
	flash_page_read(value, idx >> 7, 0x3ff & (idx << 3), 8);
	
	return 0;
}




#include "lcd.h"

#include "driver\driver_lcd.h"

#include "inttypes.h"

#include "extras.h"

#include "timers.h"

#include "battery.h"

#include <string.h>










void init_lcd(void)
{
	driver_init_lcd();
}





void lcd_send(char* text, int start)
{
	driver_lcd_clear();
	
	for(int i = start, j = 0; (i < 9) && (text[j] != 0);)
	{
		if(text[j] == '.')
		{
			if(text[j + 1] == '.')
			{
				driver_lcd_set_digit(i, text[j], true);
				i++;
			}
			else
			{
				j++;
			}
		}
		else
		{
			if(text[j + 1] == '.')
			{
				driver_lcd_set_digit(i, text[j], true);
			}
			else
			{
				
				driver_lcd_set_digit(i, text[j], false);
			}
			i++;
			j++;
		}
		
	}
	
	driver_lcd_update();
}





void lcd_set_segments(uint32_t segment)
{
	driver_lcd_set_segments(segment);
}


void lcd_clr_segments(uint32_t segment)
{
	driver_lcd_clr_segments(segment);
}	


void lcd_off(void)
{
	driver_lcd_off();
}


void lcd_on(void)
{
	driver_lcd_on();
}


void lcd_automatic_fill_segments(void)
{
	lcd_clr_segments(0xffffffff);
	

	
	lcd_set_segments(SEG_ACCU0);
	
	uint8_t battery_level = get_battery_level();
	
	if(battery_level > 20)
	{
		lcd_set_segments(SEG_ACCU1);
		
		if(battery_level > 40)
		{
			lcd_set_segments(SEG_ACCU2);
			
			if(battery_level > 60)
			{
				lcd_set_segments(SEG_ACCU3);
				
				if(battery_level > 80)
				{
					lcd_set_segments(SEG_ACCU4);
				}
			}
		}
	}
}



#ifndef LCD_H_
#define LCD_H_


#include "stdbool.h"

#include "inttypes.h"





#define SEG_RADIO				(1 << 0)
#define SEG_ACCU0				(1 << 1)
#define SEG_ACCU1				(1 << 2)
#define SEG_ACCU2				(1 << 3)
#define SEG_ACCU3				(1 << 4)
#define SEG_ACCU4				(1 << 5)
#define SEG_KEY					(1 << 6)
#define SEG_WARN				(1 << 7)
#define SEG_VALVE				(1 << 8)
#define SEG_COIN				(1 << 9)
#define SEG_ERROR				(1 << 10)
#define SEG_BRACE_OVER	(1 << 11)
#define SEG_GRAD				(1 << 12)
#define SEG_PRESSURE		(1 << 13)
#define SEG_KWH					(1 << 14)
#define SEG_M3					(1 << 15)
#define SEG_SLASH_H			(1 << 16)
#define SEG_BRACE_UNDER	(1 << 17)
#define SEG_TRIANGLE4		(1 << 18)
#define SEG_TRIANGLE3		(1 << 19)
#define SEG_TRIANGLE2		(1 << 20)
#define SEG_TRIANGLE1		(1 << 21)
#define SEG_TRIANGLE0		(1 << 22)



void init_lcd(void);

//void lcd_send(int line_nr, char* text);

void lcd_send(char* text, int start);

void lcd_set_digit(uint8_t position, uint8_t value, bool dot);

void lcd_set_segments(uint32_t segment);

void lcd_clr_segments(uint32_t segment);

void lcd_off(void);

void lcd_on(void);

void lcd_automatic_fill_segments(void);




#endif /*LCD_H_*/

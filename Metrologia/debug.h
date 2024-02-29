
#ifndef DEBUG_H_
#define DEBUG_H_



#include "extras.h"

#include "stdbool.h"



//INFO, WARNING i ERROR
#define DEBUG_LEVEL	0

//WARNING i ERROR
//#define DEBUG_LEVEL	1

//ERROR
//#define DEBUG_LEVEL	2




#ifdef DEBUG_LEVEL

void init_debug(void);

void debug_send_text(int log_level, char* text, char* file, int line);

bool debug_is_transmitting(void);

#if DEBUG_LEVEL == 0

#define debug_info(text) debug_send_text(0, text, __FILE__, __LINE__)
	
#define debug_warning(text) debug_send_text(1, text, __FILE__, __LINE__)

#define debug_error(text) debug_send_text(2, text, __FILE__, __LINE__)

#elif DEBUG_LEVEL == 1

#define debug_info(text) {}

#define debug_warning(text) debug_send_text(1, text, __FILE__, __LINE__)

#define debug_error(text) debug_send_text(2, text, __FILE__, __LINE__)

#elif DEBUG_LEVEL == 2

#define debug_info(text) {}

#define debug_warning(text) {}

#define debug_error(text) debug_send_text(2, text, __FILE__, __LINE__)

#endif

#else

#define init_debug() {}
	
#define debug_is_transmitting()	false

#define debug_info(text) {}

#define debug_warning(text) {}

#define debug_error(text) {}

#endif



#endif /*DEBUG_H_*/

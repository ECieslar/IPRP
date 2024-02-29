

#include "debug.h"

#include "driver/driver_debug.h"

#include <string.h>





#ifdef DEBUG_LEVEL

/*Funkcja inicjalizujaca podsystem debug*/
void init_debug(void)
{
	init_driver_debug();
	
	debug_info("Inicjalizacja logow");
}



/*Funkcja wysylajaca informacje na wyjscie debug*/
void debug_send_text(int log_level, char* text, char* file, int line)
{
	//wybierz typ wiadomosci i wedlug wybranego typu rozpocznij wiadomosc
	switch(log_level)
	{
		case 0:
		{
			driver_debug_send((uint8_t*)"INFO", strlen("INFO"));
			break;
		}
		
		case 1:
		{
			driver_debug_send((uint8_t*)"WARNING", strlen("WARNING"));
			break;
		}
		
		case 2:
		{
			driver_debug_send((uint8_t*)"ERROR", strlen("ERROR"));
			break;
		}
	}
	
	driver_debug_send((uint8_t*)" ", 1);
	
	//wstaw nasza wiadomosc
	driver_debug_send((uint8_t*)text, strlen(text));
	
	//jesli ostrzezenie lub blad
	if(log_level != 0)
	{
		driver_debug_send((uint8_t*)" ", 1);
		
		//nazwa pliu, w którym nastapilo ostrzezenie lub blad
		driver_debug_send((uint8_t*)file, strlen(file));
		
		driver_debug_send((uint8_t*)" ", 1);
		
		char c_line[11];
		
		itoa(line, c_line, 10);
		
		//numer lini, w której nastapilo ostrzezenie lub blad
		driver_debug_send((uint8_t*)c_line, strlen(c_line));
	}
	
	//przejdz do nastepnej linii
	driver_debug_send((uint8_t*)"\xd\xa", 2);
}



/*Funkcja zwracajaca informacje, czy jest transmisja na wyjscie debug*/
bool debug_is_transmitting(void)
{
	return driver_debug_is_transmitting();
}



#endif


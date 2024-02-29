

#include "hub_communication.h"

#include "driver/driver_hub_communication.h"

#include "crc.h"

#include "ultrasonic_meter.h"

#include "pressure_meter.h"

#include "valve.h"

#include "bellows.h"

#include "calorimeter.h"

#include <string.h>




//maksymalna liczba ramek w buforze
static const int max_hub_send_buffer_count = 4;

//maksymalna dlugosc dla pojedynczej nadawanej ramki
static const int max_hub_send_buffer_size = 512;

//maksymalna dlugosc dla odbieranej ramki
static const int max_hub_receive_buffer_size = 512;

//bufor nadajnika
static uint8_t send_buffer[max_hub_send_buffer_count][max_hub_send_buffer_size];

//bajt poczatkowy w ramce
static const uint8_t start_byte = 0x55;

//bajt koncowy w ramce
static const uint8_t stop_byte = 0xff;

//bufor odbiornika
static uint8_t receive_buffer[max_hub_receive_buffer_size];

//wskaznik na pierwszy bajt w buforze odbiornika
static int start_receive_buffer;

//wskaznik na ostatni bajt w buforze odbiornika
static int stop_receive_buffer;

//wskaznik na pierwsza ramke w buforze nadajnika
static int start_send_buffer;

//wskaznik na ostatnia ramke w buforze nadajnika
static int stop_send_buffer;



/*Inicjalizacja komunikacji z modulem koncentratora*/
void init_hub_communication(void)
{
	start_send_buffer = 0;
	
	stop_send_buffer = 0;
	
	init_driver_hub_communication();
}


/*Funkcja sprawdzajaca, czy jest miejsce na kolejna ramke w buforze nadajnika*/
static bool is_send_buffer_available()
{
	int tmp_stop_send_buffer = stop_send_buffer + 1;
	
	if(tmp_stop_send_buffer >= max_hub_send_buffer_count)
	{
		tmp_stop_send_buffer = 0;
	}
	
	if(tmp_stop_send_buffer == start_send_buffer)
	{
		return false;
	}
	
	return true;
}


/*Funkcja probujaca wyslac dane poprzez sterownik nadajnika*/
static void try_send_data()
{
	//jezeli nadajnik sterownika nic nie wysyla, to mozna wyslac nasze dane
	if(driver_send_hub_communication_data(send_buffer[start_send_buffer][0], send_buffer[start_send_buffer] + 1))
	{
		start_send_buffer++;
	
		if(start_send_buffer >= max_hub_send_buffer_count)
		{
			start_send_buffer = 0;
		}
	}
}



/*Funkcja wysylajaca ramke do modulu koncentratora*/
void send_hub_data(enum hub_communication_functions function, uint8_t* data, uint16_t length)
{
	//czy jest wolne miejsce na kolejna ramke
	if(!is_send_buffer_available())
	{
		return;
	}
		
	int idx = 1;
	
	send_buffer[stop_send_buffer][idx++] = start_byte;
	
	send_buffer[stop_send_buffer][idx++] = 0x80;
	
	send_buffer[stop_send_buffer][idx++] = length >> 8;
	
	send_buffer[stop_send_buffer][idx++] = length & 0xff;
	
	send_buffer[stop_send_buffer][idx++] = function;
	
	for(int i = 0; i < length; i++)
	{
		send_buffer[stop_send_buffer][idx++] = data[i];
	}
	
	uint32_t crc32 = get_crc32(send_buffer[stop_send_buffer] + 2, length + 4);
	
	send_buffer[stop_send_buffer][idx++] = (crc32 >> 24) & 0xff;
	
	send_buffer[stop_send_buffer][idx++] = (crc32 >> 16) & 0xff;
	
	send_buffer[stop_send_buffer][idx++] = (crc32 >> 8) & 0xff;
	
	send_buffer[stop_send_buffer][idx++] = crc32 & 0xff;
	
	send_buffer[stop_send_buffer][idx++] = stop_byte;
	
	send_buffer[stop_send_buffer][0] = idx - 1;
	
	stop_send_buffer++;
	
	if(stop_send_buffer >= max_hub_send_buffer_count)
	{
		stop_send_buffer = 0;
	}
	
	try_send_data();
}



/*Funkcja zwracająca liczbę bajtów z bufora odbiornika*/
static uint16_t get_receive_buffer_length(void)
{
	uint16_t tmp_stop_receive_buffer = stop_receive_buffer;
	
	if(stop_receive_buffer < start_receive_buffer)
	{
		 tmp_stop_receive_buffer += max_hub_receive_buffer_size;
	}
	
	return tmp_stop_receive_buffer - start_receive_buffer;
}



/*Funkcja analizująca odebrane ramki*/
static void analyse_hub_data(enum hub_communication_functions function, uint8_t* data, uint16_t length)
{
	switch(function)
	{
		case hcf_ultrasonic_meter:
		{
			if(length >= 3)
			{
				ultrasonic_meter_value(data);
				
				if(length >= 5)
				{
					ultrasonic_meter_temperature(data + 3);
				}
			}
				
			break;
		}
		
		case hcf_pressure_meter:
		{
			set_pressure_meter_values(data, length);
			
			break;
		}
		
		case hcf_valve_current:
		{
			if(length >= 2)
			{
				valve_current(data);
			}
			
			break;
		}
		
		case hcf_valve_hallotron_tick:
		{
			if(length >= 10)
			{
				hallotron_tick(data);
			}
			
			break;
		}
		
		case hcf_calorimeter_holding_registers:
		{
			calorimeter_modbus_holding_registers(data, length);
			break;
		}
		
		case hcf_calorimeter_input_registers:
		{
			calorimeter_modbus_input_registers(data, length);
			break;
		}
		
		case hcf_calorimeter_caloric_value:
		{
			if(length >= 4)
			{
				set_calorimeter_calorific_value(data);
			}
			break;
		}
		
		
		default:
		{
			break;
		}
	}
}



/*Funkcja sprawdzajaca komunikacje z modulem koncentratora*/
void check_hub_communication(void)
{
	//czy jest coś do wysłania
	if(start_send_buffer != stop_send_buffer)
	{
		try_send_data();
	}
	
	//pobierz bajt z bufora odbiornika
	int character = get_driver_hub_communication_value();
	
	//jeśli zwrócono -1, to bufor odbiornika jest pusty
	if(character == -1)
	{
		return;
	}
	
	//pobierz wszystkie bajty z bufora odbiornika
	while(character != -1)
	{
		uint8_t u8_character = character;
		
		int next_stop_buffer = stop_receive_buffer + 1;
		
		if(next_stop_buffer >= max_hub_receive_buffer_size)
		{
			next_stop_buffer = 0;
		}
		
		//sprawdź, czy nasz bufor ma jeszcze miejsce
		if(next_stop_buffer != start_receive_buffer)
		{
			receive_buffer[stop_receive_buffer] = u8_character;
			
			stop_receive_buffer = next_stop_buffer;
		}
		
		character = get_driver_hub_communication_value();
	}

	
	do
	{
		//analizuj tylko ramki >= 10 bajtów
		if(get_receive_buffer_length() < 10)
		{
			return;
		}
		
		//poszukaj bajtu początkowego
		do
		{
			if(receive_buffer[start_receive_buffer] == start_byte)
			{
				break;
			}
			else
			{
				start_receive_buffer++;
				
				if(start_receive_buffer >= max_hub_receive_buffer_size)
				{
					start_receive_buffer = 0;
				}
			}
			
			if(start_receive_buffer == stop_receive_buffer)
			{
				return;
			}
		}
		while(1);
		
		//analizuj tylko ramki >= 10 bajtów
		if(get_receive_buffer_length() < 10)
		{
			return;
		}
		
		//bufor tymczasowy
		uint8_t tmp_receive_buffer[max_hub_receive_buffer_size];
		
		int receive_buffer_length = get_receive_buffer_length();
				
		//przepisz bufor wejściowy do bufora tymczasowego
		for(int i = 0, j = start_receive_buffer; i < receive_buffer_length; i++)
		{
			tmp_receive_buffer[i] = receive_buffer[j++];
			
			if(j >= max_hub_receive_buffer_size)
			{
				j = 0;
			}
		}
		
		//sprawdź typ ramki odebranej
		if(tmp_receive_buffer[1] == 0x00)
		{
			uint16_t receive_frame_length = tmp_receive_buffer[2];
			receive_frame_length <<= 8;
			receive_frame_length |= tmp_receive_buffer[3];
			
			//czy liczba bajtów w buforze tymczasowym jest wystarczająca
			if(receive_frame_length + 10 <= receive_buffer_length)
			{
				//czy ramka jest zakończona poprawnie
				if(tmp_receive_buffer[receive_frame_length + 9] == stop_byte)
				{
					uint32_t crc32 = get_crc32(tmp_receive_buffer + 1, receive_frame_length + 4);
					
					//czy odebrana suma kontrolna jest zgodna z tą obliczoną
					if(	(tmp_receive_buffer[receive_frame_length + 5] == ((crc32 >> 24) & 0xff)) &&
							(tmp_receive_buffer[receive_frame_length + 6] == ((crc32 >> 16) & 0xff)) &&
							(tmp_receive_buffer[receive_frame_length + 7] == ((crc32 >> 8) & 0xff)) &&
							(tmp_receive_buffer[receive_frame_length + 8] == (crc32 & 0xff)))
					{
						//ramka jest poprawna, można przejść do analizy otrzymanych danych
						analyse_hub_data((enum hub_communication_functions)tmp_receive_buffer[4], tmp_receive_buffer + 5, receive_frame_length);
						
						//przesuń się na kolejne dane
						start_receive_buffer += receive_frame_length + 10;
					}
					else
					{
						//przesuń się o jeden bajt w buforze odbiornika
						start_receive_buffer++;
					}
				}
				else
				{
					//przesuń się o jeden bajt w buforze odbiornika
					start_receive_buffer++;
				}
			}
			else
			{
				//za mało danych w buforze odbiornika, nie ma co analizować, należy zaczekać
				return;
			}
		}
		else
		{
			//przesuń się o jeden bajt w buforze odbiornika
			start_receive_buffer++;
		}
		
		if(start_receive_buffer >= max_hub_receive_buffer_size)
		{
			start_receive_buffer -= max_hub_receive_buffer_size;
		}
	}
	while(1);
}


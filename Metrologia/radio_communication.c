

#include "radio_communication.h"

#include "driver/driver_radio_communication.h"

#include "timers.h"

#include "crc.h"

#include <string.h>




static const uint8_t radio_protocol_version = 0x00;

//maksymalna liczba ramek w buforze
static const int max_radio_send_buffer_count = 8;

//maksymalna dlugosc dla pojedynczej nadawanej ramki
static const int max_radio_send_buffer_size = 266;

//maksymalna dlugosc dla odbieranej ramki
static const int max_radio_receive_buffer_size = 266;

//bufor nadajnika
static uint8_t send_buffer[max_radio_send_buffer_count][max_radio_send_buffer_size];

//bajt poczatkowy w ramce
static const uint8_t start_byte = 0x55;

//bajt koncowy w ramce
static const uint8_t stop_byte = 0xff;

//bufor odbiornika
static uint8_t receive_buffer[max_radio_receive_buffer_size];

//wskaznik na pierwszy bajt w buforze odbiornika
static int start_receive_buffer;

//wskaznik na ostatni bajt w buforze odbiornika
static int stop_receive_buffer;

//wskaznik na pierwsza ramke w buforze nadajnika
static int start_send_buffer;

//wskaznik na ostatnia ramke w buforze nadajnika
static int stop_send_buffer;

static uint8_t send_counter = 0x00;

static uint32_t send_frames_counter;

static uint32_t receive_frames_counter;

static uint8_t tmp_sender;





/*Funkcja sprawdzajaca, czy jest miejsce na kolejna ramke w buforze nadajnika*/
static bool is_send_buffer_available()
{
	int tmp_stop_send_buffer = stop_send_buffer + 1;
	
	if(tmp_stop_send_buffer >= max_radio_send_buffer_count)
	{
		tmp_stop_send_buffer = 0;
	}
	
	if(tmp_stop_send_buffer == start_send_buffer)
	{
		return false;
	}
	
	return true;
}



/*Funkcja próbująca wysłać dane poprzez sterownik nadajnika*/
static void try_send_data()
{
	if(start_send_buffer != stop_send_buffer)
	{
		//jezeli nadajnik sterownika nic nie wysyla, to mozna wyslac nasze dane
		if(driver_radio_communication_send_data(send_buffer[start_send_buffer][0], send_buffer[start_send_buffer] + 1))
		{
			send_frames_counter++;

			start_send_buffer++;
		
			if(start_send_buffer >= max_radio_send_buffer_count)
			{
				start_send_buffer = 0;
			}
		}
	}
}
	



/*Funkcja wysylajaca ramke do modulu komunikacyjnego*/
void send_radio_data(enum radio_communication_functions function, uint8_t* data, uint8_t length)
{
	//czy jest wolne miejsce na kolejna ramke
	if(!is_send_buffer_available())
	{
		return;
	}
		
	int idx = 1;
	
	send_buffer[stop_send_buffer][idx++] = start_byte;
	
	send_buffer[stop_send_buffer][idx++] = tmp_sender | radio_protocol_version;
	
	send_buffer[stop_send_buffer][idx++] = send_counter++;
	
	send_buffer[stop_send_buffer][idx++] = length;
	
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
	
	if(stop_send_buffer >= max_radio_send_buffer_count)
	{
		stop_send_buffer = 0;
	}
	
	try_send_data();
}



//static void send_data()
//{
	//tmp_sender = 0x80;
	/*
	uint8_t data[20];
	
	for(int i = 0; i < 20; i++)
	{
		data[i] = 20 + i;
	}
	
	//driver_radio_communication_send_data(20, data);
	
	send_radio_data(rcf_server, data, 20);
	send_radio_data(rcf_server, data, 20);
	*/
	
	/*
	uint8_t data[266];
	
	uint8_t obj_id[2] = {0x04, 0x0C};
	
	//send_radio_data(rcf_server, data, smart_gas_object(obj_id, 2, data));
	send_radio_data(rcf_radio, obj_id, 2);
	*/
//}



//void (*callback_data_sent)()


/*Inicjalizacja komunikacji z modulem koncentratora*/
void init_radio_communication(void)
{
	init_driver_radio_communication(try_send_data);
	
	send_frames_counter = 0;

	receive_frames_counter = 0;
	
	//register_time_event(send_data, 1000, false);
}



/*Funkcja zwracająca liczbę bajtów z bufora odbiornika*/
static uint16_t get_receive_buffer_length(void)
{
	uint16_t tmp_stop_receive_buffer = stop_receive_buffer;
	
	if(stop_receive_buffer < start_receive_buffer)
	{
		 tmp_stop_receive_buffer += max_radio_receive_buffer_size;
	}
	
	return tmp_stop_receive_buffer - start_receive_buffer;
}




/*Funkcja analizująca odebrane ramki*/
static void analyse_radio_data(enum radio_communication_functions function, uint8_t* data, uint8_t length)
{
	tmp_sender = 0x00;
	
	uint8_t response_data[266];
	
	receive_frames_counter++;
	
	switch(function)
	{
		case rcf_server:
		{
			int response_length = smart_gas_object(data, length, response_data, 266, sgos_radio);
			
			if(response_length != -1)
			{
				send_radio_data(rcf_server, response_data, response_length);
			}
			break;
		}
		
		case rcf_radio:
		{
			int response_length = smart_gas_object(data, length, response_data, 266, sgos_radio);
			
			if(response_length != -1)
			{
				send_radio_data(rcf_radio, response_data, response_length);
			}
			
			break;
		}
		
		case rcf_repeat_demand:
		{
			break;
		}
		
		case rcf_baudrate:
		{
			break;
		}
		
		default:
		{
			break;
		}
	}
}



void radio_communication_send_smart_gas(enum smart_gas_object_ids obj_id, uint8_t* data, uint16_t length)
{
	uint8_t buffer[max_radio_send_buffer_size];
	
	buffer[0] = obj_id >> 8;
				
	buffer[1] = obj_id & 0xff;
	
	memcpy(buffer + 2, data, length);
	
	send_radio_data(rcf_server, buffer, length + 2);
}





/*Funkcja sprawdzajaca komunikacje z modulem koncentratora*/
void check_radio_communication(void)
{
	/*
	//czy jest coś do wysłania
	if(start_send_buffer != stop_send_buffer)
	{
		try_send_data();
	}
	*/
	
	//pobierz bajt z bufora odbiornika
	int character = driver_radio_communication_get_value();
	
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
		
		if(next_stop_buffer >= max_radio_receive_buffer_size)
		{
			next_stop_buffer = 0;
		}
		
		//sprawdź, czy nasz bufor ma jeszcze miejsce
		if(next_stop_buffer != start_receive_buffer)
		{
			receive_buffer[stop_receive_buffer] = u8_character;
			
			stop_receive_buffer = next_stop_buffer;
		}
		
		character = driver_radio_communication_get_value();
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
				
				if(start_receive_buffer >= max_radio_receive_buffer_size)
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
		uint8_t tmp_receive_buffer[max_radio_receive_buffer_size];
		
		int receive_buffer_length = get_receive_buffer_length();
				
		//przepisz bufor wejściowy do bufora tymczasowego
		for(int i = 0, j = start_receive_buffer; i < receive_buffer_length; i++)
		{
			tmp_receive_buffer[i] = receive_buffer[j++];
			
			if(j >= max_radio_receive_buffer_size)
			{
				j = 0;
			}
		}
		
		//sprawdź typ ramki odebranej
		if((tmp_receive_buffer[1] & 0x80) == 0x80)
		{
			uint8_t receive_frame_length = tmp_receive_buffer[3];
			
			//czy liczba bajtów w buforze tymczasowym jest wystarczająca
			if(receive_frame_length + 10 <= receive_buffer_length)
			{
				//czy ramka jest zakoćczona poprawnie
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
						analyse_radio_data((enum radio_communication_functions)tmp_receive_buffer[4], tmp_receive_buffer + 5, receive_frame_length);
						
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
		
		if(start_receive_buffer >= max_radio_receive_buffer_size)
		{
			start_receive_buffer -= max_radio_receive_buffer_size;
		}
	}
	while(1);
}



uint32_t get_radio_send_frames_counter(void)
{
	return send_frames_counter;
}



uint32_t get_radio_receive_frames_counter(void)
{
	return receive_frames_counter;
}


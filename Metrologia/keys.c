


#include "keys.h"

#include "timers.h"

#include "menu.h"

#include "driver\driver_keys.h"

#include "status.h"

#include "inttypes.h"



static uint8_t keys;

static uint8_t previous_keys;

static uint8_t keys_tab[3] = {0, 0, 0};

static uint8_t keys_tab_idx = 0;

static uint8_t key_scan_repeat;

static uint32_t key_count_tick;

static int key_activation_timeout_idx;

static bool key_activation;





static uint8_t keys_mapped[4] = 	{
									KM_SW1,
									KM_SW2,
									KM_SW3,
									};



void check_key_activation(void)
{
	if(driver_is_key_activation())
	{
		key_activation = true;
		
		start_time_event(key_activation_timeout_idx);
		
		driver_reset_key_activation();
	}
}



static void check_keys()
{
	static uint8_t tmp_keys;
	

	if(driver_keys_is_sw_1())
	{
		tmp_keys |= 0x01;
	}
	
	if(driver_keys_is_sw_2())
	{
		tmp_keys |= 0x02;
	}
	
	if(driver_keys_is_sw_3())
	{
		tmp_keys |= 0x04;
	}
	

	
	static bool key_pressed = false;
	
	static uint8_t last_key_pressed;
	
	static uint8_t key_press_delay = 0;
	
	
	
	keys_tab[keys_tab_idx++] = tmp_keys;
	tmp_keys = 0;
	if(keys_tab_idx >= key_scan_repeat)
		keys_tab_idx = 0;
	

	int tmp1 = keys_tab[0];
	int tmp2 = keys_tab[0];


	int i;
	for(i = 1; i < key_scan_repeat; i++)
	{
		tmp1 &= keys_tab[i];
		tmp2 |= keys_tab[i];
	}

	int tmp = tmp1 ^ tmp2;
	keys = (tmp & previous_keys) | (~tmp & tmp1);

	if(keys)
	{
		set_status_flag(flag_button);
	}
	else
	{
		clr_status_flag(flag_button);
	}

	//czy zmienił się stan klawiatury
	if(previous_keys != keys)
	//nastąpiła zmiana stanu klawiatury
	{
		//każdy ustawiony bit w zmiennej dif oznacza zmianę na klawiszu
		uint32_t dif = keys ^ previous_keys;

		int i;
		//szukaj zmienionego klawisza
		for(i = 0; i <= 23; i++)
		{
			//czy nastąpiła zmiana na i-tym klawiszu
			if(dif & (1 << i))
			{
				uint8_t key_change;

				//pobierz identyfikator zmienionego klawisza
				key_change = keys_mapped[i];

				//czy nastąpiło naciśnięcie klawisza
				if(keys & (1 << i))
				//naciśnięto klawisz
				{
					//przekaż do menu naciśnięty klawisz
					menu_event(menu_event_key_down, &key_change);

					//zapamiętaj ostatnio naciśnięty klawisz
					last_key_pressed = key_change;

					//zapamiętaj, że klawisz jest naciśnięty
					key_pressed = true;

					//opóźnienie wysłania stanu klawiatury
					//key_press_delay = 250;
					key_press_delay = 31;
				}
				else
				//zwolniono klawisz
				{
					//przekaż do menu zwolniony klawisz
					menu_event(menu_event_key_up, &key_change);

					key_pressed = false;
				}
			}
		}
	}
	else
	//nie ma zmiany na klawiaturze
	{
		//czy jest naciśnięty któryś z klawiszy
		if(keys != 0)
		{
			//jeśli naciśnięto klawisz i minął odpowiednio długi czas, to wyślij do menu stan naciśnięcia ostatniego klawisza
			if(key_pressed & (key_press_delay-- == 0))
			{
				//wyślij kontynuację naciśnięcia klawisza
				menu_event(menu_event_key_press, &last_key_pressed);

				key_press_delay = 12;
			}
		}
	}

	previous_keys = keys;
}



static void key_activation_timeout(void)
{
	key_activation = false;
	
	stop_time_event(key_activation_timeout_idx);
}




void init_keys(void)
{
	keys = 0;
	
	key_scan_repeat = 3;
	
	key_count_tick = 0;
	
	init_driver_keys();
	
	register_time_event(check_keys, 5, false);
	
	key_activation = false;
	
	key_activation_timeout_idx = register_time_event(key_activation_timeout, 50, true);
}


bool is_sw_1(void)
{
	if(keys & 0x01)
	{
		return true;
	}
	
	return false;
}


bool is_sw_2(void)
{
	if(keys & 0x02)
	{
		return true;
	}
	
	return false;
}


bool is_sw_3(void)
{
	if(keys & 0x04)
	{
		return true;
	}
	
	return false;
}





void keys_tick(void)
{
	if(is_sw_1())
	{
		key_count_tick++;
	}
	else
	{
		key_count_tick = 0;
	}
}


uint32_t get_keys_tick(void)
{
	return key_count_tick;
}



void keys_prepare_deep_sleep_entry(void)
{
	driver_keys_prepare_deep_sleep_entry();
}


void keys_prepare_deep_sleep_exit(void)
{
	driver_keys_prepare_deep_sleep_exit();
}



bool is_key_activation(void)
{
	return key_activation;
}



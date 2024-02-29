
#ifndef VERSION_H_
#define VERSION_H_



#define HARDWARE_VERSION	4




#define FIRMWARE_VERSION_MAJOR	0
#define FIRMWARE_VERSION_MINOR	117
#define FIRMWARE_VERSION_EXTRA	17


/*
	0.0.1
		2017-08-07
		Szkielet oprogramowania podstawowego modulu gazomierza
		
	0.1.0
		2017-08-17
		Obsługa pinów i wyświetlacza LCD
		
	0.2.0
		2017-08-18
		Obsługa timera i aktualizacja czasu
		
	0.3.0
		2017-08-21
		Utworzenie posystemu obsługującego menu
		
	0.4.0
		2017-08-21
		Utworzenie posystemu obsługującego klawisze
		
	0.5.0
		2017-08-22
		Modyfikacja obsługi LCD - nieblokujące funkcje
		
	0.6.0
		2017-08-25
		Zmiana wewnętrznego zegara na 16 MHz
		Początek implementacji obsługi czujnika ultradźwiękowego
		
	0.7.0
		2017-08-28
		Odczyt przepływu z czujnika ultradźwiękowego
		
	0.8.0
		2017-08-29
		Dodanie trybów oszczędzania energii
		
	0.9.0
		2017-08-31
		Uzupełnienie trybów oszczędzania energii
		
	0.10.0
		2017-09-27
		Usunięcie błędu w module wyświetlacza LCD
		
	0.11.0
		2017-10-03
		Usunięcie błędu powodującego czasem zawieszenie się odczytu z czujnika ultradźwiękowego
		
	0.12.0
		2017-10-06
		Początek implementacji obsługi czujnika ciśnienia
		
	0.13.0
		2017-10-12
		Pełna implementacja czujnika ciśnienia
		
	0.14.0
		2017-10-16
		Oprócz przepływu gazu podawana jest także wartość skumulowana, którą można wyzerować, oraz można przełączać się pomiędzy trybem normalnym i inspekcyjnym
		
	0.15.0
		2017-10-16
		Modyfikacja odczytu pomiaru z czujnika ciśnienia
		
	0.16.0
		2017-10-17
		Implementacja pomiaru temperatury dla czujnika ultradźwiękowego
		
	0.16.1
		2017-10-17
		Poprawa błędu, który blokował analizę danych z czujnika ultradźwiękowego
		
	0.17.0
		2017-10-23
		Co 0,3 L jest generowany impuls na pinach PA5 i PA6 
		
	0.18.0
		2018-02-08
		Wstępna implementacja dla nowej implementacji gazomierza z nowym procesorem
		
	0.19.0
		2018-02-09
		Uruchomienie obsługi wyświetlacza LCD z użyciem podsystemu HAL
		
	0.20.0
		2018-02-14
		Obsługa wyświetlania cyfr na wyświetlaczu LCD
		
	0.21.0
		2018-02-15
		Ukończenie obsługi wyświetlacza LCD
		
	0.22.0
		2018-02-16
		Implementacja czujnika ultradźwiękowego
		
	0.23.0
		2018-02-20
		Implementacja czujnika ciśnienia
		
	0.24.0
		2018-03-02
		Implementacja odczytu napięcia zasilania
		
	0.25.0
		2018-03-05
		Implementacja IrDA
		
	0.26.0
		2018-03-06
		Wstępna implementacja sterowania zaworem
		
	0.27.0
		2018-03-06
		Pełna implementacja sterowania zaworem
		
	0.28.0
		2018-03-13
		Uruchomienie kwarca zegarkowego (LPTIM) i oparcie pomiaru czasu i daty na timerze LPTIM
		
	0.29.0
		2018-03-13
		Wstępna optymalizacja energetyczna
		
	0.30.0
		2018-03-15
		Optymalizacja energetyczna z wykorzystaniem trybu 'stop mode'
		
	0.31.0
		2018-03-20
		Obsługa watchdoga
		
	0.32.0
		2018-03-26
		Optymalizacja energetyczna - moduł przetwornika ADC włączany tylko na czas pomiaru
		
	0.33.0
		2018-04-05
		Zmiana w zarządzaniu trybami poboru energii
		Umożliwienie pomiaru ciśnienia w trybie najniższego poboru mocy
		
	0.34.0
		2018-04-05
		Umożliwienie pomiaru napięcia w trybie najniższego poboru mocy
		
	0.35.0
		2018-04-12
		Dodanie opcji aktywacji/deaktywacji IrDA za pomocą kontaktronu
		
	0.36.0
		2018-04-17
		Poprawki odnośnie obliczania i wyświetlania wartości z gazomierza ultradźwiękowego
		
	0.37.0
		2018-04-18
		Usprawnienie odbioru danych z czujnika ultradźwiękowego
		"Wzmocnienie" kwarca zegarkowego
		
	0.38.0
		2018-05-07
		Początek implementacji kalibracji wewnętrznego zegara RC
		
	0.39.0
		2018-05-08
		Modyfikacja transmisji z czujnikiem ultradźwiękowym i dodanie dodatkowych statystyk dla tej transmisji
		
	0.39.1
		2018-05-09
		Usunięcie błędu w analizie ramek z czujnika ultradźwiękowego
		
	0.39.2
		2018-05-09
		Modyfikacja odbioru ramek z czujnika ultradźwiękowego
		
	0.40.0
		2018-05-10
		Dodanie obsługi wystąpienia nienormalnego pomiaru czujnika ultradźwiękowego
		
	0.41.0
		2018-05-10
		Ustawienie początkowych parametrów ciśnienia i temperatury
		
	0.42.0
		2018-05-10
		Implementacja transmisji IrDA z wykorzystaniem DMA
		
	0.43.0
		2018-05-21
		Dodanie menu inicjalizacyjnego, które miga wszystkimi segmentami wyświetlacza
		
	0.44.0
		2018-05-22
		Dodanie procedur odpowiedzialnych za obliczanie pojemności baterii
		Modyfikacja funkcjonalności wyświetlania segmentów LCD (bez cyfr)
		
	0.45.0
		2018-05-23
		Usprawnienie komunikacji z czujnikiem ultradźwiękowym w trybie niskiego poboru energii
		
	0.46.0
		2018-06-19
		Wstępna implementacja odbierania ramek z danymi na porcie IrDA z wykorzystaniem DMA
		
	0.47.0
		2018-06-21
		Modyfikacja odbioru ramek na porcie IrDA
		
	0.48.0
		2018-06-21
		Modyfikacja odbioru ramek na porcie IrDA
		Dodanie ustawiania daty i czasu przez port IrDA
		
	0.49.0
		2018-06-22
		Modyfikacja odbioru ramek na porcie IrDA
		
	0.50.0
		2018-06-25
		Dodanie odpytania o wersję
		
	0.51.0
		2018-06-25
		Dodanie odpytania o parametry i statystyki czujnika ultradźwiękowego
		
	0.52.0
		2018-06-25
		Dodanie odpytania o wartości czujnika ciśnienia
		
	0.53.0
		2018-06-25
		Dodanie odpytania o napięcie baterii
		
	0.54.0
		2018-06-25
		Sterowanie zaworem
		
	0.55.0
		2018-06-26
		Obsługa ramki pustej i odpytanie o pojemność baterii
		
	0.56.0
		2018-06-27
		Dodanie odpytania o statystyki transmisji IrDA
		
	0.57.0
		2018-06-27
		Dodanie funkcji obsługujących zapis i odczyt z pamięci EEPROM
		Dodanie ustawiania i pobierania wartości numeru seryjnego
		
	0.57.1
		2018-06-28
		Modyfikacja funkcji odpowiadających na transmisję IrDA - utworzenie bardziej zwartego kodu, bez zmiany funkcjonalności
		
	0.58.0
		2018-06-29
		Dodanie zerowania licznika i ustawiania czujnika w tryb normalny lub inspekcyjny
		
	0.59.0
		2018-06-29
		Dodanie pobierania i ustawiania okresu zapisu do archiwum
		
	0.60.0
		2018-07-11
		Pomiar napięcia na baterii zgodny z płytką nr 3
		
	0.61.0
		2018-07-11
		Pomiar prądu na zaworze zgodny z płytką nr 3
		
	0.62.0
		2018-07-11
		Dodanie obsługi SPI2
		
	0.63.0
		2018-07-11
		Obsługa IrDA zgodna z płytką nr 3
		
	0.63.1
		2018-07-13
		Usunięcie błędu powodującego niedokładny pomiar napięcia i prądu zaworu
		
	0.64.0
		2018-07-16
		Wstępna obsługa protokołu SMART-GAS
		
	0.65.0
		2018-07-16
		Obsługa zapisu numeru seryjnego poprzez SMART-GAS n IrDA
		
	0.66.0
		2018-07-17
		Wstępna obsługa błędów SMART-GAS
		
	0.67.0
		2018-07-17
		Odczyt i zapis szerokości i długości geograficznej
		
	0.68.0
		2018-07-18
		Dodanie ograniczeń na wartość szerokości i długości geograficznej (obiekt w protokole SMART-GAS dostaje odpowiedni błąd)
		
	0.69.0
		2018-07-18
		Wydzielenie kodu odpowiedzialnego za obsługę warstwy obiektowej w protokole SMART-GAS
		
	0.70.0
		2018-07-19
		Odczyt wersji sprzętowej i programowej urządzenia dla SMART-GAS
		
	0.71.0
		2018-07-19
		Dodanie obsługi następujących obiektów SMART-GAS: CLIENT_ID, GAS_METER_SERIAL_NBR, DEVICE_TYPE_NAME, STANDARD_PARAMETER_LIST_VERSION
		
	0.72.0
		2018-07-31
		Obsługa czasu UNIX
		
	0.73.0
		2018-08-01
		Pomiar czasu oparty na czas UNIX
		
	0.74.0
		2018-08-01
		Obsługa przesunięcia zegara
		
	0.75.0
		2018-08-01
		Modyfikacja funkcji ustawiających parametry w SMART-GAS
		
	0.76.0
		2018-08-01
		Obsługa obiektu TIME_ZONE_OFFSET w protokole SMART-GAS
		
	0.77.0
		2018-08-02
		Obsługa obiektów TIME_ZONE_OFFSET_WINTER i TIME_ZONE_OFFSET_SUMMER w protokole SMART-GAS
		
	0.78.0
		2018-08-02
		Obsługa obiektu CLOCK_SYNCHRONISATION_FROM_GSM w protokole SMART-GAS
		
	0.79.0
		2018-08-03
		Obsługa obiektów BATTERY_LEVEL i TEMPERATURE w protokole SMART-GAS
		
	0.80.0
		2018-08-07
		Obsługa obiektu UNITS w protokole SMART-GAS
		
	0.81.0
		2018-08-08
		Obsługa obiektu VOLUME w protokole SMART-GAS
		
	0.82.0
		2018-08-08
		Obsługa obiektu COMMAND_VOLUME_OFFSET w protokole SMART-GAS
		
	0.83.0
		2018-08-09
		Obsługa obiektu CALORIFIC_VALUE
		
	0.84.0
		2018-08-09
		Obsługa obiektu VOLUME (zapis) w protokole SMART-GAS
		
	0.85.0
		2018-08-13
		Obliczanie energii i obsługa obiektu ENERGY w protokole SMART-GAS
		
	0.86.0
		2018-08-13
		Obsługa obiektu COMMAND_ENERGY_OFFSET w protokole SMART-GAS
		
	0.87.0
		2018-08-13
		Obsługa obiektu COUNTERS_REGISTRATION_PERIOD w protokole SMART-GAS
		
	0.88.0
		2018-08-13
		Obsługa obiektów COUNTERS_ARCHIVE_LAST_TIME, COUNTERS_ARCHIVE_LAST_VOLUME i COUNTERS_ARCHIVE_LAST_ENERGY
		
	0.89.0
		2018-08-13
		Obsługa obiektu GAS_METER_Q_MAX w protokole SMART-GAS
		
	0.90.0
		2018-08-14
		Obsługa obiektu TEMPORARY_FLOW w protokole SMART-GAS
		
	0.91.0
		2018-08-14
		Refaktoryzacja kodu związanego z pomiarem przepływu, pojemności i energii oraz obsługa obiektu HOURLY_USAGE_CURRENT w protokole SMART-GAS
		
	0.92.0
		2018-08-14
		Obsługa obiektu MONTHLY_DATA_LATCH_DAY w protokole SMART-GAS
		
	0.93.0
		2018-08-16
		Obsługa obiektu MONTHLY_AND_DAILY_DATA_LATCH_HOUR w protokole SMART-GAS
		
	0.94.0
		2018-08-16
		Obsługa obiektu MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME w protokole SMART-GAS
		
	0.95.0
		2018-08-16
		Obsługa obiektu LAST_DAILY_LATCH_TIME w protokole SMART-GAS
		
	0.96.0
		2018-08-16
		Obsługa obiektu LAST_DAILY_LATCH_VOLUME i LAST_DAILY_LATCH_ENERGY w protokole SMART-GAS
		
	0.97.0
		2018-08-23
		Dodanie obsługi własnego obiektu odpowiedzialnego za kalibrację przepływu w zależności od temperatury i przepływu
		
	0.97.1
		2018-08-24
		Poprawa błędu związanego z pobieraniem z pamięci EEPROM obiektów CLIENT_ID, GAS_METER_SERIAL_NBR i DEVICE_TYPE_NAME w protokole SMART-GAS
		
	0.98.0
		2019-04-05
		Odbiór danych z huba (przepływ i temperatura czujnika ultradźwiekowego)
		
	0.99.0
		2019-04-08
		Modyfikacja odbioru danych z huba (przepływ i temperatura czujnika ultradźwiekowego)
		
	0.100.0
		2019-04-09
		Usunięcie sprzętowej obsługi ciśnieniomierza i pobieranie wartości ciśnienia (i temperatury) z koncentratora gazomierza
		
	0.101.0
		2019-04-09
		Utworzenie wielobufora nadajnika dla komunikacji z koncentratorem
		
	0.101.1
		2019-04-10
		Poprawki w nadawaniu dla komunikacji z koncentratorem
		
	0.102.0
		2019-04-11
		Przeniesienie sprzętowej obsługi zaworu do koncentratora
		
	0.102.1
		2019-04-12
		Poprawki obsługi zaworu
		
	0.103.0
		2019-04-16
		Implementacja hallotronu
		
	0.104.0
		2019-04-18
		Rezygnacja z HAL i przejście na operacje na rejestrach dla obliczania CRC
		
	0.104.1
		2019-04-24
		Modyfikacja sprzętowego obliczania CRC
		
	0.105.0
		2019-04-25
		Rezygnacja z HAL i przejście na operacje na rejestrach dla przetwornika ADC
		
	0.106.0
		2019-04-25
		Rezygnacja z HAL i przejście na operacje na rejestrach dla SPI
		
	0.107.0
		2019-05-06
		Rezygnacja z HAL i przejście na operacje na rejestrach dla LCD
		
	0.108.0
		2019-05-07
		Rezygnacja z HAL i przejście na operacje na rejestrach dla obsługi zegarów
		
	0.109.0
		2019-06-04
		Modyfikacja sprzętowej obsługi klawiatury dla nowego obwodu
		
	0.109.1
		2019-06-04
		Modyfikacja sprzętowej obsługi komunikacji z hubem dla nowego obwodu
		
	0.109.2
		2019-06-05
		Modyfikacja sprzętowej obsługi komunikacji na IrDA
		
	0.109.3
		2019-07-02
		Usunięcie błędu zawieszającego działanie po powrocie zasilania
		
	0.110.0
		2019-07-03
		Eliminacja sterowników HAL
		
	0.110.1
		2019-07-04
		Modyfikacja transmisji IrDA
		
	0.110.2
		2019-07-05
		Obsługa czujnika temperatury, ciśnienia i wilgotności
		
	0.110.3
		2019-07-09
		Implementacja obsługi pamięci FLASH
		
	0.110.4
		2019-07-11
		Implementacja sterownika obsługi portu do komunikacji z modułem komunikacyjnym
		
	0.110.5
		2019-07-15
		Implementacja nadawania ramek do modułu komunikacyjnego (radio)
		
	0.111.0
		2019-07-16
		Implementacja odbioru ramek z modułu komunikacyjnego (radio)
		
	0.111.1
		2019-07-17
		Wyodrębnienie sterownika DMA
		
	0.111.2
		2019-07-19
		Wysył ramek do modułu komunikacyjnego z wykorzystaniem DMA
		
	0.111.3
		2019-07-19
		Wysył ramek do modułu koncentratora z wykorzystaniem DMA
		
	0.111.4
		2019-07-22
		Poprawki odnośnie odbioru danych z modułu komunikacyjnego i dodanie do menu statystyk odnośnie modułu komunikacyjnego
		
	0.111.5
		2019-07-23
		Implementacja odpowiedzi na ramkę o obiekt SMART-GAS z modułu komunikacyjnego i serwera
		
	0.111.6
		2019-07-24
		Wykorzystanie sprzętowego RTC do generowania 1s przerwań
		
	0.111.7
		2019-07-25
		Wykorzystanie sprzętowego RTC do obsługi zliczania czasu i daty
		
	0.111.8
		2019-07-31
		Obsługa obiektu EVENT_LINK w protokole SMART-GAS
		
	0.111.9
		2019-08-01
		Obsługa obiektów EVENT_UDP_PORT i EVENT_TCP_PORT w protokole SMART-GAS
		
	0.111.10
		2019-08-02
		Obsługa obiektów EVENT_PHONE i EVENT_IP w protokole SMART-GAS
		
	0.111.11
		2019-08-05
		Obsługa obiektu SIM_PIN w protokole SMART-GAS
		
	0.111.12
		2019-08-06
		Obsługa obiektów SIM_ICCID, SIM_PHONE i SIM_IP w protokole SMART-GAS
		
	0.111.13
		2019-08-07
		Obsługa obiektów SIM_SMSC, SIM_SMS_VALIDITY i GPRS_APN w protokole SMART-GAS
		
	0.111.14
		2019-08-08
		Obsługa obiektów GPRS_USER, GPRS_PASSWORD i GSM_QUALITY w protokole SMART-GAS
		
	0.111.15
		2019-08-27
		Obsługa obiektów GSM_NETWORK_CODE, GSM_BTS_LAC i GSM_BTS_CELL_ID w protokole SMART-GAS
		
	0.111.16
		2019-08-28
		Obsługa obiektów MODEM_STATE, MODEM_SMS_RECEIVED i MODEM_SMS_SENT w protokole SMART-GAS
		Zmiana Software Packa Keil::STM32L0xx_DFP v2.0.1, która wymusiła zmianę nazw rejestrów GPIO
		
	0.111.17
		2019-08-29
		Obsługa obiektów MODEM_GPRS_PACKETS_RECEIVED, MODEM_GPRS_PACKETS_SENT, MODEM_WAKEUP_COUNT i MODEM_WAKEUP_DURATION w protokole SMART-GAS
		
	0.112.0
		2019-09-02
		Obsługa obiektów MKG_MODULE_TYPE, MKG_HARDWARE_VERSION, MKG_SOFTWARE_VERSION i MKG_SERIAL_NUMBER w protokole SMART-GAS
		
	0.112.1
		2019-09-04
		Obsługa obiektów TEMPORARY_FLOW_PERIOD_LATCHED i TEMPORARY_FLOW_MONTHLY_LATCHED w protokole SMART-GAS
		
	0.112.2
		2019-09-06
		Obsługa obiektów HOURLY_FLOW_PERIOD_LATCHED i HOURLY_FLOW_MONTHLY_LATCHED w protokole SMART-GAS
		
	0.112.3
		2019-09-06
		Uwzględnienie impulsów z gazomierza miechowego w obliczaniu przepływu i objętości
		
	0.112.4
		2019-09-10
		Obsługa obiektów PREPAID_TOPUP_ID i PREPAID_TOPUP_END_TIME w protokole SMART-GAS
		
	0.112.5
		2019-09-11
		Obsługa obiektów PREPAID_TOPUP_END_VOLUME, PREPAID_TOPUP_END_ENERGY i PREPAID_VALVE_CONTROL w protokole SMART-GAS
		
	0.112.6
		2019-09-16
		Obsługa obiektów WDP_ARP, WDP_AT i WDP_MR w protokole SMART-GAS
		
	0.112.7
		2019-09-17
		Obsługa obiektów WPP_EVENT_SEND_SN, WPP_IV_IN i WPP_KEY w protokole SMART-GAS
		
	0.112.8
		2019-09-25
		Obsługa obiektów TEMPORARY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED, TEMPORARY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED,
		HOURLY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED i HOURLY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED w protokole SMART-GAS
		
	0.112.9
		2019-09-30
		Usunięcie błędu w inicjalizacji obiektów SIM_SMSC i GPRS_APN
		
	0.112.10
		2019-10-02
		Wstępna obsługa odbioru obiektów z puli 0x600-0x6ff w protokole Smart-Gas
		
	0.112.11
		2019-10-03
		Obsługa odbioru obiektów z puli 0x600-0x6ff w protokole Smart-Gas
		
	0.112.12
		2019-10-04
		Obsługa odbioru obiektów z puli 0x600-0x6ff w protokole Smart-Gas
		
	0.112.13
		2019-10-09
		Zapisa do pamięci FLASH liczydeł zatrzaśniętych COUNTERS_ARCHIVE
		
	0.112.14
		2019-10-10
		Obsługa własnego obiektu PREPARE_PARAMETERS w protokole SMART-GAS
		
	0.112.15
		2019-10-15
		Dodanie zabezpieczenia, które zachowuje wartość pojemności i energii po resecie
		
	0.112.16
		2019-10-16
		Modyfikacja obiektów COUNTERS_ARCHIVE
		
	0.112.17
		2019-10-17
		Optymalizacja funkcji zapisującej do EEPROM (średnio 4 krotne przyśpieszenie)
		
	0.112.18
		2019-10-22
		Optymalizacja zapisu do pamięci FLASH
		
	0.112.19
		2019-10-23
		Wprowadzenie korekty do odmierzania krótkich przerw czasowych
		
	0.112.20
		2019-10-24
		Poprawki w obiekcie COUNTERS_ARCHIVE i wyszukiwanie obiektów COUNTERS_ARCHIVE
		
	0.112.21
		2019-10-29
		Rozszerzenie wyszukiwania obiektów COUNTERS_ARCHIVE o pamięć FLASH
		
	0.112.22
		2019-11-05
		Modyfikacja procedury odmierzającej pomiar krótkich odcinków czasowych
		
	0.112.23
		2019-11-06
		Dodanie weryfikacji zachodzenia na siebie parametrów z pamięci EEPROM
		
	0.112.24
		2019-11-07
		Modyfikacja obsługi parametrów w pamięci EEPROM
		
	0.113.0
		2019-11-12
		Wykorzystanie mechanizmów preprocesora do łatwiejszej implementacji parametrów w pamięci EEPROM

	0.113.1
		2019-11-13
		Obsługa linii EXTRST_1
		
	0.113.2
		2019-11-20
		Usprawnienie w zarządzaniu poborem energii
		
	0.113.3
		2019-11-22
		Modyfikacja obsługi wejścia w niski pobór energii
		
	0.113.4
		2019-11-26
		Usunięcie błędu w korekcji wewnętrznego zegara RC (korekcja nie działa poprawnie w trybie najniższego poboru energii)
		
	0.113.5
		2019-11-28
		Reset z klawiatury
		
	0.113.6
		2019-12-03
		Modyfikacja obsługi menu w trybie niskiego poboru energii
		
	0.114.0
		2019-12-04
		Dostosowanie komunikacji z płytką hub'a do dłuższych ramek
		
	0.114.1
		2019-12-05
		Wstępna implementacja pobierania obiektów z kalorymetru
		
	0.114.2
		2019-12-10
		Implementacja nieblokującego pobierania obiektów SmartGas
		
	0.114.3
		2019-12-11
		Uzupełnienie implementacji nieblokującego pobierania obiektów SmartGas
		
	0.114.4
		2019-12-12
		Implementacja pobierania parametrów z kalorymetra
		
	0.114.5
		2019-12-17
		Usunięcie błędu w analizie ramki od modułu komunikacyjnego
		
	0.114.6
		2019-12-18
		Budowa systemu logów
		
	0.114.7
		2019-12-18
		Modyfikacje systemu debug
		
	0.114.8
		2020-01-08
		Modyfikacja menu (uproszczenie)
		
	0.114.9
		2020-01-09
		Drobne modyfikacje i dodanie komentarzy
		
	0.114.10
		2020-01-14
		Modyfikacja aktualizacji wartości gazomierza miechowego
		
	0.114.11
		2020-01-16
		Obsługa obiektu VOLUME_MAX_VALUE
		
	0.114.12
		2020-01-21
		Modyfikacja aktualizacji wartości gazomierza miechowego
		
	0.115.0
		2020-01-22
		Zmiany w menu pokazującym wartość pomiaru przepływu i objętości
		
	0.115.1
		2020-01-24
		Modyfikacja obliczania pojemności gazomierza miechowego
		
	0.115.2
		2020-01-28
		Implementacja obiektu MKG_GSM_CLOCK
		
	0.115.3
		2020-01-29
		Uporządkowanie konwersji czasu pomiędzy UNIX a DateTime
		
	0.115.4
		2020-01-30
		Obsługa zapisu obiektu GAS_METER_Q_MAX
		
	0.115.5
		2020-02-04
		Obsługa obiektu TIME_ZONE_AUTO_OFFSET
		
	0.115.6
		2020-02-05
		Automatyczna zmiana czasu letniego/zimowego
		
	0.115.7
		2020-02-06
		Podawanie w menu wartości energii
		
	0.115.8
		2020-02-07
		Modyfikacja ustawiania i pobierania czasu lokalnego i UTC
		
	0.115.9
		2020-02-12
		Stworzenie obiektów do pobierania danych poprzez MODBUS z kalorymetru
		
	0.115.10
		2020-02-17
		Implementacja pobierania i ustawiania rejestrów MODBUS kalorymetru
		
	0.115.11
		2020-02-18
		Zmiany w obsłudze kalorymetru
		
	0.115.12
		2020-03-03
		Obsługa obiektów WPP_CRYPTOGRAPHY_OBLIGATORY i WPP_ACCESS_CONTROL_OBLIGATORY
		
	0.115.13
		2020-03-04
		Obsługa obiektów SMS_PHONE_NUMBERS_RESTRICTION i PERMITTED_PHONE_NUMBER
		
	0.116.0
		2020-03-18
		Utworzenie szkieletu procedur służących inicjalizacji parametrów domyślnych
		
	0.116.1
		2020-03-19
		Inicjalizacja parametrów domyślnych
		
	0.116.2
		2020-03-20
		Inicjalizacja parametrów domyślnych
		
	0.116.3
		2020-03-25
		Inicjalizacja parametrów domyślnych
		
	0.116.4
		2020-03-26
		Inicjalizacja parametrów domyślnych
		
	0.116.5
		2020-04-01
		Inicjalizacja parametrów domyślnych
		
	0.116.6
		2020-04-06
		Przeliczanie objętości do parametrów standardowych
		
	0.116.7
		2020-04-07
		Poprawki dla obliczania objętości
		
	0.116.8
		2020-04-08
		Zabezpieczenie wartości objętości i energii przed zanikiem zasilania
		
	0.116.9
		2020-04-15
		Zabezpieczenie wartości objętości i energii przed zanikiem zasilania
		
	0.116.10
		2020-05-25
		Wstępna implementacja logów
		
	0.116.11
		2020-05-27
		Implementacja logów
		
	0.116.12
		2020-05-29
		Implementacja logów
		
	0.116.13
		2020-06-24
		Implementacja obiektów VALVE_OPERATION, VALVE_ACTIVATION_TIMEOUT i VALVE_STATE
		
	0.116.14
		2020-06-25
		Implementacja obiektów VALVE_LEAK_TEST_DURATION i VALVE_LEAK_TEST_VOLUME
		
	0.116.15
		2020-06-26
		Implementacja obiektów VALVE_ERROR_CODE, VALVE_OPEN_COUNT i VALVE_CLOSE_COUNT
		
	0.116.16
		2020-06-29
		Inicjalizacja i przygotowanie parametrów pracy zaworu
		
	0.116.17
		2020-06-30
		Logika działania dla obiektów Smart-Gas zaworu
		
	0.116.18
		2020-07-02
		Implementacja próby szczelności zaworu
		
	0.116.19
		2020-07-08
		Początek implementacji znacznika STATUS
		
	0.116.20
		2020-07-09
		Implementacja obiektów STATUS, STATUS_PERIODIC_LATCHED i STATUS_MONTHLY_LATCHED
		
	0.116.21
		2020-07-10
		Uzupełnienie znacznika STATUS
		
	0.117.0
		2020-07-13
		Implementacja obiektów STATUS_UP_EVENT_MASK i STATUS_DOWN_EVENT_MASK
		
	0.117.1
		2020-08-13
		Początek implementacji harmonogramów
		
	0.117.2
		2020-08-17
		Implementacja harmonogramów
		
	0.117.3
		2020-08-18
		Implementacja harmonogramów
		
	0.117.4
		2020-08-20
		Obsługa dni tygodnia
		
	0.117.5
		2020-08-20
		Modyfikacja obsługi dni tygodnia
		
	0.117.6
		2020-08-24
		Implementacja harmonogramów
		
	0.117.7
		2020-08-25
		Implementacja harmonogramów
		
	0.117.8
		2020-08-26
		Implementacja harmonogramów
		
	0.117.9
		2020-08-28
		Implementacja harmonogramów
		
	0.117.10
		2020-08-31
		Implementacja harmonogramów
		
	0.117.11
		2020-09-01
		Harmonogramy: funkcja pobierająca zawartość harmonogramów
		
	0.117.12
		2020-09-03
		Harmonogramy: modyfikacji funkcji pobierających i zapisujących zawartość harmonogramów
		
	0.117.13
		2020-09-04
		Obsługa obiektu SCHEDULE_COMMAND
		
	0.117.14
		2020-09-07
		Poprawki w sterownikach odpowiedzialnych za komunikację
		
	0.117.15
		2020-09-08
		Komentarze dla sterownika komunikującego się z hubem
		
	0.117.16
		2020-09-22
		Wydłużenie czasu sygnału obecności transmisji z hubem
		
	0.117.17
		2020-09-25
		Usunięcie problemu związanego z odbieraniem danych przez hub (danie hub'owi czasu na wyjście z trybu stop)
*/

#endif /*VERSION_H_*/

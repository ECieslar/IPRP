/******************************************************************************
 *
 * \file sg_wp.h
 * \brief Definicje warstwy pakietowej (WP) protokołu SMART-GAS (IGG-0201:2018).
 *
 * \copyright ELEKTROMETAL SA (c) 2018, Wszelkie prawa zastrzeżone
 * \version $Revision: 296553 $
 * \date $Date: 2020-09-30 11:19:01 +0200 (śr.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef SG_WP_H_
#define SG_WP_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Lista typów całkowitych 8, 16 i 32 bitowych. */
#include "sg_proto.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/* Stałe ogólne i założenia. */
enum SG_WP_IMPL_GENERAL
{
    SG_WP_DATA_MAX_LEN = 1024, /* Maksymalny rozmiar (liczba oktetów) danych w pakiecie (WP). */
};

/*!
 * \brief SG WP error enumeration.
 */
enum SG_WP_ERR
{
    SG_WP_OK, /*!< No error. */
    SG_WP_ERR_BAD_INPUT_PARAM, /*!< Bad input parameter(s). */
    SG_WP_ERR_BAD_OUPUT_BUF_SIZE, /*! Output buffer size is unsufficient. */
};

/******************************************************************************
* Types and Typedefs
******************************************************************************/

/*!
 * \brief Error result code type.
 */
typedef enum SG_WP_ERR SG_WP_ERR_t;

/*
 * Warstwa pakietowa (WP) (pkt 4).
 */

/* 
 * Definicja WP_CTRL - Pole kontrolne protokołu. 
 * Struktura kontrolna WP (pkt 4.1). 
 */
typedef struct SG_WP_CTRL
{
    uint8_t ctrl_ver : 1; /* Flaga wersji protokołu.
	                       * Dla obecnej wersji protokołu zawsze 1. */
    uint8_t ctrl_sn : 1; /* Flaga numeru seryjnego.
						  * Czy nagłówek zawiera numer seryjny urządzenia WP_SN:
						  * 0 – nie; 1 – tak.
						  * Pole wymagane w przypadku transmisji technikami, w których nie można inaczej
						  * jednoznacznie zidentyfikować nadawcy, np. GPRS z wykorzystaniem publicznego
						  * adresu IP oraz w odpowiedzi na transmisję inicjowaną przez SC z CTRL_SN=1. */
    uint8_t ctrl_enc : 1; /* Flaga kryptografii.
	                       * Czy stosuje się szyfrowanie i uwierzytelnianie realizowane
						   * z wykorzystaniem algorytmu AES:
						   * 0 – nie, brak pól WP_MAC i WP_IV;
						   * 1 – tak, WP_HDR zawiera WP_MAC i WP_IV. */
    uint8_t ctrl_rad : 1; /* Flaga adresu zwrotnego.
						   * Czy przesyłany jest adres zwrotny WP_RAD:
						   * 0 – nie, odpowiedź na pakiet odesłana jest na adres zdarzeń WPP_DAD
						   * właściwy dla techniki transmisyjnej wybranej przez CTRL_RLK;
						   * 1 – tak, odpowiedź na pakiet odesłana jest na adres zwrotny WP_RAD,
						   * który jest adekwatny do techniki transmisyjnej wybranej przez CTRL_RLK.
						   * Dla transmisji z rejestratora wartość nie ma znaczenia i ustawiana jest na 0. */
    uint8_t ctrl_rlk : 2; /* Technika transmisyjna komunikacji zwrotnej.
						   * Rodzaj techniki transmisyjnej i łącza do przesłania danych z rejestratora:
						   * 0 – brak. Odpowiedź nie zostanie wysłana, ma uzasadnienie tylko dla zlecenia PUT;
						   * 1 – SMS;
						   * 2 – GPRS – UDP/IP;
						   * 3 – GPRS – TCP/IP.
						   * Odpowiedź zostanie wysłana na adres zgodnie z ustawieniem flagi CTRL_RAD.
						   * Dla transmisji z rejestratora wynosi 0.
						   * Dla transmisji do rejestratora istotne jest dla transmisji GSM, dla innych technik
						   * odpowiedź jest przesyłana do nadawcy tę samą techniką, którą otrzymane zostało zapytanie. */
    uint8_t ctrl_dir : 1; /* Flaga kierunku komunikacji.
	                       * W którym kierunku jest wysłany pakiet WP:
						   * 0 – z rejestratora;
						   * 1 – do rejestratora. */
    uint8_t ctrl_acc : 1; /* Flaga kontroli dostępu.
	                       * Czy nagłówek zawiera pola kontroli dostępu:
						   * 0 – nie, brak pól WP_ACC_LVL i WP_ACC_PASS;
						   * 1 – tak, WP_HDR zawiera WP_ACC_LVL i WP_ACC_PASS. */
} SG_WP_CTRL_t;

/* Definicja nagłówka (pkt 4.1). */
typedef struct SG_WP_HDR
{
    SG_WP_CTRL_t crtl; /* Pole kontrolne protokołu. 
						* Struktura kontrolna WP. */
    uint8_t padding_1; /* _field for padding_ */
    uint16_t wp_len; /* Długość pakietu. 
					  * Długość pakietu wyrażona w bajtach i obejmująca wszystkie pola za polem WP_LEN, poza WP_CRC. */
    uint32_t wp_sn; /* Numer seryjny rejestratora. 
					 * Określa numer seryjny rejestratora przy transmisji w obu kierunkach – z i do rejestratora.
					 * Numery seryjne są unikalne w skali globalnej. Producent jest ograniczony do korzystania z przydzielonych puli numerów seryjnych.
					 * Pojedyncza pula adresów określona jest przez wartość najbardziej znaczącego bajtu numeru seryjnego.
					 * Pulami zawiaduje Izba Gospodarcza Gazownictwa i na wniosek przydziela pulę adresów.
					 * Dotychczas przydzielone pule prezentowane są na stronie internetowej IGG (https://www.igg.pl/kst-rozne).
					 * W ramach przydzielonej puli producent ma swobodę wykorzystywania numeru seryjnego,
					 * w tym ma możliwość kodowania dodatkowych informacji za pomocą numeru seryjnego. */
    uint32_t wp_mac; /* Ciąg uwierzytelniający. 
	                  * Podpis cyfrowy pakietu. Zgodność wartości pola WP_MAC
					  * z wartością wyznaczoną po stronie odbiorczej potwierdza, że pakiet został
					  * skonstruowany przez nadawcę znającego klucz tajny – tym samym uwierzytelnia pakiet. */
    uint32_t wp_iv; /* Wektor inicjalizacyjny. 
					 * Wartość inicjalizująca dla procedury szyfrowania i uwierzytelniania pakietu.
					 * Odpowiadające zmienne dla każdego kierunku transmisji w rejestratorze 
					 * i SC są zwiększane podczas wysłania lub odbioru pakietu. */
    uint8_t wp_rad[8]; /*!< \brief Adres zwrotny.
						*   \details Adres związany z łączem komunikacyjnym wybranym w CTRL_RLK.
						* W zależności od numeru łącza są to:
						* 1. Dla techniki SMS pole jest 8-bajtowe, a wartość jest numerem telefonu
						* zapisanym w formacie BCD (z numerem kierunkowym kraju, bez zer wiodących i znaku plus).
						* Dla nr. telefonu +48601234567 pole ma wartość: 48601234567FFFFF
						* 2. Dla techniki GPRS – UDP/IP pole jest 6-bajtowe, a wartość jest adresem IP (4B)
						* i następującym po nim numerem portu UDP (2B).
						* 3. Dla techniki GPRS – TCP/IP pole jest 6-bajtowe, a wartość jest adresem IP (4B)
						* i następującym po nim numerem portu TCP (2B).
						*/
    uint8_t wp_seso : 1; /*!< \brief Inicjator sesji komunikacji.
						  *    \details Strona inicjująca sesję komunikacyjną:
						  *    0 – rejestrator, zdarzenie; 1 – SC. */
    uint8_t wp_sesid : 7; /*!< \brief Identyfikator sesji komunikacji.
	                      * \details Wartość nadawana przez SC lub rejestrator identyfikująca sesję.
						  * Odpowiedź na pakiet z SC zawiera tę samą wartość identyfikatora pozwalającą na kojarzenie odpowiedzi
						  * z wysłanym pakietem. Dla komunikacji inicjowanej z rejestratora identyfikator stanowi numer sekwencyjny
						  * inkrementowany przy każdym wysłaniu pakietu inicjowanym przez urządzenie bez zapytania z SC. Przyjmuje wartości 0-127. */
    uint8_t wp_acclvl; /* Poziom dostępu. 
						* Wartość poziomu dostępu. 
						* Wartość 0 oznacza najniższy poziom dostępu (o najniższych uprawnieniach),
						* wartość 15 oznacza najwyższy poziom dostępu. Wskazanie poziomu dostępu umożliwia dostęp do zleceń na obiektach,
						* których wykonanie jest chronione określeniem minimalnego poziomu dostępu. Niewskazanie poziomu dostępu do zlecenia
						* dla obiektu odznacza brak ograniczeń w dostępie, wartość 15 oznacza dostęp producencki. Pole występuje tylko gdy CTRL_ACC=1. */
    uint8_t padding_2[2]; /* _field for padding_ */
    uint32_t wp_accpass; /* Hasło poziomu dostępu. 
						  * Hasło weryfikujące uprawnienie do korzystania ze wskazanego poziomu dostępu.
						  * Hasło jest liczbą 4-bajtową. Pole występuje tylko gdy CTRL_ACC=1. */
} SG_WP_HDR_t;

/* Definicja jednostki pakietu (pkt 4.1). */
typedef struct SG_WP_PDU
{
    SG_WP_HDR_t header; /* Nagłówek. */
    uint16_t data_len; /* Liczba bajtów danych wksanych w data_ptr. Nie powinna przekraczać limitu SG_WP_DATA_MAX_LEN. */
    uint8_t* data_ptr; /* (WP_DATA) Wskaźnik na dane. */
    uint16_t crc; /* WP_CRC Cyclic Redundancy Check 
				   * 2-bajtowa funkcja skrótu liczona po zakończeniu szyfrowania
				   * dla całego pakietu obejmującego pola WP_HDR i WP_DATA według wielomianu CRC-CCITT (0xFFFF).
				   * Przykładowy pakiet z CRC: 8603008004009D4E. */
} SG_WP_PDU_t;

/* Parametry warstwy pakietowej (pkt 4.3). */
typedef struct SG_WP_Params
{
    /* Tworzenie i weryfikacja pakietu. */
    uint32_t wpp_iv_in; /* Bieżąca wartość IV dla pakietów przychodzących. 
						 * Ustawiana na WP_IV po każdym poprawnie odebranym pakiecie.
						 * Zmienna zapewniająca niepowtarzalność pakietów przychodzących. */
    uint32_t wpp_iv_out; /* Bieżąca wartość IV dla pakietów wychodzących. 
						  * Inkrementowana po każdym wysłanym pakiecie.
						  * Zmienna zapewniająca niepowtarzalność pakietów wychodzących. */

    /* Cykliczne numery sesji. */
    uint8_t wpp_sesid_out : 7; /*!< \brief Bieżący identyfikator sesji rejestratora.
							    *   \brief Numer ostatniego zlecenia nadanego przez daną stronę.
							    * Inkrementowany cyklicznie przed wysłaniem nowego zlecenia.
							    * Wartości: 1-0x7F. Błąd: powinno być od zera! */

    /* Stałe kryptograficzne i domyślne ustawienia komunikacji. */
    uint8_t wpp_key[16]; /* Klucz szyfrujący. 
						  * Klucz symetryczny 128-bitowy wykorzystywany do szyfrowania i po przekształceniu
						  * do uwierzytelniania pakietu z wykorzystaniem algorytmu AES. */
    uint8_t wpp_dad[8]; /*!< \brief Domyślny adres odpowiedzi.
						 *   \details Adres na jaki wysyłane są z rejestratora zdarzenia, pakiety wynikające
						 * z funkcjonowania harmonogramu lub odpowiedź na sesję inicjowaną z SC, 
						 * jeśli flaga CTRL_RAD=0. Adresy zależne od techniki ustawiane są parametrami 
						 * określonymi w podrozdziale „Konfiguracja zdarzeń”.
						 * Wartość: Konfigurowalna w ramach skojarzenia z określonym SC.
						 */
    uint8_t wpp_dlk : 2; /*!< \brief Domyślne łącze odpowiedzi.
						  *   \details Domyślna technika transmisyjna dla zdarzeń z rejestratora określona
                          * parametrem EVENT_LINK.
                          * Wartości:
						  * 0 – odpowiedzi GSM zablokowane,
						  * 1 – SMS,
						  * 2 – GPRS – UDP/IP,
						  * 3 – GPRS – TCP/IP
						  */
} SG_WP_Params_t;

/******************************************************************************
* Global Variables
******************************************************************************/

/*!
 * \brief The SMART-GAS packet layer's parameters.
 */
extern SG_WP_Params_t sg_wp_params;

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*!
 * \brief Creates a new session.
 *
 * \details The global \param sg_wp_prams.
 */
void create_new_session( void );

/*!
 * \brief Gets the packet's session id.
 *
 * \return The packet session's id value in range from 0 to 127.
 */
uint8_t get_packet_session_id();

/*!
 * \brief Gets size of the packet's header.
 *
 * \param [in] hdr The packet's header.
 *
 * \return Size of the packet's header.
 */
uint8_t get_packet_header_size( const SG_WP_HDR_t* hdr );

/*!
 * \brief Gets the full size of the packet.
 *
 * \param [in] pkt The packet.
 *
 * \return Size of the packet.
 */
uint16_t get_packet_size( const SG_WP_PDU_t* pkt );

/*!
 * \brief Serializes the packet to the \param buff table.
 *
 * \details The output buffer is filled with aproperate packet structure's data
 * due to flags features. The CRC value isn't set.
 *
 * \param [in]  pkt The packet structure.
 * \param [out] buff The output data buffer.
 * \param [in]  buff_size The maximum output data buffer's length.
 * \param [out] pkt_size The size of serialized packet.
 *
 * \return The serialization result: SG_OK if successful; SG_WP_ERR code otherwise.
 */
SG_WP_ERR_t serialize_packet( const SG_WP_PDU_t* pkt, uint8_t* buff,
                              const uint16_t buff_size, uint16_t* pkt_size );

/*!
 * \brief Serializes the packet to the \param buff table.
 *
 * \note The packet buffer has to have assigned proper size for data
 * to successful transfer input data to packet.
 *
 * \param buff input data buffer with pakcket
 * \param buff_len input data buffer length
 * \param pkt output (deserialized) packet
 *
 * \returnThe serialization result: SG_OK if successful; SG_WP_ERR code otherwise.
 */
SG_WP_ERR_t deserialize_packet( const uint8_t* buff, const uint16_t buff_len,
                                SG_WP_PDU_t* pkt );

/*!
 * \brief Encrypt the packet \param pkt with the key \param key.
 */
SG_WP_ERR_t encrypt_packet( SG_WP_PDU_t* pkt, const uint8_t* key );

#endif /* SG_WP_H_ */

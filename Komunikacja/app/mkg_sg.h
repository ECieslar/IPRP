/*!****************************************************************************
 *
 * \file mkg_sw.h
 * \brief The MKG module's software definitions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef MKG_SG_H_
#define MKG_SG_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Declarations of integer types with specified width. */
#include <stdbool.h> /* bool decalration */
#include "../sg/sg_obj.h" /* SMART-GAS protocol. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*!
 * \brief MKG module's SMART-GAS object identifiers.
 */
enum MKG_SG_Obj_ID
{
    MKG_SG_OBJ_MOD_ID = SG_OBJ_POOL_4_MFC_LOW, /*!< Module identifier. */
    MKG_SG_OBJ_HW_VER = SG_OBJ_POOL_4_MFC_LOW + 1U, /*!< Hardware version. */
    MKG_SG_OBJ_SW_VER = SG_OBJ_POOL_4_MFC_LOW + 2U, /*!< Software version. */
    MKG_SG_OBJ_SN = SG_OBJ_POOL_4_MFC_LOW + 3U, /*!< Serial number. */
    MKG_SG_OBJ_BYE = SG_OBJ_POOL_4_MFC_LOW + 4U, /*!< BYE. */
    MKG_SG_OBJ_GSM_CLOCK = SG_OBJ_POOL_5_MFC_LOW, /*!< GSM clock. */
    MKG_SG_OBJ_SC_TIMEOUT = SG_OBJ_POOL_1_MFC_LOW, /*!< Waiting time for SC activity. */
};

/*! \brief MKG operational error codes. */
enum MKG_status
{
    MKG_OK, /*!< Successful operation. */
    MKG_INVALID_INPUT_PARAMETER_ERROR, /*!< Invalid parameter. */
    MKG_READ_DENIED_ERROR, /*!< Read denied. */
    MKG_WRITE_DENIED_ERROR, /*!< Write denied. */
    MKG_INSUFFICIENT_RIGHTS, /*!< Insufficient parameter. */
    MKG_TOO_LARGE_OBJECT_DATA, /*!< Too large object data. */
    MKG_UNSUPPORTED_OBJ_ID_ERROR, /*!< Unsupported object id. */
};

/******************************************************************************
 * Types and Typedefs
 ******************************************************************************/

/*! \brief MKG operational status type. */
typedef enum MKG_status MKG_status_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/*! \brief Time of waiting for data from SC (in seconds; 0 - unlimited). */
uint8_t wait_for_cs_time_limit;

/* The MKG data supported in SMART-GAS. */
struct modem_mfc_data_
{
    const uint8_t id[4]; /* MKG_SG_OBJ_MOD_ID (0x480) - Identyfikator modułu ASCII (max. 4B). */
    const uint8_t hw_ver[4]; /* MKG_SG_OBJ_HW_VER */
    const uint8_t sw_ver[4]; /* MKG_SG_OBJ_SW_VER */
    uint8_t sn[4]; /* MKG_SG_OBJ_SN */
    const uint8_t bye[4]; /* MKG_SG_OBJ_BYE */
} modem_mfc_data;

/* The SIM data required by SMART-GAS objects. */
struct modem_sim_data_
{
    uint32_t ip; /* SIM_IP (0x419) - Prywatny adres IP karty SIM, na kolejnych bajtach kolejne pola adresu IP (4B). */
    uint8_t sms_validity; /* SIM_SMS_VALIDITY (0x109) - Czas ważności wysyłanej wiadomości SMS
     (0x0B - 1h, 0x47 - 6h, 0x8F - 12h, 0xA7 - 24h, 0xA8 - 48h, 0xA9 - 72h, 0xAD - 7dni, 0xFF - max) (1B). */
    char pin[8 + 1]; /* SIM_PIN (0x507) - PIN do karty SIM, w formacie ASCII (max. 8B). */
    uint8_t iccid[11]; /* SIM_ICCID (0x508) - Numer seryjny karty SIM w formacie BCD (11B). */
    uint8_t phone[8]; /* SIM_PHONE (0x509) - Numer telefonu kodowany jest w formacie BCD
     (z numerem kierunkowym kraju, bez zer wiodących i znaku plus).
     Dla nr-u telefonu +48601234567 pole ma wartość: 48601234567FFFFF (8B). */
    char smsc[16 + 1]; /* SIM_SMSC (0x50A) - Numer centrum wiadomości (SMSC), w formacie ASCII (max. 16B). */
    char gprs_apn[32 + 1]; /* GPRS_APN (0x50B) - Adres APN prywatnego lub publicznego np. erainternet.pl, w formacie ASCII. (maks. 32B). */
    char gprs_user[16 + 1]; /* GPRS_USER (0x50C) - Opis  Użytkownik APN, w formacie ASCII (max. 16B). */
    char gprs_password[16 + 1]; /* GPRS_PASSWORD (0x50D) - Hasło do APN, w formacie ASCII (max. 16B). */
} modem_sim_data;

/* The protocol settings required by SMART-GAS objects. */
struct proto_data_
{
    bool wpp_evt_snd_sn; /* WPP_EVENT_SEND_SN (0x123) - Flaga wskazująca na to, czy w transmisji inicjowanej przez urządzenie jest wysyłany numer fabryczny urządzenia w warstwie pakietowej (CTRL_SN=1). Wartość 0 oznacza nie, wartość 1 oznacza tak. Domyślna wartość to 1 – tak. */
    uint32_t wpp_iv_in; /* WPP_IV_IN (0x426) - Największa dotychczas przyjęta wartość wektora inicjalizacyjnego. */
    uint8_t wpp_key[16]; /* WPP_KEY (0x51B) - Klucz kryptograficzny algorytmu AES-128 służący do szyfrowania, a po przekształceniu także do uwierzytelniania. */
    uint8_t wpp_access_pass[16]; /* WPP_ACCESS_PASS_TABLE (0x60D) - 16-elementowa tablica z hasłami (ACCESS_PASS) do poszczególnych poziomów dostępu. */
    bool wpp_crypt_oblig; /* WPP_CRYPTOGRAPHY_OBLIGATORY (0x11C) - Flaga wskazująca czy stosowanie kryptografii w komunikacji do urządzenia jest obowiązkowe. Wartość 1 oznacza obowiązkowe, wartość 0 oznacza nieobowiązkowe. Gdy flaga jest ustawiona, to pakiety bez kryptografii nie są przyjmowane poza pakietami odczytującymi tylko trzy OBJ_ID: SERIAL_NBR, WPP_CRYPTOGRAPHY_OBLIGATORY oraz WPP_IV_IN. W technice SMS i włączonej fladze odczyt ww. obiektów bez kryptografii jest możliwy tylko wtedy, gdy jest włączone filtrowanie numerów telefonów SMS_PHONE_NUMBERS_RESTRICTION. */
    bool wpp_ac_oblig; /* WPP_ACCESS_CONTROL_OBLIGATORY (0x11D) - Flaga wskazująca czy stosowanie mechanizm ograniczania dostępu do obiektów jest aktywny. Wartość 1 oznacza że jest aktywny i obowiązkowe jest autoryzowanie poziomu dostępu hasłem, wartość 0 oznacza nieobowiązkowe. Gdy flaga jest ustawiona, to odwołanie do obiektów wymagających praw dostępu bez wykazania praw dostępu skończy się niepowodzeniem. */
    bool sms_senders_filtr; /* SMS_PHONE_NUMBERS_RESTRICTION (0x125) - Filtrowanie SMS-ów w oparciu o listę dozwolonych numerów telefonów. 0 – wyłączona obsługa (domyślnie), 1 – obsługa włączona. Włączenie filtrowania powoduje, że rejestrator przyjmuje SMS-y tylko z numeru EVENT_PHONE oraz numerów wymienionych na liście SMS_PHONE_NUMBERS_TABLE, SMS-y odebrane z innych numerów odrzuca. Wyłączenie filtrowania powoduje, że w technice SMS nie są wysyłane potwierdzenia datagramów żądane przez SC. */
    uint8_t sms_senders_tab[3][8]; /* SMS_PHONE_NUMBERS_TABLE (0x60F) - Minimum 8-elementowa tablica z zawierająca listę telefonicznych numerów zastrzeżonych. Jest zbudowana tylko z jednego obiektu PERMITTED_PHONE_NUMBER. Obiekt musi wspierać przy odczycie i zapisie co najmniej parametr zlecenia TBL_ACC_IDX (zakres indeksów). */
} proto_data;


/* The event data required by SMART-GAS objects. */
struct modem_event_data_
{
    uint8_t link; /* SG_OBJ_ID_EVENT_LINK (0x10C) - Domyślne łącze przesyłania zdarzeń:
     0 - przesyłanie zdarzeń zabronione, 1 - SMS, 2 - GPRS (UDP/IP), 3 - GPRS (TCP/IP), ... (1B) */
    uint32_t ip; /* SG_OBJ_ID_EVENT_IP (0x41D) - Numer IP, na który przesyłane są pakiety gdy ma być wykorzystana technika GPRS –
     zdarzenie gdy EVENT_LINK = 2 lub 3 (GPRS) lub odpowiedź gdy CTRL_RLK = 2 lub 3
     (GPRS). Na kolejnych bajtach umieszczane są kolejne pola adresu IP, przykładowo adres
     127.0.0.1 kodowany jest do wartości 0x7F000001. (4B) */
    uint16_t udp_port; /* SG_OBJ_ID_EVENT_UDP_PORT (0x215) - Numer portu UDP, na który przesyłane są pakiety gdy ma być wykorzystana technika
     UDP/IP – EVENT_LINK (zdarzenie) lub CTRL_RLK (odpowiedź) = 2 (GPRS UDP/IP) (2B) */
    uint16_t tcp_port; /* SG_OBJ_ID_EVENT_TCP_PORT (0x216) - Numer portu TCP, na który przesyłane są pakiety gdy ma być wykorzystana technika
     UDP/IP – EVENT_LINK (zdarzenie) lub CTRL_RLK (odpowiedź) = 3 (GPRS TCP/IP)*/
    uint8_t phone[8]; /* SG_OBJ_ID_EVENT_PHONE (0x50F) - Numer telefonu, na który przesyłane są pakiety,
     gdy ma być wykorzystana technika SMS – zdarzenie gdy EVENT_LINK = 1 (SMS) lub
     odpowiedź gdy CTRL_RLK = 1 (SMS). Numer telefon kodowany jest w formacie BCD
     (z numerem kierunkowym kraju, bez zer wiodących i znaku plus).
     Dla nr. telefonu +48601234567 pole ma wartość: 48601234567FFFFF (8B) */
} modem_event_data;

/* The statistical data required by SMART-GAS objects. */
struct modem_usage_stats_data_
{
    uint16_t sms_recv; /* MODEM_SMS_RECEIVED (0x210) - Liczba otrzymanych SMS - modulo 0x10000 (2B). */
    uint16_t sms_send; /* MODEM_SMS_SENT (0x211) - Liczba wysłanych SMS - modulo 0x10000 (2B). */
    uint16_t gprs_recv; /* MODEM_GPRS_PACKETS_RECEIVED (0x212) - Liczba otrzymanych pakietów GPRS - modulo 0x10000 (2B). */
    uint16_t gprs_send; /* MODEM_GPRS_PACKETS_SENT (0x213) - Liczba wysłanych pakietów GPRS - modulo 0x10000 (2B). */
    uint16_t wakeup_count; /* MODEM_WAKEUP_COUNT (0x214) - Liczba wybudzeń modemu (2B). */
    uint32_t wakeup_duration; /* MODEM_WAKEUP_DURATION (0x41C) - Łączny czas trwania modemu w stanie wybudzenia, w sekundach (4B). */
} modem_usage_stats_data;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * \brief Get the MKG module's SMART-GAS object.
 *
 * \param[in] obj_id SMART_GAS object id
 * \param[in/out] obj SMART-GAS object with MKG data.
 *
 * return the status of operation
 */
MKG_status_t sg_get_mkg_object( const uint16_t obj_id, SG_WO_OBJ_t *obj );

/*!
 * \brief Set the MKG module's SMART-GAS object.
 *
 * \param[in] data the string with data with SMART-GAS object
 * \param[in/out] obj SMART-GAS object to set the data
 *
 * return the status of operation
 */
MKG_status_t sg_set_mkg_object( SG_WO_OBJ_t *obj );

/*! \brief Get the MKG object name.
 *
 * \param id the MKG object's id
 * \return the name of the object.
 */
const char* sg_get_mkg_obj_id_name( enum MKG_SG_Obj_ID id );

#endif /* MKG_SW_H_ */

/*** end of file ***/

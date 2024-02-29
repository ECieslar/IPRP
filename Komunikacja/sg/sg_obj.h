/******************************************************************************
 *
 * \file sg_obj.h
 * \brief The SMART-GAS (IGG-0201:2018) object definitions.
 *
 * \copyright ELEKTROMETAL SA (c) 2018, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef SG_OBJ_H_
#define SG_OBJ_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Lista typów całkowitych 8, 16 i 32 bitowych. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*!
 * \brief Stałe ogólne i założenia dot. obiektów.
 */
enum SG_OBJ_GENERAL
{
    /*! \brief Wersja protokołu SMART-GAS (rok
     *         lub rok i miesiąc w przypadku częstszych edycji).
     */
    SG_PROTO_OBJ_VER = 2018,

    /*!
     * \brief Maksymalna liczba parametrów zlecenia.
     */
    SG_OBJ_PAR_MAX_LEN = 2,

    /*!
     * \brief Maksymalny rozmiar (liczba bajtów) szczegółów zlecenia.
     */
    SG_OBJ_PAR_MAX_DETAILS = 8,

    /*!
     * \brief Maksymalny rozmiar (liczba bajtów) zlecenia.
     */
    SG_OBJ_PAR_RAW_MAX_LEN = 32U,

    /*!
     * \brief Maksymalny rozmiar danych (liczba bajtów) obiektu.
     */
    SG_OBJ_DATA_MAX_LEN = 32U,

    /*!
     * \brief Maksymalna liczba obiektów w WO.
     */
    SG_WO_OBJ_MAX_COUNT = 20,
};

/*!
 * \brief Kierunek przekazywania obiektów.
 */
enum WO_REQ_DIR
{
    /*! \brief Obiekt przekazywany z gazomierza do SC. */
    WO_REQ_DIR_FROM_GM_TO_SC,

    /*! \brief Obiekt przekazywany z SC do gazomierza. */
    WO_REQ_DIR_FROM_SC_TO_GM,

    /*! \brief Obiekt przekazywany z modułu komunikacyjnego do gazomierza. */
    WO_REQ_DIR_FROM_MKG_TO_GM
};

/*!
 * \brief Flaga błędu zlecenia (OBJ_ERR).
 */
enum SG_OBJ_STATUS
{
    /*! \brief Obiekt bez błędu. */
    SG_OBJ_OK,

    /*! \brief Obiekt wskazujący błąd w polu danych. */
    SG_OBJ_ERROR
};

/*!
 * \brief Status operacji na obiekcie.
 */
enum SG_OBJ_OP_STATUS
{
    /*! \brief Operacja przebiegła pomyślnie. */
    SG_OBJ_OP_OK,

    /*!< Invalid input parameters. */
    SG_OBJ_OP_ERROR_INVALID_INPUT_PARAMTERS,

    /*!< Input data too short to decode an object. */
    SG_OBJ_OP_ERROR_INVALID_INPUT_DATA_SIZE,

    /*!< Output data too short to serialize an object. */
    SG_OBJ_OP_ERROR_INVALID_OUTPUT_DATA_SIZE,

    /*! \brief Przekroczenie wielkości listy obiektów. */
    SG_OBJ_OP_ERROR_OBJECT_LIST_LIMIT_EXCEEDED,

    /*! \brief Typ obiektu nieobsługiwany. */
    SG_OBJ_OP_ERROR_OBJECT_TYPE_UNSUPPORTED,
};

/*!
 * \brief Status (6.11).
 *
 * \details Status jest 32-bitowym polem ze znacznikami zajścia pewnego zjawiska
 * w rejestratorze. Zjawisko to może wyzwolić przesłanie zdarzenia do systemu.
 */
enum SG_STATUS_FLAG
{
    SG_FLAG_ERROR_SIM_CARD = 0, /* Błąd karty SIM, nie może być aktywowana. */
    SG_FLAG_ERROR_MODEM = 1, /* Błąd modemu, nie może łączyć się z siecią. */
    SG_FLAG_MAX_HOUR_FLOW = 2, /* Przekroczony maksymalny przepływ godzinowy. */
    SG_FLAG_MAX_TEMPORAL_FLOW = 3, /* Przekroczony maksymalny przepływ chwilowy. */
    SG_FLAG_TAMPER_MAGNETIC = 4, /* Wykryta ingerencja polem magnetycznym. */
    SG_FLAG_TAMPER_REMOVE = 5, /* Wykryta ingerencja polegająca na odłączeniu urządzenia od gazomierza. */
    SG_FLAG_TAMPER_OPEN = 6, /* Wykryta ingerencja polegająca na otwarciu obudowy urządzenia. */
    SG_FLAG_IR_FLAG = 7, /* Opto-port aktywowany. */
    SG_FLAG_BATTERY_LOW = 8, /* Niski poziom baterii (np. poniżej 10%). */
    SG_FLAG_LOW_VOLTAGE = 9, /* Spadek napięcia poniżej minimum. */
    SG_FLAG_BUTTON = 10, /* Stan przycisku urządzenia. */
    SG_FLAG_VALVE_ERROR = 11, /* Błąd sterowania zaworem. */
    SG_FLAG_VALVE_OPEN_ACTIVATION = 12, /* Użytkownik aktywował otwarcie zaworu. */
    SG_FLAG_VALVE_LEAK_TEST_FAILED = 13, /* Nie powiodła się próba szczelności. */
    SG_FLAG_VALVE_CLOSE_FAILED = 14, /* Nieudane domknięcie zaworu. */
    SG_FLAG_TOPUP_END = 15, /* Doładowanie wyczerpane – koniec wykupionego w ramach pakietu
     gazu lub koniec czasu. */
    SG_FLAG_LARGE_CLOCK_SET = 16, /* Istotna zmiana czasu urządzenia. */
    SG_FLAG_VOLUME_SET = 17, /* Nastąpiło przestawienie liczydła objętości. */
    SG_FLAG_ENERGY_SET = 18, /* Nastąpiło przestawienie liczydła energii. */
    SG_FLAG_INSTALLATION = 19, /* Wykonanie montażu lub demontażu. */
    SG_FLAG_RESERVED_FOR_FUTURE = 20, /* 20..26 - do przyszłego wykorzystania */
    SG_FLAG_DISCONTINUITY = 27, /* Nieciągłość rejestracji - w ramach struktur oznacza,
     że pierwszy przyrost objętości lub energii w takiej
     strukturze jest niepewny. */
    SG_FLAG_RESERVED_FOR_MANUFACTURER = 28, /* 28..31 - zarezerwowane do swobodnego wykorzystania przez producentów */
};

/*!
 * \brief Kody błędów (pkt 5.4).
 *
 * \details W przypadku wystąpienia błędu (OBJ_ERR=1) pole OBJ_LEN nie występuje,
 * a pole OBJ_DATA mające w tym przypadku stałą długość 2B określa rodzaj błędu.
 */
enum SG_OBJD_ERR
{
    SG_OBJD_ERR_NOT_FOUND = 0x8001, /* Obiekt nie został odnaleziony. */
    SG_OBJD_ERR_READ_DENIED = 0x8002, /* Odczyt wartości obiektu (wykonanie zlecenia GET) jest niemożliwy. */
    SG_OBJD_ERR_WRITE_DENIED = 0x8003, /* Zapis wartości obiektu (wykonanie zlecenia PUT) jest niemożliwy. */
    SG_OBJD_ERR_WRONG_SIZE = 0x8004, /* Niedozwolona długość obiektu – za krótka lub za długa. Błąd występuje tylko dla OBJ_ID wymagających pola długości. */
    SG_OBJD_ERR_WRONG_VALUE = 0x8005, /* Wartość obiektu spoza dozwolonego zakresu (przy zleceniu PUT). */
    SG_OBJD_ERR_UNEXPECTED_END = 0x8006, /* Pakiet się skończył, a obiekt wskazuje na to, że oczekiwane są dane związane z obiektem. */
    SG_OBJD_ERR_TOO_LARGE = 0x8007, /* Odpowiedź na zlecenie nie zmieściła się w pakiecie. */
    SG_OBJD_ERR_TBL_OUT_OF_RANGE = 0x8008, /* Próba operacji tablicowej z nieprawidłowymi indeksami – indeks poza zakresem. */
    SG_OBJD_ERR_PARAM_UNSUPPORTED = 0x8009, /* Nieobsługiwany typ parametru zlecenia (OBJ_PAR_TYPE) w kontekście danego obiektu. */
    SG_OBJD_ERR_PARAM_ERROR = 0x800A, /* Nieznany typ parametru zlecenia (OBJ_PAR_TYPE). */
    SG_OBJD_ERR_TBL_COL_ERROR = 0x800B, /* Wskazanie kolumny niezdefiniowanej w obiekcie tablicowym. */
    SG_OBJD_ERR_ACCLVL_ERROR = 0x800C, /* Brak dostępu do wykonania zlecenia na obiekcie ze względu na zbyt niski poziom dostępu (WP_ACCLVL). */
    SG_OBJD_ERR_ACCPASS_ERROR = 0x800D, /* Brak dostępu do wykonania zlecenia na obiekcie ze względu na nieprawidłowe hasło poziomu dostępu (WP_ACCPASS). */
    SG_OBJD_ERR_NOT_INITIALISED = 0x800E, /* Błąd odczytu wartości, obiekt niezainicjalizowany. */
    SG_OBJD_ERR_HARDWARE = 0x8100, /* Błąd sprzętowy wykonania zlecenia zdefiniowany przez producenta (kod początkowy). */
    SG_OBJD_ERR_HARDWARE_END = 0x81FF, /* Błąd sprzętowy wykonania zlecenia zdefiniowany przez producenta (kod końcowy). */
    SG_OBJD_ERR_FIRMWARE = 0x8200, /* Błąd programowy wykonania zlecenia zdefiniowany przez producenta (wykorzystanie np. w sytuacji gdy program znalazł się w miejscu, w którym teoretycznie nie powinien się znaleźć). */
    SG_OBJD_ERR_FIRMWARE_END = 0x82FF, /* Błąd programowy wykonania zlecenia zdefiniowany przez producenta (kod końcowy). */
    SG_OBJD_ERR_UNKNOWN = 0x8FFF, /* Błąd niezdefiniowany. */
};

/*!
 * \brief Identyfikatory pól obiektów (por. 5.3  Identyfikator obiektu OBJ_ID).
 */
enum SG_OBJ_ID_POOL
{
    SG_OBJ_POOL_1 = 0x1U, /* Pula 1 */
    SG_OBJ_POOL_2 = 0x2U, /* Pula 2 */
    SG_OBJ_POOL_3 = 0x3U, /* Pula 3 */
    SG_OBJ_POOL_4 = 0x4U, /*  Pula 4. */
    SG_OBJ_POOL_5 = 0x5U, /*  Pula 5. */
    SG_OBJ_POOL_6 = 0x6U, /*  Pula 6. */
    SG_OBJ_POOL_7 = 0x7U, /* Puli 7 (zarezerwowana). */
    SG_OBJ_POOL_8 = 0x8U, /* Puli 8 (zarezerwowana). */
    SG_OBJ_POOL_9 = 0x9U, /* Pula 9. */
    SG_OBJ_POOL_10 = 0xAU, /*  Pula 10. */
    SG_OBJ_POOL_11 = 0xBU, /*  Pula 11 (zarezerwowana). */
    SG_OBJ_POOL_12 = 0xCU, /*  Pula 12. */
    SG_OBJ_POOL_13 = 0xDU, /*  Pula 13. */
    SG_OBJ_POOL_14 = 0xEU, /* Pula 14 (zarezerwowana). */
    SG_OBJ_POOL_15 = 0xFU, /* Pula 15 (zarezerwowana). */
};

/*!
 * \brief Zakres pul obiektów (5.3  Identyfikator obiektu OBJ_ID).
 *
 * \details Identyfikatory obiektów podzielone zostały na pule, które mogą definiować długość
 * obiektu (OBJ_LEN). Część puli przeznaczona jest do wykorzystania w ramach standardu,
 * a część na parametry producenckie, które mogą rozszerzać funkcjonalność standardową.
 */
enum SG_OBJ_ID_POOL_RANGE
{
    /* Pula 1: Obiekty o długość 1B. */
    SG_OBJ_POOL_1_LOW = 0x0100, /* Obiekt 1B, zakres dolny. */
    SG_OBJ_POOL_1_HIGH = 0x01FF, /* Obiekt 1B, zakres górny. */
    SG_OBJ_POOL_1_STD_LOW = SG_OBJ_POOL_1_LOW, /* Obiekt 1B, zakres standardowy dolny. */
    SG_OBJ_POOL_1_STD_HIGH = 0x017F, /* Obiekt 1B, zakres standardowy górny. */
    SG_OBJ_POOL_1_MFC_LOW = 0x0180, /* Obiekt 1B, zakres producencki dolny. */
    SG_OBJ_POOL_1_MFC_HIGH = SG_OBJ_POOL_1_HIGH, /* Obiekt 1B, zakres producencki górny. */

    /*  Pula 2: Obiekty o długość 2B. */
    SG_OBJ_POOL_2_LOW = 0x200U, /* Obiekt 2B, zakres dolny. */
    SG_OBJ_POOL_2_HIGH = SG_OBJ_POOL_2_LOW + 0xFFU, /* Obiekt 2B, zakres górny. */
    SG_OBJ_POOL_2_STD_LOW = SG_OBJ_POOL_2_LOW, /* Obiekt 2B, zakres standardowy dolny. */
    SG_OBJ_POOL_2_STD_HIGH = SG_OBJ_POOL_2_LOW + 0x7FU, /* Obiekt 2B, zakres standardowy górny. */
    SG_OBJ_POOL_2_MFC_LOW = SG_OBJ_POOL_2_LOW + 0x80U, /* Obiekt 2B, zakres producencki dolny. */
    SG_OBJ_POOL_2_MFC_HIGH = SG_OBJ_POOL_2_HIGH, /* Obiekt 2B, zakres producencki górny. */

    /*  Pula 3: Obiekty o długość 3B. */
    SG_OBJ_POOL_3_LOW = 0x300U, /* Obiekt 3B, zakres dolny. */
    SG_OBJ_POOL_3_HIGH = SG_OBJ_POOL_3_LOW + 0xFFU, /* Obiekt 3B, zakres górny. */
    SG_OBJ_POOL_3_STD_LOW = SG_OBJ_POOL_3_LOW, /* Obiekt 3B, zakres standardowy dolny. */
    SG_OBJ_POOL_3_STD_HIGH = SG_OBJ_POOL_3_LOW + 0x7FU, /* Obiekt 3B, zakres standardowy górny. */
    SG_OBJ_POOL_3_MFC_LOW = SG_OBJ_POOL_3_LOW + 0x80U, /* Obiekt 3B, zakres producencki dolny. */
    SG_OBJ_POOL_3_MFC_HIGH = SG_OBJ_POOL_3_HIGH, /* Obiekt 3B, zakres producencki górny. */

    /*  Pula 4: Obiekty o długość 4B. */
    SG_OBJ_POOL_4_LOW = 0x400U, /* Obiekt 4B, zakres dolny. */
    SG_OBJ_POOL_4_STD_LOW = SG_OBJ_POOL_4_LOW, /* Obiekt 4B, zakres standardowy dolny. */
    SG_OBJ_POOL_4_HIGH = SG_OBJ_POOL_4_LOW + 0xFFU, /* Obiekt 4B, zakres górny. */
    SG_OBJ_POOL_4_STD_HIGH = SG_OBJ_POOL_4_LOW + 0x7FU, /* Obiekt 4B, zakres standardowy górny. */
    SG_OBJ_POOL_4_MFC_LOW = SG_OBJ_POOL_4_LOW + 0x80U, /* Obiekt 4B, zakres producencki dolny. */
    SG_OBJ_POOL_4_MFC_HIGH = SG_OBJ_POOL_4_HIGH, /* Obiekt 4B, zakres producencki górny. */

    /*  Pula 5: Długość obiektu określona polem długości OBJ_LEN o długości 1B. */
    SG_OBJ_POOL_5_LOW = 0x500U, /* Obiekt o długości od 1 do 255, zakres dolny. */
    SG_OBJ_POOL_5_HIGH = SG_OBJ_POOL_5_LOW + 0xFFU, /* Obiekt o długości danych 1B , zakres górny. */
    SG_OBJ_POOL_5_STD_LOW = SG_OBJ_POOL_5_LOW, /* Obiekt o długości danych 1B, zakres standardowy dolny. */
    SG_OBJ_POOL_5_STD_HIGH = SG_OBJ_POOL_5_LOW + 0x7FU, /* Obiekt o długości danych 1B, zakres standardowy górny. */
    SG_OBJ_POOL_5_MFC_LOW = SG_OBJ_POOL_5_LOW + 0x80U, /* Obiekt o długości danych 1B, zakres producencki dolny. */
    SG_OBJ_POOL_5_MFC_HIGH = SG_OBJ_POOL_5_HIGH, /* Obiekt o długości danych 1B, zakres producencki górny. */

    /*  Pula 6: Długość obiektu określona polem długości OBJ_LEN o długości 2B. */
    SG_OBJ_POOL_6_LOW = 0x600U, /* Obiekt o długości danych 2B, zakres dolny. */
    SG_OBJ_POOL_6_HIGH = SG_OBJ_POOL_6_LOW + 0xFFU, /* Obiekt o długości danych 2B, zakres górny. */
    SG_OBJ_POOL_6_STD_LOW = SG_OBJ_POOL_6_LOW, /* Obiekt o długości danych 2B, zakres standardowy dolny. */
    SG_OBJ_POOL_6_STD_HIGH = SG_OBJ_POOL_6_LOW + 0x7FU, /* Obiekt o długości danych 2B, zakres standardowy górny. */
    SG_OBJ_POOL_6_MFC_LOW = SG_OBJ_POOL_6_LOW + 0x80U, /* Obiekt o długości danych 2B, zakres producencki dolny. */
    SG_OBJ_POOL_6_MFC_HIGH = SG_OBJ_POOL_6_HIGH, /* Obiekt o długości danych 2B, zakres producencki górny. */

    /*  Pula 7 i 8: Zarezerwowane. */
    SG_OBJ_POOL_7_LOW = 0x700U, /* Obiekt z puli 7 (zarezerwowany), zakres dolny. */
    SG_OBJ_POOL_8_HIGH = SG_OBJ_POOL_7_LOW + 0x1FFU, /* Obiekt z puli 8 (zarezerwowany), zakres górny. */

    /* Pula 9: Obiekty o długość 1B. */
    SG_OBJ_POOL_9_LOW = 0x900U, /* Obiekt 1B, zakres dolny. */
    SG_OBJ_POOL_9_HIGH = SG_OBJ_POOL_9_LOW + 0xFFU, /* Obiekt 1B, zakres górny. */
    SG_OBJ_POOL_9_STD_LOW = SG_OBJ_POOL_9_LOW, /* Obiekt 1B, zakres standardowy dolny. */
    SG_OBJ_POOL_9_STD_HIGH = SG_OBJ_POOL_9_LOW + 0x7FU, /* Obiekt 1B, zakres standardowy górny. */
    SG_OBJ_POOL_9_MFC_LOW = SG_OBJ_POOL_9_LOW + 0x80U, /* Obiekt 1B, zakres producencki dolny. */
    SG_OBJ_POOL_9_MFC_HIGH = SG_OBJ_POOL_9_HIGH, /* Obiekt 1B, zakres producencki górny. */

    /*  Pula 10: Obiekty o długość 2B. */
    SG_OBJ_POOL_10_LOW = 0xA00U, /* Obiekt 2B, zakres dolny. */
    SG_OBJ_POOL_10_HIGH = SG_OBJ_POOL_10_LOW + 0xFFU, /* Obiekt 2B, zakres górny. */
    SG_OBJ_POOL_10_STD_LOW = SG_OBJ_POOL_10_LOW, /* Obiekt 2B, zakres standardowy dolny. */
    SG_OBJ_POOL_10_STD_HIGH = SG_OBJ_POOL_10_LOW + 0x7FU, /* Obiekt 2B, zakres standardowy górny. */
    SG_OBJ_POOL_10_MFC_LOW = SG_OBJ_POOL_10_LOW + 0x80U, /* Obiekt 2B, zakres producencki dolny. */
    SG_OBJ_POOL_10_MFC_HIGH = SG_OBJ_POOL_10_HIGH, /* Obiekt 2B, zakres producencki górny. */

    /*  Pula 11: Zarezerwowane. */
    SG_OBJ_POOL_11_LOW = 0xB00U, /* Obiekt z puli 11 (zarezerwowany), zakres dolny. */
    SG_OBJ_POOL_11_HIGH = SG_OBJ_POOL_11_LOW + 0x0FFU, /* Obiekt z puli 11 (zarezerwowany), zakres górny. */

    /*  Pula 12: Obiekty o długość 4B. */
    SG_OBJ_POOL_12_LOW = 0xC00U, /* Obiekt 4B, zakres dolny. */
    SG_OBJ_POOL_12_STD_LOW = SG_OBJ_POOL_12_LOW, /* Obiekt 4B, zakres standardowy dolny. */
    SG_OBJ_POOL_12_HIGH = SG_OBJ_POOL_12_LOW + 0xFFU, /* Obiekt 4B, zakres górny. */
    SG_OBJ_POOL_12_STD_HIGH = SG_OBJ_POOL_12_LOW + 0x7FU, /* Obiekt 4B, zakres standardowy górny. */
    SG_OBJ_POOL_12_MFC_LOW = SG_OBJ_POOL_12_LOW + 0x80U, /* Obiekt 4B, zakres producencki dolny. */
    SG_OBJ_POOL_12_MFC_HIGH = SG_OBJ_POOL_12_HIGH, /* Obiekt 4B, zakres producencki górny. */

    /*  Pula 13: Długość obiektu określona polem długości OBJ_LEN o długości 1B. */
    SG_OBJ_POOL_13_LOW = 0xD00U, /* Obiekt o długości od 1 do 255, zakres dolny. */
    SG_OBJ_POOL_13_HIGH = SG_OBJ_POOL_13_LOW + 0xFFU, /* Obiekt o długości danych 1B , zakres górny. */
    SG_OBJ_POOL_13_STD_LOW = SG_OBJ_POOL_13_LOW, /* Obiekt o długości danych 1B, zakres standardowy dolny. */
    SG_OBJ_POOL_13_STD_HIGH = SG_OBJ_POOL_13_LOW + 0x7FU, /* Obiekt o długości danych 1B, zakres standardowy górny. */
    SG_OBJ_POOL_13_MFC_LOW = SG_OBJ_POOL_13_LOW + 0x80U, /* Obiekt o długości danych 1B, zakres producencki dolny. */
    SG_OBJ_POOL_13_MFC_HIGH = SG_OBJ_POOL_13_HIGH, /* Obiekt o długości danych 1B, zakres producencki górny. */

    /*  Pula 14 i 15: Zarezerwowane. */
    SG_OBJ_POOL_14_LOW = 0xE00U, /* Obiekt z puli 14 (zarezerwowany), zakres dolny. */
    SG_OBJ_POOL_15_HIGH = SG_OBJ_POOL_14_LOW + 0x1FFU, /* Obiekt z puli 15 (zarezerwowany), zakres górny. */
};

/*!
 * \brief Lista obiektów (pkt 6).
 *
 * \details Oznaczenia grup w ramach pole „Wymaganie” wskazują na stosowalność obiektu
 * w aspekcie wynikającym z typu urządzenia, realizowanych funkcjonalności, technik komunikacji:
 * R – wszystkie urządzenia, w tym rejestrator gazomierzowy
 * Z – sterowanie zaworem
 * P – funkcja przedpłaty
 * V – funkcjonalność naliczania oparta o objętości
 * E – funkcjonalność naliczania oparta o energię
 * I – funkcjonalność detekcji ingerencji w urządzenie pomiarowe
 * S – obsługa techniki komunikacyjnej SMS
 * G – obsługa techniki komunikacyjnej pakietowej transmisji danych, GPRS
 * opcja – wskazuje na nieobligatoryjność obsługi obiektu
 */
enum SG_OBJ_ID
{
    /* Informacyjne (6.1). */

    /*!
     * \brief Numer seryjny urządzenia.
     * \details Najstarszy bajt wyróżnikiem puli nadanej producentowi.
     * Wartość obiektu jest tożsama z wartością parametru WP_SN.
     * Zapis tylko przez producenta (poziom uprawnień 0xF).
     *          Typ: 4B
     *          Dostęp: R / W (producent)
     *          Wymaganie: R
     */
    SG_OBJ_ID_SERIAL_NBR = 0x400,

    /*!
     * \brief Identyfikator klienta lub punktu poboru, w formacie ASCII.
     * \details Typ: max. 32B
     *          Dostęp: R / W
     *          Wymaganie: R opcja
     */
    SG_OBJ_ID_CLIENT_ID = 0x500,

    /*!
     * \brief Numer seryjny gazomierza, w formacie ASCII.
     * \details Typ: max. 20B
     *          Dostęp: R / W
     *          Wymaganie: R opcja
     */
    SG_OBJ_ID_GAS_METER_SERIAL_NBR = 0x501,

    /*!
     * \brief Szerokość geograficzna lokalizacji, w której zamontowane jest urządzenie,
     * wyrażona w milionowych części stopnia, poprawny zakres wartości to od -90 mln do 90 mln.
     * \details Typ: 4B
     *          Dostęp: R / W
     *          Wymaganie: R opcja
     */
    SG_OBJ_ID_LATITUDE = 0x401,

    /*!
     * \brief Długość geograficzna lokalizacji, w której zamontowane jest urządzenie,
     *         wyrażona w milionowych części stopnia, poprawny zakres wartości to
     *         od -180 mln do 180 mln.
     * \details Typ: 4B
     *          Dostęp: R / W
     *          Wymaganie: R opcja
     */
    SG_OBJ_ID_LONGITUDE = 0x402,

    /*!
     * \brief Wersja sprzętowa urządzenia.
     *
     * \details Zapis tylko przez producenta (poziom uprawnień 0xF).
     *          Typ: 4B
     *          Dostęp: R / W (producent)
     *          Wymaganie: R
     */
    SG_OBJ_ID_HARDWARE_VERSION = 0x403,

    /*!
     * \brief Wersja oprogramowania urządzenia.
     *
     * \details Zapis tylko przez producenta (poziom uprawnień 0xF).
     * Zmiana wersji oprogramowania powoduje wysłanie obiektów FIRMWARE_VERSION
     * i STANDARD_PARAMETER_LIST_VERSION na domyślny adres zdarzeń.
     *          Typ: 4B
     *          Dostęp: R / W (producent)
     *          Wymaganie: R.
     */
    SG_OBJ_ID_FIRMWARE_VERSION = 0x404,

    /*!
     * \brief Nazwa typu urządzenia zrozumiała dla człowieka, w formacie ASCII.
     *
     * \details Typ: max. 20B
     *          Dostęp: R / W (producent)
     *          Wymaganie: R.
     */
    SG_OBJ_ID_DEVICE_TYPE_NAME = 0x502,

    /*!
     * \brief Identyfikator wersji listy parametrów wynikających ze standardu.
     *
     * \details Obecna wartość to 1. Zmiana wersji listy parametrów w wyniku
     * aktualizacji oprogramowania powoduje wysłanie obiektów FIRMWARE_VERSION
     * i STANDARD_PARAMETER_LIST_VERSION na domyślny adres zdarzeń.
     *
     * \details Typ: 2B
     *          Dostęp: R / W (producent)
     *          Wymaganie: R
     */
    SG_OBJ_ID_STANDARD_PARAMETER_LIST_VERSION = 0x200,

    /* Liczydła i zużycia (6.2). */

    SG_OBJ_ID_UNITS = 0x100, /*!< \brief Jednostka wartości liczydła i przyrostów – młodszy półbajt wskazuje na wagę: 1 – 10 m3/100 kWh, 0 – 1 m3/10 kWh, 0xF – 0,1 m3/1 kWh, 0xE – 0,01 m3/0,1 kWh, domyślna wartość 0xE – 0,01 m3/0,1 kWh. Wskazanymi przez ten parametr jednostkami wyrażanych jest wiele parametrów rejestratora. Zmiana wartości UNITS nie powoduje zmiany innych parametrów, w tym VOLUME. Typ: 1B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_VOLUME = 0x405, /*!< \brief Liczydło objętości w jednostce wynikającej z UNITS. Powinno być zgodne z liczydłem mechanicznym. Typ: 4B Dostęp: R / W Wymaganie: V */
    SG_OBJ_ID_VOLUME_MAX_VALUE = 0x42B, /*!< \brief Maksymalna wartość liczydła objętości VOLUME w jednostce wynikającej z UNITS. Ustawienie wartości tego obiektu pozwala zsynchronizować moment przekręcenia VOLUME z przekręceniem liczydła mechanicznego. Przykładowa wartość VOLUME_MAX_VALUE=9 999 999 gdy UNITS=0xE oznacza, że liczydło mechaniczne przekręca się na 0 po przekroczeniu 99 999,99 m3. Domyślna wartość to 0xFFFFFFFF, czyli licznik przekręca się dopiero, gdy osiągnie maksymalną wartość 4-bajtową. Typ: 4B Dostęp: R / W Wymaganie: V */
    SG_OBJ_ID_COMMAND_VOLUME_OFFSET = 0x406, /*!< \brief Komenda przestawienia liczydła objętości o zadaną wartość w jednostce wynikającej z UNITS. Wartość ze znakiem. Typ: 4B Dostęp: - / W Wymaganie: V */
    SG_OBJ_ID_ENERGY = 0x407, /*!< \brief Liczydło energii w jednostce wynikającej z UNITS. Typ: 4B Dostęp: R / W Wymaganie: E */
    SG_OBJ_ID_COMMAND_ENERGY_OFFSET = 0x408, /*!< \brief Komenda przestawienia liczydła energii o zadaną wartość w jednostce wynikającej z UNITS. Wartość ze znakiem. Typ: 4B Dostęp: - / W Wymaganie: E */
    SG_OBJ_ID_CALORIFIC_VALUE = 0x201, /*!< \brief Wartość kaloryczna gazu służąca określaniu ilości zużywanej energii na podstawie zużywanej objętości gazu, wyrażona w Wh/m3. Typ: 2B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_COUNTERS_ARCHIVE = 0x600, /*!< \brief Minimum 170-elementowa cykliczna tablica z liczydłami zatrzaśniętymi co okres rejestracji (COUNTERS_REGISTRATION_PERIOD). Kolumny w tej tablicy to: czas zatrzasku (CLOCK), liczydło objętości (VOLUME), liczydło energii (ENERGY). Tryb odwołania do tej tablicy to TBL_ACC_TIME i TBL_ACC_TIME_BACK. Typ: tablicowy Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_COUNTERS_REGISTRATION_PERIOD = 0x101, /*!< \brief Okres rejestracji liczydeł w archiwum liczydeł wyrażony w minutach. Dopuszczalne wartości: 60, 30, 20, 15, 12. Określa jak często są zapisywane liczydła do archiwum COUNTERS_ARCHIVE. Typowa wartość to 60 – 60 minut. Typ: 1B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_TIME = 0x409, /*!< \brief Czas ostatniej rejestracji w archiwum liczydeł. Liczba minut w znaczniku czasowym musi być wielokrotnością COUNTERS_REGISTRATION_PERIOD, a liczba sekund w znaczniku jest równa 0. Typ: 4B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_VOLUME = 0x40A, /*!< \brief Objętość ostatniego wpisu do archiwum liczydeł w jednostce wynikającej z UNITS. Typ: 4B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_ENERGY = 0x40B, /*!< \brief Energia ostatniego wpisu do archiwum liczydeł w jednostce wynikającej z UNITS. Typ: 4B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_PERIOD_VOLUME_USAGE_COMPRESSED = 0x503, /*!< \brief Skompresowana postać przyrostów objętości (zużyć) z kolejnych okresów rejestracji od czasu ostatniego zatrzasku periodycznego lub zdarzenia powodującego nieciągłość naliczania zużycia np. istotna zmiana czasu, bezpośrednia zmiana liczydła objętości. Wyrażone w jednostce wynikającej z UNITS. Przyrosty za kolejne okresy rejestracji kodowane są na 1,5B. Kodowanie wartości 1,5-bajtowych jest w konwencji Big Endian. Jeśli ostatni półbajt nie jest wykorzystany, to zapełniany jest wartością 0xF. Wartość zużycia przekraczająca 1,5B jest kodowana w postaci 0xFFF. W przypadku gdy obiekt zawiera dane z niepełnego okresu rozliczeniowego, to rozmiar obiektu jest mniejszy. Ostatnia wartość dotyczy okresu rejestracji kończącego się o czasie COUNTERS_ARCHIVE_LAST_TIME. Przykład kodowania przyrostów 5,00 m3 (0x1F4), 4,00 m3 (0x190), 3,00 m3 (0x12C), 1,05 m3 (0x069), 2,58 m3 (0x102): 1F419012C069102F Typ: max. 255B, typowo do 36B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_PERIOD_ENERGY_USAGE_COMPRESSED = 0x504, /*!< \brief Skompresowana postać zużyć energii z kolejnych okresów rejestracji od czasu ostatniego zatrzasku periodycznego lub zdarzenia powodującego nieciągłość naliczania zużycia np. istotna zmiana czasu, bezpośrednia zmiana liczydła energii. Zużycia za kolejne okresy rejestracji kodowane są na 1,5B. Kodowane jak PERIOD_VOLUME_USAGE_COMPRESSED. Typ: max. 255B, typowo do 36B Dostęp: R / - Wymaganie: E */
    SG_OBJ_ID_MONTH_VOLUME_USAGE_COMPRESSED = 0x505, /*!< \brief Skompresowana postać zużyć objętości z zamknięcia kolejnych dób gazowniczych od czasu ostatniego zatrzasku miesięcznego lub zdarzenia powodującego nieciągłość naliczania zużycia np. istotna zmiana czasu, bezpośrednia zmiana liczydła objętości. Zużycia za kolejne okresy rejestracji kodowane są na 2B. Wartość zużycia przekraczająca 2B jest kodowana w postaci 0xFFFF. W przypadku gdy obiekt zawiera dane z niepełnego miesiąca, to rozmiar obiektu jest mniejszy niż liczba dni w miesiącu. Ostatnia wartość dotyczy okresu rejestracji kończącego się o czasie LAST_DAILY_LATCH_TIME. Przykład kodowania przyrostów 20,50 m3 (0x0802), 100,00 m3 (0x2710), 12,34 m3 (0x04D2), 654,32 m3 (0xFF98):02081027D20498FF Typ: max. 62B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_MONTH_ENERGY_USAGE_COMPRESSED = 0x506, /*!< \brief Skompresowana postać zużyć energii z kolejnych okresów dób gazowniczych od czasu ostatniego zatrzasku miesięcznego lub zdarzenia powodującego nieciągłość naliczania zużycia np. istotna zmiana czasu, bezpośrednia zmiana liczydła energii. Zużycia za kolejne okresy rejestracji kodowane są na 2B. Kodowanie jak MONTH_VOLUME_USAGE_COMPRESSED. Typ: max. 62B Dostęp: R / - Wymaganie: V/E */

    /* Przepływ (6.3) */

    SG_OBJ_ID_GAS_METER_Q_MAX = 0x202, /* Wartość graniczna przepływu zdefiniowana przez producenta gazomierza. Przekroczenie wartości przepływu chwilowego lub godzinowego powoduje ustawienie w statusie flagi odpowiedniego przekroczenia. Wartość wyrażona w jednostce wynikającej z UNITS na godzinę. Wartość domyślna dla gazomierza G4 to 600 – 6 m3/h. Typ: 2B Dostęp: R / W Wymaganie: V opcja */
    SG_OBJ_ID_TEMPORARY_FLOW = 0x203, /* Aktualna wartość przepływu chwilowego w jednostce wynikającej z UNITS na godzinę. Typ: 2B Dostęp: R / - Wymaganie: V opcja */
    SG_OBJ_ID_HOURLY_USAGE_CURRENT = 0x204, /* Aktualna wartość zużycia w bieżącej godzinie, w jednostce wynikającej z UNITS. Typ: 2B Dostęp: R / - Wymaganie: V opcja */
    SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED = 0x205, /* Maksymalna wartość przepływu chwilowego w bieżącym okresie rozliczeniowym (typowo dobie gazowniczej) w jednostce wynikającej z UNITS na godzinę. Typ: 2B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED = 0x206, /* Czas maksymalnej wartości przepływu chwilowego w bieżącym okresie rozliczeniowym wyrażony w minutach wstecznie względem ostatniego uzupełnienia archiwów COUNTERS_ARCHIVE_LAST_TIME. Typ: 2B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED = 0x207, /* Zatrzaśnięta w bieżącym okresie rozliczeniowym (typowo dobie gazowniczej) maksymalna wartość przepływu godzinowego w jednostce wynikającej z UNITS na godzinę. Typ: 2B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED = 0x102, /* Czas maksymalnej wartości przepływu godzinowego w bieżącym okresie rozliczeniowym wyrażony w godzinach względem ostatniego uzupełnienia archiwów COUNTERS_ARCHIVE_LAST_TIME zaokrąglonego do zakończonej pełnej godziny (zaokrąglenie ma znaczenie dla COUNTERS_REGISTRATION_PERIOD<60, np. dla COUNTERS_ARCHIVE_LAST_TIME na godz. 17:15, godziny są liczone względem godz. 17:00). Typ: 1B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED = 0x208, /* Zatrzaśnięta w bieżącym miesiącu gazowniczym maksymalna wartość przepływu chwilowego w jednostce wynikającej z UNITS na godzinę. Typ: 2B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED = 0x209, /* Czas maksymalnej wartości przepływu chwilowego w bieżącym miesiącu gazowniczym wyrażony w minutach względem czasu ostatniego zatrzasku dziennego LAST_DAILY_LATCH_TIME. Typ: 2B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED = 0x20A, /* Zatrzaśnięta w bieżącym miesiącu gazowniczym maksymalna wartość przepływu godzinowego w jednostce wynikającej z UNITS na godzinę. Typ: 2B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED = 0x20B, /* Czas maksymalnej wartości przepływu godzinowego w bieżącym miesiącu gazowniczym wyrażony w godzinach względem czasu ostatniego zatrzasku dziennego LAST_DAILY_LATCH_TIME. Typ: 2B Dostęp: R / - Wymaganie: V/E */

    /* Zegar (6.4). */

    SG_OBJ_ID_CLOCK = 0x40C, /* Zegar urządzenia wyrażony w czasie UTC. Typ: 4B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_CLOCK_SYNCHRONISATION_OFFSET = 0x20C, /* Przesunięcie zegara, które jest ustawiane gdy zachodzi potrzeba niewielkiej synchronizacji zegara (typowo do 10 minut). Wartość w sekundach, ze znakiem. Typowa wartość to zero. Typ: 2B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_TIME_ZONE_OFFSET = 0x103, /* Przesunięcie czasu lokalnego względem czasu UTC wyrażone w kwadransach. Liczba ze znakiem. Domyślne wartości to 4 – UTC+1, 8 – UTC+2. Ustawienie wartości na 127 oznacza przyjęcie TIME_ZONE_OFFSET_WINTER, a -128 przyjęcie TIME_ZONE_OFFSET_SUMMER. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_TIME_ZONE_OFFSET_WINTER = 0x104, /* Przesunięcie czasu lokalnego względem czasu UTC wyrażone w kwadransach obowiązujące zimą. Liczba ze znakiem. Domyślna wartość to 4 – UTC+1. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_TIME_ZONE_OFFSET_SUMMER = 0x105, /* Przesunięcie czasu lokalnego względem czasu UTC wyrażone w kwadransach obowiązujące latem. Liczba ze znakiem. Domyślna wartość to 8 – UTC+2. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_TIME_ZONE_AUTO_OFFSET = 0x124, /* Flaga wskazująca na to, czy urządzenie automatycznie zmienia czas lokalny z zimowego na letni i odwrotnie zgodnie z regułami dyrektywy UE 2000/84/EC. Wartość 0 oznacza nie, wartość 1 oznacza tak. Domyślna wartość to 0. Ustawienie flagi na 1 spowoduje, że wartość TIME_ZONE_OFFSET będzie automatycznie zmieniana na TIME_ZONE_OFFSET_SUMMER w ostatnią niedzielę marca o godz. 01:00 UTC i TIME_ZONE_OFFSET_WINTER w ostatnią niedzielę października o godz. 01:00 UTC. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_CLOCK_SYNCHRONISATION_FROM_GSM = 0x106, /* Flaga wskazująca na to, czy urządzenie ma synchronizować czas do czasu otrzymywanego z sieci GSM. Wartość 0 oznacza nie, wartość 1 oznacza tak. Domyślna wartość to 1 – tak. Typ: 1B Dostęp: R / W Wymaganie: S */

    /* Ingerencja (6.5) */

    SG_OBJ_ID_TAMPER_REMOVE_TIME = 0x40D, /* Czas ostatniej ingerencji zdjęcia rejestratora z gazomierza. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_REMOVE_DURATION = 0x40E, /* Łączny czas trwania ingerencji zdjęcia rejestratora z gazomierza w sekundach. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_REMOVE_COUNT = 0x20D, /* Łączna liczba ingerencji zdjęcia rejestratora z gazomierza. Typ: 2B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_REMOVE_VOLUME = 0x40F, /* Łączna objętość gazu naliczona w stanie ingerencji zdjęcia rejestratora z gazomierza w jednostkach UNITS. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_REMOVE_ENERGY = 0x410, /* Łączna energia gazu naliczona w stanie ingerencji zdjęcia rejestratora z gazomierza w jednostkach UNITS. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_OPEN_TIME = 0x411, /* Czas ostatniej ingerencji otwarcia obudowy rejestratora. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_OPEN_DURATION = 0x412, /* Łączny czas trwania ingerencji otwarcia obudowy rejestratora w sekundach. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_OPEN_COUNT = 0x20E, /* Łączna liczba ingerencji otwarcia obudowy rejestratora. Typ: 2B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_OPEN_VOLUME = 0x413, /* Łączna objętość gazu naliczona w stanie ingerencji otwarcia obudowy rejestratora w jednostkach UNITS. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_OPEN_ENERGY = 0x414, /* Łączna energia gazu naliczona w stanie ingerencji otwarcia obudowy rejestratora w jednostkach UNITS. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_MAGNETIC_TIME = 0x415, /* Czas ostatniej ingerencji polem magnetycznym. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_MAGNETIC_DURATION = 0x416, /* Łączny czas trwania ingerencji polem magnetycznym w sekundach. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_MAGNETIC_COUNT = 0x20F, /* Łączna liczba ingerencji polem magnetycznym. Typ: 2B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_MAGNETIC_VOLUME = 0x417, /* Łączna objętość gazu naliczona w stanie ingerencji polem magnetycznym w jednostkach UNITS. Typ: 4B Dostęp: R / W Wymaganie: I */
    SG_OBJ_ID_TAMPER_MAGNETIC_ENERGY = 0x418, /* Łączna energia gazu naliczona w stanie ingerencji polem magnetycznym w jednostkach UNITS. Typ: 4B Dostęp: R / W Wymaganie: I */

    /* Diagnostyki (6.6) */

    SG_OBJ_ID_BATTERY_LEVEL = 0x107, /* Poziom baterii w procentach. Nowa bateria ma 100, zużyta 0. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_TEMPERATURE = 0x108, /* Temperatura otoczenia, w stopniach Celsjusza, wartość ze znakiem. Typ: 1B Dostęp: R / - Wymaganie: R */

    /* Modem (6.7) */

    SG_OBJ_ID_SIM_PIN = 0x507U, /* PIN do karty SIM, w formacie ASCII. Typ: max. 8B Dostęp: - / W Wymaganie: S/G */
    SG_OBJ_ID_SIM_ICCID = 0x508U, /* Numer seryjny karty SIM w formacie BCD. Numer ten jest również wygrawerowany na kacie SIM. Typ: 11B Dostęp: R / - Wymaganie: S/G */
    SG_OBJ_ID_SIM_PHONE = 0x509U, /* Numer telefonu kodowany jest w formacie BCD (z numerem kierunkowym kraju, bez zer wiodących i znaku plus). Dla nr. telefonu +48601234567 pole ma wartość: 48601234567FFFFF. Typ: 8B Dostęp: R / - Wymaganie: S/G */
    SG_OBJ_ID_SIM_IP = 0x419U, /* Prywatny adres IP karty SIM, na kolejnych bajtach kolejne pola adresu IP. Typ: 4B Dostęp: R / - Wymaganie: G */
    SG_OBJ_ID_SIM_SMSC = 0x50AU, /* Numer centrum wiadomości (SMSC), w formacie ASCII. Typ: max. 16B Dostęp: R / W Wymaganie: S opcja */
    SG_OBJ_ID_SIM_SMS_VALIDITY = 0x109U, /* Czas ważności wysyłanej wiadomości SMS (0x0B - 1h, 0x47 - 6h, 0x8F - 12h, 0xA7 - 24h, 0xA8 - 48h, 0xA9 - 72h, 0xAD - 7dni, 0xFF - max) Typ: 1B Dostęp: R / W Wymaganie: S opcja */
    SG_OBJ_ID_GPRS_APN = 0x50BU, /* Adres APN prywatnego lub publicznego np. erainternet.pl, w formacie ASCII. Typ: max. 32B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_GPRS_USER = 0x50CU, /* Użytkownik APN, w formacie ASCII. Typ: max. 16B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_GPRS_PASSWORD = 0x50DU, /* Hasło do APN, w formacie ASCII. Typ: max. 16B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_GSM_QUALITY = 0x10AU, /* Moc sygnału GSM z zakresu od 0 (najsłabszy) do 31 (najmocniejszy). W przypadku wyłączonego modemu obiekt zawiera ostatnią znaną siłę sygnału. Gdy modem jeszcze nigdy nie był włączony to siła sygnału ma wartość neutralną 99 (decymalnie). Typ: 1B Dostęp: R / - Wymaganie: S/G */
    SG_OBJ_ID_GSM_NETWORK_CODE = 0x50EU, /* Kod operatora GSM do którego zalogowany jest modem – połączenie MCC i MNC, w formacie ASCII. Typ: max. 8B Dostęp: R / - Wymaganie: S/G */
    SG_OBJ_ID_GSM_BTS_LAC = 0x41AU, /* Identyfikator obszaru stacji bazowej GSM (Location Area Code), do którego zalogowany jest modem. Typ: 4B Dostęp: R / - Wymaganie: S/G */
    SG_OBJ_ID_GSM_BTS_CELL_ID = 0x41BU, /* Identyfikator Cell ID stacji bazowej GSM, do którego zalogowany jest modem. Typ: 4B Dostęp: R / - Wymaganie: S/G */
    SG_OBJ_ID_MODEM_STATE = 0x10BU, /* Stan modemu określony bitami (128-logging, 64-in roaming, 32-pin incorrect, 16-sim not inserted, 8-komunikacja z sim niemożliwa, 4-wymagany PUK, 2-, 1-). Typ: 1B Dostęp: R / - Wymaganie: S/G opcja */
    SG_OBJ_ID_MODEM_SMS_RECEIVED = 0x210U, /* Liczba odebranych SMS - modulo 0x10000, czyli po osiągnięciu 0xFFFF wartość przekręca się na 0x0000. Typ: 2B Dostęp: R / W Wymaganie: S */
    SG_OBJ_ID_MODEM_SMS_SENT = 0x211U, /* Liczba wysłanych SMS - modulo 0x10000, czyli po osiągnięciu 0xFFFF wartość przekręca się na 0x0000. Typ: 2B Dostęp: R / W Wymaganie: S */
    SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED = 0x212U, /* Liczba odebranych pakietów GPRS - modulo 0x10000, czyli po osiągnięciu 0xFFFF wartość przekręca się na 0x0000. Typ: 2B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT = 0x213U, /* Liczba wysłanych pakietów GPRS - modulo 0x10000, czyli po osiągnięciu 0xFFFF wartość przekręca się na 0x0000. Typ: 2B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_MODEM_WAKEUP_COUNT = 0x214U, /* Liczba wybudzeń modemu. Typ: 2B Dostęp: R / W Wymaganie: S/G opcja */
    SG_OBJ_ID_MODEM_WAKEUP_DURATION = 0x41CU, /* Łączny czas trwania modemu w stanie wybudzenia, w sekundach. Typ: 4B Dostęp: R / W Wymaganie: S/G opcja */

    /* Konfiguracja zdarzeń (6.8) */

    SG_OBJ_ID_EVENT_LINK = 0x10CU, /* Domyślne łącze przesyłania zdarzeń: 0 - przesyłanie zdarzeń zabronione, 1 - SMS, 2 - GPRS (UDP/IP), 3 - GPRS (TCP/IP), ... Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_EVENT_PHONE = 0x50FU, /* Numer telefonu, na który przesyłane są pakiety, gdy ma być wykorzystana technika SMS – zdarzenie gdy EVENT_LINK = 1 (SMS) lub odpowiedź gdy CTRL_RLK = 1 (SMS). Numer telefonu kodowany jest w formacie BCD (z numerem kierunkowym kraju, bez zer wiodących i znaku plus). Dla nr. telefonu +48601234567 pole ma wartość: 48601234567FFFFF Typ: 8B Dostęp: R / W Wymaganie: S */
    SG_OBJ_ID_EVENT_IP = 0x41DU, /* Numer IP, na który przesyłane są pakiety gdy ma być wykorzystana technika GPRS – zdarzenie gdy EVENT_LINK = 2 lub 3 (GPRS) lub odpowiedź gdy CTRL_RLK = 2 lub 3 (GPRS). Na kolejnych bajtach umieszczane są kolejne pola adresu IP, przykładowo adres 127.0.0.1 kodowany jest do wartości 0x7F000001. Typ: 4B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_EVENT_UDP_PORT = 0x215U, /* Numer portu UDP, na który przesyłane są pakiety gdy ma być wykorzystana technika UDP/IP – EVENT_LINK (zdarzenie) lub CTRL_RLK (odpowiedź) = 2 (GPRS UDP/IP) Typ: 2B Dostęp: R / W Wymaganie: G */
    SG_OBJ_ID_EVENT_TCP_PORT = 0x216U, /* Numer portu TCP, na który przesyłane są pakiety gdy ma być wykorzystana technika UDP/IP – EVENT_LINK (zdarzenie) lub CTRL_RLK (odpowiedź) = 3 (GPRS TCP/IP) Typ: 2B Dostęp: R / W Wymaganie: G */

    /* Zawór (6.9) */

    SG_OBJ_ID_VALVE_OPERATION = 0x10D, /* Operacja do wykonania na zaworze (1-bezpieczne otwarcie, 2-zamknięcie) Typ: 1B Dostęp: - / W Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_ACTIVATION_TIMEOUT = 0x217, /* Czas na aktywowanie przez użytkownika otwarcia zaworu wyrażony w minutach. Po tym czasie użytkownik straci możliwość otwarcia zaworu. Domyślna wartość: 1440 – 1 doba. Typ: 2B Dostęp: R / W Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_STATE = 0x10E, /* Stan zaworu: 0 – otwarty, żadna operacja nie wykonywana 1 – zamknięty, żadna operacja nie wykonywana 2 – zamknięty, oczekiwanie na aktywację 3 – otwarty, próba szczelności 4 – zamknięty, otwieranie nie powiodło się Typ: 1B Dostęp: R / - Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_LEAK_TEST_DURATION = 0x218, /* Maksymalny czas testu szczelności w sekundach. Domyślna wartość: 300 – 5 minut. Typ: 2B Dostęp: R / W Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_LEAK_TEST_VOLUME = 0x219, /* Maksymalna ilość gazu, której wypłynięcie podczas testu szczelności jeszcze nie świadczy jeszcze o jego niepowodzeniu. Wartość wyrażona w jednostce UNITS. Domyślna wartość: 2 – 0,02 m3. Typ: 2B Dostęp: R / W Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_ERROR_CODE = 0x10F, /* Kod błędu sprzętowego ostatniej operacji sterowania zaworem. Słownik producenta. Typ: 1B Dostęp: R / - Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_OPEN_COUNT = 0x21A, /* Łączna liczba prób otwarć zaworu. Typ: 2B Dostęp: R / W Wymaganie: Z/P */
    SG_OBJ_ID_VALVE_CLOSE_COUNT = 0x21B, /* Łączna liczba prób zamknięć zaworu. Typ: 2B Dostęp: R / W Wymaganie: Z/P */

    /* Przedpłaty (6.10) */

    SG_OBJ_ID_PREPAID_TOPUP_ID = 0x110, /* Identyfikator aktualnego doładowania. Typ: 1B Dostęp: R / W Wymaganie: P */
    SG_OBJ_ID_PREPAID_TOPUP_END_TIME = 0x41E, /* Czas do którego doładowanie jest aktywne, przy którym nastąpi zamknięcie zaworu. Typ: 4B Dostęp: R / W Wymaganie: P */
    SG_OBJ_ID_PREPAID_TOPUP_END_VOLUME = 0x41F, /* Stan liczydła objętości, przy którym nastąpi zamknięcie zaworu, wyrażony w jednostce wynikającej z UNIT, wartość 0xFFFFFFFF oznacza nieaktywne ograniczenie. Typ: 4B Dostęp: R / W Wymaganie: P */
    SG_OBJ_ID_PREPAID_TOPUP_END_ENERGY = 0x420, /* Stan liczydła energii, przy którym nastąpi zamknięcie zaworu, wyrażony w jednostce wynikającej z UNIT, wartość 0xFFFFFFFF oznacza nieaktywne ograniczenie. Typ: 4B Dostęp: R / W Wymaganie: P */
    SG_OBJ_ID_PREPAID_VALVE_CONTROL = 0x111, /* Flaga wskazująca na sterowanie zaworem przez mechanizm przedpłat lub ręczne (0 ręczne, 1-mechanizm przedpłat). Domyślna wartość to 1 – mechanizm przedpłat. Typ: 1B Dostęp: R / W Wymaganie: P */

    /* Status (6.11) */

    SG_OBJ_ID_STATUS = 0x421, /* Status bieżący – 32 flagi. Typ: 4B Dostęp: R / - Wymaganie: R */
    SG_OBJ_ID_STATUS_PERIODIC_LATCHED = 0x422, /* Status zatrzaśnięty w bieżącym okresie rozliczeniowym. Typ: 4B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_STATUS_MONTHLY_LATCHED = 0x423, /* Status zatrzaśnięty w bieżącym miesiącu gazowniczym. Typ: 4B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_STATUS_UP_EVENT_MASK = 0x424, /* Znaczniki wskazujące flagi błędów, których przejście z wartości 0 na wartość 1 powoduje co najmniej wykonanie zleceń wyrażonych w STATUS_COMMAND. Urządzenie może też przesłać inne OBJ_ID zależne od flagi błędów. Typ: 4B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_STATUS_DOWN_EVENT_MASK = 0x425, /* Znaczniki wskazujące flagi błędów, których przejście z wartości 1 na wartość 0 powoduje co najmniej wykonanie zleceń wyrażonych w STATUS_COMMAND. Urządzenie może też przesłać inne OBJ_ID zależne od flagi błędów. Typ: 4B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_STATUS_LATCHED = 0x42C, /* Znacznik kolumny statusu występującego w archiwach oraz strukturach periodycznych. Zawiera status za jeden zatrzaśnięty okres (np. od nieciągłości do zatrzasku periodycznego). Flagi związane z nieciągłością występują w strukturze opisującej okres po wystąpieniu nieciągłości. Przykład: Gdy w dobie gazowniczej wystąpi jedna nieciągłość z powodu zmiany stanu licznika o godzinie 05.05.2016 17:23, to PERIODIC_VOLUME_DATA_LATCHED oraz wpis do archiwum na koniec tej doby 06.05.2016 06:00 będą zawierać flagę nieciągłości FLAG_DISCONTINUITY, flagę zmiany licznika FLAG_VOLUME_SET oraz przyrosty od godziny 17:00. STATUS_LATCHED zawiera też w takim przypadku inne flagi zdarzeń, które wystąpiły między 05.05.2016 17:00 a 06.05.2016 06:00, w szczególności takie, które wystąpiły między godziną 17:00 a 17:23, np. flaga ingerencji polem magnetycznym, która wystąpiła o 17:12. Pierwszy przyrost za pierwszą godzinę 17:00-18:00 będzie niepewny ze względu na flagę nieciągłości. Poprzedni wpis w archiwach za godzinę 05.05.2016 17:00 oraz obiekt PERIODIC_VOLUME_DATA_CURRENT wysłany ze względu na wystąpienie nieciągłości nie będą zawierać flagi nieciągłości. Typ: 4B Dostęp: - / - Wymaganie: V/E */
    SG_OBJ_ID_STATUS_COMMAND = 0x520, /* Wskazuje na polecenie mające format WO_DATA. Zlecenie GET oznacza przesłanie wskazanych danych do serwera łączem zdarzeń, natomiast zlecenie PUT oznacza wykonanie ustawienia obiektu na podaną wartość bez przesyłania potwierdzenia. Zlecenia te są wykonywane wtedy, gdy zmienia się status urządzenia wyrażony w obiektach STATUS_UP_EVENT_MASK i STATUS_DOWN_EVENT_MASK. Implementacja powinna zapewnić niemożność wprowadzenia zleceń na obiektach, do których konfigurujący użytkownik nie ma uprawnień. Domyślna wartość tego obiektu to 0x0512. Typ: max. 25B Dostęp: R / W Wymaganie: R */

    /* Harmonogramy (6.12) */

    SG_OBJ_ID_SCHEDULE_TABLE = 0x601, /* Minimum 8-elementowa tablica z harmonogramami. Kolumny w tej tablicy wymienione poniżej. Obiekt musi wspierać przy odczycie i zapisie co najmniej parametr zlecenia TBL_ACC_IDX (zakres indeksów). Typ: tablicowy Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_YEAR = 0x112, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na rok harmonogramu – 2000+wartość, wartość neutralna 0xFF. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_MONTH = 0x113, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na miesiąc harmonogramu, dopuszczalne wartości to 1-12, wartość neutralna 0xFF. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_DAY_OF_MONTH = 0x114, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na dzień miesiąca harmonogramu, dopuszczalne wartości to od 1 do 31 oznaczające dzień miesiąca oraz od 128 do 158 oznaczające dzień miesiąca, ale licząc od końca miesiąca np. 128 to ostatni dzień miesiąca, 129 to przedostatni, wartość neutralna 0xFF. Błędne ustawienie 31 lutego spowoduje, że harmonogram nigdy się nie wyzwoli. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_DAY_OF_WEEK = 0x115, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na dzień tygodnia harmonogramu kodowany flagami: 1 – niedziela, 2 – poniedziałek, 4 – wtorek, 8 – środa, 16 – czwartek, 32 – piątek, 64 – sobota, wartość neutralna 0xFF. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_HOUR = 0x116, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na godzinę harmonogramu, dopuszczalne wartości to 0-23, wartość neutralna 0xFF. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_MINUTE = 0x117, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na minutę harmonogramu, dopuszczalne wartości to 0-59, wartość neutralna 0xFF. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_LOCAL_TIME = 0x118, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na flaga wskazującą na traktowanie wpisu harmonogramu jako zdefiniowanego w czasie lokalnym. Wartość 0 oznacza czas UTC, 1 oznacza czas lokalny. Typ: 1B Dostęp: - / - Wymaganie: R */
    SG_OBJ_ID_COL_SCHEDULE_COMMAND = 0x510, /* Znacznik kolumny w SCHEDULE_TABLE. Wskazuje na polecenie mające format WO_DATA. Zlecenie GET oznacza przesłanie wskazanych danych do serwera łączem zdarzeń, natomiast zlecenie PUT oznacza wykonanie ustawienia obiektu na podaną wartość bez przesyłania potwierdzenia. Implementacja powinna zapewnić niemożność wprowadzenia harmonogramu ze zleceniami na obiektach do których konfigurujący użytkownik nie ma uprawnień. Typ: max. 25B Dostęp: - / - Wymaganie: R */

    /* Struktury danych (6.13) */

    SG_OBJ_ID_PERIODIC_VOLUME_DATA_LATCHED = 0x511, /* Obiekt zwracający ustaloną w standardzie strukturę danych okresu rozliczeniowego z objętościami gazu za ostatni zatrzaśnięty okres rozliczeniowy. Wartość jest przepisywana z PERIODIC_VOLUME_DATA_CURRENT w momencie zatrzasku – wyznaczana po uzupełnieniu archiwum liczydeł (COUNTER_ARCHIVE), ale przed zamknięciem okresu rozliczeniowego. Struktura wykorzystywana przy zgłoszeniach periodycznych. W tej strukturze COUNTERS_ARCHIVE_LAST_TIME jest zawsze na koniec okresu rozliczeniowego czyli koniec poprzedniej doby gazowniczej, nawet jeżeli w bieżącym okresie pojawiły się nieciągłości i związane z tym wpisy do archiwów. Struktura może nie zawierać danych za pełen okres rozliczeniowy, jeżeli wystąpiła w nim nieciągłość rejestracji – wtedy zawiera dane na jego koniec, w szczególności liczniki, przyrosty od godziny nieciągłości (włącznie) oraz flagę nieciągłości. Bieżące wartości diagnostyczne z tej struktury mogą się zmieniać przy kolejnych odczytach tego obiektu, mimo braku nowego zatrzasku. Dokładny opis przy definicji struktur ustalonych. Typ: max. 255B Dostęp: R / - Wymaganie: V */
    SG_OBJ_ID_PERIODIC_VOLUME_DATA_CURRENT = 0x512, /* Obiekt zwracający ustaloną w standardzie strukturę danych okresu rozliczeniowego z objętościami gazu za bieżący okres rozliczeniowy (od czasu ostatniego zatrzasku periodycznego lub zdarzenia powodującego nieciągłość). Wartość jest wyznaczana po każdorazowym uzupełnieniu archiwum liczydeł (COUNTER_ARCHIVE). Po zatrzasku periodycznym lub zdarzeniu powodującym nieciągłość wartość jest czyszczona. Struktura wykorzystywana typowo przy wysyłaniu asynchronicznym – istotna zmiana czasu, alarmy oraz w harmonogramach gdy dane z trwającego okresu rozliczeniowego są potrzebne wcześniej niż na jego koniec. Bieżące wartości diagnostyczne z tej struktury mogą się zmieniać przy kolejnych odczytach tego obiektu, mimo braku nowego zapisu do archiwum liczydeł. Dokładny opis przy definicji struktur ustalonych. Typ: max. 255B Dostęp: R / - Wymaganie: V */
    SG_OBJ_ID_PERIODIC_VOLUME_DATA_DEF = 0x521, /* Definicja struktury danych obiektów PERIODIC_VOLUME_DATA_XXX w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Obiekt niekonieczny, bo owa struktura jest znana z opisu standardu. Typ: 40B Dostęp: R / - Wymaganie: V */
    SG_OBJ_ID_PERIODIC_ENERGY_DATA_LATCHED = 0x513, /* Obiekt zwracający ustaloną w standardzie strukturę danych okresu rozliczeniowego z energią gazu za ostatni zatrzaśnięty okres rozliczeniowy. Wartość jest przepisywana z PERIODIC_ENERGY_DATA_CURRENT w momencie zatrzasku – wyznaczana po uzupełnieniu archiwum liczydeł (COUNTER_ARCHIVE), ale przed zamknięciem okresu rozliczeniowego. Struktura wykorzystywana przy zgłoszeniach periodycznych. W tej strukturze COUNTERS_ARCHIVE_LAST_TIME jest zawsze na koniec okresu rozliczeniowego czyli koniec poprzedniej doby gazowniczej, nawet jeżeli w bieżącym okresie pojawiły się nieciągłości i związane z tym wpisy do archiwów. Struktura może nie zawierać danych za pełen okres rozliczeniowy, jeżeli wystąpiła w nim nieciągłość rejestracji – wtedy zawiera dane na jego koniec, w szczególności liczniki, przyrosty od godziny nieciągłości (włącznie) oraz flagę nieciągłości. Bieżące wartości diagnostyczne z tej struktury mogą się zmieniać przy kolejnych odczytach tego obiektu, mimo braku nowego zatrzasku. Dokładny opis przy definicji struktur ustalonych. Typ: max. 255B Dostęp: R / - Wymaganie: E */
    SG_OBJ_ID_PERIODIC_ENERGY_DATA_CURRENT = 0x514, /* Obiekt zwracający ustaloną w standardzie strukturę danych okresu rozliczeniowego z energią gazu za bieżący okres rozliczeniowy (od czasu ostatniego zatrzasku periodycznego lub zdarzenia powodującego nieciągłość). Wartość jest wyznaczana po każdorazowym uzupełnieniu archiwum liczydeł (COUNTER_ARCHIVE). Po zatrzasku periodycznym lub zdarzeniu powodującym nieciągłość wartość jest czyszczona. Struktura wykorzystywana typowo przy wysyłaniu asynchronicznym – istotna zmiana czasu, alarmy oraz w harmonogramach gdy dane z trwającego okresu rozliczeniowego są potrzebne wcześniej niż na jego koniec. Bieżące wartości diagnostyczne z tej struktury mogą się zmieniać przy kolejnych odczytach tego obiektu, mimo braku nowego zapisu do archiwum liczydeł. Dokładny opis przy definicji struktur ustalonych. Typ: max. 255B Dostęp: R / - Wymaganie: E */
    SG_OBJ_ID_PERIODIC_ENERGY_DATA_DEF = 0x523, /* Definicja struktury danych obiektów PERIODIC_ENERGY_DATA_XXX w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Obiekt niekonieczny, bo owa struktura jest znana z opisu standardu. Typ: 40B Dostęp: R / - Wymaganie: E */
    SG_OBJ_ID_MONTHLY_DATA_LATCHED = 0x515, /* Obiekt zwracający ustaloną w standardzie strukturę danych miesiąca gazowniczego za zatrzaśnięty miesiąc gazowniczy. Wartość jest przepisywana z MONTHLY_DATA_CURRENT w momencie zatrzasku na końcu miesiąca gazowniczego. Struktura wykorzystywana przy głoszeniach miesięcznych. W tej strukturze LAST_DAILY_LATCH_TIME jest zawsze na koniec poprzedniego miesiąca gazowniczego, nawet jeżeli w bieżącym miesiącu pojawiły się nieciągłości i związane z tym wpisy do archiwum miesięcznego. Struktura może nie zawierać danych za pełen miesiąc gazowniczy, jeżeli wystąpiła w nim nieciągłość rejestracji – wtedy zawiera dane na jego koniec, w szczególności liczniki, przyrosty od dnia nieciągłości (włącznie) oraz flagę nieciągłości. Bieżące wartości diagnostyczne z tej struktury mogą się zmieniać przy kolejnych odczytach tego obiektu, mimo braku nowego zatrzasku miesięcznego. Dokładny opis przy definicji struktur ustalonych. Typ: max. 255B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_MONTHLY_DATA_CURRENT = 0x516, /* Obiekt zwracający ustaloną w standardzie strukturę danych miesiąca gazowniczego za bieżący miesiąc gazowniczy (od czasu ostatniego zatrzasku miesięcznego lub zdarzenia powodującego nieciągłość). Wartość jest wyznaczana po każdorazowym zamknięciu doby gazowniczej. Po zatrzasku miesięcznym lub zdarzeniu powodującym nieciągłość wartość jest czyszczona. Struktura wykorzystywana typowo przy wysyłaniu asynchronicznym – istotna zmiana czasu, alarmy. Bieżące wartości diagnostyczne z tej struktury mogą się zmieniać przy kolejnych odczytach tego obiektu, mimo braku zamknięcia kolejnej doby gazowej. Dokładny opis przy definicji struktur ustalonych. Typ: max. 255B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_MONTHLY_DATA_DEF = 0x525, /* Definicja struktury danych obiektów MONTHLY_DATA_XXX w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Obiekt niekonieczny, bo owa struktura jest znana z opisu standardu. Typ: 40B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_PERIODIC_VOLUME_DATA_ARCHIVE = 0x605, /* Obiekt tablicowy, którego kolumny są zdefiniowane strukturą PERIODIC_VOLUME_DATA_DEF. Archiwum powinno mieć pojemność umożliwiającą archiwizację danych periodycznych minimum z okresu roku przy założeniu cogodzinnej rejestracji przyrostów. Wpisy do archiwum powoduje zatrzask periodyczny oraz zdarzenie powodujące nieciągłość. Archiwum może zawierać więcej wpisów niż jeden dla danej doby gazowniczej jeżeli wystąpiła nieciągłość rejestracji – wpis na zakończenie doby zawiera wtedy flagę nieciągłości. Tabela musi wspierać co najmniej parametry zlecenia TBL_ACC_TIME i TBL_ACC_TIME_BACK, czyli zakresy czasu, jak również TBL_ACC_COLS, czyli wybór kolumn zwracanych. Typ: tablicowy Dostęp: R / - Wymaganie: V */
    SG_OBJ_ID_PERIODIC_ENERGY_DATA_ARCHIVE = 0x606, /* Obiekt tablicowy, którego kolumny są zdefiniowane strukturą PERIODIC_ENERGY_DATA_DEF. Archiwum powinno mieć pojemność umożliwiającą archiwizację danych periodycznych minimum z okresu roku przy założeniu cogodzinnej rejestracji przyrostów. Wpisy do archiwum powoduje zatrzask periodyczny oraz zdarzenie powodujące nieciągłość. Archiwum może zawierać więcej wpisów niż jeden dla danej doby gazowniczej jeżeli wystąpiła nieciągłość rejestracji – wpis na zakończenie doby zawiera wtedy flagę nieciągłości. Tabela musi wspierać co najmniej parametry zlecenia TBL_ACC_TIME i TBL_ACC_TIME_BACK, czyli zakresy czasu, jak również TBL_ACC_COLS, czyli wybór kolumn zwracanych. Typ: tablicowy Dostęp: R / - Wymaganie: E */
    SG_OBJ_ID_MONTHLY_DATA_ARCHIVE = 0x607, /* Obiekt tablicowy, którego kolumny są zdefiniowane strukturą MONTHLY_DATA_DEF. Pojemność archiwum pozostaje w gestii producenta. Wpisy do archiwum powoduje zatrzask miesięczny oraz zdarzenie powodujące nieciągłość. Archiwum może zawierać więcej wpisów niż jeden dla danego miesiąca gazowniczego jeżeli wystąpiła nieciągłość rejestracji – wpis na zakończenie doby zawiera wtedy flagę nieciągłości. Tabela musi wspierać co najmniej parametry zlecenia TBL_ACC_TIME i TBL_ACC_TIME_BACK, czyli zakresy czasu, jak również TBL_ACC_COLS, czyli wybór kolumn zwracanych. Typ: tablicowy Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_USER_STRUCTURE_1 = 0x517, /* Obiekt, którego struktura jest zdefiniowana poprzez tablicę USER_STRUCTURE_1_DEF. Uniwersalny, konfigurowalny obiekt swobodnego wykorzystania. Typ: max. 255B Dostęp: R / - Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_1_DEF = 0x527, /* Definicja struktury danych obiektu USER_STRUCTURE_1 w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Typ: max. 255B Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_2 = 0x518, /* Obiekt, którego struktura jest zdefiniowana poprzez tablicę USER_STRUCTURE_2_DEF. Uniwersalny, konfigurowalny obiekt swobodnego wykorzystania. Typ: max. 255B Dostęp: R / - Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_2_DEF = 0x528, /* Definicja struktury danych obiektu USER_STRUCTURE_2 w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Typ: max. 255B Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_3 = 0x519, /* Obiekt, którego struktura jest zdefiniowana poprzez tablicę USER_STRUCTURE_3_DEF. Uniwersalny, konfigurowalny obiekt swobodnego wykorzystania. Typ: max. 255B Dostęp: R / - Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_3_DEF = 0x529, /* Definicja struktury danych obiektu USER_STRUCTURE_3 w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Typ: max. 255B Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_4 = 0x51A, /* Obiekt, którego struktura jest zdefiniowana poprzez tablicę USER_STRUCTURE_4_DEF. Uniwersalny, konfigurowalny obiekt swobodnego wykorzystania. Typ: max. 255B Dostęp: R / - Wymaganie: R opcja */
    SG_OBJ_ID_USER_STRUCTURE_4_DEF = 0x52A, /* Definicja struktury danych obiektu USER_STRUCTURE_4 w postaci kolejnych OBJ_ID kodowanych na 2B każdy (odpowiada strukturze polecenia WO_DATA ze zleceniem GET dla wszystkich obiektów). Typ: max. 255B Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_COL_OBJ_ID = 0x21C, /* Obiekt służący do oznaczania zawartości kolumny w zmiennych tablicowych, w szczególności w definicjach obiektów struktur danych. Wskazywany zawsze w konwencji Big Endian. Typ: 2B Dostęp: - / - Wymaganie: R opcja */

    /* Parametry protokołu i bezpieczeństwo (6.14) */

    SG_OBJ_ID_WDP_ARP = 0x119, /* Polityka wysyłania żądań potwierdzenia (0x00 - w ogóle, 0x01 - każdy, i - co i-ty oraz ostatni, 0x05 - co piąty oraz ostatni, 0xFF - ostatni). Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_WDP_AT = 0x11A, /* Timeout oczekiwania na potwierdzenie – po upływie timeoutu i braku potwierdzenia datagram jest retransmitowany, wyrażony w sekundach. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_WDP_MR = 0x11B, /* Maksymalna liczba retransmisji datagramu Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_WPP_EVENT_SEND_SN = 0x123, /* Flaga wskazująca na to, czy w transmisji inicjowanej przez urządzenie jest wysyłany numer fabryczny urządzenia w warstwie pakietowej (CTRL_SN=1). Wartość 0 oznacza nie, wartość 1 oznacza tak. Domyślna wartość to 1 – tak. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_WPP_IV_IN = 0x426, /* Największa dotychczas przyjęta wartość wektora inicjalizacyjnego. Typ: 4B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_WPP_KEY = 0x51B, /* Klucz kryptograficzny algorytmu AES-128 służący do szyfrowania, a po przekształceniu także do uwierzytelniania. Typ: 16B Dostęp: - / W Wymaganie: R */
    SG_OBJ_ID_WPP_ACCESS_PASS_TABLE = 0x60D, /* 16-elementowa tablica z hasłami (ACCESS_PASS) do poszczególnych poziomów dostępu. Typ: tablicowy Dostęp: - / W Wymaganie: R opcja */
    SG_OBJ_ID_ACCESS_PASS = 0x427, /* Obiekt służący do oznaczania zawartości kolumny w zmiennych tablicowych, w szczególności w tablicy WPP_ACCESS_PASS_TABLE. Typ: 4B Dostęp: - / - Wymaganie: R opcja */
    SG_OBJ_ID_WPP_CRYPTOGRAPHY_OBLIGATORY = 0x11C, /* Flaga wskazująca czy stosowanie kryptografii w komunikacji do urządzenia jest obowiązkowe. Wartość 1 oznacza obowiązkowe, wartość 0 oznacza nieobowiązkowe. Gdy flaga jest ustawiona, to pakiety bez kryptografii nie są przyjmowane poza pakietami odczytującymi tylko trzy OBJ_ID: SERIAL_NBR, WPP_CRYPTOGRAPHY_OBLIGATORY oraz WPP_IV_IN. W technice SMS i włączonej fladze odczyt ww. obiektów bez kryptografii jest możliwy tylko wtedy, gdy jest włączone filtrowanie numerów telefonów SMS_PHONE_NUMBERS_RESTRICTION. Typ: 1B Dostęp: R / W Wymaganie: R */
    SG_OBJ_ID_WPP_ACCESS_CONTROL_OBLIGATORY = 0x11D, /* Flaga wskazująca czy stosowanie mechanizm ograniczania dostępu do obiektów jest aktywny. Wartość 1 oznacza że jest aktywny i obowiązkowe jest autoryzowanie poziomu dostępu hasłem, wartość 0 oznacza nieobowiązkowe. Gdy flaga jest ustawiona, to odwołanie do obiektów wymagających praw dostępu bez wykazania praw dostępu skończy się niepowodzeniem. Typ: 1B Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_WPP_ACCESS_PERMISSIONS = 0x60E, /* Obiekt tablicowy służący do konfigurowania poziomów dostępu do obiektów. Struktura tablicowa indeksowana jest numerem OBJ_ID i ma kolumny ACCLVL_READ oraz ACCLVL_WRITE, które określają minimalny poziom dostępu do odczytu obiektu i do jego zapisu. Typ: tablicowy Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_COL_ACCLVL_READ = 0x11E, /* Obiekt służący do oznaczania zawartości kolumny w strukturze tablicowej WPP_ACCESS_PERMISSIONS, która wskazuje wymagany poziom uprawnień do odczytu obiektu. Typ: 1B Dostęp: R / W Wymaganie: R opcja */
    SG_OBJ_ID_COL_ACCLVL_WRITE = 0x11F, /* Obiekt służący do oznaczania zawartości kolumny w strukturze tablicowej WPP_ACCESS_PERMISSIONS, która wskazuje wymagany poziom uprawnień do zapisu obiektu. Typ: 1B Dostęp: - / - Wymaganie: R opcja */
    SG_OBJ_ID_SMS_PHONE_NUMBERS_RESTRICTION = 0x125, /* Filtrowanie SMS-ów w oparciu o listę dozwolonych numerów telefonów. 0 – wyłączona obsługa (domyślnie), 1 – obsługa włączona. Włączenie filtrowania powoduje, że rejestrator przyjmuje SMS-y tylko z numeru EVENT_PHONE oraz numerów wymienionych na liście SMS_PHONE_NUMBERS_TABLE, SMS-y odebrane z innych numerów odrzuca. Wyłączenie filtrowania powoduje, że w technice SMS nie są wysyłane potwierdzenia datagramów żądane przez SC. Typ: 1B Dostęp: R / W Wymaganie: S */
    SG_OBJ_ID_SMS_PHONE_NUMBERS_TABLE = 0x60F, /* Minimum 8-elementowa tablica z zawierająca listę telefonicznych numerów zastrzeżonych. Jest zbudowana tylko z jednego obiektu PERMITTED_PHONE_NUMBER. Obiekt musi wspierać przy odczycie i zapisie co najmniej parametr zlecenia TBL_ACC_IDX (zakres indeksów). Typ: tablicowy Dostęp: R / W Wymaganie: S */
    SG_OBJ_ID_PERMITTED_PHONE_NUMBER = 0x51C, /* Numer telefonu kodowany jest w formacie BCD (z numerem kierunkowym kraju, bez zer wiodących i znaku plus). Dla nr. telefonu +48601234567 pole ma wartość: 48601234567FFFFF Typ: 8B Dostęp: - / - Wymaganie: S */

    /* Czas rejestracji archiwum danych periodycznych i miesięcznych (6.15) */

    SG_OBJ_ID_MONTHLY_DATA_LATCH_DAY = 0x120, /* Dzień miesiąca końca miesiąca gazowniczego Typ: 1B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_HOUR = 0x121, /* Godzina końca doby gazowniczej i zarazem miesiąca gazowniczego Typ: 1B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_LTIME = 0x122, /* Flaga czy godzina końca doby gazowniczej i miesiąca gazowniczego wyrażona jest w czasie lokalnym czy w UTC. Wartość 0 oznacza czas UTC, 1 oznacza czas lokalny. Typ: 1B Dostęp: R / W Wymaganie: V/E */
    SG_OBJ_ID_LAST_DAILY_LATCH_TIME = 0x428, /* Czas ostatniego zatrzasku dziennego, czyli ostatniego zakończenia doby gazowniczej. Typ: 4B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_LAST_DAILY_LATCH_VOLUME = 0x429, /* Liczydło objętości gazu wyrażone w jednostce UNIT z czasu ostatniego zakończenia doby gazowniczej. Typ: 4B Dostęp: R / - Wymaganie: V/E */
    SG_OBJ_ID_LAST_DAILY_LATCH_ENERGY = 0x42A, /* Liczydło energii gazu wyrażone w jednostce UNIT z czasu ostatniego zakończenia doby gazowniczej. Typ: 4B Dostęp: R / - Wymaganie: V/E */
};

/*!
 * \brief Typy parametrów zlecenia (pkt 5.5).
 */
enum SG_OBJ_PAR_TYPE
{
    SG_OBJ_PAR_TBL_ACC_IDX = 0, /* Wskazanie zakresu indeksów fizycznych tablicy. */
    SG_OBJ_PAR_TBL_ACC_TIME = 1, /* Wskazanie zakresu czasu. */
    SG_OBJ_PAR_TBL_ACC_TIME_BACK = 2, /* Wskazanie wierszy do określonego czasu. */
    SG_OBJ_PAR_TBL_ACC_COLS = 3, /* Wskazanie kolumn. */
    SG_OBJ_PAR_TBL_ACC_TOTAL_IDX = 4, /* Wskazanie zakresu indeksów całkowitych tablicy cyklicznej. */
    SG_OBJ_PAR_TBL_ACC_COLS_INFO = 5, /* Żądanie zwrócenia listy kolumn z przesyłanymi danymi. */
    SG_OBJ_PAR_TBL_ACC_TIME_BACK_REL = 6, /* Wskazanie zakresu czasu względem aktualnego czasu. */
};

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

/*!
 * \brief Typ błędu obiektu.
 */
typedef enum SG_OBJ_STATUS SG_OBJ_STATUS_t;

/*!
 * \brief Typ błędu operacyjnego funkcji.
 */
typedef enum SG_OBJ_OP_STATUS SG_OBJ_OP_STATUS_t;

/*!
 * \brief Typ identyfikatora obiektu.
 */
typedef enum SG_OBJ_ID SG_OBJ_ID_t;

/*!
 * \brief Typ zawierający typ parametrów zlecenia.
 */
typedef enum SG_OBJ_PAR_TYPE SG_OBJ_PAR_TYPE_t;

/*
 * Warstwa obiektowa (WO) (pkt. 5 i 6)
 */

/*!
 * \brief Typ reprezentujący kierunek przekazywania obiektów.
 */
typedef enum WO_REQ_DIR WO_REQ_DIR_t;

/*!
 * \brief Zlecenie dla obiektu (WO_OBJ) (pkt 5.1).
 */
typedef struct SG_WO_OBJ
{
    /*!
     * \brief Określa kompletność danych obiektu.
     */
    uint8_t valid;

    /*!
     * \brief Flaga błędu zlecenia (OBJ_ERR).
     * \details Informacja czy wystąpił błąd zlecenia.
     *           Flaga jest ustawiana przez rejestrator w sytuacji błędu zlecenia.
     *           SC przesyła obiekty z flagą ustawioną na 0.
     */
    SG_OBJ_STATUS_t err :1;

    /*!
     * \brief Do przyszłych zastosowań. Bity zawsze są skasowane – mają ustawioną wartość 0.
     */
    uint8_t obj_unused :2;

    /*!
     * \brief Typ zlecenia.
     * \details Określenie zlecenia: 0 – odczyt (GET); 1 – zapis (PUT).
     * Kolejne zlecenia w ramach jednego WO_DATA mogą zawierać różne zlecenia i różne obiekty.
     */
    enum SG_OBJ_TSKT
    {
        SG_OBJ_TSKT_GET, SG_OBJ_TSKT_PUT
    } tskt :1;

    /*!
     * \brief Identyfikator obiektu określany przez 12-bitową liczbę.
     * \details Identyfikator ten kodowany jest w konwencji Big Endian
     * np. zlecenie GET na OBJ_ID=0x123 ma postać 0x0123.
     */
    SG_OBJ_ID_t id :12;

    /*!
     * \brief Długość pola parametrów zlecenia.
     * \details Parametr określa długość pola parametrów zlecenia (OBJ_PAR).
     * Parametr występuje, gdy OBJ_ID jest z puli 0x600-0x6FF.
     */
    uint8_t plen;

    /*!
     * \brief Liczba parametrów zlecenia. Pole dodatkowe (spoza specyfikacji.)
     */
    uint8_t par_num;

    /*! \brief Długość danych obiektu.
     * \details Długość pola OBJ_DATA. W trybie dostępu obiektowego długość pola danych musi być identyczna
     * jak wielkość obiektu w rejestratorze. Nie występuje w inicjowanej przez SC zleceniu odczytu,
     * potwierdzeniu wykonania zlecenia zapisu, ani gdy OBJ_ERR=1.
     * Pole długości nie występuje gdy OBJ_ID jest z puli bezpośrednio wskazującej na długość
     * pola danych (pule 1, 2, 3, 4, 9, 10, 12). W przypadku puli 5 oraz 13 pole długości występuje
     * i ma 1B długości, natomiast w przypadku puli 6 pole długości ma 2B długości.
     */
    uint16_t data_len;

    /*!
     * \brief Parametry zlecenia (pkt 5.5)
     * \details Lista parametrów zlecenia ma długość OBJ_PLEN.
     * Parametry zlecenia wykorzystywane są do zleceń wykonywanych na dużych obiektach.
     * Parametr występuje gdy OBJ_ID jest z puli 0x600-0x6FF.
     */
    struct SG_OBJ_PAR
    {
        SG_OBJ_PAR_TYPE_t type; /*!< \brief Typ zlecenia. */
        uint8_t details[SG_OBJ_PAR_MAX_DETAILS]; /*!< \brief Szczegóły zlecenia. */
    } par[SG_OBJ_PAR_MAX_LEN];

    /*!
     * \brief Lista parametrów w postaci nieprzetworzonej.
     */
    uint8_t par_raw[SG_OBJ_PAR_RAW_MAX_LEN];

    /*!
     * \brief Dane obiektu.
     *
     * \details Pole zawierająca dane do zapisu lub odczytane o długości OBJ_LEN
     * lub wskazanej przez pulę OBJ_ID. W przypadku, gdy OBJ_ERR=1 długość pola jest
     * ustalona na 2B.
     * Nie występuje w inicjowanej przez SC zlecenie odczytu. */
    uint8_t data[SG_OBJ_DATA_MAX_LEN];

} SG_WO_OBJ_t;

/*!
 * \brief Definicja jednostki danych warstwy obiektowej (pkt 5.1).
 */
typedef struct SG_WO_OBJ_PDU
{
    /*!
     * \brief Wskaźnik do listy obiektów (WO_DATA).
     *
     * \details Pole zawiera listę kolejno ułożonych zleceń dla obiektów.
     */
    SG_WO_OBJ_t *wo_data_ptr;

    /*!
     * \brief Liczba obiektów.
     */
    uint8_t wo_data_count;

    /*!
     * \brief Maksymalna liczba obiektów.
     */
    uint8_t wo_data_max_count;

#ifdef COMMON_OBJECT_DATA
    /*!
     * \brief Dane obiektów.
     */
    uint8_t data[SG_OBJ_DATA_MAX_LEN * SG_OBJ_DATA_MAX_LEN];
#endif /* DIRECT_OBJECT_DATA */

} SG_WO_PDU_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * \brief Set a data stream from an SMART-GAS object.
 */
SG_OBJ_OP_STATUS_t sg_serialize_obj( enum WO_REQ_DIR dir,
                                     const SG_WO_OBJ_t *obj, uint8_t *data,
                                     const uint16_t max_data_len,
                                     uint16_t *len );

/*!
 * \brief Set the data stream from PDU with SMART-GAS objects.
 */
SG_OBJ_OP_STATUS_t sg_serialize_obj_list( enum WO_REQ_DIR dir,
                                          const SG_WO_PDU_t *list,
                                          uint8_t *data,
                                          const uint16_t max_data_len,
                                          uint16_t *len );

/*!
 * \brief Set an SMART-GAS object from a data stream.
 */
SG_OBJ_OP_STATUS_t sg_deserialize_obj( enum WO_REQ_DIR dir, const uint8_t *data,
                                       const uint16_t data_len,
                                       SG_WO_OBJ_t *obj, uint16_t *obj_size );

/*!
 * \brief Set an PDU SMART-GAS objects from a data stream.
 */
SG_OBJ_OP_STATUS_t sg_deserialize_obj_list( enum WO_REQ_DIR dir,
                                            const uint8_t *data,
                                            const uint16_t data_len,
                                            SG_WO_PDU_t *list );

/*! \brief Return SG OBJ ID name. */
const char* sg_get_obj_id_name( uint16_t obj_id );

/*! \brief Return SG OBJ error name. */
const char* sg_get_obj_error_name( uint16_t error );

/*!
 * \brief Print an SMART-GAS object to str.
 * \return The lenght of output string.
 */
uint32_t sg_obj_to_str( const SG_WO_OBJ_t *obj, char *str,
                        const uint32_t str_max_len );

#endif /* SG_OBJ_H_ */

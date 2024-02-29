/******************************************************************************
 *
 * \file sg_wd.h
 * \brief Definicje warstwy datagramowej (WD) wg protokołu SMART-GAS (IGG-0201:2018 pkt 3).
 *
 * \copyright ELEKTROMETAL SA (c) 2018, Wszelkie prawa zastrzeżone
 * \version $Revision: 296553 $
 * \date $Date: 2020-09-30 11:19:01 +0200 (śr.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef SG_WD_H_
#define SG_WD_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Lista typów całkowitych 8, 16 i 32 bitowych. */
#include "sg_proto.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/* Stałe ogólne i założenia. */
enum SG_DTG_IMPL_GENERAL
{
    SG_WD_SMS_MTU = 140, /* Maksymalna długość datagramu przesyłanego przez SMS. */

    /* Maksymalny rozmiar datagramu w zależności od techniki przekazywania. */
    SG_WD_MAX_DATA_LEN = SG_WD_SMS_MTU, /* Maksymalny rozmiar danych w datagramie (WD) dla SMS. */

    WD_START = 0xFE, /* Znacznik rozpoczęcia datagramu wykorzystywany w w transmisji o charakterze strumieniowym. */
};

/*
 * Dane  elementu informacyjnego (pkt 3.1)
 */
enum IE_I
{
    SG_WD_IE_ID_MULTI_DTG = 0, /* Transmisja multidatagramowa. */
    SG_WD_IE_ID_DETAILS_DTG = 1, /* Transmisja datagramowa strumieniowa (DatagramDetails). */
    SG_WD_IE_ID_MFC_DTG_LOW = 0x80, /* Datagram producenta (dolny zakres). */
    SG_WD_IE_ID_MFC_DTG_HIGH = 0xFF, /* Datagram producenta (górny zakres). */
};

/*!
 * \brief Status of operation on datagram.
 */
enum WD_status
{
    SG_WD_OK, /*!< SG_WD_OK */
    SG_WD_ERROR_INVALID_PARAMTERS, /*!< Invalid input parameters. */
    SG_WD_ERROR_INVALID_OUTPUT_DATA_SIZE, /*!< Invalid the size of output data. */
    SG_WD_ERROR_INVALID_HEADER_SIZE, /*!< Invalid header size. */
    SG_WD_ERROR_INVALID_HEADER_DATA, /*!< Invalid data in the IE header's data. */
    SG_WD_ERROR_INVALID_HEADER_ELEMENT_SIZE, /*!< Invalid IE header's size. */
    SG_WD_ERROR_INVALID_HEADER_ELEMENT_TYPE, /*!< Invalid data in the IE header's type. */
    SG_WD_ERROR_INVALID_HEADER_ELEMENT_DATA, /*!< Invalid data in the IE header's part. */

    SG_WD_ERROR_NOT_IMPLEMENTED, /*!< SG_WD_ERROR_NOT_IMPLEMENTED */

};

/******************************************************************************
* Types and Typedefs
******************************************************************************/

/* Pole danych elementu informacyjnego identyfikacji multidatagramu DTG (dla IE_I=0) (pkt 3.1). */
typedef struct SG_DTG
{
    uint8_t dtg_ref : 7; /* Numer referencyjny multidatagramu.
                          * Każdy multidatagram stanowi rozłączny z innymi multidatagramami
                          * ciąg datagramów. Celem rozróżnienia kolejnych
                          * multidatagramów stosuje się cykliczny (numerowany od 1) numer REF. */
    uint8_t dtg_ar : 1; /* Bit potwierdzenia.
                         * Bit zapewniający kontrolę przepływu w ramach multidatagramu. */
    uint8_t dtg_tc; /* Całkowita liczba datagramów w multidatagramie.
                     * Liczba datagramów, z której składa się dany multidatagram DTG_REF. */
    uint8_t dtg_nr; /* Numer datagramu w multidatagramie.
                     * Numer aktualnie przesyłanego datagramu w ramach multidatagramu DTG_REF,
                     * numerowany od 1 do DTG_TC. */
} SG_DTG_t;

/* Pole danych elementu informacyjnego DatagramDetails DD (dla IE_I=1) (pkt 3.1). */
typedef struct SG_DD
{
    uint8_t dd_len : 8; /* Długość pola danych datagramu. Pole zawiera długość datagramu bez nagłówka, samo WD_DATA. */
    uint16_t dd_crc : 16; /* Suma kontrolna danych datagramu. Wyliczona dla pola danych WD_DATA. Pozwala zweryfikować
     poprawność przesłanych danych według wielomianu CRC CCITT (0xFFFF). */
} SG_DD_t;

/* Definicja elementu informacyjnego (IE). Sekcja charakteryzująca rodzaj zawartości (pkt 3.1). */
typedef struct SG_IE
{
    enum IE_I ie_i : 8; /* Identyfikator elementu informacyjnego. */
    uint8_t ie_l; /* Długość elementu informacyjnego. */
    union SG_IE_D /* Dane elementu informacyjnego. */
    {
        SG_DTG_t dtg; /* Datagram. */
        SG_DD_t dd; /* DatagramDetails. */
    } ie_d;
} SG_IE_t;

/* Definicja nagłówka WD (pkt 3.1). */
typedef struct SG_WD_HDR
{
    uint8_t wd_hl; /* Długość nagłówka WD_HDR poza polami WD_START oraz WD_HL. */
    uint8_t ie_count; /* Liczba elementów informacyjnych. Pole implementacyjne. */
    uint8_t ie_max_count; /* Maksymalna liczba elementów informacyjnych. Pole implementacyjne. */
    SG_IE_t *ie_ptr; /* Elementy informacyjne datagramu. */
} SG_WD_HDR_t;

/* Definicja jednostki WD (pkt 3.1). */
typedef struct SG_WD_PDU
{
    uint8_t wd_start; /* Znacznik rozpoczęcia datagramu przyjmujący wartość 0xFE.
                       * Znacznik wykorzystywany jest w transmisji o charakterze strumieniowym
                       * (np. łącze optyczne, Bluetooth RFCOMM, TCP, UDP), natomiast w transmisji
                       * o charakterze pakietowym (np. SMS, radiowa transmisja lokalna) pole to nie jest stosowane. */
    SG_WD_HDR_t header; /* Nagłówek. */
    uint8_t *data_ptr; /* Dane. */
    uint16_t data_max_len; /* Maksymalny rozmiar danych. */
    uint16_t data_len; /* Rozmiar danych. */
} SG_WD_PDU_t;

/* 
 * Parametry transmisji datagramów (pkt 3.2). 
 */
typedef struct SG_WD_Params
{
    uint8_t wdp_arp; /* Polityka wysyłania żądań potwierdzenia. 
					  * Liczba datagramów co ile wymagane jest potwierdzenie poprawności odbioru. 
					  * Dla wartości innej niż 0x00 ostatni datagram zawsze zawiera żądanie potwierdzenia.
					  * Wartości: 0x00 w ogóle, 0x01 każdy, 0x05 co piąty, 0xFF ostatni. */
    uint8_t wdp_at; /* Timeout oczekiwania na potwierdzenie. 
					 * Maksymalny czas oczekiwania na odebranie potwierdzenia w sekundach. 
					 * Po upływie czasu oczekiwania i nieodebraniu potwierdzenia datagram jest retransmitowany. 
					 * Wartości: 0-255. */
    uint8_t wdp_mr; /* Maksymalna liczba retransmisji. 
					 * Maksymalna liczba powtórzeń (retransmisji) danego datagramu. 
					 * Wartości: 0-255. */
} SG_WD_Params_t;

/*! \brief Type of status for operation. */
typedef enum WD_status WD_status_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* Parametry protokołu. */
extern SG_WD_Params_t sg_wd_params;

/******************************************************************************
 * Functions
 ******************************************************************************/

/*!
 * \brief Serialize the datagram dtg to data with max_len.
 */
WD_status_t sg_serialize_datagram( const SG_WD_PDU_t *dtg, uint8_t *data,
                                   const uint16_t max_len, uint16_t *len );

/*!
 * \brief Serialize the multidatagram dtg to data with max_len.
 */
WD_status_t sg_serialize_multidatagram( SG_WD_PDU_t *dtg, const uint8_t part,
                                        uint8_t *data, const uint16_t max_len,
                                        uint16_t *len );

/*!
 * \brief Serialize the datagram dtg to data with max_len.
 */
WD_status_t sg_deserialize_datagram( const uint8_t *data, const uint16_t len,
                                     SG_WD_PDU_t *dtg );

#endif /* SG_WD_H_ */

/*** end of file ***/

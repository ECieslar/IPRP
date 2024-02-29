/******************************************************************************
*
* \file smart_gas_proto.h
* \brief Definicje protokołu SMART-GAS (IGG-0201:2018).
*
* \copyright ELEKTROMETAL SA (c) 2018, Wszelkie prawa zastrzeżone
* \version $Revision: 293825 $
* \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
* \author $Author: kszczepanski $
*
******************************************************************************/

#ifndef SG_PROTO_H_
#define SG_PROTO_H_

/******************************************************************************
* Includes
******************************************************************************/
#include <stdint.h> /* Lista typów całkowitych 8, 16 i 32 bitowych. */

/******************************************************************************
* Defines and Enumerations
******************************************************************************/

/* Stałe ogólne i założenia. */
enum SG_PROTO_IMPL_GENERAL
{
    SG_PROTO_VER = 2018, /* Wersja protokołu SMART-GAS (rok lub rok miesiąc w przypadku częstszych edycji). */
    SG_PROTO_IMPL_VEN = 07, /* Identyfikator producenta gazomierza.
                                Najstarszy bajt w WP_SN, zgodnie ze wpisem do rejestru IGG
                                "Lista przydzielonych puli numeru seryjnego rejestratora (...)".  */
    SG_PROTO_IMPL_VER = 20190829, /* Wersja implementacji protokołu (rok lub rok miesiąc w przypadku częstszych edycji). */
};

/*!
 * \brief Technika  transmisyjna dla zdarzeń z rejestratora.
 * \details Technikę określa parametr EVENT_LINK [SG:2018] pkt 6.8.
 */
enum SG_EVENT_LINK
{
    SG_LINK_NONE,   /*!< 0 – brak.
                     *       Odpowiedź nie zostanie wysłana,
                     *       ma uzasadnienie tylko dla zlecenia PUT. */
    SG_LINK_SMS,    /*!< 1 – SMS. */
    SG_LINK_GPRS_UDP, /*!< 2 – GPRS – UDP/IP. */
    SG_LINK_GPRS_TCP, /*!< 3 – GPRS – TCP/IP. */
};

/******************************************************************************
* Types and Typedefs
******************************************************************************/

/*!
 * \brief SG_EVENT_LINK type.
 */
typedef enum SG_EVENT_LINK SG_EVENT_LINK_t;


#endif /* SG_PROTO_H_ */

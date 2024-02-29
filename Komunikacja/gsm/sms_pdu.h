/*!****************************************************************************
 *
 * \file sms_pdu.h
 *
 * \copyright ELEKTROMETAL SA (c) 2020, Wszelkie prawa zastrzeżone
 * \version $Revision: 293340 $
 * \date $Date: 2020-09-07 08:02:05 +0200 (pon.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef SMS_PDU_H_
#define SMS_PDU_H_

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h> /* Declarations of integer types with specified width. */
#include <stdbool.h> /* Declaration of bool. */
#include "../test/tinytest.h" /* Test suite definition. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*!
 * \brief Default PDU values.
 *
 * \details see: pt 9.1.2.5  Address fields in 3GPP TS 23.040 Release 15.
 */
enum PDU_TP_ADDR_TYPE
{
    PDU_TP_ADDR_TYPE_COMMON = 0x81U, /* unknown format; 80h + + 01h /telephony numbering plan (NPI)/;  */
    PDU_TP_ADDR_TYPE_INTERNATIONAL = 0x91U, /* 80h + 10h /international number/ + 01h /telephony numbering plan (NPI)/ */
};

enum SMS_PDU_ERROR_CODE
{
    SMS_PDU_OK, /* No error. */
};


/******************************************************************************
 * Types and Typedefs
 ******************************************************************************/

/*!
 * \brief Type of address.
 */
typedef enum PDU_TP_ADDR_TYPE PDU_TP_ADDR_TYPE_t;

/*
 * Address field (clause 9.1.2.5 3GPP TS 23.040 Release 15)
 * see: clause 8.2.5.2  Destination address element 3GPP TS 24.011 Release 15
 * details: details: clause 10.5.4.7 Called party BCD number 3GPP TS 04.08
 */
struct tp_addr
{
    uint8_t length; /* Address-Length: Number of semi-octets in digists part (except SHO), */
    PDU_TP_ADDR_TYPE_t type; /* Type-of-Address */
    uint8_t digits[10]; /* Address-Value: reverse BCD order in byte, odd number of digits,
     bits 5 to 8 of the last octet shall be filled with an end mark coded as "1111"
     (Spare Half Octet). */
};

/* SMS_PDU (clause 9 Protocols and protocol architecture in 3GPP TS 23.040 Release 15) */

/* SMS-SUBMIT type (clause 9.2.2.2)
 *
 * A short message from the MS to the SC.
 */
struct sms_submit
{
    /* TP-Message-Type-Indicator (Mandatory):
     * 0 - SMS-DELIVER (in the direction SC to MS);
     * 1 - SMS-SUBMIT (in the direction MS to SC) */
    uint8_t tp_mti :2;

    /* TP-Reject-Duplicates (Mandatory) */
    uint8_t tp_rd :1;

    /* TP-Validity-Period-Format (Mandatory)
     * Parameter indicating whether or not the TP-VP field is present. */
    uint8_t tp_vpf :2;

    /* Reply-Path (Mandatory) Parameter indicating the request for Reply Path.  */
    uint8_t tp_rp :1;

    /* TP-User-Data-Header-Indicator (Optional)
     * Parameter indicating that the TP-UD field contains a Header. */
    uint8_t tp_udhi :1;

    /* TP-Status-Report-Request (Optional) Parameter indicating
     * if the MS is requesting a status report. */
    uint8_t tp_srr :1;

    /* TP-Message-Reference (Mandatory) Parameter identifying the SMS-SUBMIT. */
    uint8_t tp_mr;

    /* TP-Destination-Address (Mandatory) Size: 2-12o
     * Address of the destination SME. */
    struct tp_addr *tp_da;

    /* TP-PID  TP-Protocol-Identifier (Mandatory)
     * Parameter identifying the above layer protocol, if any. */
    uint8_t tp_pid;

    /* TP-Data-Coding-Scheme (Mandatory)
     * Parameter identifying the coding scheme within the TP-User-Data. */
    uint8_t tp_dcs;

    /* TP-Validity-Period (Optional) Size: o/7o
     * Parameter identifying the time from where the message is no longer valid. */
    uint8_t tp_vp[7];

    /* TP-UDL  TP-User-Data-Length (Mandatory)
     * Parameter indicating the length of the TP-User-Data field to follow. */
    /* cppcheck-suppress cert-API01-C; see: API01-C-EX1 */
    uint8_t tp_udl;

    /* TP-UD  TP-User-Data (Optional)  Size: Dependent on the TP-DCS.
     * Strictly: User-Data pointer. */
    const uint8_t *tp_udp;
};

/* 9.2.2.1  SMS-DELIVER type */
struct sms_delivery
{
    /* TP-Message-Type-Indicator (Mandatory):
     * 0 - SMS-DELIVER (in the direction SC to MS);
     * 1 - SMS-SUBMIT (in the direction MS to SC) */
    uint8_t tp_mti :2;

    /* TP-More-Messages-to-Send  (Mandatory)
     * Parameter indicating whether or not there are more messages to send. */
    uint8_t tp_mms :1;

    /* TP-Loop-Prevention  (Optional)
     * Parameter indicating that SMS applications should inhibit
     * forwarding or automatic message generation that could cause infinite looping. */
    uint8_t tp_lp :1;

    /* TP-RP  TP-Reply-Path  (Mandatory)
     * Parameter indicating that Reply Path exists. */
    uint8_t tp_rp :1;

    /* TP-UDHI  TP-User-Data-Header-Indicator  (Optional)
     * Parameter indicating that the TP-UD field contains a Header. */
    uint8_t tp_udhi :1;

    /* TP-SRI  TP-Status-Report-Indication  (Optional)
     * Parameter indicating if the SME has requested a status report. */
    uint8_t tp_sri :1;

    /* TP-OA  TP-Originating-Address  (Mandatory)  Size: 2-12o
     * Address of the originating SME. */
    struct tp_addr tp_oa;

    /* TP-Protocol-Identifier  (Mandatory)
     * Parameter identifying the above layer protocol, if any. */
    uint8_t tp_pid;

    /* TP-Data-Coding-Scheme  (Mandatory)
     * Parameter identifying the coding scheme within the TP-User-Data. */
    uint8_t tp_dcs;

    /* TP-Service-Centre-Time-Stamp  (Mandatory)  Size: 7o
     * Parameter identifying time when the SC received the message. */
    uint8_t tp_vp[7];

    /* TP-User-Data-Length  (Mandatory)
     * Parameter indicating the length of the TP-User-Data field to follow. */
    /* cppcheck-suppress cert-API01-C; see: API01-C-EX1 */
    uint8_t tp_udl;

    /* TP-User-Data  (Optional)  Size: Dependent on the TP-DCS. */
    const uint8_t *tp_ud;
};

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * \brief Checks if the address has prefix at the front.
 * \param addr Address (phone number).
 * \return Returns true if the address has a prefix '+',
 * or false otherwise.
 */
bool is_prefix_address( const char* addr );

/*!
 * \brief Remove prefix (sign '+') from the phone number (address).
 *
 * \return Returns true if the sign was removed, or false othwerwise.
 */
bool remove_prefix_from_address( char* src );

/*!
 * \brief Convert string to PDU Called party BCD number.
 *
 * \return Number of bytes of pdu number.
 */
uint8_t sms_str_to_pdu_address( const char *src, PDU_TP_ADDR_TYPE_t type, struct tp_addr *addr );

/*!
 * \brief Converts PDU Called party BCD number to string.
 *
 * \return Number of bytes in number's string.
 */
uint8_t sms_pdu_to_str_number( const struct tp_addr *src, char *dst );

/*!
 * \brief Converts Called Party BCD number to binary string.
 *
 * \param addr A number in BCD (Called Party) format.
 * \param data Output data buffer.
 * \param max_len Maximum size of output buffer.
 * \param len The length (number octets) of output string with converted data.
 *
 * \return Returns 0 on success or an error code otherwise.
 */
uint8_t sms_address_serialize( const struct tp_addr * addr,
                              uint8_t* data,
                              const uint8_t max_len,
                              uint8_t* len);


/*!
 * \brief Convert SMS-SUBMIT structure to binary string.
 *
 * \param sms SMS_SUBMIT struct.
 * \param data Output data buffer.
 * \param max_len Maximum size of output buffer.
 * \param len The length (number octets) of output string with converted data.
 *
 * \return Returns 0 on success or an error code otherwise.
 */
uint8_t sms_submit_serialize( const struct sms_submit *sms, uint8_t *data,
                              const uint8_t max_len, uint8_t *len );

/*!
 * \brief Convert message in binary string to SMS-DELIVERY.
 *
 * \param data
 * \param len
 * \param sms
 *
 * \return
 */
uint8_t sms_delivery_deserialize( const uint8_t *data, const uint8_t len,
                                  struct sms_delivery *sms );

/******************************************************************************
* Test suite.
******************************************************************************/

#ifndef TINYTEST_NOTEST
/* Definition of test suite for tinytest. */
TINYTEST_DECLARE_SUITE(sms_pdu);
#endif /* TINYTEST_NOTEST */

#endif /* SMS_PDU_H_ */

/*** end of file ***/

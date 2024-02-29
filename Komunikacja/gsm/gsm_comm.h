/*!****************************************************************************
 *
 * \file GSM_comm.h
 * \brief GSM Communication for the MKG module.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef GSM_COMM_H_
#define GSM_COMM_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Declarations of integer types with specified width. */
#include <stdbool.h> /* Declaration of bool type. */
#include "../test/tinytest.h" /* Test suite declaration. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

#ifndef TINYTEST_NOTEST
/* Definition of test suite for tinytest. */
TINYTEST_DECLARE_SUITE(gsm_comm);
#endif /* TINYTEST_NOTEST */


/* General modem's constants. */
enum Modem_default_timeouts
{
    MODEM_ON_TIME_LIMIT_DEFAULT = 10000, /*!< Timeout for the first response of the modem after turning on. */
    MODEM_CONNECT_TIME_LIMIT_DEFAULT = 20000, /*!< Timeout for connecting the modem to the GSM network. */

};

/*! \brief Modem's power status.  */
enum Modem_status
{
    MODEM_POWER_OFF, /*!< Power off. */
    MODEM_POWER_ON, /*!< Power on. */
};

/*!
 * \brief The GSM general.
 */
enum GSM_general
{
    GSM_SMS_MAX_LENGTH = 140, /*!< The text messages contain up to 140 octets (3GPP TS 23.040 version 15.3.0 clause 3.1). */
    GSM_SMS_READ_TIMEOUT = 1000, /*!< Timeout for waiting on reading SMS from the modem in ms (own experience). */
    GSM_SMS_SEND_DELAY = 5000, /*!< Modem or network consecutive SMS delay in ms (own experience). */
    GSM_SMS_WAITING_TIMEOUT = 10000, /*!< Timeout for waiting for a new SMS from the modem in ms (own experience). */
    GSM_SMS_MAX_COUNT = 20, /*!< Maximum number of text messages. */
};

/*!
 * \brief The GSM transmission modes.
 */
enum GSM_SMS_mode
{
    GSM_SMS_PDU_MODE, /*!< SMS PDU mode */
    GSM_SMS_ASCII_MODE, /*!< SMS ASCII mode */
};

/*!
 * \brief The connection status.
 *
 * \note Enumeration values '0' and '1' results of the status
 *       returned by AT+CREG command.
 */
enum GSM_conn_status
{
    GSM_STATUS_UNKNOWN = -1, /*!< Unknown status. */
    GSM_STATUS_DISCONNECTED = 0, /*!< Disconnected */
    GSM_STATUS_CONNECTING = 2, /*!< Establishing a connection */
    GSM_STATUS_CONNECTED = 1, /*!< Connected */
    GSM_STATUS_DISCONNECTING = 3, /*!< Disconnecting */
};

/*!
 * \brief GSM error codes.
 */
enum GSM_error_code
{
    GSM_OK, /*!< Successful operation. */
    GSM_ERROR_AT_SENDING, /*!< Error while sending AT command. */
    GSM_ERROR_AT_RECEIVING, /*!< Error while receiving a response for AT command. */
    GSM_ERROR_DEVICE_PORT_NOT_DEFINED, /*!< Modem's port not defined. */
    GSM_ERROR_DEVICE_PORT_NOT_READY, /*!< Modem's port not ready. */
    GSM_ERROR_DEVICE_NOT_READY, /*!< Modem not initialized or not ready. */
    GSM_ERROR_DEVICE_NOT_RESPONDING, /*!< No response from modem. */
    GSM_ERROR_UART_READ, /*! Error while reading the UART port. */
    GSM_ERROR_UART_WRITE, /*! Error while writing the UART port. */
    GSM_ERROR_PARSING, /*!< Error while getting answer from modem. */
    GSM_ERROR_DISCONNECTED, /*!< Disconnected from GSM network. */
    GSM_ERROR_INVALID_PARAMETER, /*!< Invalid parameter. */
    GSM_ERROR_INVALID_MODE, /*!< Invalid mode of message. */
    GSM_ERROR_NO_CONFIRMATION, /*!< No confirmation of SMS sending. */
    GSM_ERROR_NO_MESSAGES, /*!< No SMS in SIM memory. */
    GSM_ERROR_MESSAGE_HEADER_INVALID_FORMAT, /*!< Message header invalid format. */
    GSM_ERROR_MESSAGE_HEADER_INVALID_PART_FORMAT, /*!< Message header invalid part format. */
    GSM_ERROR_MESSAGE_CONTENT_TOO_LONG, /*!< The content of the message is too long. */
    GSM_ERROR_NOT_IMPLEMENTED, /*!< Function not implemented. */
    GSM_ERROR_TIMEOUT, /*!< Timeout. */
    GSM_FATAL_ERROR, /*! Fatal error in GSM subsystem. */
    GSM_ERROR_UNKNOWN, /*!< Unknown error. */
};

/******************************************************************************
 * Types and Typedefs
 ******************************************************************************/

/*! \brief GSM operation status type. */
typedef enum GSM_error_code GSM_status_t;

/*! \brief GSM connection status type. */
typedef enum GSM_conn_status GSM_conn_status_t;

/*! \brief GSM message or data type. */
typedef enum GSM_SMS_mode GSM_mode_t;

/*! \brief GSM TA (Terminal Adaptor) information. */
typedef struct _GSM_TA_info
{
    uint8_t imsi[16]; /*!< IMSI in BCD (max. 15 digits) */
    uint8_t imei[16]; /*!< IMEI in BCD (max. 15 digits) */
    uint8_t imeisv[17]; /*!< IMEISV (IMEI + SV) in BCD (max. 16 digits) */
} GSM_TA_info_t;

/*! 
 * \brief GSM time with time zone structure. 
 * \details The format of time with timezone
 * is defined in 3GPP TS 03.40 clause 9.2.3.11 (TP-SCTS), and its value
 * shall be set as defined in 3GPP TS 02.42. 
 */
typedef struct _GSM_time
{
    uint8_t year; /*!< Two last digits. */
    uint8_t month; /*!< Month (from 1). */
    uint8_t day; /*!< Day (from 1). */
    uint8_t hour; /*!< Hour. */
    uint8_t min; /*!< Minute. */
    uint8_t sec; /*!< Second. */
    int8_t tz; /*!< Time Zone (in 15 minutes steps regarding to Universal Time). */
} GSM_time_t;

/*!
 * \brief GSM operator and localization structure.
 */
typedef struct _GSM_op_and_loc
{
    int16_t MCC; /*!< Mobile Country Code wg ITU-T Recommendation E.212 (3 digits). */
    int16_t MNC; /*!< Mobile Network Codes (2 or 3 digits) */
    uint16_t LAC; /*!< Location Area Code (2 octets) */
    uint16_t CID; /*!< Cell Identity (2 octets) */
    uint16_t BSIC; /*!< Base station ID (1 octet,
     *	but changed to 16-bit due to sscanf problem,
     *	see: at_posi_parser function)
     */
    int16_t RxLev;/*!< Signal strength of the base station, expressed by 1 to 64
     *	NOTE There are greater values. Data format changed to 16-bit
     *	due to sscanf problem, see: at_posi_parser function
     */
} GSM_op_and_loc_t;

/*!
 * \brief SMS list struct type.
 */
typedef struct sms_list
{
    uint8_t count; /*!< Count of messages in the list. */
    /*! \brief SMS struct. */
    /* cppcheck-suppress; cert-API01-C; see: API01-C-EX1 */
    struct sms
    {
        uint8_t id; /*!< Message id. */
        enum sms_read_status
        {
            UNREAD, READ
        } read_status;
        enum sms_proces_status
        {
            UNKNWON, NEW, PROCESSED, REJECTED
        } proc_status;
        uint8_t length; /*!< Message content's length. */
        char number[16]; /*!< Message sender's phone number. */
        char date[11]; /*!< Message sending date. */
        char time[12]; /*!< Message sending time. */
        char content[GSM_SMS_MAX_LENGTH + 1]; /*!< Message content. */
    } sms[GSM_SMS_MAX_COUNT]; /*!< SMS table. */
} sms_list_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/*! \brief GSM modem info structure. */
struct _GSM_modem_info
{
    enum Modem_status power; /*!< Modem power status. */
    GSM_conn_status_t conn; /*!< Connection status. */
    int16_t act; /*!< Access technology of the serving cell
                  * (see: 3GPP TS 27.007 version 13.7.0 clause 7.2). */
    GSM_time_t date_time; /*!< Date and time from the GSM Network. */
    GSM_mode_t mode; /*!< Mode of GSM data. */
    uint8_t quality; /*!< Signal quality. */
    uint32_t conn_time; /*!< Connection time in [ms]. */
    uint8_t retries; /*!< Retries number */
    uint32_t sent_count; /*!< Sent bytes count. */
    uint32_t recv_count; /*!< Receives bytes count. */
    GSM_TA_info_t ta_info; /*!< Terminal (TA) information (IMSI, IMEI). */
    GSM_op_and_loc_t loc; /*!< Operator and BTS localization info. */
    char manufacturer[33]; /*!< Manufacturer. */
    char model[33]; /*!< Model. */
    char revision[33]; /*!< Revision. */
    char sn[17]; /*!< Serial number. */
    char cnum[11]; /*!< Subscriber number. */
    uint8_t gw_no[18]; /*!< Phone number to call. */
    uint8_t apn[33]; /*!< APN to connect. */
} gsm_modem_info;

/* The list of messages. */
extern sms_list_t sc_sms_list;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * \brief Set UART port to communicate with GSM modem.
 *
 * \param[in] uart pointer to UART struct
 *
 * \return GSM_OK if initialization passed successfully.
 */
GSM_status_t gsm_init_modem();

/*!
 * \brief Switch on the power for the GSM modem.
 */
void gsm_power_on_modem( void );

/*
 * \brief Waits for response from GSM modem.
 *
 * \return True on successful turning on the modem,
 *         or False on the opposite result.
 */
bool gsm_wait_for_modem_on( const uint32_t time_limit );

/*!
 * \brief Connect the modem to GSM Network.
 * 
 * \details The connection to GSM Network is established
 *  asynchronically and takes much time.
 * The progress in making connection can be checked by calling
 * get_conn_status() function.
 */
void gsm_connect_modem();

/*!
 * \brief Get basic information from GSM modem.
 */
GSM_status_t gsm_get_modem_info();

/*!
 * \brief Get the connection information from the GSM network.
 */
GSM_status_t gsm_get_connection_info();

/*!
 * \brief Waits for connection to GSM Network.
 *
 * \return True on successful connect the modem,
 *         or False on timeout or error in connection.
 */
bool gsm_wait_for_connect();

/*!
 * \brief Set the mode for GSM modem.
 *  *
 * \return GSM_OK if connection is established successfully.
 */
GSM_status_t gsm_set_modem_mode( GSM_mode_t mode );

/*!
 * \brief Get the GSM mode name.
 * \param mode The mode
 * \return Mode string.
 */
const char* gsm_get_modem_mode_name( GSM_mode_t mode );

/*!
 * \brief Get GSM modem mode name.
 *
 * \return GSM mode name
 */
const char* gsm_get_modem_mode_name( GSM_mode_t mode );

/*!
 * \brief Disconnect from GSM network.
 */
void gsm_disconnect_modem( void );

/*!
 * \brief Switch off the power of the modem.
 */
void gsm_power_off_modem( void );

/*!
 * \brief Retrieves status of connection.
 * 
 * \return Connection status information
 */
GSM_conn_status_t gsm_get_modem_connection_status( void );

/*!
 * \brief Get the connection status name.
 *
 * \return Connection status name
 */
const char* gsm_get_modem_connection_status_name( enum GSM_conn_status status );

/*!
 * \brief Get quality metrics about connection with GSM network
 *
 * \return 99 the highest quality of signal.
 * \return 0 no signal.
 */
uint8_t gsm_get_modem_connection_quality( void );

/*!
 * \brief Get the time from GSM Network.
 *
 * \return time_t struct with time since 1970-01-01 00:00:00.
 */
GSM_time_t gsm_get_network_time( void );

/*! 
 * \brief Get a localization from GSM Network.
 * 
 * \return Localization info struct.
 */
GSM_op_and_loc_t gsm_get_operator_and_localization( void );

/*!
 * \brief Gets the SMSC number.
 *
 * \return SMSC number.
 */
const char* gsm_get_smsc( void );

/*!
 * \brief Sets the SMSC number.
 *
 * \details If the input string is empty,
 * internal data will be clear.
 *
 * \param smsc SMSC ASCII format number
 */
GSM_status_t gsm_set_smsc( const char* smsc );

/*! 
 * \brief Send a message in ascii format.
 * 
 * \param [in] smsc_nr SMS Center phone number
 * \param [in] dst_nr destination phone number
 * \param [in] text data to send
 * \param [in] len  length of data to send
 * 
 * \return GSM_OK on successful sending
 */
GSM_status_t gsm_send_text( GSM_mode_t mode,
                            const char *smsc_nr,
                            const char *dst_nr,
                            const uint8_t *text, const uint8_t len );

/*!
 * Wait for time_limit time for arriving a new message.
 * \return GSM_OK on successful arriving a message.
 */
GSM_status_t gsm_wait_for_new_message();

/*! 
 * \brief Receive a text message.
 * 
 * \param [out] src_nr source phone number
 * \param [out] text received data
 * \param [out] len  length of received data
 * \param [in] max_len  maximum length of buffer for received data
 * 
 * \return GSM_OK on successful reception.
 */
GSM_status_t gsm_receive_text( GSM_mode_t mode, uint8_t *src_nr, uint8_t *text,
                               uint8_t *len, uint8_t max_len );

/*! 
 * \brief Receive all text messages.
 *
 * \return GSM_OK on successful reception.
 */
GSM_status_t gsm_receive_texts( GSM_mode_t mode );

/*!
 * \brief Delete message number msg_id.
 *
 * \param msg_id Number of deleted message.
 * \return GSM_OK on succesfull deletation.
 */
GSM_status_t gsm_delete_text( uint8_t msg_id );

/*!
 * \brief Delete all messages.
 *
 * \return GSM_OK on succesfull deletation.
 */
GSM_status_t gsm_delete_texts();

/*!
 * \brief Send a data message.
 * 
 * \param [in] dst_addr dest IP address
 * \param [in] data data to send
 * \param [in] len length of data to send
 * 
 * \return GSM_OK on successful sending
 */
GSM_status_t gsm_send_data( const uint8_t *dst_addr, uint8_t *data,
                            uint8_t len );

/*! 
 * \brief Receive a data message.
 * 
 * \param [out] src_addr source IP address
 * \param [out] data received data
 * \param [out] len  length of received data
 * \param [in] max_len  maximum length of buffer for received data
 * 
 * \return GSM_OK on successful reception.
 */
GSM_status_t gsm_receive_data( uint8_t *src_addr, uint8_t *data, uint8_t *len,
                               uint8_t max_len );

/*!
 * \brief Get sent bytes count.
 *
 * \return Bytes count.
 */
uint32_t gsm_get_sent_bytes_count();

/*!
 * \brief Get received bytes count.
 *
 * \return Bytes count.
 */
uint32_t gsm_get_received_bytes_count();

#endif /* GSM_CONN_H_ */

/*** end of file ***/

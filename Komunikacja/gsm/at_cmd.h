/*!****************************************************************************
 *
 * \file at_cmd.h
 * \brief
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef AT_CMD_H_
#define AT_CMD_H_

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h> /* Declarations of integer types with specified width. */
#include "../test/tinytest.h" /* Test suite declaration. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

#ifndef TINYTEST_NOTEST
/* Definition of test suite for tinytest. */
TINYTEST_DECLARE_SUITE(at_cmd);
#endif /* TINYTEST_NOTEST */


/* General modem's constants. */
enum Modem_general
{
    MODEM_UART_READ_TIMEOUT = 50, /*!< Modem read timeout in ms (typically < 40 ms). */
    MODEM_UART_WRITE_DELAY = 50, /*!< Modem consecutive writes delay in ms. */
};

/*!
 * \brief AT Command type.
 */
enum AT_Cmd_Type
{
    AT_CMD_TYPE_GET, /* Requires: sending command and parsing valuable response. */
    AT_CMD_TYPE_SET, /* Requires: sending command and checking execution status ('OK'). */
    AT_CMD_TYPE_ASYNC /* Requires: no command is sent, response comes asynchronically. */
};

/*!
 * \brief The list of AT commands.
 */
enum AT_Cmd_ID
{
    AT_CMD_AT, /* Send empty AT command. */
    AT_CMD_ATE0, /*  Disabling Terminal Display: ATE0 (3GPP 07.07 clause 4.3, N15 AT Command Manual clause 2.4). */
    AT_CMD_CMEE, /* Report mobile termination error: +CMEE (3GPP 27.007, clause 9.1). */
    AT_CMD_CMSE, /* Message Service Failure Result Code: +CMS ERROR (3GPP TS 27.005, clause 3.2.5). */
    AT_CMD_CGMI, /* Request manufacturer identification: +CGMI (GSM 27.007 clause 5.1). */
    AT_CMD_CGMM, /* Request model identification: +CGMI (GSM 27.007 clause 5.2). */
    AT_CMD_CGMR, /* Request revision identification: +CGMR (GSM 27.007 pkt 5.3). */
    AT_CMD_CGSN, /* Request product serial number identification: +CGSN (GSM 27.007 clause 5.4). */
    AT_CMD_CIMI, /* Request international mobile subscriber identity: +CIMI (GSM 27.007 clause 5.6). */
    AT_CMD_CNUM, /* Request subscriber number: +CNUM (GSM 27.007 clause 7.1). */
    AT_CMD_CREG, /* Request network registration status: +CREG (GSM 27.007 clause 7.2). */
    AT_CMD_CREG_SET_FORMAT_2, /* Set network registration status format: +CREG=2 (GSM 27.007 clause 7.2). */
    AT_CMD_COPS, /* Request PLMN: +COPS  (GSM 27.007 clause 7.3). */
    AT_CMD_CSQ, /* Request signal quality: +CSQ (GSM 27.007 clause 8.5). */
    AT_CMD_GLTS, /* Updating BS Time: +GLTS (N15 AT Command Manual clause 19.6). */
    AT_CMD_GLTS_SET_REQUEST, /* Set updating BS Time: +GLTS (N15 AT Command Manual clause 19.6). */
    AT_CMD_POSI, /* Querying Base Station Information: +POSI (N15 AT Command Manual clause 3.3). */
    AT_CMD_CMGF, /* Message Format: +CMGF (GSM 07.05 clause 3.2.3, N15 AT Command Manual clause 5.2). */
    AT_CMD_CMGS, /* Send Message: +CMGS (GSM 07.05 clause 3.5.1, N15 AT Command Manual clause 5.7). */
    AT_CMD_CMGL, /* Request List Messages +CMGL (GSM 27.005 clause 3.4.2). */
    AT_CMD_CMGD, /* Delete Message +CMGD (GSM 27.005 clause 3.5.4). */
    AT_CMD_CMTI, /* New Message Indications to TE +CNMI (GSM 27.005, clause 3.4.1). */
    AT_CMD_CSCS, /* Select TE Character Set: +CSCS (3GPP TS 27.007, clause 5.5; N15 AT Command Manual clause 5.3). */
    AT_CMD_CSCA, /* Setting Service Centre Address: +CSCA (3GPP TS 27.005, clause 3.3.1 ; N15 AT Command Manual, clause 5.11). */
    AT_CMD_CGREG, /* Checking GPRS Network Registration status: +CGREG (3GPP 27.007 clause 10.1.20; N15 AT Command Manual clause 2.3). */
    AT_CMD_NETAPN, /* Setting Network APN: +NETAPN (N15 AT Command Manual, clause 4.5; no 3GPP equivalent; see also: +CSTT). */
    AT_CMD_XIIC, /* Setting Up PPP Link: +XIIC (N15 AT Command Manual, clause 4.6; no 3GPP AT equivalent). */
    AT_CMD_TCPSETUP, /* Setting Up TCP Connection: +TCPSETUP (N15 AT Command Manual, clause 7.1) */
    AT_CMD_TCPSEND, /* Sending TCP Data: +TCPSEND (N15 AT Command Manual, clause 7.2) */
    AT_CMD_TCPRECV, /* Receiving TCP Data: +TCPRECV (N15 AT Command Manual, clause 7.3) */
    AT_CMD_TCPCLOSE, /* Closing TCP Connection: +TCPCLOSE (N15 Command Manual, clause 7.4) */
    AT_CMD_IPSTATUS, /* Querying TCP/UDP Socket Status: +IPSTATUS (N15 Command Manual, clause 7.5) */
    AT_CMD_LIST_SIZE /* List item count. */
};

/*!
 * \brief AT Command responses.
 */
enum AT_cmd_response
{
    /* Multiple responses (0x00XX). */
    AT_CMD_RESP_AT = 0x0001, /*!< 'AT' token response. */
    AT_CMD_RESP_OK = 0x0002, /*!< 'OK' token response. */
    AT_CMD_RESP_MODEM_STARTUP = 0x0004, /*!< 'MODEM:STARTUP response. */
    AT_CMD_RESP_PBREADY = 0x0008, /*!< '+PBREADY' response. */
    AT_CMD_RESP_ERROR = 0x0010, /*!< 'ERROR' response. */
    AT_CMD_RESP_ATE0 = 0x0020, /*!< 'ATE0' response. */
    /* Single responses (0xXX00). */
    AT_CMD_RESP_CGMI = AT_CMD_CGMI << 8, /*!< 'CGMI ...' response. */
    AT_CMD_RESP_CGMM = AT_CMD_CGMM << 8, /*!< 'CGMM ...' response. */
    AT_CMD_RESP_CGMR = AT_CMD_CGMR << 8, /*!< 'CGMR ...' response. */
    AT_CMD_RESP_CGSN = AT_CMD_CGSN << 8, /*!< 'CGSN ...' response. */
    AT_CMD_RESP_CIMI = AT_CMD_CIMI << 8, /*!< response for CIMI see note in the AT_cmd table. */
    AT_CMD_RESP_CREG = AT_CMD_CREG << 8, /*!< '+CREG ...' response. */
    AT_CMD_RESP_COPS = AT_CMD_COPS << 8, /*!< '+COPS ....' response. */
    AT_CMD_RESP_CSQ = AT_CMD_CSQ << 8, /*!< '+CSQ ....' response. */
    AT_CMD_RESP_GLTS = AT_CMD_GLTS << 8, /*!< '+GLTS ....' response. */
    AT_CMD_RESP_POSI = AT_CMD_POSI << 8, /*!< '+POSI ....' response. */
    AT_CMD_RESP_CMGF = AT_CMD_CMGF << 8, /*!< '+CMGS ....' response. */
    AT_CMD_RESP_CMGS = AT_CMD_CMGS << 8, /*!< '+CMGS ....' response. */
    AT_CMD_RESP_CMGL = AT_CMD_CMGL << 8, /*!< '+CMGL ....' response. */
    AT_CMD_RESP_CMGD = AT_CMD_CMGD << 8, /*!< '+CMGD ....' response. */
    AT_CMD_RESP_CMTI = AT_CMD_CMTI << 8, /*!< '+CMTI ....' response. */
    AT_CMD_RESP_CSCA = AT_CMD_CSCA << 8, /*!< '+CSCA ....' response. */
    AT_CMD_RESP_CGREG = AT_CMD_CGREG << 8, /*!< '+CGREG ....' response. */
    AT_CMD_RESP_NETAPN = AT_CMD_NETAPN << 8, /*!< '+NETAPN ....' response. */
    AT_CMD_RESP_XIIC = AT_CMD_XIIC << 8, /*!< '+XIIC ....' response. */
    AT_CMD_RESP_TCPSETUP = AT_CMD_TCPSETUP << 8, /*! '+TCPSETUP ....' response */
    AT_CMD_RESP_TCPSEND = AT_CMD_TCPSEND << 8, /*! '+TCPSEND ....' response */
    AT_CMD_RESP_TCPRECV = AT_CMD_TCPRECV << 8, /*! '+TCPRECV ....' response */
    AT_CMD_RESP_TCPCLOSE = AT_CMD_TCPCLOSE << 8, /*! '+TCPCLOSE ....' response */
    AT_CMD_RESP_IPSTATUS = AT_CMD_IPSTATUS << 8 /*!< '+IPSTATUS ....' response */
};

/*!
 * \brief AT error codes.
 */
enum AT_status
{
    AT_OK, /*!< Successful operation. */
    AT_ERROR_DEVICE_PORT_NOT_DEFINED, /*!< Modem's port not defined. */
    AT_ERROR_DEVICE_PORT_NOT_READY, /*!< Modem's port not ready. */
    AT_ERROR_DEVICE_NOT_READY, /*!< Modem not initialized or not ready. */
    AT_ERROR_DEVICE_NOT_RESPONDING, /*!< No response from modem. */
    AT_ERROR_UART_READ, /*! Error while reading the UART port. */
    AT_ERROR_UART_WRITE, /*! Error while writing the UART port. */
    AT_ERROR_PARSING, /*!< Error while parsing an answer from modem. */
    AT_ERROR_DISCONNECTED, /*!< Disconnected from GSM network. */
    AT_ERROR_INVALID_PARAMETER, /*!< Invalid parameter. */
    AT_ERROR_NO_CONFIRMATION, /*!< No confirmation of SMS sending. */
    AT_ERROR_NO_MESSAGES, /*!< No SMS in SIM memory. */
    AT_ERROR_NOT_SUPPORTED_FORMAT, /*!< Not supported format. */
    AT_ERROR_NOT_IMPLEMENTED, /*!< Function not implemented. */
    AT_ERROR_PARTIAL_READ, /*!< Insufficied read data count
     * (maybe operation needs more time?). */
    AT_ERROR_UNKNOWN, /*!< Default (unknown) error. */
};

/*! \brief AT parser complex result with int16_t values. */
struct parser_resp_cmplx_i16
{
    int16_t val1;
    int16_t val2;
    int16_t val3;
    int16_t val4;
    int16_t val5;
    int16_t val6;
};

/*! \brief Specilized AT parser struct for message list. */
struct parser_msg_list
{
    volatile uint8_t size; /* The number of messages int the list. */
    char msgs[20][140 + 1]; /* The messages' table. */
};

/******************************************************************************
 * Types and Typedefs
 ******************************************************************************/

/*! \brief AT command id type. */
typedef enum AT_Cmd_ID AT_Cmd_ID_t;

/*! \brief AT command execution status type. */
typedef enum AT_status AT_status_t;

/*! \brief AT command response type. */
typedef enum AT_cmd_response AT_cmd_response_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/*! \brief AT command error code. */
extern uint16_t at_cmd_error;

/*! \brief AT parser simple result with int32_t value. */
extern int32_t at_parser_resp_i32;

/*! \brief AT parser simple result with 32 charactrs length value. */
extern char at_parser_resp_c64[64 + 1];

/*! \brief Specilized AT parser result with date&time value. */
extern uint8_t at_parser_date_time[7];

/*! \brief AT parser complex result with 4 int16_t values. */
extern struct parser_resp_cmplx_i16 at_parser_resp_cmplx_i16;

/*! \brief Specilized AT parser result with messages list. */
extern struct parser_msg_list at_parser_msg_list;

/*! \brief Specilized AT parser result with a new message index. */
extern volatile uint8_t at_parser_msg_new;

/*! \brief Receive bytes counter. */
extern uint32_t recv_bytes_count;

/*! \brief Sent bytes counter. */
extern uint32_t sent_bytes_count;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * \brief Get the command's name.
 */
const char* get_at_command_name( AT_Cmd_ID_t cmd );

/*!
 * \brief Send AT command functions.
 */
AT_status_t send_at_command( AT_Cmd_ID_t cmd, const char *fmt, ... );

/*!
 * \brief Receive AT command functions.
 */
AT_status_t receive_at_response( AT_Cmd_ID_t request,
                                 AT_cmd_response_t *resp_flags,
                                 const uint32_t timeout );

/*!
 * \brief Send content of message.
 *
 * \param text Message content.
 * \param len Message length
 * \return Status of operation.
 */
AT_status_t send_text( const char *text, uint16_t len );

/*!
 * \brief Send the end of content's message end.
 */
AT_status_t send_text_end( void );

/*
 * \brief Cancel message sending.
 */
AT_status_t send_text_cancel( void );

/*!
 * \brief Wait for w prompt for input the message content.
 */
AT_status_t receive_text_prompt( void );

/*!
 * \brief Wait for w prompt for input the data buffer content.
 */
AT_status_t receive_data_prompt( void );

#ifdef TEST
/* Test compatibility. */
uint8_t at_commands_test( void );
#endif /* TEST */

#endif /* AT_CMD_H_ */

/*** end of file ***/

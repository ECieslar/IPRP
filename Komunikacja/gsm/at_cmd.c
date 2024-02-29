/*!****************************************************************************
 *
 * \file at_cmd.c
 *
 * \brief 
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdarg.h> /* Declaration of va_list, va_start(), va_end(), ... */
#include <stdio.h> /* Declaration of sprintf(). */
#include <stdlib.h> /* Declaration of strtol(). */
#include <string.h> /* Declaration of strlen(), strncpy(), memset(). */
#include "at_cmd.h"
#include "../hw/mkg_hw.h"
#include "../util/debug.h" /* PRINT_DEBUG() */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

/*!
 * \brief Parser function definition.
 */
typedef uint16_t parser_func( const char *s, const char *fmt, ... );

/*!
 * \brief The AT command structure with information
 *        about request, response and parsing method.
 */
typedef struct AT_cmd
{
    AT_Cmd_ID_t id; /*!< AT command id. */
    const char *cmd; /*!< AT command string. */
    enum AT_Cmd_Type type; /*!< AT command type. */
    const char *rsp; /*!< Response string. */
    parser_func (*parser); /*!< Pointer to parser. */
    const char *fmt; /*!< Response data format. */
    volatile void *params; /*!< Pointer to parameters. */
} AT_cmd_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* AT parser's result variables. */

/*! \brief AT parser result with int32_t value. */
int32_t at_parser_resp_i32 = 0L;

/*! \brief AT parser result with 64 charactrs length value. */
char at_parser_resp_c64[64 + 1];

/*! \brief Specilized AT parser result with date&time value. */
uint8_t at_parser_date_time[7];

/*! \brief AT parser result with complex 2 int16_t value. */
struct parser_resp_cmplx_i16 at_parser_resp_cmplx_i16;

/*! \brief Specilized AT parser result with messages list. */
struct parser_msg_list at_parser_msg_list;

/*! \brief Specilized AT parser result with a new message index. */
volatile uint8_t at_parser_msg_new;

/*! \brief AT command error code. */
uint16_t at_cmd_error = 0U;

/*! \brief Receive bytes counter. */
uint32_t recv_bytes_count = 0UL;

/*! \brief Sent bytes counter. */
uint32_t sent_bytes_count = 0UL;

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/* Main processing function. */
static uint16_t process_at_token( AT_Cmd_ID_t request, const char *token );

/* Parsing response functions. */
static parser_func at_default_parser;
static parser_func at_creg_parser;
static parser_func at_cops_parser;
static parser_func at_csq_parser;
static parser_func at_cgml_parser;

/* Parsing response function N15 specific. */
static uint16_t at_glts_n15_parser( const char *s, const char *fmt, ... );
static uint16_t at_posi_n15_parser( const char *s, const char *fmt, ... );

/* Modem's transmit functions. */
static uint16_t gsm_modem_transmit( const char *data, const uint16_t len );

/* Modem's receive functions. */
static uint16_t gsm_modem_receive( char *data, const uint16_t max_len,
                                   const uint32_t timeout );

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/*! \brief AT command table for served by modem. */
static AT_cmd_t at_cmd_table[AT_CMD_LIST_SIZE] = {
    /* cmd_id,     cmd,    type,            resp, parser, fmt,   variable */
    [AT_CMD_AT] = { AT_CMD_AT, "AT", AT_CMD_TYPE_SET, "AT", at_default_parser, NULL, NULL },
    [AT_CMD_ATE0] = { AT_CMD_ATE0, "ATE0", AT_CMD_TYPE_SET, "AT", at_default_parser, NULL, NULL },
    [AT_CMD_CMEE] = { AT_CMD_CMEE, "AT+CMEE", AT_CMD_TYPE_GET, "CME", at_default_parser, "ERROR: %d", &at_cmd_error },
    [AT_CMD_CMSE] = { AT_CMD_CMSE, "AT+CMSE", AT_CMD_TYPE_GET, "CMS", at_default_parser, "ERROR: %d", &at_cmd_error },
      /* Modem info commands. */
    [AT_CMD_CGMI] = { AT_CMD_CGMI, "AT+CGMI", AT_CMD_TYPE_GET, "CGMI", at_default_parser, "%32c", &at_parser_resp_c64 },
    [AT_CMD_CGMM] = { AT_CMD_CGMM, "AT+CGMM", AT_CMD_TYPE_GET, "CGMM", at_default_parser, "%32c", &at_parser_resp_c64 },
    [AT_CMD_CGMR] = { AT_CMD_CGMR, "AT+CGMR", AT_CMD_TYPE_GET, "CGMR", at_default_parser, "%32c", &at_parser_resp_c64 },
    [AT_CMD_CGSN] = { AT_CMD_CGSN, "AT+CGSN", AT_CMD_TYPE_GET, "CGSN", at_default_parser, "%15c", &at_parser_resp_c64 },
      /* NOTE The CIMI command doesn't repeat request in response. */
    [AT_CMD_CIMI] = { AT_CMD_CIMI, "AT+CIMI", AT_CMD_TYPE_GET, "", at_default_parser, "%15c", &at_parser_resp_c64 },
    [AT_CMD_CNUM] = { AT_CMD_CNUM, "AT+CNUM", AT_CMD_TYPE_GET, "CNUM", at_default_parser, "%11c", &at_parser_resp_c64 },
      /* Network connection info commands. */
    [AT_CMD_CREG] = { AT_CMD_CREG, "AT+CREG?", AT_CMD_TYPE_GET, "CREG", at_creg_parser, 0UL, &at_parser_resp_cmplx_i16 },
    [AT_CMD_CREG_SET_FORMAT_2] = { AT_CMD_CREG_SET_FORMAT_2, "AT+CREG=2", AT_CMD_TYPE_SET, "CREG", at_default_parser, 0, 0 },
    [AT_CMD_COPS] = { AT_CMD_COPS, "AT+COPS?", AT_CMD_TYPE_GET, "COPS", at_cops_parser, 0UL, &at_parser_resp_cmplx_i16 },
    [AT_CMD_CSQ] = { AT_CMD_CSQ, "AT+CSQ", AT_CMD_TYPE_GET, "CSQ", at_csq_parser, 0UL, &at_parser_resp_i32 },
    [AT_CMD_GLTS] = { AT_CMD_GLTS, "AT+GLTS", AT_CMD_TYPE_ASYNC, "GLTS", at_glts_n15_parser, 0UL, &at_parser_date_time },
    [AT_CMD_GLTS_SET_REQUEST] = { AT_CMD_GLTS_SET_REQUEST, "AT+GLTS=1", AT_CMD_TYPE_SET, "GLTS", at_default_parser, 0, 0 },
    [AT_CMD_POSI] = { AT_CMD_POSI, "AT+POSI=1", AT_CMD_TYPE_GET, "POSI", at_posi_n15_parser, 0UL, &at_parser_resp_cmplx_i16 },
      /* SMS commands. */
    [AT_CMD_CMGF] = { AT_CMD_CMGF, "AT+CMGF", AT_CMD_TYPE_SET, "CMGF", at_default_parser, 0UL, 0UL },
    [AT_CMD_CMGS] = { AT_CMD_CMGS, "AT+CMGS", AT_CMD_TYPE_SET, "CMGS", at_default_parser, "%d", &at_parser_resp_i32 },
    [AT_CMD_CMGL] = { AT_CMD_CMGL, "AT+CMGL", AT_CMD_TYPE_GET, "CMGL", at_cgml_parser, 0UL, &at_parser_msg_list },
    [AT_CMD_CMGD] = { AT_CMD_CMGD, "AT+CMGD", AT_CMD_TYPE_SET, "CMGD", at_default_parser, 0UL, 0UL },
      /* +CMTI: <mem>,<index>, i.e.: +CMTI: "SM",1 */
    [AT_CMD_CMTI] = { AT_CMD_CMTI, "", AT_CMD_TYPE_ASYNC, "CMTI", at_default_parser, "%*s,%d", &at_parser_msg_new },
    [AT_CMD_CSCS] = { AT_CMD_CSCS, "AT+CSCS", AT_CMD_TYPE_SET, "CSCS", at_default_parser, 0UL, 0UL },
    [AT_CMD_CSCA] = { AT_CMD_CSCA, "AT+CSCA", AT_CMD_TYPE_SET, "CSCA", at_default_parser, "%*[\"]%[+0123456789]", &at_parser_resp_c64 },
     /* GPRS commands. */
    [AT_CMD_CGREG] = { AT_CMD_CGREG, "AT+CGREG?", AT_CMD_TYPE_GET, "CGREG", at_creg_parser, 0, &at_parser_resp_cmplx_i16 },
    [AT_CMD_NETAPN] = { AT_CMD_NETAPN, "AT+NETAPN", AT_CMD_TYPE_SET, "NETAPN", at_default_parser, "%64c", &at_parser_resp_c64 },
    [AT_CMD_XIIC] = { AT_CMD_XIIC, "AT+XIIC", AT_CMD_TYPE_GET, "XIIC", at_default_parser, "%*d,%s", &at_parser_resp_c64 },
    /* TCP commands. */
    [AT_CMD_TCPSETUP] = { AT_CMD_TCPSETUP, "AT+TCPSETUP", AT_CMD_TYPE_SET, "TCPSETUP", at_default_parser, "%*d,%s", &at_parser_resp_c64 },
    [AT_CMD_TCPSEND] = { AT_CMD_TCPSEND, "AT+TCPSEND", AT_CMD_TYPE_SET, "TCPSEND", at_default_parser, "%*d,%s", &at_parser_resp_c64 },
    [AT_CMD_TCPRECV] = { AT_CMD_TCPRECV, "", AT_CMD_TYPE_ASYNC, "TCPRECV", at_default_parser, "%*d,%s", &at_parser_resp_c64 },
    [AT_CMD_TCPCLOSE] = { AT_CMD_TCPCLOSE, "AT+TCPCLOSE", AT_CMD_TYPE_SET, "TCPCLOSE", at_default_parser, "%s", &at_parser_resp_c64 },
    [AT_CMD_IPSTATUS] = { AT_CMD_IPSTATUS, "AT+IPSTATUS", AT_CMD_TYPE_GET, "IPSTATUS", at_default_parser, "%s", &at_parser_resp_c64 },
};

/******************************************************************************
 * Functions
 ******************************************************************************/

const char* get_at_command_name( AT_Cmd_ID_t cmd )
{
    return at_cmd_table[cmd].cmd;
}

AT_status_t send_at_command( AT_Cmd_ID_t cmd, const char *fmt, ... )
{
    uint8_t buff[256+1]; /* The buffer for command string. */
    uint16_t buff_len = 0U;

    /* Clear the buffer. */
    memset( buff, 0U, sizeof buff );

    /* Copy command string. */
    snprintf( (char*) buff, ( sizeof buff ) - 1, "%s", at_cmd_table[cmd].cmd );
    buff_len = strlen( (char*) buff );

    /* Check for additional parameters. */
    if ( ( ( (void*) 0 ) != fmt ) && ( fmt[0] != '\0' ) )
    {
        va_list arg; /* Variable argument list handler. */

        /* Add additional parameters string. */
        va_start( arg, fmt );
        vsnprintf( (char*) &buff[buff_len], ( sizeof buff ) - buff_len - 1, fmt, arg );
        va_end( arg );
        buff_len = strlen( (char*) buff );
    }

    if ( ( ( sizeof buff ) - 1 ) > buff_len )
    {
        /* Add the end of line. */
        buff[buff_len++] = '\r';

        /* Send the command. */
        if ( hw_transmit_by_uart( GSM_MODEM_UART, buff, buff_len ) == buff_len )
        {
            return AT_OK;
        }
    }

    return AT_ERROR_UART_WRITE;
}

AT_status_t send_text( const char *text, uint16_t len )
{
    /* Send a raw data, return the count of written bytes.*/
    const uint16_t count = gsm_modem_transmit( text, len );

    /* Check if any written.*/
    if ( 0U == count )
    {
        return AT_ERROR_DEVICE_NOT_RESPONDING;
    }
    /* Check for partial write.*/
    else if ( count != len )
    {
        return AT_ERROR_UART_WRITE;
    }

    return AT_OK;
}

AT_status_t send_text_end( void )
{
    /* CTRL+Z means the end of message */
    if ( 2U == hw_transmit_by_uart( GSM_MODEM_UART, (const uint8_t*) "\x1A\r",
                                    2 ) )
    {
        return AT_OK;
    }

    return AT_ERROR_UART_WRITE;
}

AT_status_t send_text_cancel( void )
{
    /* ESC means the end of message */
    if ( 2U == hw_transmit_by_uart( GSM_MODEM_UART, (const uint8_t*) "\x1B\r",
                                    2 ) )
    {
        return AT_OK;
    }

    return AT_ERROR_UART_WRITE;
}

AT_status_t receive_at_response( AT_Cmd_ID_t request,
                                 enum AT_cmd_response *resp_flags,
                                 const uint32_t timeout )
{
    AT_status_t status = AT_ERROR_UNKNOWN;
    uint8_t buff[1024]; /* XXX Adjust the size of input buffer. */
    uint16_t buff_len = 0U;

    if ( ( (void*) 0UL ) == resp_flags )
    {
        return AT_ERROR_INVALID_PARAMETER;
    }

    memset( buff, 0, sizeof buff );

    /* Get the data from buffer. */
    buff_len = hw_receive_from_uart( GSM_MODEM_UART, buff,
                                    sizeof buff / sizeof buff[0], timeout );

    /* Parse a response */
    if ( 0U == buff_len )
    {
        status = AT_ERROR_DEVICE_NOT_RESPONDING;
    }
    else
    {
        uint16_t index = 0U; /* Index. */
        uint8_t *token = 0UL; /* Token. */
        enum AT_cmd_response tok_res = 0; /* Result of processing token. */

        /* Clear all general purpose parser output variables. */
        at_parser_resp_i32 = 0U;
        memset( at_parser_resp_c64, 0U, sizeof at_parser_resp_c64 );
        memset( &at_parser_resp_cmplx_i16, 0U, sizeof( struct parser_resp_cmplx_i16 ) );

        do
        {
            /* Ignore new line characters. */
            if ( ( '\r' == buff[index] ) && ( ( index + 1 ) < buff_len )
                 && ( '\n' == buff[index + 1] ) )
            {
                buff[index] = '\0';
                buff[index + 1] = '\0';

                if ( ( (void*) 0 ) != token )
                {
                    tok_res |= process_at_token( request, (char*) token );
                    token = 0UL;
                }
            }
            /* Skip break and set the begin of token. */
            else if ( '\0' != buff[index] )
            {
                if ( ( (void*) 0UL ) == token )
                {
                    token = &buff[index];
                }
            }
        }
        while ( index++ < buff_len );

        if ( 0 != tok_res )
        {
            *resp_flags = tok_res;
            status = AT_OK;
        }
        else
        {
            status = AT_ERROR_PARSING;
        }
    }

    return status;
}

AT_status_t receive_text_prompt( void )
{
    AT_status_t status = AT_ERROR_UNKNOWN; /* Status of operation. */
    const char *pattern = { "\r\n> " }; /* Response pattern string. */
    char s[5] = { '\0', '\0', '\0', '\0', '\0' }; /* Response string. */
    uint16_t count; /* Returned bytes count. */

    /* Wait for modem's response (4 character sequence:'<CR><LF><greater_than><space>'). */
    /* NOTE Needed long wait for prompt. */
    count = gsm_modem_receive( s, 4, 3 * MODEM_UART_READ_TIMEOUT );

    PRINT_DEBUG( "Data prompt: len=%d,text=\"%s\"", count, s );

    if ( 0U == count )
    {
        status = AT_ERROR_DEVICE_NOT_RESPONDING;
    }
    else if ( ( 4U == count ) && ( 0U == strncmp( s, pattern, 4 ) ) )
    {
        status = AT_OK;
    }
    else
    {
        status = AT_ERROR_PARTIAL_READ;
    }

    return status;
}

AT_status_t receive_data_prompt( void )
{
    AT_status_t status = AT_ERROR_UNKNOWN; /* Status of operation. */
    const char *pattern = { "\r\n> " }; /* Response pattern string. */
    char s[5] = { '\0', '\0', '\0', '\0', '\0' }; /* Response string. */
    uint16_t count; /* Returned bytes count. */

    /* Wait for modem's response (3 character sequence:'<CR><LF><greater_than>'). */
    /* NOTE Needed long wait for prompt. */
    count = gsm_modem_receive( s, 3, MODEM_UART_READ_TIMEOUT );

    PRINT_DEBUG( "Data buffer prompt: len=%d,text=\"%s\"", count, s );

    if ( 0U == count )
    {
        status = AT_ERROR_DEVICE_NOT_RESPONDING;
    }
    else if ( ( 3U == count ) && ( 0U == strncmp( s, pattern, 3 ) ) )
    {
        status = AT_OK;
    }
    else
    {
        status = AT_ERROR_PARTIAL_READ;
    }

    return status;
}


static uint16_t process_at_token( AT_Cmd_ID_t request, const char *token )
{
    uint16_t proc = 0U;

    /* Validate input parameters. */
    if ( ( ( (void*) 0UL ) == token ) || ( 0U == strlen( token ) ) )
    {
        return 0U;
    }

    /* Command confirmation 'OK'. Most frequent token. */
    if ( ( 'O' == token[0] ) && ( 'K' == token[1] ) )
    {
        proc = AT_CMD_RESP_OK;
    }
    /* Command's response token. */
    else if ( '+' == token[0] )
    {
        /* Lookup for proper command response. */
        for ( uint16_t i = 1; i < AT_CMD_LIST_SIZE; i++ )
        {
            /* Command length. */
            const uint8_t cmd_len = strlen( at_cmd_table[i].rsp );

            /* Compare token with command. */
            if ( ( 0U < cmd_len )
                    && ( 0 == strncmp( &token[1], at_cmd_table[i].rsp, cmd_len ) ) )
            {
                /* Skip whitespace. */
                uint8_t skip = 1;
                while ( ( ' ' == token[cmd_len + skip] )
                        || ( ':' == token[cmd_len + skip] ) )
                {
                    skip++;
                };

                /* Call a parser for command. */
                if ( 0 < at_cmd_table[i].parser( &token[cmd_len + skip],
                                                 at_cmd_table[i].fmt,
                                                 at_cmd_table[i].params ) )
                {
                    proc = ( i << 8 );
                }
                break;
            }
        }
    }
    /* Command error in a simple form. */
    else if ( 0 == strncmp( "ERROR", token, 5 ) )
    {
        proc = AT_CMD_RESP_ERROR;
    }
    /* Token starting with 'AT'. */
    else if ( 0 == strncmp( "AT", token, 2 ) )
    {
        proc = AT_CMD_RESP_AT;
    }
    /* Command confirmation 'ATE0'. */
    else if ( 0 == strncmp( "ATE0", token, 4 ) )
    {
        proc = AT_CMD_RESP_ATE0;
    }
    /* Modem's startup information. */
    else if ( 0 == strncmp( "MODEM:STARTUP", token, 13 ) )
    {
        proc = AT_CMD_RESP_MODEM_STARTUP;
    }
    /* Modem's phone book ready information. */
    else if ( 0 == strncmp( "+PBREADY", token, 8 ) )
    {
        proc = AT_CMD_RESP_PBREADY;
    }
    else /* Unknown token or precedence i.e. CIMI */
    {
        if ( AT_CMD_CIMI == request )
        {
            if ( 0U < at_cmd_table[AT_CMD_CIMI].parser(
                    token, at_cmd_table[AT_CMD_CIMI].fmt,
                    at_cmd_table[AT_CMD_CIMI].params ) )
            {
                proc = AT_CMD_RESP_CIMI;
            }
        }
        else if ( AT_CMD_CMGL == request )
        {
            if ( 0U < at_cmd_table[AT_CMD_CMGL].parser(
                    token, at_cmd_table[AT_CMD_CMGL].fmt,
                    at_cmd_table[AT_CMD_CMGL].params ) )
            {
                proc = AT_CMD_RESP_CMGL;
            }
        }
    }

    return proc;
}

#ifndef TINYTEST_NOTEST
/* Test cases for gprs_enable(). */
static int test_at_default_parser( const char *name __attribute__((unused)) )
{
  uint16_t rc; /* Return code. */
  char rs[64+1]; /* Return string. */

  /* Clear output. */
  memset( rs, 0U, sizeof rs );

  /* Test for true negative. */
  rc = at_default_parser( ( (void*) 0UL ), ( (void*) 0UL ) );
  TINYTEST_EQUAL_MSG( 0U, rc, "For null input parameter function should return 0 (rc=%d)", rc );
  rc = at_default_parser( ( (void*) 0UL ), "" );
  TINYTEST_EQUAL_MSG( 0U, rc, "For null input parameter function should return 0 (rc=%d)", rc );
  rc = at_default_parser( "", ( (void*) 0UL ) );
  TINYTEST_EQUAL_MSG( 0U, rc, "For null input parameter function should return 0 (rc=%d)", rc );
  rc = at_default_parser( "", "" );
  TINYTEST_EQUAL_MSG( 0U, rc, "For null input parameter function should return 0 (rc=%d)", rc );

  /* Test for true positive. */
  /* +CGMI: Neoway Corp Ltd */
  rc = at_default_parser( "Neoway Corp Ltd", "%32c", &rs );
  TINYTEST_EQUAL_MSG( 1U, rc, "For non-empty input parameter function should return 1 (rc=%d)", rc );
  TINYTEST_STR_EQUAL_MSG( "Neoway Corp Ltd", rs, "Comparison of output string failed (rs=%s)", rs );

  /* +NETAPN: "internet","","" */
  rc = at_default_parser( "\"internet\",\"\",\"\"", "%64c", &rs );
  TINYTEST_EQUAL_MSG( 1U, rc, "For non-empty input parameter function should return 1 (rc=%d)", rc );
  TINYTEST_STR_EQUAL_MSG( "\"internet\",\"\",\"\"", rs, "Comparison of output string failed (rs=%s)", rs );

  return 1;
}
#endif /* TINYTEST_NOTEST */

static uint16_t at_default_parser( const char *s, const char *fmt, ... )
{
    uint16_t ret_val; /* Parsing status. */
    va_list arg; /* Variable argument list handler. */
    int32_t scan_res;

    if ( ( ( ( (void*) 0UL ) == s ) )
            || ( ( (void*) 0UL ) == fmt )
            || ( '\0' == fmt[0] ) )
    {
        return 0U;
    }

    va_start( arg, fmt );
    scan_res = vsscanf( s, fmt, arg );

    if ( EOF == scan_res ) /* -1 */
    {
        ret_val = 0U;
    }
    else
    {
        ret_val = (uint16_t) scan_res;
    }

    va_end( arg );

    return ret_val;
}

/*
 * Parse a string in the format: "<f>,<d>[,...]".
 *    where:
 *     <f> is the output format
 *     <d> is the connection status (for values see below)
 *  for f=0
 *      <f>,<d>
 *      i.e. s=0,1
 *  for f=2
 *      <f>,<d>,\"<lac>\",\"<cid>\",<AcT>
 *      i.e. s=2,1,"CF78","0000A931",2
 */
static uint16_t at_creg_parser( const char *s, const char *fmt, ... )
{
    struct parser_resp_cmplx_i16 *ptr = 0; /* Pointer to global variable. */
    uint16_t ret_val; /* Parsing status. */
    va_list arg; /* Variable argument list handler. */
    uint16_t creg_fmt; /* Format of CREG command output. */
    int raw_val; /* Status of registration in the GSM network. */

    if ( ( (void*) 0UL ) == s ) /* fmt and va_args are excluded. */
    {
        return 0U;
    }

    /* Get an access to global variable. */
    va_start( arg, fmt );
    ptr = va_arg( arg, struct parser_resp_cmplx_i16* );
    va_end( arg );

    if ( ( (void*) 0UL ) == ptr )
    {
        return 0U;
    }

    /* Parse the input string. */

    /* Get CREG format. */
    creg_fmt = s[0] - 0x30;

    /* Get registration status. */
    raw_val = s[2] - 0x30;
    ptr->val1 = raw_val;

    /* Parsing successful until now. */
    ret_val = 1;

    /* Get LAC, CID and AcT. */
    if ( 2U == creg_fmt )
    {
        ret_val = sscanf( &s[4], "\"%hX\",\"%hX\",%hd",
                          (uint16_t*) & ( ptr->val2 ),
                          (uint16_t*) & ( ptr->val3 ),
                          (int16_t*) & ( ptr->val4 ) );
    }

    return ret_val;
}

/*
 * Parse a string in format: "<sq>,<ber>".
 */
static uint16_t at_csq_parser( const char *s, const char *fmt, ... )
{
    uint8_t *ptr = 0; /* Pointer to global variable. */
    va_list arg; /* Variable argument list handler. */
    int raw_val; /* Signal quality */

    if ( ( (void*) 0UL ) == s ) /* fmt and va_args are excluded. */
    {
        return 0U;
    }

    /* Get an access to global variable. */
    va_start( arg, fmt );
    ptr = va_arg( arg, uint8_t* );
    va_end( arg );

    if ( ( (void*) 0UL ) == ptr )
    {
        return 0U;
    }

    /* Parse input string. */
    raw_val = strtol( &s[0], (void*) 0UL, 10 );

    /*
     * RSSI linear equation: RSSI = 2 x SQ - 113 [dBm]
     *
     * There is little non-linearity at low signal level (SQ=4) in the N15.
     */
    *ptr = raw_val;

    return 1;
}

/* Parse a string in format: */
static uint16_t at_cops_parser( const char *s, const char *fmt, ... )
{
    struct parser_resp_cmplx_i16 *ptr = 0; /* Pointer to global variable. */
    va_list arg; /* Variable argument list handler. */
    uint16_t ret_val; /* Parsing status. */

    if ( ( (void*) 0UL ) == s ) /* fmt and va_args are excluded. */
    {
        return 0;
    }

    /* Get an access to global variable. */
    va_start( arg, fmt );
    ptr = va_arg( arg, struct parser_resp_cmplx_i16* );
    va_end( arg );

    if ( ( (void*) 0UL ) == ptr )
    {
        return 0;
    }

    /* Parse input string. */
    /* NOTE It seems that sscanf do not support 'hh' scanning parameter for 8-bit variables.
     *      BSIC and RxLev have to be changed for 16-bit variables.
     */
    ret_val = sscanf( s, "%*1d,%*1d,\"%3hd%2hd\"", &ptr->val1, &ptr->val2 );

    return ret_val;
}

/*
 * Parse a string in format: "<yy>/<mm>/<dd>,<hh>:<mm>:<ss>/GMT<+zz>"
 * for examples: 19/11/28,09:43:18/GMT+04
 */
static uint16_t at_glts_n15_parser( const char *s, const char *fmt, ... )
{
    uint8_t *ptr = 0; /* Pointer to global variable. */
    va_list arg; /* Variable argument list handler. */

    if ( ( (void*) 0UL ) == s ) /* fmt and va_args are excluded. */
    {
        return 0;
    }

    /* Get an access to global variable. */
    va_start( arg, fmt );
    ptr = va_arg( arg, uint8_t* );
    va_end( arg );

    if ( ( (void*) 0UL ) == ptr )
    {
        return 0;
    }

    /* Parse the year. */
    ptr[0] = strtol( &s[0], (void*) 0UL, 10 );

    /* Parse the month. */
    ptr[1] = strtol( &s[3], (void*) 0UL, 10 );

    /* Parse the day. */
    ptr[2] = strtol( &s[6], (void*) 0UL, 10 );

    /* Parse the hour. */
    ptr[3] = strtol( &s[9], (void*) 0UL, 10 );

    /* Parse the minutes. */
    ptr[4] = strtol( &s[12], (void*) 0UL, 10 );

    /* Parse the seconds. */
    ptr[5] = strtol( &s[15], (void*) 0UL, 10 );

    /* Parse the time zone.
     * NOTE The time zone contains a sign. */
    ptr[6] = strtol( &s[21], (void*) 0UL, 10 );

    return 1;
}

/*
 * Parse a string: <MODE>,<MCC>,<MNC>,<LAC>,<CI>,<BSIC>,<RxLev>,<ENDED>
 *
 * where:
 *  <MODE>: 1, indicating that all base station information will be read
 *  <MCC>: Country code
 *  <MNC>: Mobile network code, hexadecimal
 *  <LAC>: Area code, hexadecimal
 *  <CI>: Cell ID, hexadecimal
 *  <BSIC>: Base station ID, hexadecimal
 *  <RxLev>: Signal strength of the base station, expressed by 1 to 64
 *  <ENDED>: End symbol.
 *              0 indicates there is more base station information;
 *              1 indicates that this is the last line of the base station
 *                information.
 *
 * For example: 1,260,02,CF78,0000A931,09,66,1
 */
static uint16_t at_posi_n15_parser( const char *s, const char *fmt, ... )
{
    struct parser_resp_cmplx_i16 *ptr = 0; /* Pointer to global variable. */
    va_list arg; /* Variable argument list handler. */
    uint16_t ret_val; /* Parsing status. */

    if ( ( (void*) 0UL ) == s ) /* fmt and va_args are excluded. */
    {
        return 0;
    }

    /* Get an access to global variable. */
    va_start( arg, fmt );
    ptr = va_arg( arg, struct parser_resp_cmplx_i16* );
    va_end( arg );

    if ( ( (void*) 0UL ) == ptr )
    {
        return 0;
    }

    /* Parse input string. */
    /* NOTE It seems that sscanf do not support 'hh' scanning parameter for 8-bit variables.
     *      BSIC and RxLev had to be changed to 16-bit variables.
     */
    ret_val = sscanf( s, "%*1d,%3hd,%2hd,%hX,%hX,%2hX,%2hd", &ptr->val1,
                      &ptr->val2, (uint16_t*) &ptr->val3,
                      (uint16_t*) &ptr->val4, (uint16_t*) &ptr->val5,
                      &ptr->val6 );

    return ret_val;
}

static uint16_t at_cgml_parser( const char *s, const char *fmt, ... )
{
    va_list arg; /* Variable argument list handler. */
    struct parser_msg_list *ptr = 0; /* Pointer to global variable. */
    uint16_t ret_val; /* Parsing status. */

    if ( ( (void*) 0UL ) == s ) /* fmt and va_args are excluded. */
    {
        return 0;
    }

    /* Get an access to global variable. */
    va_start( arg, fmt );
    ptr = va_arg( arg, struct parser_msg_list* );
    va_end( arg );

    if ( ( (void*) 0 ) == ptr )
    {
        return 0;
    }

    /* Parse input string. */
    /* NOTE It seems that sscanf do not support 'hh' scanning parameter for 8-bit variables.
     *
     * ASCII format (+CMGF=1):
     * <\n>+CMGL: 1,"REC READ","+48532742673",,"2019/10/31,09:36:28+01"<\r>
     * <\n>0A0003020C0101033BF48B04000100007005000C00001B001A041B04FFFFFFFF0501000401000000000402001B000004030400000004040C7000000502034D43490200000001<\r>
     * <\n>OK<\r><\n>
     *
     * PDU format (+CMGF=1):
     * +CMGL: <index>,<stat>,[<alpha>],<length><CR><LF><pdu><CR><LF>
     */

    /* The below line doesn't working properly. */
    /* ret_val = sscanf( s, "%1d,\"%*s\",\"%16c\",,\"%20c\"\r\n%140c", &ptr->id, &ptr->number[0], &ptr->time[0], &ptr->data[0] ); */

    /* Determine the range of data.
     * The first line contains a header (message id, phone number and time).
     * The second line contains a message content.
     */
    strncpy( at_parser_msg_list.msgs[at_parser_msg_list.size], s,
             sizeof at_parser_msg_list.msgs[0] - 1 );
    at_parser_msg_list.size += 1;
    ret_val = at_parser_msg_list.size;

    return ret_val;
}

static uint16_t gsm_modem_transmit( const char *data, const uint16_t len )
{
    uint16_t ret_val;

    /* gsm_comm_info.modem_timeout */
    ret_val = hw_transmit_by_uart( GSM_MODEM_UART, (const uint8_t*) data, len );

    /* Update received bytes counter. */
    sent_bytes_count += len;

    return ret_val;
}

static uint16_t gsm_modem_receive( char *data, const uint16_t max_len,
                                   const uint32_t timeout )
{
    uint16_t ret_val;

    /* gsm_comm_info.modem_timeout */
    ret_val = hw_receive_from_uart( GSM_MODEM_UART, (uint8_t*) data, max_len,
                                    timeout );

    /* Update received bytes counter. */
    recv_bytes_count += ret_val;

    return ret_val;
}

uint8_t at_commands_test()
{
    /* GSM modem: AT command test compatibility. */
    for ( uint8_t i = 0; i < AT_CMD_LIST_SIZE; i++ )
    {
        AT_cmd_t *cmd = &at_cmd_table[i];

        if ( cmd && cmd->type != AT_CMD_TYPE_ASYNC )
        {
            send_at_command( cmd->id, 0UL );

            enum AT_cmd_response response_flags;

            receive_at_response( cmd->id, &response_flags,
                                 MODEM_UART_READ_TIMEOUT );

            /* Analyze response. */
        }
    }

    return 0;
}

#ifndef TINYTEST_NOTEST
/* Test suite for the unit. */
TINYTEST_START_SUITE( at_cmd );
  TINYTEST_ADD_TEST( test_at_default_parser, NULL, NULL );
TINYTEST_END_SUITE();
#endif /* TINYTEST_NOTEST */


/*** end of file ***/

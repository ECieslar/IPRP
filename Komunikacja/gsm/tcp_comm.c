/*!****************************************************************************
*
* \file tcp_cli.c
* \brief 
*
* \copyright ELEKTROMETAL SA (c) 2020, Wszelkie prawa zastrzeżone
* \version $Revision$
* \date $Date$
* \author $Author$
*
*
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include <string.h>
#include "tcp_comm.h"
#include "gsm_comm.h"
#include "gprs_comm.h" /* GPRS functions declarations. */
#include "at_cmd.h" /* AT command support functions. */
#include "../util/debug.h" /* PRINT_DEBUG() */

/******************************************************************************
* Defines and Enumerations
******************************************************************************/

enum
{
    TCP_SOCKET_DEFAULT = 0,
    TCP_SOCKET_MAX_SIZE = 4,
    TCP_SEND_HEX_ASCII_MAX_LEN = 512,
    TCP_SEND_ASCII_MAX_LEN = 1024,
    TCP_SEND_BUFF_MAX_LEN = 2048,
    TCP_RECV_TIMEOUT = 10000,
};

/******************************************************************************
* Local Types and Typedefs
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/

/******************************************************************************
* Static Function Prototypes
******************************************************************************/

/******************************************************************************
* Static Variables
******************************************************************************/

/******************************************************************************
* Functions
******************************************************************************/

bool tcp_available( void )
{
    return true;
}


#ifndef TINYTEST_NOTEST

static int test_tcp_connect( const char* name __attribute__((unused)) )
{
    bool rc;
    const char* str1 __attribute__((unused)) = "httpbin.org";
    const char* str2 = "translate.ubidots.com";
    uint16_t p1 __attribute__((unused)) = 80U;
    uint16_t p2 = 9012U;

    /* Test for true postive. */
    rc = tcp_connect( str2, p2 );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while setup TCP connection (rc=%d)", rc );
    hw_delay( 2000UL );
    return 1;
}

#endif /* TINYTEST_NOTEST */

bool tcp_connect( const char *server, uint16_t port )
{
    bool rc = false; /* Return value. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */

    /* Send the +TCPSETUP command. */
    /* AT+TCPSETUP=<n>,<ip>,<port><CR> */
    if ( AT_OK == send_at_command( AT_CMD_TCPSETUP, "=%d,\"%s\",%d", TCP_SOCKET_DEFAULT, server, port ) )
    {
        /* Get results. */
        op_res = receive_at_response( AT_CMD_TCPSETUP, &resp_flags, 10 * MODEM_UART_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) )
        {
            rc = true;

            PRINT_INFO( "TCP connection to \"%s:%d\" is successfull", server, port );
        }
    }

    return rc;
}

#ifndef TINYTEST_NOTEST

static int test_tcp_disconnect( const char* name __attribute__((unused)) )
{
    bool rc;

    /* Test for true postive. */
    rc = tcp_close();
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while closing TCP connection (rc=%d)", rc );
    hw_delay( 1000UL );
    return 1;
}

#endif /* TINYTEST_NOTEST */


bool tcp_close( void )
{
    bool rc = false; /* Return value. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */

    /* Send the +TCPCLOSE command. */
    /* AT+TCPCLOSE=<n><CR> */
    if ( AT_OK == send_at_command( AT_CMD_TCPCLOSE, "=%d", TCP_SOCKET_DEFAULT ) )
    {
        /* Get results. */
        op_res = receive_at_response( AT_CMD_TCPCLOSE, &resp_flags, MODEM_UART_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK | AT_CMD_RESP_TCPCLOSE ) ) )
        {
            rc = true;

            PRINT_INFO( "TCP connection is closed successfully" );
        }
    }

    return rc;
}


bool tcp_is_connected( void )
{
    bool rc = false; /* Return value. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */

    /* Send the +TCPCLOSE command. */
    /* AT+TCPCLOSE=<n><CR> */
    if ( AT_OK == send_at_command( AT_CMD_IPSTATUS, "=%d", TCP_SOCKET_DEFAULT ) )
    {
        /* Get results. */
        op_res = receive_at_response( AT_CMD_IPSTATUS, &resp_flags, MODEM_UART_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_IPSTATUS ) ) )
        {
            char buff[32];

            int scan_rc = sscanf( at_parser_resp_c64, "%*d,%[DISCONET],%*3c,%*d", buff );

            if ( 2 <= scan_rc )
            {
                if ( 0 == strcmp( buff, "CONNECT" ) )
                {
                    rc = true;
                }
            }
            else
            {
                PRINT_ERROR( "Get IP status error (%d)", scan_rc );
            }
        }
    }

    return rc;
}

#ifndef TINYTEST_NOTEST
#include <stdlib.h> /* rand() isn't safe but with salt generated by cpu time seems to have good randomness */

static int test_tcp_send_recv( const char* name __attribute__((unused)) )
{
    bool rc;
    const char* str2 = "FONA/3.0|POST|BBFF-58vFMFvtx9XzFfyLAbTomNCd9esSsv|MKG_01=>test2:%d|end";
    const char* str3 = "464F4E412F332E307C504F53547C4242" \
                       "46462D353876464D4676747839587A46" \
                       "66794C4162546F6D4E43643965735373" \
                       "767C4D4B475F30313D3E74657374333A" \
                       "31303030307C656E64"; /* the same message as in str2 in ASCII-HEX format with test3=10000, length 73 */
    uint16_t rv; /* Random value. */
    char buff[160+1];
    uint16_t buff_len = 0U;

    /* Clear buffer. */
    memset( buff, 0U, sizeof buff );

    /* Test for true postive. */
    /* Generate random value. */
    srand( hw_get_tick_time() );
    rv = rand();

    /* send in ASCII mode. */
    buff_len = snprintf( buff, sizeof buff - 1, str2, rv );
    rc = tcp_send( buff, buff_len, 0 );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while send TCP data (rc=%d)", rc );
    memset( buff, 0U, sizeof buff );
    buff_len = 0U;
    rc = tcp_recv( buff, sizeof buff / sizeof buff[0], &buff_len );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while recv TCP data (rc=%d,data=%s,len=%d)", rc, buff, buff_len );
    /* send in HEX ASCII mode. */
    strcpy( buff, str3 );
    buff_len = 73;
    rc = tcp_send( buff, buff_len, 1 );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while send TCP data (rc=%d)", rc );
    memset( buff, 0U, sizeof buff );
    buff_len = 0U;
    rc = tcp_recv( buff, sizeof buff / sizeof buff[0], &buff_len );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while recv TCP data (rc=%d,data=%s,len=%d)", rc, buff, buff_len );

    return 1;
}

#endif /* TINYTEST_NOTEST */

__attribute__((unused))
bool tcp_send( const char *data, uint16_t len, const int8_t mode )
{
    bool rc = false; /* Return value. */

    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */

    if ( ( 0 == mode && TCP_SEND_ASCII_MAX_LEN >= len ) /* ASCII */
            || ( 1 == mode && TCP_SEND_HEX_ASCII_MAX_LEN >= len ) ) /* HEX */
    {
        /* Send the +TCPSEND command in command mode. */
        /* AT+TCPSEND=<n>[,<length>][,<content>][,<mode>]<CR>  */
        op_res = send_at_command( AT_CMD_TCPSEND, "=%d,%d,\"%s\",%d", TCP_SOCKET_DEFAULT, len, data, mode );
    }
    else if ( TCP_SEND_BUFF_MAX_LEN >= len)
    {
        /* Send the +TCPSEND command in buffer mode. */
        /* AT+TCPSEND=<n>[,<length>],[,<mode>]<CR>  */
        /* > [<content>] */
        if ( AT_OK == send_at_command( AT_CMD_TCPSEND, "=%d,%d,,%d", TCP_SOCKET_DEFAULT, len, mode ) )
        {
            /* Wait for a propmpt */
            op_res = receive_data_prompt();

            if ( AT_OK != op_res )
            {
                PRINT_ERROR( "Error (%d) while waiting for prompt in command %s",
                             op_res, get_at_command_name( AT_CMD_TCPSEND ) );
            }
            else
            {
                /* Send the message content. */
                op_res = send_text( data, ( 1 == mode ) ? 2 * len : len );
            }
        }
    }

    if ( AT_OK == op_res )
    {
        /* Get results. */
        op_res = receive_at_response( AT_CMD_TCPSEND, &resp_flags, 10 * MODEM_UART_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_TCPSEND ) ) )
        {
            PRINT_INFO( "TCP sent data len=%d,\"%s\"", len, data );

            rc = true;
        }
        else
        {
            PRINT_ERROR( "TCP send error (%d)", op_res );
        }
    }

    return rc;
}

/* XXX Increase the received data size. */
bool tcp_recv( char *buff, const uint16_t max_len, uint16_t* len )
{
    bool rc = false; /* Return value. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */
    uint32_t time_limit = (uint32_t) TCP_RECV_TIMEOUT;

    uint32_t start_time; /* Start time of time sensitive operation. */
    bool timeout = false; /* Timeout occures. */

    if ( TCP_SEND_BUFF_MAX_LEN < max_len )
    {
        rc = false;
    }
    else
    {
        /* Start time of operation. */
        start_time = hw_get_tick_time();

        do
        {
            /* Wait for the +TCPRECV command. */
            /* AT+TCPRECV=<n>,<length>,<data><CR>  */
            /* Get results. */
            op_res = receive_at_response( AT_CMD_TCPRECV, &resp_flags, MODEM_UART_READ_TIMEOUT );

            /* Check for valid response. */
            if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_TCPRECV ) ) )
            {
                int scan_rc = sscanf( at_parser_resp_c64, "%hu,%60c", len, buff );

                if ( 2 == scan_rc )
                {
                    PRINT_INFO( "TCP receive data len=%hu,\"%s\"", *len, buff );

                    rc = true;

                    break;
                }
                else
                {
                    PRINT_ERROR( "TCP receive data error (%d)", scan_rc );
                }
                break;
            }
            /* Check for timeout. */
            if ( ( start_time + time_limit ) <= hw_get_tick_time() )
            {
                timeout = true;

                PRINT_INFO( "TCP receive timeout" );
            }
        }
        /* Repeat until not valid response or time out not elsaped. */
        while ( !timeout );
    }

    return rc;
}

/******************************************************************************
* Test suite.
******************************************************************************/

#ifndef TINYTEST_NOTEST

__attribute__((unused))
static int test_setup_gprs_connect( const char* name __attribute__((unused)) )
{
    /* First connect to GSM network if not connected. */
    if ( GSM_STATUS_DISCONNECTED == gsm_get_modem_connection_status() )
    {
        gsm_power_on_modem();
        gsm_connect_modem();
        gsm_wait_for_modem_on( MODEM_ON_TIME_LIMIT_DEFAULT );
        gsm_wait_for_connect( MODEM_CONNECT_TIME_LIMIT_DEFAULT );
    }

    /* if ( !gprs_get_state() ) */
    {
         gprs_enable( true );
         hw_delay(1000UL);
         gprs_get_ip();
    }

    return 1;
}

__attribute__((unused))
static int test_teardown_gprs_connect( const char* name __attribute__((unused)) )
{
    if ( GSM_STATUS_CONNECTED == gsm_get_modem_connection_status() )
    {
        if ( gprs_get_state() )
        {
            gprs_enable( false );
        }

        /* Disconnect from GSM. */
        gsm_disconnect_modem();
        gsm_power_off_modem();

        return 1;
    }

    return 0;
}

/* Test suite for the unit. */
TINYTEST_START_SUITE( tcp_comm );
  /* Test functions are executed in reverse order. */
  TINYTEST_ADD_TEST( test_tcp_disconnect, NULL, test_teardown_gprs_connect );
  TINYTEST_ADD_TEST( test_tcp_send_recv, NULL, NULL );
  TINYTEST_ADD_TEST( test_tcp_connect, test_setup_gprs_connect, NULL );
TINYTEST_END_SUITE();

#endif /* TINYTEST_NOTEST */

/*** end of file ***/

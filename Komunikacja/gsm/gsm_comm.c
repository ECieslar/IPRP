/*!****************************************************************************
 *
 * \file gsm_comm.c
 * \brief GSM communication implementation.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 * \mainpage GSM communication
 *
 * \section modes Tryby połączenia
 * \subsection text_mode SMS
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdbool.h>
#include <string.h> /* Declaration of memset(). */
#include <stdlib.h> /* Declaration of atoi(). */
#include "at_cmd.h"
#include "gsm_comm.h"
#include "sms_pdu.h"
#include "../hw/mkg_hw.h"
#include "../util/str_util.h"
#include "../util/debug.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/*! \brief Text messages list as result of `+CGML` command. */
sms_list_t sc_sms_list;

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/*!
 * \brief Normalize connection status value.
 * \param value Status got from the modem
 * \return Normalized status.
 */
static GSM_status_t get_merged_connection_status( int value );

/*! \brief Check for valid SMS header format.
 *
 * \return True on successfull operation, other case retrun False.
 */
static bool check_message_header_format( const char *header );

/*! \brief Parse input message for parts od message headers. */
static bool parse_message_header( const char *header, struct sms *message );

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/*! \brief GSM connection information names. */
static const char *GSM_conn_status_name[] =
    { "UNKNOWN", /* GSM_STATUS_UNKNOWN */
      "DISCONNECTED", /* GSM_STATUS_DISCONNECTED */
      "CONNECTING", /* GSM_STATUS_CONNECTING */
      "CONNECTED", /* GSM_STATUS_CONNECTED */
      "DISCONNECTING", /* GSM_STATUS_DISCONNECTING */
    };

/*! \brief GSM mode names. */
static const char *GSM_mode_name[] =
    { "SMS_PDU", /* GSM_SMS_PDU_MODE */
      "SMS_ASCII", /*	GSM_SMS_ASCII_MODE */
      "GPRS_UDP", /* GSM_GPRS_UDP_MODE */
      "GPRS_TCP", /* GSM_GPRS_TCP_MODE */
      "GPRS_TCP_SSL", /* GSM_GPRS_TCP_SSL_MODE */
    };

/*!
 * \brief SMS Centre (SMSC) number.
 * \details SMSC in ASCII format.
 *       May contains the prefix. */
static char gsm_smsc[20+1];

/******************************************************************************
 * Functions
 ******************************************************************************/

void gsm_power_on_modem( void )
{
    /* Switching power on the voltage regulator U1 to supply the modem. */
    hw_set_port_pin_state( PORTB, DC_EN_PB02_Pin, 1U );

    /* Wait for power stabilization. */
    hw_delay( 10U );

    /* Clear the modem info structure. */
    memset( &gsm_modem_info, 0U, sizeof gsm_modem_info );

    /* Setting the modem power status. */
    gsm_modem_info.power = MODEM_POWER_ON;
}

bool gsm_wait_for_modem_on( const uint32_t time_limit )
{
    uint32_t start_time; /* The start time of time sensitive operation. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* The result of command. */
    AT_cmd_response_t resp_flags; /* The response's flags. */
    bool timeout = false; /* Timeout occures. */
    bool modem_on = false; /* The status of turning on modem. */

    /* Clear the uart buffer. */
    hw_flush_uart( GSM_MODEM_UART );

    /* Start time of operation. */
    start_time = hw_get_tick_time();

    /* Auto-detection of baud rate (default 115200 b/s).
     * As alternative the buad rate can be set by command: `AT+IPR=115200`.
     */
    if ( AT_OK == send_at_command( AT_CMD_AT, 0UL ) )
    {
        if ( AT_OK == receive_at_response( AT_CMD_AT, &resp_flags,
                                           MODEM_UART_READ_TIMEOUT ) )
        {
            PRINT_INFO( "Send AT for auto-detection of baud rate" );
        }
    }

    /* Wait for a response from the modem. */
    do
    {
        /* Send the AT command and get the response. */
        if ( AT_OK == send_at_command( AT_CMD_AT, 0UL ) )
        {
            op_res = receive_at_response( AT_CMD_AT, &resp_flags,
                                          MODEM_UART_READ_TIMEOUT );
        }

        /* Check for timeout. */
        if ( ( start_time + time_limit ) <= hw_get_tick_time() )
        {
            timeout = true;
        }
    }
    while ( ! ( ( AT_OK == op_res ) /* Not successful receive of the response */
                && ( ( AT_CMD_RESP_AT | AT_CMD_RESP_OK ) & resp_flags ) ) /* nor not AT and OK response. */
            && !timeout ); /* still on time */

    /* Analyse termination's cause. */
    /* The response analysis has precedence over timeout. */
    if ( ( AT_OK == op_res ) && ( ( AT_CMD_RESP_AT | AT_CMD_RESP_OK )
            & resp_flags ) )
    {
        modem_on = true;
    }
    else if ( timeout )/* Premature return due to timeout. */
    {
        PRINT_DEBUG( "Timeout while waiting for modem response on command %s",
                     get_at_command_name( AT_CMD_AT ) );

        modem_on = false;
    }

    /* Turn off the command's echo. */
    if ( modem_on && ( AT_OK == send_at_command( AT_CMD_ATE0, 0UL ) ) )
    {
        op_res = receive_at_response( AT_CMD_ATE0, &resp_flags,
                                      MODEM_UART_READ_TIMEOUT );

        if ( ( AT_OK == op_res ) && ( ( AT_CMD_RESP_ATE0 | AT_CMD_RESP_OK )
                == resp_flags ) )
        {
            PRINT_DEBUG( "Echo turned off successfully" );

            return true;
        }
    }

    /* Turn on simple error reporting. */
    if ( modem_on && ( AT_OK == send_at_command( AT_CMD_CMEE, "=1" ) ) )
    {
        op_res = receive_at_response( AT_CMD_ATE0, &resp_flags,
                                      MODEM_UART_READ_TIMEOUT );

        if ( ( AT_OK == op_res ) && ( ( AT_CMD_RESP_OK )
                == resp_flags ) )
        {
            PRINT_DEBUG( "Extended format error reporting (1) turned on successfully" );

            return true;
        }
    }

    return false;
}

void gsm_connect_modem( void )
{
    /* Switching power on the modem by shortcut PWRKEY pin for 1,5 s. */
    hw_set_port_pin_state( PORTB, PWR_KEY_PB03_Pin, 1 );
    hw_delay( 1500 );
    hw_set_port_pin_state( PORTB, PWR_KEY_PB03_Pin, 0 );

    /* Setting the status. */
    gsm_modem_info.conn = GSM_STATUS_CONNECTING;
}

bool gsm_wait_for_connect( const uint32_t time_limit )
{
    uint32_t start_time; /* Start time of time sensitive operation. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */
    bool timeout = false; /* Timeout occures. */

    /* Clear the uart buffer. */
    hw_flush_uart( GSM_MODEM_UART );

    /* Start time of operation. */
    start_time = hw_get_tick_time();

    do
    {
        /* Send the "CREG?" command. */
        if ( AT_OK == send_at_command( AT_CMD_CREG, 0UL ) )
        {
            /* Get results. */
            op_res = receive_at_response( AT_CMD_CREG, &resp_flags,
                                          MODEM_UART_READ_TIMEOUT );

            /* Check for valid response. */
            if ( ( AT_OK == op_res ) && ( resp_flags & AT_CMD_RESP_CREG ) )
            {
                /* Get the value from the response. */
                gsm_modem_info.conn = get_merged_connection_status(
                        at_parser_resp_cmplx_i16.val1 );
            }
        }

        /* Check for timeout. */
        if ( ( start_time + time_limit ) <= hw_get_tick_time() )
        {
            timeout = true;
        }
    }
    /* Repeat until not valid response or time out not elsaped. */
    while ( ! ( ( AT_OK == op_res )
            && ( ( AT_CMD_RESP_CREG | AT_CMD_RESP_OK ) == ( resp_flags
                    & ( AT_CMD_RESP_CREG | AT_CMD_RESP_OK ) ) )
            && ( GSM_STATUS_CONNECTED == gsm_modem_info.conn ) )
            && !timeout );

    /* Analyse termination's cause. */
    /* The response analysis has precedence over timeout. */
    if ( ( AT_OK == op_res ) && ( ( AT_CMD_RESP_CREG | AT_CMD_RESP_OK )
            == ( resp_flags & ( AT_CMD_RESP_CREG | AT_CMD_RESP_OK ) ) )
         && ( GSM_STATUS_CONNECTED == gsm_modem_info.conn ) )
    {
        PRINT_DEBUG(
                "Modem is connected to GSM network successfully (creg=%s)",
                gsm_get_modem_connection_status_name( gsm_modem_info.conn ) );

        return true;
    }
    else if ( timeout )/* Premature return due to timeout. */
    {
        PRINT_ERROR( "Timeout while waiting for modem response on command %s",
                     get_at_command_name( AT_CMD_CREG ) );
    }

    return false;
}

GSM_status_t gsm_get_modem_info( void )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation result. */
    AT_cmd_response_t resp_flags; /* Modem's response flags. */
    /* Table of commands to get manufacturer, model, revision, serial number and IMSI. */
    const enum AT_Cmd_ID cmds[] =
        { AT_CMD_CGMI, AT_CMD_CGMM, AT_CMD_CGMR, AT_CMD_CGSN, AT_CMD_CIMI };

    /* Clear the uart buffer. */
    hw_flush_uart( GSM_MODEM_UART );

    /* Get modem info loop. */
    for ( uint8_t i = 0; i < ( sizeof cmds / sizeof cmds[0] ); i++ )
    {
        /* Send the command. */
        if ( AT_OK != send_at_command( cmds[i], 0UL ) )
        {
            PRINT_ERROR( "Error (%d) while sendig command %s", op_res,
                         get_at_command_name( cmds[i] ) );

            return GSM_ERROR_AT_SENDING;
        }
        else /* AT_OK */
        {
            /* Get a response.
             * NOTE CIMI takes even 550 ms. */
            op_res = receive_at_response( cmds[i], &resp_flags,
                                          60 * MODEM_UART_READ_TIMEOUT );
            if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( cmds[i] << 8 ) ) ) )
            {
                PRINT_ERROR( "Error (%d) while getting response for command %s",
                             op_res, get_at_command_name( cmds[i] ) );

                return GSM_ERROR_AT_RECEIVING;
            }
            else /* AT_OK */
            {
                PRINT_DEBUG( "Response for command %s (%s) got successfully",
                             get_at_command_name( cmds[i] ),
                             at_parser_resp_c64 );

                /* Get the result. */
                switch ( cmds[i] )
                {
                    case AT_CMD_CGMI:
                        strncpy( gsm_modem_info.manufacturer,
                                 at_parser_resp_c64,
                                 sizeof gsm_modem_info.manufacturer - 1 );
                        break;
                    case AT_CMD_CGMM:
                        strncpy( gsm_modem_info.model, at_parser_resp_c64,
                                 sizeof gsm_modem_info.model - 1 );
                        break;
                    case AT_CMD_CGMR:
                        strncpy( gsm_modem_info.revision, at_parser_resp_c64,
                                 sizeof gsm_modem_info.revision - 1 );
                        break;
                    case AT_CMD_CGSN:
                        strncpy( (char*) gsm_modem_info.ta_info.imei,
                                 at_parser_resp_c64,
                                 sizeof gsm_modem_info.ta_info.imei - 1 );
                        break;
                    case AT_CMD_CIMI:
                        strncpy( (char*) gsm_modem_info.ta_info.imsi,
                                 at_parser_resp_c64,
                                 sizeof gsm_modem_info.ta_info.imsi - 1 );
                        break;

                    /* Unused commands. */
                    case AT_CMD_AT:
                    case AT_CMD_ATE0:
                    case AT_CMD_CMEE:
                    case AT_CMD_CMSE:
                    case AT_CMD_CNUM:
                    case AT_CMD_CREG:
                    case AT_CMD_CREG_SET_FORMAT_2:
                    case AT_CMD_COPS:
                    case AT_CMD_CSQ:
                    case AT_CMD_GLTS:
                    case AT_CMD_GLTS_SET_REQUEST:
                    case AT_CMD_POSI:
                    case AT_CMD_CMGF:
                    case AT_CMD_CMGS:
                    case AT_CMD_CMGL:
                    case AT_CMD_CMGD:
                    case AT_CMD_CMTI:
                    case AT_CMD_CSCS:
                    case AT_CMD_CSCA:
                    case AT_CMD_CGREG:
                    case AT_CMD_NETAPN:
                    case AT_CMD_XIIC:
                    case AT_CMD_TCPSETUP:
                    case AT_CMD_TCPSEND:
                    case AT_CMD_TCPRECV:
                    case AT_CMD_TCPCLOSE:
                    case AT_CMD_IPSTATUS:
                    case AT_CMD_LIST_SIZE:
                    default:
                        break;
                }
            }
        }
    }

    return GSM_OK;
}

GSM_status_t gsm_get_connection_info( void )
{
    GSM_status_t ret_val = GSM_OK; /* Returned value as aggragation particle errors. */
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */

    /* Table of commands to get MCC&MNC, BSS localization, also for N15 as POSI command and BSS signal strength. */
    const enum AT_Cmd_ID cmds[] =
        {
        /* Send COPS command for getting MCC and MNC codes.
         * To use only for non AT+POSI modem implementations. */
        AT_CMD_COPS,
          /* Send CREG=2 command for getting BSS localization.
           * To use only for non AT+POSI modem implementations. */
          AT_CMD_CREG_SET_FORMAT_2,
          AT_CMD_CREG,
          /* ... or compact N15 function. */
          AT_CMD_POSI,
          /* Getting signal strength .*/
          AT_CMD_CSQ };

    /* Clear the uart buffer. */
    hw_flush_uart( GSM_MODEM_UART );

    for ( uint8_t i = 0; i < ( sizeof cmds / sizeof cmds[0] ); i++ )
    {
        GSM_status_t part_val = GSM_ERROR_UNKNOWN; /* Value returned by a single operation. */

        /* Send a command. */
        if ( AT_OK != send_at_command( cmds[i], 0UL ) )
        {
            PRINT_ERROR( "Error (%d) while sendig command %s", op_res,
                         get_at_command_name( cmds[i] ) );

            part_val = GSM_ERROR_AT_SENDING;
        }
        else /* AT_OK */
        {
            /* Get a response.
             * NOTE COPS takes even 150 ms (typ. 40 ms).  */
            op_res = receive_at_response( cmds[i], &resp_flags,
                                          4 * MODEM_UART_READ_TIMEOUT );
            if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( cmds[i] << 8 ) ) ) )
            {
                PRINT_ERROR( "Error (%d) while getting response for command %s",
                             op_res, get_at_command_name( cmds[i] ) );

                part_val = GSM_ERROR_AT_RECEIVING;
            }
            else /* AT_OK */
            {
                PRINT_DEBUG(
                        "Response for command %s (complex value) got successfully",
                        get_at_command_name( cmds[i] ) );

                /* Get the result. */
                switch ( cmds[i] )
                {
                    case AT_CMD_COPS:
                        gsm_modem_info.loc.MCC = at_parser_resp_cmplx_i16.val1;
                        gsm_modem_info.loc.MNC = at_parser_resp_cmplx_i16.val2;
                        break;
                    case AT_CMD_CREG:
                        gsm_modem_info.conn = get_merged_connection_status(
                                at_parser_resp_cmplx_i16.val1 );
                        gsm_modem_info.loc.LAC = at_parser_resp_cmplx_i16.val2;
                        gsm_modem_info.loc.CID = at_parser_resp_cmplx_i16.val3;
                        gsm_modem_info.act = at_parser_resp_cmplx_i16.val4;
                        break;
                    case AT_CMD_POSI:
                        gsm_modem_info.loc.MCC = at_parser_resp_cmplx_i16.val1;
                        gsm_modem_info.loc.MNC = at_parser_resp_cmplx_i16.val2;
                        gsm_modem_info.loc.LAC = at_parser_resp_cmplx_i16.val3;
                        gsm_modem_info.loc.CID = at_parser_resp_cmplx_i16.val4;
                        gsm_modem_info.loc.BSIC = at_parser_resp_cmplx_i16.val5;
                        gsm_modem_info.loc.RxLev =
                                at_parser_resp_cmplx_i16.val6;
                        break;
                    case AT_CMD_CSQ:
                        gsm_modem_info.quality = at_parser_resp_i32;
                        break;

                    /* Unused commands. */
                    case AT_CMD_AT:
                    case AT_CMD_ATE0:
                    case AT_CMD_CMEE:
                    case AT_CMD_CMSE:
                    case AT_CMD_CGMI:
                    case AT_CMD_CGMM:
                    case AT_CMD_CGMR:
                    case AT_CMD_CGSN:
                    case AT_CMD_CIMI:
                    case AT_CMD_CNUM:
                    case AT_CMD_CREG_SET_FORMAT_2:
                    case AT_CMD_GLTS:
                    case AT_CMD_GLTS_SET_REQUEST:
                    case AT_CMD_CMGF:
                    case AT_CMD_CMGS:
                    case AT_CMD_CMGL:
                    case AT_CMD_CMGD:
                    case AT_CMD_CMTI:
                    case AT_CMD_CSCS:
                    case AT_CMD_CSCA:
                    case AT_CMD_CGREG:
                    case AT_CMD_NETAPN:
                    case AT_CMD_XIIC:
                    case AT_CMD_TCPSETUP:
                    case AT_CMD_TCPSEND:
                    case AT_CMD_TCPRECV:
                    case AT_CMD_TCPCLOSE:
                    case AT_CMD_IPSTATUS:
                    case AT_CMD_LIST_SIZE:
                    default:
                        break;
                }

                part_val = GSM_OK;
            }
        }

        /* NOTE After COPS for N15 skip AT_CMD_CREG=2 & AT_CMD_CREG? sequence to POSI. */
        if ( ( 0U == i ) && ( strcmp( gsm_modem_info.model, "N15" ) == 0 ) )
        {
            i = 2; /* Go to POSI. */
        }

        /* Set the last error. */
        if ( GSM_OK != part_val )
        {
            ret_val = part_val;
        }
    }

    return ret_val;
}

GSM_status_t gsm_set_modem_mode( GSM_mode_t mode )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */
    char par[3]; /* Parameter string. */

    /* Setting SMS Input Mode to PDU mode for SMS (default mode). */
    switch ( mode )
    {
        /* Setting SMS Input Mode to ASCII mode for SMS. */
        case GSM_SMS_ASCII_MODE:
            /* gsm_modem_transmit( (const uint8_t*) "AT+CMGF=1\r", 10 ); */
            strncpy( par, "=1", ( sizeof par ) - 1 );
            break;

            /* Setting SMS Input Mode to PDU mode for SMS (default mode). */
        case GSM_SMS_PDU_MODE:
            /* gsm_modem_transmit( (const uint8_t*) "AT+CMGF=0\r", 10 ); */
            strncpy( par, "=0", ( sizeof par ) - 1 );
            break;

        default:
            return GSM_ERROR_NOT_IMPLEMENTED;

    }
    par[ ( sizeof par ) - 1] = '\0';

    /* Sending the command with a parameter. */
    if ( AT_OK != send_at_command( AT_CMD_CMGF, par ) )
    {
        PRINT_ERROR( "Error (%d) while setting message mode to %s", op_res,
                     gsm_get_modem_mode_name( mode ) );

        return GSM_ERROR_AT_SENDING;
    }
    else /* AT_OK */
    {
        /* Getting a response. */
        op_res = receive_at_response( AT_CMD_CSQ, &resp_flags,
                                      MODEM_UART_READ_TIMEOUT );

        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) )
        {
            PRINT_INFO( "Setting message to mode %s successfully",
                        gsm_get_modem_mode_name( mode ) );
        }
        else
        {
            PRINT_ERROR( "Error (%d) while setting message mode to %s", op_res,
                         gsm_get_modem_mode_name( mode ) );

            return GSM_ERROR_AT_RECEIVING;
        }
    }

    return GSM_OK;
}

const char* gsm_get_modem_mode_name( GSM_mode_t mode )
{
    return GSM_mode_name[mode];
}

void gsm_disconnect_modem( void )
{
    /* Setting the status. */
    gsm_modem_info.conn = GSM_STATUS_DISCONNECTING;

    /* Switching off the power by shortcut PWRKEY pin for 3 s. */
    hw_set_port_pin_state( PORTB, PWR_KEY_PB03_Pin, 1 );
    hw_delay( 3000UL );
    hw_set_port_pin_state( PORTB, PWR_KEY_PB03_Pin, 0 );
}

void gsm_power_off_modem( void )
{
    /* Switch off the voltage regulator supplying the GSM modem. */
    hw_set_port_pin_state( PORTB, DC_EN_PB02_Pin, 0 );

    /* Setting the modem power status. */
    gsm_modem_info.power = MODEM_POWER_OFF;
}

GSM_conn_status_t gsm_get_modem_connection_status( void )
{
    return get_merged_connection_status( gsm_modem_info.conn );
}

static GSM_status_t get_merged_connection_status( int value )
{
    /* Get the value from the response. */
    switch ( value )
    {
        case 2: /* Not registered, but terminal is currently searching
         * a new operator to register to. */
            return GSM_STATUS_CONNECTING;
            break;
        case 1: /* Registered, home network. */
        case 5: /* Registered, roaming. */
        case 6: /* Registered for "SMS only", home network (applicable only when <AcT> indicates E-UTRAN). */
        case 7: /* Registered for "SMS only", roaming (applicable only when <AcT> indicates E-UTRAN). */
        case 8: /* Attached for emergency bearer services only. */
        case 9: /* Registered for "CSFB not preferred", home network (applicable only when <AcT> indicates E-UTRAN). */
        case 10: /* Registered for "CSFB not preferred", roaming (applicable only when <AcT> indicates E-UTRAN). */
            return GSM_STATUS_CONNECTED;
            break;
        case 0: /* Not registered, the terminal is not currently searching
         * for a new operator for register to. */
        case 3: /* Registration is denied. */
            return GSM_STATUS_DISCONNECTED;
            break;
        default:
        case 4: /* Unknown (e.g. out of GERAN/UTRAN/E-UTRAN coverage). */
            return GSM_STATUS_UNKNOWN;
            break;
    }
}

const char* gsm_get_modem_connection_status_name( GSM_conn_status_t status )
{
    switch ( status )
    {
        case GSM_STATUS_DISCONNECTED:
            return GSM_conn_status_name[1];
        case GSM_STATUS_CONNECTING:
            return GSM_conn_status_name[2];
        case GSM_STATUS_CONNECTED:
            return GSM_conn_status_name[3];
        case GSM_STATUS_DISCONNECTING:
            return GSM_conn_status_name[4];
        default:
        case GSM_STATUS_UNKNOWN:
            return GSM_conn_status_name[0];
    }
}

/* cppcheck-suppress unusedFunction ; Unused function but part of API. */
uint8_t gsm_get_modem_connection_quality()
{
    return gsm_modem_info.quality;
}

GSM_time_t gsm_get_network_time()
{
    GSM_time_t *ptr = &gsm_modem_info.date_time;

    /* Get the date&time from special
     * at variable with last received value */
    ptr->year = at_parser_date_time[0];
    ptr->month = at_parser_date_time[1];
    ptr->day = at_parser_date_time[2];
    ptr->hour = at_parser_date_time[3];
    ptr->min = at_parser_date_time[4];
    ptr->sec = at_parser_date_time[5];
    ptr->tz = at_parser_date_time[6];

    return gsm_modem_info.date_time;
}

/* cppcheck-suppress unusedFunction ; Unused function but part of API. */
GSM_op_and_loc_t gsm_get_operator_and_localization()
{
    return gsm_modem_info.loc;
}

#ifndef TINYTEST_NOTEST
/* Test cases for gsm_set_smsc() and gsm_get_smsc(). */
static int test_gsm_set_get_smsc( const char *name __attribute__((unused)) )
{
    uint16_t rc;
    const char* rs;
    const char* str1 = 0UL;
    const char* str2 = "";
    const char* str3 = " ";
    const char* str4 = "012345678901234567890123456789"; /* 30c */
    const char* str5 = "0123456789012345678"; /* 19c */
    const char* str6 = "+48602951111"; /* 12c */

    /* Test for true negative. */
    /* Null pointer. */
    rc = gsm_set_smsc( str1 );
    TINYTEST_EQUAL_MSG( GSM_ERROR_AT_RECEIVING, rc, "Bad error code while setting %p (rc=%d)", str1, rc  );
    /* Empty text. */
    rc = gsm_set_smsc( str2 );
    TINYTEST_EQUAL_MSG( GSM_ERROR_AT_RECEIVING, rc, "Bad error code while setting string \"%s\" (rc=%d)", str2, rc  );
    /* Non-address string. */
    rc = gsm_set_smsc( str3 );
    TINYTEST_EQUAL_MSG( GSM_ERROR_AT_RECEIVING, rc, "Bad error code while setting string \"%s\" (rc=%d)", str3, rc );
    /* Too long input string. */
    rc = gsm_set_smsc( str4 );
    TINYTEST_EQUAL_MSG( GSM_ERROR_AT_RECEIVING, rc, "Bad error code while setting string \"%s\" (rc=%d)", str4, rc );

    /* Test for true positive. */
    /* Long input string. */
    rc = gsm_set_smsc( str5 );
    rs = gsm_get_smsc();
    TINYTEST_STR_EQUAL_MSG( str5, rs, "Bad return value while setting string \"%s\" (rc=%d, rs=\"%s\")", str5, rc, rs );
    /* Typical input string. */
    rc = gsm_set_smsc( str6 );
    rs = gsm_get_smsc();
    TINYTEST_STR_EQUAL_MSG( str6, rs, "Bad return value while setting string \"%s\" (rc=%d, rs=\"%s\")", str6, rc, rs );

    return 1;
}
#endif /* TINYTEST_NOTEST */

/* \return SMSC number. */
const char* gsm_get_smsc( void )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */

    /* Send the command with option to handle international format of number. */
    op_res = send_at_command( AT_CMD_CSCA, "?" );

    /* Get a response - confirmation of setting (<\n>+CSCA: <mc><\r>\n>OK<r\><\n><r>) */
    /* Increased time for proper handing of an error. */
    op_res = receive_at_response( AT_CMD_CSCA, &resp_flags, MODEM_UART_READ_TIMEOUT );

    if ( !( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK | AT_CMD_CSCA ) ) ) )
    {
        PRINT_ERROR( "Error (%d:0x%x:%d) while getting response for command %s",
                     op_res, resp_flags, at_cmd_error, get_at_command_name( AT_CMD_CSCA ) );
    }
    else /* AT_OK */
    {
        memset( gsm_smsc, 0U, sizeof gsm_smsc );
        strncpy( gsm_smsc, at_parser_resp_c64, sizeof gsm_smsc - 1 );
        PRINT_DEBUG( "SMSC is to \"%s\"", gsm_smsc );

        return gsm_smsc;
    }

    return 0UL;
}

GSM_status_t gsm_set_smsc( const char* smsc )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */
    PDU_TP_ADDR_TYPE_t type;

    /* XXX Don't remove '+' sign on the front. It doesn't working!  */
    if ( is_prefix_address( smsc ) )
    {
        type = PDU_TP_ADDR_TYPE_INTERNATIONAL;
    }
    else
    {
        type =  PDU_TP_ADDR_TYPE_COMMON;
    }

    /* Send the command with option to handle international format of number. */
    op_res = send_at_command( AT_CMD_CSCA, "=\"%s\",%d", smsc, type );

    /* Get a response - confirmation of setting (<\n>+CSCA: <mc><\r>\n>OK<r\><\n><r>) */
    /* Increased time for proper handing of an error. */
    op_res = receive_at_response( AT_CMD_CSCA, &resp_flags, MODEM_UART_READ_TIMEOUT );

    if ( !( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) ) )
    {
        PRINT_ERROR( "Error (%d:0x%x:%d) while getting response for command %s",
                     op_res, resp_flags, at_cmd_error, get_at_command_name( AT_CMD_CSCA ) );

        return GSM_ERROR_AT_RECEIVING;
    }
    else /* AT_OK */
    {
        PRINT_DEBUG( "SMSC is set to \"%s\" successfully", smsc );

        return GSM_OK;
    }
}


#ifndef TINYTEST_NOTEST

/* Test cases for sms_str_to_pdu_number. */
__attribute__((unused))
static int test_gsm_send_text( const char *name __attribute__((unused)) )
{
    uint8_t rc;

  /* Test for true negative. */
  /* Empty destination phone number. */
  rc = gsm_send_text( GSM_SMS_ASCII_MODE, NULL, NULL, (const uint8_t*) "Test", 4 );
  TINYTEST_EQUAL( GSM_ERROR_INVALID_PARAMETER, rc );
  /* Empty text. */
  rc = gsm_send_text( GSM_SMS_ASCII_MODE, NULL, "+48100101102", NULL, 4 );
  TINYTEST_EQUAL( GSM_ERROR_INVALID_PARAMETER, rc );

  /* Test for valid values. */
  
  /* Add the test cases. */
  if ( GSM_STATUS_CONNECTED == gsm_get_modem_connection_status() )
  {
      /* Set SMS Center address. */
      gsm_set_smsc( "+48602951111" ); /* T-Mobile Centrum SMS */
      send_at_command( AT_CMD_CSCS, "=\"GSM\"" );
      /* Test an ASCII message sending. */
      rc = gsm_send_text( GSM_SMS_ASCII_MODE, NULL, "+48694172804", (const uint8_t*) "Test", 4 );
      TINYTEST_EQUAL_MSG( GSM_OK, rc, "Message doesn't sent (rc=%d,at_err=%d)", rc, at_cmd_error );
      /* Test a PDU message sending. */
      rc = gsm_send_text( GSM_SMS_PDU_MODE, "+48602951111", "+48694172804", (const uint8_t*) "\xD4\xF2\x9C\x1E\x03", 5 );
      TINYTEST_EQUAL_MSG( GSM_OK, rc, "PDU text message with trailing SMSC not sent (rc=%d)", rc );
      rc = gsm_send_text( GSM_SMS_PDU_MODE, NULL, "+48694172804", (const uint8_t*) "\xD4\xF2\x9C\x2E\0x03", 5 );
      TINYTEST_EQUAL_MSG( GSM_OK, rc, "PDU text message without SMSC not sent (rc=%d)", rc );

      return 1;
  }

  return 0;
}
#endif /* TINYTEST_NOTEST */

GSM_status_t gsm_send_text( GSM_mode_t mode,
                            const char *smsc_nr,
                            const char *dst_nr,
                            const uint8_t *text, const uint8_t len )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */
    uint8_t smsc_len = 0U;
    char sms[200];
    uint8_t sms_len = 0U;

    memset( sms, 0U, sizeof sms );

    /* Check input parameters. */
    if ( ( ( (void*) 0UL ) == dst_nr ) || ( ( (void*) 0UL ) == text ) )
    {
        return GSM_ERROR_INVALID_PARAMETER;
    }

    /* Set the mode. */
    gsm_set_modem_mode( mode );

    if ( GSM_SMS_ASCII_MODE == mode )
    {
        /* Send the command. */
        op_res = send_at_command( AT_CMD_CMGS, "=\"%s\"", dst_nr );
    }
    else if ( GSM_SMS_PDU_MODE == mode )
    {
        /*  The address. */
        struct tp_addr nr_struct; /* Address struct. */
        char nr[21]; /* Text phone number. */
        uint8_t pos = 0U; /* Position in output data stream. */
        PDU_TP_ADDR_TYPE_t addr_type; /* Type of address. */
        uint8_t data[100];

        /* Clear the buffer. */
        memset( nr, 0UL, sizeof nr );

        if ( ( (void*) 0UL) != smsc_nr )
        {
            strncpy( nr, smsc_nr, sizeof nr - 1 );

            /* Remove '+'. */
            if ( is_prefix_address(nr) )
            {
                remove_prefix_from_address(nr);
                addr_type = PDU_TP_ADDR_TYPE_INTERNATIONAL;
            }
            else
            {
                addr_type = PDU_TP_ADDR_TYPE_COMMON;
            }

            /* Add SMSC address prior the header. */
            sms_str_to_pdu_address( nr, addr_type, &nr_struct );

            /* Copy the SMSC to SMS data. */
            sms_address_serialize( &nr_struct, data, sizeof data, &pos );

            /* XXX Update the first byte to the length of data SMSC (type + digits). */
            data[0] = pos - 1;

            /* XXX Don't update sms_len by pos.
             * The TPDU size doesn't contain SMSC address size.
             */
            smsc_len = pos;
        }
        else /* No SMSC. Just '\x00'. */
        {
            data[pos++] = '\x00';
            smsc_len = 1U;
        }

        /* Convert the target addres (phone number). */
        strncpy( nr, dst_nr, sizeof nr - 1 );
        if ( is_prefix_address( nr ) )
        {
            remove_prefix_from_address( nr );
            addr_type = PDU_TP_ADDR_TYPE_INTERNATIONAL;
        }
        else
        {
            addr_type = PDU_TP_ADDR_TYPE_COMMON;
        }
        sms_str_to_pdu_address( nr, addr_type, &nr_struct );

        /* Prepare the data with headers. */
        struct sms_submit sms_struct = { .tp_da = &nr_struct,
                                         .tp_udp = text,
                                         .tp_udl = len };

        sms_submit_serialize( &sms_struct, &data[pos], sizeof data - pos, &pos );

        /* Set the size. */
        sms_len = pos;

        str_bin_to_ascii( data, (char *) sms, sms_len + smsc_len );

        /* Send the command. */
        op_res = send_at_command( AT_CMD_CMGS, "=%d", sms_len );
    }
    else
    {
        return GSM_ERROR_INVALID_MODE;
    }

    /* Send a command. */
    if ( AT_OK != op_res )
    {
        PRINT_ERROR( "Error (%d) while sendig command %s", op_res,
                     get_at_command_name( AT_CMD_CMGS ) );

        return GSM_ERROR_AT_SENDING;
    }
    else /* AT_OK */
    {
        /* Wait for a propmpt */
        op_res = receive_text_prompt();

        if ( AT_OK != op_res )
        {
            PRINT_ERROR( "Error (%d) while waiting for prompt in command %s",
                         op_res, get_at_command_name( AT_CMD_CMGS ) );

            return GSM_ERROR_AT_SENDING;
        }
        else
        {
            if ( GSM_SMS_ASCII_MODE == mode )
            {
                /* Copy data (optional). */
                memcpy( sms, text, len );
                sms_len = len;

                /* Send the message content. */
                op_res = send_text( sms, sms_len );
            }
            else if ( GSM_SMS_PDU_MODE == mode )
            {
                /* Send the message content. */
                op_res = send_text( sms, 2 * ( sms_len + smsc_len ) );
            }

            if ( AT_OK != op_res )
            {
                PRINT_ERROR( "Error (%d) while sending data in command %s",
                             op_res, get_at_command_name( AT_CMD_CMGS ) );

                /* Send the message cancel mark. */
                op_res = send_text_cancel();

                if ( AT_OK != op_res )
                {
                    PRINT_ERROR(
                            "Error (%d) while cancellig sending in command %s",
                            op_res, get_at_command_name( AT_CMD_CMGS ) );

                    /* Can't send and cancel sending a message - fatal error. */
                    return GSM_FATAL_ERROR;
                }

                return GSM_ERROR_AT_SENDING;
            }
            else /* OK */
            {
                /* Send the message termination mark. */
                op_res = send_text_end();

                if ( AT_OK != op_res )
                {
                    PRINT_ERROR(
                            "Error (%d) while confirmation the end of data in command %s",
                            op_res, get_at_command_name( AT_CMD_CMGS ) );

                    /* Send the message cancel mark. */
                    op_res = send_text_cancel();

                    if ( AT_OK != op_res )
                    {
                        PRINT_ERROR(
                                "Error (%d) while cancellig sending in command %s",
                                op_res, get_at_command_name( AT_CMD_CMGS ) );

                        /* Can't send and cancel sending a message - fatal error. */
                        return GSM_FATAL_ERROR;
                    }

                    return GSM_ERROR_AT_SENDING;
                }
            }
        }

        /* Get a response - confirmation of sending (<\n>+CMGS: <mc><\r>\n>OK<r\><\n><r>) */
        /* XXX Increased time while waiting for confirmation of sending. */
        /* Check for valid response after max. 15 s (typ. 5 s). */
        op_res = receive_at_response( AT_CMD_CMGS, &resp_flags,
                                      3 * GSM_SMS_SEND_DELAY );
        if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_CMGS ) ) ) )
        {
            PRINT_ERROR( "Error (%d) while getting response for command %s",
                         op_res, get_at_command_name( AT_CMD_CMGS ) );

            return GSM_ERROR_AT_RECEIVING;
        }
        else /* AT_OK */
        {
            PRINT_DEBUG( "SMS sent: to=%s,len=%d,msg=\"%s\",confirm=%ld",
                         dst_nr, len, text, (long ) at_parser_resp_i32 );

            return GSM_OK;
        }
    }
}

GSM_status_t gsm_wait_for_new_message( void )
{
    /* TODO Set the proper time to get next message. */
    const uint32_t time_limit = GSM_SMS_WAITING_TIMEOUT; /* Waiting time for incoming a message. */
    uint32_t start_time; /* The start time of time sensitive operation. */

    /* Clear new message flag. */
    at_parser_msg_new = 0;

    /* Start time of operation. */
    start_time = hw_get_tick_time();

    /* Wait for a new message. */
    do
    {
        hw_delay( 100 );

        /* Check for timeout. */
        if ( ( start_time + time_limit ) <= hw_get_tick_time() )
        {
            break;
        }
    }
    while ( ( 0U == at_parser_msg_new ) ); /* No new message. */

    /* Analyse termination's cause. */
    /* The response analysis has precedence over timeout. */
    if ( at_parser_msg_new )
    {
        return GSM_OK;
    }

    return GSM_ERROR_TIMEOUT;
}

/* cppcheck-suppress unusedFunction ; */
GSM_status_t gsm_receive_text( GSM_mode_t mode __attribute__((unused)),
                               uint8_t *src_nr __attribute__((unused)),
                               uint8_t *text __attribute__((unused)),
                               uint8_t *len __attribute__((unused)),
                               uint8_t max_len __attribute__((unused)) )
{
    return GSM_ERROR_NOT_IMPLEMENTED;
}

GSM_status_t gsm_receive_texts( GSM_mode_t mode )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */

    /* Set an ASCII mode. */
    gsm_set_modem_mode( mode );

    /* Clear global sms table count. */
    sc_sms_list.count = 0;

    /* Clear at table for messages. */
    at_parser_msg_list.size = 0;

    /* Send the receive SMS command. */
    if ( GSM_SMS_ASCII_MODE == mode )
    {
        /* Read all massages. */
        op_res = send_at_command( AT_CMD_CMGL, "=\"ALL\"" );
    }
    else if ( GSM_SMS_PDU_MODE == mode )
    {
        /* Read all messages. */
        op_res = send_at_command( AT_CMD_CMGL, "=4" );
    }
    else
    {
        op_res = AT_ERROR_NOT_SUPPORTED_FORMAT;
    }

    if ( AT_OK != op_res )
    {
        PRINT_ERROR( "Error (%d) while sendig command %s", op_res,
                     get_at_command_name( AT_CMD_CMGL ) );

        return GSM_ERROR_AT_SENDING;
    }
    else /* AT_OK */
    {
        /* Wait for a receive up to even 10 s.*/
        op_res = (uint16_t) receive_at_response( AT_CMD_CMGL, &resp_flags,
                                                 GSM_SMS_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK != op_res ) || ! ( resp_flags & ( AT_CMD_RESP_OK ) ) )
        {
            PRINT_ERROR( "SMS receive error (%d)", op_res );

            return GSM_ERROR_UART_READ;
        }
        else
        {
            /* Check for any message. */
            if ( ! ( resp_flags & ( AT_CMD_RESP_CMGL ) ) || ( 0U
                    == at_parser_msg_list.size ) )
            {
                PRINT_INFO( "No SMS received" );

                return GSM_ERROR_NO_MESSAGES;
            }
            else /* There is any message. */
            {
                /* Determine the range of data.
                 * The first line contains a header (message id, phone number and time).
                 * The second line contains a message content.
                 *
                 * sample valid header: 1,"REC UNREAD","+48XXXXXXXXX",,"2019/10/31,09:36:28+01"
                 * sample valid content: 0A0003020C0101033BF48B04000100007005000C00001B001A
                 */

                /* Pointer to the list of sms. */
                sms_list_t *ptr = &sc_sms_list;

                /* Parse message string list after AT+CGML command and store it at SMS list. */
                for ( int i = 0; i < at_parser_msg_list.size; i++ )
                {
                    /* String with header or message content. */
                    const char *s = at_parser_msg_list.msgs[i];

                    /* Check if previous sms has valid header and hasn't content . */
                    if ( ( 0U < ptr->count ) && ( GSM_SMS_MAX_COUNT
                            >= ptr->count )
                         && ( 0U < ptr->sms[ptr->count - 1].id )
                         && ( 0 < strlen( ptr->sms[ptr->count - 1].number ) )
                         && ( 0 < strlen( ptr->sms[ptr->count - 1].date ) )
                         && ( 0 < strlen( ptr->sms[ptr->count - 1].time ) )
                         && ( 0UL == strlen( ptr->sms[ptr->count - 1].content ) ) )
                    {
                        /* Message longer then GSM_SMS_MAX_LENGTH is treaded as error. */
                        if ( GSM_SMS_MAX_LENGTH < strlen( s ) )
                        {
                            PRINT_ERROR(
                                    "Error (message too long) while parsing string no. %d \"%s\" as content at command %s",
                                    i, s, get_at_command_name( AT_CMD_CMGL ) );

                            /* Delete invalid message. */
                            gsm_delete_text( ptr->sms[ptr->count - 1].id );

                            return GSM_ERROR_MESSAGE_CONTENT_TOO_LONG;
                        }
                        else
                        {
                            /* Copy the string to clean space as the last sms's content. */
                            memset( ptr->sms[ptr->count - 1].content, 0,
                                    sizeof ( ptr->sms[0].content ) );
                            strncpy( ptr->sms[ptr->count - 1].content, s,
                                     sizeof ( ptr->sms[0].content ) - 1 );

                            PRINT_DEBUG(
                                    "Added string no. %d \"%s\" to message no. %d at command %s",
                                    i, ptr->sms[ptr->count - 1].content,
                                    ptr->sms[ptr->count - 1].id,
                                    get_at_command_name( AT_CMD_CMGL ) );
                        }
                    }
                    else /* Parse the string as meassage header. */
                    {
                        if ( !check_message_header_format( s ) )
                        {
                            PRINT_ERROR(
                                    "Error (no parts separator) while parsing string no. %d \"%s\" as header at command %s",
                                    i, s, get_at_command_name( AT_CMD_CMGL ) );

                            return GSM_ERROR_MESSAGE_HEADER_INVALID_FORMAT;
                        }
                        else
                        {
                            /* Overwrite the sms table if overflowed. */
                            if ( GSM_SMS_MAX_COUNT <= ptr->count )
                            {
                                ptr->count = 0;
                            }

                            if ( !parse_message_header(
                                    s, &ptr->sms[ptr->count] ) )
                            {
                                PRINT_ERROR(
                                        "Error (no valid parts) while parsing string no. %d \"%s\" as header at command %s",
                                        i, s,
                                        get_at_command_name( AT_CMD_CMGL ) );

                                return GSM_ERROR_MESSAGE_HEADER_INVALID_PART_FORMAT;
                            }

                            PRINT_DEBUG(
                                    "Added string no. %d \"%s\" as header of message no. %d at command %s",
                                    i, s, ptr->sms[ptr->count].id,
                                    get_at_command_name( AT_CMD_CMGL ) );

                            /* Clear the content. */
                            memset( ptr->sms[ptr->count].content, 0,
                                    sizeof ( ptr->sms[ptr->count].content ) );
                        }

                        ptr->count++;
                    }
                }

#ifdef DEBUG
                /* Print SMS list. */
                for ( uint8_t i = 0;
                        ( i < sc_sms_list.count ) && ( i
                                < ( sizeof sc_sms_list.sms / sizeof sc_sms_list
                                            .sms[0] ) ); i++ )
                {
                    struct sms *sms = &sc_sms_list.sms[i];

                    PRINT_DEBUG(
                            "SMS receive: id=%d,from=\"%s\",date=\"%s\",time=\"%s\",content=\"%s\"",
                            sms->id, sms->number, sms->date, sms->time,
                            sms->content );
                }
#endif /* DEBUG */

                return GSM_OK;
            }
        }
    }
}

/* Delete all messages. */
GSM_status_t gsm_delete_text( uint8_t msg_id )
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */

    /* Delete the message specified in <index>.
     * XXX Only valid for ASCII mode. */
    if ( AT_OK != send_at_command( AT_CMD_CMGD, "=%d,0", msg_id ) )
    {
        PRINT_ERROR( "Error (%d) while sendig command %s", op_res,
                     get_at_command_name( AT_CMD_CMGD ) );

        return GSM_ERROR_AT_SENDING;
    }
    else /* AT_OK */
    {
        /* Get a response.
         * NOTE Incresed timeout to 5 s for response for command CMGD. */
        op_res = receive_at_response( AT_CMD_CMGD, &resp_flags,
                                      100 * MODEM_UART_READ_TIMEOUT );
        if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) ) )
        {
            PRINT_ERROR( "Error (%d) while getting response for command %s",
                         op_res, get_at_command_name( AT_CMD_CMGD ) );

            return GSM_ERROR_AT_RECEIVING;
        }
        else /* AT_OK */
        {
            PRINT_DEBUG( "SMS no. %d deleted", msg_id );

            return GSM_OK;
        }
    }
}

/* Delete all messages. */
GSM_status_t gsm_delete_texts()
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */

    /* Delete all read messages from preferred message storage,
     * leaving unread messages and stored mobile
     * originated messages (whether sent or not) untouched
     * XXX Only valid for ASCII mode. */
    if ( AT_OK != send_at_command( AT_CMD_CMGD, "=1,1" ) )
    {
        PRINT_ERROR( "Error (%d) while sendig command %s", op_res,
                     get_at_command_name( AT_CMD_CMGD ) );

        return GSM_ERROR_AT_SENDING;
    }
    else /* AT_OK */
    {
        /*
         * NOTE Deleting messages consumes more time.
         * Incresed timeout to 5 s for response for the command.
         */
        op_res = receive_at_response( AT_CMD_CMGD, &resp_flags,
                                      100 * MODEM_UART_READ_TIMEOUT );
        if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) ) )
        {
            PRINT_ERROR( "Error (%d) while getting response for command %s",
                         op_res, get_at_command_name( AT_CMD_CMGD ) );

            return GSM_ERROR_AT_RECEIVING;
        }
        else /* AT_OK */
        {
            PRINT_DEBUG( "All read SMS deleted" );

            return GSM_OK;
        }
    }
}

uint32_t gsm_get_sent_bytes_count()
{
    return sent_bytes_count;
}

uint32_t gsm_get_received_bytes_count()
{
    return recv_bytes_count;
}

static bool check_message_header_format( const char *header )
{
    const char *col_pos; /* Colon position. */

    /* At first search for colons (first or any) to pretend to the header. */
    col_pos = strchr( header, ',' ); /* first occurence of ',' - after id */

    /* Check if any occures. */
    if ( ( (void*) 0 ) != col_pos )
    {
        return true;
    }

    return false;
}

/*
 * Sample valid header in ASCII mode:
 * "1,\"REC UNREAD\",\"+48XXXXXXXXX\",,\"2019/10/31,09:36:28+01\""
 */
static bool parse_message_header( const char *header, struct sms *message )
{
    const char *sop = header; /* Begin string position. */
    const char *eop; /* End string position. */
    char p[33]; /* Temporary string for parsed part. */
    uint8_t p_count = 0U; /* Count of parts. */

    do
    {
        uint32_t p_len; /* Temporary part string's length. */

        /* Check if any colon occures. */
        eop = strchr( sop, ',' );

        if ( ( (void*) 0UL ) == eop )
        {
            p_len = ( header + strlen( header ) ) - sop;
        }
        else
        {
            p_len = eop - sop;
        }

        /* Check if part isn't too big. */
        if ( ( ( sizeof p ) - 1 ) < p_len )
        {
            break;
        }

        switch ( p_count )
        {
            case 0U:
                /* part id - no quotation mark, i.e. "1," */
                memset( p, 0, sizeof p );
                strncpy( p, sop, p_len );
                message->id = strtol( p, (void*) 0UL, 10 );
                break;
            case 1U: /* status of message (ignored), i.e.: "REC UNREAD" or "REC READ" */
            case 3U: /* <empty> ignored "," */
                break;
            case 2U:
                /* phone number - with quotation marks, i.e. "\"+48XXXXXXXXX\"," */
                memset( p, 0, sizeof p );
                strncpy( p, sop + 1, p_len - 2 ); /* remove quotation */
                strncpy( message->number, p, ( sizeof message->number ) - 1 );
                break;
            case 4U: /* date part, i.e. "\"2019/10/31,09:36:28+01\"" */
                memset( p, 0, sizeof p );
                strncpy( p, sop + 1, p_len - 1 ); /* remove left quotation */
                strncpy( message->date, p, ( sizeof message->date ) - 1 );
                break;
            case 5U: /* time part, i.e. 09:36:28+01\"" */
                memset( p, 0, sizeof p );
                strncpy( p, sop, p_len - 1 ); /* remove right quotation */
                strncpy( message->time, p, ( sizeof message->time ) - 1 );
                break;
            default:
                break;
        }

        /* Increase count. */
        p_count++;

        /* Increase index. */
        if ( ( (void*) 0 ) != eop )
        {
            sop = eop + 1;
        }

    }
    while ( ( (void*) 0 ) != eop );

    if ( 6U == p_count )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************************
* Test suite.
******************************************************************************/

#ifndef TINYTEST_NOTEST

__attribute__((unused))
static int test_setup_modem( const char* name __attribute__((unused)) )
{
    /* First switch on the modem. */
    if ( MODEM_POWER_ON != gsm_modem_info.power )
    {
        gsm_disconnect_modem();
        gsm_power_off_modem();
        gsm_power_on_modem();
        gsm_connect_modem();
        gsm_wait_for_modem_on( MODEM_ON_TIME_LIMIT_DEFAULT );
    }

    return 1;
}

__attribute__((unused))
static int test_teardown_modem( const char* name __attribute__((unused)) )
{
    /* First switch on the modem. */
    if ( MODEM_POWER_ON == gsm_modem_info.power )
    {
        gsm_power_off_modem();
    }

    return 1;
}
__attribute__((unused))
static int test_setup_connect( const char* name __attribute__((unused)) )
{
    /* First connect to GSM network if not connected. */
    if ( GSM_STATUS_DISCONNECTED == gsm_get_modem_connection_status() )
    {
        gsm_power_on_modem();
        gsm_connect_modem();
        gsm_wait_for_modem_on( MODEM_ON_TIME_LIMIT_DEFAULT );
        gsm_wait_for_connect( MODEM_CONNECT_TIME_LIMIT_DEFAULT );
    }

    return 1;
}

__attribute__((unused))
static int test_teardown_connect( const char* name __attribute__((unused)) )
{
    /* First connect to GSM network if not connected. */
    if ( GSM_STATUS_CONNECTED == gsm_get_modem_connection_status() )
    {
        gsm_disconnect_modem();
        gsm_power_off_modem();
        return 1;
    }

    return 0;
}


/* Test suite for the unit. */
TINYTEST_START_SUITE( gsm_comm );
  TINYTEST_ADD_TEST( test_gsm_send_text, NULL, NULL );
  TINYTEST_ADD_TEST( test_gsm_set_get_smsc, NULL, NULL );
TINYTEST_END_SUITE();

#endif /* TINYTEST_NOTEST */


/*** end of file ***/

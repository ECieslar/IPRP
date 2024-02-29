/*!****************************************************************************
*
* \file gprs.c
* \brief Implementation of GPRS modem's functions.
*
* \copyright ELEKTROMETAL SA (c) 2020, Wszelkie prawa zastrzeżone
* \version $Revision$
* \date $Date$
* \author $Author$
*
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include <string.h> /* Declaration of memset(), memcpy(). */
#include "at_cmd.h"
#include "gsm_comm.h"
#include "gprs_comm.h" /* Declaration of the module. */
#include "../hw/mkg_hw.h"
#include "../util/debug.h" /* PRINT_DEBUG() */

/******************************************************************************
* Defines and Enumerations
******************************************************************************/

/******************************************************************************
* Local Types and Typedefs
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/
/*! \brief APN name */
char gprs_apn[32+1];

/*! \brief APN username */
char gprs_username[32+1];

/*! \brief APN user's password */
char gprs_password[32+1];

/*! \brief GPRS connection state. */
bool gprs_conn = false;

/*! \brief Assigned IP address in 4 octects. */
uint32_t gprs_ip = 0UL;

/*! \brief Assigned IP address in string form. */
char gprs_ip_str[15+1] = "";


/******************************************************************************
* Static Function Prototypes
******************************************************************************/

/******************************************************************************
* Static Variables
******************************************************************************/

/******************************************************************************
* Functions
******************************************************************************/

#ifndef TINYTEST_NOTEST
/* Test cases for gprs_set_apn() and gprs_get_apn(). */
static int test_gprs_set_get_apn( const char *name __attribute__((unused)) )
{
    bool rc; /* Returned code from gprs_set_apn(). */
    const char* rs; /* Returned string from gprs_get_apn(). */
    const char* str1 = 0UL;
    const char* str2 = "";
    const char* str3 = " ";
    const char* str4 = "012345678901234567890123456789012"; /* 33c */
    const char* str5 = "01234567890123456789012345678901"; /* 32c */

    /* Test for true negative. */
    /* Null pointer. */
    rc = gprs_set_apn( str1, str1, str1 );
    TINYTEST_FALSE_MSG( rc, "Bad return code for null first input string (rc=%d)", rc );
    rc = gprs_set_apn( str2, str1, str1 );
    TINYTEST_FALSE_MSG( rc, "Bad return code for null second input string  (rc=%d)", rc  );
    rc = gprs_set_apn( str2, str2, str1 );
    TINYTEST_FALSE_MSG( rc, "Bad return code for null third input string  (rc=%d)", rc  );

    /* Test for true positive. */
    /* Empty text. */
    rc = gprs_set_apn( str2, str2, str2 );
    TINYTEST_TRUE_MSG( rc, "Bad return code for empty strings (rc=%d)", rc );
    rs = gprs_get_apn();
    TINYTEST_STR_EQUAL_MSG( str2, rs, "Expected empty string (rs=%s)", rs  );
    /* Short input string. */
    rc = gprs_set_apn( str3, str2, str2 );
    TINYTEST_TRUE_MSG( rc, "Expected postitive returned code (rc=%d)", rc );
    rs = gprs_get_apn();
    TINYTEST_STR_EQUAL_MSG( str3, rs, "Expected other value (rs=%s)", rs );
    /* Too long input string. */
    rc = gprs_set_apn( str4, str2, str2 );
    TINYTEST_FALSE_MSG( rc, "Invalid returned value (rc=%d)", rc );
    /* Valid length. */
    rc = gprs_set_apn( str5, str2, str2 ); /* 32c */
    TINYTEST_TRUE_MSG( rc, "Invalid returned value (rc=%d)", rc );
    rs = gprs_get_apn();
    TINYTEST_STR_EQUAL_MSG( str5, rs, "Expected other value (rs=%s)", rs );

    return 1;
}
#endif /* TINYTEST_NOTEST */


bool gprs_set_apn( const char* apn, const char* username, const char* password )
{
    bool rc = false;

    if ( ( ( (void*) 0UL ) == apn )
            || ( ( (void*) 0UL ) == username )
            || ( ( (void*) 0UL ) == password ) )
    {
        return false;
    }

    if ( ( 32UL < strlen( apn ) )
            || ( 16UL < strlen( username ) )
            || ( 16UL < strlen( password ) ) )
    {
        return false;
    }

    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */

    /* Send the command with parameters. */
    op_res = send_at_command( AT_CMD_NETAPN, "=\"%s\",\"%s\",\"%s\"", apn, username, password );

    /* Get a response - confirmation of setting (<\r><\n>OK<\r><\n>). */
    op_res = receive_at_response( AT_CMD_NETAPN, &resp_flags, MODEM_UART_READ_TIMEOUT );
    if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) ) )
    {
        PRINT_ERROR( "Error (%d) while getting response for command %s",
                     op_res, get_at_command_name( AT_CMD_NETAPN ) );

        rc = false;
    }
    else /* AT_OK */
    {
        /* Set current APN. */
        memcpy( gprs_apn, apn, sizeof gprs_apn - 1 );
        memcpy( gprs_username, username, sizeof gprs_username - 1 );
        memcpy( gprs_password, password, sizeof gprs_password - 1 );

        PRINT_DEBUG( "APN set to: apn=\"%s\",username=\"%s\",password=\"%s\"",
                     gprs_apn, gprs_username, gprs_password );

        rc = true;
    }

    return rc;
}

/*
 * \brief Gets an APN name with creditials.
 * \return APN name, user and password in one comma-separated string
 * or null if APN no APN is set.
 */
const char* gprs_get_apn( void )
{
    char* rs;;

    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response. */

    /* Send the query command. */
    op_res = send_at_command( AT_CMD_NETAPN, "?" );

    /* Get a response - confirmation of setting.
     * (+NETAPN: "<apn>","<username","<password>"<\r><\n>OK<\r><\n>) */
    op_res = receive_at_response( AT_CMD_NETAPN, &resp_flags, MODEM_UART_READ_TIMEOUT );
    if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_NETAPN | AT_CMD_RESP_OK ) ) ) )
    {
        PRINT_ERROR( "Error (%d) while getting response for command %s",
                     op_res, get_at_command_name( AT_CMD_NETAPN ) );

        rs = 0UL;
    }
    else /* AT_OK */
    {
        /* Divide the string for three parts. */
        const uint8_t len = strlen( at_parser_resp_c64 );
        uint8_t ci = 0U; /* Colon index. */
        uint8_t cn = 0U; /* Colon number. */

        memset ( gprs_apn, 0UL, sizeof gprs_apn );
        memset ( gprs_username, 0UL, sizeof gprs_username );
        memset ( gprs_password, 0UL, sizeof gprs_password );

        for ( uint8_t i = 0U; i < len + 1; i++ )
        {
            if ( ( ',' == at_parser_resp_c64[i] )
                    || ( '\0' == at_parser_resp_c64[i] ) )
            {
                char* pp = 0UL; /* Param pointer. */
                uint8_t ps = 0U; /* Param start index. */
                uint8_t pl = 0U; /* Param length. */

                switch ( cn++ )
                {
                    case 0:
                        pp = gprs_apn;
                        ps = ci + 1;
                        pl = i - ps - 1;
                        break;
                    case 1:
                        pp = gprs_username;
                        ps = ci + 2;
                        pl = i - ps - 1;
                        break;
                    case 2:
                        pp = gprs_password;
                        ps = ci + 2;
                        pl = i - ps - 1;
                        break;
                    default:
                        break;
                }

                /* Copy string without `\"`. */
                if ( pp )
                {
                    if ( pl )
                    {
                        strncpy( pp, &at_parser_resp_c64[ps], pl );
                    }
                    else
                    {
                        *pp = 0U;
                    }
                }

                /* Save parameters . */
                ci = i;
            }
        };

        /* Print the current APN. */
        PRINT_INFO( "APN: apn=\"%s\",username=\"%s\",password=\"%s\"",
                     gprs_apn, gprs_username, gprs_password );

        rs = gprs_apn;
    }

    return rs;
}

#ifndef TINYTEST_NOTEST
/* Test cases for gprs_enable(). */
static int test_change_gprs_state( const char *name __attribute__((unused)) )
{
    uint8_t rc;
    const char* rs;

    /* Test for true postive. */
    rc = gprs_get_state();
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while getting GPRS (rc=%d)", rc );

    /* Set test APN. */
    gprs_set_apn("internet", "", ""); /* T-Mobile APN. */
    rc = gprs_enable( true );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while turn on GPRS transmition (rc=%d)", rc );
    hw_delay(1000UL);
    rs = gprs_get_ip();
    TINYTEST_STR_NOT_EQUAL_MSG( "0.0.0.0", rs, "Improper IP value (rs=%s)", rs );
    rc = gprs_enable( false );
    TINYTEST_EQUAL_MSG( true, rc, "Bad return value while turn off GPRS transmition (rc=%d)", rc );
    hw_delay(1000UL);
    rs = gprs_get_ip();
    TINYTEST_STR_EQUAL_MSG( "0.0.0.0", rs, "Improper IP value (rs=%s)", rs );

    return 1;
}
#endif /* TINYTEST_NOTEST */

/*
 * \brief
 * \param onoff
 * \return
 */
bool gprs_enable( bool onoff )
{
    bool rc = false;

    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation status. */
    AT_cmd_response_t resp_flags; /* Modem's response */


     /* Send the command. */
     op_res = send_at_command( AT_CMD_XIIC, "=%d", onoff );

     /* Get a response - confirmation of setting (XIIC=<onoff>"<\r><\n>OK<\r><\n>) */
     op_res = receive_at_response( AT_CMD_XIIC, &resp_flags, MODEM_UART_READ_TIMEOUT );
     if ( ! ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_OK ) ) ) )
     {
         PRINT_ERROR( "Error (%d) while getting response for command %s",
                      op_res, get_at_command_name( AT_CMD_XIIC ) );

         rc = false;
     }
     else /* AT_OK */
     {
         /* Set current GPRS connection state. */
         gprs_conn = onoff;

         PRINT_DEBUG( "GPRS connection=%d", onoff );

         rc = true;
     }

    return rc;
}

/*
 * \brief
 * \return
 */
bool gprs_get_state( void )
{
    uint8_t rc = 0U;
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */

    /* Send the "CGREG?" command. */
    if ( AT_OK == send_at_command( AT_CMD_CGREG, 0UL ) )
    {
        /* Get results. */
        op_res = receive_at_response( AT_CMD_CGREG, &resp_flags, MODEM_UART_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_CGREG | AT_CMD_RESP_OK ) ) )
        {
            /* Get the value from the response. */
            gprs_conn = at_parser_resp_cmplx_i16.val1;
        }
    }

     /* Analyse termination's cause. */
    /* The response analysis has precedence over timeout. */
    if ( ( AT_OK == op_res ) && ( ( AT_CMD_RESP_CGREG | AT_CMD_RESP_OK )
            == ( resp_flags & ( AT_CMD_RESP_CGREG | AT_CMD_RESP_OK ) ) )
         && ( GSM_STATUS_CONNECTED == gprs_conn ) )
    {
        PRINT_DEBUG(
                "Modem is connected to GPRS service (cgreg=%s)",
                gsm_get_modem_connection_status_name( gprs_conn ) );

        rc = true;
    }
    else
    {
        rc = false;
    }

    return rc;
}

/*
 * \brief Get an IP assigned in the PS network.
 * \param null terminated string with IP address in format ddd.ddd.ddd.ddd.
 */
const char* gprs_get_ip()
{
    AT_status_t op_res = AT_ERROR_UNKNOWN; /* Operation op_res. */
    AT_cmd_response_t resp_flags; /* Response flags */

    /* Send the "XIIC?" command. */
    if ( AT_OK == send_at_command( AT_CMD_XIIC, "?" ) )
    {
        /* Get results. */
        op_res = receive_at_response( AT_CMD_XIIC, &resp_flags, MODEM_UART_READ_TIMEOUT );

        /* Check for valid response. */
        if ( ( AT_OK == op_res ) && ( resp_flags & ( AT_CMD_RESP_XIIC | AT_CMD_RESP_OK ) ) )
        {
            /* Clear ip string. */
            memset( gprs_ip_str, 0U, sizeof gprs_ip_str );

            /* Get the value from the response. */
            strncpy( gprs_ip_str, at_parser_resp_c64, sizeof gprs_ip_str - 1 );

            if ( gprs_conn )
            {

                PRINT_DEBUG( "Modem is connected and has assigned IP address (conn=%s,IP=%s)",
                        gsm_get_modem_connection_status_name( gprs_conn ), gprs_ip_str );
            }
            else
            {
                PRINT_DEBUG( "Modem is disconnected" );
            }
        }
    }

    return gprs_ip_str;
}

uint32_t gprs_ip_str_to_ipv4( const char* ip_str )
{
    uint32_t ip_r = 0UL; /* Returned value. */
    uint8_t size = 0U; /* Input string size. */
    uint8_t dot_pos = 0U; /* Dot position. */
    int8_t num_part = -1; /* Numeric part. */
    char ip_str_dup[15+1]; /* IP address duplicate for conversion. */

    /* Make a duplicate of input string. */
    strncpy( ip_str_dup, ip_str, sizeof ip_str_dup - 1 );

    size = strlen( ip_str_dup );

    for ( uint8_t i = 0; i <= size; i++ )
    {
        /* Find '.' or the end of string. */
        if ( ( '.' == ip_str[i] )
              || ( '\0' == ip_str[i] ) )
        {
            /* Put termination into the dot place. */
            ip_str_dup[i] = '\0';

            switch ( ++num_part )
            {
                /* XXX atoi() is obsolate but clearer. */
                case 0:
                    ip_r = (uint8_t) ( atoi( &ip_str_dup[0] ) << 24 );
                    break;
                case 1:
                    ip_r |= (uint8_t) ( atoi( &ip_str_dup[dot_pos + 1] ) << 16 );
                    break;
                case 2:
                    ip_r |= (uint8_t) ( atoi( &ip_str_dup[dot_pos + 1] ) << 8 );
                    break;
                case 3:
                    ip_r |= (uint8_t) ( atoi( &ip_str_dup[dot_pos + 1] ) );
                    break;
                default:
                    break;
            }

            dot_pos = i;
        }
    }

    return ip_r;
}

void gprs_ipv4_to_ip_str( const uint32_t ip32, char* ip_str, const uint8_t ip_str_size )
{
    snprintf( ip_str, ip_str_size, "%u.%u.%u.%u",
                                  (uint8_t) ( ip32 >> 24 ),
                                  (uint8_t) ( ip32 >> 16 ),
                                  (uint8_t) ( ip32 >> 8 ),
                                  (uint8_t) ( ip32 ) );
    ip_str[ip_str_size - 1] = '\0';
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
        gsm_disconnect_modem();
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
    if ( GSM_STATUS_CONNECTED == gsm_get_modem_connection_status() )
    {
        /* Disconnect from GSM. */
        gsm_disconnect_modem();
        gsm_power_off_modem();

        return 1;
    }

    return 0;
}

/* Test suite for the unit. */
TINYTEST_START_SUITE( gprs_comm );
  TINYTEST_ADD_TEST( test_change_gprs_state, NULL, NULL );
  TINYTEST_ADD_TEST( test_gprs_set_get_apn, NULL, NULL );
TINYTEST_END_SUITE();

#endif /* TINYTEST_NOTEST */

/*** end of file ***/

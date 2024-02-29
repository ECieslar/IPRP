/*!****************************************************************************
 *
 * \file mkg_test.c
 * \brief Test for MKG module.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293340 $
 * \date $Date: 2020-09-07 08:02:05 +0200 (pon.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../hw/mkg_hw.h"
#include "../gm/gm_comm.h"
#include "../gsm/at_cmd.h"
#include "../gsm/gsm_comm.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

#define VERSION ((__DATE__ " " __TIME__))

#define __assert_true(con, a, b, ...) \
		assert_true( __FILE__, __LINE__, con, a, b, ##__VA_ARGS__) /*!< Macro for test assertion. */

#define debug_print printf

#define wait_for_milisec hw_delay /*!< Macro for sleep function. */

/*!
 * \brief MKG module's test GM general information.
 */
enum MKG_GSM_Test_General
{
    MKG_TEST_GSM_SW_ID_GROUP = 65, /*!< Application group id. */
    MKG_TEST_GSM_SW_ID_ITEM =    0, /*!< Application item id. */
    MKG_TEST_GSM_SW_ID_VER_MAJ = 0, /*!< Application version major. */
    MKG_TEST_GSM_SW_ID_VER_MIN = 9, /*!< Application version minor. */
    MKG_TEST_GSM_SW_ID_VER_REL = 1, /*!< Application version release. */
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
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void assert_true( const char* file, int line, uint8_t cond,
        const char* msg_true, const char* msg_false, ... );

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/

int main( void )
{
    const char* test_nr = "694172804";
    enum GSM_conn_status conn_status;
    GSM_status_t ret_val;

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    hw_init();

#ifdef DEBUG
    /* Print identification info. */
    debug_print( "MKG-01\nHW: %d_%04d_D_%d_%02d_%02d\nSW: %d_%04d_v%d_%d_%d\n",
                 MKG_HW_ID_GROUP, MKG_HW_ID_ITEM, MKG_HW_ID_REV_YEAR,
                 MKG_HW_ID_REV_MONTH, MKG_HW_ID_REV_DAY,
                 MKG_TEST_GSM_SW_ID_GROUP, MKG_TEST_GSM_SW_ID_ITEM,
                 MKG_TEST_GSM_SW_ID_VER_MAJ, MKG_TEST_GSM_SW_ID_VER_MIN,
                 MKG_TEST_GSM_SW_ID_VER_REL );
#endif /* DEBUG */

    debug_print( "\n=== GSM MODEM CONNECTION TEST ===\n\n" );

    /* Włączenie modemu GSM. */
    gsm_power_on_modem();
    debug_print( "[OK] GSM modem power on\n" );

    hw_flush_uart( GSM_MODEM_UART );

    /* Łączenie z siecią GSM. */
    gsm_connect_modem();
    debug_print( "[OK] GSM connect\n" );

    /* Oczekiwanie na włączenie 2,5 s. */
    wait_for_milisec( 2500 );

    /* Konfiguracja trybu połączenia. */
    __assert_true( ( gsm_set_modem_mode( GSM_SMS_ASCII_MODE ) == GSM_OK ),
            "[OK] GSM modem mode set to %s\n",
            "[ERROR] Can't configure GSM modem mode!\n",
            gsm_get_modem_mode_name( GSM_SMS_ASCII_MODE ) );

    /* Sprawdzenie łączenia. */
    conn_status = gsm_get_modem_connection_status();
    __assert_true(
            ( conn_status == GSM_STATUS_CONNECTING )
                    || ( conn_status == GSM_STATUS_CONNECTED ),
            "[OK] Connecting ...\n", "[ERROR] Invalid connecting status (%s)\n",
            gsm_get_modem_connection_status_name( conn_status ) );

    /* Oczekiwanie przez 15 s na nawiązanie połączenia. */
    wait_for_milisec( 15000 );

    /* Sprawdzenie połączenia. */
    conn_status = gsm_get_modem_connection_status();
    __assert_true( ( conn_status == GSM_STATUS_CONNECTED ), "[OK] Connected\n",
            "[ERROR] Connection failed (%s)\n",
            gsm_get_modem_connection_status_name( conn_status ) );

    /* Rozłączanie. */
    gsm_power_off_modem();
    debug_print( "[OK] GSM modem power off\n" );

    /* Sprawdzenie rozłaczenia. */
    conn_status = gsm_get_modem_connection_status();
    __assert_true(
            ( conn_status == GSM_STATUS_DISCONNECTING )
                    || ( conn_status == GSM_STATUS_DISCONNECTED ),
            "[OK] Disconnecting ...\n",
            "[ERROR] Invalid connection status (%s)\n",
            gsm_get_modem_connection_status_name( conn_status ) );

    wait_for_milisec( 3600 );

    /* Sprawdzenie rozłaczenia. */
    conn_status = gsm_get_modem_connection_status();
    __assert_true(
            ( conn_status == GSM_STATUS_DISCONNECTED )
                    || ( conn_status == GSM_STATUS_UNKNOWN ),
            "[OK] Disconnected\n", "[ERROR] Disconnection error (%s)\n",
            gsm_get_modem_connection_status_name( conn_status ) );

    debug_print( "\n=== GSM MODEM SMS TEST ==========\n\n" );

    wait_for_milisec( 5000 );

    /* Sprawdzenie funkcji pomocniczych. */
    gsm_power_on_modem();
    gsm_connect_modem();
    wait_for_milisec( 2500 );

    gsm_set_modem_mode( GSM_SMS_ASCII_MODE );

    wait_for_milisec( 15000 );

    /* Wysłanie SMS-a. */
    char sms_data[161];
    sprintf( sms_data, "%s", VERSION );
    ret_val = gsm_send_text( GSM_SMS_ASCII_MODE, (void*) 0UL, test_nr, (uint8_t*) sms_data,
            strlen( sms_data ) );
    __assert_true( ret_val == GSM_OK, "[OK] SMS '%s' sent to number '%s'\n",
            "[ERROR] Can't send SMS!\n", sms_data, (const char * ) test_nr );

    at_commands_test();
}

/*! \brief Assert function.  */
static void assert_true( const char* file, int line, uint8_t cond,
        const char* msg_true, const char* msg_false, ... )
{
    char msg[1024];
    size_t msg_len;
    va_list arg;

    if ( cond )
    {
        /* cppcheck-suppress va_start_wrongParameter ; Indendet use for handling true assert message. */
        va_start( arg, msg_true );
        vsprintf( (char*) msg, msg_true, arg );
    }
    else
    {
        /* cppcheck-suppress va_start_subsequentCalls ; Indendet use for handling false assert message. */
        va_start( arg, msg_false );
        vsprintf( (char*) msg, msg_false, arg );
    }

    va_end( arg );

    msg_len = strlen( (const char*) msg );

    hw_transmit_by_uart( DEBUG_UART, (uint8_t*) msg, msg_len );

    if ( !cond )
    {
        __asm volatile ("bkpt #5"); /* Assert Failed. */
    }

    return;
}

/*** end of file ***/

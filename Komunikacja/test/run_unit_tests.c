/*!****************************************************************************
*
* \file run_unit_test.c
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
#include "../hw/mkg_hw.h"       /* Declaration of hw_init(). */
#include "../gsm/at_cmd.h"      /* at_cmd unit */
#include "../gsm/sms_pdu.h"     /* sms_pdu unit */
#include "../gsm/gsm_comm.h"    /* gsm_comm unit */
#include "../gsm/gprs_comm.h"   /* gprs_comm unit */
#include "../gsm/tcp_comm.h"    /* tcp_comm unit */
#include "tinytest.h"

/******************************************************************************
* Defines and Enumerations
******************************************************************************/

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

/* Main function definition. */
TINYTEST_START_MAIN();

/* Initialize peripherals. */
  hw_init();

  gsm_disconnect_modem();
  test_setup_connect( "" );
  hw_delay( 5000U );

  TINYTEST_REG_NAME( "MKG" );

  /* Units to run tests in reverse order. */
  TINYTEST_RUN_SUITE( tcp_comm );
  TINYTEST_RUN_SUITE( gprs_comm );
  TINYTEST_RUN_SUITE( gsm_comm );
  TINYTEST_RUN_SUITE( sms_pdu );
  TINYTEST_RUN_SUITE( at_cmd );

/* Tests summary. */
TINYTEST_END_MAIN();

/*** end of file ***/

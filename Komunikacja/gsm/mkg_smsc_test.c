/*!****************************************************************************
*
* \file mkg_smsc_test.c
* \brief 
*
* \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
* \version $Revision: 256712 $
* \date $Date: 2019-11-19 15:06:28 +0100 (wt.) $
* \author $Author: kszczepanski $
*
*
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include <stdio.h>
#include "../hw/mkg_hw.h"
#include "../gsm/gsm_comm.h"


/******************************************************************************
* Defines and Enumerations
******************************************************************************/

/*!
 * \brief MKG module's test GM general information.
 */
enum MKG_GSM_Test_General
{
    MKG_TEST_SMSC_SW_ID_GROUP = 65, /*!< Application group id. */
    MKG_TEST_SMSC_SW_ID_ITEM =    0, /*!< Application item id. */
    MKG_TEST_SMSC_SW_ID_VER_MAJ = 0, /*!< Application version major. */
    MKG_TEST_SMSC_SW_ID_VER_MIN = 9, /*!< Application version minor. */
    MKG_TEST_SMSC_SW_ID_VER_REL = 1, /*!< Application version release. */
};

#define debug_print printf /* Replace printf function. */

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


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

int main( void )
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    hw_init();

#ifdef DEBUG
    /* Print identification info. */
    debug_print( "MKG-01\nHW: %d_%04d_D_%d_%02d_%02d\nSW: %d_%04d_v%d_%d_%d\n",
                 MKG_HW_ID_GROUP, MKG_HW_ID_ITEM, MKG_HW_ID_REV_YEAR,
                 MKG_HW_ID_REV_MONTH, MKG_HW_ID_REV_DAY,
                 MKG_TEST_SMSC_SW_ID_GROUP, MKG_TEST_SMSC_SW_ID_ITEM,
                 MKG_TEST_SMSC_SW_ID_VER_MAJ, MKG_TEST_SMSC_SW_ID_VER_MIN,
                 MKG_TEST_SMSC_SW_ID_VER_REL );
#endif /* DEBUG */

#ifdef DEBUG
    /* While debugging disconnect from the GSM Network
     * to allow perform full power on/off, connect/disconnect cycle.
     */
    gsm_disconnect_modem();
#endif /* DEBUG */

    /* Switch on the power for the modem. */
    gsm_power_on_modem();

    /* Switch on the modem
     * (it connects automatically to the GSM Network). */
    gsm_connect_modem();

    while (1)
    {};
}

/*** end of file ***/

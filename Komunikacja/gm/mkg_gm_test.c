/*!****************************************************************************
 *
 * \file mkg_gm_test.c
 * \brief 
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 291414 $
 * \date $Date: 2020-08-24 10:31:30 +0200 (pon.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <string.h> /* memcpy() */
#include "../hw/mkg_hw.h"
#include "../gm/gm_comm.h"
#include "../util/str_util.h"
#include "../util/debug.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*!
 * \brief MKG module's test GM general information.
 */
enum MKG_GM_Test_General
{
    MKG_TEST_GM_SW_ID_GROUP = 65, /*!< Application group id. */
    MKG_TEST_GM_SW_ID_ITEM = 0, /*!< Application item id. */
    MKG_TEST_GM_SW_ID_VER_MAJ = 0, /*!< Application version major. */
    MKG_TEST_GM_SW_ID_VER_MIN = 10, /*!< Application version minor. */
    MKG_TEST_GM_SW_ID_VER_REL = 0, /*!< Application version release. */
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

/* Set random data for given object id. */
static void set_random_data( SG_OBJ_ID_t id, uint8_t* data, const uint8_t max_data_len, uint8_t* data_len );

/******************************************************************************
 * Static Variables
 ******************************************************************************/

static const uint16_t obj_tbl[] =
    {
     /* 6.1 Informacyjne */
     SG_OBJ_ID_HARDWARE_VERSION,
     SG_OBJ_ID_FIRMWARE_VERSION,
     SG_OBJ_ID_STANDARD_PARAMETER_LIST_VERSION,
     SG_OBJ_ID_SERIAL_NBR,
     SG_OBJ_ID_LATITUDE,
     SG_OBJ_ID_LONGITUDE,
     SG_OBJ_ID_CLIENT_ID,
     SG_OBJ_ID_GAS_METER_SERIAL_NBR,
     SG_OBJ_ID_DEVICE_TYPE_NAME,

     /* 6.2 Liczydła i zużycia */
     SG_OBJ_ID_UNITS,
     SG_OBJ_ID_COUNTERS_REGISTRATION_PERIOD,
     SG_OBJ_ID_CALORIFIC_VALUE,
     SG_OBJ_ID_VOLUME,
     SG_OBJ_ID_COMMAND_VOLUME_OFFSET,
     SG_OBJ_ID_ENERGY,
     SG_OBJ_ID_COMMAND_ENERGY_OFFSET,
     SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_TIME,
     SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_VOLUME,
     SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_ENERGY,
     SG_OBJ_ID_VOLUME_MAX_VALUE,
     SG_OBJ_ID_PERIOD_ENERGY_USAGE_COMPRESSED,
     SG_OBJ_ID_MONTH_VOLUME_USAGE_COMPRESSED,
     SG_OBJ_ID_MONTH_ENERGY_USAGE_COMPRESSED,
     SG_OBJ_ID_COUNTERS_ARCHIVE,
     SG_OBJ_ID_PERIOD_VOLUME_USAGE_COMPRESSED,

     /* 6.3 Przepływ */
     SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED,
     SG_OBJ_ID_GAS_METER_Q_MAX,
     SG_OBJ_ID_TEMPORARY_FLOW,
     SG_OBJ_ID_HOURLY_USAGE_CURRENT,
     SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED,
     SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED,
     SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED,
     SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED,
     SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED,
     SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED,
     SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED,

     /* 6.4 Zegar */
     SG_OBJ_ID_TIME_ZONE_OFFSET,
     SG_OBJ_ID_TIME_ZONE_OFFSET_WINTER,
     SG_OBJ_ID_TIME_ZONE_OFFSET_SUMMER,
     SG_OBJ_ID_CLOCK_SYNCHRONISATION_FROM_GSM,
     SG_OBJ_ID_TIME_ZONE_AUTO_OFFSET,
     SG_OBJ_ID_CLOCK_SYNCHRONISATION_OFFSET,
     SG_OBJ_ID_CLOCK,

     /* 6.5 Ingerencja */
     SG_OBJ_ID_TAMPER_REMOVE_COUNT,
     SG_OBJ_ID_TAMPER_OPEN_COUNT,
     SG_OBJ_ID_TAMPER_MAGNETIC_COUNT,
     SG_OBJ_ID_TAMPER_REMOVE_TIME,
     SG_OBJ_ID_TAMPER_REMOVE_DURATION,
     SG_OBJ_ID_TAMPER_REMOVE_VOLUME,
     SG_OBJ_ID_TAMPER_REMOVE_ENERGY,
     SG_OBJ_ID_TAMPER_OPEN_TIME,
     SG_OBJ_ID_TAMPER_OPEN_DURATION,
     SG_OBJ_ID_TAMPER_OPEN_VOLUME,
     SG_OBJ_ID_TAMPER_OPEN_ENERGY,
     SG_OBJ_ID_TAMPER_MAGNETIC_TIME,
     SG_OBJ_ID_TAMPER_MAGNETIC_DURATION,
     SG_OBJ_ID_TAMPER_MAGNETIC_VOLUME,
     SG_OBJ_ID_TAMPER_MAGNETIC_ENERGY,

     /* 6.6 Diagnostyki */
     SG_OBJ_ID_BATTERY_LEVEL,
     SG_OBJ_ID_TEMPERATURE,

     /* 6.7 Modem */
     SG_OBJ_ID_SIM_SMS_VALIDITY,
     SG_OBJ_ID_GSM_QUALITY,
     SG_OBJ_ID_MODEM_STATE,
     SG_OBJ_ID_MODEM_SMS_RECEIVED,
     SG_OBJ_ID_MODEM_SMS_SENT,
     SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED,
     SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT,
     SG_OBJ_ID_MODEM_WAKEUP_COUNT,
     SG_OBJ_ID_SIM_IP,
     SG_OBJ_ID_GSM_BTS_LAC,
     SG_OBJ_ID_GSM_BTS_CELL_ID,
     SG_OBJ_ID_MODEM_WAKEUP_DURATION,
     SG_OBJ_ID_SIM_PIN,
     SG_OBJ_ID_SIM_ICCID,
     SG_OBJ_ID_SIM_PHONE,
     SG_OBJ_ID_SIM_SMSC,
     SG_OBJ_ID_GPRS_APN,
     SG_OBJ_ID_GPRS_USER,
     SG_OBJ_ID_GPRS_PASSWORD,
     SG_OBJ_ID_GSM_NETWORK_CODE,

     /* 6.8 Konfiguracja zdarzeń */
     SG_OBJ_ID_EVENT_LINK,
     SG_OBJ_ID_EVENT_UDP_PORT,
     SG_OBJ_ID_EVENT_TCP_PORT,
     SG_OBJ_ID_EVENT_IP,
     SG_OBJ_ID_EVENT_PHONE,

     /* 6.9 Zawór */
     SG_OBJ_ID_VALVE_OPERATION,
     SG_OBJ_ID_VALVE_STATE,
     SG_OBJ_ID_VALVE_ERROR_CODE,
     SG_OBJ_ID_VALVE_ACTIVATION_TIMEOUT,
     SG_OBJ_ID_VALVE_LEAK_TEST_DURATION,
     SG_OBJ_ID_VALVE_LEAK_TEST_VOLUME,
     SG_OBJ_ID_VALVE_OPEN_COUNT,
     SG_OBJ_ID_VALVE_CLOSE_COUNT,

     /* 6.10 Przedpłaty */
     SG_OBJ_ID_PREPAID_TOPUP_ID,
     SG_OBJ_ID_PREPAID_VALVE_CONTROL,
     SG_OBJ_ID_PREPAID_TOPUP_END_TIME,
     SG_OBJ_ID_PREPAID_TOPUP_END_VOLUME,
     SG_OBJ_ID_PREPAID_TOPUP_END_ENERGY,

     /* 6.11 Status */
     SG_OBJ_ID_STATUS,
     SG_OBJ_ID_STATUS_PERIODIC_LATCHED,
     SG_OBJ_ID_STATUS_MONTHLY_LATCHED,
     SG_OBJ_ID_STATUS_UP_EVENT_MASK,
     SG_OBJ_ID_STATUS_DOWN_EVENT_MASK,
     SG_OBJ_ID_STATUS_LATCHED,
     SG_OBJ_ID_STATUS_COMMAND,

     /* 6.12 Harmonogramy */
     SG_OBJ_ID_SCHEDULE_TABLE,

     /* 6.13 Struktury danych */
     SG_OBJ_ID_COL_OBJ_ID,
     SG_OBJ_ID_PERIODIC_VOLUME_DATA_LATCHED,
     SG_OBJ_ID_PERIODIC_VOLUME_DATA_CURRENT,
     SG_OBJ_ID_PERIODIC_ENERGY_DATA_LATCHED,
     SG_OBJ_ID_PERIODIC_ENERGY_DATA_CURRENT,
     SG_OBJ_ID_MONTHLY_DATA_LATCHED,
     SG_OBJ_ID_MONTHLY_DATA_CURRENT,
     SG_OBJ_ID_USER_STRUCTURE_1,
     SG_OBJ_ID_USER_STRUCTURE_2,
     SG_OBJ_ID_USER_STRUCTURE_3,
     SG_OBJ_ID_USER_STRUCTURE_4,
     SG_OBJ_ID_PERIODIC_VOLUME_DATA_DEF,
     SG_OBJ_ID_PERIODIC_ENERGY_DATA_DEF,
     SG_OBJ_ID_MONTHLY_DATA_DEF,
     SG_OBJ_ID_USER_STRUCTURE_1_DEF,
     SG_OBJ_ID_USER_STRUCTURE_2_DEF,
     SG_OBJ_ID_USER_STRUCTURE_3_DEF,
     SG_OBJ_ID_USER_STRUCTURE_4_DEF,
     SG_OBJ_ID_PERIODIC_VOLUME_DATA_ARCHIVE,
     SG_OBJ_ID_PERIODIC_ENERGY_DATA_ARCHIVE,
     SG_OBJ_ID_MONTHLY_DATA_ARCHIVE,

     /* 6.14 Parametry protokołu i bezpieczeństwo */
     SG_OBJ_ID_WDP_ARP,
     SG_OBJ_ID_WDP_AT,
     SG_OBJ_ID_WDP_MR,
     SG_OBJ_ID_WPP_CRYPTOGRAPHY_OBLIGATORY,
     SG_OBJ_ID_WPP_ACCESS_CONTROL_OBLIGATORY,
     SG_OBJ_ID_COL_ACCLVL_READ,
     SG_OBJ_ID_COL_ACCLVL_WRITE,
     SG_OBJ_ID_WPP_EVENT_SEND_SN,
     SG_OBJ_ID_SMS_PHONE_NUMBERS_RESTRICTION,
     SG_OBJ_ID_WPP_IV_IN,
     SG_OBJ_ID_ACCESS_PASS,
     SG_OBJ_ID_WPP_KEY,
     SG_OBJ_ID_PERMITTED_PHONE_NUMBER,
     SG_OBJ_ID_WPP_ACCESS_PASS_TABLE,
     SG_OBJ_ID_WPP_ACCESS_PERMISSIONS,
     SG_OBJ_ID_SMS_PHONE_NUMBERS_TABLE,

     /* 6.15 Czas rejestracji archiwum danych periodycznych i miesięcznych */
     SG_OBJ_ID_MONTHLY_DATA_LATCH_DAY,
     SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_HOUR,
     SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_LTIME,
     SG_OBJ_ID_LAST_DAILY_LATCH_TIME,
     SG_OBJ_ID_LAST_DAILY_LATCH_VOLUME,
     SG_OBJ_ID_LAST_DAILY_LATCH_ENERGY,
};


/******************************************************************************
 * Functions
 ******************************************************************************/

int main( void )
{
    int done; /* Flag for stopping the loop. */

    /* Initialization the hardware part. */
    hw_init();

    /* Wait for gas meter startup. */
    hw_delay( 2000 );

#ifdef DEBUG
    /* Print identification info. */
    printf( "MKG-01\nHW: %d_%04d_D_%d_%02d_%02d\nSW: %d_%04d_v%d_%d_%d\n",
                 MKG_HW_ID_GROUP, MKG_HW_ID_ITEM, MKG_HW_ID_REV_YEAR,
                 MKG_HW_ID_REV_MONTH, MKG_HW_ID_REV_DAY,
                 MKG_TEST_GM_SW_ID_GROUP, MKG_TEST_GM_SW_ID_ITEM,
                 MKG_TEST_GM_SW_ID_VER_MAJ, MKG_TEST_GM_SW_ID_VER_MIN,
                 MKG_TEST_GM_SW_ID_VER_REL );
#endif /* DEBUG */


    /* Loop for repetition of the test. */
    done = 0; /* Enable run. */
    while ( 0 == done )
    {
        /* GET TEST. */
        printf( "GET TEST\r\n");

        for ( uint8_t i = 0;
                i < ( sizeof obj_tbl / sizeof ( obj_tbl[0] ) );
                i++ )
        {
            GM_frame_t gm_resp = { 0 }; /* Empty frame. */
            SG_WO_OBJ_t obj_req = { 0 }, obj_resp = { 0 }; /* Empty objects. */

            /* Set the object id. */
            obj_req.id = obj_tbl[i];

            /* Current implementation just ask about required data from GM. */
            GM_frame_t gm_req;
            gm_req.func = GM_RADIO_DATA_FUNC;
            gm_req.options.sender = 1;
            gm_req.options.encypt = 0;
            gm_req.options.reserved = 0;
            gm_req.options.proto_ver = GM_PROTOCOL_VERSION;

            /* Build a PDU frame from object. */
            SG_OBJ_OP_STATUS_t status;

            status = sg_serialize_obj( WO_REQ_DIR_FROM_GM_TO_SC,
                    &obj_req, gm_req.data,
                    sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                    &gm_req.data_len );
            if ( SG_OBJ_OP_OK != status )
            {
                PRINT_ERROR("Error %d in serialization", status );
            }
            else
            {
                uint16_t obj_size;
                GM_status_t sent_ok = 0, recv_ok = 0;

                /* Send the frame about data. */
                sent_ok = gm_send_frame( &gm_req );

                /* Get a data from GM. */
                recv_ok = gm_receive_frame( &gm_resp );

                /* Debug info */
                printf( "<< [MKG] " );
                gm_print_frame( &gm_req );
                printf( " " );
                char obj_str[256];
                /* Print object. */
                sg_obj_to_str( &obj_req, obj_str, sizeof obj_str );
                printf( "%s", obj_str );
                printf( "\t" );
                printf( ">> [MCI] " );
                gm_print_frame( &gm_resp );
                printf( " " );

                /* Debug info */
                status = sg_deserialize_obj( WO_REQ_DIR_FROM_GM_TO_SC,
                                         gm_resp.data, gm_resp.data_len,
                                         &obj_resp, &obj_size );
                if ( SG_OBJ_OP_OK != status )
                {
                    PRINT_ERROR("Error %d in deserialization", status );
                }
                else
                {
                    /* Print object. */
                    sg_obj_to_str( &obj_req, obj_str, sizeof obj_str );
                    printf( "%s", obj_str );

                    char s[32];
                    str_bin_to_ascii( gm_resp.data, &s[0], gm_resp.data_len );
                    printf( " raw=0x%s", s );
                    /* Debug info */
                }

                /* Verification */
                if ( ( GM_OK == sent_ok ) && ( GM_OK == recv_ok ) )
                {
                    if ( ( SG_OBJ_OK == obj_resp.err )
                            && ( obj_req.id == obj_resp.id ) )
                    {
                        printf( " [OK]" );
                    }
                    else
                    {
                        printf( " [ERROR] OBJ_ERR" );
                    }
                }
                else
                {
                    printf( " [ERROR] COM_ERR s=%d, r=%d", sent_ok,
                                 recv_ok );
                }

                /* EOL. */
                printf( "\r" );

                hw_delay( 50 );
            }
        }

        hw_delay( 1000 );

        /* Stop. */
        done = 1;
    }

    /* Loop for repetition of the test. */
    done = 0; /* Enable run. */
    while ( 0 == done )
    {
        /* PUT TEST. */
        printf( "PUT TEST OF RANDOM DATA -- ERR_WRONG_VALUE CAN BE FALSE POSITIVE\r\n");

        for ( uint8_t i = 0; i < ( sizeof obj_tbl / sizeof ( obj_tbl[0] ) );
                i++ )
        {
            GM_frame_t gm_resp = { 0 }; /* Empty frame. */
            SG_WO_OBJ_t obj_req = { 0 }, obj_resp = { 0 }; /* Empty objects. */

            /* Set the object id. */
            obj_req.id = obj_tbl[i];

            /* Set PUT property. */
            obj_req.tskt = (bool) SG_OBJ_TSKT_PUT;

            /* Current implementation just ask about required data from GM. */
            GM_frame_t gm_req;
            gm_req.func = GM_RADIO_DATA_FUNC;
            gm_req.options.sender = 1;
            gm_req.options.encypt = 0;
            gm_req.options.reserved = 0;
            gm_req.options.proto_ver = GM_PROTOCOL_VERSION;

            /* Set the data. */
            uint8_t data[GM_FRAME_DATA_LENGTH];
            uint8_t data_len = 0;
            set_random_data( obj_req.id, data, sizeof data, &data_len );
            memcpy( obj_req.data, data, data_len > GM_FRAME_DATA_LENGTH ? GM_FRAME_DATA_LENGTH : data_len );
            obj_req.data_len = data_len;
            obj_req.valid = 1;

            /* Build a PDU frame from object. */
            SG_OBJ_OP_STATUS_t status;

            status = sg_serialize_obj( WO_REQ_DIR_FROM_MKG_TO_GM,
                    &obj_req, gm_req.data,
                    sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                    &gm_req.data_len );
            if ( SG_OBJ_OP_OK != status )
            {
                PRINT_ERROR("Error %d in serialization", status );
            }
            else
            {
                uint16_t obj_size;
                GM_status_t sent_ok = 0, recv_ok = 0;

                /* Send the frame about data. */
                sent_ok = gm_send_frame( &gm_req );

                /* Get a data from GM. */
                recv_ok = gm_receive_frame( &gm_resp );

                /* Debug info */
                printf( "<< [MKG] " );
                gm_print_frame( &gm_req );
                printf( " " );
                printf( "\t" );
                printf( ">> [MCI] " );
                gm_print_frame( &gm_resp );
                printf( " " );

                /* Debug info */

                status = sg_deserialize_obj( WO_REQ_DIR_FROM_GM_TO_SC,
                                         gm_resp.data, gm_resp.data_len,
                                         &obj_resp, &obj_size );
                if ( SG_OBJ_OP_OK != status )
                {
                    PRINT_ERROR("Error %d in deserialization", status );
                }
                else
                {
                    /* Debug info */
                    char obj_str[256];
                    char s[32];
                    /* Print object. */
                    sg_obj_to_str( &obj_resp, obj_str, sizeof obj_str );
                    printf( "%s", obj_str );
                    str_bin_to_ascii( gm_resp.data, &s[0], gm_resp.data_len );
                    printf( " raw=0x%s", s );
                    /* Debug info */
                }

                /* Verification */
                if ( ( GM_OK == sent_ok ) && ( GM_OK == recv_ok ) )
                {
                    if ( ( SG_OBJ_OK == obj_resp.err )
                            && ( obj_req.id == obj_resp.id ) )
                    {
                        printf( " [OK]" );
                    }
                    else
                    {
                        printf( " [ERROR] OBJ_ERR" );
                    }
                }
                else
                {
                    printf( " [ERROR] COM_ERR s=%d, r=%d", sent_ok,
                                 recv_ok );
                }

                /* EOL. */
                printf( "\r" );

                hw_delay( 50 );
            }
        }

        hw_delay( 1000 );

        /* Stop. */
        done = 1;
    }

    int r = 100;
    while (r--)
    {
        /* To output last data. */
        printf( "-" );
    }

}

static void set_random_data( SG_OBJ_ID_t id, uint8_t* data, const uint8_t max_data_len, uint8_t* data_len )
{
    if ( ( (void*) 0UL == data ) || ( (void*) 0UL == data_len ) )
    {
        return;
    }

    if ( ( id >= (SG_OBJ_ID_t) SG_OBJ_POOL_1_LOW ) && ( id <= (SG_OBJ_ID_t) SG_OBJ_POOL_1_HIGH ) )
    {
        if ( max_data_len > 0 )
        {
            data[0] = (uint8_t) rand();
            *data_len = 1;
        }
    }
    else if ( ( id >= (SG_OBJ_ID_t) SG_OBJ_POOL_2_LOW ) && ( id <= (SG_OBJ_ID_t) SG_OBJ_POOL_2_HIGH ) )
    {
        if ( max_data_len > 1 )
        {
            uint32_t r = (uint32_t) rand();
            data[0] = (uint8_t) ( r );
            data[1] = (uint8_t) ( r >> 8 );
            *data_len = 2;
        }
    }
    else if ( ( id >= (SG_OBJ_ID_t) SG_OBJ_POOL_3_LOW ) && ( id <= (SG_OBJ_ID_t) SG_OBJ_POOL_3_HIGH ) )
    {
        if ( max_data_len > 2 )
        {
            uint32_t r = (uint32_t) rand();
            data[0] = (uint8_t) ( r );
            data[1] = (uint8_t) ( r >> 8 );
            data[2] = (uint8_t) ( r >> 16 );
            *data_len = 3;
        }
    }
    else if ( ( id >= (SG_OBJ_ID_t) SG_OBJ_POOL_4_LOW ) && ( id <= (SG_OBJ_ID_t) SG_OBJ_POOL_4_HIGH ) )
    {
        if ( max_data_len > 3 )
        {
            uint32_t r = (uint32_t) rand();
            data[0] = (uint8_t) ( r );
            data[1] = (uint8_t) ( r >> 8 );
            data[2] = (uint8_t) ( r >> 16 );
            data[3] = (uint8_t) ( r >> 24 );
            *data_len = 4;
        }
    }
    else if ( ( id >= (SG_OBJ_ID_t) SG_OBJ_POOL_5_LOW ) && ( id <= (SG_OBJ_ID_t) SG_OBJ_POOL_5_HIGH ) )
    {
        if ( max_data_len > 4 )
        {
            uint32_t r = (uint32_t) rand();
            data[0] = (uint8_t) ( r );
            data[1] = (uint8_t) ( r >> 8 );
            data[2] = (uint8_t) ( r >> 16 );
            data[3] = (uint8_t) ( r >> 24 );
            uint32_t r1 = (uint32_t) rand();
            data[4] = (uint8_t) ( r1 );
            *data_len = 5;
        }
    }
    else if ( ( id >= (SG_OBJ_ID_t) SG_OBJ_POOL_6_LOW ) && ( id <= (SG_OBJ_ID_t) SG_OBJ_POOL_6_HIGH ) )
    {
        if ( max_data_len > 6 )
        {
            uint32_t r = (uint32_t) rand();
            data[0] = (uint8_t) ( r );
            data[1] = (uint8_t) ( r >> 8 );
            data[2] = (uint8_t) ( r >> 16 );
            data[3] = (uint8_t) ( r >> 24 );
            uint32_t r1 = (uint32_t) rand();
            data[4] = (uint8_t) ( r1 );
            data[5] = (uint8_t) ( r1 >> 8 );
            *data_len = 6;
        }
    }
}

/*** end of file ***/

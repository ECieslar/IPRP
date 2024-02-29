/*!****************************************************************************
 *
 * \file mkg_sg_app.c
 * \brief MKG module SMART-GAS application.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "mkg_sg_app.h"
#include "mkg_sg.h"
#include "../gsm/gsm_comm.h"
#include "../gm/gm_comm.h"
#include "../gsm/tcp_comm.h"
#include "../gsm/gprs_comm.h"
#include "../hw/mkg_hw.h"
#include "../util/str_util.h"
#include "../sg/sg_wd.h"
#include "../sg/sg_wp.h"
#include "../util/crc.h"
#include "../util/debug.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*! \brief General app contants. */
enum MKG_SG_APP_General
{
    SG_DATA_MAX_LEN = 2048, /* Length of data from Gas-Meter. */
    SC_DATA_MAX_LEN = 1024, /* Length of data from Central Server. */
    SC_SMS_DISPOSABLE_LIMIT = 10, /* Limit of messages gathered in one time .*/
    SC_REQUESTED_OBJECTS_LIMIT = 10 /* Object count limit requested by SC. */
};

/*! \brief Local data definitions.
 *
 * IMPORTANT Remember to align data in Flash to half word boundary.
 */
enum MKG_SG_APP_Data
{
    WORKING_TIME_WORD_FLASH_DATA = 0U, /* Position of working time counter in the data EEPROM memory. */
    WAIT_FOR_SC_TIME_SHORT_FLASH_DATA = 4U, /* Position of wait time for SC activity in the data EEPROM memory. */
    DATAGRAM_REF_NUM_BYTE_FLASH_DATA = 6U, /* Position of datagram reference number in the data EEPROM memory. */
};

/*!
 * \brief Local appllication's errors.
 */
enum MKG_SG_APP_ERRORS
{
    SG_APP_OK,                         /*!< SG_APP_OK */
    SG_APP_ERROR_NO_APP_DATA,          /*!< No application data. */
    SG_APP_ERROR_MODEM_SMSC_IS_NOT_SET,/*!< SG_APP_ERROR_MODEM_SMSC_IS_NOT_SET */
    SG_APP_ERROR_PACKET_SERIALIZATION,/*!< Error in the packet serialization. */
    SG_APP_ERROR_UNKNOW, /*!< Uknown error. */
};

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

struct msg_part
{
    uint8_t refno; /* Datagram's reference number. */
    uint8_t part; /* Datagram's part number. */
    uint8_t len; /* The lenght of data in the message. */
    uint8_t data[SG_WD_MAX_DATA_LEN]; /* Received message data. */
}; /* Received messages. */

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* Datagram reference number. */
extern uint8_t sg_dtg_ref;

/* Time of waiting for data from SC. */
extern uint8_t wait_for_cs_time_limit;

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/*!
 * \brief Initialize the local data.
 */
static void data_init( void );

/*!
 * \brief Send information about MKG to GM.
 */
static void send_GM_invitation( void );

/*!
 * \brief Send information about GSM to GM.
 */
static void send_GM_GSM_data( void );

/*!
 * \brief Wait for Gas-Meter data.
 */
static void get_GM_default_data( void );

/*!
 * \brief Update the module's local data.
 */
static void update_local_data( SG_WO_OBJ_t *obj );

/*!
 * \brief Connect to GPRS service.
 */
static bool start_gprs_client( void );


/*!
 * \brief Disconnect from GPRS service.
 */
static void stop_gprs_client();

/*!
 * \brief Send the standard information to the Central Server.
 */
static uint16_t send_data_to_CS( void );

/*!
 * \brief Wait for confirmation from CS.
 * \param cfm_dtg_id Returned in confiramtion datagram number.
 * \return Returns 0 if success; or an error code otherwise.
 *
 * \details Confirmation parameters that control behaviour o
 */
static uint16_t wait_for_confirm_from_CS( uint16_t* cfm_dtg_id );

/*!
 * \brief Send the confirmation to CS.
 * \param max_count Number of datagrams
 * \param lcrd_id the last continously received datagram's id.
 * \return Returns 0 if success; or an error code otherwise.
 */
static uint16_t send_confirm_to_CS( uint16_t max_count, uint16_t lcrd_id );

/*!
 * \brief Waits for data from the CS.
 * \return True comes data from the CS, False no data in the period.
 */
static bool wait_for_data_from_CS();

/*!
 * \brief Process the data got from the CS.
 */
static void process_data_from_CS( void );

/*!
 * \brief Send back the usage statistics to Gas Meter.
 */
static void send_GM_usage_statistics( void );

/*!
 * \brief Store the local data to the EEPROM memory.
 */
static void store_local_data( void );

/*!
 * \brief Notify the GM about end of communication session.
 */
static void send_GM_bye( void );

/*!
 * \brief Wait for power off (goes sleep).
 */
static void wait_for_power_off( void );

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/* Total working time. */
static uint32_t total_up_time;

/* GM data. */
static uint8_t gm_data[SG_DATA_MAX_LEN];
/* GM data length. */
static uint16_t gm_data_len = 0U;

/* SC data. */
static uint8_t sc_data[SC_DATA_MAX_LEN];
/* SC data length. */
static uint16_t sc_data_len = 0U;

/* Response link technique. */
static SG_EVENT_LINK_t link_type = SG_LINK_GPRS_TCP;

/* Response link address. */
static uint8_t* link_addr = (uint8_t*) "\x3e\x57\xb9\x2c\x9c\xaf"; /* 62.87.185.44:40111 */

/* Text message format. */
static const GSM_mode_t msg_fmt = GSM_SMS_PDU_MODE;

/* SC IP address and TCP/UDP port */
static char sc_ip_addr[16];
static uint16_t sc_port;

#ifdef DEBUG
/* Data debug string. */
static char data_dbg_str[ ( ( SG_DATA_MAX_LEN > SC_DATA_MAX_LEN ) ? SG_DATA_MAX_LEN : SC_DATA_MAX_LEN )
                         + 1];
#endif /* DEBUG */

/******************************************************************************
 * Functions
 ******************************************************************************/

/*! \brief The main function of the application for sending measuring data
 * from the GM (gas-meter) to the CS (Central Server).
 */
int main( void )
{
    /* TODO Add the state-driven loop. */

    /* Initialization the hardware part. */
    hw_init();

    /* Print identification info. */
    PRINT_DEBUG( "Application startup: "
                 "MKG-01 HW=%d_%d_D_%d_%02d_%02d "
                 "SW=%d_%d_v%d_%d_%d_%d",
                 MKG_HW_ID_GROUP, MKG_HW_ID_ITEM, MKG_HW_ID_REV_YEAR,
                 MKG_HW_ID_REV_MONTH, MKG_HW_ID_REV_DAY, MKG_SW_ID_GROUP,
                 MKG_SW_ID_ITEM, MKG_SW_ID_VER_MAJ, MKG_SW_ID_VER_MIN,
                 MKG_SW_ID_VER_REL, MKG_SW_ID_VER_BLD );

    /* Initialize the data part. */
    data_init();

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

    /* Wait for startup. */
    gsm_wait_for_modem_on( MODEM_ON_TIME_LIMIT_DEFAULT );

    /* Get the basic information from the modem. */
    gsm_get_modem_info();

    /* Send identification information to the GM. */
    send_GM_invitation();

    /* Wait for connection to the GSM Network. */
    gsm_wait_for_connect( MODEM_CONNECT_TIME_LIMIT_DEFAULT );

    /* Get the connection information from the GSM network. */
    gsm_get_connection_info();

    /* Send information about connection to GM. */
    send_GM_GSM_data();

    /* Wait for the main data from GM. */
    get_GM_default_data();

    /* Connect to data packet service. */
    start_gprs_client();

    /* Send the data to CS and wait for confirmation. */
    send_data_to_CS();

    /* Wait for data from CS. */
    while ( wait_for_data_from_CS() )
    {
        /* Get request from the CS,
         * pass to the GM
         * and send reply to the CS.
         */
        process_data_from_CS();
    }

    /* Disconnect from GPRS service. */
    stop_gprs_client();

    /* Disconnect from the GSM Network. */
    gsm_disconnect_modem();

    /* and power off the modem. */
    gsm_power_off_modem();

    /* Send to GM information about usage. */
    send_GM_usage_statistics();

    /* Save the local data. */
    store_local_data();

    /* Allows for power off. */
    send_GM_bye();

    /* Wait for power off. */
    wait_for_power_off();
}

static void data_init( void )
{
    /* Clear the global data tables and size. */
    memset( gm_data, 0, sizeof gm_data / sizeof gm_data[0] );
    gm_data_len = 0U;
    memset( sc_data, 0, sizeof sc_data / sizeof sc_data[0] );
    sc_data_len = 0U;

    /* Load the data from Flash memory. */
    sg_dtg_ref = hw_read_byte_data_eeprom( DATAGRAM_REF_NUM_BYTE_FLASH_DATA );

    PRINT_INFO( "Load local data: datagram reference counter: %u", sg_dtg_ref );

    total_up_time = hw_read_word_data_eeprom( WORKING_TIME_WORD_FLASH_DATA );

    PRINT_INFO( "Load local data: total up time: %lu",
                (unsigned long ) total_up_time );

    wait_for_cs_time_limit = hw_read_short_data_eeprom(
            WAIT_FOR_SC_TIME_SHORT_FLASH_DATA );

    PRINT_INFO( "Load local data: wait for SC time: %u",
                wait_for_cs_time_limit );
}

static void send_GM_invitation( void )
{
    GM_frame_t gm_req = { 0U }; /* Request frame with 0 data. */
    GM_frame_t gm_resp = { 0U }; /* Request frame with 0 data. */
    /* List of object id to be passed. */
    uint16_t req_obj_ids[] = { MKG_SG_OBJ_MOD_ID, MKG_SG_OBJ_HW_VER,
                               MKG_SG_OBJ_SW_VER, MKG_SG_OBJ_SN };
    SG_WO_OBJ_t obj; /* SG object. */

    /* Basic frame's settings. */
    gm_req.func = GM_RADIO_DATA_FUNC;
    gm_req.options.sender = 1;
    gm_req.options.encypt = 0;
    gm_req.options.reserved = 0;
    gm_req.options.proto_ver = GM_PROTOCOL_VERSION;
    gm_req.data_len = 0;

    for ( uint8_t i = 0; i < ( sizeof req_obj_ids / sizeof(SG_OBJ_ID_t) ); i++ )
    {
        /* TODO Add an error handling. */
        if ( MKG_OK == sg_get_mkg_object( req_obj_ids[i], &obj ) )
        {
            /* TODO Add an error handling. */
            if ( SG_OBJ_OP_OK == sg_serialize_obj(
                    WO_REQ_DIR_FROM_MKG_TO_GM, &obj, gm_req.data,
                    sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                    &gm_req.data_len ) )
            {
                GM_status_t gm_status;

                /* Send prepared data. */
                gm_status = gm_send_frame( &gm_req );

                if ( GM_OK != gm_status )
                {
                    PRINT_ERROR( "Error %d at receiving a response from GM",
                                 gm_status );

                    /* TODO Set a global status error code. */
                }
                else
                {
                    /* Get confirmation. */
                    gm_status = gm_receive_frame( &gm_resp );

                    if ( GM_OK != gm_status )
                    {
                        PRINT_ERROR( "Error %d at receiving a response from GM",
                                     gm_status );

                        /* TODO Add a loop to re-send data in the case of error. */
                    }
                    else
                    {

                    }
                }
            }
        }
    }
}

static void send_GM_GSM_data( void )
{
    GM_frame_t gm_req = { 0 }; /* Request frame with 0 data. */
    GM_frame_t gm_resp = { 0 }; /* Request frame with 0 data. */
    /* List of object id to be passed. */
    const SG_OBJ_ID_t req_obj_ids[] =
        { SG_OBJ_ID_MODEM_STATE, SG_OBJ_ID_GSM_QUALITY,
          SG_OBJ_ID_GSM_NETWORK_CODE, MKG_SG_OBJ_GSM_CLOCK,
          SG_OBJ_ID_GSM_BTS_LAC, SG_OBJ_ID_GSM_BTS_CELL_ID };
    SG_WO_OBJ_t obj; /* SG object. */

    /* Basic frame's settings. */
    gm_req.func = GM_RADIO_DATA_FUNC;
    gm_req.options.sender = 1;
    gm_req.options.encypt = 0;
    gm_req.options.reserved = 0;
    gm_req.options.proto_ver = GM_PROTOCOL_VERSION;
    gm_req.data_len = 0;

    for ( uint8_t i = 0; i < ( sizeof req_obj_ids / sizeof(SG_OBJ_ID_t) ); i++ )
    {
        /* TODO Add an error handling. */
        if ( MKG_OK == sg_get_mkg_object( req_obj_ids[i], &obj ) )
        {
            /* TODO Add an error handling. */
            if ( SG_OBJ_OP_OK == sg_serialize_obj(
                    WO_REQ_DIR_FROM_MKG_TO_GM, &obj, gm_req.data,
                    sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                    &gm_req.data_len ) )
            {
                GM_status_t gm_status;

                /* Send prepared data. */
                gm_status = gm_send_frame( &gm_req );

                if ( GM_OK != gm_status )
                {
                    PRINT_ERROR( "Error %d at receiving a response from GM",
                                 gm_status );

                    /* TODO Set a global status error code. */
                }
                else /* GM_OK */
                {
                    /* Get confirmation. */
                    gm_status = gm_receive_frame( &gm_resp );

                    if ( GM_OK != gm_status )
                    {
                        PRINT_ERROR( "Error %d at receiving a response from GM",
                                     gm_status );

                        /* TODO Add a loop to re-send data in the case of error. */
                    }
                    else
                    {

                    }
                }
            }
        }
    }
}

static void get_GM_default_data( void )
{
    /* List of object ids to be queried if default periodic list is empty. */
    const SG_OBJ_ID_t all_objects_list[] =
        {
          /* Informacyjne (6.1). */
          SG_OBJ_ID_SERIAL_NBR,
          SG_OBJ_ID_CLIENT_ID, SG_OBJ_ID_GAS_METER_SERIAL_NBR,
          SG_OBJ_ID_LATITUDE, SG_OBJ_ID_LONGITUDE, SG_OBJ_ID_HARDWARE_VERSION,
          SG_OBJ_ID_FIRMWARE_VERSION, SG_OBJ_ID_DEVICE_TYPE_NAME,
          SG_OBJ_ID_STANDARD_PARAMETER_LIST_VERSION,

          /* Liczydła i zużycia (6.2). */
          SG_OBJ_ID_UNITS,
          SG_OBJ_ID_VOLUME, SG_OBJ_ID_VOLUME_MAX_VALUE,
          SG_OBJ_ID_COMMAND_VOLUME_OFFSET, SG_OBJ_ID_ENERGY,
          SG_OBJ_ID_COMMAND_ENERGY_OFFSET, SG_OBJ_ID_CALORIFIC_VALUE,
          SG_OBJ_ID_COUNTERS_ARCHIVE, SG_OBJ_ID_COUNTERS_REGISTRATION_PERIOD,
          SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_TIME,
          SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_VOLUME,
          SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_ENERGY,
          SG_OBJ_ID_PERIOD_VOLUME_USAGE_COMPRESSED,
          SG_OBJ_ID_PERIOD_ENERGY_USAGE_COMPRESSED,
          SG_OBJ_ID_MONTH_VOLUME_USAGE_COMPRESSED,
          SG_OBJ_ID_MONTH_ENERGY_USAGE_COMPRESSED,

          /* Przepływ (6.3) */
          SG_OBJ_ID_GAS_METER_Q_MAX,
          SG_OBJ_ID_TEMPORARY_FLOW, SG_OBJ_ID_HOURLY_USAGE_CURRENT,
          SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED,
          SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED,
          SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED,
          SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED,
          SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED,
          SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED,
          SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED,
          SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED,

          /* Zegar (6.4). */
          SG_OBJ_ID_CLOCK,
          SG_OBJ_ID_CLOCK_SYNCHRONISATION_OFFSET, SG_OBJ_ID_TIME_ZONE_OFFSET,
          SG_OBJ_ID_TIME_ZONE_OFFSET_WINTER, SG_OBJ_ID_TIME_ZONE_OFFSET_SUMMER,
          SG_OBJ_ID_TIME_ZONE_AUTO_OFFSET,
          SG_OBJ_ID_CLOCK_SYNCHRONISATION_FROM_GSM,

          /* Ingerencja (6.5) */
          SG_OBJ_ID_TAMPER_REMOVE_TIME,
          SG_OBJ_ID_TAMPER_REMOVE_DURATION, SG_OBJ_ID_TAMPER_REMOVE_COUNT,
          SG_OBJ_ID_TAMPER_REMOVE_VOLUME, SG_OBJ_ID_TAMPER_REMOVE_ENERGY,
          SG_OBJ_ID_TAMPER_OPEN_TIME, SG_OBJ_ID_TAMPER_OPEN_DURATION,
          SG_OBJ_ID_TAMPER_OPEN_COUNT, SG_OBJ_ID_TAMPER_OPEN_VOLUME,
          SG_OBJ_ID_TAMPER_OPEN_ENERGY, SG_OBJ_ID_TAMPER_MAGNETIC_TIME,
          SG_OBJ_ID_TAMPER_MAGNETIC_DURATION, SG_OBJ_ID_TAMPER_MAGNETIC_COUNT,
          SG_OBJ_ID_TAMPER_MAGNETIC_VOLUME, SG_OBJ_ID_TAMPER_MAGNETIC_ENERGY,

          /* Diagnostyki (6.6) */
          SG_OBJ_ID_BATTERY_LEVEL,
          SG_OBJ_ID_TEMPERATURE,

          /* Modem (6.7) */
          SG_OBJ_ID_SIM_PIN, SG_OBJ_ID_SIM_ICCID,
          SG_OBJ_ID_SIM_PHONE, SG_OBJ_ID_SIM_IP,
          SG_OBJ_ID_SIM_SMSC, SG_OBJ_ID_SIM_SMS_VALIDITY,
          SG_OBJ_ID_GPRS_APN, SG_OBJ_ID_GPRS_USER, SG_OBJ_ID_GPRS_PASSWORD,
          SG_OBJ_ID_GSM_QUALITY,
          SG_OBJ_ID_GSM_NETWORK_CODE,
          SG_OBJ_ID_GSM_BTS_LAC, SG_OBJ_ID_GSM_BTS_CELL_ID,
          SG_OBJ_ID_MODEM_STATE,
          SG_OBJ_ID_MODEM_SMS_RECEIVED, SG_OBJ_ID_MODEM_SMS_SENT,
          SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED, SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT,
          SG_OBJ_ID_MODEM_WAKEUP_COUNT,
          SG_OBJ_ID_MODEM_WAKEUP_DURATION,

          /* Konfiguracja zdarzeń (6.8) */
          SG_OBJ_ID_EVENT_LINK,
          SG_OBJ_ID_EVENT_PHONE, SG_OBJ_ID_EVENT_IP, SG_OBJ_ID_EVENT_UDP_PORT,
          SG_OBJ_ID_EVENT_TCP_PORT,

          /* Zawór (6.9) */
          SG_OBJ_ID_VALVE_OPERATION,
          SG_OBJ_ID_VALVE_ACTIVATION_TIMEOUT, SG_OBJ_ID_VALVE_STATE,
          SG_OBJ_ID_VALVE_LEAK_TEST_DURATION, SG_OBJ_ID_VALVE_LEAK_TEST_VOLUME,
          SG_OBJ_ID_VALVE_ERROR_CODE, SG_OBJ_ID_VALVE_OPEN_COUNT,
          SG_OBJ_ID_VALVE_CLOSE_COUNT,

          /* Przedpłaty (6.10) */
          SG_OBJ_ID_PREPAID_TOPUP_ID,
          SG_OBJ_ID_PREPAID_TOPUP_END_TIME, SG_OBJ_ID_PREPAID_TOPUP_END_VOLUME,
          SG_OBJ_ID_PREPAID_TOPUP_END_ENERGY, SG_OBJ_ID_PREPAID_VALVE_CONTROL,

          /* Status (6.11) */
          SG_OBJ_ID_STATUS,
          SG_OBJ_ID_STATUS_PERIODIC_LATCHED, SG_OBJ_ID_STATUS_MONTHLY_LATCHED,
          SG_OBJ_ID_STATUS_UP_EVENT_MASK, SG_OBJ_ID_STATUS_DOWN_EVENT_MASK,
          SG_OBJ_ID_STATUS_LATCHED, SG_OBJ_ID_STATUS_COMMAND,

          /* Harmonogramy (6.12) */
          SG_OBJ_ID_SCHEDULE_TABLE,

          /* Struktury danych (6.13) */
          SG_OBJ_ID_PERIODIC_VOLUME_DATA_LATCHED,
          SG_OBJ_ID_PERIODIC_VOLUME_DATA_CURRENT,
          SG_OBJ_ID_PERIODIC_VOLUME_DATA_DEF,
          SG_OBJ_ID_PERIODIC_ENERGY_DATA_LATCHED,
          SG_OBJ_ID_PERIODIC_ENERGY_DATA_CURRENT,
          SG_OBJ_ID_PERIODIC_ENERGY_DATA_DEF, SG_OBJ_ID_MONTHLY_DATA_LATCHED,
          SG_OBJ_ID_MONTHLY_DATA_CURRENT, SG_OBJ_ID_MONTHLY_DATA_DEF,
          SG_OBJ_ID_PERIODIC_VOLUME_DATA_ARCHIVE,
          SG_OBJ_ID_PERIODIC_ENERGY_DATA_ARCHIVE,
          SG_OBJ_ID_MONTHLY_DATA_ARCHIVE, SG_OBJ_ID_USER_STRUCTURE_1,
          SG_OBJ_ID_USER_STRUCTURE_1_DEF, SG_OBJ_ID_USER_STRUCTURE_2,
          SG_OBJ_ID_USER_STRUCTURE_2_DEF, SG_OBJ_ID_USER_STRUCTURE_3,
          SG_OBJ_ID_USER_STRUCTURE_3_DEF, SG_OBJ_ID_USER_STRUCTURE_4,
          SG_OBJ_ID_USER_STRUCTURE_4_DEF,

          /* Parametry protokołu i bezpieczeństwo (6.14) */
          SG_OBJ_ID_WDP_ARP,
          SG_OBJ_ID_WDP_AT, SG_OBJ_ID_WDP_MR, SG_OBJ_ID_WPP_EVENT_SEND_SN,
          SG_OBJ_ID_WPP_IV_IN, SG_OBJ_ID_WPP_KEY,
          SG_OBJ_ID_WPP_ACCESS_PASS_TABLE, SG_OBJ_ID_ACCESS_PASS,
          SG_OBJ_ID_WPP_CRYPTOGRAPHY_OBLIGATORY,
          SG_OBJ_ID_WPP_ACCESS_CONTROL_OBLIGATORY,
          SG_OBJ_ID_WPP_ACCESS_PERMISSIONS,
          SG_OBJ_ID_SMS_PHONE_NUMBERS_RESTRICTION,
          SG_OBJ_ID_SMS_PHONE_NUMBERS_TABLE, SG_OBJ_ID_PERMITTED_PHONE_NUMBER,

          /* Czas rejestracji archiwum danych periodycznych i miesięcznych (6.15) */
          SG_OBJ_ID_MONTHLY_DATA_LATCH_DAY,
          SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_HOUR,
          SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_LTIME,
          SG_OBJ_ID_LAST_DAILY_LATCH_TIME, SG_OBJ_ID_LAST_DAILY_LATCH_VOLUME,
          SG_OBJ_ID_LAST_DAILY_LATCH_ENERGY,

          /* Obiekty MKG */
          MKG_SG_OBJ_MOD_ID,
          MKG_SG_OBJ_HW_VER, MKG_SG_OBJ_SW_VER, MKG_SG_OBJ_SN,
          MKG_SG_OBJ_GSM_CLOCK,
        };

    GM_frame_t gm_req = { 0 }; /* Request to GM. */
    GM_frame_t gm_resp = { 0 }; /* Response from GM. */
    SG_WO_OBJ_t obj; /* SG object. */

    /* Current implementation just ask about required data from GM. */
    gm_req.func = GM_RADIO_DATA_FUNC;
    gm_req.options.sender = 1;
    gm_req.options.encypt = 0;
    gm_req.options.reserved = 0;
    gm_req.options.proto_ver = GM_PROTOCOL_VERSION;
    gm_req.data_len = 0;

    for ( uint8_t i = 0; i < ( sizeof all_objects_list / sizeof(SG_OBJ_ID_t) ); i++ )
    {
        obj.id = (SG_OBJ_ID_t) all_objects_list[i]; /* Set an id only. */
        obj.err = SG_OBJ_OK;
        obj.tskt = SG_OBJ_TSKT_GET;
        obj.valid = 1; /* Set the data complete flag. */

        if ( SG_OBJ_OP_OK == sg_serialize_obj(
                WO_REQ_DIR_FROM_MKG_TO_GM, &obj, gm_req.data,
                sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                &gm_req.data_len ) )
        {
            /* Send the frame with request for data. */
            if ( GM_OK == gm_send_frame( &gm_req ) )
            {
                /* Get data from GM. */
                if ( GM_OK == gm_receive_frame( &gm_resp ) )
                {
                    uint16_t obj_size = 0U;
                    memset( (void*) &obj, 0U, sizeof ( obj ) );

                    if ( SG_OBJ_OP_OK == sg_deserialize_obj(
                            WO_REQ_DIR_FROM_GM_TO_SC, gm_resp.data,
                            gm_resp.data_len, &obj, &obj_size ) )
                    {

                        /* Get the important data for the module. */
                        update_local_data( &obj );

                        /* Copy an output data to the global data table. */
                        memcpy( &gm_data[gm_data_len], gm_resp.data,
                                gm_resp.data_len );
                        gm_data_len += gm_resp.data_len;
                    }
                }
            }
        }
    }

#ifdef DEBUG

#ifdef TEST_GM_DATA
    /* Check for GM data, if there isn't use sample data. */
    if ( 0UL == gm_data_len )
    {
        const uint8_t sample_gm_data[] = "\x04\x00\x47\x03\x31\x29\x05\x00\x05\x78\x7F\x8B\x1A\xD8\x05\x01\x05\x68\x48\x89\x2B\xA7\x04\x01\x30\x70\x5B\x04\x04\x02\x00\x1B\x00\x00\x04\x03\x04\x00\x00\x00\x04\x04\x05\x72\x00\x00\x05\x02\x05\xFF\xB7\xCD\x08\x3F\x02\x00\x00\x00\x01\x00\x0F\x04\x05\x00\x00\x00\x00\x84\x2B\x01\x80\x84\x06\x01\x80\x04\x07\x00\x00\x00\x00\x84\x08\x01\x80\x02\x01\x41\xFC\x86\x00\x04\x80\x01\x01\x3C\x04\x09\x92\x51\x6D\x38\x04\x0A\x00\x00\x00\x00\x04\x0B\x00\x00\x00\x00\x85\x03\x01\x80\x85\x04\x01\x80\x85\x05\x01\x80\x85\x06\x01\x80\x02\x02\x3C\x00\x02\x03\x00\x00\x02\x04\x00\x00\x02\x05\x00\x00\x02\x06\x00\x00\x02\x07\x00\x00\x01\x02\x00\x02\x08\x00\x00\x02\x09\x00\x00\x02\x0A\x00\x00\x02\x0B\x00\x00\x04\x0C\x20\x53\x6D\x38\x02\x0C\x97\x7B\x01\x03\x44\x01\x04\xDF\x01\x05\x59\x81\x24\x01\x80\x01\x06";

        /* Copy an output data to the global data table. */
        memcpy( gm_data, sample_gm_data, sizeof sample_gm_data / sizeof sample_gm_data[0] );
    }
#endif /* TEST_GM_DATA */

    str_bin_to_ascii( gm_data, data_dbg_str, gm_data_len );
    data_dbg_str[2 * gm_data_len] = '\0';

    PRINT_DEBUG( "GM data: len=%d,data=\"%s\"", gm_data_len, data_dbg_str );

#endif /* DEBUG */
}

static void update_local_data( SG_WO_OBJ_t *obj )
{
    MKG_status_t status;

    if ( ( (void*) 0UL ) == obj )
    {
        PRINT_ERROR( "Invalid input parametr." );
        return;
    }

    switch ( (int) obj->id )
    /* Not every value is type of SG_OBJ_ID_t. */
    {
        case MKG_SG_OBJ_SC_TIMEOUT:
        case SG_OBJ_ID_SIM_SMSC:
        case SG_OBJ_ID_SIM_SMS_VALIDITY:
            /* Update MKG object. */
            status = sg_set_mkg_object( obj );
            if ( MKG_OK != status )
            {
                PRINT_ERROR(
                        "Error %d occures while updating local MKG object %s",
                        status, sg_get_mkg_obj_id_name( (int ) obj->id ) );
            }
            break;

        default:
            /* do nothing */
            break;
    }
}

static bool check_modem_smsc( const char* smsc_str )
{
    if ( 0UL == strlen( smsc_str ) )
    {
        PRINT_ERROR( "SMSC not set." );

        return false;
    }
    else
    {
        uint8_t i = 0U;
        if ( '+' == smsc_str[0] )
        {
            i = 1U;
        }
        while ( i < strlen(smsc_str) )
        {
            if( ( smsc_str[i] < '0' ) || ( smsc_str[i] > '9' ) )
            {
                PRINT_ERROR( "SMSC has invalid format." );

                return false;
            }
            i++;
        }

        PRINT_INFO( "SMSC set to: %s", smsc_str );
    }

    return true;
}

static bool start_gprs_client( void )
{
    /* Make GPRS connection if needed. */
    if ( ( SG_LINK_GPRS_TCP == link_type )
         || ( SG_LINK_GPRS_UDP == link_type ) )
    {
        if ( !gprs_get_state() || ( 0 == strcmp( "0.0.0.0", gprs_get_ip() ) ) )
        {
            gprs_enable( true );

            hw_delay(1000UL);

            PRINT_INFO( "Connect to GPRS service with IP=%s", gprs_get_ip() );

            return true;
        }
    }

    return false;
}

void stop_gprs_client()
{
    /* Close GPRS connection if exists. */
    if ( ( SG_LINK_GPRS_TCP == link_type )
         || ( SG_LINK_GPRS_UDP == link_type ) )
    {
        if ( gprs_get_state() && ( 0 != strcmp( "", gprs_get_ip() ) ) )
        {
            if ( tcp_is_connected() )
            {
                tcp_close();
            }

            gprs_enable( false );
        }
    }
}

static uint16_t send_data_to_CS( void )
{
    uint16_t rc = SG_APP_ERROR_UNKNOW; /* Returned value. */

    /* Check for input data. */
    if ( 0U == gm_data_len )
    {
        PRINT_INFO( "No data to sent to SMSC" );

        rc = SG_APP_ERROR_NO_APP_DATA;
    }
    else /* Dane są niepuste. */
    {
        struct sent_stats
        {
            uint8_t sent_count;
            uint16_t sent_bytes;
        } sent_stats_counter = { 0U, 0U };

        SG_WP_PDU_t pkt; /* Data packet. */

        /* Start new session. */
        create_new_session();

        /* Set the packet's header. */
        SG_WP_CTRL_t pkt_ctrl_default = { .ctrl_ver = 0x1,
                                          .ctrl_sn = 0,
                                          .ctrl_enc = 0,
                                          .ctrl_rad = 0,
                                          .ctrl_rlk = 0,
                                          .ctrl_dir = 0,
                                          .ctrl_acc = 0 };

        pkt.header.crtl = pkt_ctrl_default;
        pkt.header.wp_seso = 0U;
        pkt.header.wp_sesid = get_packet_session_id();
        pkt.data_len = gm_data_len;
        pkt.data_ptr = gm_data;
        const uint16_t packet_size = get_packet_size( &pkt );
        pkt.header.wp_len = packet_size - 5; /* - WP_CTRL (1) - WP_LEN (2) - CRC16 (2) */

        /* The data for packet. */
        uint8_t pkt_data[SG_WP_DATA_MAX_LEN];
        uint16_t ser_rc; /* Serialize result. */
        uint16_t pkt_data_len; /* Serialize result. */

        /* Clear the data. */
        memset( pkt_data, 0U, sizeof pkt_data / sizeof pkt_data[0] );

        /* Serialize the packet to data table. */
        ser_rc = serialize_packet( &pkt, pkt_data, sizeof pkt_data / sizeof pkt_data[0], &pkt_data_len );

        if ( SG_WP_OK != ser_rc )
        {
            PRINT_ERROR( "Error (%u) in serialization the packet", ser_rc );

            rc = SG_APP_ERROR_PACKET_SERIALIZATION;
        }
        else
        {
            /* Calculate CRC. */
            pkt.crc = calculate_crc16_ccitt_false( pkt_data, pkt_data_len );

            /* Add CRC to data table. */
            pkt_data[pkt_data_len++] = (uint8_t) ( pkt.crc << 8 );
            pkt_data[pkt_data_len++] = (uint8_t) pkt.crc;

            if ( SG_LINK_SMS == link_type )
            {
                /* Check for the SC phone number. */
                if ( !check_modem_smsc( modem_sim_data.smsc ))
                {
                    rc = SG_APP_ERROR_MODEM_SMSC_IS_NOT_SET;
                }
                else
                {
                    uint8_t sms_count; /* SMS count. */
                    uint8_t sms_data_part_max_len; /* Max length of data in SMS. */

                    SG_WD_PDU_t pdu; /* Datagram unit. */

                    /* Encapsulate the packet into a datagram. */

                    /* Increment datagram reference number. */
                    sg_dtg_ref += 1;

                    /* Set the header. */
                    SG_IE_t ie_table[] = {
                            [SG_WD_IE_ID_MULTI_DTG] = { .ie_i = SG_WD_IE_ID_MULTI_DTG,
                                                        .ie_l = 3U,
                                                        .ie_d.dtg = { .dtg_ref = sg_dtg_ref, .dtg_ar = 0,
                                                                      .dtg_tc = 0, .dtg_nr = 0 }
                                                      },
                            [SG_WD_IE_ID_DETAILS_DTG] = { .ie_i = SG_WD_IE_ID_DETAILS_DTG,
                                                          .ie_l = 3U,
                                                          .ie_d.dd = { .dd_len = 0U, .dd_crc = 0U } }
                        };
                    pdu.header.ie_ptr = ie_table;
                    pdu.header.ie_max_count = sizeof ( ie_table ) / sizeof( SG_IE_t );
                    pdu.header.ie_count = pdu.header.ie_max_count;
                    pdu.header.wd_hl = sizeof ( ie_table ) - 2; /* should be 10, but in memory occupies  12 bytes. */

                    /* Determine data length size. */
                    if ( GSM_SMS_ASCII_MODE == msg_fmt )
                    {
                        /* one extra byte (+ 1) is for WD_HL */
                        sms_data_part_max_len = GSM_SMS_MAX_LENGTH - ( 2 * ( pdu.header.wd_hl + 1 ) );

                        /* Count the number of messages. */
                        sms_count = ( 0 == ( ( 2 * packet_size ) % ( sms_data_part_max_len ) ) /* Full part detection */
                                        ? ( 2 * packet_size ) / ( sms_data_part_max_len ) /* Full parts */
                                        : ( ( 2 * packet_size ) / sms_data_part_max_len ) + 1 ); /* Full parts + 1 (last, incomplete). */
                    }
                    else if ( GSM_SMS_PDU_MODE == msg_fmt )
                    {
                        /* one extra byte (+ 1) is for WD_HL */
                        sms_data_part_max_len = GSM_SMS_MAX_LENGTH - ( pdu.header.wd_hl + 1 );

                        /* Count the number of messages. */
                        sms_count = ( 0 == ( ( packet_size ) % ( sms_data_part_max_len ) ) /* Full part detection */
                                        ? (  packet_size ) / ( sms_data_part_max_len ) /* Full parts */
                                        : ( ( packet_size ) / sms_data_part_max_len ) + 1 ); /* Full parts + 1 (last, incomplete). */
                    }

                    /* Datagram count. */
                    pdu.header.ie_ptr[0].ie_d.dtg.dtg_tc = sms_count;

                    /* Set the datagram's data. */
                    pdu.data_ptr = pkt_data;
                    pdu.data_max_len = sizeof pkt_data / sizeof pkt_data[0];
                    pdu.data_len = pkt_data_len;

                    PRINT_INFO( "Datagrams to sent ref=%d,data_size=%d,count=%d", sg_dtg_ref, pkt_data_len, sms_count );

            #ifndef NO_LIMIT_SENT_SMS
                    if ( sms_count > 2 )
                    {
                        sms_count = 2;

                        PRINT_DEBUG(
                                "LIMIT_SENT_SMS (SMS count limit to %d due to debug reasons)",
                                sms_count );
                    }
            #endif /* NO_LIMIT_SENT_SMS */

                    /* Divide data onto messages and send them. */
                    uint8_t next = 0U;

                    while ( next < sms_count )
                    {
                        uint8_t msg_size; /* Message's size. */
                        uint8_t msg_data[GSM_SMS_MAX_LENGTH / 2]; /* Buffer for a single message. */

                        /* Set the AR flag if confirmation is needed. */
                        if ( ( 0x0 != sg_wd_params.wdp_arp )
                                && ( ( next % sg_wd_params.wdp_arp ) || ( next == ( sms_count - 1 ) ) ) )
                        {
                            pdu.header.ie_ptr[SG_WD_IE_ID_MULTI_DTG].ie_d.dtg.dtg_ar = 1;
                        }

                        /* Serialize part of multidatagram. */
                        WD_status_t ser_status = sg_serialize_multidatagram(
                                &pdu, next + 1, msg_data, sms_data_part_max_len / 2, &msg_size );
                        if ( SG_WD_OK != ser_status )
                        {
                            PRINT_ERROR( "Data serialize error: %d", ser_status );
                        }
                        else /* SG_WD_OK */
                        {
                            const uint8_t sms_size = msg_size * 2; /* SMS size. */
                            GSM_status_t sms_sent_status;

                            PRINT_DEBUG( "Datagram's header: ref=%d,tc=%d,nr=%d,size=%d",
                                         pdu.header.ie_ptr[0].ie_d.dtg.dtg_ref,
                                         pdu.header.ie_ptr[0].ie_d.dtg.dtg_tc,
                                         pdu.header.ie_ptr[0].ie_d.dtg.dtg_nr,
                                         pdu.header.ie_ptr[1].ie_d.dd.dd_len );

                            if ( GSM_SMS_ASCII_MODE == msg_fmt )
                            {
                                char sms_data[GSM_SMS_MAX_LENGTH + 1]; /* Buffer for a single message. */

                                /* Convert data part of binary data to ASCII. */
                                str_bin_to_ascii( msg_data, sms_data, msg_size );

                                PRINT_DEBUG( "Datagram to sent: nr=%s,len=%d,text=%s",
                                             modem_sim_data.smsc, sms_size, sms_data );

                                /* Send the message in ASCII format. */
                                sms_sent_status = gsm_send_text( GSM_SMS_ASCII_MODE,
                                                                 (void*) 0UL,
                                                                 modem_sim_data.smsc,
                                                                 (const uint8_t*) sms_data,
                                                                 sms_size );
                            }
                            else if ( GSM_SMS_PDU_MODE == msg_fmt )
                            {
#ifdef DEBUG
                                char sms_data[GSM_SMS_MAX_LENGTH + 1]; /* Buffer for a single message. */

                                /* Convert data part of binary data to ASCII. */
                                str_bin_to_ascii( msg_data, sms_data, msg_size );

                                PRINT_DEBUG( "Datagram to sent: nr=%s,len=%d,text=%s",
                                             modem_sim_data.smsc, sms_size, sms_data );
#endif /* DEBUG */

                                /* Send the message in binary format. */
                                sms_sent_status = gsm_send_text( GSM_SMS_PDU_MODE,
                                                                 modem_sim_data.smsc,
                                                                 modem_sim_data.smsc,
                                                                 msg_data, msg_size );
                            }

                            if ( GSM_OK != sms_sent_status )
                            {
                                PRINT_ERROR( "Datagram sent error: %d", sms_sent_status );
                            }
                            else /* GSM_OK */
                            {
                                PRINT_DEBUG( "Datagram sent successfully" );

                                /* Update statistics. */
                                modem_usage_stats_data.sms_send++;
                                sent_stats_counter.sent_count++;
                                sent_stats_counter.sent_bytes += gsm_get_sent_bytes_count();

                                PRINT_INFO( "Sent statistics: count=%d,bytes=%d",
                                            sent_stats_counter.sent_count,
                                            sent_stats_counter.sent_bytes );

                                /* Wait for confirmation depending on scheme. */
                                uint16_t dtg_cfm_id = 0U; /* Id of the last continously received datagram. */
                                uint16_t cfm_res = wait_for_confirm_from_CS( &dtg_cfm_id );

                                /* Określ numer następnego datagramu do wysłania. */
                                if ( 0U == cfm_res )
                                {
                                    /* Check confirmation policy. */
                                    if ( 0U == sg_wd_params.wdp_arp )
                                    {
                                        /* Allow to send the next. */
                                        next++;
                                    }
                                    else /* Confirmation required. */
                                    {
                                        if ( 0U == dtg_cfm_id )
                                        {
                                            /* Not confirmed anyone. */
                                            next = 0U;
                                        }
                                        else if ( dtg_cfm_id == next )
                                        {
                                            /* Confirmed. Allow to sent next. */
                                            next++;
                                        }
                                        else
                                        {
                                            /* Confirmed but not the last sent. Back to previous. */
                                            next = dtg_cfm_id + 1;
                                        }
                                    }
                                }
                                else /* Errors in processing confirmation. */
                                {
                                    PRINT_ERROR( "Datagram confirmation error: %d", cfm_res );
                                }
                            }
                        }
                    }

                    PRINT_INFO( "Datagram sent statistics: dtg_ref=%d,dtg_total_cnt=%d,"
                                "total_bytes=%d,sent_cnt=%d,sent_bytes=%d",
                                pdu.header.ie_ptr[0].ie_d.dtg.dtg_ref,
                                pdu.header.ie_ptr[0].ie_d.dtg.dtg_tc, gm_data_len,
                                sent_stats_counter.sent_count,
                                sent_stats_counter.sent_bytes );

                    rc = SG_APP_OK;
                }
            }
            else if ( SG_LINK_GPRS_TCP == link_type )
            {
                bool tcp_rc;

                /* Get adress and port from object. */
                gprs_ipv4_to_ip_str( ( link_addr[0] << 24 ) | ( link_addr[1] << 16 ) | ( link_addr[2] << 8 ) | ( link_addr[3] ),
                                     sc_ip_addr, sizeof sc_ip_addr / sizeof sc_ip_addr[0] );

                sc_port = ( link_addr[4] << 8 ) | link_addr[5];

                /* Connect to SC server */
                tcp_rc = tcp_connect( sc_ip_addr, sc_port );

                if ( !tcp_rc )
                {
                    PRINT_ERROR( "Can't connect to %s at sc_port %d", sc_ip_addr, sc_port );
                }
                else
                {
                    /* XXX These data should be shared among SMS and packet send and receive part to save memory. */

                    char tcp_data[2 * SG_WP_DATA_MAX_LEN + 1]; /* Buffer for a single message. */

                    /* Convert data part of binary data to HEX-ASCII. */
                    str_bin_to_ascii( pkt_data, tcp_data, pkt_data_len );

                    /* Send in HEX-ASCII format. */
                    tcp_rc = tcp_send( tcp_data, pkt_data_len, 1 );

                    /* TODO Add checking and returning the size of sent data
                     * and resend packet if needed.
                     */
                    if ( !tcp_rc )
                    {
                        PRINT_ERROR( "Can't send data to %s at sc_port %d", sc_ip_addr, sc_port );
                    }
                    else
                    {
                        /* No response is required after successful sending. */
                    }

                    /* Connection will be closed before closing GPRS connection. */
                }
            }
        }
    }

    return rc;
}

/* If confirmation is required (cfm_mode != CONFIRM_NONE),
 * the funtion waits for a message from CS and then
 * decode id and returns 0 and the last continously
 * received datagram's id in \param cfm_dtg_id.
 * If no confirmation is required the function returns 0.
 */
static uint16_t wait_for_confirm_from_CS( uint16_t* cfm_dtg_id )
{
    if ( (void*) 0UL == cfm_dtg_id )
    {
        return 2U; /* Invalid input parameter. */
    }

    /* Required confirmation. */
    if ( 0x0 != sg_wd_params.wdp_arp )
    {
        uint32_t max_wait_time; /* Maximum waiting time for message from CS. UINT32_MAX means infinity. */

        /* Assign infinity if no time specified. */
        if ( 0UL == sg_wd_params.wdp_at )
        {
            max_wait_time = UINT32_MAX; /* infinity */
        }
        else /* Get relative time. */
        {
            max_wait_time = ( sg_wd_params.wdp_at * 1000 ) + hw_get_tick_time();
        }

        /* Wait for a message  specified period. */
        while ( max_wait_time >= hw_get_tick_time() )
        {
            /* Decode the message. */
            /* Return the message id. */
        }
    }

    return 0U;
}

static uint16_t send_confirm_to_CS( uint16_t max_count,
                                    uint16_t lcrd_id )
{
    /* Required confirmation. */
    if ( 0x0 != sg_wd_params.wdp_arp )
    {
        /* Send the datagram with empty data field. */
    }

    if ( !check_modem_smsc( modem_sim_data.smsc ))
    {
        return SG_APP_ERROR_MODEM_SMSC_IS_NOT_SET;
    }

    /* Encapsulate the data into a datagram. */

    /* Set the header with only DTG part and set the AR flag */
    SG_IE_t ie_table[] =
        { [SG_WD_IE_ID_MULTI_DTG] =
            { .ie_i = SG_WD_IE_ID_MULTI_DTG, .ie_l = 3U,
              .ie_d.dtg = { .dtg_ref = sg_dtg_ref, .dtg_ar = 1, .dtg_tc = max_count, .dtg_nr = lcrd_id } }
        };

    SG_WD_PDU_t pdu; /* Datagram unit. */

    pdu.header.ie_ptr = ie_table;
    pdu.header.ie_max_count = sizeof ( ie_table ) / sizeof(SG_IE_t);
    pdu.header.ie_count = pdu.header.ie_max_count;
    pdu.header.wd_hl = sizeof ( ie_table ); /* XXX Check the size. Shoule be 5. */

    PRINT_INFO( "Datagram confirmation to sent ref=%d,ar=%d,tc=%d,nr=%d", sg_dtg_ref,
                pdu.header.ie_ptr[SG_WD_IE_ID_MULTI_DTG].ie_d.dtg.dtg_ar,
                max_count,
                lcrd_id );

    uint8_t sms_data[GSM_SMS_MAX_LENGTH]; /* Buffer for a single message. */
    uint8_t sms_size = 0U; /* Message's size. */

    /* Serialize part of multidatagram. */
    WD_status_t ser_status = sg_serialize_multidatagram(
            &pdu, 1U, sms_data, sizeof sms_data, &sms_size );
    if ( SG_WD_OK != ser_status )
    {
        PRINT_ERROR( "Data serialize error: %d", ser_status );
    }
    else /* SG_WD_OK */
    {
        GSM_status_t sms_sent_status;

        PRINT_DEBUG( "Datagram's header: ref=%d,tc=%d,nr=%d,size=%d",
                     pdu.header.ie_ptr[SG_WD_IE_ID_MULTI_DTG].ie_d.dtg.dtg_ref,
                     pdu.header.ie_ptr[SG_WD_IE_ID_MULTI_DTG].ie_d.dtg.dtg_tc,
                     pdu.header.ie_ptr[SG_WD_IE_ID_MULTI_DTG].ie_d.dtg.dtg_nr,
                     sms_size );

#ifdef DEBUG
        char sms_data_dbg[2 * GSM_SMS_MAX_LENGTH + 1]; /* Buffer for a single message. */

        /* Convert data part of binary data to ASCII. */
        str_bin_to_ascii( sms_data, sms_data_dbg, sms_size );

        PRINT_DEBUG( "Datagram to sent: nr=%s,len=%d,text=%s",
                     modem_sim_data.smsc, sms_size, sms_data_dbg );
#endif /* DEBUG */

        /* Send the message in binary format. */
        sms_sent_status = gsm_send_text( GSM_SMS_PDU_MODE,
                                         modem_sim_data.smsc,
                                         modem_sim_data.smsc,
                                         sms_data, sms_size );

        if ( GSM_OK != sms_sent_status )
        {
            PRINT_ERROR( "Datagram sent error: %d", sms_sent_status );
        }
        else /* GSM_OK */
        {
            PRINT_DEBUG( "Datagram sent successfully" );

            /* Update statistics. */
            modem_usage_stats_data.sms_send++;

            PRINT_INFO( "Sent statistics: bytes=%u", (unsigned int) gsm_get_sent_bytes_count() );
        }
    }

    PRINT_INFO( "Datagram sent statistics: dtg_ref=%d,dtg_total_cnt=%d,"
                "total_bytes=%d,sent_bytes=%u",
                pdu.header.ie_ptr[0].ie_d.dtg.dtg_ref,
                pdu.header.ie_ptr[0].ie_d.dtg.dtg_tc, gm_data_len,
                (unsigned int) gsm_get_sent_bytes_count() );

    return 0U;
}

/*
 * NOTE Datagrams pointed by struct (list) \param dtgs have to be ordered.
 */
static uint8_t get_last_consistent_datagram( struct msg_part* dtgs, uint8_t dtg_count, const uint8_t dtg_cnt_limit )
{
    if ( ( (void*) 0UL == dtgs ) )
    {
        PRINT_ERROR( "Null input parameter" )

        return 0U;
    }

    if ( 0U == dtg_count )
    {
        return 0U;
    }

    for ( uint8_t i = 0; ( i < dtg_count - 1 ) && ( i < dtg_cnt_limit ); i++ )
    {
        if ( 1U == ( dtgs[i + 1].part - dtgs[i].part ) )
        {
            return dtgs[i].part;
        }
    }

    return dtgs[dtg_count - 1].part;
}

/*
 * Funkcja odczytuje wiadomości z modemu, sprawdza kompletność
 * pojedynczych wiadomości, jak i całego datagramu, następnie
 * umieszcza dane datagramu w sc_data.
 * UWAGA Wiadomości nie muszą przychodzić w uporządkowanej
 * kolejności dlatego konieczne jest ich uszeregowanie.
 */
static bool wait_for_data_from_CS()
{
    uint32_t max_wait_time; /* Maximum waiting time for message from CS. UINT32_MAX means infinity. */

    struct msg_part sg_msg[SC_SMS_DISPOSABLE_LIMIT]; /* Received messages. */

    uint8_t sg_msg_count = 0U; /* Received messages count. */

    struct recv_stats /* Receive statistics. */
    {
        uint8_t recv_count;
        uint16_t recv_bytes;
    } recv_stats_counter = { 0U, 0U };

    SG_DTG_t first_dtg_ie; /* Header for the first message. */

    /* Clear the SC global data length. */
    sc_data_len = 0U;

    /* Delete all read messages. */
    gsm_delete_texts();

    /* Assign infinity if no time specified. */
    if ( 0UL == wait_for_cs_time_limit )
    {
        max_wait_time = UINT32_MAX; /* infinity */
    }
    else /* Get relative time. */
    {
        max_wait_time = ( wait_for_cs_time_limit * 1000 ) + hw_get_tick_time();
    }

    /* Casting to ulong is due to imcompatibility between gcc and cppcheck in uint32_t handling. */
    PRINT_DEBUG( "Waiting time for SC data is set to %lu ms",
                 (unsigned long ) max_wait_time );

    /* Wait for specified period. */
    while ( max_wait_time >= hw_get_tick_time() )
    {
        if ( SG_LINK_SMS == link_type )
        {
            GSM_status_t sms_recv_status; /* Receive status. */

            /* TODO Change SMS format to binary (PDU). */
            if ( GSM_SMS_ASCII_MODE == msg_fmt )
            {
                /* Get a text message. */
                sms_recv_status = gsm_receive_texts( GSM_SMS_ASCII_MODE );
            }
            else if ( GSM_SMS_PDU_MODE == msg_fmt )
            {
                /* Get a text message. */
                sms_recv_status = gsm_receive_texts( GSM_SMS_PDU_MODE );
            }

            if ( GSM_ERROR_NO_MESSAGES == sms_recv_status )
            {
                PRINT_DEBUG( "No requests from CS" );
            }
            else if ( GSM_OK != sms_recv_status )
            {
                PRINT_DEBUG( "Datagram receive error: %d", sms_recv_status );
            }
            else /* GSM_OK */
            {
                /* Convert messages to datagrams. */
                for ( uint8_t i = 0; i < sc_sms_list.count; i++ )
                {
                    struct sms *sms = (void*) 0UL; /* Pointer to single message. */

                    uint8_t msg_size = 0U; /* Message's size. */
                    uint8_t msg_data[GSM_SMS_MAX_LENGTH / 2]; /* Buffer for a single message. */

                    SG_WD_PDU_t pdu; /* Single datagram. */
                    SG_IE_t ie_table[] = { { 0 }, { 0 } }; /* Header template. */
                    uint8_t data[SG_WD_MAX_DATA_LEN]; /* Received data table. */

                    /* Set the header for received datagram. */
                    pdu.header.ie_ptr = ie_table;
                    pdu.header.ie_max_count = sizeof ( ie_table ) / sizeof(SG_IE_t);
                    pdu.header.ie_count = 0U;

                    /* Set the common datagram's data. */
                    pdu.data_ptr = data;
                    pdu.data_len = 0U;
                    pdu.data_max_len = sizeof data / sizeof data[0];

                    /* Get the next message. */
                    sms = &sc_sms_list.sms[i];

                    /* Assert message. */
                    assert( ( (void* ) 0UL ) != sms );

                    if ( GSM_SMS_ASCII_MODE == msg_fmt )
                    {
                        PRINT_DEBUG(
                                "Message received: nr=%d,phone=%s,time=%s,content=%s",
                                sms->id, sms->number, sms->time, sms->content );

                        /* Convert data part from ASCII to binary data. */
                        str_ascii_to_bin( sms->content, msg_data,
                                          strlen( sms->content ) );
                        msg_size = strlen( sms->content ) / 2;
                    }
                    else if ( GSM_SMS_PDU_MODE == msg_fmt )
                    {
                        PRINT_DEBUG(
                                "Message received: nr=%d,phone=%s,time=%s,content=%s",
                                sms->id, sms->number, sms->time, sms->content );
                    }

                    /* Deserialize a datagram. */
                    WD_status_t deser_status = sg_deserialize_datagram( msg_data,
                                                                        msg_size,
                                                                        &pdu );

                    if ( SG_WD_OK != deser_status )
                    {
                        PRINT_ERROR( "Datagram deserialization error (%d)",
                                     deser_status );
                    }
                    else /* Deserialization ok. */
                    {
                        PRINT_DEBUG(
                                "Datagram's header: ref=%d,tc=%d,nr=%d,size=%d,crc=%02X",
                                pdu.header.ie_ptr[0].ie_d.dtg.dtg_ref,
                                pdu.header.ie_ptr[0].ie_d.dtg.dtg_tc,
                                pdu.header.ie_ptr[0].ie_d.dtg.dtg_nr,
                                pdu.header.ie_ptr[1].ie_d.dd.dd_len,
                                pdu.header.ie_ptr[1].ie_d.dd.dd_crc );

                        /* Check the length of data. */
                        if ( pdu.data_len != pdu.header.ie_ptr[1].ie_d.dd.dd_len )
                        {
                            PRINT_ERROR( "Datagram data length error (%d)",
                                         pdu.data_len );
                        }
                        else /* The valid length. */
                        {
                            /* Validate the data based on CRC. */
                            const uint16_t crc = calculate_crc16_ccitt_false(
                                    pdu.data_ptr, pdu.data_len );

                            if ( crc != pdu.header.ie_ptr[1].ie_d.dd.dd_crc )
                            {
                                PRINT_ERROR( "Datagram crc error (%hu)", crc );
                            }
                            else /* crc ok */
                            {
                                /* Copy header from the first. */
                                if ( 1U == pdu.header.ie_ptr[0].ie_d.dtg.dtg_nr )
                                {
                                    /* Copy datagram header. */
                                    first_dtg_ie.dtg_ref = pdu.header.ie_ptr[0].ie_d.dtg.dtg_ref;
                                    first_dtg_ie.dtg_tc = pdu.header.ie_ptr[0].ie_d.dtg.dtg_tc;
                                    first_dtg_ie.dtg_nr = 1U;
                                    first_dtg_ie.dtg_ar = pdu.header.ie_ptr[0].ie_d.dtg.dtg_ar;
                                }

                                /* Copy the message to the input buffer. */
                                memcpy( &sg_msg[sg_msg_count].data, pdu.data_ptr,
                                        pdu.data_len );
                                sg_msg[sg_msg_count].len = pdu.data_len;
                                sg_msg[sg_msg_count].refno = pdu.header.ie_ptr[0]
                                        .ie_d.dtg.dtg_ref;
                                sg_msg[sg_msg_count].part = pdu.header.ie_ptr[0]
                                        .ie_d.dtg.dtg_nr;
                                sg_msg_count++;

                                /* Delete the origin message. */
                                gsm_delete_text( sms->id );
                            }
                        }
                    }
                }

                /* Update statistics. */
                modem_usage_stats_data.sms_recv += sg_msg_count;
                recv_stats_counter.recv_count++;
                recv_stats_counter.recv_bytes += gsm_get_received_bytes_count();

                PRINT_INFO( "Receive statistics: count=%d,bytes=%d",
                            recv_stats_counter.recv_count,
                            recv_stats_counter.recv_bytes );

                /* Analyze messages. */
                if ( 0U == sg_msg_count )
                {
                    /* Do nothing. */
                }
                else /* sg_msg_count > 0 */
                {
                    bool is_complete = false; /* Complete datagram. */
                    uint8_t dtg_part_count = 0; /* Part count. */

                    /* Check for all parts. */
                    for ( uint8_t i = 0; i < sg_msg_count; i++ )
                    {
                        if ( first_dtg_ie.dtg_ref == sg_msg[i].refno )
                        {
                            dtg_part_count++;
                        }
                    }

                    if ( dtg_part_count == first_dtg_ie.dtg_tc )
                    {
                        is_complete = true;
                    }

                    /* Check if datagram is complete. */
                    if ( !is_complete ) /* datagram not complete */
                    {
                        /* Check consistent datagram's string. */
                        uint8_t last_consist_dtg; /* Last consistent datagram. */

                        last_consist_dtg = get_last_consistent_datagram( sg_msg, sg_msg_count, SC_SMS_DISPOSABLE_LIMIT );

                        /* Send the confirmation to CS. */
                        send_confirm_to_CS( first_dtg_ie.dtg_tc, last_consist_dtg );

                        /* Wait for the next message. */
                        gsm_wait_for_new_message();
                    }
                    else /* Datagram complete. */
                    {
                        /* Order the data */
                        int8_t prev_msg = -1; /* The first or previous message. */

                        /* Find the first message. */
                        for ( int8_t i = 0; i < sg_msg_count; i++ )
                        {
                            if ( ( sg_msg[i].refno == first_dtg_ie.dtg_ref )
                                    && ( sg_msg[i].part <= first_dtg_ie.dtg_tc ) )
                            {
                                /* Set the first message. */
                                prev_msg = i;
                                break;
                            }
                        }

                        /* Filter and sort the data. */
                        for ( int8_t i = 0; i < sg_msg_count; i++ )
                        {
                            if ( ( first_dtg_ie.dtg_ref == sg_msg[i].refno )
                                    && ( sg_msg[i].part <= first_dtg_ie.dtg_tc ) )
                            {
                                if ( ( -1 != prev_msg ) && ( i != prev_msg ) )
                                {
                                    /* Check for previous (older) message. */
                                    if ( sg_msg[i].part < sg_msg[prev_msg].part )
                                    {
                                        struct msg_part m; /* Temporary message. */
                                        /* Copy the previous to temporary. */
                                        m.refno = sg_msg[prev_msg].refno;
                                        m.part = sg_msg[prev_msg].part;
                                        m.len = sg_msg[prev_msg].len;
                                        memcpy( m.data, sg_msg[prev_msg].data,
                                                m.len );
                                        /* Copy the current to previous. */
                                        sg_msg[prev_msg].refno = sg_msg[i].refno;
                                        sg_msg[prev_msg].part = sg_msg[i].part;
                                        sg_msg[prev_msg].len = sg_msg[i].len;
                                        memcpy( sg_msg[prev_msg].data,
                                                sg_msg[i].data, sg_msg[i].len );
                                        /* Copy the temporary to current. */
                                        sg_msg[i].refno = m.refno;
                                        sg_msg[i].part = m.part;
                                        sg_msg[i].len = m.len;
                                        memcpy( sg_msg[i].data, m.data, m.len );
                                        /* Set the new previous index. */
                                        prev_msg = i;
                                    }
                                }
                            }
                        }

                        /* Copy the ordered data. */
                        for ( uint8_t i = 0; i < sg_msg_count; i++ )
                        {
                            if ( first_dtg_ie.dtg_ref == sg_msg[i].refno )
                            {
                                /* Copy the datagram's data to general SC data. */
                                memcpy( &sc_data[sc_data_len], sg_msg[i].data,
                                        sg_msg[i].len );
                                sc_data_len += sg_msg[i].len;
                            }
                        }

    #ifdef DEBUG
                        str_bin_to_ascii( sc_data, data_dbg_str, sc_data_len );
                        data_dbg_str[2 * sc_data_len] = '\0';

                        PRINT_DEBUG( "SC data: len=%d,data=\"%s\"", sc_data_len,
                                     data_dbg_str );
    #endif
                        /* There aren't more messages. Go to process. */
                        return true;
                    }
                }
            } /* GSM_OK */
        }
        else if ( SG_LINK_GPRS_TCP == link_type )
        {
            bool tcp_rc;

            /* XXX These data should be shared among SMS and packet send and receive part to save memory. */

            char tcp_data[2 * SG_WP_DATA_MAX_LEN + 1]; /* Buffer for a single message. */
            uint16_t tcp_data_len;

            tcp_rc = tcp_recv( tcp_data, sizeof tcp_data / sizeof tcp_data[0], &tcp_data_len );

            if ( !tcp_rc )
            {
                PRINT_ERROR( "Can't recv data from %s at sc_port %d", sc_ip_addr, sc_port );
            }
            else
            {
                SG_WP_PDU_t pkt; /* Data packet. */
                uint16_t ser_rc; /* Serialize result. */

                /* The data for packet. */
                uint8_t pkt_data[SG_WP_DATA_MAX_LEN];
                uint16_t pkt_data_len; /* Serialize result. */

                /* Assign data to buffer for pakcet. */
                pkt.data_ptr = sc_data;

                PRINT_DEBUG( "Recv data from %s at sc_port %d: len=%d,data=\"%s\"",
                             sc_ip_addr, sc_port, tcp_data_len, tcp_data );

                /* Convert data part of binary data to HEX-ASCII. */
                str_ascii_to_bin( tcp_data, pkt_data, tcp_data_len );
                pkt_data_len = tcp_data_len / 2;

                /* Clear the data. */
                memset( pkt_data, 0U, sizeof pkt_data / sizeof pkt_data[0] );

                /* Serialize the packet to data table. */
                ser_rc = deserialize_packet( pkt_data, pkt_data_len, &pkt );

                if ( SG_WP_OK != ser_rc )
                {
                    PRINT_ERROR( "Error (%u) in deserialization the packet", ser_rc );

                    //rc = SG_APP_ERROR_PACKET_SERIALIZATION;
                }
                else
                {
                    /* Check CRC. */
                    uint16_t crc = calculate_crc16_ccitt_false( pkt_data, pkt_data_len );
                    if ( crc == pkt.crc )
                    {
                        PRINT_ERROR( "Error in CRC (packet=%u, computed=%d)", pkt.crc, crc );
                    }
                    else
                    {
                        sc_data_len = pkt.data_len;

#ifdef DEBUG
                        str_bin_to_ascii( sc_data, data_dbg_str, sc_data_len );
                        data_dbg_str[2 * sc_data_len] = '\0';

                        PRINT_DEBUG( "SC data: len=%d,data=\"%s\"", sc_data_len,
                                 data_dbg_str );
#endif
                        /* There aren't more messages. Go to process. */
                        return true;
                    }
                }
            }
        }
    } /* time loop */

    return false;
}

static void process_data_from_CS( void )
{
    GM_frame_t gm_req = { 0U }; /* Request to GM. */
    GM_frame_t gm_resp = { 0U }; /* Response from GM. */
    SG_WO_OBJ_t objs[SC_REQUESTED_OBJECTS_LIMIT]; /* Object table. */
    SG_WO_PDU_t obj_list; /* object list. */
    SG_OBJ_OP_STATUS_t status; /* The operation status on object. */

    /* Check for data input. */
    if ( 0U == sc_data_len )
    {
        PRINT_ERROR( "No data from SC" );

        return;
    }

    /* Preapre the list of objects. */
    obj_list.wo_data_ptr = &objs[0];
    obj_list.wo_data_max_count = sizeof objs / sizeof objs[0];
    obj_list.wo_data_count = 0U;

    /* Deserialize the data stream from SC. */
    status = sg_deserialize_obj_list( WO_REQ_DIR_FROM_SC_TO_GM, sc_data,
                                      sc_data_len, &obj_list );

    if ( SG_OBJ_OP_OK != status )
    {
        PRINT_ERROR( "Error %d while deserialization of the object list.",
                     status );

        /* TODO Co z tym zrobić?
         * 1. Liczbę obiektów ograniczono do wo_data_max_count
         * 2. W przypadku wystąpienia błędu konieczna jest odpowiedź do nadawcy.
         */
    }

    /* Clear the data tables and size for GM data. */
    memset( gm_data, 0U, sizeof gm_data / sizeof gm_data[0] );
    gm_data_len = 0U;

    /* Current implementation just ask about required data from GM. */
    gm_req.func = GM_RADIO_DATA_FUNC;
    gm_req.options.sender = 0x1U;
    gm_req.options.encypt = 0x0U;
    gm_req.options.reserved = 0x0U;
    gm_req.options.proto_ver = (uint8_t) GM_PROTOCOL_VERSION;
    gm_req.data_len = 0U;

    /* Send each object to GM. */
    for ( int8_t i = 0; i < obj_list.wo_data_count; i++ )
    {
        SG_WO_OBJ_t *obj = &obj_list.wo_data_ptr[i]; /* Object from the list. */

        /* Check if valid object. */
        if ( ( ( (void*) 0UL ) == obj ) || ( 0U == obj->valid ) )
        {
            char obj_str[256];

            /* Print an invalid object. */
            sg_obj_to_str( obj, obj_str, sizeof obj_str );

            PRINT_ERROR( "No object or object invalid: %s", obj_str );

            /* Skip to the next. */
            continue;
        }

        /* Serialize object from the list. */
        status = sg_serialize_obj(
                WO_REQ_DIR_FROM_SC_TO_GM, obj, gm_req.data,
                sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                &gm_req.data_len );

        if ( SG_OBJ_OP_OK == status )
        {
            GM_status_t gm_status; /* GM communication error code. */

            /* Send the frame about data. */
            gm_status = gm_send_frame( &gm_req );

            if ( GM_OK != gm_status )
            {
                PRINT_ERROR( "Error %d at sending a request to GM", gm_status );
            }
            else /* GM_OK */
            {
                /* Get a data from GM. */
                gm_status = gm_receive_frame( &gm_resp );

                if ( GM_OK != gm_status )
                {
                    PRINT_ERROR( "Error %d at receiving a response from GM",
                                 gm_status );
                }
                else /* GM_OK */
                {
                    /* Clean object. */
                    uint16_t obj_size = 0U;
                    memset( (void*) obj, 0U, sizeof ( obj ) );

                    /* Deserialize an object from a response. */
                    status = sg_deserialize_obj( WO_REQ_DIR_FROM_GM_TO_SC,
                                                 gm_resp.data, gm_resp.data_len,
                                                 obj, &obj_size );

                    if ( SG_OBJ_OP_OK == status )
                    {
                        /* Get the important data for the module. */
                        update_local_data( obj );

                        /* Copy an output data to the global data table. */
                        memcpy( &gm_data[gm_data_len], gm_resp.data,
                                gm_resp.data_len );
                        gm_data_len += gm_resp.data_len;
                    }
                    else /* Bad deserialization. */
                    {
                        PRINT_ERROR( "Deserialize error %d", status );
                    }
                }
            }
        }
        else /* Bad serialization. */
        {
            PRINT_ERROR( "Serialize error %d", status );
        }
    }

#ifdef DEBUG
    str_bin_to_ascii( gm_data, data_dbg_str, gm_data_len );
    data_dbg_str[2 * gm_data_len] = '\0';

    PRINT_DEBUG( "GM data: len=%d,data=\"%s\"", gm_data_len, data_dbg_str );
#endif /* DEBUG */

    /* Send the response to CS. */
    send_data_to_CS();
}

static void store_local_data( void )
{
    /* Save the datagram reference's number. */
    hw_write_byte_data_eeprom( DATAGRAM_REF_NUM_BYTE_FLASH_DATA, sg_dtg_ref );

    PRINT_INFO( "Store local data: datagram reference counter: %u", sg_dtg_ref );

    /* Save the last working time in seconds. */
    total_up_time += ( hw_get_tick_time() / 1000U ) + 1; /* +1 s for time to go in standby. */
    hw_write_word_data_eeprom( WORKING_TIME_WORD_FLASH_DATA, total_up_time );

    PRINT_INFO( "Store local data: total up time counter: %lu",
                (unsigned long ) total_up_time );

    /* Save the wait time for SC. */
    hw_write_short_data_eeprom( WAIT_FOR_SC_TIME_SHORT_FLASH_DATA,
                                wait_for_cs_time_limit );

    PRINT_INFO( "Store local data: wait for SC time: %u",
                wait_for_cs_time_limit );
}

static void send_GM_usage_statistics( void )
{
    GM_frame_t gm_req = { 0 }; /* Request frame with 0 data. */
    GM_frame_t gm_resp = { 0 }; /* Request frame with 0 data. */
    /* List of object id to be passed. */
    const SG_OBJ_ID_t req_obj_ids[] = {
          SG_OBJ_ID_MODEM_SMS_RECEIVED, SG_OBJ_ID_MODEM_SMS_SENT,
          SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED,
          SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT, SG_OBJ_ID_MODEM_WAKEUP_COUNT,
          SG_OBJ_ID_MODEM_WAKEUP_DURATION };

    SG_WO_OBJ_t obj; /* SG object. */

    /* Basic frame's settings. */
    gm_req.func = GM_RADIO_DATA_FUNC;
    gm_req.options.sender = 1;
    gm_req.options.encypt = 0;
    gm_req.options.reserved = 0;
    gm_req.options.proto_ver = GM_PROTOCOL_VERSION;
    gm_req.data_len = 0;

    for ( uint8_t i = 0; i < sizeof req_obj_ids / sizeof(SG_OBJ_ID_t); i++ )
    {
        if ( MKG_OK == sg_get_mkg_object( req_obj_ids[i], &obj ) )
        {
            if ( SG_OBJ_OP_OK == sg_serialize_obj(
                    WO_REQ_DIR_FROM_MKG_TO_GM, &obj, gm_req.data,
                    sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                    &gm_req.data_len ) )
            {
                /* Send prepared data. */
                if ( GM_OK == gm_send_frame( &gm_req ) )
                {
                    /* Get confirmation. */
                    if ( GM_OK == gm_receive_frame( &gm_resp ) )
                    {
                        /* TODO Add a loop to re-send data in the case of error. */

                        PRINT_INFO( "Sent usage statistics %s",
                                    sg_get_obj_id_name( req_obj_ids[i] ) );
                    }
                }
                else
                {
                    /* TODO Set a global status error code. */
                }
            }
        }
    }
}

static void send_GM_bye( void )
{
    GM_frame_t gm_req = { 0 };
    SG_WO_OBJ_t obj; /* SG object. */

    /* Basic frame's settings. */
    gm_req.func = GM_RADIO_DATA_FUNC;
    gm_req.options.sender = 1;
    gm_req.options.encypt = 0;
    gm_req.options.reserved = 0;
    gm_req.options.proto_ver = GM_PROTOCOL_VERSION;
    gm_req.data_len = 0;

    /* Get the modem status. */
    if ( MKG_OK == sg_get_mkg_object( MKG_SG_OBJ_BYE, &obj ) )
    {
        /* Build a PDU frame from objects. */
        if ( SG_OBJ_OP_OK == sg_serialize_obj(
                WO_REQ_DIR_FROM_MKG_TO_GM, &obj, gm_req.data,
                sizeof ( gm_req.data ) / sizeof ( gm_req.data[0] ),
                &gm_req.data_len ) )
        {
            /* Send prepared data. */
            if ( GM_OK == gm_send_frame( &gm_req ) )
            {

            }
            else
            {
                /* TODO Set a global status error code. */
            }
        }
    }
}

static void wait_for_power_off( void )
{
    PRINT_INFO( "Going in standby. Ready for power off" );

#ifndef DEBUG
    hw_enter_standby();
#else
    while ( 1 )
    {
    };
#endif
}

/*** end of file ***/

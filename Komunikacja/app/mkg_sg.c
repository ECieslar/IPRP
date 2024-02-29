/*!****************************************************************************
 *
 * \file mkg_sg.c
 * \brief The MKG module's SMART-GAS extension.
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
#include <stdio.h>
#include <string.h>

#include "mkg_sg.h"
#include "mkg_sg_app.h"
#include "../gsm/gsm_comm.h"
#include "../hw/mkg_hw.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* The MKG data supported in SMART-GAS. */
struct modem_mfc_data_ modem_mfc_data =
    {
        { '1', 'G', 'K', 'M', }, /* ASCII - reverse order */
          { 20, MKG_HW_ID_REV_YEAR, MKG_HW_ID_REV_MONTH, MKG_HW_ID_REV_DAY, },
          { MKG_SW_ID_VER_MAJ, MKG_SW_ID_VER_MIN, MKG_SW_ID_VER_REL,
            MKG_SW_ID_VER_BLD & 0xFF },
          { 0xFF, 0xFF, 0xFF, 0xFF },
          { '\0', 'E', 'Y', 'B' } /* ASCII - reverse order */
    };

/* The SIM data required by SMART-GAS objects. */
struct modem_sim_data_ modem_sim_data;

/* The event data required by SMART-GAS objects. */
struct modem_event_data_ modem_event_data;

/* The statistical data required by SMART-GAS objects. */
struct modem_usage_stats_data_ modem_usage_stats_data =
    { 0, 0, 0, 0, 0, 0 };

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/

MKG_status_t sg_get_mkg_object( const uint16_t obj_id, SG_WO_OBJ_t *obj )
{
    if ( ( (void*) 0UL ) == obj )
    {
        return MKG_INVALID_INPUT_PARAMETER_ERROR;
    }

    obj->id = obj_id;
    obj->err = SG_OBJ_OK;
    obj->tskt = SG_OBJ_TSKT_PUT; /* Getting an object means putting it into an output stream, so PUT is set. */

    switch ( obj_id )
    {
        /* MKG specific objects. */

        case MKG_SG_OBJ_MOD_ID: /* 4B */
            memcpy( obj->data, modem_mfc_data.id,
                    sizeof ( modem_mfc_data.id ) );
            break;

        case MKG_SG_OBJ_HW_VER: /* 4B */
            memcpy( obj->data, modem_mfc_data.hw_ver,
                    sizeof ( modem_mfc_data.hw_ver ) );
            break;

        case MKG_SG_OBJ_SW_VER: /* 4B */
            memcpy( obj->data, modem_mfc_data.sw_ver,
                    sizeof ( modem_mfc_data.sw_ver ) );
            break;

        case MKG_SG_OBJ_SN: /* 4B */
            memcpy( obj->data, modem_mfc_data.sn,
                    sizeof ( modem_mfc_data.sn ) );
            break;

        case MKG_SG_OBJ_BYE: /* 4B */
            memcpy( obj->data, modem_mfc_data.bye,
                    sizeof ( modem_mfc_data.bye ) );
            break;

        case MKG_SG_OBJ_GSM_CLOCK: /* 7B */
            /* Get last accuired date&time from GSM. */
            gsm_get_network_time();

            obj->data[0] = gsm_modem_info.date_time.year;
            obj->data[1] = gsm_modem_info.date_time.month;
            obj->data[2] = gsm_modem_info.date_time.day;
            obj->data[3] = gsm_modem_info.date_time.hour;
            obj->data[4] = gsm_modem_info.date_time.min;
            obj->data[5] = gsm_modem_info.date_time.sec;
            obj->data[6] = gsm_modem_info.date_time.tz;
            obj->data_len = 7;
            break;

        case MKG_SG_OBJ_SC_TIMEOUT: /* 1B */
            obj->data[0] = wait_for_cs_time_limit;
            break;

            /* SMART-GAS v2018 - Modem (6.7) */

        case SG_OBJ_ID_SIM_PIN: /* ASCII (max. 8B)  */
            memcpy( obj->data, modem_sim_data.pin,
                    sizeof ( modem_sim_data.pin ) - 1 );
            obj->data_len = strlen( modem_sim_data.pin );
            break;

        case SG_OBJ_ID_SIM_ICCID: /* BCD (BE z uzupełnieniem do 0xF) */
            memcpy( obj->data, modem_sim_data.iccid,
                    sizeof ( modem_sim_data.iccid ) );
            break;

        case SG_OBJ_ID_SIM_PHONE: /* BCD (BE z uzupełnieniem do 0xF) */
            memcpy( obj->data, modem_sim_data.phone,
                    sizeof ( modem_sim_data.phone ) );
            break;

        case SG_OBJ_ID_SIM_IP: /* 4B */
            obj->data[0] = modem_sim_data.ip >> 24;
            obj->data[1] = ( modem_sim_data.ip >> 16 ) & 0xFF;
            obj->data[2] = ( modem_sim_data.ip >> 8 ) & 0xFF;
            obj->data[3] = ( modem_sim_data.ip ) & 0xFF;
            break;

        case SG_OBJ_ID_SIM_SMSC: /* ASCII (max. 16B) */
            memcpy( obj->data, modem_sim_data.smsc,
                    sizeof ( modem_sim_data.smsc ) - 1 );
            obj->data_len = strlen( modem_sim_data.smsc );
            break;

        case SG_OBJ_ID_SIM_SMS_VALIDITY: /* 1B */
            obj->data[0] = modem_sim_data.sms_validity;
            break;

        case SG_OBJ_ID_GPRS_APN: /* ASCII (max. 32B) */
            memcpy( obj->data, modem_sim_data.gprs_apn,
                    sizeof ( modem_sim_data.gprs_apn ) - 1 );
            obj->data_len = strlen( modem_sim_data.gprs_apn );
            break;

        case SG_OBJ_ID_GPRS_USER: /* ASCII (max. 16B)  */
            memcpy( obj->data, modem_sim_data.gprs_user,
                    sizeof ( modem_sim_data.gprs_user ) - 1 );
            obj->data_len = strlen( modem_sim_data.gprs_user );
            break;

        case SG_OBJ_ID_GPRS_PASSWORD: /* ASCII (max. 16B) */
            memcpy( obj->data, modem_sim_data.gprs_password,
                    sizeof ( modem_sim_data.gprs_password ) - 1 );
            obj->data_len = strlen( modem_sim_data.gprs_password );
            break;

        case SG_OBJ_ID_GSM_QUALITY:
            obj->data[0] = gsm_modem_info.quality;
            break;

        case SG_OBJ_ID_GSM_NETWORK_CODE:
            sprintf( (char*) obj->data, "%3d%02d", gsm_modem_info.loc.MCC,
                     gsm_modem_info.loc.MNC );
            obj->data_len = 5;
            break;

        case SG_OBJ_ID_GSM_BTS_LAC: /* 4B */
            obj->data[0] = 0;
            obj->data[1] = 0;
            obj->data[2] = gsm_modem_info.loc.LAC >> 8;
            obj->data[3] = gsm_modem_info.loc.LAC & 0xF;
            break;

        case SG_OBJ_ID_GSM_BTS_CELL_ID: /* 4B */
            obj->data[0] = 0;
            obj->data[1] = 0;
            obj->data[2] = gsm_modem_info.loc.CID >> 8;
            obj->data[3] = gsm_modem_info.loc.CID & 0xF;
            break;

        case SG_OBJ_ID_MODEM_STATE:
            obj->data[0] = 0;
            if ( GSM_STATUS_CONNECTED == gsm_modem_info.conn )
            {
                obj->data[0] |= 0x80;
            }
            /* TODO Add more flags. */
            break;

        case SG_OBJ_ID_MODEM_SMS_RECEIVED: /* 2B */
            obj->data[0] = modem_usage_stats_data.sms_recv >> 8;
            obj->data[1] = modem_usage_stats_data.sms_recv & 0xFF;
            break;

        case SG_OBJ_ID_MODEM_SMS_SENT: /* 2B */
            obj->data[0] = modem_usage_stats_data.sms_send >> 8;
            obj->data[1] = modem_usage_stats_data.sms_send & 0xFF;
            break;

        case SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED: /* 2B */
            obj->data[0] = modem_usage_stats_data.gprs_recv >> 8;
            obj->data[1] = modem_usage_stats_data.gprs_recv & 0xFF;
            break;
        case SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT: /* 2B */
            obj->data[0] = modem_usage_stats_data.gprs_send >> 8;
            obj->data[1] = modem_usage_stats_data.gprs_send & 0xFF;
            break;
        case SG_OBJ_ID_MODEM_WAKEUP_COUNT: /* 2B */
            obj->data[0] = modem_usage_stats_data.wakeup_count >> 8;
            obj->data[1] = modem_usage_stats_data.wakeup_count & 0xFF;
            break;
        case SG_OBJ_ID_MODEM_WAKEUP_DURATION: /* 4B */
            obj->data[0] = modem_usage_stats_data.wakeup_duration >> 24;
            obj->data[1] = modem_usage_stats_data.wakeup_duration >> 16;
            obj->data[2] = modem_usage_stats_data.wakeup_duration >> 8;
            obj->data[3] = modem_usage_stats_data.wakeup_duration & 0xFF;
            break;

            /* SMART-GAS v2018 - Konfiguracja zdarzeń (6.8) */
        case SG_OBJ_ID_EVENT_LINK: /* 1B */
            obj->data[0] = modem_event_data.link;
            break;

        case SG_OBJ_ID_EVENT_PHONE: /* 8B */
            memcpy( obj->data, modem_event_data.phone,
                    sizeof ( modem_event_data.phone ) );
            obj->data_len = sizeof ( modem_event_data.phone );
            break;

        case SG_OBJ_ID_EVENT_IP: /* 4B */
            obj->data[0] = modem_event_data.ip >> 24;
            obj->data[1] = modem_event_data.ip >> 16;
            obj->data[2] = modem_event_data.ip >> 8;
            obj->data[3] = modem_event_data.ip & 0xFF;
            break;

        case SG_OBJ_ID_EVENT_UDP_PORT: /* 2B */
            obj->data[0] = modem_event_data.udp_port >> 8;
            obj->data[1] = modem_event_data.udp_port & 0xFF;
            break;

        case SG_OBJ_ID_EVENT_TCP_PORT: /* 2B */
            obj->data[0] = modem_event_data.udp_port >> 8;
            obj->data[1] = modem_event_data.udp_port & 0xFF;
            break;

        default:
            return MKG_UNSUPPORTED_OBJ_ID_ERROR;
    }

    /* Set the data complete flag. */
    obj->valid = 1;

    return MKG_OK;
}

MKG_status_t sg_set_mkg_object( SG_WO_OBJ_t *obj )
{
    if ( ( (void*) 0UL ) == obj )
    {
        return MKG_INVALID_INPUT_PARAMETER_ERROR;
    }

    /* Setting an object means getting it from an input stream, so GET is required. */
    if ( ( SG_OBJ_OK == obj->err ) && ( SG_OBJ_TSKT_GET == obj->tskt )
         && ( 1U == obj->valid ) )
    {
        switch ( (uint16_t) obj->id )
        /* Convert to uint16_t to unify MKG and SKG ids. */
        {
            /* MKG specific objects. */
            case MKG_SG_OBJ_MOD_ID:
            case MKG_SG_OBJ_HW_VER:
            case MKG_SG_OBJ_SW_VER:
            case MKG_SG_OBJ_BYE:
            case MKG_SG_OBJ_GSM_CLOCK:
                return MKG_WRITE_DENIED_ERROR;

            case MKG_SG_OBJ_SC_TIMEOUT: /* 1B */
                wait_for_cs_time_limit = obj->data[0];
                break;

            case SG_OBJ_ID_GSM_QUALITY:
            case SG_OBJ_ID_GSM_NETWORK_CODE:
            case SG_OBJ_ID_GSM_BTS_LAC:
            case SG_OBJ_ID_GSM_BTS_CELL_ID:
            case SG_OBJ_ID_MODEM_STATE:
            case SG_OBJ_ID_MODEM_SMS_RECEIVED:
            case SG_OBJ_ID_MODEM_SMS_SENT:
            case SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED:
            case SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT:
            case SG_OBJ_ID_MODEM_WAKEUP_COUNT:
            case SG_OBJ_ID_MODEM_WAKEUP_DURATION:
                return MKG_WRITE_DENIED_ERROR;

            case MKG_SG_OBJ_SN:
                /* TODO Add support for the rights to write. */
                return MKG_INSUFFICIENT_RIGHTS;
                break;

                /* SMART-GAS v2018 - Modem (6.7) */
            case SG_OBJ_ID_SIM_SMSC: /* ASCII (max. 16B) */
                if ( 16U >= obj->data_len )
                {
                    memcpy( modem_sim_data.smsc, obj->data, obj->data_len );
                }
                else
                {
                    return MKG_TOO_LARGE_OBJECT_DATA;
                }
                break;

            case SG_OBJ_ID_SIM_SMS_VALIDITY:
                modem_sim_data.sms_validity = obj->data[0];
                break;

            case SG_OBJ_ID_GPRS_APN: /* ASCII (max. 32B) */
            case SG_OBJ_ID_GPRS_USER: /* ASCII (max. 16B)  */
            case SG_OBJ_ID_GPRS_PASSWORD: /* ASCII (max. 16B) */

                /* SMART-GAS v2018 - Konfiguracja zdarzeń (6.8) */
            case SG_OBJ_ID_EVENT_LINK:
            case SG_OBJ_ID_EVENT_PHONE:
            case SG_OBJ_ID_EVENT_IP:
            case SG_OBJ_ID_EVENT_UDP_PORT:
            case SG_OBJ_ID_EVENT_TCP_PORT:

            default:
                return MKG_UNSUPPORTED_OBJ_ID_ERROR;
        }
    }

    return MKG_OK;
}

const char* sg_get_mkg_obj_id_name( enum MKG_SG_Obj_ID id )
{
    switch ( id )
    {
        case MKG_SG_OBJ_MOD_ID:
            return "MKG_SG_OBJ_MOD_ID";
        case MKG_SG_OBJ_HW_VER:
            return "MKG_SG_OBJ_HW_VER";
        case MKG_SG_OBJ_SW_VER:
            return "MKG_SG_OBJ_SW_VER";
        case MKG_SG_OBJ_SN:
            return "MKG_SG_OBJ_SN";
        case MKG_SG_OBJ_BYE:
            return "MKG_SG_OBJ_BYE";
        case MKG_SG_OBJ_GSM_CLOCK:
            return "MKG_SG_OBJ_GSM_CLOCK";
        case MKG_SG_OBJ_SC_TIMEOUT:
            return "MKG_SG_OBJ_SC_TIMEOUT";
        default:
            return sg_get_obj_id_name( (int) id );
    }
}

/*** end of file ***/

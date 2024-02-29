/*!****************************************************************************
 *
 * \file sg_obj.c
 * \brief SMART-GAS object implementation functions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 296553 $
 * \date $Date: 2020-09-30 11:19:01 +0200 (śr.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h> /* strlen */
#include "sg_obj.h"
#include "../util/str_util.h"

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

/*
 * Return default size of data for object.
 */
static uint16_t get_object_default_data_size( enum SG_OBJ_ID_POOL obj_pool );

/******************************************************************************
 * Functions
 ******************************************************************************/

SG_OBJ_OP_STATUS_t sg_serialize_obj( enum WO_REQ_DIR dir __attribute__((unused)),
                                     const SG_WO_OBJ_t *obj, uint8_t *data,
                                     const uint16_t max_data_len,
                                     uint16_t *len )
{
    uint16_t index = 0; /* Data stream index. */

    /* Validate the input data. */
    if ( ( ( (void*) 0UL ) == obj ) || ( ( (void*) 0UL ) == data )
         || ( ( (void*) 0UL ) == len ) )
    {
        return SG_OBJ_OP_ERROR_INVALID_INPUT_PARAMTERS;
    }

    /* Check for proper output size. */
    if ( 2U > max_data_len )
    {
        return SG_OBJ_OP_ERROR_INVALID_OUTPUT_DATA_SIZE;
    }
    else
    {
        /* Get an object header. */
        data[index++] = ( obj->err << 7 ) | ( obj->tskt << 4 )
                        | ( ( obj->id >> 8 ) & 0x0F );
        data[index++] = ( obj->id & 0xFF );
    }

    /* Check for proper output data size. */
    if ( ( 2U + get_object_default_data_size( obj->id >> 8 ) ) > max_data_len )
    {
        return SG_OBJ_OP_ERROR_INVALID_OUTPUT_DATA_SIZE;
    }

    /* Get an object data. */
    if ( SG_OBJ_TSKT_PUT == obj->tskt )
    {
        if ( obj->valid )
        {
            /* Get the type of object. */
            switch ( obj->id >> 8 )
            {
                case SG_OBJ_POOL_1:
                    data[index++] = obj->data[0];
                    break;
                case SG_OBJ_POOL_2:
                    data[index++] = obj->data[1];
                    data[index++] = obj->data[0];
                    break;
                case SG_OBJ_POOL_3:
                    data[index++] = obj->data[2];
                    data[index++] = obj->data[1];
                    data[index++] = obj->data[0];
                    break;
                case SG_OBJ_POOL_4:
                    data[index++] = obj->data[3];
                    data[index++] = obj->data[2];
                    data[index++] = obj->data[1];
                    data[index++] = obj->data[0];
                    break;
                case SG_OBJ_POOL_5:
                    data[index++] = obj->data_len & 0xFF;
                    for ( uint8_t j = 0; j < obj->data_len; j++ )
                    {
                        data[index++] = obj->data[j];
                    }
                    break;
                case SG_OBJ_POOL_6:
                    data[index++] = obj->plen;
                    if ( obj->plen )
                    {
                        for ( uint8_t j = 0;
                                ( j < obj->plen ) && ( j
                                        < SG_OBJ_PAR_RAW_MAX_LEN ); j++ )
                        {
                            data[index++] = obj->par_raw[j];
                        }
                    }
                    data[index++] = obj->data_len;
                    for ( uint8_t j = 0; j < obj->data_len; j++ )
                    {
                        data[index++] = obj->data[j];
                    }
                    break;
                case SG_OBJ_POOL_13:
                    data[index++] = obj->data_len >> 8;
                    data[index++] = obj->data_len & 0xFF;
                    for ( uint16_t j = 0;
                            ( j < obj->data_len ) && ( j < SG_OBJ_DATA_MAX_LEN );
                            j++ )
                    {
                        data[index++] = obj->data[j];
                    }
                    break;
                default:
                    return SG_OBJ_OP_ERROR_OBJECT_TYPE_UNSUPPORTED;
            }
        }
    }

    *len = index;

    return SG_OBJ_OP_OK;
}

/* cppcheck-suppress unusedFunction ; Unused function but part of API. */
SG_OBJ_OP_STATUS_t sg_serialize_obj_list( enum WO_REQ_DIR dir,
                                          const SG_WO_PDU_t *list,
                                          uint8_t *data,
                                          const uint16_t max_data_len,
                                          uint16_t *len )
{
    SG_OBJ_OP_STATUS_t status; /* Operational status. */
    uint16_t index = 0; /* Data stream index. */

    if ( ( ( (void*) 0UL ) == list ) || ( ( (void*) 0UL ) == data )
         || ( ( (void*) 0UL ) == len ) )
    {
        return SG_OBJ_OP_ERROR_INVALID_INPUT_PARAMTERS;
    }

    for ( uint8_t i = 0;
            ( i < list->wo_data_count ) && ( i < list->wo_data_max_count );
            i++ )
    {
        const SG_WO_OBJ_t *obj = &list->wo_data_ptr[i];
        uint16_t obj_size = 0;

        status = sg_serialize_obj( dir, obj, &data[index], max_data_len - index,
                                   &obj_size );

        if ( SG_OBJ_OP_OK == status )
        {
            index += obj_size;
        }
        else /* Break serialization on error. */
        {
            break;
        }
    }

    *len = index;

    return status;
}

SG_OBJ_OP_STATUS_t sg_deserialize_obj( enum WO_REQ_DIR dir, const uint8_t *data,
                                       const uint16_t data_len,
                                       SG_WO_OBJ_t *obj, uint16_t *obj_size )
{
    uint16_t index = 0; /* Position in data stream. */

    if ( ( ( (void*) 0UL ) == data ) || ( ( (void*) 0UL ) == obj )
         || ( ( (void*) 0UL ) == obj_size ) )
    {
        return SG_OBJ_OP_ERROR_INVALID_INPUT_PARAMTERS;
    }

    /* Get object error flag, method and id. */
    if ( data_len > 1 )
    {
        obj->err = (SG_OBJ_STATUS_t) ( data[0] >> 7 );
        obj->tskt = (enum SG_OBJ_TSKT) ( ( data[0] >> 4 ) & 0x1 );
        obj->id = (SG_OBJ_ID_t) ( ( data[0] & 0x0F ) << 8 ) | ( data[1] );
        /* Update processed size. */
        index = 2;
    }
    else /* Incomplete the data to decode object. */
    {
        /* Object incomplete. */
        obj->valid = 0;

        return SG_OBJ_OP_ERROR_INVALID_INPUT_DATA_SIZE;
    }

    if ( SG_OBJ_ERROR == obj->err )
    {
        /* Get error ID (LE). */
        if ( data_len > 3 )
        {
            obj->data[0] = data[3];
            obj->data[1] = data[2];
            /* UUpdate processed size */
            index += 2;
            obj->valid = 1;
        }
        else /* Incomplete data object. */
        {
            obj->valid = 0;
        }
    }
    else /* SG_OBJ_OK */
    {
        /* GET request from SC contains only id. */
        if ( ( WO_REQ_DIR_FROM_SC_TO_GM == dir ) && ( SG_OBJ_TSKT_GET
                == obj->tskt ) )
        {
            obj->valid = 1;
        }
        else /* Other type of requests contains the data. */
        {
            /* Get the type of object. */
            switch ( obj->id >> 8 )
            {
                case SG_OBJ_POOL_1:
                    if ( data_len > 2 )
                    {
                        obj->data[0] = data[2];
                        /* Update processed size. */
                        index += 1;
                        obj->valid = 1;
                    }
                    else /* Incomplete data object. */
                    {
                        obj->valid = 0;
                    }
                    break;
                case SG_OBJ_POOL_2:
                    if ( data_len > 3 )
                    {
                        obj->data[0] = data[3];
                        obj->data[1] = data[2];
                        /* Update processed size. */
                        index += 2;
                        obj->valid = 1;
                    }
                    else /* Incomplete data object. */
                    {
                        obj->valid = 0;
                    }
                    break;
                case SG_OBJ_POOL_3:
                    if ( data_len > 4 )
                    {
                        obj->data[0] = data[4];
                        obj->data[1] = data[3];
                        obj->data[2] = data[2];
                        /* Update processed size. */
                        index += 3;
                        obj->valid = 1;
                    }
                    else /* Incomplete data object. */
                    {
                        obj->valid = 0;
                    }
                    break;
                case SG_OBJ_POOL_4:
                    if ( data_len > 5 )
                    {
                        obj->data[0] = data[5];
                        obj->data[1] = data[4];
                        obj->data[2] = data[3];
                        obj->data[3] = data[2];
                        /* Update processed size. */
                        index += 4;
                        obj->valid = 1;
                    }
                    else /* Incomplete data object. */
                    {
                        obj->valid = 0;
                    }
                    break;
                case SG_OBJ_POOL_5:
                    if ( data_len > 2 )
                    {
                        obj->data_len = data[2];

                        if ( ( data_len >= ( 3 + obj->data_len ) ) && ( obj
                                                                                ->data_len
                                                                        <= SG_OBJ_DATA_MAX_LEN ) )
                        {
                            for ( uint8_t j = 0; j < obj->data_len; j++ )
                            {
                                obj->data[j] = data[3 + j];
                            }
                            /* Update processed size. */
                            index += 1 + obj->data_len;
                            obj->valid = 1;
                        }
                        else /* Incomplete or too large data object. */
                        {
                            obj->valid = 0;
                        }
                    }
                    else /* Incomplete data object. */
                    {
                        obj->valid = 0;
                    }
                    break;
                case SG_OBJ_POOL_6:
                    /* Get parameters. */
                    if ( data_len > 2 )
                    {
                        obj->plen = data[2];

                        if ( ( data_len >= ( 3 + obj->plen ) ) && ( obj->plen
                                <= SG_OBJ_PAR_MAX_LEN ) )
                        {
                            for ( uint8_t j = 0; j < obj->plen; j++ )
                            {
                                obj->par_raw[j] = data[3 + j];
                            }
                            /* Update processed size. */
                            index += 1 + obj->plen;
                            obj->valid = 1;
                        }
                        else /* Incomplete or too large data object. */
                        {
                            obj->valid = 0;
                        }
                    }
                    else /* Incomplete or too large data object. */
                    {
                        obj->valid = 0;
                    }

                    /* Get data. */
                    if ( data_len >= ( 3 + obj->plen ) )
                    {
                        obj->data_len = data[3 + obj->plen];

                        if ( ( data_len >= ( 4 + obj->plen + obj->data_len ) ) && ( obj
                                                                                            ->data_len
                                                                                    <= SG_OBJ_DATA_MAX_LEN ) )
                        {
                            for ( uint8_t j = 0; j < obj->data_len; j++ )
                            {
                                obj->data[j] = data[4 + obj->plen + j];
                            }
                            /* Update processed size. */
                            index += 1 + obj->plen + obj->data_len;
                            /* Do not overwrite the false value valid flag. */
                            obj->valid = ( 0 == obj->valid ) ? 0 : 1;
                        }
                        else /* Incomplete or too large data object. */
                        {
                            obj->valid = 0;
                        }
                    }
                    else /* Incomplete data of the object. */
                    {
                        obj->valid = 0;
                    }
                    break;
                default:
                    obj->valid = 0;
                    break;
            }
        }
    }

    /* Set the processed size. */
    *obj_size = index;

    return SG_OBJ_OP_OK;
}

SG_OBJ_OP_STATUS_t sg_deserialize_obj_list( enum WO_REQ_DIR dir,
                                            const uint8_t *data,
                                            const uint16_t data_len,
                                            SG_WO_PDU_t *list )
{
    SG_OBJ_OP_STATUS_t res; /* Result of deserialization. */
    uint16_t index = 0U; /* Proceeded. */
    uint8_t obj_num = 0U;

    if ( ( ( (void*) 0UL ) == data ) || ( ( (void*) 0UL ) == list ) )
    {
        return SG_OBJ_OP_ERROR_INVALID_INPUT_PARAMTERS;
    }

    do
    {
        SG_WO_OBJ_t *obj = &list->wo_data_ptr[obj_num];
        uint16_t obj_size = 0U;

        res = sg_deserialize_obj( dir, &data[index], data_len - index, obj,
                                  &obj_size );

        if ( SG_OBJ_OP_OK == res )
        {
            index += obj_size;
            list->wo_data_count = ++obj_num;
        }

        /* Check the limit. */
        if ( list->wo_data_max_count <= obj_num )
        {
            return SG_OBJ_OP_ERROR_OBJECT_LIST_LIMIT_EXCEEDED;
        }
    }
    while ( ( index < data_len ) && ( SG_OBJ_OP_OK == res ) );

    return res;
}

uint32_t sg_obj_to_str( const SG_WO_OBJ_t *obj, char *str,
                        const uint32_t str_max_len )
{
    uint32_t pos = 0;

    if ( ( (void*) 0UL ) == str )
    {
        return 0;
    }

    if ( ( (void*) 0UL ) == obj )
    {
        snprintf( &str[pos], str_max_len - pos - 1, "<null>" );
        pos = strlen( str );

        return pos;
    }

    snprintf( &str[pos], str_max_len - pos - 1,
              "OBJ_ID=0x%04X (%s),OBJ_TSKT=%d,OBJ_ERR=%d", obj->id,
              sg_get_obj_id_name( obj->id ), obj->tskt, obj->err );
    pos = strlen( str );

    if ( SG_OBJ_ERROR == obj->err )
    {
        snprintf(
                &str[pos],
                str_max_len - pos - 1,
                ",ERROR=%s",
                sg_get_obj_error_name(
                        (uint16_t) ( ( obj->data[0] << 8 ) | obj->data[1] ) ) );
        pos = strlen( str );
    }
    else
    {
        if ( ( obj->id >= (SG_OBJ_ID_t) SG_OBJ_POOL_1_LOW ) && ( obj->id
                <= (SG_OBJ_ID_t) SG_OBJ_POOL_1_HIGH ) )
        {
            snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_DATA=0x%02X",
                      (uint8_t) obj->data[0] );
            pos = strlen( str );
        }
        else if ( ( obj->id >= (SG_OBJ_ID_t) SG_OBJ_POOL_2_LOW )
                && ( obj->id <= (SG_OBJ_ID_t) SG_OBJ_POOL_2_HIGH ) )
        {
            snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_DATA=0x%04X",
                      (uint16_t) ( ( obj->data[0] << 8 ) | obj->data[1] ) );
            pos = strlen( str );
        }
        else if ( ( obj->id >= (SG_OBJ_ID_t) SG_OBJ_POOL_3_LOW )
                && ( obj->id <= (SG_OBJ_ID_t) SG_OBJ_POOL_3_HIGH ) )
        {
            /* Casting to unsigned long is due to incompatability between cppecheck and gcc. */
            snprintf(
                    &str[pos],
                    str_max_len - pos - 1,
                    ",OBJ_DATA=0x%06lX",
                    (unsigned long) ( ( obj->data[0] << 16 )
                            | ( obj->data[1] << 8 ) | obj->data[2] ) );
            pos = strlen( str );
        }
        else if ( ( obj->id >= (SG_OBJ_ID_t) SG_OBJ_POOL_4_LOW )
                && ( obj->id <= (SG_OBJ_ID_t) SG_OBJ_POOL_4_HIGH ) )
        {
            /* Casting to unsigned long is due to incompatability between cppecheck and gcc. */
            snprintf(
                    &str[pos],
                    str_max_len - pos - 1,
                    ",OBJ_DATA=0x%08lX",
                    (unsigned long) ( ( obj->data[0] << 24 )
                            | ( obj->data[1] << 16 ) | ( obj->data[2] << 8 )
                            | obj->data[3] ) );
            pos = strlen( str );
        }
        else if ( ( obj->id >= (SG_OBJ_ID_t) SG_OBJ_POOL_5_LOW )
                && ( obj->id <= (SG_OBJ_ID_t) SG_OBJ_POOL_5_HIGH ) )
        {
            snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_LEN=%d",
                      obj->data_len );
            if ( obj->data_len )
            {
                char data_str[2 * SG_OBJ_DATA_MAX_LEN + 1];
                str_bin_to_ascii( obj->data, data_str, obj->data_len );

                snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_DATA=0x%s",
                          data_str );
                pos = strlen( str );
            }
        }
        else if ( ( obj->id >= (SG_OBJ_ID_t) SG_OBJ_POOL_6_LOW )
                && ( obj->id <= (SG_OBJ_ID_t) SG_OBJ_POOL_6_HIGH ) )
        {
            snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_PLEN=%d",
                      obj->plen );
            pos = strlen( str );

            if ( obj->plen )
            {
                char datails_str[2 * SG_OBJ_PAR_RAW_MAX_LEN + 1];
                str_bin_to_ascii( obj->par_raw, datails_str, obj->plen - 1 );
                snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_PAR={0x%s}",
                          datails_str );
                pos = strlen( str );
            }

            snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_LEN=%d",
                      obj->data_len );
            pos = strlen( str );

            if ( obj->data_len )
            {
                char data_str[2 * SG_OBJ_DATA_MAX_LEN + 1];
                str_bin_to_ascii( obj->data, data_str, obj->data_len );

                snprintf( &str[pos], str_max_len - pos - 1, ",OBJ_DATA=0x%s",
                          data_str );
                pos = strlen( str );
            }
        }
    }

    return pos;
}

const char* sg_get_obj_id_name( uint16_t obj_id )
{
    const char *name;

    switch ( obj_id )
    {
        /* 6.1 Informacyjne . */
        case SG_OBJ_ID_STANDARD_PARAMETER_LIST_VERSION:
            name = "STANDARD_PARAMETER_LIST_VERSION";
            break;
        case SG_OBJ_ID_SERIAL_NBR:
            name = "SERIAL_NBR";
            break;
        case SG_OBJ_ID_LATITUDE:
            name = "LATITUDE";
            break;
        case SG_OBJ_ID_LONGITUDE:
            name = "LONGITUDE";
            break;
        case SG_OBJ_ID_HARDWARE_VERSION:
            name = "HARDWARE_VERSION";
            break;
        case SG_OBJ_ID_FIRMWARE_VERSION:
            name = "FIRMWARE_VERSION";
            break;
        case SG_OBJ_ID_CLIENT_ID:
            name = "CLIENT_ID";
            break;
        case SG_OBJ_ID_GAS_METER_SERIAL_NBR:
            name = "GAS_METER_SERIAL_NBR";
            break;
        case SG_OBJ_ID_DEVICE_TYPE_NAME:
            name = "DEVICE_TYPE_NAME";
            break;

            /* 6.2 Liczydła i zużycia . */
        case SG_OBJ_ID_UNITS:
            name = "UNITS";
            break;
        case SG_OBJ_ID_COUNTERS_REGISTRATION_PERIOD:
            name = "COUNTERS_REGISTRATION_PERIOD";
            break;
        case SG_OBJ_ID_CALORIFIC_VALUE:
            name = "CALORIFIC_VALUE";
            break;
        case SG_OBJ_ID_VOLUME:
            name = "VOLUME";
            break;
        case SG_OBJ_ID_COMMAND_VOLUME_OFFSET:
            name = "COMMAND_VOLUME_OFFSET";
            break;
        case SG_OBJ_ID_ENERGY:
            name = "ENERGY";
            break;
        case SG_OBJ_ID_COMMAND_ENERGY_OFFSET:
            name = "COMMAND_ENERGY_OFFSET";
            break;
        case SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_TIME:
            name = "COUNTERS_ARCHIVE_LAST_TIME";
            break;
        case SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_VOLUME:
            name = "COUNTERS_ARCHIVE_LAST_VOLUME";
            break;
        case SG_OBJ_ID_COUNTERS_ARCHIVE_LAST_ENERGY:
            name = "COUNTERS_ARCHIVE_LAST_ENERGY";
            break;
        case SG_OBJ_ID_VOLUME_MAX_VALUE:
            name = "VOLUME_MAX_VALUE";
            break;
        case SG_OBJ_ID_PERIOD_ENERGY_USAGE_COMPRESSED:
            name = "PERIOD_ENERGY_USAGE_COMPRESSED";
            break;
        case SG_OBJ_ID_MONTH_VOLUME_USAGE_COMPRESSED:
            name = "MONTH_VOLUME_USAGE_COMPRESSED";
            break;
        case SG_OBJ_ID_MONTH_ENERGY_USAGE_COMPRESSED:
            name = "MONTH_ENERGY_USAGE_COMPRESSED";
            break;
        case SG_OBJ_ID_COUNTERS_ARCHIVE:
            name = "COUNTERS_ARCHIVE";
            break;
        case SG_OBJ_ID_PERIOD_VOLUME_USAGE_COMPRESSED:
            name = "PERIOD_VOLUME_USAGE_COMPRESSED";
            break;

            /* 6.3 Przepływ . */
        case SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED:
            name = "HOURLY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED";
            break;
        case SG_OBJ_ID_GAS_METER_Q_MAX:
            name = "GAS_METER_Q_MAX";
            break;
        case SG_OBJ_ID_TEMPORARY_FLOW:
            name = "TEMPORARY_FLOW";
            break;
        case SG_OBJ_ID_HOURLY_USAGE_CURRENT:
            name = "HOURLY_USAGE_CURRENT";
            break;
        case SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED:
            name = "TEMPORARY_FLOW_PERIOD_LATCHED";
            break;
        case SG_OBJ_ID_TEMPORARY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED:
            name = "TEMPORARY_FLOW_PERIOD_LATCHED_TIME_COMPRESSED";
            break;
        case SG_OBJ_ID_HOURLY_FLOW_PERIOD_LATCHED:
            name = "HOURLY_FLOW_PERIOD_LATCHED";
            break;
        case SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED:
            name = "TEMPORARY_FLOW_MONTHLY_LATCHED";
            break;
        case SG_OBJ_ID_TEMPORARY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED:
            name = "TEMPORARY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED";
            break;
        case SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED:
            name = "HOURLY_FLOW_MONTHLY_LATCHED";
            break;
        case SG_OBJ_ID_HOURLY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED:
            name = "HOURLY_FLOW_MONTHLY_LATCHED_TIME_COMPRESSED";
            break;

            /* 6.4 Zegar . */
        case SG_OBJ_ID_TIME_ZONE_OFFSET:
            name = "TIME_ZONE_OFFSET";
            break;
        case SG_OBJ_ID_TIME_ZONE_OFFSET_WINTER:
            name = "TIME_ZONE_OFFSET_WINTER";
            break;
        case SG_OBJ_ID_TIME_ZONE_OFFSET_SUMMER:
            name = "TIME_ZONE_OFFSET_SUMMER";
            break;
        case SG_OBJ_ID_CLOCK_SYNCHRONISATION_FROM_GSM:
            name = "CLOCK_SYNCHRONISATION_FROM_GSM";
            break;
        case SG_OBJ_ID_TIME_ZONE_AUTO_OFFSET:
            name = "TIME_ZONE_AUTO_OFFSET";
            break;
        case SG_OBJ_ID_CLOCK_SYNCHRONISATION_OFFSET:
            name = "CLOCK_SYNCHRONISATION_OFFSET";
            break;
        case SG_OBJ_ID_CLOCK:
            name = "CLOCK";
            break;

            /* 6.5 Ingerencja . */
        case SG_OBJ_ID_TAMPER_REMOVE_COUNT:
            name = "TAMPER_REMOVE_COUNT";
            break;
        case SG_OBJ_ID_TAMPER_OPEN_COUNT:
            name = "TAMPER_OPEN_COUNT";
            break;
        case SG_OBJ_ID_TAMPER_MAGNETIC_COUNT:
            name = "TAMPER_MAGNETIC_COUNT";
            break;
        case SG_OBJ_ID_TAMPER_REMOVE_TIME:
            name = "TAMPER_REMOVE_TIME";
            break;
        case SG_OBJ_ID_TAMPER_REMOVE_DURATION:
            name = "TAMPER_REMOVE_DURATION";
            break;
        case SG_OBJ_ID_TAMPER_REMOVE_VOLUME:
            name = "TAMPER_REMOVE_VOLUME";
            break;
        case SG_OBJ_ID_TAMPER_REMOVE_ENERGY:
            name = "TAMPER_REMOVE_ENERGY";
            break;
        case SG_OBJ_ID_TAMPER_OPEN_TIME:
            name = "TAMPER_OPEN_TIME";
            break;
        case SG_OBJ_ID_TAMPER_OPEN_DURATION:
            name = "TAMPER_OPEN_DURATION";
            break;
        case SG_OBJ_ID_TAMPER_OPEN_VOLUME:
            name = "TAMPER_OPEN_VOLUME";
            break;
        case SG_OBJ_ID_TAMPER_OPEN_ENERGY:
            name = "TAMPER_OPEN_ENERGY";
            break;
        case SG_OBJ_ID_TAMPER_MAGNETIC_TIME:
            name = "TAMPER_MAGNETIC_TIME";
            break;
        case SG_OBJ_ID_TAMPER_MAGNETIC_DURATION:
            name = "TAMPER_MAGNETIC_DURATION";
            break;
        case SG_OBJ_ID_TAMPER_MAGNETIC_VOLUME:
            name = "TAMPER_MAGNETIC_VOLUME";
            break;
        case SG_OBJ_ID_TAMPER_MAGNETIC_ENERGY:
            name = "TAMPER_MAGNETIC_ENERGY";
            break;

            /* 6.6 Diagnostyki . */
        case SG_OBJ_ID_BATTERY_LEVEL:
            name = "BATTERY_LEVEL";
            break;
        case SG_OBJ_ID_TEMPERATURE:
            name = "TEMPERATURE";
            break;

            /* 6.7 Modem . */
        case SG_OBJ_ID_SIM_SMS_VALIDITY:
            name = "SIM_SMS_VALIDITY";
            break;
        case SG_OBJ_ID_GSM_QUALITY:
            name = "GSM_QUALITY";
            break;
        case SG_OBJ_ID_MODEM_STATE:
            name = "MODEM_STATE";
            break;
        case SG_OBJ_ID_MODEM_SMS_RECEIVED:
            name = "MODEM_SMS_RECEIVED";
            break;
        case SG_OBJ_ID_MODEM_SMS_SENT:
            name = "MODEM_SMS_SENT";
            break;
        case SG_OBJ_ID_MODEM_GPRS_PACKETS_RECEIVED:
            name = "MODEM_GPRS_PACKETS_RECEIVED";
            break;
        case SG_OBJ_ID_MODEM_GPRS_PACKETS_SENT:
            name = "MODEM_GPRS_PACKETS_SENT";
            break;
        case SG_OBJ_ID_MODEM_WAKEUP_COUNT:
            name = "MODEM_WAKEUP_COUNT";
            break;
        case SG_OBJ_ID_SIM_IP:
            name = "SIM_IP";
            break;
        case SG_OBJ_ID_GSM_BTS_LAC:
            name = "GSM_BTS_LAC";
            break;
        case SG_OBJ_ID_GSM_BTS_CELL_ID:
            name = "GSM_BTS_CELL_ID";
            break;
        case SG_OBJ_ID_MODEM_WAKEUP_DURATION:
            name = "MODEM_WAKEUP_DURATION";
            break;
        case SG_OBJ_ID_SIM_PIN:
            name = "SIM_PIN";
            break;
        case SG_OBJ_ID_SIM_ICCID:
            name = "SIM_ICCID";
            break;
        case SG_OBJ_ID_SIM_PHONE:
            name = "SIM_PHONE";
            break;
        case SG_OBJ_ID_SIM_SMSC:
            name = "SIM_SMSC";
            break;
        case SG_OBJ_ID_GPRS_APN:
            name = "GPRS_APN";
            break;
        case SG_OBJ_ID_GPRS_USER:
            name = "GPRS_USER";
            break;
        case SG_OBJ_ID_GPRS_PASSWORD:
            name = "GPRS_PASSWORD";
            break;
        case SG_OBJ_ID_GSM_NETWORK_CODE:
            name = "GSM_NETWORK_CODE";
            break;

            /* 6.8 Konfiguracja zdarzeń . */
        case SG_OBJ_ID_EVENT_LINK:
            name = "EVENT_LINK";
            break;
        case SG_OBJ_ID_EVENT_UDP_PORT:
            name = "EVENT_UDP_PORT";
            break;
        case SG_OBJ_ID_EVENT_TCP_PORT:
            name = "EVENT_TCP_PORT";
            break;
        case SG_OBJ_ID_EVENT_IP:
            name = "EVENT_IP";
            break;
        case SG_OBJ_ID_EVENT_PHONE:
            name = "EVENT_PHONE";
            break;

            /* 6.9 Zawór . */
        case SG_OBJ_ID_VALVE_OPERATION:
            name = "VALVE_OPERATION";
            break;
        case SG_OBJ_ID_VALVE_STATE:
            name = "VALVE_STATE";
            break;
        case SG_OBJ_ID_VALVE_ERROR_CODE:
            name = "VALVE_ERROR_CODE";
            break;
        case SG_OBJ_ID_VALVE_ACTIVATION_TIMEOUT:
            name = "VALVE_ACTIVATION_TIMEOUT";
            break;
        case SG_OBJ_ID_VALVE_LEAK_TEST_DURATION:
            name = "VALVE_LEAK_TEST_DURATION";
            break;
        case SG_OBJ_ID_VALVE_LEAK_TEST_VOLUME:
            name = "VALVE_LEAK_TEST_VOLUME";
            break;
        case SG_OBJ_ID_VALVE_OPEN_COUNT:
            name = "VALVE_OPEN_COUNT";
            break;
        case SG_OBJ_ID_VALVE_CLOSE_COUNT:
            name = "VALVE_CLOSE_COUNT";
            break;

            /* 6.10 Przedpłaty . */
        case SG_OBJ_ID_PREPAID_TOPUP_ID:
            name = "PREPAID_TOPUP_ID";
            break;
        case SG_OBJ_ID_PREPAID_VALVE_CONTROL:
            name = "PREPAID_VALVE_CONTROL";
            break;
        case SG_OBJ_ID_PREPAID_TOPUP_END_TIME:
            name = "PREPAID_TOPUP_END_TIME";
            break;
        case SG_OBJ_ID_PREPAID_TOPUP_END_VOLUME:
            name = "PREPAID_TOPUP_END_VOLUME";
            break;
        case SG_OBJ_ID_PREPAID_TOPUP_END_ENERGY:
            name = "PREPAID_TOPUP_END_ENERGY";
            break;

            /* 6.11 Status . */
        case SG_OBJ_ID_STATUS:
            name = "STATUS";
            break;
        case SG_OBJ_ID_STATUS_PERIODIC_LATCHED:
            name = "STATUS_PERIODIC_LATCHED";
            break;
        case SG_OBJ_ID_STATUS_MONTHLY_LATCHED:
            name = "STATUS_MONTHLY_LATCHED";
            break;
        case SG_OBJ_ID_STATUS_UP_EVENT_MASK:
            name = "STATUS_UP_EVENT_MASK";
            break;
        case SG_OBJ_ID_STATUS_DOWN_EVENT_MASK:
            name = "STATUS_DOWN_EVENT_MASK";
            break;
        case SG_OBJ_ID_STATUS_LATCHED:
            name = "STATUS_LATCHED";
            break;
        case SG_OBJ_ID_STATUS_COMMAND:
            name = "STATUS_COMMAND";
            break;

            /* 6.12 Harmonogramy . */
        case SG_OBJ_ID_COL_SCHEDULE_YEAR:
            name = "COL_SCHEDULE_YEAR";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_MONTH:
            name = "COL_SCHEDULE_MONTH";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_DAY_OF_MONTH:
            name = "COL_SCHEDULE_DAY_OF_MONTH";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_DAY_OF_WEEK:
            name = "COL_SCHEDULE_DAY_OF_WEEK";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_HOUR:
            name = "COL_SCHEDULE_HOUR";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_MINUTE:
            name = "COL_SCHEDULE_MINUTE";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_LOCAL_TIME:
            name = "COL_SCHEDULE_LOCAL_TIME";
            break;
        case SG_OBJ_ID_COL_SCHEDULE_COMMAND:
            name = "COL_SCHEDULE_COMMAND";
            break;
        case SG_OBJ_ID_SCHEDULE_TABLE:
            name = "SCHEDULE_TABLE";
            break;

            /* 6.13 Struktury danych . */
        case SG_OBJ_ID_COL_OBJ_ID:
            name = "COL_OBJ_ID";
            break;
        case SG_OBJ_ID_PERIODIC_VOLUME_DATA_LATCHED:
            name = "PERIODIC_VOLUME_DATA_LATCHED";
            break;
        case SG_OBJ_ID_PERIODIC_VOLUME_DATA_CURRENT:
            name = "PERIODIC_VOLUME_DATA_CURRENT";
            break;
        case SG_OBJ_ID_PERIODIC_ENERGY_DATA_LATCHED:
            name = "PERIODIC_ENERGY_DATA_LATCHED";
            break;
        case SG_OBJ_ID_PERIODIC_ENERGY_DATA_CURRENT:
            name = "PERIODIC_ENERGY_DATA_CURRENT";
            break;
        case SG_OBJ_ID_MONTHLY_DATA_LATCHED:
            name = "MONTHLY_DATA_LATCHED";
            break;
        case SG_OBJ_ID_MONTHLY_DATA_CURRENT:
            name = "MONTHLY_DATA_CURRENT";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_1:
            name = "USER_STRUCTURE_1";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_2:
            name = "USER_STRUCTURE_2";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_3:
            name = "USER_STRUCTURE_3";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_4:
            name = "USER_STRUCTURE_4";
            break;
        case SG_OBJ_ID_PERIODIC_VOLUME_DATA_DEF:
            name = "PERIODIC_VOLUME_DATA_DEF";
            break;
        case SG_OBJ_ID_PERIODIC_ENERGY_DATA_DEF:
            name = "PERIODIC_ENERGY_DATA_DEF";
            break;
        case SG_OBJ_ID_MONTHLY_DATA_DEF:
            name = "MONTHLY_DATA_DEF";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_1_DEF:
            name = "USER_STRUCTURE_1_DEF";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_2_DEF:
            name = "USER_STRUCTURE_2_DEF";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_3_DEF:
            name = "USER_STRUCTURE_3_DEF";
            break;
        case SG_OBJ_ID_USER_STRUCTURE_4_DEF:
            name = "USER_STRUCTURE_4_DEF";
            break;
        case SG_OBJ_ID_PERIODIC_VOLUME_DATA_ARCHIVE:
            name = "PERIODIC_VOLUME_DATA_ARCHIVE";
            break;
        case SG_OBJ_ID_PERIODIC_ENERGY_DATA_ARCHIVE:
            name = "PERIODIC_ENERGY_DATA_ARCHIVE";
            break;
        case SG_OBJ_ID_MONTHLY_DATA_ARCHIVE:
            name = "MONTHLY_DATA_ARCHIVE";
            break;

            /* 6.14 Parametry protokołu i bezpieczeństwo . */
        case SG_OBJ_ID_WDP_ARP:
            name = "WDP_ARP";
            break;
        case SG_OBJ_ID_WDP_AT:
            name = "WDP_AT";
            break;
        case SG_OBJ_ID_WDP_MR:
            name = "WDP_MR";
            break;
        case SG_OBJ_ID_WPP_CRYPTOGRAPHY_OBLIGATORY:
            name = "WPP_CRYPTOGRAPHY_OBLIGATORY";
            break;
        case SG_OBJ_ID_WPP_ACCESS_CONTROL_OBLIGATORY:
            name = "WPP_ACCESS_CONTROL_OBLIGATORY";
            break;
        case SG_OBJ_ID_COL_ACCLVL_READ:
            name = "COL_ACCLVL_READ";
            break;
        case SG_OBJ_ID_COL_ACCLVL_WRITE:
            name = "COL_ACCLVL_WRITE";
            break;
        case SG_OBJ_ID_WPP_EVENT_SEND_SN:
            name = "WPP_EVENT_SEND_SN";
            break;
        case SG_OBJ_ID_SMS_PHONE_NUMBERS_RESTRICTION:
            name = "SMS_PHONE_NUMBERS_RESTRICTION";
            break;
        case SG_OBJ_ID_WPP_IV_IN:
            name = "WPP_IV_IN";
            break;
        case SG_OBJ_ID_ACCESS_PASS:
            name = "ACCESS_PASS";
            break;
        case SG_OBJ_ID_WPP_KEY:
            name = "WPP_KEY";
            break;
        case SG_OBJ_ID_PERMITTED_PHONE_NUMBER:
            name = "PERMITTED_PHONE_NUMBER";
            break;
        case SG_OBJ_ID_WPP_ACCESS_PASS_TABLE:
            name = "WPP_ACCESS_PASS_TABLE";
            break;
        case SG_OBJ_ID_WPP_ACCESS_PERMISSIONS:
            name = "WPP_ACCESS_PERMISSIONS";
            break;
        case SG_OBJ_ID_SMS_PHONE_NUMBERS_TABLE:
            name = "SMS_PHONE_NUMBERS_TABLE";
            break;

            /* 6.15 Czas rejestracji archiwum danych periodycznych i miesięcznych . */
        case SG_OBJ_ID_MONTHLY_DATA_LATCH_DAY:
            name = "MONTHLY_DATA_LATCH_DAY";
            break;
        case SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_HOUR:
            name = "MONTHLY_AND_DAILY_DATA_LATCH_HOUR";
            break;
        case SG_OBJ_ID_MONTHLY_AND_DAILY_DATA_LATCH_LTIME:
            name = "MONTHLY_AND_DAILY_DATA_LATCH_LOCAL_TIME";
            break;
        case SG_OBJ_ID_LAST_DAILY_LATCH_TIME:
            name = "LAST_DAILY_LATCH_TIME";
            break;
        case SG_OBJ_ID_LAST_DAILY_LATCH_VOLUME:
            name = "LAST_DAILY_LATCH_VOLUME";
            break;
        case SG_OBJ_ID_LAST_DAILY_LATCH_ENERGY:
            name = "LAST_DAILY_LATCH_ENERGY";
            break;

            /* Własne . */
        default:
            name = "UNKNOWN";
            break;
    }

    return name;
}

const char* sg_get_obj_error_name( uint16_t error )
{
    const char *name;

    switch ( error )
    {
        case SG_OBJD_ERR_NOT_FOUND:
            name = "ERR_NOT_FOUND";
            break;
        case SG_OBJD_ERR_READ_DENIED:
            name = "ERR_READ_DENIED";
            break;
        case SG_OBJD_ERR_WRITE_DENIED:
            name = "ERR_WRITE_DENIED";
            break;
        case SG_OBJD_ERR_WRONG_SIZE:
            name = "ERR_WRONG_SIZE";
            break;
        case SG_OBJD_ERR_WRONG_VALUE:
            name = "ERR_WRONG_VALUE";
            break;
        case SG_OBJD_ERR_UNEXPECTED_END:
            name = "ERR_UNEXPECTED_END";
            break;
        case SG_OBJD_ERR_TOO_LARGE:
            name = "ERR_TOO_LARGE";
            break;
        case SG_OBJD_ERR_TBL_OUT_OF_RANGE:
            name = "ERR_TBL_OUT_OF_RANGE";
            break;
        case SG_OBJD_ERR_PARAM_UNSUPPORTED:
            name = "ERR_PARAM_UNSUPPORTED";
            break;
        case SG_OBJD_ERR_PARAM_ERROR:
            name = "ERR_PARAM_ERROR";
            break;
        case SG_OBJD_ERR_TBL_COL_ERROR:
            name = "ERR_TBL_COL_ERROR";
            break;
        case SG_OBJD_ERR_ACCLVL_ERROR:
            name = "ERR_ACCLVL_ERROR";
            break;
        case SG_OBJD_ERR_ACCPASS_ERROR:
            name = "ERR_ACCPASS_ERROR";
            break;
        case SG_OBJD_ERR_NOT_INITIALISED:
            name = "ERR_NOT_INITIALISED";
            break;
        case SG_OBJD_ERR_UNKNOWN:
            name = "ERR_UNKNOWN";
            break;

        default:
            if ( ( error >= SG_OBJD_ERR_HARDWARE ) && ( error
                    <= SG_OBJD_ERR_HARDWARE_END ) )
            {
                name = "ERR_HARDWARE";
            }
            else if ( ( error >= SG_OBJD_ERR_FIRMWARE )
                    && ( error <= SG_OBJD_ERR_FIRMWARE_END ) )
            {
                name = "ERR_FIRMWARE";
            }
            else
            {
                name = "UNKNOWN";
            }
    }

    return name;
}

static uint16_t get_object_default_data_size( enum SG_OBJ_ID_POOL obj_pool )
{
    switch ( obj_pool )
    {
        case SG_OBJ_POOL_1:
            return 1U;
        case SG_OBJ_POOL_2:
            return 2U;
        case SG_OBJ_POOL_3:
            return 3U;
        case SG_OBJ_POOL_4:
            return 4U;
        case SG_OBJ_POOL_5:
            return SG_OBJ_DATA_MAX_LEN + 1;
        case SG_OBJ_POOL_6:
            return (SG_OBJ_DATA_MAX_LEN + SG_OBJ_PAR_RAW_MAX_LEN + 2);
        case SG_OBJ_POOL_13:
            return (SG_OBJ_DATA_MAX_LEN + SG_OBJ_PAR_RAW_MAX_LEN + 3);
        case SG_OBJ_POOL_7:
        case SG_OBJ_POOL_8:
        case SG_OBJ_POOL_9:
        case SG_OBJ_POOL_10:
        case SG_OBJ_POOL_11:
        case SG_OBJ_POOL_12:
        case SG_OBJ_POOL_14:
        case SG_OBJ_POOL_15:
        default:
            return 0;
    }
}

/*** end of file ***/

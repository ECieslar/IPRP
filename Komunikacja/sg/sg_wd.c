/*!****************************************************************************
 *
 * \file sg_dtg.c
 * \brief Implementacja warstwy datagramowej (WD)
 *        wg protokołu SMART-GAS (IGG-0201:2018 pkt 3).
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

#include "sg_wd.h"
#include "../util/crc.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* Parametry protokołu. */
SG_WD_Params_t sg_wd_params =
{
    .wdp_arp = 0x0, /* Domyślnie brak potwierdzeń. */
    .wdp_at =  0,   /* Brak timeoutu. */
    .wdp_mr = 0, /* Brak retransmisji. */
};


/*! \brief Datagram reference number. */
uint8_t sg_dtg_ref = 0U;

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/

/*
 * \param dtg datagram struct
 * \param data the serialized output data
 * \param max_len the maximum length of output data
 * \param len serialized length of data
 */
WD_status_t sg_serialize_datagram( const SG_WD_PDU_t *dtg, uint8_t *data,
                                      const uint16_t max_len, uint16_t *len )
{
    return sg_serialize_multidatagram( dtg, 1, data, max_len, len );
}

/*
 * \param dtg datagram struct
 * \param part the part of the packet in the datagram
 * \param data the serialized output data
 * \param max_len the maximum length of output data
 * \param len serialized length of data
 */
WD_status_t sg_serialize_multidatagram( SG_WD_PDU_t *dtg, const uint8_t part,
                                        uint8_t *data, const uint16_t max_len,
                                        uint16_t *len )
{
    uint8_t index = 0U; /* Index. */
    uint16_t data_offset_index = 0U; /* Base data index. */
    uint8_t data_size = 0U; /* Data size. */
    uint16_t data_crc = 0xFFFF; /* CRC for data. */

    /* Validate input parameters. */
    if ( ( ( (void*) 0UL ) == dtg )
            || ( ( (void*) 0UL ) == data ) || ( ( (void*) 0UL ) == len ) )
    {
        return SG_WD_ERROR_INVALID_PARAMTERS;
    }

    /* Calculate CRC and length at this point to set it at the header. */
    if ( ( 0U != dtg->data_len ) && ( ( (void*) 0UL ) != dtg->data_ptr ) )
    {
        /* Calculate the base data index and data size. */
        data_offset_index = max_len * ( part - 1 );
        data_size =
                ( ( data_offset_index + max_len ) <= dtg->data_len ) ?
                        max_len : dtg->data_len - data_offset_index;

        /* Calculate the data CRC. */
        data_crc = calculate_crc16_ccitt_false(
                &dtg->data_ptr[data_offset_index], data_size );
    }

    /* Start of package for datastream package . */
    if ( (( index + 1 ) <= max_len ) && ( dtg->wd_start != 0 ) )
    {
        data[index++] = dtg->wd_start;
    }
    else
    {
        return SG_WD_ERROR_INVALID_OUTPUT_DATA_SIZE;
    }

    /* Header size. */
    if ( ( index + 1 ) <= max_len )
    {
        data[index++] = dtg->header.wd_hl;
    }
    else
    {
        return SG_WD_ERROR_INVALID_OUTPUT_DATA_SIZE;
    }

    /* Set the header. */
    if ( dtg->header.wd_hl > 0 )
    {
        /* Check the valid header data. */
        if ( ( dtg->header.ie_count != 0 ) && ( dtg->header.ie_ptr != 0 ) )
        {
            for ( uint8_t i = 0; i < dtg->header.ie_count; i++ )
            {
                SG_IE_t *ie = &dtg->header.ie_ptr[i];

                if ( ie != 0 )
                {
                    if ( ( index + 5 ) <= max_len )
                    {
                        data[index++] = (uint8_t) ie->ie_i; /* IE_I */
                        data[index++] = ie->ie_l; /* IE_L */
                        switch ( ie->ie_i )
                        {
                            case SG_WD_IE_ID_MULTI_DTG:

                                /* Set the datagram number. */
                                ie->ie_d.dtg.dtg_nr = part;

								/* IE_D.DTG_REF (7b) | IE_D.DTG_AR (1b) */
                                data[index++] = ( ie->ie_d.dtg.dtg_ref << 1 )
                                        | ( ie->ie_d.dtg.dtg_ar & 0x1 ); 
								/* IE_D.DTG_TC (1B) */
                                data[index++] = ie->ie_d.dtg.dtg_tc; 
                                /* IE_D.DTG_NR (1B) */
                                data[index++] = ie->ie_d.dtg.dtg_nr; 
                                break;

                            case SG_WD_IE_ID_DETAILS_DTG:

                                /* Set the data CRC. */
                                ie->ie_d.dd.dd_len = data_size;
                                data[index++] = ie->ie_d.dd.dd_len; /* IE_D.DD_LEN (1B) */

                                /* Set the data CRC. */
                                ie->ie_d.dd.dd_crc = data_crc;

								/* IE_D.DD_LEN (2B:Lo) */
                                data[index++] = (uint8_t) ( ie->ie_d.dd.dd_crc & 0xFF ); 
                                /* IE_D.DD_LEN (2B:Hi) */
                                data[index++] = (uint8_t) ( ie->ie_d.dd.dd_crc >> 8 );
                                break;

                            case SG_WD_IE_ID_MFC_DTG_LOW:
                            case SG_WD_IE_ID_MFC_DTG_HIGH:
                            default:
                                return SG_WD_ERROR_INVALID_HEADER_ELEMENT_TYPE;
                        }
                    }
                    else
                    {
                        return SG_WD_ERROR_INVALID_OUTPUT_DATA_SIZE;
                    }
                }
                else
                {
                    return SG_WD_ERROR_INVALID_HEADER_ELEMENT_DATA;
                }
            }
        }
        else
        {
            return SG_WD_ERROR_INVALID_HEADER_DATA;
        }
    }

    /* Set the data. */

    /* Check the valid of data. */
    if ( ( dtg->data_len != 0 ) && ( dtg->data_ptr != (void*) 0UL ) )
    {
        const uint8_t data_begin_index = index; /* Data start index. */

        /* Copy data. */
        while ( ( index - data_begin_index ) < data_size )
        {
            data[index] = dtg->data_ptr[data_offset_index + index
                    - data_begin_index];
            index++;
        }
    }

    *len = index;

    return SG_WD_OK;
}

WD_status_t sg_deserialize_datagram( const uint8_t *data, const uint16_t len,
                                     SG_WD_PDU_t *dtg )
{
    uint8_t index = 0U; /* Index. */

    /* Validate input parameters. */
    if ( ( (void*) 0UL == data ) || ( 0U == len ) || ( (void*) 0UL == dtg ) )
    {
        return SG_WD_ERROR_INVALID_PARAMTERS;
    }

    /* Header size. */
    if ( ( index + 1U ) <= len )
    {
        dtg->header.wd_hl = data[index++];
    }
    else
    {
        return SG_WD_ERROR_INVALID_OUTPUT_DATA_SIZE;
    }

    /* Set the header. */
    if ( dtg->header.wd_hl > 0 )
    {
        if ( ( index + dtg->header.wd_hl ) <= len )
        {
            for ( int i = 0;
                    ( index < len ) && ( i < dtg->header.ie_max_count ); i++ )
            {
                SG_IE_t *ie = &dtg->header.ie_ptr[i];
                dtg->header.ie_count = i + 1;

                if ( ie != 0 )
                {
                    ie->ie_i = data[index++]; /* IE_I */
                    ie->ie_l = data[index++]; /* IE_L */

                    if ( ( index + ie->ie_l ) <= len )
                    {
                        switch ( ie->ie_i )
                        {
                            case SG_WD_IE_ID_MULTI_DTG:
                                ie->ie_d.dtg.dtg_ref = data[index] >> 1; /* IE_D.DTG_REF (7b) */
                                ie->ie_d.dtg.dtg_ar = data[index++] & 0x1; /* IE_D.DTG_AR (1b) */
                                ie->ie_d.dtg.dtg_tc = data[index++]; /* IE_D.DTG_TC (1B) */
                                ie->ie_d.dtg.dtg_nr = data[index++]; /* IE_D.DTG_NR (1B) */
                                break;

                            case SG_WD_IE_ID_DETAILS_DTG:
                            {
                                uint16_t crc;
                                ie->ie_d.dd.dd_len = data[index++]; /* IE_D.DD_LEN (1B) */
                                crc = (uint8_t) data[index++]; /* IE_D.DD_LEN (2B:Lo) */
                                crc |= (uint8_t) data[index++] << 8; /* IE_D.DD_LEN (2B:Hi) */
                                ie->ie_d.dd.dd_crc = crc;
                            }
                                break;

                            case SG_WD_IE_ID_MFC_DTG_LOW:
                            case SG_WD_IE_ID_MFC_DTG_HIGH:
                            default:
                                return SG_WD_ERROR_INVALID_HEADER_ELEMENT_TYPE;
                        }
                    }
                }
                else
                {
                    return SG_WD_ERROR_INVALID_HEADER_ELEMENT_SIZE;
                }
            } /* end of for ie */
        }
        else
        {
            return SG_WD_ERROR_INVALID_HEADER_SIZE;
        }
    }

    /* Set the data. */

    /* Check the valid of data. */
    if ( ( dtg->data_max_len != 0 ) && ( dtg->data_ptr != 0 ) )
    {
        const uint8_t data_begin_index = index; /* Data start index. */
        uint16_t data_len = 0;

        /* Copy data. */
        while ( ( index < len ) && ( data_len < dtg->data_max_len ) )
        {
            dtg->data_ptr[data_len] = data[index];
            index++;
            data_len = index - data_begin_index;
            dtg->data_len = data_len;
        }
    }

    return SG_WD_OK;
}

/*** end of file ***/

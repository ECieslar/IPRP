/*!****************************************************************************
 *
 * \file gm_comm.c
 * \brief Gas-Meter (MCH board) communication implementation.
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
#include "gm_comm.h"
#include "../hw/mkg_hw.h"
#include "../util/str_util.h"

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/* General modem's constants. */
enum Gas_Meter_general
{
	GM_UART_WRITE_DELAY  =  50U, /*!< GM write delay. */
	GM_UART_READ_TIMEOUT = 200U, /*!< GM read timeout. */
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

/*!
 * \brief Get a forward frame's number.
 */
static uint8_t get_frame_counter();

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/

GM_status_t gm_send_frame( GM_frame_t* frame )
{
	uint8_t buf[sizeof (GM_frame_t)];
	uint16_t tx_size;

	/* Set the frame's beginning tag. */
	buf[0] = GM_FRAME_BEGIN;

	/* Set the frame's options. */
	buf[1] = (uint8_t) ( ( frame->options.sender << 7 )
			| ( frame->options.encypt << 6 )
			| ( ( frame->options.reserved & 0x03 ) << 4  )
			| ( frame->options.proto_ver ) );

	/* Set the frame's counter. */
	frame->counter = get_frame_counter();
	buf[2] = frame->counter;

	/* Set the frame data length. */
	buf[3] = frame->data_len > GM_FRAME_DATA_LENGTH ? GM_FRAME_DATA_LENGTH : frame->data_len;

	/* Set the frame's function. */
	buf[4] = frame->func;

	/* Set the frame's data. */
	if ( frame->data_len > 0 )
	{
		memcpy( &buf[5], frame->data, frame->data_len );
	}

	/* Set the frame's checksum. */
	frame->crc32 = hw_get_crc32( &buf[1], 4 + frame->data_len );

	buf[5 + frame->data_len] = (uint8_t) ( frame->crc32 >> 24 );
	buf[6 + frame->data_len] = (uint8_t) ( frame->crc32 >> 16 );
	buf[7 + frame->data_len] = (uint8_t) ( frame->crc32 >> 8 );
	buf[8 + frame->data_len] = (uint8_t) ( frame->crc32 );

	/* Set the frame's ending tag. */
	buf[9 + frame->data_len] = GM_FRAME_END;

	/* Transmit the frame to the UART port. */
	tx_size = hw_transmit_by_uart( GAS_METER_UART, buf, GM_FRAME_SIZE_MIN + frame->data_len ) ;

#ifndef DISABLE_GM_COMM_DEBUG
        char s[2 * sizeof (GM_frame_t) + 1];
        str_bin_to_ascii( buf, s, tx_size );
        printf( "Tx: %d %s ", tx_size, s );
#endif /* DISABLE_GM_COMM_DEBUG */

	if ( ( GM_FRAME_SIZE_MIN + frame->data_len ) == tx_size )
	{
		return GM_OK;
	}

	return GM_COMM_WRITE_ERROR;
}

GM_status_t gm_receive_frame( GM_frame_t* frame )
{
	GM_status_t status;
	uint8_t buf[sizeof (GM_frame_t)];
	uint16_t read_data_len;

	/* Check the input parameters. */
	if ( ( (void*) 0UL ) == frame )
	{
		return GM_INVALID_PARAMETER_ERROR;
	}

	/* Get the frame from the port. */
	read_data_len = hw_receive_from_uart( GAS_METER_UART, buf, sizeof (GM_frame_t), 50UL );

	/* Check for any read data. */
	if ( 0U == read_data_len )
    {
        status = GM_COMM_READ_ERROR;
    }
	else /* read_data_len > 0 */
	{
#ifndef DISABLE_GM_COMM_DEBUG
        char s[2 * sizeof (GM_frame_t) + 1];
        str_bin_to_ascii( buf, s, read_data_len );
        printf( "Rx: %d %s ", read_data_len, s );
#endif /* DISABLE_GM_COMM_DEBUG */

	    uint16_t index = 0;

		/* Find the frame beginning tag. */
		while ( ( buf[index] != GM_FRAME_BEGIN )
				&& ( index++ < read_data_len ) ) {};

		/* Check the frame beginning tag. */
		if ( GM_FRAME_BEGIN != buf[index] )
        {
            status = GM_COMM_FRAME_FORMAT_ERROR;
        }
		else /* GM_FRAME_BEGIN at buf[index] */
		{
			if ( ( index + GM_FRAME_SIZE_MIN ) > read_data_len )
            {
                status = GM_COMM_FRAME_LENGTH_ERROR;
            }
			else /* Frame size ok */
			{
			    /* Get options field. */
				frame->options.sender = (uint8_t) ( buf[index + 1] >> 7 );
				frame->options.encypt = (uint8_t) ( ( buf[index + 1] >> 6 ) & 0x1 );
				frame->options.reserved = (uint8_t) ( ( buf[index + 1] >> 4 ) & 0x2 );
				frame->options.proto_ver = (uint8_t) ( buf[index + 1] & 0x7 );

				/* Get the frame's number. */
				frame->counter = buf[index + 2];

				/* Get the data length. */
				frame->data_len = buf[index + 3];

				/* Get the function. */
				frame->func = buf[index + 4];

				/* Check if all data are in read data. */
				if ( ( index + GM_FRAME_SIZE_MIN + frame->data_len ) > read_data_len )
                {
                    status = GM_COMM_FRAME_DATA_LENGTH_ERROR;
                }
				else /* length ok */
				{
				    /* Get the frame's data fields. */
					memcpy( frame->data, &buf[index + 5], frame->data_len );

					/* Calculate the checksum. */
					const uint32_t crc = hw_get_crc32( &buf[index + 1], 4 + frame->data_len );

					/* Get the frame's checksum. */
					frame->crc32 = (uint32_t) ( buf[index + 5 + frame->data_len] << 24 )
									| ( buf[index + 6 + frame->data_len] << 16)
									| ( buf[index + 7 + frame->data_len] << 8)
									| ( buf[index + 8 + frame->data_len] );

					/* Check the checksum. */
					if ( crc != frame->crc32 )
                    {
                        status = GM_COMM_FRAME_CRC_ERROR;
                    }
					else
					{
						if ( GM_FRAME_END == buf[index + GM_FRAME_SIZE_MIN + frame->data_len - 1] )
						{
							status = GM_OK;
						}
						else
						{
							status = GM_COMM_FRAME_FORMAT_ERROR;
						}
					}
				}
			}
		}
	}

	return status;
}

void gm_print_frame( const GM_frame_t* frame )
{
    if ( ( (void*) 0UL ) == frame )
    {
        printf( "<null>" );

        return;
    }

    printf("FRAME={OPTIONS=(sender=%d,encrypt=%d,reserved=%d,proto_ver=%d),counter=%d,data_length=%d,function=0x%02X,",
                   frame->options.sender, frame->options.encypt,
                   frame->options.reserved, frame->options.proto_ver,
                   frame->counter, frame->data_len, frame->func );

    char s[GM_FRAME_DATA_LENGTH];
    str_bin_to_ascii( frame->data, s, frame->data_len );

    printf( "data=%s,crc32=0x%04lX}", s, (unsigned long) frame->crc32 );
}

static uint8_t get_frame_counter()
{
	static uint8_t g_frame_counter = 0;

	return g_frame_counter++;
}

/*** end of file ***/

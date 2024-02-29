/*!****************************************************************************
*
* \file sg_wp.c
* \brief SMART-GAS packet layer implementation.
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
#include <stdint.h> /* Declarations of integer types with specified width. */
#include <string.h> /* memcpy(). */
#include <stdio.h> /* printf() */
#include "sg_wp.h" /* The layer declarations. */
#include "sg_proto.h"
#include "../util/crc.h" /* The crc functions. */
#include "aes/aes.h"

/******************************************************************************
* Defines and Enumerations
******************************************************************************/

#define COMPL_TO_16(x) (x % 16U ? ( x / 16U ) + 1U : x / 16U)

/******************************************************************************
* Local Types and Typedefs
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/
/*!
 * \brief Global SMART-GAS packet layer's parameters.
 *
 * \note There are some fields in the structure
 *       that needs to be stored in a secure location.
 */
SG_WP_Params_t sg_wp_params = { .wpp_dlk = SG_LINK_SMS,
                                .wpp_dad = { "\x00\x00\x00\x00\x00\x00\x00\x00" } };


/******************************************************************************
* Static Function Prototypes
******************************************************************************/

/*!
 * \brief Convert WP_CTRL field to byte.
 * \param ctrl control field
 * \return byte representation of ontrol field.
 */
uint8_t wp_ctrl_to_byte(const SG_WP_CTRL_t ctrl);

void uint16_to_le_bytes( const uint16_t ui16, uint8_t *bytes );
void uint32_to_le_bytes( const uint32_t ui32, uint8_t *bytes );

/******************************************************************************
* Static Variables
******************************************************************************/

/******************************************************************************
* Functions
******************************************************************************/

/*
 * Wg [SG:2018] pkt 4.2 "Rejestrator  tworząc  nową  sesję  podstawia  do  WP_SESID
 * wartość WPP_SESID_OUT, po czym inkrementuje (modulo 128) wartość WPP_SESID_OUT",
 * jednak wygodniej i bardziej czytelnie jest inkrementacja sesji identyfikatora
 * sesji przed jest rozpoczęciem niż po.
 */
void create_new_session( void )
{
    uint8_t s = sg_wp_params.wpp_sesid_out;

    sg_wp_params.wpp_sesid_out = ++s % 128;
}

uint8_t get_packet_session_id()
{
    return sg_wp_params.wpp_sesid_out;
}

/* \return The size of the packet's header or zero. */
uint8_t get_packet_header_size( const SG_WP_HDR_t* hdr )
{
    uint8_t len = 0UL;

    if ( ((void*) 0UL) != hdr )
    {
        /* XXX Check out the real size of structures returned by sizeof */

        /* Add the header size (should be 29 = 32 - 3 for padding). */
        len = sizeof (SG_WP_HDR_t) - 2;

        /* Odd the control fields dependencies in the header. */

        /* Check if serial numer isn't set. */
        if ( 0U == hdr->crtl.ctrl_sn )
        {
            /* header doesn't contain WP_SN field (should be 4 less). */
            len -= sizeof hdr->wp_sn;
        }
        /* Check if cryptography isn't  set. */
        if ( 0U == hdr->crtl.ctrl_enc )
        {
            /* header doesn't contain WP_MAC and WP_IV field (should be 8 less). */
            len -= sizeof hdr->wp_mac;
            len -= sizeof hdr->wp_iv;
        }
        /* Check if return address isn't set. */
        if ( 0U == hdr->crtl.ctrl_rad )
        {
            /* header doesn't contain WP_RAD (should be 4 less). */
            len -= sizeof hdr->wp_rad / sizeof hdr->wp_rad[0];
        }
        /* Check if access control isn't set. */
        if ( 0U == hdr->crtl.ctrl_acc )
        {
            /* header doesn't contain WP_ACC_LVL and WP_ACC_PASS (should be 5 less). */
            len -= sizeof hdr->wp_acclvl;
            len -= sizeof hdr->wp_accpass;
        }
    }

    return len;
}

/* \return The size of the packet or zero. */
uint16_t get_packet_size( const SG_WP_PDU_t* pkt )
{
    uint16_t len = 0UL;

    if ( ((void*) 0UL) != pkt )
    {
        /* Get the header size with WP_LEN (2) and WP_CTRL (1) */
        len = get_packet_header_size( &pkt->header );

        /* Add the data length (should not exceed SG_WP_DATA_MAX_LEN). */
        len += pkt->data_len;

        /* Add the data length (shoule be 2). */
        len += sizeof pkt->crc;
    }

    return len;
}

SG_WP_ERR_t serialize_packet( const SG_WP_PDU_t* pkt, uint8_t* buff,
                              const uint16_t buff_size, uint16_t* pkt_size )
{
    uint16_t len = 0UL;

    /* Validate the input parameters. */
    if ( ( ((void*) 0UL) == pkt )
            || ( ((void*) 0UL) == buff )
            || ( ((void*) 0UL) == pkt_size ) )
    {
        return SG_WP_ERR_BAD_INPUT_PARAM;
    }

    /* Check for a room for the packet's header. */
    if ( buff_size < sizeof (SG_WP_HDR_t) )
    {
        return SG_WP_ERR_BAD_OUPUT_BUF_SIZE;
    }

    /* Output buffer size is sufficient. */
    /* XXX How the bits will be ordered? */
    buff[len++] = ( pkt->header.crtl.ctrl_ver << 7 )
            | ( pkt->header.crtl.ctrl_sn << 6 )
            | ( pkt->header.crtl.ctrl_enc << 5 )
            | ( pkt->header.crtl.ctrl_rad << 4 )
            | ( pkt->header.crtl.ctrl_rlk << 2 )
            | ( pkt->header.crtl.ctrl_dir << 1 )
            | ( pkt->header.crtl.ctrl_acc << 0 );

    /* WP_LEN (2B) */
    buff[len++] = (uint8_t) ( pkt->header.wp_len & 0xFF );
    buff[len++] = (uint8_t) ( pkt->header.wp_len >> 8 );

    /* Get into account the control field's dependencies in the header. */

    /* Check if serial numer is set. */
    if ( pkt->header.crtl.ctrl_sn )
    {
        /* WP_HDR contains WP_SN (should be 4 bytes). */
        buff[len++] = (uint8_t) (pkt->header.wp_sn);
        buff[len++] = (uint8_t) (pkt->header.wp_sn >> 8);
        buff[len++] = (uint8_t) (pkt->header.wp_sn >> 16);
        buff[len++] = (uint8_t) (pkt->header.wp_sn >> 24);
    }
    /* Check if cryptography is set. */
    if ( pkt->header.crtl.ctrl_enc )
    {
        /* WP_HDR contains WP_MAC and WP_IV (should be 8 less). */
        buff[len++] = (uint8_t) (pkt->header.wp_mac);
        buff[len++] = (uint8_t) (pkt->header.wp_mac >> 8);
        buff[len++] = (uint8_t) (pkt->header.wp_mac >> 16);
        buff[len++] = (uint8_t) (pkt->header.wp_mac >> 24);
        buff[len++] = (uint8_t) (pkt->header.wp_iv);
        buff[len++] = (uint8_t) (pkt->header.wp_iv >> 8);
        buff[len++] = (uint8_t) (pkt->header.wp_iv >> 16);
        buff[len++] = (uint8_t) (pkt->header.wp_iv >> 24);
    }
    /* Check if return address is  set. */
    if ( pkt->header.crtl.ctrl_rad )
    {
        /* WP_RAD */
        if ( SG_LINK_SMS == pkt->header.crtl.ctrl_rlk )
        {
            /* for SMS set 8 bytes */
            for ( uint8_t i = 0U;
                    ( i < 8U ) && ( i < ( sizeof pkt->header.wp_rad / sizeof pkt->header.wp_rad[0]) );
                    i++ )
            {
                buff[len++] = (uint8_t) pkt->header.wp_rad[i];
            }
        }
        else if ( ( SG_LINK_GPRS_UDP == pkt->header.crtl.ctrl_rlk )
                 || ( SG_LINK_GPRS_TCP == pkt->header.crtl.ctrl_rlk ) )
        {
            /* for GPRS set 6 bytes */
            for ( uint8_t i = 0U;
                    ( i < 6U ) && (i < ( sizeof pkt->header.wp_rad / sizeof pkt->header.wp_rad[0]) );
                    i++ )
            {
                buff[len++] = (uint8_t) pkt->header.wp_rad[i];
            }
        }
        else /* XXX Other values are not supported. */
        {
        }
    }

    /* WP_SESO and WP_SESID */
    buff[len++] = (uint8_t) ( pkt->header.wp_seso << 7 ) | pkt->header.wp_sesid;

    /* Check if access control i set. */
    if ( pkt->header.crtl.ctrl_acc )
    {
        /* WP_HDR contains WP_ACC_LVL and WP_ACC_PASS. */
        buff[len++] = pkt->header.wp_acclvl;
        buff[len++] = (uint8_t) pkt->header.wp_accpass ;
        buff[len++] = (uint8_t) (pkt->header.wp_accpass << 8);
        buff[len++] = (uint8_t) (pkt->header.wp_accpass << 16);
        buff[len++] = (uint8_t) (pkt->header.wp_accpass << 24);
    }

    /* Check for a room for the packet's data. */
    if ( buff_size < ( len + pkt->data_len ) )
    {
        return SG_WP_ERR_BAD_OUPUT_BUF_SIZE;
    }
    else /* Output buffer size is sufficient. */
    {
        /* Copy the data . */
        memcpy( &buff[len], pkt->data_ptr, pkt->data_len );
        len += pkt->data_len;
    }

    /* Check for a room for the packet's data. */
    if ( buff_size < ( len + sizeof pkt->crc ) )
    {
        return SG_WP_ERR_BAD_OUPUT_BUF_SIZE;
    }
    else /* Output buffer size is sufficient. */
    {
        uint16_t crc = calculate_crc16_ccitt_false( buff, len );
        buff[len++] = (uint8_t) crc;
        buff[len++] = crc >> 8;
    }

    *pkt_size = len;

    return SG_WP_OK;
}

SG_WP_ERR_t deserialize_packet( const uint8_t* buff, const uint16_t buff_len,
                                SG_WP_PDU_t* pkt )
{
    uint16_t len = 0UL;

    /* Validate the input parameters. */
    if ( ( ((void*) 0UL) == buff )
            || ( ((void*) 0UL) == pkt ) )
    {
        return SG_WP_ERR_BAD_INPUT_PARAM;
    }

    /* Check for a room for the packet's header. */
    if ( buff_len < sizeof (SG_WP_HDR_t) )
    {
        return SG_WP_ERR_BAD_OUPUT_BUF_SIZE;
    }

    /* Decode WP_CTRL (1B) */
    pkt->header.crtl.ctrl_ver = buff[len] << 7;
    pkt->header.crtl.ctrl_sn  = buff[len] << 6;
    pkt->header.crtl.ctrl_enc = buff[len] << 5;
    pkt->header.crtl.ctrl_rad = buff[len] << 4;
    pkt->header.crtl.ctrl_rlk = buff[len] << 2;
    pkt->header.crtl.ctrl_dir = buff[len] << 1;
    pkt->header.crtl.ctrl_acc = buff[len] << 0;
    len += 1;

    /* Decode WP_LEN (2B) */
    pkt->header.wp_len = (uint16_t) ( buff[len] | ( buff[len + 1] << 8 ) );
    len += 2;

    /* Get into account the control field's dependencies in the header. */

    /* Check if serial numer is set. */
    if ( pkt->header.crtl.ctrl_sn )
    {
        /* WP_HDR contains WP_SN (should be 4 bytes). */
        pkt->header.wp_sn = (uint32_t) ( buff[len] << 24 )
                            | ( buff[len + 1] << 16 )
                            | ( buff[len + 2] << 8 )
                            | ( buff[len + 3] );
        len += 4;
    }
    /* Check if cryptography is set. */
    if ( pkt->header.crtl.ctrl_enc )
    {
        /* WP_HDR contains WP_MAC and WP_IV (should be 8 less). */
        pkt->header.wp_mac = (uint32_t) ( buff[len] << 24 )
                                    | ( buff[len + 1] << 16 )
                                    | ( buff[len + 2] << 8 )
                                    | ( buff[len + 3] );
        len += 4;
        pkt->header.wp_iv = (uint32_t) ( buff[len] << 24 )
                                    | ( buff[len + 1] << 16 )
                                    | ( buff[len + 2] << 8 )
                                    | ( buff[len + 3] );
        len += 4;
    }
    /* Check if return address is set. */
    if ( pkt->header.crtl.ctrl_rad )
    {
        /* WP_RAD */
        if ( SG_LINK_SMS == pkt->header.crtl.ctrl_rlk )
        {
            /* for SMS set 8 bytes */
            for ( uint8_t i = 0;
                    ( i < 8 ) && (i < ( sizeof pkt->header.wp_rad / sizeof pkt->header.wp_rad[0]) );
                    i++ )
            {
                pkt->header.wp_rad[i] = buff[len++];
            }
        }
        else if ( ( SG_LINK_GPRS_UDP == pkt->header.crtl.ctrl_rlk )
                 || ( SG_LINK_GPRS_TCP == pkt->header.crtl.ctrl_rlk ) )
        {
            /* for GPRS set 6 bytes */
            for ( uint8_t i = 0;
                    ( i < 6 ) && (i < ( sizeof pkt->header.wp_rad / sizeof pkt->header.wp_rad[0]) );
                    i++ )
            {
                pkt->header.wp_rad[i] = buff[len++];
            }
        }
        else /* XXX Other values are not supported. */
        {
        }
    }

    /* WP_SESO and WP_SESID */
    pkt->header.wp_seso  = buff[len] >> 7;
    pkt->header.wp_sesid = buff[len] & 0x7F;
    len += 1;

    /* Check if access control is set. */
    if ( pkt->header.crtl.ctrl_acc )
    {
        /* WP_HDR contains WP_ACC_LVL and WP_ACC_PASS. */
        pkt->header.wp_acclvl = buff[len++];
        pkt->header.wp_accpass = (uint8_t) ( buff[len] << 24 )
                                    | ( buff[len] << 16 )
                                    | ( buff[len] << 8 )
                                    | ( buff[len] );
        len += 4;
    }

    /* Check for a room for the packet's data. */
    if ( buff_len < ( len + pkt->data_len ) )
    {
        return SG_WP_ERR_BAD_OUPUT_BUF_SIZE;
    }
    else /* Output buffer size is sufficient. */
    {
        /* Copy the data . */
        memcpy( pkt->data_ptr, &buff[len], pkt->data_len );
        len += pkt->data_len;
    }

    /* CRC (2B) */
    pkt->crc = (uint16_t) ( buff[len] | ( buff[len + 1] << 8 ) );

    return SG_WP_OK;
}

SG_WP_ERR_t encrypt_packet( SG_WP_PDU_t* pkt, const uint8_t* key )
{
    /* Sprawdzenie znacznika szyfrowania. */
    if ( 1 != pkt->header.crtl.ctrl_enc )
    {
        return 1 /* SG_WP_ENCRYPTION_ERROR */;
    }

    /* Szyfrowanie danych. */

    /* Wyodrębnij cześć do zaszyfrowania (od RAD, przez SESO/SESID do DATA włączenie). */
    uint16_t pkt_len = pkt->data_len + get_packet_header_size(&pkt->header);
    /* Bufor manipulacyjny o rozmiarze pakietu dopełnionym do modulo 16. */
    uint16_t buff_size = COMPL_TO_16(pkt_len) * 16;
    uint8_t buff[buff_size];
    uint16_t buff_len = 0U;

    printf("\npacket len=%d, buffer size=%d", pkt_len, buff_size );

    memcpy( buff, pkt->header.wp_rad, sizeof pkt->header.wp_rad );
    buff_len += sizeof pkt->header.wp_rad;
    buff[buff_len] = ( pkt->header.wp_seso << 7 ) | pkt->header.wp_sesid;
    buff_len += 1;
    memcpy( &buff[buff_len], pkt->data_ptr, pkt->data_len );
    buff_len += pkt->data_len;
    /* Wyznacz liczbę bloków szyfrujących (16-bitowe). */
    uint8_t enc_block_count = COMPL_TO_16( buff_len );
    /* Przygotuj bloki szyfrujące. */
    uint8_t enc_blocks[enc_block_count * 16U];
    for ( uint8_t i = 0U; i < enc_block_count; i++)
    {
        /* WP_CTRL */
        enc_blocks[i*16+0] = wp_ctrl_to_byte(pkt->header.crtl);
        /* WP_LEN (LEN) */
        uint16_to_le_bytes( pkt->header.wp_len, &enc_blocks[i*16+1] );
        /* WP_SN (LE) */
        uint32_to_le_bytes( pkt->header.wp_sn, &enc_blocks[i*16+3] );
        /* WP_IV (LE) */
        uint32_to_le_bytes( pkt->header.wp_iv, &enc_blocks[i*16+7] );
        /* WP_UNUSED */
        memset( &enc_blocks[i*16+11], 0U, 3U );
        /* BLOCK_NR (LE) */
        uint16_to_le_bytes( i, &enc_blocks[i*16+14] );

        printf("\nin_block[%u]=", i);
        for (uint8_t j = 0U; j < 16U; j++)
        {
            printf("%02x", enc_blocks[i*16+j]);
        }

        /* Zaszyfruj bloki. */
        aes_encrypt( &enc_blocks[i*16], key );

        printf("\nen_block[%u]=", i);
        for (uint8_t j = 0; j < 16; j++)
        {
            printf("%02x", enc_blocks[i*16+j]);
        }
    }
    printf("\norg_part=");
    for ( uint8_t i = 0U; i < buff_len; i++)
    {
        printf("%02x",buff[i]);
    }
    printf("\nenc_part=");

    /* Połącz część do zaszyfrowania z blokami szyfrującymi. */
    for ( uint8_t i = 0U; i < buff_len; i++)
    {
        buff[i] ^= enc_blocks[i];
        printf("%02x",buff[i]);
    }
    printf("\n\n");

    /* Przepisz zaszyfrowaną część do źródłowych pól (od RAD, przez SESO/SESID do DATA włączenie). */
    memcpy( pkt->header.wp_rad, buff, sizeof pkt->header.wp_rad );
    pkt->header.wp_seso  = buff[sizeof pkt->header.wp_rad] >> 7;
    pkt->header.wp_sesid = buff[sizeof pkt->header.wp_rad] & 0x7F;
    memcpy( pkt->data_ptr, &buff[sizeof pkt->header.wp_rad + 1], pkt->data_len );

    /* Wyznaczenie części uwierzytelniającej (WP_MAC). */

    /* Ustaw zera w nowym kluczu uwierzytelniającym. */
    uint8_t mac_key[16];
    memset( mac_key, 0U, sizeof mac_key );
    aes_encrypt( mac_key, key );

    printf("\nmac_key=");
    for ( uint8_t i = 0U; i < sizeof mac_key; i++)
    {
        printf("%02x", mac_key[i]);
    }

    /* Przygotuj kompletny pakiet. */
    printf("\nserial=%d, len=%d", serialize_packet( pkt, buff, sizeof buff, &buff_len ), buff_len );

    /* Usuń crc z bufora. */
    buff_len -= 2;

    printf("\npacket=");
    for ( uint8_t i = 0U; i < buff_len; i++)
    {
        printf("%02x", buff[i]);
    }

    /* Oblicz liczbę bloków szyfrujących dla ciągu uwierzytleniającego. */
    enc_block_count = COMPL_TO_16( buff_len );

    /* Dopełnij bufor zerami do pełnego bloku. */
    memset( &buff[buff_len], 0U, enc_block_count * 16 - buff_len );

    printf("\ncompl_packet=");
    for ( uint8_t i = 0U; i < enc_block_count * 16; i++)
    {
        printf("%02x", buff[i]);
    }

    /* Oblicz ciąg uwierzytelniający. */
    uint8_t block[16];

    /* Block 1. */
    memcpy(block, buff, 16);

    for ( uint8_t i = 0U; i < enc_block_count; i++ )
    {
        printf("\nblki[%d]=", i );
        for ( uint8_t j = 0U; j < sizeof block; j++ )
        {
            printf( "%02x", block[j]);
        }

        aes_encrypt( block, mac_key );

        printf("\nblko[%d]=", i);
        for ( uint8_t j = 0U; j < sizeof block; j++ )
        {
            printf("%02x", block[j]);
        }

        if ( i < (enc_block_count - 1) )
        {
            printf("\nblkn[%d]=", i);
            for ( uint8_t j = 0U; j < sizeof block; j++ )
            {
                printf("%02x", buff[(i + 1) * 16 + j]);
            }

            for (uint8_t j = 0U; j < 16; j++)
            {
                block[j] ^= buff[(i + 1) * 16 + j];
            }

            printf("\nblkr[%d]=", i);
            for ( uint8_t j = 0U; j < sizeof block; j++ )
            {
                printf("%02x", block[j]);
            }
        }
    }

    uint8_t* mac_ptr = block;
    uint32_t new_mac = ( *mac_ptr ) | ( *(mac_ptr + 1) << 8 ) | ( *(mac_ptr + 2) << 16 ) | ( *(mac_ptr + 3) << 24 );
    pkt->header.wp_mac = new_mac;

    printf("\nmac=%02x-%02x-%02x-%02x", (uint8_t) (new_mac >> 24), (uint8_t) (new_mac >> 16), (uint8_t) (new_mac >> 8), (uint8_t) (new_mac) );

    /* Oblicz CRC */
    uint16_t crc = calculate_crc16_ccitt_false( buff, buff_len );
    pkt->crc =crc;

    return SG_WP_OK;
}

uint8_t wp_ctrl_to_byte( const SG_WP_CTRL_t ctrl )
{
    return ( ( ctrl.ctrl_ver << 7 )
                | ( ctrl.ctrl_sn << 6 )
                | ( ctrl.ctrl_enc << 5 )
                | ( ctrl.ctrl_rad << 4 )
                | ( ctrl.ctrl_rlk << 2 )
                | ( ctrl.ctrl_dir << 1 )
                | ( ctrl.ctrl_acc << 0 ) );
}

void uint16_to_le_bytes( const uint16_t ui16, uint8_t *bytes )
{
    if ( ((void*) (0UL)) != bytes)
    {
        bytes[0] = (uint8_t) ui16; /* low */
        bytes[1] = (uint8_t) ( ui16 >> 8 ); /* high */
    }
}

void uint32_to_le_bytes( const uint32_t ui32, uint8_t *bytes )
{
    if ( ((void*) (0UL)) != bytes)
    {
        bytes[0] = (uint8_t) ui32; /* low */
        bytes[1] = (uint8_t) ( ui32 >> 8 );
        bytes[2] = (uint8_t) ( ui32 >> 16 );
        bytes[3] = (uint8_t) ( ui32 >> 24 ); /* high */
    }
}

/*** end of file ***/

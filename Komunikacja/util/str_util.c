/*!****************************************************************************
 *
 * \file str_util.c
 * \brief Miscellaneous string conversion functions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 265212 $
 * \date $Date: 2020-01-17 15:30:28 +0100 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "str_util.h"

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

/* Converts a single byte to ASCII (2 bytes) representation. */
static void bin_to_ascii( const uint8_t s, char *t );

/* Converts double ASCII characters to a single binary byte representation. */
static void ascii_to_bin( const char *s, uint8_t *t );

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/******************************************************************************
 * Functions
 ******************************************************************************/

/* Converts a byte stream s of length len to ASCII t. */
void str_bin_to_ascii( const uint8_t *s, char *t, const uint8_t len )
{
    for ( uint8_t i = 0; i < len; i++ )
    {
        bin_to_ascii( s[i], &t[2 * i] );
    }

    t[2 * len] = '\0';
}

/* Converts a byte stream s of length len to ASCII t. */
void str_ascii_to_bin( const char *s, uint8_t *t, const uint8_t len )
{
    /* Check the length of ASCII string is even. */
    if ( ( len % 2 ) != 0 )
    {
        return;
    }

    for ( uint8_t i = 0; i < len / 2; i++ )
    {
        ascii_to_bin( &s[i * 2], &t[i] );
    }
}

static void bin_to_ascii( const uint8_t s, char *t )
{
    const uint8_t sh = s >> 4;
    const uint8_t sl = s & 0x0F;

    if ( sh > 9 )
    {
        *t = (char) ( 'A' + sh - 10 );
    }
    else
    {
        *t = (char) ( '0' + sh );
    }

    if ( sl > 9 )
    {
        *( t + 1 ) = (char) ( 'A' + sl - 10 );
    }
    else
    {
        *( t + 1 ) = (char) ( '0' + sl );
    }
}

/* Converts double ASCII char s to single byte bin format. */
static void ascii_to_bin( const char *s, uint8_t *t )
{
    uint8_t t0, t1; /* Temporary variable. */
    uint8_t i; /* Iterative variable. */

    /* Check for valid inputs. */
    if ( ( ( (void*) 0UL ) == s ) || ( ( (void*) 0UL ) == t ) )
    {
        return;
    }

    /* For each of two bytes. */
    for ( i = 0; i < 2; i++ )
    {
        /* First character at [0-9] class. */
        if ( ( s[i] >= '0' ) && ( s[i] <= '9' ) )
        {
            t0 = s[i] - '0';
        }
        /* First character at [A-F] class. */
        else if ( ( s[i] >= 'A' ) && ( s[i] <= 'F' ) )
        {
            t0 = s[i] - 'A' + 0x0A;
        }
        /* Bad character. */
        else
        {
            return;
        }

        /* Move to the right position. */
        if ( 0 == i )
        {
            t1 = t0 << 4;
        }
        else
        {
            t1 |= t0;
        }
    }

    *t = t1;
}

/*** end of file ***/

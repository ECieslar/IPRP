/*!****************************************************************************
*
* \file sms_pdu.c
*
* \copyright ELEKTROMETAL SA (c) 2020, Wszelkie prawa zastrzeżone
* \version $Revision: 293340 $
* \date $Date: 2020-09-07 08:02:05 +0200 (pon.) $
* \author $Author: kszczepanski $
*
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include <string.h> /* strlen */
#include "sms_pdu.h"

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

/*!
 * \brief Convert telephony number character
 * to Called party BCD number character.
 *
 * see: Table 10.5.118 in clause 10.5.4.7 of 3GPP TS 04.08
 *
 * \return semi-octet value
 */
static uint8_t c_to_cp_bcd( char c );

/******************************************************************************
* Static Variables
******************************************************************************/

/******************************************************************************
* Functions
******************************************************************************/

#ifndef TINYTEST_NOTEST
/* Test cases for is_address_prefix. */
static int test_is_address_prefix( const char* name __attribute__((unused)) )
{
    bool rc;
    const char* addr1 = (void *) 0UL;
    const char* addr2 = "";
    const char* addr3 = "123";
    const char* addr4 = "+12";

     /* Test for true negative. */
    rc = is_prefix_address( addr1 );
    TINYTEST_FALSE_MSG( rc, "Null input address (rc=%d)", rc );
    rc = is_prefix_address( addr2 );
    TINYTEST_FALSE_MSG( rc, "Empty input address (rc=%d)", rc );
    rc = is_prefix_address( addr3 );
    TINYTEST_FALSE_MSG( rc, "Not-prefixed input address (rc=%d, addr=%s)", rc, addr3 );

    /* Test for true positive. */
    rc = is_prefix_address( addr4 );
    TINYTEST_TRUE_MSG( rc, "Not-prefixed input address (rc=%d, addr=%s)", rc, addr4 );

    return 1;
}
#endif /* TINYTEST_NOTEST */

bool is_prefix_address( const char* addr )
{
    if ( ( ( void* ) 0UL ) != addr )
    {
        if ( '+' == addr[0] )
        {
            return true;
        }
    }

    return false;
}

#ifndef TINYTEST_NOTEST
/* Test cases for c_to_cp_bcd. */
static int test_remove_prefix_from_address( const char* name __attribute__((unused)) )
{
    char *s = 0UL; /* Tested string. */
    char i[5] = { "\0\0\0\0\0" }; /* Input string. */

    /* Test for invalid input parameters. */
    remove_prefix_from_address( s );
    TINYTEST_EQUAL_MSG( ((void*) 0UL), s, "Not null string" );

    s = i;
    remove_prefix_from_address( s );
    TINYTEST_STR_EQUAL_MSG( "", s, "Not empty string" );

    strncpy( i, "a", sizeof i - 1 );
    s = i;
    remove_prefix_from_address( s );
    TINYTEST_STR_EQUAL_MSG( "a", s, "Modifed string" );

    strncpy( i, "+", sizeof i - 1 );
    s = i;
    remove_prefix_from_address( s );
    TINYTEST_STR_EQUAL_MSG( "", s, "Not modified string" );

    strncpy( i, "+123", sizeof i - 1 );
    s = i;
    remove_prefix_from_address( s );
    TINYTEST_STR_EQUAL_MSG( "123", s, "Not modified string" );

    return 1;
}
#endif /* TINYTEST_NOTEST */


bool remove_prefix_from_address( char* src )
{
    if ( ( ( void* ) 0UL ) != src )
    {
        if ( '+' == src[0] )
        {
            uint8_t n = strlen( src );

            memmove( &src[0], &src[1], n - 1 );
            src[n - 1] = '\0';

            return true;
        }
    }

    return false;
}

#ifndef TINYTEST_NOTEST
/* Test cases for sms_str_to_pdu_number. */
static int test_sms_str_to_pdu_address( const char* name __attribute__((unused)) )
{
    struct tp_addr dst; /* Result struct. */
    uint8_t res; /* Returned value. */

    /* Test void or empty inputs parametrs. */
    TINYTEST_EQUAL( 0, sms_str_to_pdu_address( NULL, PDU_TP_ADDR_TYPE_COMMON, NULL ) );
    TINYTEST_EQUAL( 0, sms_str_to_pdu_address( NULL, PDU_TP_ADDR_TYPE_COMMON, &dst ) );
    TINYTEST_EQUAL( 0, sms_str_to_pdu_address( "", PDU_TP_ADDR_TYPE_COMMON, NULL ) );
    TINYTEST_EQUAL( 0, sms_str_to_pdu_address( "", PDU_TP_ADDR_TYPE_COMMON, &dst ) );

    /* Test for invalid conversion. */
    /* Test for invalid inputs parametr. */
    TINYTEST_EQUAL_MSG( 0, sms_str_to_pdu_address( "012345678901234567891", PDU_TP_ADDR_TYPE_COMMON, &dst ),
                        "The phone number length is out of range (21)");

    /* Test for valid conversions. */

    /* Test for expansion to even number. */
    res = sms_str_to_pdu_address( "0123456789012345678", PDU_TP_ADDR_TYPE_COMMON, &dst );
    TINYTEST_EQUAL_MSG( 10, res, "The length 19 shoud be expanded to 20 (size: 10)" );
    TINYTEST_EQUAL_MSG( 19, dst.length, "The length shoud stay 19" );
    TINYTEST_EQUAL_MSG( 0xF8, dst.digits[9], "Invalid value at octet 10" );
    TINYTEST_BIN_ARR_EQUAL_MSG( "\x10\x32\x54\x76\x98\x10\x32\x54\x76\xF8", dst.digits, 10,
                                "Invalid digists value in the struct" );

    /* Test for 1 byte conversion. */
    res = sms_str_to_pdu_address( "01", PDU_TP_ADDR_TYPE_COMMON, &dst ); /* "01" -> length: 2, digits: 0x10 */
    TINYTEST_EQUAL_MSG( 1, res, "Invalid returned value" );
    TINYTEST_EQUAL_MSG( PDU_TP_ADDR_TYPE_COMMON, dst.type, "Invalid value at octet 0" );
    TINYTEST_EQUAL_MSG( 0x10, dst.digits[0], "Invalid value at octet 0" );

    /* Test for conversion a number with prefix. */
    res = sms_str_to_pdu_address( "+420", PDU_TP_ADDR_TYPE_INTERNATIONAL, &dst ); /* "+420" -> length: 2, digits: 0x024F */
    TINYTEST_EQUAL_MSG( 2, res, "Invalid returned value" );
    TINYTEST_EQUAL_MSG( PDU_TP_ADDR_TYPE_INTERNATIONAL, dst.type, "Invalid returned address type" );
    TINYTEST_EQUAL_MSG( 0x4F, dst.digits[0], "Invalid conversion at octet 1" );
    TINYTEST_EQUAL_MSG( 0x02, dst.digits[1], "Invalid conversion at octet 2" );

    /* Test for conversion a sample number. */
    res = sms_str_to_pdu_address( "+48100101102", PDU_TP_ADDR_TYPE_INTERNATIONAL, &dst ); /* "+48100101102" -> length=12, size=6 digits= 0x4F1800011120 */
    TINYTEST_EQUAL_MSG( 6, res, "Invalid returned value" );
    TINYTEST_EQUAL_MSG( PDU_TP_ADDR_TYPE_INTERNATIONAL, dst.type, "Invalid returned address type" );
    TINYTEST_EQUAL_MSG( 12, dst.length, "Invalid output value in the structure (length)" );
    TINYTEST_BIN_ARR_EQUAL_MSG( "\x4F\x18\x00\x01\x11\x20", dst.digits, 6, "Invalid output value in structure (digits)" );

    return 1;
}
#endif /* TINYTEST_NOTEST */

uint8_t sms_str_to_pdu_address( const char* src, PDU_TP_ADDR_TYPE_t type, struct tp_addr* dst )
{
    uint8_t addr_len; /* Length of source address (number). */
    uint8_t size; /* Number of bytes to hold source address. */

    /* Check input parameters. */
    if ( ( (void *) 0UL == src ) || ( (void *) 0UL == dst ) )
    {
        return 0U;
    }

    /* Get the number's size. */
    addr_len = strlen( src );

    /* Set the number of bytes containing the address. */
    if ( addr_len % 2 )
    {
        /* If odd, complement to even up. */
        size = (addr_len + 1) / 2;
    }
    else
    {
        size = addr_len / 2;
    }

    /* Check for valid input size. */
    if ( ( sizeof dst->digits ) < size )
    {
        return 0U;
    }

    /* Clear output digits. */
    memset( dst->digits, 0U, ( sizeof dst->digits ) );

    /* Convert source number to Called party BCD number. */
    for ( uint8_t i = 0; i < size; i++ )
    {
        dst->digits[i] = c_to_cp_bcd( src[2 * i] )
                        | ( c_to_cp_bcd( src[2 * i + 1] ) << 4 );
    }

    dst->length = addr_len; /*  Number of useful semi-octets within the Address-Value field. */
    dst->type = type;

    return size;
}

#ifndef TINYTEST_NOTEST
static int test_sms_address_serialize( const char* name __attribute__((unused)) )
{
    uint8_t res; /* Returned value. */

    /* Input parameters. */
    /* Address struct with empty number . */
    const struct tp_addr addr0 = { .length = 0U,
                                   .type = PDU_TP_ADDR_TYPE_COMMON,
                                   .digits = "\x00" };
    /* Address struct with the number 48100101102. */
    const struct tp_addr addr1 = { .length = 11U,
                                   .type = PDU_TP_ADDR_TYPE_INTERNATIONAL,
                                   .digits = "\x84\x01\x10\x10\x01\xF2" };
    uint8_t data[10];
    const uint8_t max_len = sizeof data;
    uint8_t len;

    /* Test void or empty inputs parametrs. */
    TINYTEST_EQUAL_MSG( 1, sms_address_serialize( NULL, data, max_len, &len ), "Input param addr is NULL" );
    TINYTEST_EQUAL_MSG( 1, sms_address_serialize( &addr1, NULL, max_len, &len ), "Input param data is NULL" );
    TINYTEST_EQUAL_MSG( 1, sms_address_serialize( &addr1, data, max_len, NULL ), "Input param len is NULL" );

    /* Test for invalid size of inputs parametr. */
    TINYTEST_EQUAL_MSG( 2, sms_address_serialize( &addr1, data, 1U, &len ), "Insufficient output size" );

    /* Test for valid conversion. */
    res = sms_address_serialize( &addr0, data, max_len, &len );
    TINYTEST_EQUAL_MSG( 0, res, "Invalid returned value" );
    TINYTEST_EQUAL_MSG( 1, len, "Invalid output size conversion" );
    TINYTEST_EQUAL_MSG( 0x00, data[0], "Invalid conversion for empty struct" );

    res = sms_address_serialize( &addr1, data, max_len, &len );
    TINYTEST_EQUAL_MSG( 0, res, "Invalid returned value" );
    TINYTEST_EQUAL_MSG( 8, len, "Invalid output size conversion" );
    TINYTEST_BIN_ARR_EQUAL_MSG( "\x0B\x91\x84\x01\x10\x10\x01\xF2", data, 8,
                                "Invalid conversion for the struct with a sample number" );

    if ( 0 == res )
    {
        return 1;
    }

    return 0;
}
#endif /* TINYTEST_NOTEST */

uint8_t sms_address_serialize( const struct tp_addr * addr,
                              uint8_t* data,
                              const uint8_t max_len,
                              uint8_t* len)
{
    const uint8_t digs_size = ( ( addr->length % 2U ) /* The size of digits part in the struct. */
                            ? ( addr->length + 1U ) : ( addr->length ) ) / 2U;
    uint8_t pos = 0U; /* position in stream. */

    /* Check for valid input parameters. */
     if ( ( ( (void *) 0UL ) == addr )
             || ( ( (void*) 0UL ) == data )
             || ( ( (void*) 0UL ) == len ) )
     {
         return 1U; /* SMS_PDU_INVALID_INPUT_PARAMETERS */
     }

    /* Check for sufficient output buffer size. */
    if ( max_len < ( digs_size + 2U ) )
    {
        return 2U; /* SMS_PDU_INSUFFICIENT_OUTPUT_SIZE */
    }
    else
    {
        /* Set the address part. */
        data[pos++] = addr->length; /* only digits part */

        /* Add type and digits. */
        if ( 0U < addr->length )
        {
            data[pos++] = addr->type;
            memcpy( &data[pos], addr->digits, digs_size );
            pos += digs_size;
        }
    }

    /* Assign the useful length of output data. */
    *len = pos;

    return 0U;
}

#ifndef TINYTEST_NOTEST
static int test_sms_submit_serialize( const char* name __attribute__((unused)) )
{
    struct sms_submit ss; /* Input struct. */
    uint8_t res; /* Returned value. */

    /* Auxillary data. */
    struct tp_addr dst_nr;

    /* Prepare destination address (number). */
    sms_str_to_pdu_address( "48100101102", PDU_TP_ADDR_TYPE_INTERNATIONAL, &dst_nr );

    /* Test data. */
    ss.tp_da = &dst_nr;
    ss.tp_udp = (const uint8_t *) "\xD4\xF2\x9C\x0E"; /* "Test" in 7-bit GSM encoding. */
    ss.tp_udl = 4U;

    /* Preapre an output buffer. */
    uint8_t out_buf[200];
    uint8_t out_buf_len = 0U;
    memset( out_buf, 0U, sizeof out_buf );

    /* Test for invalid conversions. */

    /* Test for valid conversion. */
    res = sms_submit_serialize( &ss, out_buf, sizeof out_buf, &out_buf_len );
    TINYTEST_EQUAL_MSG( 0, res, "Invalid returned value" );
    TINYTEST_EQUAL_MSG( 17, out_buf_len, "Invalid output length" );
    TINYTEST_BIN_ARR_EQUAL_MSG( "\x01\x00\x0B\x91\x84\x01\x10\x10\x01\xF2\x00\x00\x04\xD4\xF2\x9C\x0E",
                                    out_buf, 17, "Invalid returned data" );

    return 1;
}
#endif /* TINYTEST_NOTEST */

/* TPDU  Transfer protocol data unit is defined in 3GPP TS 03.40, clause 9.2.3 */
uint8_t sms_submit_serialize( const struct sms_submit* sms,
                              uint8_t* data,
                              const uint8_t max_len,
                              uint8_t* len)
{
    uint8_t pos = 0U; /* Position index. */
    uint8_t sub_ret; /* Sub part's converstion result. */
    uint8_t sub_len = 0U; /* Sub part's length. */

    /* Check for valid input parameters. */
    if ( ( ( (void *) 0UL ) == sms )
            || ( ( (void*) 0UL ) == data )
            || ( ( (void*) 0UL ) == len ) )
    {
        return 1; /* SMS_PDU_INVALID_INPUT_PARAMETERS */
    }

    /* Check for sufficient output buffer size. */
    if ( max_len < ( 2U ) )
    {
        return 2U; /* SMS_PDU_INSUFFICIENT_OUTPUT_SIZE */
    }
    else
    {
        /* Set the basic parameters.*/
        /* Unfortunatly actualy it;s not sure which tp parameters to set.
         * Probably (see: 9.2.2.2 at 3GPP TS 23.040):
         * 1st byte: TP-MTI (2b), TP-RD (b), TP-VPF (2b), TP-RP (b), TP-UDHI (b), TP-SRR (b)
         * 2nd byte: TP-MR (I - 1o);
         * Below values come from N15 datasheet pt A.1 Content of PDU SMS Messages . */
        data[pos++] = 0x01; /* Indicates basic parameters  for N15 (TP-SRR=1). */
        data[pos++] = 0x00; /* Indicates message baseline value for N15. */

        /* Length of recipient number. */
        sub_ret = sms_address_serialize( sms->tp_da, &data[pos], max_len - pos, &sub_len );

        if ( SMS_PDU_OK != sub_ret )
        {
            return sub_ret; /* Retunr uniform error code. */
        }
        else /* SMS_PDU_OK */
        {
            pos += sub_len;

            /* TP-Protocol-Identifier
             * TP-PID= 0x00 - "straightforward case of simple MS-to-SC short message transfer".
             * pt 9.2.3.9 at 3GPP TS 24.040 */
            data[pos++] = 0x00;

            /* TP-Data-Coding-Scheme (see: pt 9.2.3.10 at 3GPP TS 23.040
             * and pt 4 SMS Data Coding Scheme at 3GPP TS 23.038)
             * N15 manual in an example propose TP-DCS=0x08 for UCS2. */
            data[pos++] = 0x00; /* 7 bit GSM */

            /* Check for sufficient output size. */
            if ( max_len >= ( pos + 1 + sms->tp_udl ) )
            {
                /* Set the user data. */
                data[pos++] = sms->tp_udl;
                memcpy( &data[pos], sms->tp_udp, sms->tp_udl );
                pos += sms->tp_udl;
            }
        }
    }

    *len = pos;

    return 0U;
}

#ifndef TINYTEST_NOTEST
/* Test cases for sms_str_to_pdu_number. */
static int test_c_to_cp_bcd( const char *name __attribute__((unused)) )
{
    /* Test for invalid values. */
    TINYTEST_EQUAL( 0x0F, c_to_cp_bcd(  0  ) );
    TINYTEST_EQUAL( 0x0F, c_to_cp_bcd( '+' ) );
    TINYTEST_EQUAL( 0x0F, c_to_cp_bcd( 0xFF ) );

    /* Test for valid values. */
    TINYTEST_EQUAL( 0,    c_to_cp_bcd( '0' ) );
    TINYTEST_EQUAL( 1,    c_to_cp_bcd( '1' ) );
    TINYTEST_EQUAL( 8,    c_to_cp_bcd( '8' ) );
    TINYTEST_EQUAL( 9,    c_to_cp_bcd( '9' ) );
    TINYTEST_EQUAL( 0x0A, c_to_cp_bcd( '*' ) );
    TINYTEST_EQUAL( 0x0B, c_to_cp_bcd( '#' ) );
    TINYTEST_EQUAL( 0x0C, c_to_cp_bcd( 'a' ) );
    TINYTEST_EQUAL( 0x0D, c_to_cp_bcd( 'b' ) );
    TINYTEST_EQUAL( 0x0E, c_to_cp_bcd( 'c' ) );

    return 1;
}
#endif /* TINYTEST_NOTEST */

static uint8_t c_to_cp_bcd( char c )
{
    if ( c >= '0' && c <= '9' )
    {
        return ( c - '0' );
    }
    else if ( c == '*' )
    {
        return 0x0AU;
    }
    else if ( c == '#' )
    {
        return 0x0BU;
    }
    else if ( c >= 'a' && c <= 'c' )
    {
        return ( c - 'a' + 0x0CU );
    }
    else
    {
        return 0x0FU;
    }
}

/******************************************************************************
* Test suite.
******************************************************************************/

#ifndef TINYTEST_NOTEST
/* Test suite for the unit. */
TINYTEST_START_SUITE( sms_pdu );
  /* The last one is executed as first. */
  TINYTEST_ADD_TEST( test_sms_submit_serialize, NULL, NULL );
  TINYTEST_ADD_TEST( test_sms_address_serialize, NULL, NULL );
  TINYTEST_ADD_TEST( test_sms_str_to_pdu_address, NULL, NULL );
  TINYTEST_ADD_TEST( test_remove_prefix_from_address, NULL, NULL );
  TINYTEST_ADD_TEST( test_is_address_prefix, NULL, NULL );
  TINYTEST_ADD_TEST( test_c_to_cp_bcd, NULL, NULL );
TINYTEST_END_SUITE();
#endif /* TINYTEST_NOTEST */

/*** end of file ***/

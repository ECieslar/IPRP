/*!****************************************************************************
*
* \file gprs.h
* \brief GPRS modem's functions
*
* \copyright ELEKTROMETAL SA (c) 2020, Wszelkie prawa zastrzeżone
* \version $Revision$
* \date $Date$
* \author $Author$
*
******************************************************************************/

#ifndef GPRS_COMM_H_
#define GPRS_COMM_H_

/******************************************************************************
* Includes
******************************************************************************/

#include <stdint.h> /* Declarations of integer types with specified width. */
#include <stdbool.h> /* Declaration of bool type. */
#include "../test/tinytest.h" /* Test suite declaration. */

/******************************************************************************
* Defines and Enumerations
******************************************************************************/
#ifndef TINYTEST_NOTEST
/* Definition of test suite for tinytest. */
TINYTEST_DECLARE_SUITE(gprs_comm);
#endif /* TINYTEST_NOTEST */

/******************************************************************************
* Types and Typedefs
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

/*!
 * \brief Set the APN name with creditials.
 * \param apn null terminated string with APN name.
 * \param username null terminated string with user name.
 * \param password null terminated string with password
 * \return '1' for successful result; 0 otherwise.
 * \todo Change return result to error code.
 */
bool gprs_set_apn( const char* apn, const char* username, const char* password );

/*!
 * \brief Gets the APN name.
 * \return APN string or null if APN no APN is set.
 */
const char* gprs_get_apn( void );

/*!
 * \brief Enable or disable due to \param onoff state GPRS communication.
 * \param onoff
 * \return GPRS communication state.
 */
bool gprs_enable( bool onoff );

/*!
 * \brief Get the GPRS communication state.
 * \return the GPRS state adequate to result of +CGREG command.
 */
bool gprs_get_state( void );

/*!
 * \brief Get an IP assigned in the PS network.
 * \return null terminated string with IP address in format ddd.ddd.ddd.ddd.
 */
const char* gprs_get_ip( void );

/*!
 * \brief Convert an IP string to 32-bit size value.
 * \param ip_str null terminated string with IP address
 * \return IP 32-bit value in LE order (AAA.BBB.CCC.DDD -> 0xaabbccdd).
 */
uint32_t gprs_ip_str_to_ipv4( const char* ip_str );

/*!
 * \brief Convert an IP 32-bit size value to string.
 * \param ip32 32-bit value in LE order (AAA.BBB.CCC.DDD -> 0xaabbccdd)
 * \param ip_str null terminated string with IP address.
 */
void gprs_ipv4_to_ip_str( const uint32_t ip32, char* ip_str, const uint8_t ip_str_size );

#endif /* GPRS_COMM_H_ */

/*** end of file ***/

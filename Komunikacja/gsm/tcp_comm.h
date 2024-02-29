/*!****************************************************************************
*
* \file tcp_comm.h
* \brief  TCP client functions.
*
* \copyright ELEKTROMETAL SA (c) 2020, Wszelkie prawa zastrzeżone
* \version $Revision$
* \date $Date$
* \author $Author$
*
******************************************************************************/

#ifndef TCP_COMM_H_
#define TCP_COMM_H_

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
TINYTEST_DECLARE_SUITE(tcp_comm);
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
 * \brief
 * \return
 */
bool tcp_available( void );

/*!
 * \brief Make a connection to \param server on \param port.
 * \param server Target server
 * \param port Target server's port
 * \return Connection status: true if connaction
 *          is made successful, and false otherwise.
 */
bool tcp_connect( const char *server, uint16_t port );

/*!
 * \brief Close an active or broken connection to server.
 * \return Close connection status.
 */
bool tcp_close( void );

/*!
 * \brief Checks if connection is active.
 * \return Returns true if connection is active;
 *         false if connection is broken.
 */
bool tcp_is_connected( void );

/*!
 * \brief
 * \param data
 * \param len
 * \param mode  0 - ASCII; 1 - HEX
 * \return
 */
bool tcp_send( const char *data, uint16_t len, const int8_t mode );

/*!
 * \brief
 * \param buff
 * \param max_len
 * \param len
 * \return
 */
bool tcp_recv( char *buff, const uint16_t max_len, uint16_t* len );

#endif /* TCP_COMM_H_ */

/*** end of file ***/

/*!****************************************************************************
 *
 * \file crc.h
 * \brief CRC calulcation definitions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 265212 $
 * \date $Date: 2020-01-17 15:30:28 +0100 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef CRC_H_
#define CRC_H_

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h> /* Declarations of integer types with specified width. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

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
 * \brief Calculate CRC based on polynomial CCITT with base 0xFFFF.
 *
 * \param[in] data The input buffer with data to calculate CRC.
 * \param[in] len The length of input data.
 *
 * \return Calculated CRC value.
 */
uint16_t calculate_crc16_ccitt_false( const uint8_t *data, const uint16_t len );

#endif /* CRC_H_ */

/*** end of file ***/

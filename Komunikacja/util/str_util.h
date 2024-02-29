/*!****************************************************************************
 *
 * \file str_util.h
 * \brief Miscellaneous string conversion function definitions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 265212 $
 * \date $Date: 2020-01-17 15:30:28 +0100 (pt.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef STR_UTIL_H_
#define STR_UTIL_H_

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
 * \brief Converts a binary byte stream s of length len to ASCII stream t.
 */
void str_bin_to_ascii( const uint8_t *s, char *t, uint8_t len );

/*!
 * \brief Converts an ASCII stream s of length len to byte binary stream t.
 */
void str_ascii_to_bin( const char *s, uint8_t *t, const uint8_t len );

#endif /* STR_UTIL_H_ */

/*** end of file ***/

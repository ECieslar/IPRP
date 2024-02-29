/*!****************************************************************************
 *
 * \file debug.c
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

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "debug.h"
#include "../hw/mkg_hw.h"

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
 * \brief  Retargets the C library printf function to the USART.
 * \param  None
 * \retval None
 */
/* cppcheck-suppress unusedFunction ; The function used at the linker level. */
PUTCHAR_PROTOTYPE
{
    /* Implementation of fputc. */
    hw_transmit_by_uart( DEBUG_UART, (uint8_t*) &ch, 1 );

    return ch;
}

/*** end of file ***/

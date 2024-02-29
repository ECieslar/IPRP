/*!****************************************************************************
 *
 * \file mkg_sw.h
 * \brief The MKG module's software definitions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 291414 $
 * \date $Date: 2020-08-24 10:31:30 +0200 (pon.) $
 * \author $Author: kszczepanski $
 *
 ******************************************************************************/

#ifndef MKG_SG_APP_H_
#define MKG_SG_APP_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Declarations of integer types with specified width. */
#include "..\build.h" /* Auto-incremented build number. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*!
 * \brief MKG module's general information.
 */
enum MKG_General
{
    MKG_SW_ID_GROUP = 65, /*!< Application group id. */
    MKG_SW_ID_ITEM = 1262, /*!< Application item id. */
    MKG_SW_ID_VER_MAJ = 0, /*!< Application version major. */
    MKG_SW_ID_VER_MIN = 15, /*!< Application version minor. */
    MKG_SW_ID_VER_REL = 1, /*!< Application version release. */
    MKG_SW_ID_VER_BLD = BUILD_NUMBER, /*!< Application version build. */
};

/******************************************************************************
 * Types and Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

#endif /* MKG_SG_APP_H_ */

/*** end of file ***/

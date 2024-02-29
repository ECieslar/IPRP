/*!****************************************************************************
 *
 * \file gm_comm.h
 * \brief Gas-Meter communication definitions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 293825 $
 * \date $Date: 2020-09-11 14:53:21 +0200 (pt.) $
 * \author $Author: kszczepanski $
 *
 * Responsibilities:
 * - establish connection to Gas-Meter by UART port
 * - bi-directional communication with Gas-Meter with internal protocol
 *
 ******************************************************************************/

#ifndef GM_COMM_H_
#define GM_COMM_H_

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h> /* Declarations of integer types with specified width. */

#include "../sg/sg_obj.h" /* SMART-GAS object definition. */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/

/*!
 * \brief Gas-Meter general constants.
 */
enum GM_comm_general
{
    GM_PROTOCOL_VERSION = 0x0, /* Protocol version. */
    GM_FRAME_DATA_LENGTH = 255U, /* Data field's length. */
    GM_FRAME_BEGIN = 0x55U, /* The begin frame's tag. */
    GM_FRAME_END = 0xFFU, /* The end frame's tag. */
    GM_FRAME_SIZE_MIN = 10, /* The frame minimal size. */
};

/*!
 * \brief Gas-Meter operational error codes.
 */
enum GM_error_code
{
    GM_OK, /*!< Successful operation. */
    GM_DEVICE_NOT_READY, /*!< Port to communicate with GM not ready. */
    GM_DEVICE_NOT_RESPONDING, /*!< No response from GM. */
    GM_COMM_WRITE_ERROR, /*!< Can't write to GM. */
    GM_COMM_READ_ERROR, /*!< Can't read from GM. */
    GM_COMM_FRAME_LENGTH_ERROR, /*!< Frame lenght too short. */
    GM_COMM_FRAME_FORMAT_ERROR, /*!< Frame format error. */
    GM_COMM_FRAME_DATA_LENGTH_ERROR, /*!< Frame size error. */
    GM_COMM_FRAME_CRC_ERROR, /*!< CRC frame error. */
    GM_INVALID_PARAMETER_ERROR, /*!< Invalid parameter. */
    GM_INSUFFICIEND_OUTPUT_SIZE_ERROR,
};

/*!
 * \brief Functions in communication between MKG and MCI boards.
 */
enum GM_comm_function
{
    GM_SG_DATA_FUNC = 0x01,
    GM_BUADRATE_FUNC = 0x02,
    GM_REPEAT_FUNC = 0x03,
    GM_RADIO_DATA_FUNC = 0x04,
};

/*!
 * \brief GM frame struct.
 */
struct GM_frame
{
    uint8_t header; /* The begin of the frame (0x55). */
    struct frame_opt
    {
        uint8_t sender :1; /* Sender */
        uint8_t encypt :1; /* Encryption. */
        uint8_t reserved :2; /* Reserved (should be zeros). */
        uint8_t proto_ver :4; /* Protocol version */
    } options;
    uint8_t counter; /* The counter incremented after each frame. */
    uint16_t data_len; /* The data's length. */
    enum GM_comm_function func; /* The function field. */
    uint32_t crc32; /* A frame checksum field. */
    uint8_t footer; /* The end of the frame (0xFF). */
    uint8_t data[GM_FRAME_DATA_LENGTH]; /* The data field. */
};

/******************************************************************************
 * Types and Typedefs
 ******************************************************************************/

/*!
 * \brief The Gas-Meter operational status type.
 */
typedef enum GM_error_code GM_status_t;

/*!
 * \brief The Gas-meter frame type.
 */
typedef struct GM_frame GM_frame_t;

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * \brief Receive a frame from Gas-Meter unit.
 *
 * \param [out] frame received frame
 *
 * \return GM_OK on successful connect.
 */
GM_status_t gm_receive_frame( GM_frame_t *frame );

/*!
 * \brief Send a frame to Gas-Meter unit.
 *
 * \param [in] frame frame to sent
 *
 * \return GM_OK on successful connect.
 */
GM_status_t gm_send_frame( GM_frame_t *frame );

/*!
 * \brief Print frame to standard output.
 */
void gm_print_frame( const GM_frame_t *frame );

#endif /* GM_COMM_H_ */

/*** end of file ***/

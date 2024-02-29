/*!****************************************************************************
*
* \file hw_mkg.h
* \brief The MKG module's hardware definitions.
*
* \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
* \version $Revision: 264128 $
* \date $Date: 2020-01-10 10:04:58 +0100 (pt.) $
* \author $Author: kszczepanski $
*
******************************************************************************/

#ifndef MKG_HW_H_
#define MKG_HW_H_

/******************************************************************************
* Includes
******************************************************************************/
#include <stdint.h> /* Declarations of integer types with specified width. */
#include <stm32l071xx.h> /* Processor's register and structure definitions */

/******************************************************************************
* Defines and Enumerations
******************************************************************************/
/*!
 * \brief MKG module's general hardware information.
 */
enum MKG_HW_General
{
	MKG_HW_ID_GROUP 	= 32,	/*!< Board group id. */
	MKG_HW_ID_ITEM  	= 1186,	/*!< Board item id. */
	MKG_HW_ID_REV_YEAR  = 19, 	/*!< Board revision year. */
	MKG_HW_ID_REV_MONTH = 5, 	/*!< Board revision month. */
	MKG_HW_ID_REV_DAY   = 14, 	/*!< Board revision day. */
};

/*!
 * \brief The functional pin mapping for the MKG module.
 */
enum MKG_Pin
{
	/* Port A Pins */
#ifdef DEBUG
    UART4_TX_PA00_Pin   = ( 1 << 0 ),  /* USART4_TX - Transmitter for debugging. */
#endif /* DEBUG */
	UART2_TX_PA02_Pin	= ( 1 << 2 ),  /* USART2_TX	- Transmitter to communicate with the Gas-meter . */
	UART2_RX_PA03_Pin	= ( 1 << 3 ),  /* USART2_RX	- Receiver to communicate with the Gas-meter. */
	UART1_TX_PA09_Pin	= ( 1 << 9 ),  /* USART1_TX	- Transmitter to communicate with the GSM modem . */
	UART1_RX_PA10_Pin	= ( 1 << 10 ), /* USART1_RX	- Receiver to communicate with the GSM modem. */

	/* PORT B Pins */
	DC_EN_PB02_Pin 		= ( 1 << 2 ),  /* DC_EN 	- Output to switch on GSM modem's
	                                                  voltage regulator (should be 'high' after startup). */
	PWR_KEY_PB03_Pin 	= ( 1 << 3 ),  /* PWR_KEY	- Output to switch on GSM modem
	                                                  (1,2 s 'high' - on, 3 s 'high' - off). */
};


/*!
 * \brief The GPIO port mapping for the MKG module.
 */
enum MKG_io_port
{
	PORTA = (uint32_t) GPIOA_BASE,	/* USART to communicate with GSM modem. */
	PORTB = (uint32_t) GPIOB_BASE,	/* USART to communicate with Gas-Meter. */
};

/*!
 * \brief UART port mapping for MKG.
 */
enum MKG_uart_port
{
	GSM_MODEM_UART = (uint32_t) USART1_BASE,	/* UART to communicate with GSM modem. */
	GAS_METER_UART = (uint32_t) USART2_BASE,	/* UART to communicate with Gas-Meter. */

#ifdef DEBUG
    DEBUG_UART     = (uint32_t) USART4_BASE,    /* UART for debugging. */
#endif /* DEBUG */

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

/*
 * General functions.
 *
 */

/*! \brief The hardware initialization and configuration. */
void hw_init(void);

/*
 * Time functions.
 */

/*! \brief Get the 1 ms ticks from startup. */
uint32_t hw_get_tick_time( void );


/*! \brief Wait for delay in ms. */
void hw_delay( uint32_t delay );


/*
 * I/O functions.
 */

/*! \brief Sets the port's pin to the state state. */
void hw_set_port_pin_state( enum MKG_io_port port, uint16_t pin, uint8_t state );

/*
 * UART's functions.
 */

/*! \brief Flush the UART's port input buffer.*/
void hw_flush_uart( enum MKG_uart_port port );

/*! \brief Transmit the data about len length by UART port. */
uint16_t hw_transmit_by_uart( enum MKG_uart_port port, const uint8_t* data, uint16_t len );

/*! \brief Receive the max_len maximum length's the data from UART port. */
uint16_t hw_receive_from_uart( enum MKG_uart_port port, uint8_t* data,
                               const uint16_t max_len, const uint32_t timeout );

/*
 * Flash operation functions.
 */

/*!
 * \brief Read byte from Flash memory.
 */
uint8_t hw_read_byte_data_eeprom( uint32_t addr );

/*!
 * \brief Read short (half-word) from Flash memory.
 */
uint16_t hw_read_short_data_eeprom( uint32_t addr );

/*!
 * \brief Read word from Flash memory.
 */
uint32_t hw_read_word_data_eeprom( uint32_t addr );

/*!
 * \brief Write byte to Flash memory.
 */
void hw_write_byte_data_eeprom( uint32_t addr, uint8_t byte );

/*!
 * \brief Write short (half-word) to Flash memory.
 */
void hw_write_short_data_eeprom( uint32_t addr, uint16_t ushort );

/*!
 * \brief Write word to Flash memory.
 */
void hw_write_word_data_eeprom( uint32_t addr, uint32_t word );

/*
 * Power managment functions.
 */

/*!
 * \brief Enter in sleep mode.
 */
void hw_enter_standby( void );


/*
 * Miscellaneous functions.
 */

/*!
 * \brief Get CRC-32 code for data stream of length len.
 * \param data Data to CRC calculation.
 * \param len The length of data.
 * \return Calculated CRC 32 value.
 */
uint32_t hw_get_crc32( const uint8_t *data, uint32_t len );

#endif /* MKG_HW_H_ */

/*** end of file ***/


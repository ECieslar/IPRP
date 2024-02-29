/*!****************************************************************************
 *
 * \file hw_mkg.c
 * \brief The MKG module's hardware functions.
 *
 * \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
 * \version $Revision: 265212 $
 * \date $Date: 2020-01-17 15:30:28 +0100 (pt.) $
 * \author $Author: kszczepanski $
 *
 * Responsibilities:
 * - the hardware configuration and initialization.
 * - the hardware (pins, UARTs) access functions.
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "mkg_hw.h"

#include <stm32l071xx.h>	/* Processor's register and structure definitions */
#include <core_cm0plus.h>	/* CMSIS ARM's definitions */

/******************************************************************************
 * Defines and Enumerations
 ******************************************************************************/
#define XLCK_FREQ 24000000U /* 24 MHz */

/******************************************************************************
 * Local Types and Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* System Clock Frequency (Core Clock)
 * The variable is required by STM's framework (system_stm32l0xxh file). */
uint32_t SystemCoreClock = XLCK_FREQ; /* 24 MHz */

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Static Variables
 ******************************************************************************/

/*!
 * \brief CPU's ticks from startup.
 * \note After more than 49 days (2^32 ticks) the counter will turn around.
 */
static volatile uint32_t ms_tick;

/******************************************************************************
 * Functions
 ******************************************************************************/

/*!
 * \brief HardFault interruption handler.
 *
 *  \note This function replaces default Default_Handler
 *  	  defined in the startup_*.s file
 */
/* cppcheck-suppress unusedFunction; ISR handler */
void HardFault_Handler( void )
{
    __asm volatile ("bkpt #5");
}

/*!
 * \brief SysTick's interruption handler.
 *
 *  \note This function replaces default Default_Handler
 *  	  defined in the startup_*.s file
 */
/* cppcheck-suppress unusedFunction; ISR handler */
void SysTick_Handler( void )
{
    ms_tick++;
}

uint32_t hw_get_tick_time( void )
{
    return ms_tick;
}

void hw_delay( uint32_t delay )
{
    uint32_t tickstart = hw_get_tick_time();
    uint32_t wait = delay;

    /* Add a period to guaranty minimum wait */
    if ( wait < 0xFFFFFFFFU )
    {
        wait++;
    }

    while ( ( hw_get_tick_time() - tickstart ) < wait )
        ;
}

void hw_init( void )
{
    /*
     * Init FLASH.
     */

    /* Enable the FLASH preread buffer/ */
    FLASH->ACR |= FLASH_ACR_PRE_READ;

    /*
     * Init SysTick.
     */
    /* Configure the SysTick to have interrupt in 1 ms time basis. */
    SysTick_Config( SystemCoreClock / 1000UL );
    /* Configure the SysTick IRQ priority. */
    NVIC_SetPriority( SysTick_IRQn, 0 );

    /*
     * Init the clock.
     */

    /* Enable the clock to the Power Control peripheral. */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* Configure the main internal regulator output voltage. */
    PWR->CR |= PWR_CR_VOS; /* Clear (set '0x3' - VCore=1,2V, 0 - forbidden) */
    PWR->CR &= ~PWR_CR_VOS_1; /* VCore=1,8V; CPU_performance=high (CPU_Freq_max=32MHz) */

    /* Enable HSE oscillator. */
    RCC->CR |= RCC_CR_HSEON;
    /* Wait for readiness. */
    while ( 0UL == ( RCC->CR & RCC_CR_HSERDY ) )
    {
        /* For robust implementation, add here time-out management */
    }

    /* Configure PLL to 24 MHz */
    RCC->CR &= ~RCC_CR_PLLON;
    while ( 0UL != ( RCC->CR & RCC_CR_PLLRDY ) )
    {
        /* For robust implementation, add here time-out management */
    }
    /* Set latency to 1 wait state. */
    FLASH->ACR |= FLASH_ACR_LATENCY;
    /* Set the PLL multiplier to 8 and divider by 2 (8 MHz x 6 / 2 ). */
    RCC->CFGR |= ( RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLDIV2 );
    /* Enable PLL. */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait until PLLRDY is set. */
    while ( 0UL == ( RCC->CR & RCC_CR_PLLRDY ) )
    {
        /* For robust implementation, add here time-out management */
    }

    /* Set AHB prescaler (SYSCLK), APB low-speed prescaler (APB1)
     * and APB high-speed prescaler (APB2) to 1 (no division). */
    RCC->CFGR &= ~ ( RCC_CFGR_HPRE_0 | RCC_CFGR_PPRE1_0 | RCC_CFGR_PPRE2_0 );

    /*  Select PLL as system clock. */
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait until the PLL is switched on. */
    while ( RCC_CFGR_SWS_PLL != ( RCC->CFGR & RCC_CFGR_SWS ) )
    {
        /* For robust implementation, add here time-out management */
    }

    /* Specifies APB as the USART1 and USART2 clock source. */
    RCC->CCIPR &= ~ ( RCC_CCIPR_USART1SEL_0 | RCC_CCIPR_USART2SEL_0 );

    /*
     * Init the GPIO.
     */

    /* Enable the IOPORT peripheral clock for GPIOB. */
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    /* Delay after an RCC peripheral clock enabling */
    while ( 0 == ( RCC->IOPENR & RCC_IOPENR_GPIOBEN ) )
    {
        /* For robust implementation, add here time-out management */
    }

    /* Configure DC_EN and PWR_KEY pins as outputs. */
    /* Set 'reset' state. */
    GPIOB->BRR |= ( DC_EN_PB02_Pin | PWR_KEY_PB03_Pin );
    /* Configure the Output speed as 'Low speed'. */
    GPIOB->OSPEEDR &= ~ ( GPIO_OSPEEDER_OSPEED2 | GPIO_OSPEEDER_OSPEED3 );
    /* Configure the Output Type as 'Output push-pull' */
    GPIOB->OTYPER &= ~ ( GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_3 );
    /* Configure IO Direction mode as Output. */
    GPIOB->MODER &= ~ ( GPIO_MODER_MODE2 | GPIO_MODER_MODE3 )
            | ( GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 );
    /* Configure no pull-up and no pull-down. */
    GPIOB->PUPDR &= ~ ( GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 );

    /*
     * Init the USARTs.
     */

    /* USART1 and USART2 clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
#ifdef DEBUG
    if ( DEBUG_UART == (uint32_t) USART4_BASE )
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART4EN;
    }
#endif /* DEBUG */

    /* Enable the IOPORT peripheral clock for GPIOA. */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    /* Delay after an RCC peripheral clock enabling */
    while ( 0 == ( RCC->IOPENR & RCC_IOPENR_GPIOAEN ) )
    {
        /* For robust implementation, add here time-out management */
    }

    /*
     * USART1 GPIO Configuration
     */

    /* Configure UART1_TX and UART1_RX pins. */
    /* Configure Alternate function as AF4 (0100b). */
    GPIOA->AFR[1] |= ( GPIO_AFRH_AFSEL9 | GPIO_AFRH_AFSEL10 )
            & ( ( 0x04 << GPIO_AFRH_AFSEL9_Pos ) 
            | ( 0x04 << GPIO_AFRH_AFSEL10_Pos ) );
    /* Configure the Output speed as 'Very high speed' (11b). */
    GPIOA->OSPEEDR |= ( GPIO_OSPEEDER_OSPEED9 | GPIO_OSPEEDER_OSPEED10 );
    /* Configure the Output Type as 'Output push-pull' */
    GPIOA->OTYPER &= ~ ( GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_10 );
    /* Configure IO Direction mode as 'Alternate function mode'. */
    GPIOA->MODER &= ~ ( GPIO_MODER_MODE9 | GPIO_MODER_MODE10 )
            | ( GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1 );
    /* Configure no pull-up and no pull-down. */
    GPIOA->PUPDR &= ~ ( GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10 );

    /*
     * UART1 Configuration
     */

    /* Disable USART. */
    USART1->CR1 &= ~USART_CR1_UE;

    /* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
     *  the UART Word Length, Parity, Mode and oversampling:
     *  set OVER8 bit to '0' for 16 bit oversampling.
     *  set the M(0,1) bits to '0' for 8 bit word length
     *  set PCE to '0' for no parity
     *  set TE and RE to '1' to enable Tx and Rx
     */
    USART1->CR1 &= ~ ( USART_CR1_M1 | USART_CR1_OVER8 | USART_CR1_M0
                       | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE
                       | USART_CR1_RE );
    USART1->CR1 |= ( 0x0 << USART_CR1_M1_Pos ) | ( 0x0 << USART_CR1_OVER8_Pos )
                   | ( 0x0 << USART_CR1_M0_Pos ) | ( 0x0 << USART_CR1_PCE_Pos )
                   | ( 0x1 << USART_CR1_TE_Pos ) | ( 0x1 << USART_CR1_RE_Pos );

    /* Configure the UART Stop Bits  */
    USART1->CR2 &= ~USART_CR2_STOP;

    /* Clear UART HardWare Flow Control for TTL transmition. */
    USART1->CR3 &= ~ ( USART_CR3_RTSE | USART_CR3_CTSE );
    /* Disable Overrun. */
    USART1->CR3 |= USART_CR3_OVRDIS;

    /* In asynchronous mode, the following bits must be kept cleared:
     * - LINEN and CLKEN bits in the USART_CR2 register,
     * - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
     */
    USART1->CR2 &= ~ ( USART_CR2_LINEN | USART_CR2_CLKEN );
    USART1->CR3 &= ~ ( USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN );

    /* Set the baud rate for 115200 bps (24 MHz / 115200). */
    USART1->BRR = 0x00D0UL; /* 208 */

    /* Enable USART. */
    USART1->CR1 |= USART_CR1_UE;

    /*
     * USART2 GPIO Configuration.
     */

    /* Configure USART2_TX and USART2_RX pins. */
    /* Configure Alternate function as AF4 (0100b). */
    GPIOA->AFR[0] |= ( GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3 )
            & ( ( 0x04 << GPIO_AFRL_AFSEL2_Pos )
            | ( 0x04 << GPIO_AFRL_AFSEL3_Pos ) );
    /* Configure the Output speed as 'Very high speed' (11b). */
    GPIOA->OSPEEDR |= ( GPIO_OSPEEDER_OSPEED2 | GPIO_OSPEEDER_OSPEED3 );
    /* Configure the Output Type as 'Output push-pull' */
    GPIOA->OTYPER &= ~ ( GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_3 );
    /* Configure IO Direction mode as 'Alternate function mode'. */
    GPIOA->MODER &= ~ ( GPIO_MODER_MODE2 | GPIO_MODER_MODE3 )
            | ( GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1 );
    /* Configure no pull-up and no pull-down. */
    GPIOA->PUPDR &= ~ ( GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 );

    /*
     * UART2 Configuration
     */

    /* Disable USART. */
    USART2->CR1 &= ~USART_CR1_UE;

    /* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
     *  the UART Word Length, Parity, Mode and oversampling:
     *  set OVER8 bit to '0' for 16 bit oversampling.
     *  set the M(0,1) bits to '0' for 8 bit word length
     *  set PCE to '0' for no parity
     *  set TE and RE to '1' to enable Tx and Rx
     */
    USART2->CR1 &= ~ ( USART_CR1_M1 | USART_CR1_OVER8 | USART_CR1_M0
                       | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE
                       | USART_CR1_RE );
    USART2->CR1 |= ( 0x0 << USART_CR1_M1_Pos ) | ( 0x0 << USART_CR1_OVER8_Pos )
                   | ( 0x0 << USART_CR1_M0_Pos ) | ( 0x0 << USART_CR1_PCE_Pos )
                   | ( 0x1 << USART_CR1_TE_Pos ) | ( 0x1 << USART_CR1_RE_Pos );

    /* Configure the UART Stop Bits  */
    USART2->CR2 &= ~USART_CR2_STOP;

    /* Clear UART HardWare Flow Control for TTL transmition. */
    USART2->CR3 &= ~ ( USART_CR3_RTSE | USART_CR3_CTSE );
    /* Disable Overrun. */
    USART2->CR3 |= USART_CR3_OVRDIS;

    /* In asynchronous mode, the following bits must be kept cleared:
     * - LINEN and CLKEN bits in the USART_CR2 register,
     * - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
     */
    USART2->CR2 &= ~ ( USART_CR2_LINEN | USART_CR2_CLKEN );
    USART2->CR3 &= ~ ( USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN );

    /* Set the baud rate for 115200 bps (24 MHz / 115200). */
    USART2->BRR = 0x00D0UL; /* 208 */

    /* Enable USART. */
    USART2->CR1 |= USART_CR1_UE;

#ifdef DEBUG
    if ( DEBUG_UART == (uint32_t) USART4_BASE )
    {
        /*
         * USART4 GPIO Configuration.
         */

        /* Configure USART4_TX (PA0) pin. */
        /* Configure Alternate function as AF6. */
        GPIOA->AFR[0] |= ( GPIO_AFRL_AFSEL0 )
                & ( 0x06 << GPIO_AFRL_AFSEL0_Pos );
        /* Configure the Output speed as 'Very high speed' (11b). */
        GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEED0;
        /* Configure the Output Type as 'Output push-pull' */
        GPIOA->OTYPER &= ~GPIO_OTYPER_OT_0;
        /* Configure IO Direction mode as 'Alternate function mode'. */
        GPIOA->MODER &= ~GPIO_MODER_MODE0 | GPIO_MODER_MODE0_1;
        /* Configure no pull-up and no pull-down. */
        GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;

        /*
         * UART4 Configuration
         */

        /* Disable USART. */
        USART4->CR1 &= ~USART_CR1_UE;

        /* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
         *  the UART Word Length, Parity, Mode and oversampling:
         *  set OVER8 bit to '0' for 16 bit oversampling.
         *  set the M(0,1) bits to '0' for 8 bit word length
         *  set PCE to '0' for no parity
         *  set TE  to '1' to enable Tx
         */
        USART4->CR1 &= ~ ( USART_CR1_M1 | USART_CR1_OVER8 | USART_CR1_M0
                           | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE );
        USART4->CR1 |= ( 0x0 << USART_CR1_M1_Pos )
                | ( 0x0 << USART_CR1_OVER8_Pos ) | ( 0x0 << USART_CR1_M0_Pos )
                | ( 0x0 << USART_CR1_PCE_Pos ) | ( 0x1 << USART_CR1_TE_Pos );

        /* Configure the UART Stop Bits  */
        USART4->CR2 &= ~USART_CR2_STOP;

        /* Clear UART HardWare Flow Control for TTL transmition. */
        USART4->CR3 &= ~ ( USART_CR3_RTSE | USART_CR3_CTSE );
        /* Disable Overrun. */
        USART4->CR3 |= USART_CR3_OVRDIS;

        /* In asynchronous mode, the following bits must be kept cleared:
         * - LINEN and CLKEN bits in the USART_CR2 register,
         * - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
         */
        USART4->CR2 &= ~ ( USART_CR2_LINEN | USART_CR2_CLKEN );
        USART4->CR3 &= ~ ( USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN );

        /* Set the baud rate for 115200 bps (24 MHz / 115200). */
        USART4->BRR = 0x00D0UL; /* 208 */

        /* Enable USART. */
        USART4->CR1 |= USART_CR1_UE;
    }
#endif /* DEBUG */

    /*
     * Init the CRC unit.
     */

    /* Enable CRC AHB clock. */
    RCC->AHBENR |= RCC_AHBENR_CRCEN;

    /* Initialize CRC calculator. */
    /* Set initialization value. */
    CRC->INIT = 0xFFFFFFFF;

    /* Set bit-reversed output format (0x1) and
     * Bit reversal done by byte (0x1).
     * Polynomial size default (32-bit).
     */
    CRC->CR = CRC_CR_REV_OUT | CRC_CR_REV_IN_0;
}

__attribute__ ((always_inline))
__inline void hw_set_port_pin_state( enum MKG_io_port port, uint16_t pin,
                                     uint8_t state )
{
    GPIO_TypeDef *gpio = (GPIO_TypeDef*) port;

    if ( 0UL == IS_GPIO_ALL_INSTANCE( gpio ) )
    {
        return;
    }

    if ( state )
    {
        gpio->BSRR |= pin;
    }
    else
    {
        gpio->BRR |= pin;
    }
}

void hw_flush_uart( enum MKG_uart_port port )
{
    USART_TypeDef *uart = (USART_TypeDef*) port;

    if ( ( 0UL == port ) || ( 0UL == IS_UART_INSTANCE( uart ) ) )
    {
        return;
    }

    /* Clear FE flag. */
    if ( 0UL != ( uart->ISR & USART_ISR_FE ) )
    {
        uart->ICR |= USART_ICR_FECF;
    }

    /* Wait for RXNE is set or timeout. */
    while ( 0UL != ( uart->ISR & USART_ISR_RXNE ) )
    {
        uint8_t c __attribute__((unused));

        c = (uint8_t) ( uart->RDR & 0xFFU );
    }
}

uint16_t hw_transmit_by_uart( enum MKG_uart_port port, const uint8_t *data,
                              uint16_t len )
{
    USART_TypeDef *uart = (USART_TypeDef*) port;
    uint16_t cnt = 0U;

    if ( ( 0UL == port ) || ( ( (void*) 0UL ) == data ) || ( 0U == len )
         	|| ( 0UL == IS_UART_INSTANCE( uart ) ) )
    {
        return 0;
    }

    for ( cnt = 0; cnt < len; cnt++ )
    {
        /* Wait for shifting data from input data register (TDR)
         * to the output shift register.
         * Clearing the TXE bit is always performed
         * by a write to the transmit data register. */
        while ( 0UL == ( uart->ISR & USART_ISR_TXE ) )
        {
            /* For robust implementation, add here time-out management */
        }

        uart->TDR = (uint8_t) ( data[cnt] & 0xFFU );
    }

    /*  After writing the last data wait until TC=1. */
    while ( 0UL == ( uart->ISR & USART_ISR_TC ) )
    {
        /* For robust implementation, add here time-out management */
    }
    /* Clear transfer complete flag */
    USART1->ICR = USART_ICR_TCCF;

    return cnt;
}

uint16_t hw_receive_from_uart( enum MKG_uart_port port, uint8_t *data,
                               const uint16_t max_len, const uint32_t timeout )
{
    USART_TypeDef *uart = (USART_TypeDef*) port;
    uint16_t cnt = 0U; /* Read bytes. */
    uint8_t to = 0U; /* Timeout flag - no timeout. */
    uint32_t ts, tc; /* Ticks: start and current. */
    /* const uint32_t timeout = 1000UL; *//* Timeout. Experiments say about 27-28 ms
     delay in getting 9 bytes answer for 'AT' command. */

    if ( ( 0UL == port ) || ( ( (void*) 0UL ) == data ) || ( 0U == max_len )
         	|| ( 0UL == IS_UART_INSTANCE( uart ) ) )
    {
        return 0;
    }

    /* Start time. */
    ts = hw_get_tick_time();

    while ( ( cnt < max_len ) && !to )
    {
        /* Wait for RXNE is set or timeout. */
        while ( 0UL == ( uart->ISR & USART_ISR_RXNE ) )
        {
            /* Check out timeout. */
            tc = hw_get_tick_time();
            if ( ( tc - ts ) > timeout )
            {
                to = 1;
                break;
            }

            /* Clear FE flag. */
            if ( ( uart->ISR & USART_ISR_FE ) != 0 )
            {
                uart->ICR |= USART_ICR_FECF;
            }
        }

        /* Read data register, if RXNE was set (no timeout). */
        if ( !to )
        {
            uint32_t tl __attribute__((unused)); /* Ticks of the last read data. */

            data[cnt++] = (uint8_t) ( uart->RDR & 0xFFU );
            tl = hw_get_tick_time();
        }
    }

    return cnt;
}

/* Unlock an access to EEPROM. */
static void hw_unlock_eeprom( void )
{
    while ( 0UL != ( FLASH->SR & FLASH_SR_BSY ) )
    {
        /* For robust implementation, add here time-out management */
    }
    if ( 0UL != ( FLASH->PECR & FLASH_PECR_PELOCK ) )
    {
        FLASH->PEKEYR = 0x89ABCDEF; /* PEKEY1 */
        FLASH->PEKEYR = 0x02030405; /* PEKEY2 */
    }
}

/* Lock an access to EEPROM. */
static void hw_lock_eeprom( void )
{
    while ( 0UL != ( FLASH->SR & FLASH_SR_BSY ) )
    {
        /* For robust implementation, add here time-out management */
    }
    FLASH->PECR |= FLASH_PECR_PELOCK;
}

uint8_t hw_read_byte_data_eeprom( uint32_t addr )
{
    return *(uint8_t*) ( DATA_EEPROM_BASE + addr );
}

uint16_t hw_read_short_data_eeprom( uint32_t addr )
{
    return *(uint16_t*) ( DATA_EEPROM_BASE + addr );
}

uint32_t hw_read_word_data_eeprom( uint32_t addr )
{
    return *(uint32_t*) ( DATA_EEPROM_BASE + addr );
}

void hw_write_byte_data_eeprom( uint32_t addr, uint8_t byte )
{
    /* Unlock an access to EEPROM. */
    hw_unlock_eeprom();

    *(uint8_t*) ( DATA_EEPROM_BASE + addr ) = byte;

    /* Lock an access to EEPROM. */
    hw_lock_eeprom();
}

__attribute__((unused))
/* XXX Remember to align destination number to 16-bit boundery. */
void hw_write_short_data_eeprom( uint32_t addr, uint16_t ushort )
{
    /* Unlock an access to EEPROM. */
    hw_unlock_eeprom();

    *(uint16_t*) ( DATA_EEPROM_BASE + addr ) = ushort;

    /* Lock an access to EEPROM. */
    hw_lock_eeprom();
}

void hw_write_word_data_eeprom( uint32_t addr, uint32_t word )
{
    /* Unlock an access to EEPROM. */
    hw_unlock_eeprom();

    *(uint32_t*) ( DATA_EEPROM_BASE + addr ) = word;

    /* Lock an access to EEPROM. */
    hw_lock_eeprom();
}

void hw_enter_standby( void )
{
    /* Set DEEPSLEPP bit in ARM Core. */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    /* Set Power Down in Deepsleep. */
    PWR->CR |= PWR_CR_PDDS;
    /* Clear CWUF. */
    PWR->CR &= ~PWR_CR_CWUF;

    /* Enter in standby. */
    __WFI();
}

uint32_t hw_get_crc32( const uint8_t *data, uint32_t len )
{
    uint32_t i = 0U;

    CRC->CR |= CRC_CR_RESET;

    for ( i = 0U; i < ( len / 4U ); i++ )
    {
        CRC->DR = ( (uint32_t) data[4U * i] << 24U )
                | ( (uint32_t) data[4U * i + 1U] << 16U )
                | ( (uint32_t) data[4U * i + 2U] << 8U )
                | ( (uint32_t) data[4U * i + 3U] );
    }

    switch ( len % 4U )
    {
        default:
        case 0:
            break;

        case 1U:
            *(uint8_t volatile*) ( &CRC->DR ) = data[4U * i];
            break;

        case 2U:
            *(uint16_t volatile*) ( &CRC->DR ) =
                    ( (uint32_t) data[4U * i] << 8U )
                    	| (uint32_t) data[4U * i + 1U];
            break;
        case 3U:
            *(uint16_t volatile*) ( &CRC->DR ) =
                    ( (uint32_t) data[4U * i] << 8U )
                    	| (uint32_t) data[4U * i + 1U];
            *(uint8_t volatile*) ( &CRC->DR ) = data[4U * i + 2U];
            break;
    }

    return ( 0xffffffffU ^ CRC->DR );
}

/*** end of file ***/

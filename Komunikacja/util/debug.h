/*!****************************************************************************
*
* \file debug.h
*
* \copyright ELEKTROMETAL SA (c) 2019, Wszelkie prawa zastrzeżone
* \version $Revision: 291414 $
* \date $Date: 2020-08-24 10:31:30 +0200 (pon.) $
* \author $Author: kszczepanski $
*
******************************************************************************/

#ifndef DEBUG_H_
#define DEBUG_H_

/******************************************************************************
* Includes
******************************************************************************/

#include <stdint.h> /* Declarations of integer types with specified width. */
#include <stdio.h> /* Declarations of printf(). */
#include "../hw/mkg_hw.h" /* Declaration of hw_get_tick_time(). */

/******************************************************************************
* Defines and Enumerations
******************************************************************************/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef DEBUG
/* There is a conflict between CppCheck and GCC in interpreting 1st parameter of uint32_t type
 * (CppCheck expects unsigned long int type but GCC resolve uint32_t as long unsigned int
 * and gives warning "format '%u' expects argument of type 'unsigned int', but argument 2 has
 * type 'uint32_t {aka long unsigned int}' [-Wformat=]").
 * Better solution than suppressing CppCheck warning by putting the comment
 * "// cppcheck-suppress invalidPrintfArgType_uint" above pointed line
 * is casting to unsigned long.
 */
#define PRINT_DEBUG(...) \
    do { printf( "[DEBUG]\t%lu\t%s(), %s:%d\t", (unsigned long) hw_get_tick_time(), \
                 __func__, __FILE__ , __LINE__ ); \
         printf(__VA_ARGS__); printf("\n"); } while(0);
#else
#define PRINT_DEBUG(...)
#endif

#ifdef DEBUG
/* See note for DEBUG. */
#define PRINT_INFO(...) \
    do { printf( "[INFO]\t%lu\t%s(), %s:%d\t", (unsigned long) hw_get_tick_time(), \
                 __func__, __FILE__ , __LINE__ ); \
         printf(__VA_ARGS__); printf("\n"); } while(0);
#else
#define PRINT_INFO(...)
#endif

/* See note for DEBUG. */
#define PRINT_ERROR(...) \
    do { printf( "[ERROR]\t%lu\t%s(), %s:%d\t", (unsigned long) hw_get_tick_time(), \
                 __func__, __FILE__ , __LINE__ ); \
         printf(__VA_ARGS__); printf("\n"); } while(0);

/******************************************************************************
* Types and Typedefs
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

#endif /* DEBUG_H_ */

/*** end of file ***/

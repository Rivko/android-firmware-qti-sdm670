#ifndef BOOT_LOGGER_UART_H
#define BOOT_LOGGER_UART_H
/*===========================================================================

                    BOOT LOGGER UART HEADER FILE

DESCRIPTION
   This file contains the function interface for logging messages during boot
   to the uart buffer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/15   elt     Added boot_log_ram_to_uart
05/28/14   ck      Added optional data argument to boot_log_message_uart
08/20/13   lm      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/**
 * Each log message is prefixed with a timestamp, this defines how many digits
 * the timestamp should have, currenly we allow 9 digits to be displayed.
 */
#define LOG_MAX_NUM_DIGIT_DISPLAY    9

/**
 * Defines the max timestamp value we can display.
 * we only display 9 digits, anything beyond this value will overflow the buffer
 */
#define LOG_MAX_TIMESTAMP_VAL        1000000000

/**
* Uart buffer size 
*/
#define UART_BUF_SIZE	512

/*===========================================================================

**  Function :  boot_log_init_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will initiate the uart driver.
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_UART
  void boot_log_init_uart(void);
#else
  static inline void boot_log_init_uart(void)
  {
  }
#endif

/*===========================================================================

**  Function :  boot_log_deinit_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will deinitilize the uart driver.
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_UART
	void boot_log_deinit_uart(void);
#else
	static inline void boot_log_deinit_uart(void)
	{
	}
#endif
/*===========================================================================

**  Function :  boot_log_message_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will transmit the message available in the uart buffer.
*
* @param[in] m  log message
* @param[in] t  timestamp to log
* @param[in] l  type of the log
* @param[in] o  Optional data that can be added to entry
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_UART
void boot_log_message_uart(char *, uint32, char, char *);
#else
static inline void boot_log_message_uart(char * m, uint32 t, char l, char * c)
{
}
#endif

/*===========================================================================

**  Function :  boot_log_ram_to_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will dump the contents of the boot log ram buffer to the uart.
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#if (defined(FEATURE_BOOT_LOGGER_UART) &&        \
     defined(FEATURE_BOOT_LOGGER_RAM) &&         \
     defined(BOOT_ASYNC_UART_FEATURE))
void boot_log_ram_to_uart(void);
#else
static inline void boot_log_ram_to_uart(void)
  {
  }
#endif

#endif /*#ifndef BOOT_LOGGER_UART_H*/


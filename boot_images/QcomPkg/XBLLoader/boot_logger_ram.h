#ifndef BOOT_LOGGER_RAM_H
#define BOOT_LOGGER_RAM_H

/*===========================================================================

                            boot logger ram
                            Header File
 
GENERAL DESCRIPTION
  This header file contains declarations and definitions for Boot Logger
  RAM module.

Copyright 2010-2012, 2014 by Qualcomm Technologies Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  
when       who          what, where, why
--------   --------     ------------------------------------------------------
05/28/14   ck           Added optional data argument to boot_log_message_ram
08/30/12   dh           add a size argument to boot_log_copy_ram
01/11/11   dh           fix a function prototype spell
11/10/10   dh           Add boot_log_copy_ram api
08/11/10   dh           Initial port from 7x30 
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
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


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_log_init_ram

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will initiate the logging buffer and meta info in ram.
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
#ifdef FEATURE_BOOT_LOGGER_RAM
  void boot_log_init_ram(struct boot_log_init_data_t *);
#else
  static inline void boot_log_init_ram(struct boot_log_init_data_t *init_data)
  {
  }
#endif


/*===========================================================================

**  Function :  boot_log_message_ram

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will copy the given message to logging buffer and prefix the
* message by a given timestamp.
* The log_type is generally used to show what type of timing is used, but
* it could technically be used for other type of info.
*
* @param[in] m  log message
*
* @param[in] t  timestamp to log
*
* @param[in] l  type of the log
*
* @param[in] o  optional data to be printed
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
#ifdef FEATURE_BOOT_LOGGER_RAM
void boot_log_message_ram(char *, uint32 , char, char *);
#else
static inline void boot_log_message_ram(char * m, uint32 t, char l, char * c)
{
}
#endif


/*===========================================================================

**  Function :  boot_log_copy_ram

** ==========================================================================
*/
/*!
* 
* @brief
* Copy the log and meta info in one memory location to some other 
* memory location. This can be used to save log between sbl
*
* @param[in] dst_meta_info_addr  destination address to save meta info
*
* @param[in] dst_log_buf_addr  destination address to save message log
*
* @param[in] src_meta_info_addr  source address to read meta info
*
* @param[in] src_log_buf_addr  source address to read message log
*
* @param[in] log_buf_size  size of the message log to copy in bytes
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
#ifdef FEATURE_BOOT_LOGGER_RAM
  void boot_log_copy_ram(void *, void * , uint32, void *, void *, uint32);
#else
  static inline void boot_log_copy_ram
  (
    void *dst_meta_info_addr,
    void *dst_log_buf_addr,
    uint32 dst_log_buf_size,  
    void *src_meta_info_addr,
    void *src_log_buf_addr,
    uint32 src_log_buf_size
  )
  {
  }
#endif


#endif  /* BOOT_LOGGER_RAM_H */

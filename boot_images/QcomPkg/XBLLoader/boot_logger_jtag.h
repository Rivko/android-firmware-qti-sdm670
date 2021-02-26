#ifndef BOOT_LOGGER_JTAG_H
#define BOOT_LOGGER_JTAG_H

/*===========================================================================

                            boot logger jtag 
                              Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for outputting to
  a JTAG terminal window.

Copyright 2010, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------- 
10/05/15   ck      Corrected copyright
08/11/10   dh      Initial port from 7x30 
============================================================================*/
 
/*===========================================================================
                              INCLUDES
===========================================================================*/ 
 
#include "boot_comdef.h"
 
/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/ 
 
 
/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_log_jtag_dump_buffer

** ==========================================================================
*/
/*!
* 
* @brief
* Takes a pointer to a buffer and a buffer length.  It will output whatever
* is in the buffer to a JTAG terminal window, interpreting each byte as a char.
* This function will timeout, not outputting any character, if a JTAG terminal
* window is not open.
*
* @param[in] ptr  Address of the buffer
*
* @param[in] size  length of the buffer in bytes
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
#ifdef FEATURE_BOOT_LOGGER_JTAG
  void boot_log_jtag_dump_buffer(void *, uint32);
#else
  static inline void boot_log_jtag_dump_buffer(void * ptr, uint32 size)
  {
  }
#endif


/*===========================================================================

**  Function :  boot_log_jtag_term_putc

** ==========================================================================
*/
/*!
* 
* @brief
* Pushes a single character to the JTAG terminal window.
* It checks to make sure the previous character has been read by the terminal.
* If no JTAG terminal is open then it will skip any remaining output.  
* If it timeouts several times then it will never attempt
* to write out to JTAG again.
* This function only works on ARM7
*
* @param[in] c single character to display.
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
#ifdef FEATURE_BOOT_LOGGER_JTAG
  void boot_log_jtag_term_putc(char);
#else
  static inline void boot_log_jtag_term_putc(char c)
  {
  }
#endif

#endif /*BOOT_LOGGER_JTAG_H*/

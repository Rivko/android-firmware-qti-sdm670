#ifndef BOOT_STATISTICS_H
#define BOOT_STATISTICS_H

/*===========================================================================

                          Boot Statistics Interface

GENERAL DESCRIPTION
 Contains common data used for boot statistics

Copyright 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/14   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function : boot_statistics_flash_read_start

** ==========================================================================
*/
/*!
* 
* @brief  
*   Called before reading from flash.  This gets the current timer value and
*   stores it for use when boot_statistics_flash_read_stop is called.
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
void boot_statistics_flash_read_start(void);


/*===========================================================================

**  Function : boot_statistics_flash_read_stop

** ==========================================================================
*/
/*!
* 
* @brief  
*   Called after reading from flash.  This gets the current timer value
*   calculates the delta from the start value, and updates the flash statistics.
*
* @par Input
*   bytes_read    Number of bytes read from flash
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
void boot_statistics_flash_read_stop(uint32 bytes_read);


/*===========================================================================

**  Function : boot_statistics_log_flash

** ==========================================================================
*/
/*!
* 
* @brief  
*   Reports the flash throughput (speed, total bytes read, total time)
*   via the boot logger.
*
* @par Input
*   None
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
void boot_statistics_log_flash(void);


/*===========================================================================

**  Function : boot_statistics_reset_flash_byte_counter

** ==========================================================================
*/
/*!
* 
* @brief  
*   A counter seperate from the total flash bytes counter exists so functions
*   can access the number of flash bytes read during a particular period.  This
*   function resets that counter.
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
void boot_statistics_reset_flash_byte_counter(void);


/*===========================================================================

**  Function : boot_statistics_get_flash_byte_counter

** ==========================================================================
*/
/*!
* 
* @brief  
*   A counter seperate from the total flash bytes counter exists so functions
*   can access the number of flash bytes read during a particular period.  This
*   function returns the value in that counter.
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
uint32 boot_statistics_get_flash_byte_counter(void);


#endif /* BOOT_STATISTICS_H */

#ifndef BOOT_EDL_H
#define BOOT_EDL_H

/*===========================================================================

 Filename: boot_edl.h

 GENERAL DESCRIPTION: This file contains API's to put the device in EDL mode
 
   
Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who        what, where, why
--------   ---        ----------------------------------------------------------
07/14/17   Daison     Initial revision.

===========================================================================*/


/*=============================================================================

**  Function :  boot_dload_transition_pbl_forced_dload

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the magic numbers for PBL to detect and enter forced
*   download mode.  It then calls the target specific function to trigger a
*   system reset.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   Set's PBL magic numbers to enter forced download, resets target, never to
*   return.
* 
*/
void boot_dload_transition_pbl_forced_dload( void );


#endif
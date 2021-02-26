#ifndef BOOT_MC_H
#define BOOT_MC_H

/*=============================================================================

                              Boot Main Control

GENERAL DESCRIPTION
  This file contains the main control functions for SBL execution.

Copyright 2010-2011, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
02/01/11   dxiang  Add boot logical function pointer type 
07/22/10   aus     Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_sbl_shared.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* The boot procedure type */
typedef void (*boot_procedure_func_type)
(
  bl_shared_data_type *bl_shared_data
);

/* The boot logical type */
typedef boot_boolean (*boot_logical_func_type)
(
  bl_shared_data_type *bl_shared_data
);
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_do_procedures

** ==========================================================================
*/
/*!
* 
* @brief
*    Processes an array of boot procedures.
* 
* @param[in] bl_shared_data Pointer to the shared data passed between functions
* @param[in] procs          Pointer to the function pointer table 
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
void boot_do_procedures
(
  bl_shared_data_type *bl_shared_data,
  boot_procedure_func_type *procs
);

/*===========================================================================

**  Function :  boot_save_reset_register_log

** ==========================================================================
*/
/*!
* 
* @brief
*   Save reset registers to global variable register_log. This is used to 
*   debug hardware reset by dumping registers in downloader mode.
*  
* @param[in] bl_shared_data Pointer to the shared data passed between functions
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
void boot_save_reset_register_log
( 
  bl_shared_data_type *bl_shared_data 
);

#endif  /* BOOT_MC_H */

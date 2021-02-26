/*=============================================================================

                              Boot Main Control

GENERAL DESCRIPTION
  This file contains the main control functions for SBL execution.

Copyright 2010-2011, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   ck      Corrected copyright
03/21/11   dxiang  In boot_do_procedures(), do not force an error fatal if the 
                   procedures parameter is NULL
07/22/10   aus     Initial creation
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_error_handler.h"
#include "boot_mc.h"
#include "boot_sbl_shared.h"
#include "boot_util.h"
#include "err_reset_detect.h"
#include BOOT_PBL_H

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifdef FEATURE_MULTIPROCESSOR
/* Array to store the register data passed from Boot ROM */
err_reset_log_type   register_log;
#endif /* FEATURE_MULTIPROCESSOR */

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

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
)
{
  boot_procedure_func_type *cur_proc;

  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  if (procs != NULL)
  {
    for ( cur_proc = procs; *cur_proc != NULL; cur_proc++ )
    {
      (*cur_proc)( bl_shared_data );
    }
  }
} /* boot_do_procedures() */

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
)
{
#ifdef FEATURE_MULTIPROCESSOR
	/* Backup 26 uint32 reset registers stored by PBL at BOOT_STACK_BASE.
     SP and LR SYS, IRQ, FIQ, ABT, UDF - 10 uint32 registers
	 SVC_R0
	 SVC_R1 
	 ......
	 ......
	 SVC_SP <---- (BOOT_STACK_BAES - 0x8)
	 SVC_LR <---- (BOOT_STACK_BAES - 0x4)
	 SVC_PC <---- (BOOT_STACK_BASE)
    */
  qmemcpy((void *)&register_log, 
	      (const void *)(boot_pbl_get_boot_stack_base()
					   - sizeof(err_reset_log_type)), sizeof(err_reset_log_type));

#endif /* FEATURE_MULTIPROCESSOR */
} /* boot_save_reset_register_log() */

#ifndef BOOT_SBL_SHARED_H
#define BOOT_SBL_SHARED_H

/*===========================================================================

                          Boot Shared Header File

GENERAL DESCRIPTION
 Contains common defintion used by BOOT module.

Copyright 2010-2011, 2013-2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/16   kpa     relocate sbl_build_type to boot_build_component.h
06/13/16   digant  prototype for get_sync_type
11/09/15   plc     Added function to query current EL
10/05/15   ck      Corrected copyright
06/24/15   sng     BuildType Enum Updated to have DDI
10/14/14   kpa     Added sbl_build_type
02/04/14   ck      Removed exit_ptr as it's not needed in Bear family
01/15/14   ck      Changed sbl_exit_ptr_type to uint64
12/02/13   ck      Added SBL - QSEE interface structure
09/01/11   dh      Remove miheader.h from include headers
08/04/10   aus     Merged the bl_shared_data structure for all SBLs
07/22/10   aus     Merged the bl_shared_data structure for sbl2 and sbl3
05/03/10   plc     Initial creation for secboot 3.0
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include BOOT_PBL_H
#include "boot_comdef.h"
#include "boot_msm.h"
#include "boot_sbl_qsee_interface.h"
#include "boot_build_component.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define EL_3 0x3
#define EL_1 0x1

/* Forward declarations so that we don't have to include the full
 * header file for each module here */
struct bl_shared_data_target_type;

/* Function pointer type that will transfer control to AMSS. */
typedef uint64 (*sbl_exit_ptr_type)(void*);

/*---------------------------------------------------------------------------
  SBL SHARED DATA STRUCTURE
  
  NOTE: Only data that needs to be shared between modules should go
        in this structure
---------------------------------------------------------------------------*/
typedef struct bl_shared_data_type
{  
  /* Data shared from previous SBL */
  struct boot_sbl_if_shared_info_type         *sbl_shared_data;

  /* Target-specific shared data structure, if any */
  struct bl_shared_data_target_type           *target;

  /* Path to execute determined by build flags */
  sbl_build_type                              build_type;

  /* SBL - QSEE Interface Structure */
  struct boot_sbl_qsee_interface               sbl_qsee_interface;

} bl_shared_data_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Macro :  sbl_kick_watchdog

** ==========================================================================
*/
/*!
* 
* @brief
*    Watchdog timer circuit reset macro.
* 
* @par Dependencies
*   The watchdog needs to be kicked at least once every TBDms.
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#if !defined(BOOT_WATCHDOG_DISABLED)
#define SBL_KICK_WATCHDOG() \
          HWIO_OUT( WDOG_RESET, HWIO_FMSK( WDOG_RESET, WATCH_DOG ) )
#else
#define SBL_KICK_WATCHDOG()
#endif /* BOOT_WATCHDOG_DISABLED */
 
/*===========================================================================

**  Function :  sbl_loop_here

** ==========================================================================
*/
/*!
* 
* @brief
*  This function is called by boot_error_handler only. We perform
*  clean up of the registers and loop until JTAG is connected.
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
void sbl_loop_here( void );

/*===========================================================================

**  Function :  sbl1_get_current_el

** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns the value of the current exception level 
*  we are operating in 
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32  -  value of current exception level
* 
* @par Side Effects
*   None
* 
*/
uint32 sbl1_get_current_el( void );

/*===========================================================================

**  Function :  get_sync_type

** ==========================================================================
*/
/*!
* 
* @brief
*  This function is called by boot_sync_c_handler only. We check the 
*  synchronous exception type by reading the ESR_EL3 register.  The ISS 
*  bits of this register are dependent upon exception type, and could be 
*  made use of in future improvements.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32 sync_type
* 
* @par Side Effects
*   None
* 
*/
uint32 get_sync_type( void );

#endif /* BOOT_SBL_SHARED_H */

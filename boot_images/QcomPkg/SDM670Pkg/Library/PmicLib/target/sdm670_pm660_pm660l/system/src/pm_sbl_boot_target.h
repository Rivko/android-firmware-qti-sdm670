#ifndef __PMIC_SBL_BOOT_TARGET_H__
#define __PMIC_SBL_BOOT_TARGET_H__

/*! \file pm_sbl_boot_target.h
*  \n
*  \brief This file contains PMIC device initialization function and globals declaration.
*  \n
*  \n &copy; Copyright 2016 Qualcomm Technologies Inc, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   aab     Added active/retentsion level config reg address
02/05/16   aab     Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"



/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

#define MISC_SLAVE_ADDR                              0x0
#define MISC_BASE_ADDRESS                            0x900
#define MISC_VSET1_L5_RET_ADDR_LB                (MISC_BASE_ADDRESS + 0x4C)
#define MISC_VSET1_L5_RET_ADDR_UB                (MISC_BASE_ADDRESS + 0x4D)
#define MISC_VSET2_L5_ACTIVE_ADDR_LB                   (MISC_BASE_ADDRESS + 0x4E)
#define MISC_VSET2_L5_ACTIVE_ADDR_UB                   (MISC_BASE_ADDRESS + 0x4F)


/*===========================================================================

                        FUNCTION PROTOTYPES 

===========================================================================*/
pm_err_flag_type 
pm_check_pbs_ram_warm_reset_seq_presence(uint32 device_index);

#endif //__PMIC_SBL_BOOT_TARGET_H__

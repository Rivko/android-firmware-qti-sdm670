#ifndef __PM_PBS_H__
#define __PM_PBS_H__
/*! \file 
 *  \n
 *  \brief  pm_pbs.h ---- PMIC PBS driver
 *  \n
 *  \n This header file contains enums, macros and public API definitions for PMIC PBS driver.
 *  \n
 *  \n &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/16  aab     Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_lib_err.h"




/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/**
  @struct pm_pbs_ram_dataset_type
  @brief Type of data that described information regarding
         PBS User Space region.
  @
 */
typedef struct
{
  char word1; /* Val */
  char word2; /* Offset, Condition  */
  char word3; /* Base Addr, Test Mask */
  char word4; /* Command + SlaveID */
}pm_pbs_ram_dataset_type;


typedef enum
{
    PM_PBS_HEADER_SLEEP_DATASET,
    PM_PBS_HEADER_PON_REASON_DATASET,
    PM_PBS_HEADER_RTRR_DATASET,
    PM_PBS_HEADER_LPG_DATASET,
    PM_PBS_HEADER_SPARE_REG_DATASET,
    PM_PBS_HEADER_INVALID
} pm_pbs_header_dataset_type;


/*=========================================================================
                           Enums
==========================================================================*/


  

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/**
 * @brief This function is SMC Syscall to configure LPG/Spare Reg PBS dataset
 *
 * @details
 *
 * @param[in] pbs_ram_dataset_cfg: dataset type
         [in] pmic_index: pmic device number.
 *	 [in] dataset_size: PBS RAM user space size.
 *	 [in] dataset_ptr: pointer to a buffer contains data to be load onto RAM space.
 *
 * @return
 * int : +ve value on success otherwise -1
 */
int pm_pbs_ram_dataset_cfg_syscall
(
  pm_pbs_header_dataset_type pbs_ram_dataset_cfg,
  uint32  pmic_index,
  pm_pbs_ram_dataset_type* dataset_ptr,
  uint32  dataset_size
);


#endif /* __PM_PBS_H__ */

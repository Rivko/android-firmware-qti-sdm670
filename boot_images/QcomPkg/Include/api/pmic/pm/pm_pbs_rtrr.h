#ifndef __PM_PBS_RTRR_H__
#define __PM_PBS_RTRR_H__

/*! \file pm_pbs_rtrr.h
 *  \n
 *  \brief This header file contains API and type definitions for PBS Client driver.
 *  \n
 *  \n &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/pmic/pm/pm_pbs_rtrr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/07/16   akm      Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef struct
{
  uint32         version;                                    /* Starts from 1 and increments if we add more data */
  uint32         data[160];                                     /* RTRR data   */
} pm_pbs_rtrr_dump_data_type;

/*=========================================================================
      Function Definitions
==========================================================================*/

/** 
 * @name pm_pbs_rtrr_ram_dump 
 *  
 * @brief T
 * 
 * @param[in]  pmic_chip. Primary PMIC chip: 0. Secondary PMIC Chip: 1
 * @param[in]  data: Pointer to PBS RAM data. 
 * @param[in]  size: Size of PBS RAM data. 
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI errors.
 */

pm_err_flag_type
pm_pbs_rtrr_ram_dump(uint32 slave_id, pm_pbs_rtrr_dump_data_type *rtrr_info_ptr);

#endif /* __PM_PBS_RTRR_H__ */

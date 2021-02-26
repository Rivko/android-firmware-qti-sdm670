/*! \file pm_sbl_smem.c
*  \n
*  \brief This file contains PMIC Shared Memory initialization function.
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/16   aab     Added a call to pm_log_pon_reasons(); logs parsed PON Reasons.
07/11/16   aab     Added call to pm_sbl_schg_save_charger_info_to_smem() in pm_smem_init()
03/23/16   aab     Updated pm_pon_reasons_store_smem() to support GEN1/GEN2 PON peripheral
01/24/16   aab     Updated pm_pon_reasons_store_smem() to support PON GEN2 Module
07/14/15   as      PMIC index depends on target.
07/03/13   kt      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_pbs_info.h"
#include "pm_boot.h"
#include "pm_pon.h"
#include "smem.h"
#include "hw_module_type.h"
#include "pm_app_chgr.h"
#include "pm_chg.h"


/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/
static pm_err_flag_type pm_pon_reasons_store_smem (void) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint64*          pon_smem_ptr = NULL;
  uint64           pon_reason_arr[2]={0}; 
  uint8            pmic_index = (uint8) PMIC_A_SLAVEID_PRIM / 2;
  peripheral_info_type  pon_peripheral_info;
  uint8 sid_index = pmic_index * 2;

  
  err_flag |= pm_pon_get_all_pon_reasons(pmic_index, &pon_reason_arr[0]); //Get all the PON reasons

  pm_get_peripheral_info(sid_index, &pon_peripheral_info);
  if ( pon_peripheral_info.peripheral_subtype  >= PM_HW_MODULE_PON_LV_PON_GEN2_PRIMARY) 
  {
     err_flag |= pm_pon_get_fault_reasons(pmic_index,   &pon_reason_arr[1]); //Get Fault reasons
  }

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
     return err_flag;
  }
    
  /* Storing the PON reasons to shared memory */
  pon_smem_ptr = (uint64 *) smem_alloc( SMEM_POWER_ON_STATUS_INFO, sizeof(pon_reason_arr));

  if (pon_smem_ptr == NULL)
  {
      return PM_ERR_FLAG_INVALID;
  }

  DALSYS_memscpy(pon_smem_ptr, sizeof(pon_reason_arr), pon_reason_arr, sizeof(pon_reason_arr));
  
  return PM_ERR_FLAG_SUCCESS;
}


pm_err_flag_type pm_smem_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   //PMICs are Not currently supported in RUMI
  uint8 pmic_index = (uint8) PMIC_A_SLAVEID_PRIM / 2;
  pm_model_type pmic_model = PMIC_IS_INVALID;
  pmic_model = pm_get_pmic_model(pmic_index);
  if((pmic_model == PMIC_IS_UNKNOWN) || (pmic_model == PMIC_IS_INVALID))
  {
      return err_flag = PM_ERR_FLAG_SUCCESS; //Temp Fix:  PMICs are Not currently supported in RUMI
  }

  //store the PBS info in the global context
  err_flag = pm_pbs_info_store_glb_ctxt ();

  //store the PON reasons in the shared memory
  err_flag |= pm_pon_reasons_store_smem ();

  err_flag |= pm_log_pon_reasons(TRUE);

  err_flag |= pm_sbl_schg_info_to_smem_update();

  return err_flag; 
}


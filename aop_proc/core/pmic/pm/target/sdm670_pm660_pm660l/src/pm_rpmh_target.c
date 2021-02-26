/*! \file pm_rpmh_target.c
*  \n
*  \brief This file contains PMIC functions to run sleep enter and sleep exit settings.
*  \n
*  &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/target/sdm670_pm660_pm660l/src/pm_rpmh_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/15    kt      Initial version.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "CoreVerify.h"
#include "pm_comm.h"
#include "pm_target_information.h"
#include "pm_rpmh_target.h"
#include "pm_config.h"
#include "pm_prm_init.h"
#include "pm_rpmh.h"
#include "pm_vrm.h"
//#include "vrm_inc.h"


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

#define LDO_L5A_SLAVE_ADDR                          0x01
#define LDO_L5A_BASE_ADDR                           0x00004400
#define LDO_L5A_VSET_LB_ADDR                        (LDO_L5A_BASE_ADDR + 0x00000040)

#define LDO_L6A_SLAVE_ADDR                          0x01
#define LDO_L6A_BASE_ADDR                           0x00004500
#define LDO_L6A_VSET_LB_ADDR                        (LDO_L6A_BASE_ADDR + 0x00000040)

#define LDO_L5A_INDEX 4
#define LDO_L6A_INDEX 5

#define PMI_PMIC_INDEX  1  //PMIC index for interface pmic
#define PM_NUM_OF_PM8998_LDO   29
#define PM_GROOT               0x0

static boolean g_pm_stub_pmic_type = TRUE;

/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/**
 * @name pm_rpmh_sleep_init
 *
 * @brief Initializes the sleep settings LUT.
 *
 * @param  sleep_mode: can be vdd_min/XO shutdown
 *
 * @return None.
 *
 * @sideeffects None.
 *
 * @sideeffects Sleep settings should be available in config.
 *
 */
__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_rpmh_sleep_init()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    return err_flag;
}

/**
 * @name pm_rpmh_enter_sleep
 *
 * @brief This function calls the driver layer PMIC calls to set
 *        the Rails to the state prior to entering sleep.
 *
 * @param  sleep_mode: can be vdd_min/XO shutdown
 *
 * @return PMIC error code.
 *
 * @sideeffects Will over-write any client configuration
 *              requests. Any additions to the below function
 *              should be cautiously be added.
 *
 */
/*pm_err_flag_type pm_rpmh_enter_sleep (pm_sleep_mode_type sleep_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    return err_flag;
}*/

/**
 * @name pm_rpmh_exit_sleep
 *
 * @brief This function calls the driver layer PMIC calls to set
 *        the Rails back to the prior state of entering sleep.
 *
 * @param sleep_mode: can be vdd_min/XO shutdown
 *
 * @return PMIC error code.
 *
 * @sideeffects Will restore any client configuration requests.
 *              Any additions to the below function should be
 *              cautiously be added.
 *
 */
/*pm_err_flag_type pm_rpmh_exit_sleep (pm_sleep_mode_type sleep_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    return err_flag;
}*/

__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_rpmh_platform_pre_init(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   
   return err_flag;
   
}

__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_rpmh_platform_init(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   return err_flag;
}

__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_rpmh_platform_post_init(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   return err_flag;
}

pm_err_flag_type pm_rpmh_update_rsrc_config(vrm_pmic_rsrc_data *data, uint8 *access_allowed)
{

   vrm_result result = VRM_SUCCESS;

   if (g_pm_stub_pmic_type == TRUE)
   {
      data->mode_info.seq_type   = VRM_NO_PMIC_SEQ;
      data->volt_info.seq_type   = VRM_NO_PMIC_SEQ;
      data->hr_info.seq_type     = VRM_NO_PMIC_SEQ;
      data->enable_info.seq_type = VRM_NO_PMIC_SEQ;
      data->enable_info.current_state = data->enable_info.min_state;
      data->volt_info.current_state = data->volt_info.min_state;
      data->mode_info.current_state = data->mode_info.min_state;
      data->hr_info.current_state = data->hr_info.min_state;
      data->settling_en = 0;
      data->settling_err_en = 0;
      data->dep_en = 0;
      
      if (*access_allowed == TRUE)
      {
         result = vrm_register_rsrc(data);
         CORE_VERIFY(result == VRM_SUCCESS);
      }

      *access_allowed = FALSE;
   }

   return PM_ERR_FLAG_SUCCESS;
}

pm_err_flag_type pm_rpmh_pmic_detect_err_init(void)
{

   uint32 num_rsrc = 0;
   vrm_init();
   
   //Register for SMPS on PM660
   num_rsrc = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM, 0);
   pm_vrm_vreg_register_resources(0, PM_VRM_PERIPH_SMPS, num_rsrc);
   
   //Register for LDO on PM660
   num_rsrc = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, 0);
   pm_vrm_vreg_register_resources(0, PM_VRM_PERIPH_LDO, num_rsrc);
   
   // Register for CLK on PM660
   pm_vrm_clk_register_resources(0, PM_VRM_PERIPH_CLK, PM_MAX_NUM_CLKS - 1);
   
   //Register for SMPS on PM660L
   num_rsrc = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM, 1);
   pm_vrm_vreg_register_resources(1, PM_VRM_PERIPH_SMPS, num_rsrc);
   
   //Register for LDO on PM660L
   num_rsrc = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, 1);
   pm_vrm_vreg_register_resources(1, PM_VRM_PERIPH_LDO, num_rsrc);
   
   
   //Register for BOB on PM660L
   num_rsrc = pm_target_information_get_periph_count_info(PM_VRM_PERIPH_BOB, 1);
   pm_vrm_vreg_register_resources(1, PM_VRM_PERIPH_BOB, num_rsrc);
   
   
   pm_vrm_target_register_resources();

   vrm_post_init();

   pm_rpmh_init();
    
   pm_prm_init();

   return PM_ERR_FLAG_SUCCESS;
}


/**
 * @name pm_is_target_specific_stub_enable
 *
 * @brief Target specific Stubbed code can be added here based on RUMI/VIRTIO platform
 *
 * @param No param
 *
 * @return g_pm_stub_pmic_type
 *
 * @sideeffects Will stubbed pmic driver 
 *
 */

__attribute__((section("pm_cram_reclaim_pool")))
boolean pm_is_target_specific_stub_enable(void)
{
   if( PMIC_IS_PM660 == pm_get_pmic_model(PM_GROOT))
   {
      g_pm_stub_pmic_type = FALSE;
    }
   else
   {
      g_pm_stub_pmic_type = TRUE;
   }

    return g_pm_stub_pmic_type;

}
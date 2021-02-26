/*! \file vrm_config.c
*
*  \brief Contains target specific vrm based config data.
*  \n
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/670/vrm_config_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_config.h"
#include "vrm_utils.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_PBS_SOC_SLEEP_ARG 3
#define VRM_PBS_SOC_WAKE_ARG 4

//******************************************************************************
// Global Data
//******************************************************************************
// seq id values based on array index of seq start addr above
static uint32 vrm_seq_id_arr[VRM_MAX_SEQ] =
{
   [VRM_GENERIC_SEQ] = 0x0,
   [VRM_NO_PMIC_SEQ] = 0x1,
   [VRM_PMIC_WRITE_SEQ] = 0x2,
   [VRM_SPMI_BYTE_WRITE_SEQ] = 0x3,
};

static vrm_config_data vrm_config = 
{
   .vrm_slave_id = CMD_DB_SLV_ID_VRM,
   .min_settings_drv = RSC_DRV_GLOBAL_AOP,
   .seq_id_arr = vrm_seq_id_arr,
   .soc_pbs_sleep_arg = VRM_PBS_SOC_SLEEP_ARG,
   .soc_pbs_wake_arg = VRM_PBS_SOC_WAKE_ARG,
};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

//******************************************************************************
// API Functions
//******************************************************************************

vrm_config_data* vrm_get_config_data()
{
   uint32 drv_id = 0;

   CORE_VERIFY(DRV_ID_LOOKUP_SUCCESS == get_version_drv_id(RSC_DRV_GLOBAL_AOP, &drv_id));

   vrm_config.min_settings_drv = drv_id;

   return &vrm_config;
}


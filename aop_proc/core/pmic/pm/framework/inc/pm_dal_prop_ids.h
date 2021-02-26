#ifndef PM_DAL_PROP_IDS_H
#define PM_DAL_PROP_IDS_H
/*! \file pm_dal_prop_ids.h
 *  
 *  \brief   This file contains all the available PMIC DAL device config property values.
 *  \details This file contains all the available PMIC DAL device config property values.
 *  
 *  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/inc/pm_dal_prop_ids.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/13   hs      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

//PMIC HW peripheral specific properties -- starting from 1
enum
{
   PM_PROP_CLK_REG = 1,
   PM_PROP_SMPS_REG,
   PM_PROP_LDO_REG,
   PM_PROP_VS_REG,
   PM_PROP_BOB_REG,
   PM_PROP_UNIFIED_RAILS_STEPPER_RATE,
   PM_PROP_BOB_STEPPER_RATE,
   PM_PROP_SMPS_NUM,
   PM_PROP_LDO_NUM,
   PM_PROP_VS_NUM,
   PM_PROP_BOB_NUM,
   PM_PROP_SMPS_RAIL,
   PM_PROP_SMPS_RSRC,
   PM_PROP_SMPS_DEP,
   PM_PROP_LDO_RAIL,
   PM_PROP_LDO_RSRC,
   PM_PROP_LDO_DEP,
   PM_PROP_VS_RAIL,
   PM_PROP_VS_RSRC,
   PM_PROP_VS_DEP,
   PM_PROP_BOB_RAIL,
   PM_PROP_BOB_RSRC,
   PM_PROP_BOB_DEP,
   PM_PROP_CLK_INFO,
   PM_PROP_CLK_RSRC,
   PM_PROP_CLK_DEP,
   PM_PROP_CLK_COMMON,
   PM_PROP_TARGET_RSRC_INFO,
   PM_PROP_TARGET_RSRC_NUM,
   PM_PROP_RPMH_DRV_ID,
   PM_PROP_PRM_PAM_RSRCS,
   PM_PROP_PRM_PAM_RSRC_NUM,
   PM_PROP_PRM_DEVICE_RSRCS,
};

#endif // PM_DAL_PROP_IDS_H

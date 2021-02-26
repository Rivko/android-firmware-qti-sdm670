#ifndef __PM_VRM_H__
#define __PM_VRM_H__

/*! \file pm_vrm.h
 *  \n
 *  \brief This file contains PMIC VRM initialization related definitions.
 *  \n  
 *  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/vrm/inc/pm_vrm.h#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/16   kt     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "vrm_inc.h"
#include "pm_app_vrm.h"
#include "pm_vrm_vreg.h"
#include "pm_vrm_clk.h"
#include "pm_vrm_target.h"

/*===========================================================================

                     TYPE DEFINITIONS/MACROS

===========================================================================*/

#define PM_VREG_READY_IRQ_LATCHED_CLR_DATA 0x2
#define PM_STEPPER_DELAY_MASK 0x3

typedef struct
{
   char* name;
   char* alias_name;
}pm_pwr_resource;

typedef struct
{
   pm_pwr_resource  *parent; // parent resource
   pm_pwr_resource  **children; // array of child resources
   uint32           num_children; // num of children
}pm_pwr_resource_dependency_info;

//typedef struct
//{
//   pm_pwr_resource  *parent; // parent resource
//   pm_pwr_resource  **children; // array of child resources
//   uint32           reserved; // not used. kept to use auto-gen code
//}pm_pwr_resource_dependency_info;

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/

void pm_vrm_init(void);

pm_err_flag_type pm_vrm_get_drv_mask(uint32 drv_mask, uint32* ret_drv_mask);

#endif //__PM_VRM_H__



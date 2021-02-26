#ifndef __PM_CONFIG_TARGET_H__
#define __PM_CONFIG_TARGET_H__

/*! \file pm_config_target.h
*  \n
*  \brief Contains enums and macro definitions used in config data.
*  \n
*  \n &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/qcs605/pm_config_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/19/14   as      Created.
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_config.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

#define PM_DRV_NONE    0x0
#define PM_DRV_TZ     (0x1 << RSC_DRV_GLOBAL_TZ) 
#define PM_DRV_HYP    (0x1 << RSC_DRV_GLOBAL_HYP) 
#define PM_DRV_HLOS   (0x1 << RSC_DRV_GLOBAL_HLOS) 
#define PM_DRV_SEC    (0x1 << RSC_DRV_GLOBAL_SECPROC) 
#define PM_DRV_AUD    (0x1 << RSC_DRV_GLOBAL_AUDIO) 
#define PM_DRV_SEN    (0x1 << RSC_DRV_GLOBAL_SENSORS) 
#define PM_DRV_AOP    (0x1 << RSC_DRV_GLOBAL_AOP) 
#define PM_DRV_DBG    (0x1 << RSC_DRV_GLOBAL_DEBUG) 
#define PM_DRV_GPU    (0x1 << RSC_DRV_GLOBAL_GPU) 
#define PM_DRV_DIS    (0x1 << RSC_DRV_GLOBAL_DISPLAY) 
#define PM_DRV_MDMS   (0x1 << RSC_DRV_GLOBAL_MODEM_SW) 
#define PM_DRV_MDMH   (0x1 << RSC_DRV_GLOBAL_MODEM_HW) 
#define PM_DRV_COM    (0x1 << RSC_DRV_GLOBAL_COMPUTE_DSP) 
#define PM_DRV_ARC    (0x1 << RSC_DRV_GLOBAL_ARC_CPRF)

typedef enum
{
  PM_NONE_MODE_VAL     = 0x0,
  PM_RET_MODE_VAL      = 0x4,
  PM_LPM_MODE_VAL      = 0x5,
  PM_AUTO_MODE_VAL     = 0x6,
  PM_NPM_MODE_VAL      = 0x7,
  PM_INVALID_MODE_VAL  = 0x8,
}pm_mode_val_type; 

#endif // __PM_CONFIG_TARGET_H__



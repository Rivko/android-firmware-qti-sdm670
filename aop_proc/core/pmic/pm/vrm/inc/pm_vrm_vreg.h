#ifndef __PM_VRM_VREG_H__
#define __PM_VRM_VREG_H__

/*! \file pm_vrm_vreg.h
 *  \n
 *  \brief This file contains PMIC VRM VREG related definitions.
 *  \n  
 *  \n &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/vrm/inc/pm_vrm_vreg.h#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/16   kt     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm.h"
#include "pm_smps_driver.h"
#include "pm_ldo_driver.h"
#include "pm_vs_driver.h"
#include "pm_bob_driver.h"

/*===========================================================================

                     TYPE DEFINITIONS/MACROS

===========================================================================*/

typedef struct 
{
    unsigned        AccessAllowed:1;        // 0 – No voting Access; 1 – Voting access allowed
    unsigned        AlwaysOn:1;             // 0 - allow to be turned off; 1 - always on
    unsigned        MinMode:4;              // 0 - AUTO, 1- IPEAK, 2 - NPM
    unsigned        SafetyHR:26;            // Safety Headroom in mV
    // 32 - bit
    unsigned        MinVoltage:16;          // in mV
    unsigned        MaxVoltage:16;          // in mV
    // 32 - bit
    unsigned        EnableFixedTime:16;            // in micro secs
    unsigned        EnableStepperWarmUpTime:16;    // in micro secs
    // 32 - bit
    unsigned        VoltFixedUpTime:16;            // in micro secs
    unsigned        VoltFixedDownTime:16;          // in micro secs
    // 32 - bit
    unsigned        VoltStepperExtraUpTime:16;     // in micro secs
    unsigned        VoltStepperExtraDownTime:16;   // in micro secs
    // 32 - bit
    unsigned        VoltDischargeTime:16;          // in micro secs
    unsigned        ModeFixedTime:16;              // in micro secs
    // 32 - bit
    unsigned        VregReadyEn:1;          // VREG_READY based settling or not  
    unsigned        VregReadyErrEn:1;       // VREG_READY settling error enabled or not 
    unsigned        VregWdogTimer:5;        // Needed only if VREG_READY settling error enabled
    unsigned        AopHandling:1;          // AopHandling
    unsigned        VoltSeqType:8;          // Voltage sequence type
    unsigned        EnableSeqType:8;        // Enable sequence type
    unsigned        ModeSeqType:8;          // Mode sequence type
    // 32 - bit  
    unsigned        HrSeqType:8;            // Headroom sequence type
    unsigned        DrvOwnMask:16;          // Owner drv ids mask
    unsigned        Reserved:8;             // Reserved
    // 32 - bit
}pm_vrm_vreg_rail_info_type;

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/

void pm_vrm_vreg_pre_register_resource_dependencies(uint8 pmic_index, pm_vrm_periph_type vreg_type, uint32 num_rails);
void pm_vrm_vreg_register_resources(uint8 pmic_index, pm_vrm_periph_type vreg_type, uint32 num_rails);


#endif //__PM_VRM_VREG_H__



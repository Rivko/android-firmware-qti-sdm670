#ifndef __PM_VRM_TARGET_H__
#define __PM_VRM_TARGET_H__

/*! \file pm_vrm_target.h
 *  \n
 *  \brief This file contains PMIC VRM TARGET specific resource related definitions.
 *  \n  
 *  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/target/qcs605_pm660_pm8005/inc/pm_vrm_target.h#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/19/14   as      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm.h"

/*===========================================================================

                     TYPE DEFINITIONS/MACROS

===========================================================================*/

typedef struct 
{
    unsigned        AccessAllowed:1;        // 0 – No voting Access; 1 – Voting access allowed
    unsigned        SlaveId:4;              // rsrc slave id
    unsigned        BaseAddr:16;            // rsrc base address
    unsigned        RsrcCategory:3;         // rsrc category
    unsigned        EnableAddrOff:8;        // Enable addr offset
    // 32 - bit
    unsigned        EnableFixedTime:16;     // in micro secs
    unsigned        EnableSeqType:8;        // Enable sequence type
    unsigned        ModeAddrOff:8;          // Mode addr offset
    // 32 - bit
    unsigned        ModeFixedTime:8;        // in micro secs
    unsigned        ModeSeqType:8;          // Mode sequence type
    unsigned        VoltAddrOff:8;          // Mode addr offset
    // 32 - bit
    unsigned        VoltFixedTime:8;        // in micro secs
    unsigned        VoltSeqType:8;          // Mode sequence type
    unsigned        MaxVoltage:16;          // Max Voltage
    // 32 - bit
    unsigned        DrvOwnMask:16;          // Owner drv ids mask
    unsigned        Reserved:16;            // Reserved
    // 32 - bit
}pm_vrm_rsrc_info_type;

typedef struct 
{
   char* name;                    // Resource name
   uint32 PmicIndex;              // PMIC index
   pm_vrm_periph_type PeriphType; // PMIC VRM APP peripheral type
   uint32 PeriphId;               // Peripheral Id
   pm_vrm_rsrc_info_type config;
}pm_vrm_target_rsrc_info_type;

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/

//To rgister pre dependcency info for target specific resources
void pm_vrm_target_pre_register_resource_dependencies(void);

// Target specific resources that needs to be registered in VRM (example: pbs, gpio)
void pm_vrm_target_register_resources(void);

#endif //__PM_VRM_TARGET_H__



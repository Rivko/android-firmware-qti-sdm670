#ifndef __PM_PRM_DEVICE_DEFS_H__
#define __PM_PRM_DEVICE_DEFS_H__

/*===========================================================================
DESCRIPTION
This file contains prototype definitions prm device layer 

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/inc/pm_prm_device_defs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   akm     Created
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "pm_prm_device.h"

/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

DEFINITIONS

===========================================================================*/
//VREG definitions for scalar configuration
#define PM_PRM_CLIENT_VREG_DEP(chipid, rtype, index) \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,mode), #rtype#chipid#index, VRM_RSRC_REGULATOR, VRM_MODE,    0, UNPA_CLIENT_REQUIRED, 0}, \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,mV),   #rtype#chipid#index, VRM_RSRC_REGULATOR, VRM_VOLTAGE, 0, UNPA_CLIENT_REQUIRED, 0}, \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,en),   #rtype#chipid#index, VRM_RSRC_REGULATOR, VRM_ENABLE,  0, UNPA_CLIENT_REQUIRED, 0}

#define PM_PRM_DEV_RSRC_VREG_MODE(chipid, rtype, index) \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,mode), #rtype#chipid#index, VRM_RSRC_REGULATOR, VRM_MODE, 0, 0, 0}

#define PM_PRM_DEV_RSRC_VREG_VOLTAGE(chipid, rtype, index) \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,mV),   #rtype#chipid#index, VRM_RSRC_REGULATOR, VRM_VOLTAGE, 0, 0, 0}

#define PM_PRM_DEV_RSRC_VREG_ENABLE(chipid, rtype, index) \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,en),   #rtype#chipid#index, VRM_RSRC_REGULATOR, VRM_ENABLE, 0, 0, 0}

//SMPS definitions for scalar configuration
#define PM_PRM_CLIENT_SMPS_DEP(chipid, rtype, index) PM_PRM_CLIENT_VREG_DEP(chipid, rtype, index)

//LDO definitions for scalar configuration
#define PM_PRM_CLIENT_LDO_DEP(chipid, rtype, index) PM_PRM_CLIENT_VREG_DEP(chipid, rtype, index)

//CLK definitions for scalar configuration
#define PM_PRM_CLIENT_CLK_DEP(chipid, rtype, index) \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,en),   #rtype#chipid#index, VRM_RSRC_XOB, VRM_ENABLE,  0, UNPA_CLIENT_REQUIRED, 0}

#define PM_PRM_DEV_RSRC_CLK(chipid, rtype, index) \
    {PM_PRM_DEV_RSRC_NAME(chipid,rtype,index,en), #rtype#chipid#index, VRM_RSRC_XOB, VRM_ENABLE, 0, 0, 0}

#endif // __PM_PRM_DEVICE_DEFS_H__

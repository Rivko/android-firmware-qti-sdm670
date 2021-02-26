#ifndef __PM_PRM_DEVICE_DEFS_H__
#define __PM_PRM_DEVICE_DEFS_H__
/*===========================================================================
DESCRIPTION
This file contains prototype definitions prm device layer 

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/inc/vector/pm_prm_device_defs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/21/16   rl      Created
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "pm_prm_device.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_vs.h"
#include "pm_clk.h"
#include "npa.h"
#include "npa_resource.h"

/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

DEFINITIONS

===========================================================================*/    

#define PM_PRM_DEP_CONSTRUCT(RES_NAME)       \
    {                                        \
        RES_NAME,                            \
        NPA_CLIENT_VECTOR,                   \
    },                                       \
    {                                        \
        RES_NAME,                            \
        NPA_CLIENT_SUPPRESSIBLE_VECTOR,      \
    }

//CLK definitions for vector configuration
#define PM_PRM_CLIENT_CLK_DEP(chip, rtype, idx) \
    PM_PRM_DEP_CONSTRUCT(PM_PRM_RSRC_NAME(chip, rtype, idx, vec))

#define PM_PRM_DEV_RSRC_CLK(chip, rtype, idx, pmic_rtype, ctrl, rnum)    \
  {                                                                      \
    .rsrc_name      = PM_PRM_RSRC_NAME(chip,rtype,idx,vec),              \
    .rsrc_node      = PM_PRM_NODE_NAME(chip,rtype,idx),                  \
    .rsrc_type      = pmic_rtype,                                        \
    .rsrc_vrm_type  = PM_PRM_RSRC_VRM_CLK,                               \
    .cmd_db_name    = #rtype#chip#idx,                                   \
    .rsrc_units     = "on_off",                                          \
    .pmic_chip      = #chip,                                             \
    .ctrl_type      = ctrl,                                              \
    .periph_id      = rnum,                                              \
    .rsrc_key       = PM_RSRC_KEY_VEC,                                   \
    .pam_req        = 0                                                  \
  }
    
//VS definitions for vector configurations
#define PM_PRM_CLIENT_VREG_DEP(chip, rtype, idx) \
    PM_PRM_DEP_CONSTRUCT(PM_PRM_RSRC_NAME(chip, rtype, idx, vec))


#define PM_PRM_DEV_RSRC_VREG(chip, rtype, idx, pmic_rtype, ctrl, rnum)   \
  {                                                                      \
    .rsrc_name      = PM_PRM_RSRC_NAME(chip,rtype,idx,vec),              \
    .rsrc_node      = PM_PRM_NODE_NAME(chip,rtype,idx),                  \
    .rsrc_type      = pmic_rtype,                                        \
    .rsrc_vrm_type  = PM_PRM_RSRC_VRM_VREG,                              \
    .cmd_db_name    = #rtype#chip#idx,                                   \
    .rsrc_units     = "on_off",                                          \
    .pmic_chip      = #chip,                                             \
    .ctrl_type      = ctrl,                                              \
    .periph_id      = rnum,                                              \
    .rsrc_key       = PM_RSRC_KEY_VEC,                                   \
    .pam_req        = 0                                                  \
  }

  

uint32
pm_prm_process_rsrc (npa_client_handle client,
                     npa_resource *resource,
                     npa_resource_state mode_id,
                     uint32 index, uint32 dep_idx,
                     uint32 *curr_msg_id);



#endif // __PM_PRM_DEVICE_DEFS_H__

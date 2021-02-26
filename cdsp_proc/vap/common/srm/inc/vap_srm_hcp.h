/**
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef _VAP_SRM_HCP_H_
#define _VAP_SRM_HCP_H_

#include "vap_srm_internal.h"

/*===========================================================================
 Defines and Constants
===========================================================================*/

/*===========================================================================
 Type definitions
===========================================================================*/


/*===========================================================================
 Functions
===========================================================================*/
int vap_srmHcpResourcesInit(int client_handle);
int vap_srmHandleHcpSidRegGet(
        unsigned int u32Pid,
        unsigned int *pu32Val);
int vap_srmHandleHcpSidRegSet(
        unsigned int u32Pid,
        unsigned int u32Val,
        unsigned int *pu32Err,
        unsigned int *pu32ValBefore,
        unsigned int *pu32ValAfter);
t_StMapNode* vap_srm_hcpGetMapEntries();
#endif //_VAP_SRM_HCP_H_

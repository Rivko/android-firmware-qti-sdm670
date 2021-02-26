/**
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef _VAP_SRM_INTERNAL_H_
#define _VAP_SRM_INTERNAL_H_

/*===========================================================================
 Defines and Constants
===========================================================================*/

/*===========================================================================
 Type definitions
===========================================================================*/

/**
* @enum
* @brief Error flags. Add new error conditions here.
*/
enum{
    ERR_FLAGSHIFT_INVALID_PID,
    ERR_FLAGSHIFT_INVALID_VA,
    ERR_FLAGSHIFT_HCP_SID_VERIFICATION_FAIL,
    ERR_FLAGSHIFT_HCP_UNAUTHORIZED_ACCESS,
////////////////////////////////////
    ERR_FLAGSHIFT_MAX = 31
}t_enVapSrmErrFlagShift;

/**
* @struct   t_StMapNode
* @brief    Secure resource item.
*/
typedef struct StMapNode{
    struct StMapNode *pNext;
    unsigned pageno_virt;        /* Virtual page number */
    unsigned pageno_phys;        /* Physical page number */
    unsigned int start_vaddr_l;
    unsigned int start_vaddr_h;
    unsigned int u32Size;
}t_StMapNode;

/*===========================================================================
 Functions
===========================================================================*/

#endif //_VAP_SRM_INTERNAL_H_

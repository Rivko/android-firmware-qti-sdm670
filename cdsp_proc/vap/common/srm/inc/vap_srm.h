/**
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef _VAP_SRM_H_
#define _VAP_SRM_H_

/*===========================================================================
 Defines and Constants
===========================================================================*/
#define VAP_SRM_DRIVER_NAME "VAP_SRM_DRIVER"
#define VAP_SRM_HCP_SID_REG_SET             (0+QDI_PRIVATE)
#define VAP_SRM_HCP_SID_REG_GET             (1+QDI_PRIVATE)
#define VAP_SRM_OWNER_SET                   (2+QDI_PRIVATE)
#define VAP_SRM_OWNER_RELEASE               (3+QDI_PRIVATE)
#define VAP_SRM_GET_HCP_SID_INFO            (4+QDI_PRIVATE)
/*===========================================================================
 Type definitions
===========================================================================*/
/**
* @enum
* @brief    Error IDs.
*/
typedef enum{
    SRM_ERR_SUCCESS = 0,
    SRM_ERR_FAIL,/*< Generic  */
    SRM_ERR_NULLPTR,/*< NULL Ptr  */
    SRM_ERR_MEM_FAIL,/*< Memory failure: failed to allocate or failed to access  */
    SRM_ERR_INVALID_PARAM,/*< Invalid parameter  */
    SRM_ERR_INVALID_STATE,/*< Resource is in invalid state  */
    SRM_ERR_NO_RESOURCE,/*< Resources unavailable  */
    SRM_ERR_INCOMPLETE_CONFIG,/*< Current configuration is not complete/valid  */
    SRM_ERR_INTERRUPTED,/*< Operation interrupted  */
    /////////////////////////////////////////////////////////
    SRM_ERR_NUM
}t_enVapSrmErr;

/**
* @enum
* @brief Owner ID.
*/
enum{
    OWNER_ID_DEFAULT,
    OWNER_ID_HCP,
    OWNER_ID_DMA,
///////////////////////////
    OWNER_ID_MAX
}t_enVapSrmOwnerId;

/*===========================================================================
 Functions
===========================================================================*/
int vap_srmInit(void);

#endif //_VAP_SRM_H_

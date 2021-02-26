#ifndef __HDCPSRM_H__
#define __HDCPSRM_H__

/*===========================================================================
   Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/15/17   aw     Initial version

===========================================================================*/

#define HDCP_SRM_CMD_UPDATE_SRM 1
#define HDCP_SRM_CMD_CHECK_DEVICE_ID 2
#define HDCP_SRM_CMD_GET_VERSION 3
#define HDCP_SRM_CMD_INIT_SRM 4
#define HDCP_SRM_CMD_REMOVE_SRM 5
#define HDCP_SRM_CMD_INVALID 6

typedef struct {
    uint32_t command_id;
    int32_t retval;
} hdcp_srm_rsp_t;

typedef struct {
    uint32_t command_id;
    uint32_t srm_length;
    uint8_t srm_data[1];
} hdcp_srm_update_srm_req_t;

typedef struct {
    uint32_t command_id;
    int32_t retval;
} hdcp_srm_update_srm_rsp_t;

typedef struct {
    uint32_t command_id;
    uint32_t num_device_ids;
    uint8_t device_ids[1];
} hdcp_srm_check_device_ids_req_t;

typedef struct {
    uint32_t command_id;
    int32_t retval;
} hdcp_srm_check_device_ids_rsp_t;

typedef struct {
    uint32_t command_id;
    int32_t retval;
    uint16_t srm_version;
} hdcp_srm_get_srm_version_rsp_t;

typedef struct {
    uint32_t command_id;
    int32_t retval;
} hdcp_srm_init_srm_rsp_t;

typedef struct {
    uint32_t command_id;
    int32_t retval;
} hdcp_srm_remove_srm_rsp_t;

int32_t SRM_initSRM();
int32_t SRM_updateSRM(const void *srmData_ptr, size_t srmData_len);
int32_t SRM_validateRecvIds(const uint8_t *recvIdList_ptr, size_t recvIdList_num);
int32_t SRM_getCurrentSRMVersion(uint32_t *version_ptr);

#endif

/** @file  
                        km_defs.h

  Header file containing the data types needed to communicate with TZ app (keymaster)

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 07/28/16   SA     Initial version

=============================================================================*/

#ifndef KM_DEF_H
#define KM_DEF_H

#define KEYMASTER_CMD_ID 0x100UL
#define KEYMASTER_UTILS_CMD_ID 0x200UL

/* Supported Keymaster commands */
typedef enum {
    KEYMASTER_DELETE_ALL_KEYS				= (KEYMASTER_CMD_ID + 14UL),
    KEYMASTER_GET_VERSION                   = (KEYMASTER_UTILS_CMD_ID + 0UL),
    KEYMASTER_SET_ROT						= (KEYMASTER_UTILS_CMD_ID + 1UL),
    KEYMASTER_READ_LK_DEVICE_STATE			= (KEYMASTER_UTILS_CMD_ID + 2UL),
    KEYMASTER_WRITE_LK_DEVICE_STATE			= (KEYMASTER_UTILS_CMD_ID + 3UL),
    KEYMASTER_MILESTONE_CALL				= (KEYMASTER_UTILS_CMD_ID + 4UL),
    KEYMASTER_SET_BOOT_STATE                = (KEYMASTER_UTILS_CMD_ID + 8UL),
    KEYMASTER_LAST_CMD_ENTRY				= (int)0xFFFFFFFFULL
} keymaster_cmd_t;


/**
  Data structure for sending KEYMASTER_SET_ROT command to keymaster
  
  @param[in]   cmd_id      Requested command

**/
typedef struct _km_get_version_req_t {
	UINT32 cmd_id;
}__attribute__ ((packed)) km_get_version_req;

/**
  Data structure for receiving KEYMASTER_SET_ROT command's reponse

  @param[out]   status                Status of the request
  @param[out]   major_version         major version
  @param[out]   minor_version         minor version
  @param[out]   ta_major_version      TA major version
  @param[out]   ta_minor_version      TA minor version

**/
typedef struct _km_get_version_rsp_t {
	INT32 status;
        UINT32 major_version;
        UINT32 minor_version;
        UINT32 ta_major_version;
        UINT32 ta_minor_version;
}__attribute__ ((packed)) km_get_version_rsp;

/**
  Data structure for sending KEYMASTER_SET_ROT command to keymaster

  @param[in]   cmd_id      Requested command
  @param[in]   rot_ofset   Offset from the top of the struct.
  @param[in]   rot_size    Size of the ROT
  
**/
typedef struct _km_set_rot_req_t {
	UINT32 cmd_id;
	UINT32 rot_ofset;
	UINT32 rot_size;
}__attribute__ ((packed)) km_set_rot_req;

/**
  Data structure for receiving KEYMASTER_SET_ROT command's reponse
  
  @param[out]   status      Status of the request
  
**/
typedef struct _km_set_rot_rsp_t {
	INT32 status;
}__attribute__ ((packed)) km_set_rot_rsp;

/**
  Data structure for sending KEYMASTER_DELETE_ALL_KEYS command 
  
  @param[in]   cmd_id      Requested command
  
**/
typedef struct _km_reset_req_t {
  UINT32 cmd_id;
}__attribute__ ((packed))km_reset_req;


/**
  Data structure for receiving the response of KEYMASTER_DELETE_ALL_KEYS command 
  
  @param[in]   status     Status of the request
  
**/
typedef struct _km_reset_rsp_t {
  INT32 status;
}__attribute__ ((packed))km_reset_rsp;

/**
  Data structure for sending KEYMASTER_MILESTONE_CALL command
  
  @param[in]   cmd_id      Requested command
  
**/
typedef struct _km_set_milestone_req_t {
  UINT32 cmd_id;
}__attribute__ ((packed))km_set_milestone_req;

/**
  Data structure for receiving the response of KEYMASTER_DELETE_ALL_KEYS command 
  
  @param[in]   status     Status of the request
  
**/
typedef struct _km_set_milestone_rsp_t {
  INT32 status;
}__attribute__ ((packed))km_set_milestone_rsp;

/**
  Data structure for sending KEYMASTER_WRITE_LK_DEVICE_STATE & KEYMASTER_READ_LK_DEVICE_STATE commands to keymaster
  
  @param[in]   cmd_id      Requested command
  @param[in]   data        pointer address to the buffer for read and write
  @param[in]   len         size of buffer
  
**/
typedef struct km_send_devrw_req_t
{
	UINT32 cmd_id;
	UINT32 data;
	UINT32 len;
}__attribute__ ((packed)) km_send_devrw_req;

/**
  Data structure for receiving KEYMASTER_WRITE_LK_DEVICE_STATE & KEYMASTER_READ_LK_DEVICE_STATE commands to keymaster
  We only use status to check the status of the request 
  
  @param[in]   status   Status of the request      
  
**/
typedef struct km_send_devrw_rsp_t
{
	UINT32 cmd_id;
	UINT32 data;
	INT32 status;
}__attribute__ ((packed)) km_send_devrw_rsp;

#endif


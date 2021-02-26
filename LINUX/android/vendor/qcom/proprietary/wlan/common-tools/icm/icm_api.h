/*
 * Copyright (c) 2012 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * =====================================================================================
 *
 *       Filename:  icm_api.h
 *
 *    Description:  API definitions for ICM
 *
 *        Version:  1.0
 *        Created:  05/14/2012 12:13:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */

#ifndef _ICM_API_H_
#define _ICM_API_H_

#include "icm.h"

#define MAX_NUM_APS 10

#ifndef __ATTRIB_PACK
#define __ATTRIB_PACK __attribute__ ((packed))
#endif

typedef enum ICM_MSG_ID {
    SET_CHANNEL_SELECTION_MODE_REQ,
    SET_CHANNEL_SELECTION_MODE_RSP,
    GET_RANKED_CHANNELS_REQ,
    GET_RANKED_CHANNELS_RSP,
    EXCLUDE_CHANNEL_REQ,
    EXCLUDE_CHANNEL_RSP,
    PENALIZE_CHANNELS_BY_SSID_REQ,
    PENALIZE_CHANNELS_BY_SSID_RSP,
    GET_STATE_REQ,
    GET_STATE_RSP,
    CANCEL_SCAN_REQ,
    CANCEL_SCAN_RSP,
    CHANNEL_SET_REQ,
    CHANNEL_SET_RSP,
}ICM_MSG_ID_T;


typedef enum ICM_RSP_STATUS {
    ICM_RSP_STATUS_SUCCESS,
    ICM_RSP_STATUS_SCAN_IN_PROGRESS,
    ICM_RSP_STATUS_NOT_INITIALIZED,
    ICM_RSP_STATUS_INVALID_PARAM,
    ICM_RSP_STATUS_SCAN_CANCELLED,
    ICM_RSP_STATUS_NO_MATCHING_APS,
    ICM_RSP_STATUS_FAILURE=0xff,
} ICM_RSP_STATUS_T;

typedef struct set_channel_selection_mode_req {
    u_int8_t  band;
    u_int8_t  channelConfig;
    u_int8_t  selectionMode;
    u_int8_t  homeChannel;
} __ATTRIB_PACK SET_CHANNEL_SELECTION_MODE_REQ_T;

typedef struct set_channel_selection_mode_rsp {
    u_int8_t status;
} __ATTRIB_PACK SET_CHANNEL_SELECTION_MODE_RSP_T;

typedef struct icm_msg {
    u_int8_t messageId;
    u_int8_t transactionId;
    u_int16_t len;
    /* Will contain rest of data */
    u_int8_t data[0];
}  __ATTRIB_PACK ICM_MSG_T;

typedef struct get_ranked_channels_req {
    u_int8_t    band;
    u_int8_t    channelConfig;
    char        deviceName[IFNAMSIZ];
    u_int8_t    scanType;
    u_int8_t    maxChannels;
    u_int8_t    maxAPsPerChannel;
} __ATTRIB_PACK GET_RANKED_CHANNELS_REQ_T;

typedef struct ap_info {
    u_int8_t    ssidLen;
    u_int8_t    ssid[MAX_SSID_LEN];
    u_int8_t    bssid[ETH_ALEN];
    int8_t      rssi;
    u_int8_t    channelUtilization;
} __ATTRIB_PACK AP_INFO_T;

typedef struct  channel_info {
    u_int8_t    channelId;
    u_int16_t   usability;
    u_int8_t    numAPs;
    /* Will contain array of AP_INFO_T elements */
    AP_INFO_T   accessPoints[0];
} __ATTRIB_PACK CHANNEL_INFO_T;

typedef struct get_ranked_channels_rsp {
    u_int8_t       status;
    u_int8_t       numChannels;
    /* Will contain array of CHANNEL_INFO_T elements */
    CHANNEL_INFO_T channels[0];
} __ATTRIB_PACK GET_RANKED_CHANNELS_RSP_T;

typedef struct exclude_channel_req {
    u_int8_t    band;
    u_int8_t    channelConfig;
    u_int8_t    numChannels;
    /* Will contain array of u_int8_t elements */
    u_int8_t    channelList[0];
} __ATTRIB_PACK EXCLUDE_CHANNEL_REQ_T;

typedef struct exclude_channel_rsp {
    u_int8_t status;
} __ATTRIB_PACK EXCLUDE_CHANNEL_RSP_T;

typedef struct penalize_channels_by_ssid_req {
    u_int8_t    band;
    u_int8_t    channelConfig;
    u_int8_t    ssidLen;
    u_int8_t    ssid[MAX_SSID_LEN];
    u_int16_t   baselineUsability;
} __ATTRIB_PACK PENALIZE_CHANNELS_BY_SSID_REQ_T;

typedef struct penalize_channels_by_ssid_rsp {
    u_int8_t    status;
} __ATTRIB_PACK PENALIZE_CHANNELS_BY_SSID_RSP_T;

typedef struct get_state_req {
    u_int8_t band;
    u_int8_t channelConfig;
} __ATTRIB_PACK GET_STATE_REQ_T;

typedef struct get_state_rsp {
    u_int8_t status;
    u_int8_t state;
} __ATTRIB_PACK GET_STATE_RSP_T;

typedef struct cancel_scan_req {
    u_int8_t band;
    u_int8_t channelConfig;
} __ATTRIB_PACK CANCEL_SCAN_REQ_T;

typedef struct cancel_scan_rsp {
    u_int8_t status;
} __ATTRIB_PACK CANCEL_SCAN_RSP_T;

typedef struct channel_set_req {
    u_int8_t band;
    u_int8_t channelConfig;
    char     deviceName[IFNAMSIZ];
} __ATTRIB_PACK CHANNEL_SET_REQ_T;

typedef struct channel_set_rsp {
    u_int8_t status;
} __ATTRIB_PACK CHANNEL_SET_RSP_T;


/* Function declarations */
extern int icm_channel_selection_mode_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status);

extern int icm_get_ranked_channels_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status,
        u_int8_t numChannels,
        CHANNEL_INFO_T *channels);

extern int icm_exclude_channel_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status);

extern int icm_penalize_channels_by_ssid_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status);

extern int icm_get_state_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status,
        u_int8_t state);

extern int icm_cancel_scan_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status);


extern int icm_channel_set_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status);

#endif  /* _ICM_API_H_ */


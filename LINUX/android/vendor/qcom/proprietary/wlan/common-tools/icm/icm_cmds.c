/*
 * Copyright (c) 2012,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2012 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * =====================================================================================
 *
 *       Filename:  icm_cmds.c
 *
 *    Description:  ICM Command handler functions
 *
 *        Version:  1.0
 *        Created:  05/24/2012 04:30:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>

#include "icm.h"
#include "icm_api.h"


/*
 * Function     : icm_handle_channel_selection_mode_req
 * Description  : handles the SET_CHANNEL_SELECTION_MODE_REQ client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_channel_selection_mode_req(ICM_DEV_INFO_T* pdev,
        ICM_MSG_T* pmsg)
{
    SET_CHANNEL_SELECTION_MODE_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    int parse_status = FAILURE;
    int send_status = FAILURE;
    u_int16_t expected_len = sizeof(SET_CHANNEL_SELECTION_MODE_REQ_T) +
        sizeof(ICM_MSG_T);
    int i;
    int isHomeChannelValid = FALSE;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received SET_CHANNEL_SELECTION_MODE_REQ\n");

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    preq = (SET_CHANNEL_SELECTION_MODE_REQ_T*)pmsg->data;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"selectionMode = %u\n", preq->selectionMode);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"homeChannel = %u\n", preq->homeChannel);

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->selectionMode >= ICM_CH_SELECTION_MODE_INVALID) {
        err("Invalid selectionMode %u sent by peer.",
                preq->selectionMode);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* XXX - Handle case where ICM might not be initialized */

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if (picm == NULL || i == pdev->conf.num_radios) /* We didn't find a matching picm */
    {
        /* Shouldn't happen under the current assumptions of DBDC
           platforms being used. */
        err("Unsupported band used in request by peer");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->homeChannel == 0) {
        isHomeChannelValid = TRUE;
    } else {
        for (i = 0; i < picm->chlist.count; i++)
        {
            if (preq->homeChannel == picm->chlist.ch[i].channel) {
                isHomeChannelValid = TRUE;
                break;
            }
        }
    }

    /* XXX - Verify homeChannel and band combination */
    if (!isHomeChannelValid) {
        err("Invalid homeChannel %u sent by peer.",
                preq->homeChannel);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    picm->channel_width =  preq->channelConfig;
    picm->def_channel = preq->homeChannel;
    picm->ch_selection_mode = preq->selectionMode;
    rsp_status = ICM_RSP_STATUS_SUCCESS;
    parse_status = SUCCESS;

out:
    send_status = icm_channel_selection_mode_rsp(pdev,
            pmsg->transactionId,
            rsp_status);

    if (parse_status == SUCCESS && send_status == SUCCESS) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

/*
 * Function     : icm_handle_get_ranked_channels_req
 * Description  : handles the GET_RANKED_CHANNELS_REQ_T client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_get_ranked_channels_req(ICM_DEV_INFO_T* pdev,
        ICM_MSG_T* pmsg)
{
    GET_RANKED_CHANNELS_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    u_int16_t expected_len = sizeof(GET_RANKED_CHANNELS_REQ_T) +
        sizeof(ICM_MSG_T);
    int i;
    bool ispresent = FALSE;
    char temp_dev_ifname[IFNAMSIZ];

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received GET_RANKED_CHANNELS_REQ\n");

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u\n",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto bad;
    }

    preq = (GET_RANKED_CHANNELS_REQ_T*)pmsg->data;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);
    /* deviceName should be NULL-terminated. But you never know. So add
       a NULL at the end for safety. */
    preq->deviceName[sizeof(preq->deviceName) - 1] = '\0';
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"deviceName = %s\n", preq->deviceName);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"scanType = %u\n", preq->scanType);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"maxChannels = %u\n", preq->maxChannels);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"maxAPsPerChannel = %u\n", preq->maxAPsPerChannel);

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto bad;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto bad;
    }

    /* XXX - Handle case where ICM might not be initialized */

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if (picm == NULL || i == pdev->conf.num_radios) /* We didn't find a matching picm */
    {
        err("Invalid band %u sent by peer", preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto bad;
    }

    if (icm_is_dev_ifname_present(picm, preq->deviceName,
                &ispresent) != SUCCESS) {
        err("Error while trying to determine if deviceName %s sent by "
                " peer is valid for configured band %u",
                preq->deviceName,
                preq->band);
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto bad;
    }

    if (!ispresent) {
        err("deviceName %s sent by peer is not valid",
                preq->deviceName);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto bad;
    }

    /* If any bit other than ICM_SCAN_TYPE_CHANNEL_SCAN and
       ICM_SCAN_TYPE_SPECTRAL_SCAN is set, then the bitmask is invalid.
       A bitmask of 0 is accepted (and indicates that the most recent
       results are to be returned, without a fresh scan). */
    if (preq->scanType &
            (~(ICM_SCAN_TYPE_CHANNEL_SCAN | ICM_SCAN_TYPE_SPECTRAL_SCAN))) {
        err("Invalid scanType %u sent by peer.",
                preq->scanType);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto bad;
    }

    if (icm_is_scanner_thread_active(picm) == TRUE) {
        err("Scan for band %u sent by peer already in progress.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_SCAN_IN_PROGRESS;
        goto bad;
    }

    picm->channel_width =  preq->channelConfig;
    picm->scan_config.scan_type = preq->scanType;
    picm->scan_config.max_channels = preq->maxChannels;
    picm->scan_config.max_aps_per_channel = preq->maxAPsPerChannel;
    picm->reqmsg_params.transactionId = pmsg->transactionId;
    os_strlcpy(temp_dev_ifname, picm->dev_ifname, IFNAMSIZ);
    /* XXX - Can restore after scan. But
       for now, it won't be required */
    os_strlcpy(picm->dev_ifname, preq->deviceName, IFNAMSIZ);

    if (icm_activate_scanner_thread(picm) == FAILURE) {
        err("Could not start scanner thread");
        rsp_status = ICM_RSP_STATUS_FAILURE;
        os_strlcpy(picm->dev_ifname, temp_dev_ifname, IFNAMSIZ);
        goto bad;
    }

    return SUCCESS;

bad:
    icm_get_ranked_channels_rsp(pdev, pmsg->transactionId, rsp_status, 0, NULL);

    return FAILURE;
}

/*
 * Function     : icm_handle_exclude_channel_req
 * Description  : handles the EXCLUDE_CHANNEL_REQ_T client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_exclude_channel_req(ICM_DEV_INFO_T* pdev, ICM_MSG_T* pmsg)
{
    EXCLUDE_CHANNEL_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    ICM_CHANNEL_LIST_T *chlist = NULL;
    int parse_status = FAILURE;
    int send_status = FAILURE;
    /* Min base length. We will be factoring in numChannels as well */
    u_int16_t expected_len = sizeof(EXCLUDE_CHANNEL_REQ_T) + sizeof(ICM_MSG_T);
    int i = 0, j = 0;
    u_int8_t *channelList = NULL;
    char channelListStr[2048];
    char *strtempptr = NULL;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received EXCLUDE_CHANNEL_REQ\n");

    if (pmsg->len < expected_len ) {
        err("Invalid message length %u sent by peer. Expected at least %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    preq = (EXCLUDE_CHANNEL_REQ_T *)pmsg->data;

    expected_len += preq->numChannels;

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    channelList = (u_int8_t *)preq->channelList;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"numChannels = %u\n", preq->numChannels);
    strtempptr = channelListStr;

    for (i = 0; i < preq->numChannels; i++)
    {
        strtempptr += snprintf(strtempptr, 5, "%u,", channelList[i]);
    }

    if (preq->numChannels != 0) {
        strtempptr[-1] = '\0';
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelList: %s\n", channelListStr);
    }

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if (picm == NULL || i == pdev->conf.num_radios) {
        /* Shouldn't happen under the current assumptions of DBDC
           platforms being used. */
        err("Unsupported band sent in request by peer");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (icm_is_scanner_thread_active(picm) == TRUE) {
        err("Scan for band %u sent by peer is in progress",
                preq->band);
        rsp_status = ICM_RSP_STATUS_SCAN_IN_PROGRESS;
        goto out;
    }

    if(!picm->is_prev_scaninfo_available) {
        err("No previous scan results available for band %u sent by peer",
                preq->band);
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto out;
    }

    if (picm->band == ICM_BAND_2_4G) {
        chlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        chlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    if (preq->numChannels == 0) {
        err("Expecting non-zero value for numChannels");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* Since we have a small data set, we use simple
       logic to parse it. */
    for (i = 0; i < preq->numChannels; i++) {
        for (j = 0; j < chlist->count; j++) {
            if (chlist->ch[j].channel == channelList[i]) {
                break;
            }
        }

        if (j == chlist->count) {
            err("Invalid channel %u sent by peer",
                    channelList[i]);
            rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
            goto out;
        }
    }

    /* Now that we know *all* the channels in the request
       are valid, we parse again to actually set the exclusion
       flag. */
    for (i = 0; i < preq->numChannels; i++) {
        for (j = 0; j < chlist->count; j++) {
            if (chlist->ch[j].channel == channelList[i]) {
                ICM_SET_CHANNEL_EXCLUDE(picm, chlist->ch[j].channel);
                break;
            }
        }
    }

    picm->scan_config.scan_type = 0;

    /* Re-run channel selection so that the exclusions take effect */
    icm_scan_and_select_channel(picm, FALSE);

    rsp_status = ICM_RSP_STATUS_SUCCESS;
    parse_status = SUCCESS;

out:
    send_status = icm_exclude_channel_rsp(pdev,
            pmsg->transactionId,
            rsp_status);

    if (parse_status == SUCCESS && send_status == SUCCESS) {
        return SUCCESS;
    } else {
        return FAILURE;
    }


}

/*
 * Function     : icm_handle_penalize_channels_by_ssid_req
 * Description  : handles the PENALIZE_CHANNELS_BY_SSID_REQ_T client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_penalize_channels_by_ssid_req(ICM_DEV_INFO_T* pdev,
        ICM_MSG_T* pmsg)
{
    PENALIZE_CHANNELS_BY_SSID_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    ICM_CHANNEL_LIST_T *chlist = NULL;
    int penalizedchannel = 0;
    bool penalizationdone = FALSE;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    int parse_status = FAILURE;
    int send_status = FAILURE;
    u_int16_t expected_len = sizeof(PENALIZE_CHANNELS_BY_SSID_REQ_T) +
        sizeof(ICM_MSG_T);
    int i = 0, j = 0;
    char ssidStr[512];
    char *tempstrptr = NULL;
    char ssidHexStr[512];
    char *temphexstrptr = NULL;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received PENALIZE_CHANNELS_BY_SSID_REQ\n");

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    preq = (PENALIZE_CHANNELS_BY_SSID_REQ_T*)pmsg->data;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"ssidLen = %u\n", preq->ssidLen);

    tempstrptr = ssidStr;
    temphexstrptr = ssidHexStr;
    for (i = 0; i < MAX_SSID_LEN; i++)
    {
        if (preq->ssid[i]) {
            tempstrptr += snprintf(tempstrptr, 5, "%c", preq->ssid[i]);
        } else {
            /* What if the SSID contains '\0'? :) We don't bother about
               these nitty-gritties since this is a print for info
               purposes. We have a hex dump anyway. */
            tempstrptr += snprintf(tempstrptr, 5, "\\0");
        }

        temphexstrptr += snprintf(temphexstrptr, 5, "%02X ", preq->ssid[i]);
    }
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"ssid (\\0: NULL or Actual \\0. See hexdump for disambiguation): %s\n",ssidStr);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"ssid hexdump: %s\n",ssidHexStr);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"baselineUsability = %u\n",preq->baselineUsability);

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->ssidLen > MAX_SSID_LEN) {
        err("Invalid ssidLen %u sent by peer.",
                preq->ssidLen);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if (picm == NULL || i == pdev->conf.num_radios) {
        /* Shouldn't happen under the current assumptions of DBDC
           platforms being used. */
        err("Info for unsupported band requested by peer");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (icm_is_scanner_thread_active(picm) == TRUE) {
        err("Scan for band %u sent by peer is in progress.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_SCAN_IN_PROGRESS;
        goto out;
    }

    if(!picm->is_prev_scaninfo_available) {
        err("No previous scan results available for band %u sent by peer",
                preq->band);
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto out;
    }

    for (i = 0; i < MAX_SCAN_ENTRIES; i++)
    {
        if (IS_SCAN_ELEM_VALID(picm, i)) {
            if (ICM_GET_SCAN_ELEM_SSID_LEN(picm, i) == preq->ssidLen &&
                    !memcmp(preq->ssid,
                        ICM_GET_SCAN_ELEM_SSID(picm, i),
                        preq->ssidLen)) {
                break;
            }
        }
    }

    if (i == MAX_SCAN_ENTRIES) {
        err("SSID sent by peer not found in scan results");
        rsp_status = ICM_RSP_STATUS_NO_MATCHING_APS;
        goto out;
    }

    if (picm->band == ICM_BAND_2_4G) {
        chlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        chlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    for (i = 0; i < MAX_SCAN_ENTRIES; i++)
    {
        if (IS_SCAN_ELEM_VALID(picm, i)) {
            if(!memcmp(preq->ssid,
                        ICM_GET_SCAN_ELEM_SSID(picm, i),
                        MAX_SSID_LEN)) {
                penalizedchannel = ICM_GET_SCAN_ELEM_CHANNEL(picm, i);

                for (j = 0; j < chlist->count; j++) {
                    if (chlist->ch[j].channel == penalizedchannel) {
                        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Penalizing channel %d\n", chlist->ch[j].channel);
                        ICM_SET_CHANNEL_BLUSABILITY(picm,
                                chlist->ch[j].channel,
                                preq->baselineUsability);
                        penalizationdone = TRUE;
                    }
                }
            }
        }
    }

    if (!penalizationdone) {
        err("SSID sent by peer not found in scanned channels for band %u",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    picm->scan_config.scan_type = 0;

    /* Re-run channel selection so that the penalizations take effect */
    icm_scan_and_select_channel(picm, FALSE);

    rsp_status = ICM_RSP_STATUS_SUCCESS;
    parse_status = SUCCESS;

out:
    send_status = icm_penalize_channels_by_ssid_rsp(pdev,
            pmsg->transactionId,
            rsp_status);

    if (parse_status == SUCCESS && send_status == SUCCESS) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

/*
 * Function     : icm_handle_get_state_req
 * Description  : handles the GET_STATE_REQ client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_get_state_req(ICM_DEV_INFO_T* pdev, ICM_MSG_T* pmsg)
{
    GET_STATE_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    u_int8_t state = 0;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    int parse_status = FAILURE;
    int send_status = FAILURE;
    u_int16_t expected_len = sizeof(GET_STATE_REQ_T) + sizeof(ICM_MSG_T);
    int i;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received GET_STATE_REQ\n");

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    preq = (GET_STATE_REQ_T*)pmsg->data;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if ((picm == NULL) || ((i == pdev->conf.num_radios) /* We didn't find a matching picm */
            && (pdev->state != ICM_STATE_INITIALIZING))) {
        /* Shouldn't happen under the current assumptions of DBDC
           platforms being used. */
        err("Info for unsupported band requested by peer");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if ((i < pdev->conf.num_radios) &&
            (picm->substate == ICM_STATE_CHANNEL_SCAN ||
             picm->substate == ICM_STATE_SPECTRAL_SCAN)) {
        state = picm->substate;
    } else {
        state = pdev->state;
    }

    rsp_status = ICM_RSP_STATUS_SUCCESS;
    parse_status = SUCCESS;

out:
    send_status = icm_get_state_rsp(pdev,
            pmsg->transactionId,
            rsp_status,
            state);

    if (parse_status == SUCCESS && send_status == SUCCESS) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

/*
 * Function     : icm_handle_cancel_scan_req
 * Description  : handles CANCEL_SCAN_REQ client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_cancel_scan_req(ICM_DEV_INFO_T* pdev, ICM_MSG_T* pmsg)
{
    CANCEL_SCAN_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    int parse_status = FAILURE;
    int send_status = FAILURE;
    u_int16_t expected_len = sizeof(CANCEL_SCAN_REQ_T) + sizeof(ICM_MSG_T);
    int i;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received CANCEL_SCAN_REQ\n");

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    preq = (CANCEL_SCAN_REQ_T*)pmsg->data;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if (picm == NULL || i == pdev->conf.num_radios) {
        /* Shouldn't happen under the current assumptions of DBDC
           platforms being used. */
        err("Info for unsupported band requested by peer");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (icm_is_scanner_thread_active(picm) != TRUE) {
        err("No scan in progress for band %u sent by peer", preq->band);
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto out;
    }

    if (icm_is_80211scan_done(picm) == TRUE) {
        err("802.11 channel scan already completed for "
                "band %u sent by peer", preq->band);
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto out;
    }

    icm_set_80211scan_cancel_requested(picm, TRUE);

    rsp_status = ICM_RSP_STATUS_SUCCESS;
    parse_status = SUCCESS;

out:
    send_status = icm_cancel_scan_rsp(pdev,
            pmsg->transactionId,
            rsp_status);

    if (parse_status == SUCCESS && send_status == SUCCESS) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

/*
 * Function     : icm_handle_channel_set_req
 * Description  : handles CHANNEL_SET_REQ client command
 * Input params : pointer to ICM_DEV_INFO_T and pointer to msg structure
 * Return       : success/failure
 *
 */
static int icm_handle_channel_set_req(ICM_DEV_INFO_T* pdev, ICM_MSG_T* pmsg)
{
    CHANNEL_SET_REQ_T* preq = NULL;
    ICM_INFO_T* picm = NULL;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    int parse_status = FAILURE;
    int send_status = FAILURE;
    u_int16_t expected_len = sizeof(CHANNEL_SET_REQ_T) + sizeof(ICM_MSG_T);
    ICM_CHANSET_JOB_T *chanset = NULL;
    int i;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Received CHANNEL_SET_REQ\n");

    if (pmsg->len != expected_len ) {
        err("Invalid message length %u sent by peer. Expected %u",
                pmsg->len,
                expected_len);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    preq = (CHANNEL_SET_REQ_T*)pmsg->data;

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"band = %u\n", preq->band);
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"channelConfig = %u\n", preq->channelConfig);
    /* deviceName should be NULL-terminated. But you never know. So add
       a NULL at the end for safety. */
    preq->deviceName[sizeof(preq->deviceName) - 1] = '\0';
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"deviceName = %s\n", preq->deviceName);

    if (preq->band >= ICM_BAND_INVALID) {
        err("Invalid band %u sent by peer.",
                preq->band);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (preq->channelConfig >= ICM_CH_BW_INVALID) {
        err("Invalid channelConfig %u sent by peer.",
                preq->channelConfig);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    /* Find picm corresponding to band */
    for (i = 0; i < pdev->conf.num_radios; i++) {
        picm = get_picm(i);

        if (picm->band == preq->band) {
            break;
        }
    }

    if (picm == NULL || i == pdev->conf.num_radios) {
        /* Shouldn't happen under the current assumptions of DBDC
           platforms being used. */
        err("Info for unsupported band requested by peer");
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    if (!icm_is_dev_ifname_valid(preq->deviceName)) {
        err("deviceName %s sent by peer is not a valid 802.11 VAP name",
                preq->deviceName);
        rsp_status = ICM_RSP_STATUS_INVALID_PARAM;
        goto out;
    }

    chanset = (ICM_CHANSET_JOB_T *)malloc(sizeof(ICM_CHANSET_JOB_T));

    if (chanset == NULL) {
        err("Could not allocate memory for channel set job");
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto out;
    }

    /* TBD: Re-run best channel selection with latest channel config,
       exclusions etc */
    os_strlcpy(chanset->dev_ifname, preq->deviceName, IFNAMSIZ);
    os_strlcpy(picm->dev_ifname, preq->deviceName, IFNAMSIZ);
    chanset->transactionId = pmsg->transactionId;

    if (gettimeofday(&chanset->timeout, NULL) < 0) {
        err("Could not get initial queueing time for channel set job");
        rsp_status = ICM_RSP_STATUS_FAILURE;
        goto out;
    }

    /* Roll-over should not happen over lifetime of product on systems
       with 64 bit time_t.
       On systems with 32 bit time_t, roll-over is due in the year 2038.
       Such systems are supposed to be phased out before then. If our
       code is ported to a system still running 32 bit time_t in 2038, the
       entire system operation is jeopardized anyway, so we don't
       bother to handle roll-over.  */
    chanset->timeout.tv_sec += ICM_CHANSET_DEVUP_TIMEOUT_S;

    icm_chanset_ll_add(picm, &picm->main_list, chanset);

    if (!icm_is_chanset_thread_active(picm)) {
        if (icm_activate_chanset_thread(picm) != SUCCESS) {
            err("Could not activate channel set thread");
            rsp_status = ICM_RSP_STATUS_FAILURE;
            goto out;
        }
    }

    return SUCCESS;

out:
    send_status = icm_channel_set_rsp(pdev,
            pmsg->transactionId,
            rsp_status);

    if (parse_status == SUCCESS && send_status == SUCCESS) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

/*
 * Function     : icm_parse_client_cmd
 * Description  : handles client command
 * Input params : pointer to ICM_DEV_INFO_T,
 *                pointer to buffer containing msg structure,
 *                length of buffer
 * Return       : success/failure
 *
 */
int icm_parse_client_cmd(ICM_DEV_INFO_T* pdev, void *buf, int buflen)
{
    ICM_MSG_T* pmsg = (ICM_MSG_T*)buf;

    if (buflen < sizeof(ICM_MSG_T)) {
        err("Size of command (%u) lesser than minimum expected (%u)",
                buflen,
                sizeof(ICM_MSG_T));
        err("Cannot process command or send any reply");
        return FAILURE;
    }

    switch (pmsg->messageId) {
        case SET_CHANNEL_SELECTION_MODE_REQ:
            icm_handle_channel_selection_mode_req(pdev, pmsg);
            break;
        case GET_RANKED_CHANNELS_REQ:
            icm_handle_get_ranked_channels_req(pdev, pmsg);
            break;
        case EXCLUDE_CHANNEL_REQ:
            icm_handle_exclude_channel_req(pdev, pmsg);
            break;
        case PENALIZE_CHANNELS_BY_SSID_REQ:
            icm_handle_penalize_channels_by_ssid_req(pdev, pmsg);
            break;
        case GET_STATE_REQ:
            icm_handle_get_state_req(pdev, pmsg);
            break;
        case CANCEL_SCAN_REQ:
            icm_handle_cancel_scan_req(pdev, pmsg);
            break;
        case CHANNEL_SET_REQ:
            icm_handle_channel_set_req(pdev, pmsg);
            break;
        default:
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_CMDS,"Invalid Command\n");
            return FAILURE;
    }

    return SUCCESS;
}

int icm_send_msg_to_client(ICM_DEV_INFO_T* pdev, ICM_MSG_T* pmsg, int len)
{
    int err = -1;
    ICM_INET_T* pinet = NULL;

    pinet = ICM_GET_ADDR_OF_INETINFO(pdev);

    if (CONFIGURED_SOCK_TYPE(pdev) == SOCK_TYPE_UDP) {
        err = sendto(pinet->listener, pmsg, len, 0, (struct sockaddr*)&pinet->peer_addr, pinet->peer_addr_len);
    } else if (CONFIGURED_SOCK_TYPE(pdev) == SOCK_TYPE_TCP) {
        err = send(pinet->client_fd, pmsg, len, 0);
    }
    return err;
}

/*
 * Function     : icm_channel_selection_mode_rsp
 * Description  : prepares and sends SET_CHANNEL_SELECTION_MODE_RSP msg
 *                to client
 * Input params : pointer to ICM_DEV_INFO_T,
 *                transactionId and status
 * Return       : success/failure
 *
 */
int icm_channel_selection_mode_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status)
{
    u_int16_t len = sizeof(SET_CHANNEL_SELECTION_MODE_RSP_T) +
        sizeof(ICM_MSG_T);
    ICM_MSG_T* pmsg = (ICM_MSG_T*)malloc(len);
    int ret = 0;

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for "
                "SET_CHANNEL_SELECTION_MODE_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = SET_CHANNEL_SELECTION_MODE_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    SET_CHANNEL_SELECTION_MODE_RSP_T* prsp =
        (SET_CHANNEL_SELECTION_MODE_RSP_T*)pmsg->data;
    prsp->status = status;

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

/*
 * Function     : icm_get_ranked_channels_rsp
 * Description  : prepares and sends GET_RANKED_CHANNELS_RSP msg
 *                to client
 * Input params : pointer to ICM_DEV_INFO_T,
 *                transactionId, status, number of channels,
 *                array of CHANNEL_INFO_T elements.
 * Return       : success/failure
 *
 */
int icm_get_ranked_channels_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status,
        u_int8_t numChannels,
        CHANNEL_INFO_T *channels)
{
    u_int16_t len = 0;
    ICM_MSG_T* pmsg = NULL;
    int ret = 0;
    int i;

    len = (sizeof(CHANNEL_INFO_T) * numChannels) +
        sizeof(GET_RANKED_CHANNELS_RSP_T) +
        sizeof(ICM_MSG_T);

    if (channels != NULL) {
        CHANNEL_INFO_T *curChannel = channels;
        size_t apLen;
        for (i = 0; i < numChannels; i++) {
            apLen = (curChannel->numAPs * sizeof(AP_INFO_T));
            len += apLen;
            curChannel = (CHANNEL_INFO_T *) (((u_int8_t *) curChannel) +
                    sizeof(CHANNEL_INFO_T) + apLen);
        }
    }

    pmsg = (ICM_MSG_T*)malloc(len);

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for "
                "GET_RANKED_CHANNELS_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = GET_RANKED_CHANNELS_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    GET_RANKED_CHANNELS_RSP_T* prsp =
        (GET_RANKED_CHANNELS_RSP_T*)pmsg->data;
    prsp->status = status;
    prsp->numChannels = numChannels;
    if (channels != NULL) {
        memcpy(prsp->channels,
                channels,
                len - sizeof(ICM_MSG_T) - 2 * sizeof(u_int8_t));
    }

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

/*
 * Function     : icm_exclude_channel_rsp
 * Description  : prepares EXCLUDE_CHANNEL_RSP response msg for client
 * Input params : pointer to ICM_DEV_INFO_T, transaction ID, and status
 * Return       : success/failure
 *
 */
int icm_exclude_channel_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status)
{
    u_int16_t len = sizeof(EXCLUDE_CHANNEL_RSP_T) + sizeof(ICM_MSG_T);
    ICM_MSG_T* pmsg = (ICM_MSG_T*)malloc(len);
    int ret = 0;

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for EXCLUDE_CHANNEL_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = EXCLUDE_CHANNEL_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    EXCLUDE_CHANNEL_RSP_T* prsp = (EXCLUDE_CHANNEL_RSP_T*)pmsg->data;
    prsp->status = status;

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

/*
 * Function     : icm_penalize_channels_by_ssid_rsp
 * Description  : prepares PENALIZE_CHANNELS_BY_SSID_RSP response msg for client
 * Input params : pointer to ICM_DEV_INFO_T, transaction ID, and status
 * Return       : success/failure
 *
 */
int icm_penalize_channels_by_ssid_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status)
{
    u_int16_t len = sizeof(PENALIZE_CHANNELS_BY_SSID_RSP_T) + sizeof(ICM_MSG_T);
    ICM_MSG_T* pmsg = (ICM_MSG_T*)malloc(len);
    int ret = 0;

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for "
                "PENALIZE_CHANNELS_BY_SSID_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = PENALIZE_CHANNELS_BY_SSID_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    PENALIZE_CHANNELS_BY_SSID_RSP_T* prsp =
        (PENALIZE_CHANNELS_BY_SSID_RSP_T*)pmsg->data;
    prsp->status = status;

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

/*
 * Function     : icm_get_state_rsp
 * Description  : prepares and sends GET_STATE_RSP msg to client
 * Input params : pointer to ICM_DEV_INFO_T,
 *                transactionId, status and state
 * Return       : success/failure
 *
 */
int icm_get_state_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status,
        u_int8_t state)
{
    u_int16_t len = sizeof(GET_STATE_RSP_T) + sizeof(ICM_MSG_T);
    ICM_MSG_T* pmsg = (ICM_MSG_T*)malloc(len);
    int ret = 0;

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for GET_STATE_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = GET_STATE_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    GET_STATE_RSP_T* prsp = (GET_STATE_RSP_T*)pmsg->data;
    prsp->status = status;
    prsp->state = state;

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

/*
 * Function     : icm_cancel_scan_rsp
 * Description  : prepares CANCEL_SCAN_RSP response msg for client
 * Input params : pointer to ICM_DEV_INFO_T, transaction ID, and status
 * Return       : success/failure
 *
 */
int icm_cancel_scan_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status)
{
    u_int16_t len = sizeof(CANCEL_SCAN_RSP_T) + sizeof(ICM_MSG_T);
    ICM_MSG_T* pmsg = (ICM_MSG_T*)malloc(len);
    int ret = 0;

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for "
                "CANCEL_SCAN_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = CANCEL_SCAN_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    CANCEL_SCAN_RSP_T* prsp =
        (CANCEL_SCAN_RSP_T*)pmsg->data;
    prsp->status = status;

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

/*
 * Function     : icm_channel_set_rsp
 * Description  : prepares CHANNEL_SET_RSP response msg for client
 * Input params : pointer to ICM_DEV_INFO_T, transaction ID, and status
 * Return       : success/failure
 *
 */
int icm_channel_set_rsp(ICM_DEV_INFO_T* pdev,
        u_int8_t transactionId,
        u_int8_t status)
{
    u_int16_t len = sizeof(CHANNEL_SET_RSP_T) + sizeof(ICM_MSG_T);
    ICM_MSG_T* pmsg = (ICM_MSG_T*)malloc(len);
    int ret = 0;

    if (pmsg == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate buffer for "
                "CHANNEL_SET_RSP_T\n",
                __func__);
        return FAILURE;
    }

    pmsg->messageId = CHANNEL_SET_RSP;
    pmsg->transactionId = transactionId;
    pmsg->len = len;

    CHANNEL_SET_RSP_T* prsp =
        (CHANNEL_SET_RSP_T*)pmsg->data;
    prsp->status = status;

    ret = icm_send_msg_to_client(pdev, pmsg, len);

    free(pmsg);

    if (ret < 0) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

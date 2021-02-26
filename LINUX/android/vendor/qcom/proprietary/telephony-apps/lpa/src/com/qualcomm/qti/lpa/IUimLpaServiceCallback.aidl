/******************************************************************************
 * @file    IUimLpaServiceCallback.aidl
 * @brief   This interface describes the APIs for the callback that a client
 *          which uses IUimLpaService should implement in order to
 *          be notified of asynchronous indications.
 *
 * @version 00.00.01
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 ******************************************************************************/

package com.qualcomm.qti.lpa;

import com.qualcomm.qti.lpa.UimLpaDownloadProgress;
import com.qualcomm.qti.lpa.UimLpaProfile;

interface IUimLpaServiceCallback {
    /**
     * Add Profile response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *    more error codes to be added
     *
     * @return None
     */
    void uimLpaAddProfileResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Enable Profile Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimLpaEnableProfileResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Diable Profile Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimLpaDisableProfileResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Delete Profile Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimLpaDeleteProfileResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Update Nickname Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimLpaUpdateNicknameResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Reset eUicc Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimLpaeUICCMemoryResetResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Get Profile Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @param profiles
     *    List of profiles
     *
     * @return None
     */
    void uimLpaGetProfilesResponse(in int slot, in int token,
        in int responseCode, in UimLpaProfile[]  profiles);

    /**
     * Get EID Response.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @param eid
     *
     * @return None
     */
    void uimLpaGetEidResponse(in int slot, in int token,
        in int responseCode, in byte[] eid);

    /**
     * Get server addresses response
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @param smdp, smds
     *    SMDP, SMDS address
     *
     * @return None
     */
    void uimLpaGetSrvAddrResponse(in int slot, in int token,
        in int responseCode, in String smdp, in String smds);

    /**
     * Set server addresses response
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @param None
     *
     * @return None
     */
    void uimLpaSetSrvAddrResponse(in int slot, in int token,
        in int responseCode);

    /**
     * Notifies the Download Progress.
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param responseCode
     *    UIM_LPA_RESP_SUCCESS = 0;
     *    UIM_LPA_RESP_FAILURE = 1;
     *
     * @param progress
     *    consists information on Download progress.
     *
     * @return None
     */
    void uimLpaDownloadProgressIndication(in int slot,
        in int responseCode, in UimLpaDownloadProgress progress);

    /**
    * Send radio state to the client
    *
    * @param slot
    *    UIM_LPA_SLOT0  = 0;
    *    UIM_LPA_SLOT1  = 1;
    *    UIM_LPA_SLOT2  = 2;
    *
    * @param state
    *    UIM_LPA_RADIO_UNAVAILABLE = 0;
    *    UIM_LPA_RADIO_OFF = 1;
    *    UIM_LPA_RADIO_ON = 2;
    *
    * @return None
    */
    void uimLpaRadioStateIndication(in int slot, in int state);
}

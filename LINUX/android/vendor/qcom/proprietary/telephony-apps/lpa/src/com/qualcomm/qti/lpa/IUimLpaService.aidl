/******************************************************************************
 * @file    IUimLpaService.aidl
 * @brief   This interface provides the APIs to enable Local Profile Assistant
 *          for eUICC
 *
 * @version 00.00.01
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 ******************************************************************************/

package com.qualcomm.qti.lpa;

import com.qualcomm.qti.lpa.IUimLpaServiceCallback;

interface IUimLpaService {
    /**
     * registerCallback will be used by a client to register a callback to be
     * notified asynchronously
     *
     * @param cb
     *    Defines the callback interface
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int registerCallback(in IUimLpaServiceCallback cb);

    /**
     * deregisterCallback will be used by a client to deregister a callback to
     * be notified asynchronously
     *
     * @param cb
     *    Defines the callback interface
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int deregisterCallback(in IUimLpaServiceCallback cb);

    /**
     * uimLpaAddProfile request to add profile
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param activationCode
     *    The byte array representing activationCode.
     *
     * @param confirmationCode
     *    The byte array representing confirmationCode.
     *
     * @return immediate return error.
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaAddProfile(in int slot, in int token, in String activationCode,
        in String confirmationCode);

    /**
     * uimEnableProfile request to Enable profile
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param iccid
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaEnableProfile(in int slot, in int token, in byte[] iccid);

    /**
     * uimDisableProfile request to Disable profile
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param iccid
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaDisableProfile(in int slot, in int token, in byte[] iccid);

    /**
     * uimDeleteProfile request to Delete profile
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param iccid
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaDeleteProfile(in int slot, in int token, in byte[] iccid);

    /**
     * uimUpdateNickname request to Update Nickname
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param iccid
     * @param nickname
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaUpdateNickname(in int slot, in int token, in byte[] iccid,
        in String nickname);

    /**
     * uimeUICCMemoryReset request to reset eUicc memory
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @param options
     *
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaeUICCMemoryReset(in int slot, in int token, in int option);

    /**
     * uimGetProfiles request to Get List of Profiles
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaGetProfiles(in int slot, in int token);

    /**
     * uimGetEid request to Get EID
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @param token
     *
     * @return
     *    UIM_LPA_SUCCESS       = 0;
     *    UIM_LPA_ERROR         = 1;
     */
    int uimLpaGetEid(in int slot, in int token);

    /**
     * uimLpaUserConsent request to send user consent
     *
     * @param slot
     *    UIM_LPA_SLOT0  = 0;
     *    UIM_LPA_SLOT1  = 1;
     *    UIM_LPA_SLOT2  = 2;
     *
     * @return
     *    UIM_LPA_USER_NOT_OK  = 0;
     *    UIM_LPA_USER_OK      = 1;
     */
    int uimLpaUserConsent(in int slot, in int token, in boolean userOk);

/**
 * uimLpaGetSrvAddr request to query server addresses
 *
 * @param slot
 *    UIM_LPA_SLOT0  = 0;
 *    UIM_LPA_SLOT1  = 1;
 *    UIM_LPA_SLOT2  = 2;
 *
 * @return
 *    UIM_LPA_USER_NOT_OK  = 0;
 *    UIM_LPA_USER_OK      = 1;
 */
int uimLpaGetSrvAddr(in int slot, in int token);

/**
 * uimLpaSetSrvAddr request to change smdp server address
 *
 * @param slot
 *    UIM_LPA_SLOT0  = 0;
 *    UIM_LPA_SLOT1  = 1;
 *    UIM_LPA_SLOT2  = 2;
 *
 * @return
 *    UIM_LPA_USER_NOT_OK  = 0;
 *    UIM_LPA_USER_OK      = 1;
 *
 * @param smdp
 *    SMDP server address
 */
int uimLpaSetSrvAddr(in int slot, in int token, in String smdp);

}

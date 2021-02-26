/******************************************************************************
 * @file    IUimRemoteSimlockService.aidl
 * @brief   This interface provides the APIs for remote simlock
 *
 * @version 00.00.03
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 ******************************************************************************/

package com.qualcomm.qti.remoteSimlock;

import com.qualcomm.qti.remoteSimlock.IUimRemoteSimlockServiceCallback;

interface IUimRemoteSimlockService {
    /**
     * registerCallback will be used by a client to register a callback to be
     * notified asynchronously
     *
     * @param cb
     *    Defines the callback interface
     *
     * @return
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int registerCallback(in IUimRemoteSimlockServiceCallback cb);

    /**
     * deregisterCallback will be used by a client to deregister a callback to
     * be notified asynchronously
     *
     * @param cb
     *    Defines the callback interface
     *
     * @return
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int deregisterCallback(in IUimRemoteSimlockServiceCallback cb);

    /**
     * uimRemoteSimlockProcessSimlockData Send lock/unlock blob to SIMLock engine
     *
     * @param token
     *
     * @param simlockData
     *    The byte array representing simlock blob for lock and unlock.
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockProcessSimlockData(in int token, in byte[] simlockData);

    /**
     * uimRemoteSimlockGetSharedKey to get encrypted symmetric key from SIMLock engine
     *
     * @param token
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockGetSharedKey(in int token);

    /**
     * uimRemoteSimlockGenerateHMAC request to SIMLock engine to generate and return
     * HMAC on the input data
     *
     * @param token
     *
     * @parma data
     *    Input data for which HMAC needs to be generated
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockGenerateHMAC(in int token, in byte[] data);

    /**
     * uimRemoteSimlockGetVersion to get highest supported major/minor version of
     * incoming blob that is supported by SIMLock engine
     *
     * @param token
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockGetVersion(in int token);

    /**
     * uimRemoteSimlockGetSimlockStatus to get current SIMLock status
     *
     * @param token
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockGetSimlockStatus(in int token);

    /**
     * uimRemoteSimlockGenerateRequest request to Simlock engine to generate
     * a request. The parameter requestType indicates the type of
     * request to generate. Only blob request is supported at the
     * present time.
     *
     * @param token
     *
     * @param requestType
     *    UIM_REMOTE_GENERATE_BLOB_REQUEST_TYPE   = 1;
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockGenerateRequest(in int token, in int requestType);

    /**
     * uimRemoteSimlockUnlockTimer requests to Simlock engine to start and stop
     * unlock device timer to unlock the device for the specific time to
     * download the blob. The Simlock engine unlocks the device only once per
     * UICC power cycle.
     *
     * @param token
     *
     * @param requestType
     *    UIM_REMOTE_START_UNLOCK_TIMER    = 1;
     *    UIM_REMOTE_STOP_UNLOCK_TIMER     = 2;
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    int uimRemoteSimlockUnlockTimer(in int token, in int requestType);
}

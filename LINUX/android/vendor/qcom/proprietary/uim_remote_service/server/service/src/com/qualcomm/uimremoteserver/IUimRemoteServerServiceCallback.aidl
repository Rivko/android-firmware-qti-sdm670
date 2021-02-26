/******************************************************************************
 * @file    IUIMRemoteServerServiceCallback.aidl
 * @brief   This interface describes the APIs for the callback that a client
 *          which uses IUIMRemoteServerService should implement in order to be notified
 *          of asynchronous indications.
 *
 * @version 00.00.01
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All rights reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.uimremoteserver;

interface IUimRemoteServerServiceCallback {
    /**
     * Send Server connect response to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param connStatus
     *    UIM_REMOTE_SERVER_SUCCESS                 = 0;
     *    UIM_REMOTE_SERVER_CONNECT_FAILURE         = 1;
     *    UIM_REMOTE_SERVER_MSG_SIZE_TOO_LARGE      = 2;
     *    UIM_REMOTE_SERVER_MSG_SIZE_TOO_SMALL      = 3;
     *    UIM_REMOTE_SERVER_CONNECT_OK_CALL_ONGOING = 4;
     *
     * @param maxMessageSize
     *    must be present for UIM_REMOTE_SERVER_MSG_SIZE_TOO_LARGE and contain the
     *    the suitable message size
     *
     * @return None
     */
    void uimRemoteServerConnectResp(in int slot, in int connStatus, in int maxMessageSize);

    /**
     * Send Server disconnect response to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param disconnStatus
     *    UIM_REMOTE_SERVER_SUCCESS = 0;
     *    UIM_REMOTE_SERVER_FAILURE = 1;
     *
     * @return None
     */
    void uimRemoteServerDisconnectResp(in int slot, in int disconnStatus);

    /**
     * Send Server APDU response to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param respStatus
     *    UIM_REMOTE_SERVER_SUCCESS                 = 0;
     *    UIM_REMOTE_SERVER_INTERNAL_FAILURE        = 1;
     *    UIM_REMOTE_SERVER_SIM_NOT_READY           = 2;
     *    UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF = 3;
     *    UIM_REMOTE_SERVER_SIM_ABSENT              = 4;
     *
     * @param respData
     *    apdu response data
     *
     * @return None
     */
    void uimRemoteServerApduResp(in int slot, in int respStatus, in byte[] respData);

    /**
     * Send Server Transfer Atr response to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param transferStatus
     *    UIM_REMOTE_SERVER_SUCCESS                 = 0;
     *    UIM_REMOTE_SERVER_INTERNAL_FAILURE        = 1;
     *    UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF = 3;
     *    UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_ON  = 18;
     *    UIM_REMOTE_SERVER_SIM_ABSENT              = 4;
     *    UIM_REMOTE_SERVER_SIM_DATA_NOT_AVAILABLE  = 6;
     *
     * @param atr
     *    must be present on SUCCESS
     *
     * @return None
     */
    void uimRemoteServerTransferAtrResp(in int slot, in int transferStatus, in byte[] atr);

    /**
     * Send Server power response to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param powerStatus
     *    UIM_REMOTE_SERVER_SUCCESS                 = 0;
     *    UIM_REMOTE_SERVER_GENERIC_FAILURE         = 2;
     *    UIM_REMOTE_SERVER_SIM_ABSENT              = 11;
     *    UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF = 17;
     *    UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_ON  = 18;
     *
     * @return None
     */
    void uimRemoteServerPowerResp(in int slot, in int powerStatus);

    /**
     * Send Server reset SIM response to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param resetStatus
     *    UIM_REMOTE_SERVER_SUCCESS                 = 0;
     *    UIM_REMOTE_SERVER_GENERIC_FAILURE         = 2;
     *    UIM_REMOTE_SERVER_SIM_ABSENT              = 11;
     *    UIM_REMOTE_SERVER_SIM_NOT_READY           = 16;
     *    UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF = 17;
     *
     * @return None
     */
    void uimRemoteServerResetSimResp(in int slot, in int resetStatus);

    /**
     * Send Server disconnect indication to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param disconnectType
     *    UIM_REMOTE_SERVER_DISCONNECT_TYPE_GRACEFUL  = 0;
     *    UIM_REMOTE_SERVER_DISCONNECT_TYPE_IMMEDIATE = 1;
     *
     * @return None
     */
    void uimRemoteServerDisconnectInd(in int slot, in int disconnectType);

    /**
     * Send Server status indication to the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param status
     *    UIM_REMOTE_SERVER_STATUS_UNKNOWN_ERROR       = 0;
     *    UIM_REMOTE_SERVER_STATUS_CARD_RESET          = 1;
     *    UIM_REMOTE_SERVER_STATUS_CARD_NOT_ACCESSIBLE = 2;
     *    UIM_REMOTE_SERVER_STATUS_CARD_REMOVED        = 3;
     *    UIM_REMOTE_SERVER_STATUS_CARD_INSERTED       = 4;
     *    UIM_REMOTE_SERVER_STATUS_RECOVERED           = 5;
     *
     * @return None
     */
    void uimRemoteServerStatusInd(in int slot, in int status);
}

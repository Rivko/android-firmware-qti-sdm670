/******************************************************************************
 * @file    IUIMRemoteServerService.aidl
 * @brief   This interface describes the APIs for the service which allows
 *          a client to make a remote UIM Server available to the service
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

import com.qualcomm.uimremoteserver.IUimRemoteServerServiceCallback;

interface IUimRemoteServerService {
    /**
     * registerCallback will be used by a client to register a callback to be
     * notified asynchronously
     *
     * @param cb
     *    Defines the callback interface
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int registerCallback(in IUimRemoteServerServiceCallback cb);

    /**
     * deregisterCallback will be used by a client to deregister a callback to be
     * notified asynchronously
     *
     * @param cb
     *    Defines the callback interface
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int deregisterCallback(in IUimRemoteServerServiceCallback cb);

    /**
     * Connect to Server and set the maximum message size
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param maxMessageSize
     *    proposed maximum size of the message
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int uimRemoteServerConnectReq(in int slot, in int maxMessageSize);

    /**
     * Disconnect to Server
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int uimRemoteServerDisconnectReq(in int slot);

    /**
     * Send Server ISO 7816 streaming APDU request
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param cmd
     *    APDU command data to card
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int uimRemoteServerApduReq(in int slot, in byte[] cmd);

    /**
     * Send Server transfer Atr request
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int uimRemoteServerTransferAtrReq(in int slot);

    /**
     * Send Server power request
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param state
     *    UIM_REMOTE_SERVER_POWER_OFF     = 0;
     *    UIM_REMOTE_SERVER_POWER_ON      = 1;
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int uimRemoteServerPowerReq(in int slot, in boolean state);

    /**
     * Send Server reset SIM request
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return
     *    UIM_REMOTE_SERVER_SUCCESS       = 0;
     *    UIM_REMOTE_SERVER_ERROR         = 1;
     */
    int uimRemoteServerResetSimReq(in int slot);
}

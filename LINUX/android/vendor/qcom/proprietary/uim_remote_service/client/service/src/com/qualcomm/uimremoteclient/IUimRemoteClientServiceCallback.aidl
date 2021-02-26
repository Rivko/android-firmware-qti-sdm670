/******************************************************************************
 * @file    IUIMRemoteClientServiceCallback.aidl
 * @brief   This interface describes the APIs for the callback that a client
 *          which uses IUIMRemoteClientService should implement in order to
 *          be notified of asynchronous indications.
 *
 * @version 00.00.01
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All rights reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *******************************************************************************/

package com.qualcomm.uimremoteclient;

interface IUimRemoteClientServiceCallback {
    /**
     * Send Event response to the client
     *
     * @param responseCode
     *    UIM_REMOTE_RESP_SUCCESS = 0;
     *    UIM_REMOTE_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimRemoteEventResponse(in int slot, in int responseCode);

    /**
     * Send APDU response to the client
     *
     * @param responseCode
     *    UIM_REMOTE_RESP_SUCCESS = 0;
     *    UIM_REMOTE_RESP_FAILURE = 1;
     *
     * @return None
     */
    void uimRemoteApduResponse(in int slot, in int responseCode);

    /**
     * Send an APDU indication requesting client to send an APDU to
     * the remote UIM card
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @param apduCmd
     *    APDU command data sent to the remote UIM
     *
     * @return None
     */
    void uimRemoteApduIndication(in int slot, in byte[] apduCmd);

    /**
     * Send a connect indication requesting a connection to client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return None
     */
    void uimRemoteConnectIndication(in int slot);

    /**
     * Send a diconnect indication requesting a disconnection from the client
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return None
     */
    void uimRemoteDisconnectIndication(in int slot);

    /**
     * Send a power up indication requesting the remote UIM card to be powered up
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return None
     */
    void uimRemotePowerUpIndication(in int slot);

    /**
     * Send a power down indication requesting the remote UIM card to be powered down
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return None
     */
    void uimRemotePowerDownIndication(in int slot);

    /**
     * Send a reset indication requesting the remote UIM card to be reset
     *
     * @param slot
     *    UIM_REMOTE_SLOT0  = 0;
     *    UIM_REMOTE_SLOT1  = 1;
     *    UIM_REMOTE_SLOT2  = 2;
     *
     * @return None
     */
    void uimRemoteResetIndication(in int slot);

    /**
     * Send radio state to the client
     *
     * @param state
     *    RADIO_UNAVAILABLE = 0;
     *    RADIO_OFF = 1;
     *    RADIO_ON = 2;
     *
     * @return None
     */
    void uimRemoteRadioStateIndication(in int slot, in int state);
}

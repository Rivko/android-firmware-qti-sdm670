/******************************************************************************
 * @file    ILteDirectDiscoveryStatusListener.aidl
 * @brief   This interface describes the callbacks and unsol indications
 *          for LTE Direct Discovery service.
 * @version 00.00.01
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.ltedirect.internal;

import com.qualcomm.qti.ltedirect.internal.LteDirectDiscoveryCapability;

/**
 * Listener type for receiving the asynchronous responses and unsolicited indications.
 */
oneway interface ILteDirectDiscoveryStatusListener {
    /**
     * Notifies the device LTE-D capability of all SUBs.
     *
     * @param capability LTE-D capability of SUB1 & SUB2
     * @return           none
     *
     * @see              ILteDirectDiscovery#getDeviceCapability
     */
    void onDeviceCapabilityChanged (in LteDirectDiscoveryCapability capability);

    /**
     * Notifies the LTE-D service status
     *
     * @param publishAllowed    Publish status
     *                          FALSE(0) = Currently UE is out of announcing coverage
     *                          TRUE(1)  = Currently UE is in announcing coverage
     * @param subscribeAllowed  Subscribe status
     *                          FALSE(0) = Currently UE is out of monitoring coverage
     *                          TRUE(1)  = Currently UE is in monitoring coverage
     * @return                  none
     *
     * @see                     ILteDirectDiscovery#initializeClient
     */
    void onServiceStatus (in int publishAllowed, in int subscribeAllowed);

    /**
     * Notifies the response status of the
     * {@link ILteDirectDiscovery#terminate}
     *
     * @param txnId      Transaction id
     * @param result     Result status as defined in LteDirectDiscovery#OperationResult
     * @return           none
     *
     * @see              ILteDirectDiscovery#terminate
     */
    void onTerminateResponse (in int txnId, in int result);

    /**
     * Notifies the response status of
     * {@link ILteDirectDiscovery#getServiceStatus}
     *
     * @param txnId      Transaction id
     * @param result     Result status as defined in LteDirectDiscovery#OperationResult
     * @param publishAllowed    Publish status
     *                          FALSE(0) = Currently UE is out of announcing coverage
     *                          TRUE(1)  = Currently UE is in announcing coverage
     * @param subscribeAllowed  Subscribe status
     *                          FALSE(0) = Currently UE is out of monitoring coverage
     *                          TRUE(1)  = Currently UE is in monitoring coverage
     * @return           none
     *
     * @see              ILteDirectDiscovery#getServiceStatus
     */
    void onGetServiceStatusResponse (in int txnId, in int result,
            in int publishAllowed, in int subscribeAllowed);

    /**
     * Notifies the response status of the
     * {@link ILteDirectDiscovery#publish}
     * The result status of each expression is notified
     * through the
     * {@link ILteDirectDiscovery#onExpressionStatus}
     *
     * @param txnId      Transaction id
     * @param result     Result status as defined in LteDirectDiscovery#OperationResult
     * @return           none
     *
     * @see              ILteDirectDiscovery#publish
     */
    void onPublishResponse (in int txnId, in int result);

    /**
     * Notifies the response status of the
     * {@link ILteDirectDiscovery#cancelPublish}
     *
     * @param txnId      Transaction id
     * @param result     Result status as defined in LteDirectDiscovery#OperationResult
     * @return           none
     *
     * @see              ILteDirectDiscovery#cancelPublish
     */
    void onCancelPublishResponse (in int txnId, in int result);

    /**
     * Notifies the response status of the
     * {@link ILteDirectDiscovery#subscribe}
     * The result status of each expression is notified
     * through the
     * {@link ILteDirectDiscovery#onExpressionStatus}
     *
     * @param txnId      Transaction id
     * @param result     Result status as defined in LteDirectDiscovery#OperationResult
     * @return           none
     *
     * @see              ILteDirectDiscovery#subscribe
     */
    void onSubscribeResponse (in int txnId, in int result);

    /**
     * Notifies the response status of the
     * {@link ILteDirectDiscovery#cancelSubscribe}
     *
     * @param txnId      Transaction id
     * @param result     Result status as defined in LteDirectDiscovery#OperationResult
     * @return           none
     *
     * @see              ILteDirectDiscovery#cancelSubscribe
     */
    void onCancelSubscribeResponse (in int txnId, in int result);

    /**
     * Notifies App authorization failure from LTE-D server.
     *
     * @param result     Authorization error code as defined in
     *                   LteDirectDiscovery#ResultCode
     * @return           none
     */
    void onAuthorizationResult (in int result);

    /**
     * Notifies the status of the Expressions
     *
     * @param expression Expression (PA ID)
     * @param result     Expression Result status as defined in
     *                   LteDirectDiscovery#ResultCode
     * @return           none
     *
     * @see              ILteDirectDiscovery#publish
     * @see              ILteDirectDiscovery#subscribe
     */
    void onExpressionStatus (in String expression, in int result);

    /**
     * Transmission Status
     *
     * @param expression Expression (PA-ID)
     * @param status     Transmission status
     *                    0 = completed
     *                    1 = pendingTransmission
     * @return           none
     *
     * @see              ILteDirectDiscovery#publish
     */
    void onTransmissionStatus (in String expression, in int status);

    /**
     * Reception Status
     *
     * @param expression Expression (PA-ID)
     * @param status     Reception status
     *                    0 = completed
     *                    1 = pendingSubscription
     * @return           none
     *
     * @see              ILteDirectDiscovery#subscribe
     */
    void onReceptionStatus (in String expression, in int status);

    /**
     * Notifies the Match Event
     *
     * @param expression Subscribed Expression (PA-ID)
     * @param matchedExpression Matched Expression (PA-ID)
     * @param state      Match start/end
     *                    0 = Match Start
     *                    1 = Match End
     * @param metaDataIndex   Index value to look for changes in MetaData field
     * @param metaData   Meta Data associated with the Expression
     *                   (e.g. postal address, phone number, URL etc)
     * @return           none
     *
     * @see              ILteDirectDiscovery#subscribe
     */
    void onMatchEvent (in String expression, in String matchedExpression,
            in int state, in int metaDataIndex, in String metaData);
}

/******************************************************************************
 * @file    ILteDirectDiscovery.aidl
 * @brief   This interface describes the APIs for LTE Direct Discovery service.
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

import com.qualcomm.qti.ltedirect.internal.ILteDirectDiscoveryStatusListener;
import com.qualcomm.qti.ltedirect.internal.LteDirectDiscoveryCapability;

/**
 *  Provides APIs for LTE Direct Discovery
 */
interface ILteDirectDiscovery {
    /**
     * Get device LTE-D capability of each subscriptions
     *
     * @param        None
     * @return       LTE-D capability of SUB1 & SUB2
     *
     * @see          ILteDirectDiscoveryStatusListener#onDeviceCapabilityChanged
     */
    LteDirectDiscoveryCapability getDeviceCapability();

    /**
     * Set Phone Subscription ID
     *
     * @param subId    Subscription ID
     * @return         Success/Failure.
     *
     * @see            ILteDirectDiscoveryStatusListener
     */
    boolean setPhoneSubscriptionId(in int subId);

    /**
     * Stop all ongoing operations
     *
     * @param txnId       Transaction id
     * @return            None. Response will be notified through the listener callback
     *                    {@link ILteDirectDiscoveryStatusListener#onTerminateResponse}
     *                    registered in initializeClient.
     *
     * @see               ILteDirectDiscoveryStatusListener#onTerminateResponse
     */
    oneway void terminate(in int txnId);

    /**
     * Get publish and subscribe service status
     *
     * @param txnId       Transaction id
     * @return            None. Response will be notified through the listener callback
     *                    {@link ILteDirectDiscoveryStatusListener#onGetServiceStatusResponse}
     *                    registered in initializeClient.
     *
     * @see               ILteDirectDiscoveryStatusListener#onGetServiceStatusResponse
     */
    oneway void getServiceStatus(in int txnId);

    /**
     * Publish an expression.
     *
     * @param txnId       Transaction id
     * @param expression  Expression or ProSe Application ID (PA ID) to be published
     *                    ProSe Application ID is composed of the following parts:
     *                    i. The ProSe Application ID Name is described in its
     *                       entirety by a data structure characterized by different
     *                       levels e.g., broad-level business category (Level 0) /
     *                       business sub-category (Level 1) / business name (Level 2) /
     *                       shop ID (Level 3). For the purpose of presentation, a
     *                       ProSe Application ID Name is usually displayed as a string
     *                       of labels in which the labels represent hierarchical levels.
     *                   ii. The PLMN ID that corresponds to the PLMN that assigned the
     *                       ProSe Application ID Name.
     *
     *                   Example: "mcc345.mnc012.ProSeApp.Food.Restaurants.Italian"
     *
     * @param expressionValidityTime Validity time of the expression
     *                    Units: minutes
     *                    Minimum value: 1
     *                    Maximum value: 525600 (1 year)
     * @param metaData    Meta data to be transmitted along with the expression
     * @param discoveryType Restricted/Open (Currently supports only Open)
     * @param duration    Duration for which to announce the code
     *                    Units: minutes
     *                    Minimum value: 1 min
     *                    Maximum value: 30 min
     * @return            None. Response will be notified through the listener callback
     *                    {@link ILteDirectDiscoveryStatusListener#onPublishResponse}
     *                    registered in initializeClient.
     *
     * @see               ILteDirectDiscoveryStatusListener#onPublishResponse
     * @see               ILteDirectDiscoveryStatusListener#onExpressionStatus
     * @see               ILteDirectDiscoveryStatusListener#onTransmissionStatus
     */
    oneway void publish(in int txnId, in String expression, in int expressionValidityTime,
            in String metaData, in int discoveryType, in int duration);

    /**
     * Cancel the publication of an expression
     *
     * @param txnId       Transaction id
     * @param expression  Expression (PA-ID)
     * @return            None. Response will be notified through the listener callback
     *                    {@link ILteDirectDiscoveryStatusListener#onCancelPublishResponse}
     *                    registered in initializeClient.
     *
     * @see               ILteDirectDiscoveryStatusListener#onCancelPublishResponse
     */
    oneway void cancelPublish(in int txnId, in String expression);

    /**
     * Subscribe an expression.
     *
     * @param txnId       Transaction id
     * @param expression  Expression or ProSe Application ID (PA ID) to be subscribed
     *                    ProSe Application ID is composed of the following parts:
     *                    i. The ProSe Application ID Name is described in its
     *                       entirety by a data structure characterized by different
     *                       levels e.g., broad-level business category (Level 0) /
     *                       business sub-category (Level 1) / business name (Level 2) /
     *                       shop ID (Level 3). For the purpose of presentation, a
     *                       ProSe Application ID Name is usually displayed as a string
     *                       of labels in which the labels represent hierarchical levels.
     *                   ii. The PLMN ID that corresponds to the PLMN that assigned the
     *                       ProSe Application ID Name.
     *
     *                   Example: "mcc345.mnc012.ProSeApp.Food.Restaurants.Italian"
     *
     * @param expressionValidityTime Validity time of the expression
     *                    Units: minutes
     *                    Minimum value: 1
     *                    Maximum value: 525600 (1 year)
     * @param discoveryType Restricted/Open (Currently supports only Open)
     * @param duration    Duration for which to subscribe the code
     *                    Units: minutes
     *                    Minimum value: 1 min
     *                    Maximum value: 30 min
     * @return            None. Response will be notified through the listener callback
     *                    {@link ILteDirectDiscoveryStatusListener#onSubscribeResponse}
     *                    registered in initializeClient.
     *
     * @see               ILteDirectDiscoveryStatusListener#onSubscribeResponse
     * @see               ILteDirectDiscoveryStatusListener#onExpressionStatus
     * @see               ILteDirectDiscoveryStatusListener#onReceptionStatus
     * @see               ILteDirectDiscoveryStatusListener#onMatchEvent
     */
    oneway void subscribe(in int txnId, in String expression, in int expressionValidityTime,
            in int discoveryType, in int duration);

    /**
     * Cancel the subscription.
     *
     * @param txnId       Transaction id
     * @param expression  Expression (PA-ID)
     * @return            None. Response will be notified through the listener callback
     *                    {@link ILteDirectDiscoveryStatusListener#onCancelSubscribeResponse}
     *                    registered in initializeClient.
     *
     * @see               ILteDirectDiscoveryStatusListener#onCancelSubscribeResponse
     */
    oneway void cancelSubscribe(in int txnId, in String expression);
}

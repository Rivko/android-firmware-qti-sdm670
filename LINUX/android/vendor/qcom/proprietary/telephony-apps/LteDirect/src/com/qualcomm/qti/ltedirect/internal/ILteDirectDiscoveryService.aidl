/******************************************************************************
 * @file    ILteDirectDiscoveryService.aidl
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
import com.qualcomm.qti.ltedirect.internal.ILteDirectDiscovery;

/**
 *  Provides APIs for LTE Direct Discovery Service
 */
interface ILteDirectDiscoveryService {
    /**
     * Initialze the client for service
     *
     * @param osAppId  Os App Id
     * @param listener The asynchronous responses and unsolicited indications will be notified
     *                 through the callback functions in the listener.
     * @return         Client handle in case of success
     *                 NULL in case of failure
     *
     * @see            ILteDirectDiscoveryStatusListener
     */
    ILteDirectDiscovery initializeClient(in String osAppId, in ILteDirectDiscoveryStatusListener listener);

    /**
     * Release client.
     *
     * @param osAppId  Os App Id
     * @return         Success/Failure.
     *
     * @see            ILteDirectDiscoveryStatusListener
     */
    boolean releaseClient(in String osAppId);
}

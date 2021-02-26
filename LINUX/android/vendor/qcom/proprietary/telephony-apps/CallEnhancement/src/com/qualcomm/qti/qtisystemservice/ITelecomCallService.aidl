/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import android.telecom.ParcelableCall;
import com.qualcomm.qti.qtisystemservice.ICallStateListener;

interface ITelecomCallService {
    void registerCallStateChanged(ICallStateListener listener);
    void unregisterCallStateChanged(ICallStateListener listener);
    boolean hasAnyActiveCall();
    boolean hasAnyHeldCall();
    ParcelableCall getForegroundCall();
}

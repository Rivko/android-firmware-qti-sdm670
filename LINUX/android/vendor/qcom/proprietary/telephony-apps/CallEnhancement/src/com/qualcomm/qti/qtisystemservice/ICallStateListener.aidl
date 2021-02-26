/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import android.telecom.ParcelableCall;

oneway interface ICallStateListener {
     void onCallStateChanged(in ParcelableCall call, int newState, int oldState);
}

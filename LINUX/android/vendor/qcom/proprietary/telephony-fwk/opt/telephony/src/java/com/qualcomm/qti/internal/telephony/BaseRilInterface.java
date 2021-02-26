/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.os.Handler;
import android.os.Message;

public interface BaseRilInterface{

    void registerForServiceReadyEvent(Handler h, int what, Object obj);
    void unRegisterForServiceReadyEvent(Handler h);

    boolean isServiceReady();
    boolean setLocalCallHold(int phoneId, boolean enable);

    void sendPhoneStatus(int isReady, int phoneId);
    void getOmhCallProfile(int modemApnType, Message callbackMsg, int phoneId);
}

/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.launcherunreadservice;

import com.qti.launcherunreadservice.IUnreadNumberCallback;
import android.os.IBinder;
interface IGetUnreadNumber {
    Map GetUnreadNumber();
    void registerUnreadNumberCallback(IBinder token, IUnreadNumberCallback callback);
    void unRegisterUnreadNumberCallback(IBinder token);
}

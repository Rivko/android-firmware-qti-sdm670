/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.launcherunreadservice;

import android.content.ComponentName;

interface IUnreadNumberCallback {
    void onUnreadNumberChanged(in ComponentName componentName, int num);
}

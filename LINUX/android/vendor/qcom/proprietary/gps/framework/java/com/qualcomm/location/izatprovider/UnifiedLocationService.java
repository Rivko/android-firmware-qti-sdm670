/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyrightf (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Not a Contribution, Apache license notifications and
  license are retained for attribution purposes only.
=============================================================================*/

/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.location.izatprovider;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class UnifiedLocationService extends Service {
    private static final String TAG = "UnifiedLocationService";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);

    private IzatProvider mProvider;

    @Override
    public void onCreate() {
        super.onCreate();
        mProvider = IzatProvider.getFusedProvider(getApplicationContext());
    }

    @Override
    public IBinder onBind(Intent intent) {
        if (VERBOSE_DBG) {
            Log.v(TAG, "onBind()");
        }

        // We expect LMS to call onEnable() after binding to it
        IBinder res = null;
        if (mProvider != null) {
            res = mProvider.getBinder();
        }

        return res;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        if (VERBOSE_DBG) {
            Log.v(TAG, "onUnbind()");
        }

        if (mProvider != null) {
            mProvider.onDisable();
        }

        return false;
    }
}


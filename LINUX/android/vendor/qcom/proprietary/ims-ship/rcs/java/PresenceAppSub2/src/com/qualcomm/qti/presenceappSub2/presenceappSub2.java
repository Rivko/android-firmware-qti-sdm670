/*************************************************************************
 Copyright (c)2012-2013,2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*************************************************************************/

package com.qualcomm.qti.presenceappSub2;

import android.app.Application;
import android.util.Log;

public class presenceappSub2 extends Application {
    final String TAG = "Presence_UI2: presenceappSub2";
    @Override
        public void onCreate() {
            Log.d(TAG, "onCreate()");
            super.onCreate();
        }

    @Override
        public void onTerminate() {
            Log.d(TAG, "onTerminate()");
            super.onTerminate();
        }


}

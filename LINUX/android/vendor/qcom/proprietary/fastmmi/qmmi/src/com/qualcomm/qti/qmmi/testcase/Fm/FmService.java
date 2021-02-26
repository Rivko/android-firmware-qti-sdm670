/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Fm;

import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class FmService extends BaseService {


    private final IBinder binder = new MyBinder();

    public class MyBinder extends Binder{
        FmService getService() {
            return FmService.this;
        }
    };

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");
        LogUtils.logi("Loading FM-JNI Library");
        try {
            System.loadLibrary("qcomfm_jni");
        } catch (Exception e) {
            LogUtils.logi("Loading FM-JNI Library fail:" + e);
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.loge("NoClassDefFoundError occur!");
            t.printStackTrace();
        }

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return binder;
    }

    @Override
    public void register() { }

    public int stop(TestCase testCase) {
        return 0;
    }

    @Override
    public int run(TestCase testCase) {
        LogUtils.logi("Fm service run");
        return 0;
    }
}

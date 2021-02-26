/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Flashlight;

import android.content.Context;
import android.content.Intent;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class FlashlightService extends BaseService {
    private CameraManager mCameraManager;
    private String mCameraId;
    private boolean mDoInit = true;
    Thread mThread;
    Runnable mRunnable;

    @Override
    public void register() {
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi( "onStartCommand");
        try {
            mCameraManager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
            tryInitCamera();
        } catch (Exception e) {
            LogUtils.loge(e.toString());
        }
        return super.onStartCommand(intent, flags, startId);
    }

    private void tryInitCamera() {
        try {
            String[] ids = mCameraManager.getCameraIdList();
            for (String id : ids) {
                CameraCharacteristics c = mCameraManager.getCameraCharacteristics(id);
                Boolean flashAvailable = c.get(CameraCharacteristics.FLASH_INFO_AVAILABLE);
                Integer lensFacing = c.get(CameraCharacteristics.LENS_FACING);
                if (flashAvailable != null && flashAvailable
                        && lensFacing != null && lensFacing == CameraCharacteristics.LENS_FACING_BACK) {
                    mCameraId = id;
                    return;
                }
            }
        } catch (Throwable e) {
            LogUtils.loge("Couldn't initialize." + e);
            return;
        }
    }

    public void setFlashlight(boolean enabled) {
        try {
            LogUtils.logi("cameraId:" + mCameraId + ",mCameraManager:" + mCameraManager);

            if (mCameraId == null) {
                while (mDoInit) {
                    Thread.sleep(1000);
                    tryInitCamera();
                    if (mCameraId != null) {
                        break;
                    }
                }
            }
            if (mCameraId != null)
                mCameraManager.setTorchMode(mCameraId, enabled);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public int run(TestCase testCase) {
        mRunnable = new Runnable() {
            @Override
            public void run() {
                setFlashlight(true);
            }
        };
        mThread = new Thread(mRunnable);
        mThread.start();
        updateView(testCase.getName(), getString(R.string.flashlisht_onoff));
        return 0;
    }

    @Override
    public int stop(TestCase testCase) {
        mDoInit = false;
        if (mRunnable != null && mThread != null) {
            try {
                mThread.join();
                mThread = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        setFlashlight(false);
        return 0;
    }
}

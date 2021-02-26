/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.seccamsample;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import android.app.Activity;

import android.content.res.Configuration;
import android.os.Bundle;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import android.graphics.ImageFormat;

import com.qualcomm.qti.seccamsample.R;
import com.qualcomm.qti.seccamsample.SecureCamera2API;
import com.qualcomm.qti.seccamapi.SecureSurface;
import com.qualcomm.qti.seccamapi.SecureCamera2Surface;

public class Camera2APIActivity extends CameraActivity {

    private SecureCamera2API previewCamera_ = null;
    private static final Integer previewCameraID_ = 1;
    private static final Integer previewWidth_ = 1280;
    private static final Integer previewHeight_ = 720;
    private static final Integer previewFormat_ = ImageFormat.YUV_420_888;
    private static final Integer previewNumOfBuffers_ = 2;
    private static final Integer previewSurfaceNumOfBuffers_ = 3;

    private SecureCamera2API captureCamera_ = null;
    private static final Integer captureCameraID_ = 2; //2 is the index for the front IR camera on MTP
    private static Integer captureWidth_ = 1920;
    private static Integer captureHeight_ = 1920;
    private static final Integer captureFormat_ = ImageFormat.RAW_PRIVATE;
    private static final Integer captureNumOfBuffers_ = 2;

    //=========================================================================
    private class StartTask extends AsyncTask<String, Void, Boolean> {

        protected void onPreExecute() {
            Log.d(getLogTag(), "Camera2APIActivity::StartTask::onPreExecute");
        }

        protected Boolean doInBackground(String... tmpStr) {
            Log.d(getLogTag(), "Camera2APIActivity::StartTask::doInBackground");
            try {
                Boolean result = Boolean.FALSE;

                if (captureCameraEnabled_) {
                    if (captureCamera_ != null) {
                        Log.e(LOG_TAG, "Secure camera already running.. ignoring StartTask");
                        return Boolean.TRUE;
                    }
                    captureCamera_ = new SecureCamera2API(Camera2APIActivity.this);
                    if (captureCamera_.start(captureCameraID_,
                            captureWidth_, captureHeight_, captureFormat_, captureNumOfBuffers_)) {
                        result = Boolean.TRUE;
                    }
                }
                if (previewCameraEnabled_) {
                    if (previewCamera_ != null) {
                        Log.e(LOG_TAG, "Secure camera already running.. ignoring StartTask");
                        return Boolean.TRUE;
                    }
                    previewCamera_ = new SecureCamera2API(Camera2APIActivity.this);
                    if(previewCamera_.start(previewCameraID_, getPreviewView(),
                            previewWidth_, previewHeight_, previewFormat_, previewNumOfBuffers_,
                            previewSurfaceNumOfBuffers_)) {
                        result = Boolean.TRUE;
                    }
                }
                return result;
            } catch (Exception e) {
                Log.e(LOG_TAG, "StartTask Exception caught", e);
            }
            return Boolean.FALSE;
        }

        protected void onPostExecute(Boolean result) {
            Log.d(getLogTag(), "Camera2APIActivity::StartTask::onPostExecute");
            startTask_ = null;
        }
    }

    //=========================================================================
    private class EndTask extends AsyncTask<String, Void, Boolean> {

        protected void onPreExecute() {
            Log.d(getLogTag(), "Camera2APIActivity::EndTask::onPreExecute");
        }

        protected Boolean doInBackground(String... tmpStr) {
            Log.d(getLogTag(), "Camera2APIActivity::EndTask::doInBackground");
            try {
                if (previewCamera_ != null) {
                    previewCamera_.stop();
                    previewCamera_ = null;
                }

                if (captureCamera_ != null) {
                    captureCamera_.stop();
                    captureCamera_ = null;
                }
            } catch (Exception e) {
                Log.e(LOG_TAG, "EndTask Exception caught", e);
                return Boolean.FALSE;
            }
            return Boolean.TRUE;
        }

        protected void onPostExecute(Boolean result) {
            Log.d(getLogTag(), "Camera2APIActivity::EndTask::onPostExecute");
            endTask_ = null;
        }
    }

    //=========================================================================
    @Override
    protected AsyncTask<String, Void, Boolean> getStartTask() {
        return new StartTask();
    }

    @Override
    protected AsyncTask<String, Void, Boolean> getEndTask() {
        return new EndTask();
    }

}

/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.seccamsample;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.graphics.ImageFormat;

import com.qualcomm.qti.seccamsample.R;
import com.qualcomm.qti.seccamapi.SecureSurface;
import com.qualcomm.qti.seccamapi.SecureCameraSurface;
import com.qualcomm.qti.seccamapi.SecCamServiceClient;
import com.qualcomm.qti.seccamapi.SecCamServiceVendorClient;

public class CameraActivity
    extends Activity
    implements SurfaceHolder.Callback, SecureSurface.FrameCallback {

    protected SurfaceView cameraPreview_;
    protected SurfaceHolder surfaceHolder_;

    protected static final boolean useVendorLib_ = false;
    // Setting captureCameraEnabled to false will allow to only test secure preview
    protected static final boolean captureCameraEnabled_ = true;
    protected static final boolean previewCameraEnabled_ = true;
    protected static final boolean useInitBio_ = true;

    protected AsyncTask<String, Void, Boolean> startTask_ = null;
    protected AsyncTask<String, Void, Boolean> endTask_ = null;

    protected static final String LOG_TAG = "SECCAM-SAMPLE-APP";
    protected static final long DEFAULT_BIOMETRICS_BUFFER_SIZE = 20 * 1024;

    protected CameraActivity cameraActivity_ = null;

    protected Boolean frameCallbackEnabled_ = Boolean.TRUE;
    protected int frameCallbackReturnParamsSize_ = 3 * Long.BYTES;
    protected byte[] frameCallbackReturnParams_ = new byte[frameCallbackReturnParamsSize_];
    protected long lastFrameNumber_ = 0;
    protected long lastFrameTimestamp_ = 0;
    protected int droppedFrames_ = 0;

    //=========================================================================
    public String getLogTag() {
        return LOG_TAG;
    }

    public SurfaceView getPreviewView() {
        return cameraPreview_;
    }

    //=========================================================================
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(getLogTag(), "CameraActivity::onCreate");

        // Prevent screen from going to sleep during preview
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_camera);
        cameraPreview_ = (SurfaceView) findViewById(R.id.preview_view);
        surfaceHolder_ = cameraPreview_.getHolder();
        surfaceHolder_.addCallback(this);
        surfaceHolder_.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        final Button doneButton = (Button) findViewById(R.id.done_button);
        doneButton.setOnClickListener(mDoneButtonClickListener);
        cameraActivity_ = this;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(getLogTag(), "CameraActivity::onDestroy");
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(getLogTag(), "CameraActivity::onStart");
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(getLogTag(), "CameraActivity::onStop");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(getLogTag(), "CameraActivity::onResume");
        // Since the camera resources are released when the app is paused, the camera and the preview must be reopened on resume
        cameraPreview_.setVisibility(SurfaceView.VISIBLE);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(getLogTag(), "CameraActivity::onCreate");
        // It is vital to release camera resources when the app is not active, otherwise a runtime exception is thrown
        cameraPreview_.setVisibility(SurfaceView.GONE);
    }

    //=========================================================================
    private OnClickListener mDoneButtonClickListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            // Hide the SufaceView before stopping the secure preview. This is done to avoid a green screen which may appear
            // as a result of cleaning the preview buffers, while still displaying them on the screen.
            cameraPreview_.setVisibility(SurfaceView.GONE);
            final Button doneButton = (Button) findViewById(R.id.done_button);
            doneButton.setVisibility(View.GONE);
            finish();
        }
    };

    //=========================================================================
    // Method frameCallbackReturnParamsAsLong(...) converts native uint64_t to Java long type.
    //=========================================================================
    protected long frameCallbackReturnParamsAsLong(byte[] returnParams, int index) {
        long ret = 0L;
        if (returnParams.length >= (Long.BYTES * (index+1))) {
            ByteBuffer buf = ByteBuffer.allocate(Long.BYTES).order(ByteOrder.nativeOrder());
            buf.put(returnParams, (Long.BYTES * index), Long.BYTES);
            buf.flip();
            ret =  buf.getLong();
        }
        return ret;
    }

    //=========================================================================
    // The following code is an example of how to handle user defined parameters returned from a TA with
    // the frame notification callback.
    // Note: Method enableFrameCallback(...) of the secure capture surface should be invoked to enable the callback.
    // In this example the TA returns 3 UINT64 elements:
    //   [0] - Camera ID
    //   [1] - Frame ID
    //   [2] - Frame Timestamp
    // The values of the frame parameters above is equal to the values in the SecureSurface.FrameInfo structure.
    //=========================================================================
    public void onSecureFrameAvalable(SecureSurface.FrameInfo frameInfo, byte[] returnParams) {
        Log.d(getLogTag(), "CameraActivity::onSecureFrameAvalable - " +
                "Id: " + frameInfo.frameNumber_ + ", " +
                "Time Offest: " + (frameInfo.timeStamp_-lastFrameTimestamp_) + ", " +
                "Dropped : " + droppedFrames_ + ", " +
                "[" + frameInfo.width_ + "(" + frameInfo.stride_ + ")x" + frameInfo.height_ +  "], " +
                "ret = [" +
                frameCallbackReturnParamsAsLong(returnParams, 0) + ", "+
                frameCallbackReturnParamsAsLong(returnParams, 1) + ", "+
                frameCallbackReturnParamsAsLong(returnParams, 2) + "]");

        if (lastFrameNumber_ != 0) {
            droppedFrames_ += ((frameInfo.frameNumber_ - 1 ) - lastFrameNumber_);
        }
        lastFrameNumber_ = frameInfo.frameNumber_;
        lastFrameTimestamp_ = frameInfo.timeStamp_;
    }

    //=========================================================================
    protected void dispatchVendorCommand_exchangeTimestamps() {
        // Set a bundle with the data needed for the command.
        Bundle timestamp_bundle = new Bundle();
        timestamp_bundle.putLong("hlosTimestamp", System.currentTimeMillis());

        // Dispatch the command using the vendor command ID, and the created bundle.
        SecCamServiceClient.getInstance().dispatchVendorCommand(SecCamServiceVendorClient.MSG_VENDOR_EXCHANGE_TIMESTAMP, timestamp_bundle);
    }

    //=========================================================================
    protected void dispatchVendorCommand_initBiometricsBuffer(long bufferSize) {
         // Set a bundle with the data needed for the command.
        Bundle initBio_bundle = new Bundle();
        initBio_bundle.putLong("bufferSize", bufferSize);

        // Dispatch the command using the vendor command ID, and the created bundle.
        SecCamServiceClient.getInstance().dispatchVendorCommand(SecCamServiceVendorClient.MSG_VENDOR_INIT_BIOMETRICS_BUFFER, initBio_bundle);
    }

    //=========================================================================
    private class VendorTask extends AsyncTask<String, Void, Boolean> {

        protected void onPreExecute() {
            Log.d(getLogTag(), "CameraActivity::VendorTask::onPreExecute");
        }

        protected Boolean doInBackground(String... tmpStr) {
            try {
                Log.d(getLogTag(), "CameraActivity::VendorTask::doInBackground");

                if (useVendorLib_) {
                    //Usage of a vendor custom command, exchanging timestamps between HLOS and TZ.
                    dispatchVendorCommand_exchangeTimestamps();
                }

                if (useInitBio_) {
                    dispatchVendorCommand_initBiometricsBuffer(DEFAULT_BIOMETRICS_BUFFER_SIZE);
                }

            } catch (Exception e) {
                Log.e(LOG_TAG, "CameraActivity::VendorTask::doInBackground - StartTask Exception caught", e);
                return Boolean.FALSE;
            }

            return Boolean.TRUE;
        }

        protected void onPostExecute(Boolean result) {
            Log.d(getLogTag(), "CameraAPIActivity::VendorTask::onPostExecute");
        }
    }

    //=========================================================================
    protected AsyncTask<String, Void, Boolean> getStartTask() {
        return null;
    }

    protected AsyncTask<String, Void, Boolean> getEndTask() {
        return null;
    }

    //=========================================================================
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(getLogTag(), "CameraActivity::surfaceCreated");

        new VendorTask().execute();

        if (startTask_ == null) {
            startTask_ = getStartTask();
            startTask_.execute();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(getLogTag(), "CameraActivity::surfaceDestroyed");
        if (endTask_ == null) {
            endTask_ = getEndTask();
            endTask_.execute();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(getLogTag(), "CameraActivity::surfaceChanged - format:" + format +
                ", [" + width + "x" + height +"]");
    }
}

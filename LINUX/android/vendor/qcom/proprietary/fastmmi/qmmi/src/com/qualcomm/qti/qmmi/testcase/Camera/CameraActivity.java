/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.qmmi.testcase.Camera;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.Face;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.SparseIntArray;
import android.util.Size;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class CameraActivity extends BaseActivity{

    private static String FRONT_IMG_NAME = "FRONT_PIC.jpg";
    private static String BACK_AUX_IMG_NAME = "BACK_AUX_PIC.jpg";
    private static String BACK_MAIN_IMG_NAME = "BACK_MAIN_PIC.jpg";

    private static String CAMERA_BACK_MAIN = "back";
    private static String CAMERA_FRONT = "front";

    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Button mTakePicBtn;
    private TextView mInfoText;
    private FaceView mFaceView;
    private Toast mToast = null;

    private boolean mDoSnapshot = false;
    private boolean mDoFaceDetect = false;
    private boolean mEverDetected = false;
    private boolean mEverShoted = false;
    private String mImageName = null;
    private String mCameraId = null;
    private int mState;
    private boolean mFlashSupported;
    private Size mPreviewSize;

    //Camera state: Showing camera preview.
    private static final int STATE_PREVIEW = 0;
    //Camera state: Waiting for capture.
    private static final int STATE_WAITING_CAPTURE = 1;
    private static final int STATE_WAITING_LOCK = 2;
    //Camera state: Picture was taken.
    private static final int STATE_PICTURE_TAKEN = 3;
    //Max preview width that is guaranteed by Camera2 API
    private static final int MAX_PREVIEW_WIDTH = 1920;
    //Max preview height that is guaranteed by Camera2 API
    private static final int MAX_PREVIEW_HEIGHT = 1080;

    private CameraManager mCameraMgr = null;
    private CameraDevice mCameraDevice = null;

    //An additional thread for running tasks that shouldn't block the UI.
    private HandlerThread mBackgroundThread;

    //Handler for running tasks in the background.
    private Handler mBackgroundHandler = null;

    //ImageReader} that handles still image capture.
    private ImageReader mImageReader;

    // Semaphore to prevent the app from exiting before closing the camera.
    private Semaphore mCameraOpenCloseLock = new Semaphore(1);

    private CaptureRequest.Builder mPreviewRequestBuilder;

    //CameraCaptureSession } for camera preview.
    private CameraCaptureSession mCaptureSession;

    //Conversion from screen rotation to JPEG orientation.
    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();
    static {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    //Orientation of the camera sensor
    private int mSensorOrientation;

    /**
     * This a callback object for the ImageReader. "onImageAvailable" will be called when a
     * still image is ready to be saved.
     */
    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener
            = new ImageReader.OnImageAvailableListener() {

        @Override
        public void onImageAvailable(ImageReader reader) {
            mBackgroundHandler.post(new ImageSaver(reader.acquireNextImage(), mImageName));
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);

        initData();
        initView();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        LogUtils.logi("onPause");
        closeCamera();
        stopBackgroundThread();
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        LogUtils.logi("onDestroy");
        super.onDestroy();
    }

    private void initData(){

        startBackgroundThread();

        String method = mTestCase.getMethod();
        if (method != null && CameraService.isMethodSupported(method)) {
            if (method.equalsIgnoreCase("snapshot")) {
                mDoSnapshot = true;
            }else if(method.equalsIgnoreCase("face_detect")){
                mEverDetected = false;
                mDoFaceDetect = true;
            }
        } else {
            LogUtils.loge("no method , or not support method");
        }
    }

    private void initView(){
        mTakePicBtn = (Button) findViewById(R.id.take_picture);
        mTakePicBtn.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View arg0) {
                takePicture();
            }
        });

        mSurfaceView = (SurfaceView) findViewById(R.id.surface_view);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(new SurfaceHolder.Callback(){

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                // TODO Auto-generated method stub
                LogUtils.logi("surfaceChanged");
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                // TODO Auto-generated method stub
                LogUtils.logi("surfaceCreated");
                openCamera();
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                // TODO Auto-generated method stub
                LogUtils.logi("surfaceDestroyed");
            }
        });

        mSurfaceView.addOnLayoutChangeListener(new View.OnLayoutChangeListener() {
            @Override
            public void onLayoutChange(View v, int left, int top, int right,
                                       int bottom, int oldLeft, int oldTop, int oldRight,
                                       int oldBottom) {
                int width = right - left;
                int height = bottom - top;
                if (mFaceView != null) {
                    mFaceView.setSurfaceTextureSize(width, height);
                }
            }
        });

        mFaceView = (FaceView) findViewById(R.id.face_view);

        mInfoText = (TextView) findViewById(R.id.case_detail_title);

    }

    private void openCamera(){
        LogUtils.loge("Int Camera And Preview");

        setUpCameraOutputs(mSurfaceView.getWidth(), mSurfaceView.getHeight());

        try {

            if (mCameraId == null || mCameraId.equals("")) {
                updateUIwithError(true, R.string.camera_device_id_not_found);
                return;
            }

            if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }

            mCameraMgr.openCamera(mCameraId,
                    new CameraDevice.StateCallback(){

                        @Override
                        public void onDisconnected(CameraDevice camera) {
                            // TODO Auto-generated method stub
                            LogUtils.loge("Camera disconnected");
                            camera.close();
                            mCameraDevice = null;
                            updateUIwithError(true, R.string.camera_open_error);
                            mCameraOpenCloseLock.release();
                        }

                        @Override
                        public void onError(CameraDevice camera, int error) {
                            // TODO Auto-generated method stub
                            LogUtils.loge("Open Camera Failed" + error);
                            camera.close();
                            mCameraDevice = null;
                            updateUIwithError(true, R.string.camera_open_error);
                            mCameraOpenCloseLock.release();
                        }

                        @Override
                        public void onOpened(CameraDevice camera) {
                            // TODO Auto-generated method stub
                            LogUtils.loge("Open Camera Success");
                            mCameraDevice = camera;
                            updateUIwithError(false, 0);
                            mCameraOpenCloseLock.release();
                            createCameraPreviewSession();
                        }
                    },
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    /**
     * Sets up member variables related to camera.
     *
     * @param width  The width of available size for camera preview
     * @param height The height of available size for camera preview
     */
    private void setUpCameraOutputs(int width, int height) {
        String cameraType = mTestCase.getParameter().get("type");
        LogUtils.logi("camera case type : " + cameraType);

        try {
            mCameraMgr = (CameraManager)getSystemService(Context.CAMERA_SERVICE);
            String[] idList = mCameraMgr.getCameraIdList();
            LogUtils.logi("id num : " + idList.length);

            for (String cameraId : idList) {
                LogUtils.logi("camera id in IdList : " + cameraId);

                CameraCharacteristics characteristics
                        = mCameraMgr.getCameraCharacteristics(cameraId);

                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                LogUtils.logi("facing : " + facing.byteValue());

                if (facing != null){
                    if (cameraType.equalsIgnoreCase(CAMERA_BACK_MAIN) && facing == CameraCharacteristics.LENS_FACING_BACK) {
                        mImageName = BACK_MAIN_IMG_NAME;
                        mCameraId = cameraId;
                        LogUtils.logi("CAMERA MAIN BACK Case");
                    } else if (cameraType.equalsIgnoreCase(CAMERA_FRONT) && facing == CameraCharacteristics.LENS_FACING_FRONT) {
                        mImageName = FRONT_IMG_NAME;
                        mCameraId = cameraId;
                        LogUtils.logi("Front CAMERA Case");
                    }
                }

                LogUtils.logi("Current case camera id : " + mCameraId);

                if (mCameraId == null || mCameraId.equals("")){
                    continue;
                }

                StreamConfigurationMap map = characteristics.get(
                        CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                if (map == null) {
                    continue;
                }

                // For still image captures, we use the largest available size.
                Size largest = Collections.max(
                        Arrays.asList(map.getOutputSizes(ImageFormat.JPEG)),
                        new CompareSizesByArea());
                mImageReader = ImageReader.newInstance(largest.getWidth(), largest.getHeight(),
                        ImageFormat.JPEG, /*maxImages*/2);
                mImageReader.setOnImageAvailableListener(
                        mOnImageAvailableListener, mBackgroundHandler);

                mSensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);

                // Check if the flash is supported.
                Boolean available = characteristics.get(CameraCharacteristics.FLASH_INFO_AVAILABLE);
                mFlashSupported = available == null ? false : available;
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    private void createCameraPreviewSession() {
        LogUtils.logi("Create Camera Preview Session");

        try {
            mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewRequestBuilder.addTarget(mSurfaceHolder.getSurface());
            mState = STATE_PREVIEW;
            mCameraDevice.createCaptureSession(
                    Arrays.asList(mSurfaceHolder.getSurface(), mImageReader.getSurface()),
                    new CameraCaptureSession.StateCallback(){

                        @Override
                        public void onConfigureFailed(CameraCaptureSession arg0) {
                            // TODO Auto-generated method stub
                            updateUIwithError(true, R.string.camera_preview_error);
                        }

                        @Override
                        public void onConfigured(CameraCaptureSession session) {
                            // TODO Auto-generated method stub
                            if (mCameraDevice == null) return;

                            LogUtils.logi("onConfigured");

                            mCaptureSession = session;

                            // Auto focus should be continuous for camera preview.
                            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                                    CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                            /**mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE,
                                    CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);**/
                            mPreviewRequestBuilder.set(CaptureRequest.STATISTICS_FACE_DETECT_MODE,
                                    CameraMetadata.STATISTICS_FACE_DETECT_MODE_FULL);
                            // Flash is automatically enabled when necessary.
                            //setAutoFlash(mPreviewRequestBuilder);
                            // Finally, we start displaying the camera preview.
                            try {
                                mCaptureSession.setRepeatingRequest(mPreviewRequestBuilder.build(),
                                        mCaptureCallback,
                                        mBackgroundHandler);
                            } catch (CameraAccessException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                            }
                        }

                    },
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private CameraCaptureSession.CaptureCallback mCaptureCallback =
            new CameraCaptureSession.CaptureCallback() {

                @Override
                public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                        TotalCaptureResult result) {
                    // TODO Auto-generated method stub
                    LogUtils.logi("onCaptureCompleted");
                    mCaptureSession = session;
                    checkState(result);
                }

                @Override
                public void onCaptureProgressed(CameraCaptureSession session, CaptureRequest request,
                        CaptureResult partialResult) {
                    // TODO Auto-generated method stub
                    //LogUtils.logi("onCaptureProgressed");
                    mCaptureSession = session;
                    checkState(partialResult);
                }

                private void checkState(CaptureResult result){
                    LogUtils.logi("checkState");

                    final Face[] faces = result.get(CaptureResult.STATISTICS_FACES);

                    if(faces != null) {
                        LogUtils.logd("faces : " + faces.length);
                        CameraActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mFaceView.setFaces(faces, mCameraId);
                            }
                        });
                    }

                    //LogUtils.logd("mDoFaceDetect : " + mDoFaceDetect);
                    //LogUtils.logd("mEverDetected : " + mEverDetected);

                    if(mDoFaceDetect && !mEverDetected) {
                        if (faces != null && faces.length > 0){
                            mEverDetected = true;
                            LogUtils.logd("face detected, set case pass");
                            updateResultForCase(mTestCase.getName(),TestCase.STATE_PASS);
                        } else {
                            updateResultForCase(mTestCase.getName(),TestCase.STATE_FAIL);
                        }
                    }

                    switch (mState) {
                        case STATE_PREVIEW:
                            LogUtils.logd("mDoFaceDetect : " + mDoFaceDetect);
                            LogUtils.logd("mDoSnapshot : " + mDoSnapshot);
                            LogUtils.logd("mEverShoted : " + mEverShoted);
                            if (!mDoFaceDetect && mDoSnapshot && !mEverShoted) {
                                LogUtils.logd("Do Snap Shot : ");
                                mEverShoted = true;
                                takePicture();
                            }
                            break;
                        case STATE_WAITING_LOCK:
                            captureStillPicture();
                        case STATE_WAITING_CAPTURE:
                            unlockFocus();
                    }
                }
    };

    private void captureStillPicture() {
        try {

            if (null == mCameraDevice) {
                return;
            }

            // This is the CaptureRequest.Builder that we use to take a picture.
            final CaptureRequest.Builder captureBuilder =
                    mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
            captureBuilder.addTarget(mImageReader.getSurface());

            // Use the same AE and AF modes as the preview.
            captureBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                    CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
            //setAutoFlash(captureBuilder);

            // Orientation
            int rotation = getWindowManager().getDefaultDisplay().getRotation();
            captureBuilder.set(CaptureRequest.JPEG_ORIENTATION, getOrientation(rotation));

            mState = STATE_WAITING_CAPTURE;
            mCaptureSession.stopRepeating();
            mCaptureSession.capture(captureBuilder.build(), mCaptureCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    /**
     * Unlock the focus. This method should be called when still image capture sequence is
     * finished.
     */
    private void unlockFocus() {
        try {
            // Reset the auto-focus trigger
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                    CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
            //setAutoFlash(mPreviewRequestBuilder);
            mCaptureSession.capture(mPreviewRequestBuilder.build(), mCaptureCallback,
                    mBackgroundHandler);
            // After this, the camera will go back to the normal state of preview.
            mState = STATE_PREVIEW;
            mCaptureSession.setRepeatingRequest(mPreviewRequestBuilder.build(), mCaptureCallback,
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private int getOrientation(int rotation) {
        // Sensor orientation is 90 for most devices, or 270 for some devices (eg. Nexus 5X)
        // We have to take that into account and rotate JPEG properly.
        // For devices with orientation of 90, we simply return our mapping from ORIENTATIONS.
        // For devices with orientation of 270, we need to rotate the JPEG 180 degrees.
        return (ORIENTATIONS.get(rotation) + mSensorOrientation + 270) % 360;
    }

    private void setAutoFlash(CaptureRequest.Builder requestBuilder) {
        if (mFlashSupported) {
            requestBuilder.set(CaptureRequest.CONTROL_AE_MODE,
                    CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
        }
    }

    private void takePicture(){
        try {
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                    CameraMetadata.CONTROL_AF_TRIGGER_START);
            mState = STATE_WAITING_LOCK;
            mCaptureSession.capture(mPreviewRequestBuilder.build(), mCaptureCallback,
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void updateUIwithError(final boolean isError, final int resId){
        LogUtils.loge("Error : " + isError);
        this.runOnUiThread(new Runnable(){

            @Override
            public void run() {
                // TODO Auto-generated method stub
                if (isError){
                    updateResultForCase(mTestCase.getName(),TestCase.STATE_FAIL);
                    mSurfaceView.setVisibility(View.INVISIBLE);
                    mFaceView.setVisibility(View.INVISIBLE);
                    mTakePicBtn.setVisibility(View.INVISIBLE);
                    mInfoText.setVisibility(View.VISIBLE);
                    mInfoText.setText(getString(resId));
                } else {
                    mSurfaceView.setVisibility(View.VISIBLE);
                    mFaceView.setVisibility(View.VISIBLE);
                    mTakePicBtn.setVisibility(View.VISIBLE);
                    mInfoText.setVisibility(View.INVISIBLE);
                }
            }

        });
    }

    /**
     * Saves a JPEG Image into the specified File
     */
    private class ImageSaver implements Runnable {

        /**
         * The JPEG image
         */
        private final Image mImage;
        /**
         * The file we save the image into.
         */
        private final String mImageName;

        public ImageSaver(Image image, String imageName) {
            mImage = image;
            mImageName = imageName;
        }

        @Override
        public void run() {

            ByteBuffer buffer = mImage.getPlanes()[0].getBuffer();
            byte[] bytes = new byte[buffer.remaining()];
            buffer.get(bytes);
            FileOutputStream output = null;
            try {
                String filePath = getApplicationContext().getFilesDir()+ "/" + mImageName;
                File file = new File(filePath);
                output = new FileOutputStream(file);
                output.write(bytes);
                updateResultForCase(mTestCase.getName(),TestCase.STATE_PASS);
                mTestCase.addTestData("snapshop_picture", filePath);
                showToast(getString(R.string.camera_saved_pic));
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                mImage.close();
                if (null != output) {
                    try {
                        output.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    /**
     * Compares two {@code Size}s based on their areas.
     */
    static class CompareSizesByArea implements Comparator<Size> {

        @Override
        public int compare(Size lhs, Size rhs) {
            // We cast here to ensure the multiplications won't overflow
            return Long.signum((long) lhs.getWidth() * lhs.getHeight() -
                    (long) rhs.getWidth() * rhs.getHeight());
        }

    }

    /**
     * Given {@code choices} of {@code Size}s supported by a camera, choose the smallest one that
     * is at least as large as the respective texture view size, and that is at most as large as the
     * respective max size, and whose aspect ratio matches with the specified value. If such size
     * doesn't exist, choose the largest one that is at most as large as the respective max size,
     * and whose aspect ratio matches with the specified value.
     *
     * @param choices           The list of sizes that the camera supports for the intended output
     *                          class
     * @param textureViewWidth  The width of the texture view relative to sensor coordinate
     * @param textureViewHeight The height of the texture view relative to sensor coordinate
     * @param maxWidth          The maximum width that can be chosen
     * @param maxHeight         The maximum height that can be chosen
     * @param aspectRatio       The aspect ratio
     * @return The optimal {@code Size}, or an arbitrary one if none were big enough
     */
    private static Size chooseOptimalSize(Size[] choices, int textureViewWidth,
            int textureViewHeight, int maxWidth, int maxHeight, Size aspectRatio) {

        // Collect the supported resolutions that are at least as big as the preview Surface
        List<Size> bigEnough = new ArrayList<>();
        // Collect the supported resolutions that are smaller than the preview Surface
        List<Size> notBigEnough = new ArrayList<>();
        int w = aspectRatio.getWidth();
        int h = aspectRatio.getHeight();
        for (Size option : choices) {
            if (option.getWidth() <= maxWidth && option.getHeight() <= maxHeight &&
                    option.getHeight() == option.getWidth() * h / w) {
                if (option.getWidth() >= textureViewWidth &&
                    option.getHeight() >= textureViewHeight) {
                    bigEnough.add(option);
                } else {
                    notBigEnough.add(option);
                }
            }
        }

        // Pick the smallest of those big enough. If there is no one big enough, pick the
        // largest of those not big enough.
        if (bigEnough.size() > 0) {
            return Collections.min(bigEnough, new CompareSizesByArea());
        } else if (notBigEnough.size() > 0) {
            return Collections.max(notBigEnough, new CompareSizesByArea());
        } else {
            LogUtils.loge("Couldn't find any suitable preview size");
            return choices[0];
        }
    }

    /**
     * Starts a background thread and its {@link Handler}.
     */
    private void startBackgroundThread() {
        if (mBackgroundThread == null){
            LogUtils.loge("Start Bg Thread");
            mBackgroundThread = new HandlerThread("CameraBackground");
            mBackgroundThread.start();
            if (mBackgroundHandler == null){
                mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
            }
        }
    }

    /**
     * Stops the background thread and its {@link Handler}.
     */
    private void stopBackgroundThread() {
        if (mBackgroundThread != null){
            LogUtils.loge("Stop Bg Thread");
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void closeCamera() {
        LogUtils.logi("closeCamera");
        try {
            mCameraOpenCloseLock.acquire();
            if (null != mCaptureSession) {
                mCaptureSession.close();
                mCaptureSession = null;
            }
            if (null != mCameraDevice) {
                mCameraDevice.close();
                mCameraDevice = null;
            }
            if (null != mImageReader) {
                mImageReader.close();
                mImageReader = null;
            }
        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        } finally {
            mCameraOpenCloseLock.release();
        }
    }

    private void showToast(final String text) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mToast == null){
                    mToast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    @Override
    protected int getLayoutId() {
        // TODO Auto-generated method stub
        return R.layout.camera_act;
    }

}

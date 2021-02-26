/* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.vt;

import android.content.Context;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraCharacteristics.Key;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureRequest.Builder;
import android.hardware.camera2.params.OutputConfiguration;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.util.Range;
import android.util.Size;
import android.view.Surface;

import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.utils.Config;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Wrapper class for Camera2.
 *
 */
class Camera2 extends Camera {
    private static final String TAG = "Camera2";

    public static final CaptureRequest.Key<Byte> AV_TIMER =
            new CaptureRequest.Key<Byte>("org.codeaurora.qcamera3.av_timer.use_av_timer",
             byte.class);
    public static final byte ENABLE_AV_TIMER = 1;

    private CameraManager mCameraManager;
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCameraSession;
    private Builder mCaptureBuilder;

    private Surface mPreviewSurface;
    private Surface mRecordingSurface;

    private int mOpenState;
    private boolean mIsPreviewStarted;
    private boolean mIsRecordingStarted;
    private boolean mIsCreateSessionPending;

    private Rect mCropRegion;
    private Range<Integer> mFps;
    private int mFrameSkippingRate;

    private int mPendingStartRequest;
    private int mSessionId;
    private int mSensorOrientation;
    private int mCameraFacing;
    private ConfigIms mCameraConfigIms;
    private CameraActionSoundHelper mActionSoundHelper;

    public static final int CAMERA_STATE_CLOSED = 0;
    public static final int CAMERA_STATE_OPENING = 1;
    public static final int CAMERA_STATE_OPEN = 2;

    public static final int CAMERA_REQUEST_START_NONE = 0;
    public static final int CAMERA_REQUEST_START_PREVIEW = 1;
    public static final int CAMERA_REQUEST_START_RECORDING = 2;


    private static final int ZOOM_CONSTANT_NOT_SUPPORTED = 1;
    private static final int GENERIC_CONSTANT_INVALID_VALUE = -1;

    /* package */
    Camera2(Context context, String id, Camera.Listener listener) {
        super(context, id, listener);
        Log.i(this, "Ctor, id=" + id);
        mCameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try {
            mSensorOrientation = getCameraCharacteristic(mCameraId,
                    CameraCharacteristics.SENSOR_ORIENTATION);
            mCameraFacing = getCameraCharacteristic(mCameraId,
                    CameraCharacteristics.LENS_FACING);
            Log.i(this, "Ctor: Camera sensor orientation = " + mSensorOrientation +
                " cameraFacing = " + mCameraFacing);

            mActionSoundHelper = new CameraActionSoundHelper(context);
            mCameraConfigIms =
                    new ConfigIms(getDefaultPreviewSize(), ImsMediaConstants.DEFAULT_FPS,
                    ImsMediaConstants.PORTRAIT_MODE);
        } catch (CameraAccessException e) {
            notifyOnError(e.getReason());
        }
        reset();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void open() throws CameraAccessException {
        Log.i(this, "open");

        List<String> ids = Arrays.asList(mCameraManager.getCameraIdList());
        Log.v(this, "open: Available camera IDs=" + ids);
        if (!ids.contains(mCameraId)) {
            throw new CameraException("Incorrect id, " + mCameraId);
        } else if (!isClosed()) {
            Log.i(this, "open: camera is already opened, state" + mOpenState);
            return;
        }

        try {
            mOpenState = CAMERA_STATE_OPENING;
            mCameraManager.openCamera(mCameraId, mStateListener, null);
            mActionSoundHelper.open();
        } catch (CameraAccessException e) {
            Log.e(this, "open: Failed to open camera, id=" + mCameraId + "Exception=" + e);
            mOpenState = CAMERA_STATE_CLOSED;
            throw e;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void close() {
        Log.i(this, "close");

        try {
            stopPreview();
        } catch (Exception e) {
            Log.e(this, "close: Failed to close camera preview/recording, exception=" + e);
        }

        if (mCameraSession != null) {
            mCameraSession.close();
        }

        if (mCameraDevice != null) {
            mCameraDevice.close();
        }

        mActionSoundHelper.close();

        reset();
    }

    private void reset() {
        Log.v(this, "reset");

        mCameraDevice = null;
        mOpenState = CAMERA_STATE_CLOSED;
        mIsPreviewStarted = false;
        mIsRecordingStarted = false;
        mPendingStartRequest = CAMERA_REQUEST_START_NONE;
        mPreviewSurface = null;
        mRecordingSurface = null;
        mFps = null;
        mCropRegion = null;
        mCameraSession = null;
        mSessionId = 0;
        mIsCreateSessionPending= false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setZoom(float v) throws CameraAccessException {
        Log.i(this, "setZoom " + v);
        failIfClosed();

        if (v < getMinZoom() || v > getMaxZoom()) {
            Log.e(this, "setZoom: incorrect zoom value. v=" + v);
            throw new CameraAccessException(CameraAccessException.CAMERA_ERROR);
        }

        Rect activeRegion = getCameraCharacteristic(mCameraId,
                CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        Log.v(this, "setZoom: ActiveArraySize=" + activeRegion);

        Rect cropRegion = new Rect();

        // Find the center of Active Aray.
        int xCenter = activeRegion.width() / 2;
        int yCenter = activeRegion.height() / 2;

        // Compute the delta by which the active array needs to be insetted.
        // NOTE: We divide the delta by 2 since we inset the rect from both
        // sides left-top and bottom-right.
        int xDelta = (int) (activeRegion.width() / (2*v));
        int yDelta = (int) (activeRegion.height() / (2*v));

        // Inset the active region by delta to achieve zoom.
        cropRegion.set(xCenter - xDelta, yCenter - yDelta, xCenter + xDelta, yCenter + yDelta);
        Log.v(this, "setZoom: CropRegionSize=" + cropRegion);

        mCropRegion = cropRegion;
        if (mCameraSession != null) {
            updateParams(mCaptureBuilder);
            restartRepeating(build(mCaptureBuilder));
        } else {
            Log.i(this, "Setting zoom is deferred.");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void reconfigure(ConfigIms cfg) throws CameraAccessException {
        Log.i(this, "reconfigure " + cfg);
        failIfClosed();

        mCameraConfigIms = cfg;
        mFps = computeBestFps(cfg.fps);

        if (mCameraSession != null) {
            updateParams(mCaptureBuilder);
            restartRepeating(build(mCaptureBuilder));
        } else {
            Log.i(this, "Reconfiguration is deferred.");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isZoomSupported() throws CameraAccessException {
        // Note: ImsCamera has to be open to query for zoom.
        // However, it is not required for Camera2. To be consistent
        // we either need to enforce the same for camera2 (throw an exception),
        // or we should update ImsCamera to open (if it's not already open) and
        // and then query for zoom.
        // TODO Check if UI opens the camera before querying for camera capabilities.
        return getMaxZoom() > ZOOM_CONSTANT_NOT_SUPPORTED;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isFacingFront() throws CameraAccessException {
        return mCameraFacing == CameraCharacteristics.LENS_FACING_FRONT;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public float getMaxZoom() throws CameraAccessException {
        try {
            final float maxZoom = getCameraCharacteristic(mCameraId,
                    CameraCharacteristics.SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);
            Log.v(this, "getMaxZoom: maxZoom=" + maxZoom);
            return maxZoom;
        } catch (CameraAccessException e) {
            Log.e(this, "getMaxZoom: Failed to retrieve Max Zoom, " + e);
            throw e;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public float getMinZoom() throws CameraAccessException {
        if (!isOpen()) {
            throw new CameraAccessException(CameraAccessException.CAMERA_ERROR);
        }
        final int ZOOM_MIN_VALUE = 1;
        return ZOOM_MIN_VALUE;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Size getPreviewSize() throws CameraAccessException {
        return mCameraConfigIms.getSize();
    }

    private Size getDefaultPreviewSize() throws CameraAccessException {
        StreamConfigurationMap map = getCameraCharacteristic(mCameraId,
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        return map.getOutputSizes(SurfaceTexture.class)[0];
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startPreview(Surface surface) throws CameraAccessException {
        Log.i(this, "startPreview, surface=" + surface);

        if (isPreviewStarted()) {
            Log.i(this, "startPreview: Preview has already started.");
            return;
        }

        failIfClosed();
        failIfSurfaceNull(surface, "Error preview surface is null");


        mPreviewSurface = surface;
        mIsPreviewStarted = true;
        mSessionId = genSessionId();

        if (shallDefer()) {
            Log.i(this, "startPreview: Deferring startPreview request, currRequest="
                 + mPendingStartRequest);
            mPendingStartRequest = CAMERA_REQUEST_START_PREVIEW;
        } else {
            doStartPreview();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void stopPreview() throws CameraAccessException {
        Log.i(this, "stopPreview");
        final boolean isRecordingStarted = isRecordingStarted();

        if (!isRecordingStarted && !isPreviewStarted()) {
            return;
        }

        mIsPreviewStarted = mIsRecordingStarted = false;
        clearStartRequest();
        cancelSession();

        if (isRecordingStarted) {
            mActionSoundHelper.onRecordingStopped();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startRecording(Surface previewSurface, Surface recordingSurface)
            throws CameraAccessException {
        Log.i(this, "startRecording: PreviewSurface=" + previewSurface + " RecordingSurface="
                    + recordingSurface);

        if (isRecordingStarted()) {
            Log.i(this, "startRecording: Recording has already started.");
            return;
        }

        failIfClosed();
        failIfSurfaceNull(previewSurface, "Error preview surface is null");
        failIfSurfaceNull(recordingSurface, "Error recording surface is null");


        mRecordingSurface = recordingSurface;
        mPreviewSurface = previewSurface;
        mIsRecordingStarted = mIsPreviewStarted = true;
        mSessionId = genSessionId();

        if (shallDefer()) {
            Log.i(this, "startRecording: Deferring startRecording request, currRequest="
                    + mPendingStartRequest);
            mPendingStartRequest = CAMERA_REQUEST_START_RECORDING;
        } else {
            doStartRecording();
        }
        mActionSoundHelper.onRecordingStarted();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void stopRecording() throws CameraAccessException {
        Log.i(this, "stopRecording");

        if (!isRecordingStarted()) {
            return;
        }

        mIsRecordingStarted = false;
        if (mIsPreviewStarted) {
            startPreview(mPreviewSurface);
        } else {
            cancelSession();
        }
        mActionSoundHelper.onRecordingStopped();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isOpen() {
        return mOpenState != CAMERA_STATE_CLOSED;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPreviewStarted() {
        return mIsPreviewStarted;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isRecordingStarted() {
        return mIsRecordingStarted;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getSensorOrientation() throws CameraAccessException {
        return getCameraCharacteristic(mCameraId, CameraCharacteristics.SENSOR_ORIENTATION);
    }

    /**
     * {@inheritDoc}
     * @param rate
     */
    @Override
    public void setRecorderFrameRate(int rate) throws CameraAccessException {
        if (rate > mCameraConfigIms.getFps()) {
            Log.e(this, "setRecorderFrameRate: Frame rate is greater than max allowed.");
            return;
        }

        mFrameSkippingRate = mCameraConfigIms.getFps() / rate;

        if (shallDefer()) {
            Log.d(this, "setRecorderFrameRate: Deferred.");
            return;
        }

        // Frame skipping applicable only when the recording has started.
        if(!isRecordingStarted()) {
            Log.d(this, "setRecorderFrameRate: Deferred. Recording has not started.");
            return;
        }

        if (mCameraSession != null) {
            updateParams(mCaptureBuilder);
            restartRepeating(build(mCaptureBuilder));
        } else {
            Log.i(this, "setRecorderFrameRate: Deferred.");
        }
    }

    /**
     * Build requests using the target Surfaces that produces desired frame rate
     * for recording and preview sessions.
     * @param builder The builder which must be pre-configured, e.g. zoom, av timer.
     * @return List of CaptureRequest objects.
     * @see {@link #updateParams(Builder)}
     * @see #setRecorderFrameRate(int)
     */
    private List<CaptureRequest> build(CaptureRequest.Builder builder) {
        builder.addTarget(mPreviewSurface);

        if (!isRecordingStarted()) {
            return Arrays.asList(builder.build());
        }

        builder.removeTarget(mRecordingSurface);

        List<CaptureRequest> l = new ArrayList<>();
        for (int i = 1; i < mFrameSkippingRate; ++i) {
            l.add(builder.build());
        }

        builder.addTarget(mRecordingSurface);
        l.add(builder.build());
        return l;
    }


    /**
     * Sets output FPS.
     * @param fps New FPS value.
     * @throws CameraAccessException
     */
    private android.util.Range<Integer> computeBestFps(int v) throws CameraAccessException {
        Log.v(this, "computeBestFps " + v);

        android.util.Range<Integer>[] fpsRange = getCameraCharacteristic(mCameraId,
                CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
        Log.v(this, "computeBestFps: Available FPS Ranges: " + fpsRange);
        if (fpsRange == null || fpsRange.length < 1) {
            throw new CameraAccessException(CameraAccessException.CAMERA_ERROR);
        }

        android.util.Range<Integer> bestFps = fpsRange[0];
        int minDelta = Math.abs(v - bestFps.getUpper());
        for (android.util.Range<Integer> curr : fpsRange) {
            int delta = Math.abs(v - curr.getUpper());
            if (delta < minDelta) {
                bestFps = curr;
                minDelta = delta;
            }
        }

        minDelta = Math.abs(bestFps.getUpper() - bestFps.getLower());
        for (android.util.Range<Integer> curr : fpsRange) {
            if (curr.getUpper() != bestFps.getUpper()) continue;
            int delta = Math.abs(curr.getUpper() - curr.getLower());
            if (delta < minDelta) {
                bestFps = curr;
                minDelta = delta;
            }
        }

        return bestFps;
    }

    private void updateParams(CaptureRequest.Builder builder) {
        builder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);

        if (isSupported(builder, AV_TIMER)) {
            Log.v(this, "Enabling AV timer...");
            builder.set(AV_TIMER, ENABLE_AV_TIMER);
        }

        if (mFps!=null) {
            Log.v(this, "Setting FPS_RANGE=" + mFps);
            builder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, mFps);
        }

        if (mCropRegion != null) {
            Log.v(this, "Setting CROP_REGION=" + mCropRegion);
            builder.set(CaptureRequest.SCALER_CROP_REGION, mCropRegion);
        }

    }

    private void exectutePendingRequests() {
        Log.i(this, "exectutePendingRequests: request=" + mPendingStartRequest);

        try {
            switch (mPendingStartRequest) {
                case CAMERA_REQUEST_START_PREVIEW:
                    doStartPreview();
                    break;
                case CAMERA_REQUEST_START_RECORDING:
                    doStartRecording();
                    break;
            }
        } catch (CameraAccessException e) {
            // TODO Notify
        }
    }

    private <T> T getCameraCharacteristic(String cameraId, Key<T> key)
            throws CameraAccessException {
        CameraCharacteristics characteristics = mCameraManager.getCameraCharacteristics(mCameraId);
        return characteristics.get(key);
    }

    private boolean isSupported(CaptureRequest.Builder builder, CaptureRequest.Key<?> key) {
        // There seems to be an issue 'CameraCharacteristics.getAvailableCaptureRequestKeys()`
        // API - the returned list doesn't have any vendor tag and contains only AOSP tags.
        // This leaves us with below ugly hack to check if a vendor tag is supported or not.
        try {
            builder.get(key);
            return true;
        } catch (IllegalArgumentException e) {
            return false;
        }
    }

    /**
     * {@link CameraDevice.StateListener} is called when {@link CameraDevice} changes its status.
     */
    private CameraDevice.StateCallback mStateListener = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(CameraDevice cameraDevice) {
            Log.i(TAG, "onOpened: CameraDevice is opened, id=" + getId());
            mCameraDevice = cameraDevice;
            if (isClosed()) {
                Log.i(TAG, "onOpened: Camera open completed after close requested.");
                close();
            } else {
                mOpenState = CAMERA_STATE_OPEN;
                exectutePendingRequests();
            }
        }

        @Override
        public void onDisconnected(CameraDevice cameraDevice) {
            Log.i(TAG, "onDisconnected: CameraDevice is disconnected, id=" + getId());
            close();
        }

        @Override
        public void onError(CameraDevice cameraDevice, int error) {
            Log.e(TAG, "onError: CameraDevice is disconnected, id=" + getId() + "error=" + error);
            close();
        }

    };

    private void abortCaptures() {
        Log.i(this, "abortCaptures: Aborting captures..., id=" + getId());

        try {
            if (mCameraSession != null) {
                mCameraSession.abortCaptures();
                mCameraSession.close();
                mCameraSession = null;
            }
        } catch (CameraAccessException e) {
            Log.e(this, "abortCaptures: Failed to abort capture, " + e);
            close();
            // TODO Rethrow
        }
    }

    private void cancelSession() {
        Log.i(this, "cancelSession: Canceling session..., id=" + getId());

        try {
            mSessionId = genSessionId();
            if (mCameraSession != null) {
                mCameraSession.stopRepeating();
                mCameraSession.close();
                mCameraSession = null;
            }
        } catch (CameraAccessException e) {
            Log.e(this, "stopSession: Failed to stop repeating session, " + e);
            close();
            // TODO Rethrow
        }
    }

    private void doStartRepeating(List<CaptureRequest> request) throws CameraAccessException {
        if (request.isEmpty()) {
            throw new Camera.CameraException("Empty CaptureRequest");
        }

        final int size = request.size();
        Log.d(this, "doStartRepeating: Size=" + size);
        if (size == 1) {
            mCameraSession.setRepeatingRequest(request.get(0), null, null);
        } else {
            mCameraSession.setRepeatingBurst(request, null, null);
        }
    }

    private void startRepeating(List<CaptureRequest> request) {
        Log.i(this, "startRepeating: Starting session..., id=" + getId());

        if (mCameraSession == null) {
            Log.e(this, "startRepeating: Session is null.");
            return;
        }

        try {
            Log.v(this, "startRepeating: session=" + mCameraSession);
            doStartRepeating(request);
        } catch (CameraAccessException e) {
            Log.e(this, "startRepeating: Failed to start repeating session, " + e);
            close();
            // TODO Rethrow
        }
    }

    private void restartRepeating(List<CaptureRequest> request) {
        Log.i(this, "restartRepeating: Re-starting session... id=" + getId());

        try {
            if (mCameraSession != null) {
                Log.v(this, "restartRepeating: Stopping repeating session...");
                mCameraSession.stopRepeating();
                Log.v(this, "restartRepeating: Starting repeating session...");
                doStartRepeating(request);
            } else {
                Log.e(this, "restartRepeating: Session is null.");
            }
        } catch (CameraAccessException e) {
            Log.e(this, "restartRepeating: Failed to start restart repeating session Ex="
                     + e);
            close();
            // TODO Rethrow
        }
    }

    private void doStartPreview() throws CameraAccessException {
        Log.i(this, "doStartPreview: surface=" + mPreviewSurface);

        failIfSurfaceNull(mPreviewSurface, "Error preview surface is null");

        try {
            clearStartRequest();
            mCaptureBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

            final List<OutputConfiguration> surfaces = Arrays.asList(new OutputConfiguration(
                    mPreviewSurface));

            mCameraSession = null;
            doCreateCaptureSession(surfaces, new CaptureSessionListener(mSessionId));
        } catch (CameraAccessException e) {
            Log.e(this, "startPreview: Failed to start preview, " + e);
            close();
        }
    }

    private void doStartRecording() throws CameraAccessException {
        Log.i(this, "doStartRecording: Surface=" + mRecordingSurface);

        failIfSurfaceNull(mPreviewSurface, "Error preview surface is null");
        failIfSurfaceNull(mRecordingSurface, "Error recording surface is null");

        try {
            clearStartRequest();
            mCaptureBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);

            final boolean shallComputeRotation = Config.isConfigEnabled(mContext,
                    org.codeaurora.ims.R.bool.config_device_allow_telephony_calculate_rotation);
            int rotation = shallComputeRotation ? Camera2.calculateRotation(
                    mSensorOrientation, mCameraConfigIms, isFacingFront())
                    : OutputConfiguration.ROTATION_0;
            rotation = Camera2.adjustRotationCounterclockwise(rotation);
            Log.i(this, "doStartRecording: shallComputeRotation=" + shallComputeRotation
                    + " Camera2 API rotation=" + rotation);
            OutputConfiguration recordingOutputConfig
                    = new OutputConfiguration(mRecordingSurface, rotation);
            final List<OutputConfiguration> surfaces = Arrays.asList(new OutputConfiguration(
                    mPreviewSurface), recordingOutputConfig);
            mCameraSession = null;
            doCreateCaptureSession(surfaces, new CaptureSessionListener(mSessionId));
        } catch (Exception e) {
            Log.e(this, "startRecording: Failed to start recording, " + e);
            close();
            throw e;
            // TODO: Notify...
        }
    }

    /**
     * Method used to convert rotation from clockwise to
     * counterclockwise direction. This is required as the
     * OutputConfiguration constructor expects the rotation
     * value in the counterclockwise direction.
     *
     * @param rotation Clockwise rotation value.
     * @return Corresponding counterclockwise rotation value.
     */
    static int adjustRotationCounterclockwise(int rotation) {
        switch(rotation) {
            case OutputConfiguration.ROTATION_0:
            case OutputConfiguration.ROTATION_180:
                // No correction required for these rotation values.
                return rotation;
            case OutputConfiguration.ROTATION_90:
                return OutputConfiguration.ROTATION_270;
            case OutputConfiguration.ROTATION_270:
                return OutputConfiguration.ROTATION_90;
            default:
                Log.e(TAG, "adjustRotationCounterclockwise: Invalid rotation value "
                        + rotation);
                return rotation;
        }
    }

    static int calculateRotation(int sensorOrientation,
            ConfigIms cameraConfigIms, boolean isFacingFront) {
        Log.i(TAG, "calculateRotation mSensorOrientation =" + sensorOrientation
                    + " cameraConfigIms=" + cameraConfigIms);
        int orientationMode = cameraConfigIms.getOrientationMode();
        if (orientationMode == ImsMediaConstants.PORTRAIT_MODE) {
            Log.i(TAG, "calculateRotation Portrait mode so rotating by " + sensorOrientation);
            return getOutputConfigurationRotation(sensorOrientation);
        } else if (orientationMode == ImsMediaConstants.DYNAMIC_MODE) {
            return getCvoRotation(sensorOrientation, cameraConfigIms, isFacingFront);
        } else if (orientationMode == ImsMediaConstants.LANDSCAPE_MODE){
            Log.i(TAG, "calculateRotation Landscape mode no rotation");
        }
        return OutputConfiguration.ROTATION_0;
    }

    static int getCvoRotation(int sensorOrientation, ConfigIms cameraConfigIms,
            boolean isFacingFront) {
        if (cameraConfigIms.getWidth() < cameraConfigIms.getHeight()) {
            Log.i(TAG, "calculateRotation CVO mode and w<h so rotating by " + sensorOrientation);
            return getOutputConfigurationRotation(sensorOrientation);
        } else {
            // This compensation is done to compensate for contract between Telephony and IMSVT lib
            // When device held in natural orientation
            // For front camera - camera frame captured top of head should be on the right side
            // For back camera - camera frame captured top of head should be on the left side
            final int rotationFront = (sensorOrientation + 90) % 360;
            final int rotationBack = (sensorOrientation + 270) % 360;
            int rotation = isFacingFront ? rotationFront : rotationBack;
            Log.i(TAG, "calculateRotation CVO mode and w>=h so rotating by " + rotation);
            return getOutputConfigurationRotation(rotation);
        }
    }

    static int getOutputConfigurationRotation(int angle) {
        switch (angle) {
            case 0:
                return OutputConfiguration.ROTATION_0;
            case 90:
                return OutputConfiguration.ROTATION_90;
            case 180:
                return OutputConfiguration.ROTATION_180;
            case 270:
                return OutputConfiguration.ROTATION_270;
            default:
                return OutputConfiguration.ROTATION_0;
        }
    }

    private void clearStartRequest() {
        Log.i(this, "clearStartRequest: request=" + mPendingStartRequest);
        mPendingStartRequest = CAMERA_REQUEST_START_NONE;
    }

    private boolean shallDefer() {
        return mCameraDevice==null || mIsCreateSessionPending;
    }

    private void doCreateCaptureSession(List<OutputConfiguration> o,
            CameraCaptureSession.StateCallback l) throws CameraAccessException {
        if (mIsCreateSessionPending) {
            throw new CameraAccessException(CameraAccessException.CAMERA_ERROR,
                    "Only one session request is allowed.");
        }
        if (mCameraDevice==null) {
            throw new CameraAccessException(CameraAccessException.CAMERA_ERROR,
                    "Camera device is not ready (null).");
        }

        mIsCreateSessionPending = true;
        mCameraDevice.createCaptureSessionByOutputConfigurations(o, l, null);
    }

    private boolean isOpened() {
        return mOpenState == CAMERA_STATE_OPEN;
    }

    private class CaptureSessionListener extends CameraCaptureSession.StateCallback {
        private int id;

        public CaptureSessionListener(int v) {
            id = v;
        }

        @Override
        public void onConfigured(CameraCaptureSession session) {
            Log.i(this, "onConfigured: SessionId=" + id + " GlobalSessionId=" +mSessionId);
            mIsCreateSessionPending = false;
            if (!isRequestPending()) {
                if (id == mSessionId) Camera2.this.onConfigured(session);
            } else {
                exectutePendingRequests();
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            Log.e(this, "onConfigureFailed: SessionId=" + id + " GlobalSessionId=" + mSessionId);
            mIsCreateSessionPending = false;
            if (!isRequestPending()) {
                if (id == mSessionId) Camera2.this.onConfigureFailed(session);
            } else {
                exectutePendingRequests();
            }
        }
    }

    private void onConfigured(CameraCaptureSession session) {
        Log.i(this, "onConfigured: session=" + session);
        mCameraSession = session;

        if (mCaptureBuilder != null) {
            updateParams(mCaptureBuilder);
            startRepeating(build(mCaptureBuilder));
        } else {
            Log.e(this, "onConfigured: Builder is null.");
        }
    }

    private void onConfigureFailed(CameraCaptureSession session) {
        Log.e(this, "onConfigureFailed: onConfigureFailed.");
        close();
    }

    private boolean isRequestPending() {
        return mPendingStartRequest != CAMERA_REQUEST_START_NONE;
    }

    private int genSessionId() {
        if (++mSessionId == Integer.MAX_VALUE) {
            mSessionId = 0;
        }
        Log.d(this, "generateSessionId: SessionId=" + mSessionId);
        return mSessionId;
    }

    private boolean isOpening() {
        return mOpenState == CAMERA_STATE_OPENING;
    }

    private boolean isClosed() {
        return mOpenState == CAMERA_STATE_CLOSED;
    }

    private void failIfClosed() throws CameraAccessException {
        if (!isClosed()) {return;}

        Log.e(this, "failIfClosed: Camera is closed...no operation is allowed.");
        throw new CameraAccessException(CameraAccessException.CAMERA_ERROR);
    }

    private void failIfSurfaceNull(Surface surface, String msg) throws CameraAccessException {
        if (surface!=null) {return;}

        Log.e(this, "failIfSurfaceNull: " + msg);
        throw new CameraAccessException(CameraAccessException.CAMERA_ERROR, msg);
    }
}

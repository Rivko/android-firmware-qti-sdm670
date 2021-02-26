/* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.ims.vt;

import android.content.Context;
import android.hardware.camera2.CameraAccessException;
import android.telecom.VideoProfile.CameraCapabilities;
import android.util.Size;
import android.view.Surface;

import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.utils.Config;
import org.codeaurora.ims.CallModify;
import org.codeaurora.ims.ImsCallSessionImpl;
import org.codeaurora.ims.QtiCallConstants;

/**
 * The class is responsible for managing camera related requests.
 */
public class CameraController implements ImsMedia.CameraListener,
                                         ImsCallSessionImpl.Listener,
                                         Camera.Listener,
                                         ImsMedia.MediaStateListener {
    private static String TAG = "CameraController";
    private static CameraController sInstance;

    private Context mContext;
    private ImsMedia mMedia;
    private Camera mCamera;
    private boolean mIsRecordingEnabled = false;

    // Flag to indicate whether recording has started or not False: recording hasn't started True:
    // recording has started
    private static boolean mIsRecordingStarted = false;

    private Surface mPreviewSurface;
    private Surface mRecordingSurface;
    private Camera.ConfigIms mCameraConfigIms =
            new Camera.ConfigIms(ImsMediaConstants.DEFAULT_HEIGHT, ImsMediaConstants.DEFAULT_WIDTH,
            ImsMediaConstants.DEFAULT_FPS, ImsMediaConstants.PORTRAIT_MODE);
    private int mRecorderFrameRate = mCameraConfigIms.getFps();

    private final Object mSyncObject = new Object();
    private ImsCallSessionImpl mCameraOwner;

    private CameraController(Context context, ImsMedia media) {
        mContext = context;
        mMedia = media;
        mMedia.addCameraListener(this);
        mMedia.addMediaStateListener(this);
        CameraManager.init(context);
    }

    /**
     * Opens the requested camera and closes all other camera.
     *
     * Re-opening camera is relatively expensive (time consuming) operation which results in
     * noticeable delay at UI side. To minimize this delay we don't re-open (close/open)
     * the camera again if it is already opened by another session. We simply adjust the camera
     * owner, that is silently remove the old owner (without closing the camera) and add a
     * new owner.
     *
     * @param cameraId Camera to open.
     * @param session Session which requests to open the camera.
     */
    private void doOpen(String cameraId, ImsCallSessionImpl session)
            throws CameraAccessException {
        Log.i(this, "doOpen mCamera =" + mCamera + " cameraId=" + cameraId +
                " Session=" + getSessionInfo(session));

        releaseCurrentOwner();
        if (mCamera != null && !mCamera.getId().equals(cameraId)) {
            closeCamera();
        }

        final boolean reOpen = mCamera == null;
        if (reOpen) {
            mCamera = CameraManager.open(cameraId, this);
        }

        setOwner(session);

        if (reOpen && mCameraConfigIms != null) {
            mCamera.reconfigure(mCameraConfigIms);
            mCamera.setRecorderFrameRate(mRecorderFrameRate);
        }

        final int mountAngle = mCamera.getSensorOrientation();
        final int cameraFacing = mCamera.isFacingFront() ?
                ImsMediaConstants.CAMERA_FACING_FRONT : ImsMediaConstants.CAMERA_FACING_BACK;
        mMedia.setCameraInfo(cameraFacing, mountAngle);

    }

    /**
     * closes all open cameras.
     */
    private void closeCamera() {
        if (mCamera != null) {
            mCamera.close();
        }
        mCamera = null;
    }

    /**
     * Initializes CameraController singleton. This must be called once.
     * @param context Application context to use.
     * @return The instance of CameraController.
     */
    public static synchronized CameraController init(Context context, ImsMedia media) {
        if (sInstance == null) {
            sInstance = new CameraController(context, media);
        } else {
            throw new RuntimeException("CameraController: Multiple initialization");
        }
        return sInstance;
    }

    /**
     * Provides access to CameraController.
     * @return The instance of CameraController.
     * @see CameraController#init
     */
    public static synchronized CameraController getInstance() {
        if (sInstance != null) {
            return sInstance;
        } else {
            throw new RuntimeException("CameraController: Not initialized");
        }
    }

    /**
     * Opens the requested camera and closes all other camera. if the requested camera is already
     * open the request will be ignored.
     * @param cameraId Camera to open.
     * @param session IMS session which requests access to camera.
     * TODO Check if same camera ids can be used for both ImsCamera and Camera2.
     */
    public void open(String cameraId, ImsCallSessionImpl session) {
        Log.i(this, "open: cameraId=" + cameraId + " session=" + getSessionInfo(session));
        if (cameraId == null) {
            Log.e(this, "open: Invalid camera id, " + cameraId);
            throw new RuntimeException("CameraController: Invalid camera id.");
        }

        try {
            doOpen(cameraId, session);
        } catch (CameraAccessException e) {
            Log.e(this, "open: Failed to open, cameraId=" + cameraId + " Exception=" + e);
            sendCameraStatus(session, e.getReason());
        }
    }

    /**
     * Removes the session from the list owners and closes the camera if the list is empty.
     * @param session IMS session to be removed from the list.
     */
    public void close(ImsCallSessionImpl session) {
        Log.i(this, "close");
        doClose(session);
    }


    /**
     * Sets the preview surface and updates (starts/stops) camera session (preview/recording)
     *
     * @param surface Preview surface.
     */
    public void setPreviewSurface(Surface surface, ImsCallSessionImpl session) {
        synchronized (mSyncObject) {
            Log.i(this, "setPreviewSurface, surface=" + surface +
                        " Session=" + getSessionInfo(session));

            if (!isOwner(session)) {
                Log.i(this, "setPreviewSurface: " + getSessionInfo(session) +
                 " is not a camera owner. Camera owner=" + getSessionInfo(mCameraOwner));
                return;
            }

            try {
                if (surface != null && mPreviewSurface != null
                        && !ImsMedia.haveSameParent(mPreviewSurface, surface)) {
                    Log.i(this, "setPreviewSurface: Restarting camera preview...");
                    mPreviewSurface = null;
                    updateCameraSession();
                }
                mPreviewSurface = surface;
                updateCameraSession();
            } catch (CameraAccessException e) {
                sendCameraStatus(e.getReason());
            }
        }
    }

    public void setZoom(float value) {
        if (mCamera == null || !mCamera.isOpen()) {
            Log.w(this, "setZoom: Camera is not in correct state, camera=" + mCamera);
            return;
        }

        try {
            // Zoom index scale ranges from 0 - 100 in the UI.
            // We pass the zoom value based on that scale and
            // convert it to the correct zoom value based on the
            // actual camera main/max zoom. This is due to a
            // limitation as some of the API's getMaxZoom is marked as @hide
            final float maxZoom = mCamera.getMaxZoom();
            final float minZoom = mCamera.getMinZoom();
            Log.v(this, "setZoom: maxZoom=" + maxZoom + " minZoom=" + minZoom + " value=" + value);

            mCamera.setZoom( minZoom +
                    (value * (maxZoom - minZoom)) / QtiCallConstants.CAMERA_MAX_ZOOM);
        } catch (CameraAccessException e) {
            Log.v(this, "Failed to change zoom, value=" + value + "Exception=" + e);
        }
    }

    CameraCapabilities getCameraCapabilities() {
        Log.v(this, "getCameraCapabilities mCamera=" + mCamera);
        if (mCamera != null) {
            try {
                final Size size = mCamera.getPreviewSize();
                final Boolean isZoomSupported = mCamera.isZoomSupported();
                final float maxZoom = mCamera.getMaxZoom();
                Log.i(this, "getCameraCapabilities: PreviewSize=" + size + " isZoomSupported="
                        + isZoomSupported + " maxZoom=" + maxZoom);
                return new CameraCapabilities(size.getWidth(), size.getHeight(),
                        isZoomSupported, maxZoom);
            } catch (CameraAccessException e) {
                sendCameraStatus(e.getReason());
                return null;
            }
        } else {
            return null;
        }
    }

    /**
     * Sets a session as a camera owner and subscribes for session events.
     * @param session IMS session to be added to the list.
     */
    private void setOwner(ImsCallSessionImpl session) {
        synchronized (mSyncObject) {
            Log.i(this, "setOwner: Session=" + getSessionInfo(session));
            if (!isOwner(session)) {
                releaseCurrentOwner();
                mCameraOwner = session;
                mCameraOwner.addListener(this);
            } else {
                Log.i(this, "setOwner: The session is already registered as camera owner, session="
                   + session);
            }
        }
    }

    private void removeOwner(ImsCallSessionImpl session) {
        synchronized (mSyncObject) {
            Log.i(this, "removeOwner: Session=" + getSessionInfo(session));
            if (isOwner(session)) {
                releaseCurrentOwner();
            } else {
                Log.w(this, "removeOwner: The session is not the correct owner. cameraOwner="
                 + getSessionInfo(session));
            }
        }
    }

    private boolean isOwner(ImsCallSessionImpl session) {
        synchronized (mSyncObject) {
            return session != null && session == mCameraOwner;
        }
    }

    private void releaseCurrentOwner() {
        synchronized (mSyncObject) {
            Log.i(this, "releaseCurrentOwner: Session=" + getSessionInfo(mCameraOwner));
            if (mCameraOwner != null) {
                mCameraOwner.removeListener(this);
                mCameraOwner = null;
            }
        }
    }


    private void doClose(ImsCallSessionImpl session) {
        Log.i(this, "doClose: Camera=" + mCamera + " Session=" + getSessionInfo(session));
        synchronized (mSyncObject) {
            removeOwner(session);

            if (mCameraOwner == null) {
                closeCamera();
                mPreviewSurface = null;
            } else {
                Log.w(this, "doClose: Not closing camera=" + mCamera +
                        " another session still requires it, session="
                        + getSessionInfo(mCameraOwner));
            }
        }
    }

    /**
     * startPreview does setPreviewSurface, Starting Preview and recording
     * @param surface
     */
    private void updateCameraSession() throws CameraAccessException {
        Log.i(this, "updateCameraSession");
        if (mCamera == null || !mCamera.isOpen()) {
            Log.e(this, "updateCameraSession: Camera is not in correct state, camera=" + mCamera);
            return;
        }

        final boolean isPreviewStarted = mCamera.isPreviewStarted();
        final boolean isRecordingStarted = mCamera.isRecordingStarted();
        final boolean canStartPreview = mPreviewSurface != null;
        final boolean canStartRecording = mIsRecordingEnabled &&
                        canStartPreview;

        Log.v(this, "updateCameraSession mPreviewSurface=" + mPreviewSurface +
             " mIsRecordingEnabled=" + mIsRecordingEnabled + " mRecordingSurface="
              + mRecordingSurface);
        Log.v(this, "updateCameraSession canStartRecording=" + canStartRecording +
             " isRecordingStarted=" + isRecordingStarted);

        if (canStartRecording && !isRecordingStarted) {
            mCamera.startRecording(mPreviewSurface, mRecordingSurface);
        } else if (canStartPreview && !isPreviewStarted) {
            mCamera.startPreview(mPreviewSurface);
        } else if (!canStartPreview && isPreviewStarted) {
            mCamera.stopPreview();
        }
    }


    public void onRecordingEnabled() {
        Log.i(this, "onRecordingEnabled");
        try {
            mIsRecordingEnabled = true;
            updateCameraSession();
        } catch(CameraAccessException e) {
            sendCameraStatus(e.getReason());
        }
    }

    public void onRecordingDisabled() {
        Log.i(this, "onStopReadyEvent");
        mIsRecordingEnabled = false;
    }

    @Override
    public void onMediaDeinitialized() {
        Log.i(this, "onMediaDeinitialized. closing Camera");
        if (mCameraOwner != null) {
            close(mCameraOwner);
        }
        // In general, calling close() is enough. However, it has some internal checks which
        // might prevent closing of camera in some cases, from the other side closeCamera() has
        // no checks, so it is guaranteed that the camera will be closed.
        closeCamera();
        mIsRecordingEnabled = false;
        mRecordingSurface = null;
        mPreviewSurface = null;
    }

    @Override
    public void onMediaInitialized() {
        // no-op
    }

    /**
     * Re-configures camera.
     * Note that we also overwrite the recorder frame rate.
     */
    public void onCameraConfigChanged(int w, int h, int fps, Surface surface, int orientationMode) {
        mCameraConfigIms = new Camera.ConfigIms(w, h, fps, orientationMode);
        mRecorderFrameRate = fps;
        mRecordingSurface = surface;
        Log.i(this, "onCameraParamsReady, cameraConfig=" + mCameraConfigIms);

        if ( mCamera == null || !mCamera.isOpen()) {
            Log.i(this, "onParamReadyEvent: Camera is not open deferring configuration...");
            return;
        }

        try {
            if (mCamera.isPreviewStarted()) {
                mCamera.stopPreview();
            }
            mCamera.reconfigure(mCameraConfigIms);
            onUpdateRecorderFrameRate(mRecorderFrameRate);
            updateCameraSession();
            sendCameraCapabilities();
        } catch (CameraAccessException e) {
            sendCameraStatus(e.getReason());
        }
    }

    @Override
    public void onUpdateRecorderFrameRate(int rate) {
        Log.i(this, "onUpdateRecorderFrameRate, rate=" + rate);
        mRecorderFrameRate = rate;
        if (mCamera == null || !mCamera.isOpen()) {
            Log.i(this, "onUpdateRecorderFrameRate: Camera is not open deferring...");
            return;
        }

        try {
            mCamera.setRecorderFrameRate(rate);
        } catch (CameraAccessException e) {
            sendCameraStatus(e.getReason());
        }
    }

    private void sendCameraCapabilities() {
        CameraCapabilities cc = getCameraCapabilities();
        if (cc == null || mCameraOwner == null) {
            Log.e(this, "sendCameraCapabilities: Camera capabilities or camera owner is null");
            return;
        }

        final ImsVideoCallProviderImpl provider = mCameraOwner.getImsVideoCallProviderImpl();
        if (provider == null) {
            Log.e(this, "sendCameraCapabilities: ImsVideoCallProvider is not available (null).");
            return;
        }

        Log.i(this, "sendCameraCapabilities: New capabilities= " + cc);
        provider.changeCameraCapabilities(cc);
    }

    private String getSessionInfo(ImsCallSessionImpl session) {
        if (session == null) {
            return null;
        }
        return session.toSimpleString();
    }

    private void sendCameraStatus(ImsCallSessionImpl session, int error) {
        if (session == null) {
            Log.e(this, "sendCameraStatus: session is null. CameraStatus=" + error);
            return;
        }

        final ImsVideoCallProviderImpl provider = session.getImsVideoCallProviderImpl();
        if (provider == null) {
            Log.e(this, "sendCameraStatus: ImsVideoCallProvider is not available (null).");
            return;
        }

        Log.v(this, "sendCameraStatus: Notifying Session=" + session.getCallId());
        provider.sendCameraStatus(true);
    }

    private void sendCameraStatus(int error) {
        synchronized (mSyncObject) {
            if (mCameraOwner != null) {
                sendCameraStatus(mCameraOwner, error);
            }
        }
    }

    @Override
    public void onError(Camera camera, int error) {
        Log.e(this, "CameraFailed: cameraId=" + camera.getId() + " Error=" + error);
        sendCameraStatus(error);
    }

    @Override
    public void onDisconnected(ImsCallSessionImpl session) {
    }

    @Override
    public void onClosed(ImsCallSessionImpl session) {
        Log.i(this, "onClosed: Session=" + getSessionInfo(session));
        close(session);
    }

    @Override
    public void onUnsolCallModify(ImsCallSessionImpl session, CallModify callModify) {
    }
}

/* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.vt;

import android.content.Context;
import android.hardware.camera2.CameraAccessException;
import android.util.Size;
import android.util.Log;
import android.view.Surface;

/**
 * Base class used to support different camera implementations.
 * The class is used to hide implementation details of ImsCamera which is based on {@code camera1}
 * APIs and {@code Camera2} which is based on Camera2 APIs.
 */
public abstract class Camera {
    private static final String TAG = "VideoCall_Camera";
    Listener mListener;

    protected Context mContext;
    protected String mCameraId;

    public Camera(Context context, String id, Listener listener) {
        mContext = context;
        mCameraId = id;
        mListener = listener;
    }

    public interface Listener {
        /**
         * Notifies about camera failures.
         * @param camera Camera where the failure detected.
         * @param error Error code of the failures.
         */
        void onError(Camera camera, int error);
    }

    /*package*/
    static class ConfigIms {
        public ConfigIms(int w, int h, int f, int mode) {
            size = new Size(w, h);
            fps = f;
            orientationMode = mode;
        }

        /**
         * Constructs the parameters (@see ConfigIms) to configure camera - width, height, fps
         * @param size Dimensions of camera frames (width and height). Can't be null.
         * @param fps Rate of camera frames (frames per second)
         * @throw NullPointerException if size is null.
         */
        public ConfigIms(Size s, int f, int mode) {
            if (s == null) {
                throw new NullPointerException("Size passed to ConfigIms is null.");
            }
            size = s;
            fps = f;
            orientationMode = mode;
        }

        Size size;
        int fps;
        int orientationMode;

        @Override
        public String toString() {
            return "Camera.ConfigIms(width=" + getWidth() + ", height=" + getHeight() + ", fps="
                    + getFps() + ", orientationMode=" + orientationMode + ")";
        }

        public Size getSize() {
            return size;
        }

        public int getWidth() {
            return size.getWidth();
        }

        public int getHeight() {
            return size.getHeight();
        }

        public int getFps() {
            return fps;
        }

        public int getOrientationMode() {
            return orientationMode;
        }
    }

    static public class CameraException extends CameraAccessException {
        public static final int CAMERA_ERROR = CameraAccessException.CAMERA_ERROR;

        public CameraException(int problem) {
            super(problem);
        }

        public CameraException(int problem, String message) {
            super(problem, message);
        }

        public CameraException(String message) {
            this(CAMERA_ERROR, null);
        }
    }

    /**
     * Retrieves the preview size. The preview size is used to configure the preview surface.
     * @throws CameraAccessException
     */
    public abstract Size getPreviewSize() throws CameraAccessException;

    /**
     * Opens the camera.
     * @throws CameraAccessException
     */
    public abstract void open() throws CameraAccessException;

    /**
     * Closes the camera.
     */
    public abstract void close();

    /**
     * Sets camera zoom.
     * @param v Camera zoom value.
     * @throws CameraAccessException
     */
    public abstract void setZoom(float v) throws CameraAccessException;

    /**
     * Reconfigures camera with negotiated with peer configuration.
     * Note: Preview and Recording must be stopped prior to reconfiguring the camera.
     * @param cfg Negotated configuration.
     * @throws CameraAccessException
     */
    public abstract void reconfigure(ConfigIms cfg) throws CameraAccessException;

    /**
     * Checks if zoom supported.
     * @return true if zoom supported, false otherwise
     * @throws CameraAccessException
     */
    public abstract boolean isZoomSupported() throws CameraAccessException;

    /**
     * Check which camera is open
     * @return true if camera is facing front
     */
    public abstract boolean isFacingFront() throws CameraAccessException;

    /**
     * Gets maximum supported zoom.
     * @return Maximum supported zoom.
     * @throws CameraAccessException
     */
    public abstract float getMaxZoom() throws CameraAccessException;

    /**
     * Gets minimum supported zoom.
     * @return Minimum supported zoom.
     * @throws CameraAccessException
     */
    public abstract float getMinZoom() throws CameraAccessException;

    /**
     * Starts camera preview.
     * @param surface Preview surface.
     * @throws CameraAccessException
     */
    public abstract void startPreview(Surface surface) throws CameraAccessException;

    /**
     * Stops camera preview and recording.
     * @throws CameraAccessException
     */
    public abstract void stopPreview()  throws CameraAccessException;

    /**
     * Starts camera recording and preview.
     * @param previewSurface Preview surface.
     * @param recordingSurface Recording surface.
     * @throws CameraAccessException
     */
    public abstract void startRecording(Surface previewSurface, Surface recordingSurface)
            throws CameraAccessException;

    /**
     * Stops camera recording.
     * @throws CameraAccessException
     */
    public abstract void stopRecording() throws CameraAccessException;

    /**
     * @return true if the camera is open, false otherwise.
     */
    public abstract boolean isOpen();

    /**
     * @return true if preview started, false otherwise.
     */
    public abstract boolean isPreviewStarted();

    /**
     * @return true if recording started, false otherwise.
     */
    public abstract boolean isRecordingStarted();

    /**
     * @return The sensor orientation (mount angle) value.
     * @throws CameraAccessException
     */
    public abstract int getSensorOrientation() throws CameraAccessException;

    /**
     * @return ID of the camera.
     */
    public String getId() {
        return mCameraId;
    }

    /**
     * @return Re-configures camera's recording stream to produce frames with given rate.
     * @param rate New rate to use for recording session.
     * @see #reconfigure(ConfigIms)
     * @throws CameraAccessException
     */
    public abstract void setRecorderFrameRate(int rate) throws CameraAccessException;

    @Override
    public String toString() {
        return super.toString() + " - CameraId=" + getId();
    }

    /**
     * Notify listeners about an error.
     * @param error
     */
    protected void notifyOnError(int error) {
        if (mListener!=null) {
            mListener.onError(this, error);
        }
    }

    private static void log(String msg) {
        Log.d(TAG, msg);
    }

    private static void loge(String msg) {
        Log.e(TAG, msg);
    }

    protected static void logNotSupported(String fn) {
        Log.d(TAG, fn + ": Not supported");
    }
}

/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.device;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.hardware.camera2.CameraManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.View;

import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.ViewSize;
import com.qualcomm.qti.sta.engine.JniManager.FrameSession;
import com.qualcomm.qti.sta.mainui.ImageCapturer;
import com.qualcomm.qti.sta.mainui.CameraActivity.CaptureType;
import com.qualcomm.qti.sta.mainui.preview.AutoPreviewCapturer;
import com.qualcomm.qti.sta.mainui.preview.ButtonPreviewCapturer;

/**
 * Class to handle the camera
 */
public abstract class CameraHandler {
    private static final String LOG_TAG = "CameraHandler";

    protected static final int MIN_WIDTH = 1920;
    protected static final int MIN_HEIGHT = 1080;

    /**
     * Interface for camera notifications
     */
    public interface CameraListener {
        /**
         * Notification on snapshot taken
         *
         * @param snapshot
         *            snapshot as a bitmap
         */
        public void onSnapshot(Bitmap snapshot);

        /**
         * Notification when frame becomes available
         *
         * @param data
         *            frame bytes
         * @param width
         *            width of the frame
         * @param height
         *            height of the frame
         * @return PreviewSession current preview session
         */
        public FrameSession onFrameAvailable(byte[] data, int width, int height);

        /**
         * Notification on Frame session started
         */
        public void onFrameSessionStarted();

        /**
         * Notification on Frame session cancelled
         */
        public void onFrameSessionCancelled();
    }

    private ImageCapturer imageCapturer = null;

    protected int viewRotation;
    protected ViewSize pictureSize;
    protected ViewSize previewSize;

    protected final Context context;
    protected final CameraListener cameraListener;

    protected CoordinateSpace pictureSpace;
    protected CoordinateSpace previewSpace;

    protected ViewSize surfaceSize = null;
    protected float surfaceViewRatio = 1;

    protected boolean flashLightOn = false;
    protected boolean continuousFocusSupported = false;

    /**
     * Constructor for the camera handler class
     *
     * @param context
     *            application context
     * @param listener
     *            camera event listener
     */
    public CameraHandler(Context context, CameraListener listener) {
        this.context = context;
        this.cameraListener = listener;

        pictureSpace = CoordinateSpace.EMPTY;
        previewSpace = CoordinateSpace.EMPTY;
    }

    /**
     * gets the display orientation for the camera
     *
     * @return display orientation angle
     */
    public int getDisplayOrientation() {
        return viewRotation;
    }

    /**
     * picks the preview size
     *
     * @param display
     *            activity display
     * @return preview size
     */
    protected ViewSize pickPreviewSize(Display display) {
        DisplayMetrics metrics = new DisplayMetrics();
        display.getMetrics(metrics);

        // Allowed picture aspect ratios fall within +/- 10% of image aspect ratio
        float aspectRatioMin = 0;
        float aspectRatioMax = 0;
        if (viewRotation == 90 || viewRotation == 270) {
            aspectRatioMin = (float) metrics.heightPixels / (float) metrics.widthPixels * 0.9f;
            aspectRatioMax = (float) metrics.heightPixels / (float) metrics.widthPixels * 1.1f;
        } else {
            aspectRatioMin = (float) metrics.widthPixels / (float) metrics.heightPixels * 0.9f;
            aspectRatioMax = (float) metrics.widthPixels / (float) metrics.heightPixels * 1.1f;
        }

        return getMatchingPreviewSize(aspectRatioMin, aspectRatioMax);
    }

    /**
     * method to update the coordinate spaces
     */
    protected void updateCoordinateSpaces() {
        if (viewRotation == 90 || viewRotation == 270) {
            previewSpace = new CoordinateSpace(previewSize.height, previewSize.width,
                    Orientation.ORIENTATION_0, false);
            pictureSpace = new CoordinateSpace(pictureSize.height, pictureSize.width,
                    Orientation.ORIENTATION_0, false);

        } else {
            previewSpace = new CoordinateSpace(previewSize.width, previewSize.height,
                    Orientation.ORIENTATION_90, false);
            pictureSpace = new CoordinateSpace(pictureSize.width, pictureSize.height,
                    Orientation.ORIENTATION_90, false);
        }
    }

    /**
     * picks the picture size
     *
     * @return picture size
     */
    protected ViewSize pickPictureSize() {
        // Allowed picture aspect ratios fall within +/- 1% of preview aspect ratio
        final float aspectRatioMin = (float) previewSize.width / (float) previewSize.height * 0.99f;
        final float aspectRatioMax = (float) previewSize.width / (float) previewSize.height * 1.01f;
        return getMatchingPictureSize(aspectRatioMin, aspectRatioMax);
    }

    /**
     * gets the surface size
     *
     * @return surface size
     */
    public ViewSize getSurfaceSize() {
        return surfaceSize;
    }

    /**
     * gets the surface view ratio
     *
     * @return surface view ratio
     */
    public float getSurfaceViewRatio() {
        return surfaceViewRatio;
    }

    /**
     * clean up method for camera actor
     */
    protected void clear() {
        if (imageCapturer != null) {
            imageCapturer.wrapUp();
            imageCapturer = null;
        }

        pictureSpace = CoordinateSpace.EMPTY;
        previewSpace = CoordinateSpace.EMPTY;
    }

    /**
     * Start capturing snapshot or preview frames based on mode type
     *
     * @param captureType
     *            capture type
     * @return true if started looking into, false otherwise
     */
    public boolean startCapture(CaptureType captureType) {
        if (imageCapturer != null && !imageCapturer.isType(captureType)) {
            imageCapturer.wrapUp();
            imageCapturer = null;
        }
        if (imageCapturer == null) {
            imageCapturer = ImageCapturer.getInstance(captureType, context, cameraListener);
        }
        return imageCapturer.start(this);
    }

    /**
     * Stop looking for snap
     */
    public void stopCapturing() {
        if (imageCapturer != null) {
            imageCapturer.stop();
        }
        removeCaptureCallbacks();
    }

    /**
     * Start sending the preview frames
     */
    public void startSendingFrames() {
        if (imageCapturer != null) {
            if (imageCapturer instanceof AutoPreviewCapturer
                    || imageCapturer instanceof ButtonPreviewCapturer) {
                startFrameCycle();
            }
        }
    }

    /**
     * Gets the capture status
     *
     * @return capture status
     */
    public boolean isCaptureOn() {
        return (imageCapturer != null && imageCapturer.isCaptureOn()) ? true : false;
    }

    /**
     * Get the picture space {@link CoordinateSpace}
     *
     * @return the picture frame {@link CoordinateSpace}
     * @throws RuntimeException
     *             if {@link CameraManager} is not {@link #open()}ed
     */
    public CoordinateSpace getPictureFrameSpace() {
        return pictureSpace;
    }

    /**
     * Get the preview space {@link CoordinateSpace}
     *
     * @return the picture frame {@link CoordinateSpace}
     * @throws RuntimeException
     *             if {@link CameraManager} is not {@link #open()}ed
     */
    public CoordinateSpace getPreviewFrameSpace() {
        return previewSpace;
    }

    /**
     * Method called on roi changed
     */
    public void roiChanged() {
        if (imageCapturer != null) {
            imageCapturer.onRoiChanged(this);
        }
    }

    /**
     * gets the exif rotated bitmap
     *
     * @param data
     *            image data
     * @return rotated bitmap if there is an angle specified in exif
     */
    protected Bitmap getExifRotatedBitmap(byte[] data) {
        Bitmap snapshot = null;
        try {
            String savedFile = Utils.getLocalImageFile(context);
            saveJpgImage(context, data, savedFile);
            int angle = Utils.getExifRotation(savedFile);
            Log.d(LOG_TAG, "Jpg orientation: " + angle);
            snapshot = BitmapFactory.decodeByteArray(data, 0, data.length);
            if (angle != 0) {
                Matrix matrix = new Matrix();
                matrix.postRotate(angle);
                snapshot = Bitmap.createBitmap(snapshot, 0, 0, snapshot.getWidth(),
                        snapshot.getHeight(), matrix, true);
            }
        } catch (IOException e) {
            Log.d(LOG_TAG, "getting exif information fetched: " + e.getMessage());
            e.printStackTrace();
        }
        return snapshot;
    }

    /**
     * Saves the jpg image
     *
     * @param context
     *            application context
     * @param data
     *            image data
     * @param jpgFile
     *            jpg file to be stored
     * @throws IOException
     *             file i/o errors
     */
    protected void saveJpgImage(Context context, byte[] data, String jpgFile) throws IOException {
        File file = new File(jpgFile);
        if (file.exists()) {
            file.delete();
        }
        FileOutputStream outputStream = new FileOutputStream(jpgFile);
        outputStream.write(data);
        outputStream.close();
    }

    /**
     * method to stop the on going activity in the camera
     */
    public abstract void removeCaptureCallbacks();

    /**
     * method to start the sending frame cycle
     */
    public abstract void startFrameCycle();

    /**
     * Gets the camera initialized state
     *
     * @return true if initialized false otherwise
     */
    public abstract boolean isCameraInitialized();

    /**
     * Initializes the camera
     *
     * @param display
     *            window display
     * @return true if initialized, false otherwise
     */
    public abstract boolean initialize(Display display);

    /**
     * gets the view associated with camera
     *
     * @return view
     */
    public abstract View getView();

    /**
     * Wrap up the camera object
     */
    public void wrapUp() {
        if (imageCapturer != null) {
            imageCapturer.stop();
        }
    }

    /**
     * Toggles the camera flash
     */
    public abstract boolean toggleFlash();

    /**
     * toggles the macro focus mode
     */
    public abstract void toggleMacroFocusMode();

    /**
     * Checks if the flash is on for the camera or not
     *
     * @return true if flash is on, false otherwise
     */
    public boolean isFlashOn() {
        return flashLightOn;
    }

    /**
     * Checks if the current focus mode is macro
     *
     * @return true if the mode is set to macro, false otherwise
     */
    public abstract boolean isMacroFocusMode();

    /**
     * gets the most matching preview size
     *
     * @param aspectRatioMin
     *            minimum aspect ration
     * @param aspectRatioMax
     *            maximum aspect ratio
     * @return matching preview size
     */
    public abstract ViewSize getMatchingPreviewSize(float aspectRatioMin, float aspectRatioMax);

    /**
     * gets the most matching picture size
     *
     * @param aspectRatioMin
     *            minimum aspect ration
     * @param aspectRatioMax
     *            maximum aspect ratio
     * @return matching picture size
     */
    public abstract ViewSize getMatchingPictureSize(float aspectRatioMin, float aspectRatioMax);

    /**
     * Method to start taking snapshot
     */
    public abstract void startSnapshot();
}

/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.hardware;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.util.Log;

import com.qualcomm.qti.panoramaui.CameraPreviewView;
import com.qualcomm.qti.panoramaui.data.AppError;
import com.qualcomm.qti.panoramaui.data.CaptureDirection;
import com.qualcomm.qti.panoramaui.data.CaptureOrientation;

/**
 * This class provides high-level Panorama capture functionality.
 */
public class PanoramaActor {
    /**
     * Classes may implement this interface to receive notifications about the status of capturing
     * a panorama. All methods will be invoked on the application's main thread.
     */
    public static interface EventListener {
        /**
         * Invoked when Panorama capture is started. It will be the first {@link EventListener}
         * method invoked.
         */
        void onCaptureStarted();

        /**
         * Invoked when the Panorama engine has determined in which orientation the Panorama capture
         * is running. It will be invoked exactly once for each Panorama capture session, before
         * {@link #onDirectionEstablished(CaptureDirection)}. For some implementations, it may be
         * invoked substantially prior to the direction being established; in others, it may be
         * invoked immediately prior.
         * @param orientation a {@link CaptureOrientation} value, indicating the orientation of
         *      Panorama capture
         */
        void onOrientationEstablished(CaptureOrientation orientation);

        /**
         * Invoked when the Panorama engine has determined in which direction the Panorama capture
         * is running. It will be invoked exactly once for each Panorama capture session, but may
         * not be invoked right away; the Panorama engine may need to consume several key frames to
         * determine this. It will always occur after {@link
         * #onOrientationEstablished(CaptureOrientation)}.
         * @param direction a {@link CaptureDirection} value, indicating the direction of Panorama
         *      capture. If the reported {@link CaptureOrientation} was {@link
         *      CaptureOrientation#HORIZONTAL}, direction will be either {@link
         *      CaptureDirection#LEFT} or {@link CaptureDirection#RIGHT}. If the reported {@link
         *      CaptureOrientation} was {@link CaptureOrientation#VERTICAL}, direction will be
         *      either {@link CaptureDirection#UP} or {@link CaptureDirection#DOWN}.
         */
        void onDirectionEstablished(CaptureDirection direction);

        /**
         * Invoked periodically with the instantaneous speed of Panorama capture
         * @param speed the instantaneous speed value
         */
        void onSpeedUpdate(float speed);

        /**
         * Invoked whenever the Panorama engine selects a frame as a key frame
         * @param bitmap a thumbnail {@link Bitmap} of the key frame
         * @param positionX a int indicate the tracking position of current frame
         * @param positionY a int indicate the tracking position of current frame
         */
        void onKeyFrame(Bitmap bitmap, int positionX, int positionY);

        /**
         * Invoked if the Panorama capture is cancelled (via
         * {@link PanoramaEngine#cancelCapture()}). This method is mutually exclusive with
         * {@link #onCaptureCompleted(ByteBuffer)}; only one callback method will be invoked for a
         * given capture session.
         */
        void onCaptureCancelled();

        /**
         * Invoked if the Panorama capture is completed (via
         * {@link PanoramaEngine#completeCapture()}). This method is mutually exclusive with
         * {@link #onCaptureCancelled()}; only one callback method will be invoked for a
         * given capture session.
         * @param jpegData a {@link ByteBuffer} containing the JPEG-compressed Panorama image. This
         *      buffer may or may not contain EXIF data.
         */
        void onCaptureCompleted(ByteBuffer jpegData);

        /**
         * Invoked if the Panorama engine encounters an error. After an error is generated, one of
         * {@link #onCaptureCancelled()} or {@link #onCaptureCompleted(ByteBuffer)} will be invoked,
         * depending on whether the error is recoverable or not.
         * @param error an {@link AppError} representing the error
         */
        void onError(AppError error);
    }

    private enum State { UNINITIALIZED, INITIALIZED, CAPTURING }

    private static final String TAG = "PanoramaActor";

    private final PanoramaEngine panoramaEngine;
    private final CameraActor cameraActor;

    private State state;

    public PanoramaActor(CameraActor cameraActor, SensorActor sensorActor, CameraPreviewView view) {
        if (cameraActor == null) {
            throw new IllegalArgumentException("cameraActor cannot be null");
        } else if (sensorActor == null) {
            throw new IllegalArgumentException("sensorActor cannot be null");
        } else if (view == null) {
            throw new IllegalArgumentException("view cannot be null");
        }

        this.cameraActor = cameraActor;
        panoramaEngine = new StandalonePanoramaEngine(cameraActor, sensorActor, view);

        state = State.UNINITIALIZED;
    }

    public void init() {
        if (state != State.UNINITIALIZED) {
            throw new RuntimeException("PanoramaActor is already initialized");
        }

        // TODO: make sure CameraActor is active

        panoramaEngine.init();
        state = State.INITIALIZED;
    }

    public void release() {
        if (state == State.UNINITIALIZED) {
           Log.w(TAG, "PanoramaActor is not initialized");
           return;
        }

        if (state == State.CAPTURING) {
            panoramaEngine.cancelCapture();
        }
        panoramaEngine.release();
        state = State.UNINITIALIZED;
        Log.d(TAG, "Released");
    }

    public void startCapture(EventListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener cannot be null");
        } else if (state == State.UNINITIALIZED) {
            throw new RuntimeException("PanoramaActor is not initialized");
        } else if (state == State.CAPTURING) {
            Log.w(TAG, "PanoramaActor is already capturing");
            return;
        }

        panoramaEngine.startCapture(listener);
        state = State.CAPTURING;
    }

    public void stopCapture() {
        stopCapture(false);
    }

    public void stopCapture(boolean cancel) {
        if (state == State.UNINITIALIZED) {
            throw new RuntimeException("PanoramaActor is not initialized");
        } else if (state != State.CAPTURING) {
            Log.w(TAG, "PanoramaActor is not capturing");
            return;
        }

        if (cancel) {
            panoramaEngine.cancelCapture();
        } else {
            panoramaEngine.completeCapture();
        }
        state = State.INITIALIZED;
    }
}

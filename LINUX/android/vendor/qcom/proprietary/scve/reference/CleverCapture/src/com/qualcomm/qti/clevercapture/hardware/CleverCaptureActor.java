/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercapture.hardware;

import java.util.Vector;

import android.graphics.RectF;
import android.util.Log;

import com.qualcomm.qti.clevercapture.data.AppError;
import com.qualcomm.qti.clevercaptureui.CameraPreviewView;
import com.qualcomm.qti.clevercaptureui.utils.LogFileDump;

public class CleverCaptureActor {
    public static interface EventListener {
        void onCaptureStarted();
        void onPicture(int frameId, int reserved);
        void onCaptureCompleted();
        void onCaptureCancelled();
        void onError(AppError error);

        void onHighMotion(boolean motion);
    }

    public enum State { UNINITIALIZED, INITIALIZED, CAPTURING }

    private static final String TAG = "CleverCaptureActor";

    private final CleverCaptureEngine clevercaptureEngine;
    private final CameraActor cameraActor;

    private State state;

    public CleverCaptureActor(CameraActor cameraActor, CameraPreviewView view) {
        if (cameraActor == null) {
            throw new IllegalArgumentException("cameraActor cannot be null");
        } else if (view == null) {
            throw new IllegalArgumentException("view cannot be null");
        }

        this.cameraActor = cameraActor;
        clevercaptureEngine = new CleverCaptureEngine(cameraActor, view);

        state = State.UNINITIALIZED;
    }

    public void init() {
        if (state != State.UNINITIALIZED) {
            throw new RuntimeException("CleverCaptureActor is already initialized");
        }

        clevercaptureEngine.init();
        state = State.INITIALIZED;
    }

    public void release() {
        if (state == State.UNINITIALIZED) {
            throw new RuntimeException("CleverCaptureActor is not initialized");
        }

        if (state == State.CAPTURING) {
            clevercaptureEngine.cancelCapture();
        }
        clevercaptureEngine.release();
        state = State.UNINITIALIZED;
    }

    public void startCapture(EventListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener cannot be null");
        } else if (state == State.UNINITIALIZED) {
            throw new RuntimeException("CleverCaptureActor is not initialized");
        } else if (state == State.CAPTURING) {
            Log.w(TAG, "CleverCaptureActor is already capturing");
            return;
        }

        clevercaptureEngine.startCapture(listener);
        state = State.CAPTURING;
    }

    public void stopCapture() {
        stopCapture(false);
    }

    public void stopCapture(boolean cancel) {
        if (state == State.UNINITIALIZED) {
            throw new RuntimeException("CleverCaptureActor is not initialized");
        } else if (state != State.CAPTURING) {
            Log.w(TAG, "CleverCaptureActor is not capturing");
            return;
        }

        if (cancel) {
            clevercaptureEngine.cancelCapture();
        } else {
            clevercaptureEngine.completeCapture();
        }
        state = State.INITIALIZED;
    }

    public void updateRoi(RectF roi) {
        cameraActor.updateRoi(roi);
        clevercaptureEngine.updateRoi(roi);
    }

    public void removeRoi() {
         cameraActor.removeRoi();
        clevercaptureEngine.removeRoi();
    }

    public void submitForEval(int frameId) {
        clevercaptureEngine.submitForEval(frameId);
    }

    public Vector<Integer> getPostProcessingResult() {
        return clevercaptureEngine.getPostProcessingResult();
    }

    public void setLogObject(LogFileDump dump) {
        clevercaptureEngine.setLogObject(dump);
    }
}

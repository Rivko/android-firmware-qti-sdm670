/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.panoramaui.hardware;

import android.graphics.PointF;
import android.os.Handler;
import android.util.Log;

import com.qualcomm.qti.panoramaui.AeAfUIHandler;

/**
 * class to manage AE AF settings
 */

public abstract class AeAfHandler {
    private static final String TAG = AeAfHandler.class.getSimpleName();
    protected static final int TOUCH_FOCUS_AREA_WEIGHT = 500;
    protected static final int FOCUS_EXPIRE_DELAY = 5000;

    protected class MappedArea {
        public float x;
        public float y;
        public float width;
        public float height;

        public void set(float x, float y, float width, float height) {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }
    }

    /**
     * Interface for notification from AeAfCamera2
     */
    public interface AeAfCameraListener {
        /**
         * Notification on focus failed
         */
        public void onAutoFocusFailed();

        /**
         * Notification on auto Focus expired
         */
        public void onAutoFocusExpired();
    }

    private final Handler handler;
    protected final AeAfCameraListener cameraListener;
    protected final AeAfUIHandler aeAfUIHandler;

    public enum AeAfType {
        NONE, CONTINUOUS, LOCK_AND_FOCUS, LOCK_AND_FOCUS_REGION;
    };

    protected enum TouchAreaState {
        BUSY, DONE, GONE;
    }

    Runnable focusExpireRunnable = new Runnable() {
        /** {@inheritDoc} */
        @Override
        public void run() {
            if(cameraListener != null) {
                Log.d(TAG, "Focus expired");
                cameraListener.onAutoFocusExpired();
            }
        }
    };
    protected AeAfType type = AeAfType.NONE;

    private boolean shouldLock3A;
    private MappedArea rotatedArea = new MappedArea();
    protected boolean firstTime = true;

    /**
     * Constructor for the class
     *
     * @param aeAfUIHandler
     *            Ae AF UI handler
     * @param shouldLock3A
     *            lock 3A support
     */
    public AeAfHandler(AeAfUIHandler aeAfUIHandler, boolean shouldLock3A, AeAfCameraListener cameraListener) {
        this.shouldLock3A = shouldLock3A;
        this.aeAfUIHandler = aeAfUIHandler;
        this.cameraListener = cameraListener;
        handler = new Handler();
    }

    /**
     * Checks if the AF is locked
     *
     * @return true if locked, false otherwise
     */
    public boolean is3aLocked() {
        return (type == AeAfType.LOCK_AND_FOCUS);
    }

    /**
     * check if the locking is support
     *
     * @return true if supported, false otherwise
     */
    public boolean is3aLockSupported() {
        return shouldLock3A;
    }

    public abstract void setRegionAt(PointF touchPoint, int viewWidth, int viewHeight, int angle);

    /**
     * get a point rotated based on camera angle
     *
     * @param touchPoint
     *            point at which user taps
     * @param width
     *            preview view width
     * @param height
     *            preview view height
     * @param angle
     *            camera angle
     * @return rotated point
     */
    protected MappedArea getRotatedArea(PointF touchPoint, int width, int height, int angle) {
        float startX;
        float startY;

        if (angle == 90) {
            startX = touchPoint.y;
            startY = width - 1 - touchPoint.x;
            rotatedArea.set(startX, startY, height, width);

        } else if (angle == 180) {
            startX = width - 1 - touchPoint.x;
            startY = height - 1 - touchPoint.y;
            rotatedArea.set(startX, startY, width, height);

        } else if (angle == 270) {
            startX = height - 1 - touchPoint.y;
            startY = touchPoint.x;
            rotatedArea.set(startX, startY, height, width);
        } else {// 0
            startX = touchPoint.x;
            startY = touchPoint.y;
            rotatedArea.set(startX, startY, width, height);
        }

        return rotatedArea;
    }

    /**
     * Starts the focus expire timer after touch-to-focus
     */
    protected void startFocusExpiredTimer() {
        Log.d(TAG, "Focus timer started for " + FOCUS_EXPIRE_DELAY);
        handler.postDelayed(focusExpireRunnable, FOCUS_EXPIRE_DELAY);
    }
    /**
     * Clears the focus expire timer if active
     */
    protected void clearFocusExpiredTimer() {
        Log.d(TAG, "Removing any pending callbacks");
        handler.removeCallbacks(focusExpireRunnable);
    }


    /**
     * Clean up the handler
     */
    public void clear() {
        clearFocusExpiredTimer();
        firstTime = true;
    }
}

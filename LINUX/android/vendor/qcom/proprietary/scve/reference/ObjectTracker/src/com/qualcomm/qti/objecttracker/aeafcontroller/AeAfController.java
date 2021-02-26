/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.aeafcontroller;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Map;

import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.util.FloatMath;
import android.util.Log;

import com.qualcomm.qti.objecttracker.cameraengine.CameraManager;
import com.qualcomm.qti.objecttracker.cameraengine.CameraManager.Mode;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject.State;

/**
 * This class implements an auto-exposure/auto-focus controller that works in conjunction with
 * the {@link ObjectTracker}. It only operates when a single object is being tracked; when multiple
 * objects are being tracked, the standard AE/AF behaviour will be active.
 */
public class AeAfController {
    /**
     * This interface provides a method for interested parties to get diagnostic snapshots of the
     * state of the {@link AeAfController}, for debug purposes.
     */
    public static interface DiagnosticListener {
        /**
         * This method will be invoked whenever the internal state of the {@link AeAfController}
         * changes
         * @param diagnostics a {@link String} representing the internal state of the
         *      {@link AeAfController}
         */
        void onUpdate(String diagnostics);
    }

    private static final String TAG = "AeAfController";
    private static final float EXPOSURE_METERING_MIN_SIZE_FRACTION = 0.2f;
    private static final float EXPOSURE_METERING_EXPANSION_FACTOR = 0.33f;
    private static final float STABLE_POSITION_AREA_CHANGE_LIMIT = 0.2f;
    private static final float STABLE_POSITION_RADIUS_FRACTION = 0.05f;
    private static final int STABLE_POSITION_REACHED_DELAY_MS = 500;
    private static final int STABLE_POSITION_LOCK_AE_DELAY_MS = STABLE_POSITION_REACHED_DELAY_MS + 300;
    private static final int STABLE_POSITION_DO_AUTOFOCUS_DELAY_MS = STABLE_POSITION_LOCK_AE_DELAY_MS + 200;

    private final CameraManager cameraManager;
    private final ObjectTracker tracker;
    private final Handler handler;

    private boolean running;
    private DiagnosticListener diagnosticListener;
    private CoordinateSpace cameraSpace;
    private float exposureMeteringMinSize;
    private float stablePositionRadius;
    private StabilityMode stabilityMode;
    private final Rect referencePosition = new Rect();
    private final Rect currentPosition = new Rect();
    private final Rect meteringPosition = new Rect();

    /**
     * Consturct an {@link AeAfController}
     * @param cameraManager the {@link CameraManager} which this {@link AeAfController} should
     *      control
     * @param tracker an {@link ObjectTracker} to which the {@link AeAfController} should listen
     *      for tracking events
     */
    public AeAfController(CameraManager cameraManager, ObjectTracker tracker) {
        if (cameraManager == null) {
            throw new IllegalArgumentException("cameraManager cannot be null");
        } else if (tracker == null) {
            throw new IllegalArgumentException("tracker cannot be null");
        }

        this.cameraManager = cameraManager;
        this.tracker = tracker;
        handler = new Handler(Looper.myLooper());
    }

    /**
     * Start the {@link AeAfController}. This method must be invoked after the {@link CameraManager}
     * with which this object was created has been opened.
     * @throws RuntimeException if {@link AeAfController} is already running, or if CameraManager
     *      is not open
     */
    public void start() {
        if (running) {
            throw new RuntimeException("AeAfController already running");
        } else if (cameraManager.getMode() == Mode.CLOSED) {
            throw new RuntimeException("CameraManager must be opened to start AeAfController");
        }

        Log.v(TAG, "AeAfController started");

        cameraSpace = cameraManager.getPreviewFrameSpace();
        exposureMeteringMinSize = Math.max(cameraSpace.width, cameraSpace.height) *
                EXPOSURE_METERING_MIN_SIZE_FRACTION;
        stablePositionRadius = Math.max(cameraSpace.width, cameraSpace.height) *
                STABLE_POSITION_RADIUS_FRACTION;
        referencePosition.setEmpty();
        currentPosition.setEmpty();
        meteringPosition.setEmpty();

        running = true;
        tracker.registerListener(trackerEventListener);

        onDisabled();
    }

    /**
     * Stop the {@link AeAfController}.
     * @throws RuntimeException if {@link AeAfController} has not been {@link #start()}ed
     */
    public void stop() {
        if (!running) {
            throw new RuntimeException("AeAfController not running");
        }

        tracker.unregisterListener(trackerEventListener);
        running = false;

        clearStabilityEvents();
        cameraSpace = null;
        exposureMeteringMinSize = 0f;
        stablePositionRadius = 0f;
        referencePosition.setEmpty();
        currentPosition.setEmpty();
        meteringPosition.setEmpty();

        Log.v(TAG, "AeAfController stopped");
    }

    /**
     * Attach a {@link DiagnosticListener} to this instance
     * @param listener a {@link DiagnosticListener} which will be notified whenever the internal
     *      state of this {@link AeAfController} changes
     */
    public void setDiagnosticListener(DiagnosticListener listener) {
        diagnosticListener = listener;
    }

    private void onTrackedObjectPositionUpdate(
            Map<TrackedObject, TrackedObject.Position> positions) {
        if (!running) {
            return;
        }

        TrackedObject.Position pos = null;
        Iterator<TrackedObject.Position> it = positions.values().iterator();
        if (it.hasNext()) {
            pos = it.next();
            currentPosition.set(pos.position);
        }

        if (pos != null && pos.state == State.ACQUIRED && !currentPosition.isEmpty()) {
            float currentCenterX = currentPosition.exactCenterX();
            float currentCenterY = currentPosition.exactCenterY();
            float referenceCenterX = referencePosition.exactCenterX();
            float referenceCenterY = referencePosition.exactCenterY();
            int currentArea = currentPosition.width() * currentPosition.height();
            int referenceArea = referencePosition.width() * referencePosition.height();

            // N.B. Check for either movement of center or change in area outside stable limits
            if (referencePosition.isEmpty() ||
                    FloatMath.hypot(currentCenterX - referenceCenterX,
                    currentCenterY - referenceCenterY) > stablePositionRadius ||
                    (float)Math.abs(currentArea - referenceArea) / referenceArea >
                    STABLE_POSITION_AREA_CHANGE_LIMIT) {
                referencePosition.set(currentPosition);
                clearStabilityEvents();
                scheduleStabilityEvents();
                if (stabilityMode == StabilityMode.DISABLED) {
                    onEnable();
                } else if (stabilityMode == StabilityMode.STABLE) {
                    onUnstable();
                }
            }
        } else if (stabilityMode != StabilityMode.DISABLED) {
            referencePosition.setEmpty();
            clearStabilityEvents();
            onDisabled();
        }
    }

    private void scheduleStabilityEvents() {
        handler.postDelayed(onStable, STABLE_POSITION_REACHED_DELAY_MS);
        handler.postDelayed(onStableFocus, STABLE_POSITION_DO_AUTOFOCUS_DELAY_MS);
    }

    private void clearStabilityEvents() {
        handler.removeCallbacks(onStable);
        handler.removeCallbacks(onStableFocus);
    }

    private Runnable onStable = new Runnable() {
        @Override
        public void run() {
            // Make sure metering area is at least the minimum size, and entirely contained within
            // the image frame.
            meteringPosition.set(currentPosition);
            meteringPosition.inset(
                    -(int)(meteringPosition.width() * EXPOSURE_METERING_EXPANSION_FACTOR) / 2,
                    -(int)(meteringPosition.height() * EXPOSURE_METERING_EXPANSION_FACTOR) / 2);
            if (meteringPosition.width() < exposureMeteringMinSize) {
                meteringPosition.inset(
                        (int)((meteringPosition.width() - exposureMeteringMinSize) / 2f), 0);
            }
            if (meteringPosition.height() < exposureMeteringMinSize) {
                meteringPosition.inset(0,
                        (int)((meteringPosition.height() - exposureMeteringMinSize) / 2f));
            }
            if (meteringPosition.left < 0) {
                meteringPosition.offset(-meteringPosition.left, 0);
            } else if (meteringPosition.right > cameraSpace.width) {
                meteringPosition.offset(cameraSpace.width - meteringPosition.right, 0);
            }
            if (meteringPosition.top < 0) {
                meteringPosition.offset(0, -meteringPosition.top);
            } else if (meteringPosition.bottom > cameraSpace.height) {
                meteringPosition.offset(0, cameraSpace.height - meteringPosition.bottom);
            }
            meteringPosition.intersect(0, 0, cameraSpace.width, cameraSpace.height);

            stabilityMode = StabilityMode.STABLE;
            cameraManager.updateAutoExposureLock(false);
            cameraManager.updateAutoExposureMeteringArea(meteringPosition);
            cameraManager.updateAutoFocusArea(currentPosition);
            cameraManager.applyAutoFocusAutoExposure();
            updateDiagnostics(false, meteringPosition, currentPosition);
        }
    };

    private Runnable onStableFocus = new Runnable() {
        @Override
        public void run() {
            cameraManager.doAutoFocus();
        }
    };

    private void onUnstable() {
        stabilityMode = StabilityMode.UNSTABLE;
        cameraManager.updateAutoExposureLock(true);
        cameraManager.updateAutoExposureMeteringArea(null);
        cameraManager.updateAutoFocusArea(null);
        cameraManager.applyAutoFocusAutoExposure();
        updateDiagnostics(true, null, null);
    }

    private void onEnable() {
        stabilityMode = StabilityMode.UNSTABLE;
        cameraManager.updateAutoExposureLock(false);
        cameraManager.updateAutoExposureMeteringArea(null);
        cameraManager.updateAutoFocusArea(null);
        cameraManager.applyAutoFocusAutoExposure();
        updateDiagnostics(false, null, null);
    }

    private void onDisabled() {
        stabilityMode = StabilityMode.DISABLED;
        cameraManager.updateAutoExposureLock(false);
        cameraManager.updateAutoExposureMeteringArea(null);
        cameraManager.updateAutoFocusArea(null);
        cameraManager.applyAutoFocusAutoExposure();
        updateDiagnostics(false, null, null);
    }

    private void updateDiagnostics(boolean isAeLocked, Rect meteringArea, Rect focusArea) {
        if (diagnosticListener != null) {
            String s = String.format(Locale.US,
                    "AE_AF_stability:%s\nAE_locked:%B\nAE_meter:%s\nAF_area:%s",
                    stabilityMode.name(), isAeLocked,
                    meteringArea == null ? "unset" : meteringArea.toShortString(),
                    focusArea == null ? "unset" : focusArea.toShortString());
            diagnosticListener.onUpdate(s);
        }
    }

    private ObjectTracker.EventListener trackerEventListener = new ObjectTracker.EventListener() {
        @Override
        public void onTrackerEvent(long frameTimestamp,
                Map<TrackedObject, TrackedObject.Position> positions) {
            final Map<TrackedObject, TrackedObject.Position> copy =
                    new LinkedHashMap<TrackedObject, TrackedObject.Position>(positions);
            handler.post(new Runnable() {
                @Override
                public void run() {
                    onTrackedObjectPositionUpdate(copy);
                }
            });
        }
    };

    private enum StabilityMode {
        DISABLED,
        UNSTABLE,
        STABLE
    }
}

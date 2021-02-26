/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera;

import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.params.MeteringRectangle;
import android.os.Handler;
import android.util.Log;
import android.util.Size;

import com.qualcomm.qti.fr_camera.metrics.Metrics;
import com.qualcomm.qti.fr_camera.metrics.StateMetric;

import java.util.Arrays;
import java.util.Objects;

/**
 * This class manages the AE/AF algorithm for a {@link CameraCaptureSession}. It is intended to work
 * in conjunction with {@link CameraActor}.
 */
/*package*/ class AeAfController {
    /**
     * This interface describes callbacks that {@link AeAfController} makes to the class which
     * manages the overall {@link CameraCaptureSession}.
     */
    public interface EventListener {
        /**
         * Request that a new preview {@link CaptureRequest} be configured (via
         * {@link #configurePreviewCaptureRequest(CaptureRequest.Builder)}) due to AE/AF changes
         */
        void onReconfigureCapture();

        /**
         * Indicates that a new AF operation has started
         */
        void onFocusing();

        /**
         * Indicates that the current AF operation has been cancelled
         */
        void onFocusingCancelled();

        /**
         * Indicates that the current AF operation has completed successfully
         */
        void onFocused();

        /**
         * Indicates that the current AF operation has completed unsuccessfully (focus is not sharp)
         */
        void onFocusFailed();
    }

    private enum State { IDLE, FOCUSING, CLOSED }

    private static final String TAG = "SocialCamera";
    private static final float AE_MIN_METERING_SIZE = 0.15f; // 15% of min(sensor_w, sensor_h)
    private static final float AF_MIN_METERING_SIZE = 0.05f; // 5% of min(sensor_w, sensor_h)
    private static final MeteringRectangle[] EMPTY_AE_AF_METERING_RECT = new MeteringRectangle[] {
            new MeteringRectangle(0, 0, 0, 0, 0) };
    private static final Metrics metrics = Metrics.getInstance();

    private final EventListener listener;
    private final boolean isAeMeteringSupported;
    private final boolean isAfMeteringSupported;
    private final Size activeArraySize;
    private final float minAeMeteringSize;
    private final float minAfMeteringSize;
    private final StateMetric afStateMetric = new StateMetric("AFState", null);
    private final StateMetric afRegionMetric = new StateMetric("AFRgn", null);
    private final StateMetric aeRegionMetric = new StateMetric("AERgn", null);

    private State state = State.IDLE;
    private int previewCaptureRequestSequenceId;
    private boolean isAeEnabled; // updated whenever configuring a preview capture request
    private boolean isAfEnabled; // updated whenever configuring a preview capture request
    private int lastAfMode = -1;
    private AfStrategy afStrategy;
    private MeteringRectangle[] aeMeteringRect = EMPTY_AE_AF_METERING_RECT;
    private MeteringRectangle[] afMeteringRect = EMPTY_AE_AF_METERING_RECT;
    private boolean afRequired = true;

    /**
     * Construct a new {@link AeAfController}
     * @param characteristics the {@link CameraCharacteristics} for the active {@link CameraDevice}
     * @param listener a {@link EventListener} to notify for {@link AeAfController} events
     * @throws NullPointerException if characteristics or listener are null
     */
    public AeAfController(CameraCharacteristics characteristics, EventListener listener) {
        Objects.requireNonNull(characteristics, "characteristics may not be null");
        Objects.requireNonNull(listener, "listener may not be null");
        this.listener = listener;

        Integer maxRegionsAe = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AE);
        Integer maxRegionsAf = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AF);
        assert maxRegionsAe != null;
        assert maxRegionsAf != null;
        isAeMeteringSupported = (maxRegionsAe != 0);
        isAfMeteringSupported = (maxRegionsAf != 0);

        Rect sensorSize = characteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        assert sensorSize != null;
        activeArraySize = new Size(sensorSize.width(), sensorSize.height());
        int minSize = Math.min(activeArraySize.getWidth(), activeArraySize.getHeight());
        minAeMeteringSize = AE_MIN_METERING_SIZE * minSize;
        minAfMeteringSize = AF_MIN_METERING_SIZE * minSize;

        metrics.addMetric(afStateMetric);
        metrics.addMetric(afRegionMetric);
        metrics.addMetric(aeRegionMetric);
    }

    /**
     * Close this {@link AeAfController}. Any outstanding AF operations will be cancelled. After
     * invoking this method, no further methods on this instance may be invoked.
     */
    public void close() {
        if (state == State.FOCUSING) {
            listener.onFocusingCancelled();
        }

        metrics.removeMetric(afStateMetric);
        metrics.removeMetric(afRegionMetric);
        metrics.removeMetric(aeRegionMetric);

        state = State.CLOSED;
    }

    /**
     * Set a new AE/AF metering region. This may also trigger an autofocus operation (depending on
     * the camera AF mode).
     * @param region a {@link RectF}, in active array coordinates (ranging from [0,0] to
     *               [active array width, active array height]), describing the region to use for
     *               AE/AF metering. This may specify a single point or an area. If null, the
     *               current AE/AF metering area will be cleared.
     * @throws IllegalStateException if {@link #close()} has been invoked
     */
    public void setAeAfRegion(RectF region) {
        if (state == State.CLOSED) {
            throw new IllegalStateException("Cannot set AE/AF region after closed");
        }

        if (region == null || !isAeMeteringSupported) {
            aeMeteringRect = EMPTY_AE_AF_METERING_RECT;
        } else {
            RectF temp = new RectF(region);
            final float dx = minAeMeteringSize - Math.min(minAeMeteringSize, temp.width());
            final float dy = minAeMeteringSize - Math.min(minAeMeteringSize, temp.height());
            temp.inset(-dx / 2f, -dy / 2f);
            if (temp.intersect(0, 0, activeArraySize.getWidth(), activeArraySize.getHeight())) {
                aeMeteringRect = new MeteringRectangle[] { new MeteringRectangle(
                        (int)temp.left, (int)temp.top, (int)(temp.right - temp.left),
                        (int)(temp.bottom - temp.top), 1) };
            } else {
                aeMeteringRect = EMPTY_AE_AF_METERING_RECT;
            }
        }

        if (region == null || !isAfMeteringSupported) {
            afMeteringRect = EMPTY_AE_AF_METERING_RECT;
        } else {
            RectF temp = new RectF(region);
            final float dx = minAfMeteringSize - Math.min(minAfMeteringSize, temp.width());
            final float dy = minAfMeteringSize - Math.min(minAfMeteringSize, temp.height());
            temp.inset(-dx / 2f, -dy / 2f);
            if (temp.intersect(0, 0, activeArraySize.getWidth(), activeArraySize.getHeight())) {
                afMeteringRect = new MeteringRectangle[] { new MeteringRectangle(
                        (int)temp.left, (int)temp.top, (int)(temp.right - temp.left),
                        (int)(temp.bottom - temp.top), 1) };
            } else {
                afMeteringRect = EMPTY_AE_AF_METERING_RECT;
            }
        }

        afRequired = true;

        if ((isAeEnabled && isAeMeteringSupported) || isAfEnabled) {
            listener.onReconfigureCapture();
        }
    }

    /**
     * Request that the camera be focused in preparation for still image capture
     * @return true if the camera is already focused (and a still image may be captured
     *         immediately), else false
     */
    public boolean deferStillImageCaptureForFocus() {
        boolean isFocusRequired = true;

        switch (state) {
            case IDLE:
                if (afStrategy == null || afStrategy.isAfRequiredForStillImageCapture()) {
                    afRequired = true;
                    listener.onReconfigureCapture();
                } else {
                    isFocusRequired = false;
                }
                break;

            case FOCUSING:
                // N.B. Already focusing; don't issue a new AF request, but return true so that
                // caller waits for AF to complete
                break;

            case CLOSED:
                throw new IllegalStateException("Cannot deferStillImageCaptureForFocus after " +
                        "closed");

            default:
                throw new IllegalStateException("Unknown state [" + state + "]");
        }

        return isFocusRequired;
    }

    /**
     * Configure the provided preview {@link CaptureRequest.Builder} with the current AE/AF
     * settings. This should be invoked every time a preview {@link CaptureRequest} is being
     * configured, whether in response to an {@link EventListener#onReconfigureCapture()} callback
     * or not. The AE/AF mode must already be configured on this {@link CaptureRequest.Builder}
     * before invoking this method.
     * @throws NullPointerException if request is null
     * @throws IllegalStateException if {@link #close()} has been invoked
     */
    public void configurePreviewCaptureRequest(CaptureRequest.Builder request) {
        if (state == State.CLOSED) {
            throw new IllegalStateException("Cannot configure preview capture request when " +
                    "AE/AF controller is disabled");
        }

        final Integer aeMode = request.get(CaptureRequest.CONTROL_AE_MODE);
        assert aeMode != null;
        isAeEnabled = (aeMode != CaptureRequest.CONTROL_AE_MODE_OFF);

        final Integer afMode = request.get(CaptureRequest.CONTROL_AF_MODE);
        assert afMode != null;
        isAfEnabled = (afMode != CaptureRequest.CONTROL_AF_MODE_OFF &&
                afMode != CaptureRequest.CONTROL_AF_MODE_EDOF);

        if (afMode != lastAfMode) {
            if (state == State.FOCUSING) {
                listener.onFocusingCancelled();
                state = State.IDLE;
            }

            switch (afMode) {
                case CaptureRequest.CONTROL_AF_MODE_OFF:
                case CaptureRequest.CONTROL_AF_MODE_EDOF:
                    afStrategy = new AfStrategyDisabled();
                    break;

                case CaptureRequest.CONTROL_AF_MODE_AUTO:
                case CaptureRequest.CONTROL_AF_MODE_MACRO:
                    afStrategy = new AfStrategyTriggered();
                    break;

                case CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE:
                case CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_VIDEO:
                    afStrategy = new AfStrategyContinuous();
                    break;

                default:
                    throw new IllegalStateException("Unknown AF mode [" + afMode + "]");
            }

            lastAfMode = afMode;
            afRequired = true;
        }

        if (!isAeMeteringSupported) {
            aeRegionMetric.setState("unsupported");
        } else if (!isAeEnabled) {
            aeRegionMetric.setState("disabled");
        } else {
            String aeMeteringRectStr = Arrays.deepToString(aeMeteringRect);
            Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_AE_REGIONS.getName() + "] to [" +
                    aeMeteringRectStr + "]");
            request.set(CaptureRequest.CONTROL_AE_REGIONS, aeMeteringRect);
            aeRegionMetric.setState(aeMeteringRectStr);
        }

        if (!isAfMeteringSupported) {
            afRegionMetric.setState("unsupported");
        } else if (!isAfEnabled) {
            afRegionMetric.setState("disabled");
        } else {
            String afMeteringRectStr = Arrays.deepToString(afMeteringRect);
            Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_AF_REGIONS.getName() + "] to [" +
                    afMeteringRectStr + "]");
            request.set(CaptureRequest.CONTROL_AF_REGIONS, afMeteringRect);
            afRegionMetric.setState(afMeteringRectStr);
        }

        if (isAfEnabled) {
            // Set AF trigger to IDLE. The AfStrategy will change this if necessary.
            request.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_IDLE);
        }

        if (afRequired) {
            if (state == State.FOCUSING) {
                listener.onFocusingCancelled();
                state = State.IDLE;
            }

            afStrategy.onAfRequired(request);
            afRequired = false;
        }
    }

    /**
     * Configure the provided picture {@link CaptureRequest.Builder} with the AE/AF settings applied
     * to the last preview {@link CaptureRequest.Builder} (via
     * {@link #configurePreviewCaptureRequest(CaptureRequest.Builder)})
     * @param request the {@link CaptureRequest.Builder} to configure with AE/AF settings
     * @throws IllegalStateException if {@link #close()} has been invoked
     */
    public void configurePictureCaptureRequest(CaptureRequest.Builder request) {
        if (state == State.CLOSED) {
            throw new IllegalStateException("Cannot configure preview capture request when " +
                    "AE/AF controller is disabled");
        }

        if (isAeMeteringSupported && isAeEnabled) {
            Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_AE_REGIONS.getName() + "] to [" +
                    Arrays.deepToString(aeMeteringRect) + "]");
            request.set(CaptureRequest.CONTROL_AE_REGIONS, aeMeteringRect);
        }

        if (isAfMeteringSupported && isAfEnabled) {
            Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_AF_REGIONS.getName() + "] to [" +
                    Arrays.deepToString(afMeteringRect) + "]");
            request.set(CaptureRequest.CONTROL_AF_REGIONS, afMeteringRect);
        }
    }

    /**
     * Set the preview {@link CaptureRequest} sequence ID (as obtained from
     * {@link CameraCaptureSession#setRepeatingRequest(CaptureRequest, CameraCaptureSession.CaptureCallback, Handler)}).
     * This must be invoked after each call to
     * {@link #configurePreviewCaptureRequest(CaptureRequest.Builder)}, to ensure that only
     * {@link CaptureResult}s from frames with the relevant AE/AF configuration are analyzed for
     * AF state changes.
     * @param id the sequence ID corresponding to the setRepeatingRequest invocation for the preview
     *           {@link CaptureRequest} configured via
     *           {@link #configurePreviewCaptureRequest(CaptureRequest.Builder)}
     */
    public void setPreviewCaptureRequestSequenceId(int id) {
        previewCaptureRequestSequenceId = id;
    }

    /**
     * Process a {@link CaptureResult} and analyze it for AF state changes
     * @param result a {@link CaptureResult} to process
     */
    public void processPreviewCaptureResult(CaptureResult result) {
        if (result.getSequenceId() < previewCaptureRequestSequenceId) {
            // N.B. This is from an older capture request. Don't process AF state changes, we have
            // a newer request with the latest AF configuration in-flight.
            return;
        }

        // N.B. According to the docs, this should never be null, but I have seen it so :(
        Integer afState = result.get(CaptureResult.CONTROL_AF_STATE);
        if (afState != null) {
            afStrategy.processAfState(afState);
            afStateMetric.setState(afStateToString(afState));
        }
    }

    private static String afStateToString(int afState) {
        switch (afState) {
            case CaptureResult.CONTROL_AF_STATE_INACTIVE:
                return "inactive";

            case CaptureResult.CONTROL_AF_STATE_PASSIVE_SCAN:
                return "passive_scan";

            case CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED:
                return "passive_focused";

            case CaptureResult.CONTROL_AF_STATE_PASSIVE_UNFOCUSED:
                return "passive_unfocused";

            case CaptureResult.CONTROL_AF_STATE_ACTIVE_SCAN:
                return "active_scan";

            case CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED:
                return "focused";

            case CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
                return "not_focused";

            default:
                return Integer.toString(afState);
        }
    }

    private interface AfStrategy {
        boolean isAfRequiredForStillImageCapture();
        void onAfRequired(CaptureRequest.Builder request);
        void processAfState(int afState);
    }

    // CONTROL_AF_MODE_OFF and CONTROL_AF_MODE_EDOF
    private class AfStrategyDisabled implements AfStrategy {
        @Override
        public boolean isAfRequiredForStillImageCapture() {
            return false;
        }

        @Override
        public void onAfRequired(CaptureRequest.Builder request) {
        }

        @Override
        public void processAfState(int afState) {
        }
    }

    // CONTROL_AF_MODE_AUTO and CONTROL_AF_MODE_MACRO
    private class AfStrategyTriggered implements AfStrategy {
        @Override
        public boolean isAfRequiredForStillImageCapture() {
            return true;
        }

        @Override
        public void onAfRequired(CaptureRequest.Builder request) {
            Log.v(TAG, "Triggering AF sweep");
            request.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_START);
            state = State.FOCUSING;
            listener.onFocusing();
        }

        @Override
        public void processAfState(int afState) {
            if (state == State.IDLE) {
                return;
            }

            switch (afState) {
                case CaptureResult.CONTROL_AF_STATE_INACTIVE:
                case CaptureResult.CONTROL_AF_STATE_PASSIVE_SCAN:
                case CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED:
                case CaptureResult.CONTROL_AF_STATE_PASSIVE_UNFOCUSED:
                    Log.w(TAG, "Unexpected AF state [" + afState + "] while autofocusing");
                    state = State.IDLE;
                    listener.onFocusingCancelled();
                    break;

                case CaptureResult.CONTROL_AF_STATE_ACTIVE_SCAN:
                    // N.B. Scan in progress, remain in current state
                    break;

                case CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED:
                    Log.v(TAG, "Triggered AF completed");
                    state = State.IDLE;
                    listener.onFocused();
                    break;

                case CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
                    Log.v(TAG, "Triggered AF failed");
                    state = State.IDLE;
                    listener.onFocusFailed();
                    break;

                default:
                    throw new IllegalStateException("Unknown AF state [" + afState + "]");
            }
        }
    }

    // CONTROL_AF_MODE_CONTINUOUS_PICTURE and CONTROL_AF_MODE_CONTINUOUS_VIDEO
    private class AfStrategyContinuous implements AfStrategy {
        @Override
        public boolean isAfRequiredForStillImageCapture() {
            return false;
        }

        @Override
        public void onAfRequired(CaptureRequest.Builder request) {
            Log.i(TAG, "Moving to FOCUSING state for passive focus operation");
            state = State.FOCUSING;
            listener.onFocusing();
        }

        @Override
        public void processAfState(int afState) {
            if (state == State.IDLE) {
                switch (afState) {
                    case CaptureResult.CONTROL_AF_STATE_INACTIVE:
                    case CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED:
                    case CaptureResult.CONTROL_AF_STATE_PASSIVE_UNFOCUSED:
                        // N.B. AF not currently active, remain in current state
                        break;

                    case CaptureResult.CONTROL_AF_STATE_PASSIVE_SCAN:
                        Log.v(TAG, "Passive AF started");
                        state = State.FOCUSING;
                        listener.onFocusing();
                        break;

                    case CaptureResult.CONTROL_AF_STATE_ACTIVE_SCAN:
                    case CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED:
                    case CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
                        Log.w(TAG, "Active AF scan detected by continuous AF strategy; ignoring");
                        break;

                    default:
                        throw new IllegalStateException("Unknown AF state [" + afState + "]");
                }
            } else {
                switch (afState) {
                    case CaptureResult.CONTROL_AF_STATE_INACTIVE:
                        Log.w(TAG, "Unexpected AF state [" + afState + "] while autofocusing");
                        state = State.IDLE;
                        listener.onFocusingCancelled();
                        break;

                    case CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED:
                        Log.v(TAG, "Passive AF completed");
                        state = State.IDLE;
                        listener.onFocused();
                        break;

                    case CaptureResult.CONTROL_AF_STATE_PASSIVE_UNFOCUSED:
                        Log.v(TAG, "Passive AF failed");
                        state = State.IDLE;
                        listener.onFocusFailed();
                        break;

                    case CaptureResult.CONTROL_AF_STATE_PASSIVE_SCAN:
                        // N.B. Scan in progress, remain in current state
                        break;

                    case CaptureResult.CONTROL_AF_STATE_ACTIVE_SCAN:
                    case CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED:
                    case CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
                        Log.w(TAG, "Active AF scan detected by continuous AF strategy; ignoring");
                        break;

                    default:
                        throw new IllegalStateException("Unknown AF state [" + afState + "]");
                }
            }
        }
    }
}

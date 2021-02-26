/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.objecttrackerdemo.renderer;

import java.util.LinkedHashMap;
import java.util.Map;

import android.util.Log;

import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.Renderer;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TrackedObjectRenderer;

/**
 * This class acts as an adapter between {@link GLFrame}s (produced by a {@link GLFrameTranslator})
 * and {@link Renderer}s (which consume textures). It takes care of texture binding, frame tracking,
 * and handling {@link TrackedObject} updates.
 */
/*package*/ class GLFrameRendererAdapter {
    private static final String TAG = GLFrameRendererAdapter.class.getSimpleName();

    private final ObjectTracker tracker;

    // These objects may only be accessed in the thread in which GLFrameRenderAdapter is created
    private int inputTexture;
    private int trackedObjectTexture;
    private Renderer renderer;
    private boolean isTrackedObjectRenderer;
    private GLFrame inputFrame;
    private GLFrame currentTrackedFrame;
    private boolean inputDirty;

    // These objects may be accessed in arbitrary thread contexts; protect with lock
    private final Object lock = new Object();
    private final LinkedHashMap<TrackedObject, TrackedObject.Position> trackedObjects =
            new LinkedHashMap<TrackedObject, TrackedObject.Position>();
    private GLFrame trackedObjectsFrame;
    private boolean trackedObjectsDirty;
    private long nextFrameTimestamp;
    private GLFrame nextTrackedFrame;

    /**
     * Construct a new {@link GLFrameRendererAdapter}
     * @param tracker an {@link ObjectTracker} instance from which to receive {@link TrackedObject}
     *      updates
     * @throws IllegalArgumentException if tracker is null
     */
    public GLFrameRendererAdapter(ObjectTracker tracker) {
        if (tracker == null) {
            throw new IllegalArgumentException("tracker may not be null");
        }

        this.tracker = tracker;

        inputDirty = true;
        trackedObjectsDirty = true;
        nextFrameTimestamp = -1;

        tracker.registerListener(listener);
    }

    /**
     * Set the GL texture handles to which {@link GLFrame}s will be bound
     * @param inputTexture the GL texture to which to bind the current {@link GLFrame}
     * @param trackedObjectTexture the GL texture to which to bind the frame against which
     *      {@link TrackedObject} were last reported
     */
    public void setTextures(int inputTexture, int trackedObjectTexture) {
        this.inputTexture = inputTexture;
        this.trackedObjectTexture = trackedObjectTexture;
        inputDirty = true;

        synchronized (lock) {
            trackedObjectsDirty = true;
        }
    }

    /**
     * Set the {@link Renderer} which should be used for rendering the current {@link GLFrame}.
     * If an instance of {@link TrackedObjectRenderer} is provided, it will additionally be provided
     * the position and {@link GLFrame} for the current {@link TrackedObject}s.
     * @param renderer the {@link Renderer} with which to render {@link GLFrame}s
     */
    public void setRenderer(Renderer renderer) {
        this.renderer = renderer;
        isTrackedObjectRenderer = (renderer != null && renderer instanceof TrackedObjectRenderer);
        inputDirty = true;

        if (!isTrackedObjectRenderer) {
            if (currentTrackedFrame != null) {
                currentTrackedFrame.release();
                currentTrackedFrame = null;
            }

            if (nextTrackedFrame != null) {
                nextTrackedFrame.release();
                nextTrackedFrame = null;
            }
        }

        synchronized (lock) {
            trackedObjectsDirty = true;
        }
    }

    /**
     * Update this {@link GLFrameRendererAdapter} with the most recent input {@link GLFrame}
     * @param frame a {@link GLFrame} with a non-zero reference count.
     * @throws IllegalArgumentException if frame is null
     */
    public void doInputFrame(GLFrame frame) {
        if (frame == null) {
            throw new IllegalArgumentException("frame may not be null");
        }

        if (inputFrame != null) {
            inputFrame.release();
        }
        inputFrame = frame.retain();
        inputDirty = true;

        if (isTrackedObjectRenderer) {
            synchronized (lock) {
                // Note: the nextFrameTimestamp value is reported by the ObjectTracker, which
                // reports the value when tracking is started. In theory, it is possible that the
                // frame would already have been presented to doInputFrame before the ObjectTracker
                // reported it to us (via our FrameEventListener). In practice, however, frames
                // must first be translated by the GLFrameTraslator before they make it here,
                // meaning that the ObjectTracker will have already reported the next frame
                // timestamp before the frame could ever make it here.
                if (frame.timestamp == nextFrameTimestamp) {
                    if (nextTrackedFrame != null) {
                        nextTrackedFrame.release();
                    }
                    nextTrackedFrame = frame.retain();
                    nextFrameTimestamp = -1;
                }
            }
        }
    }

    /**
     * Render the current frame using the active {@link Renderer}
     */
    public void render() {
        if (inputFrame == null) {
            return;
        }

        if (inputDirty) {
            inputDirty = false;
            inputFrame.bindToTexture(inputTexture);
            renderer.setInputTexture(null, inputTexture);
        }

        if (isTrackedObjectRenderer) {
            synchronized (lock) {
                if (trackedObjectsDirty) {
                    trackedObjectsDirty = false;

                    if (currentTrackedFrame != null) {
                        currentTrackedFrame.release();
                        currentTrackedFrame = null;
                    }

                    if (trackedObjectsFrame != null) {
                        currentTrackedFrame = trackedObjectsFrame.retain();
                        trackedObjectsFrame.release();
                        trackedObjectsFrame = null;
                    } else {
                        Log.w(TAG, "Missed a tracked object frame, substituting current frame");
                        currentTrackedFrame = inputFrame.retain();
                    }

                    currentTrackedFrame.bindToTexture(trackedObjectTexture);
                    // N.B. Don't need to copy trackedObjects, since the collection is never
                    // modified, just replaced (by onTrackerEvent)
                    ((TrackedObjectRenderer)renderer).setTrackedObjects(trackedObjects, null,
                            trackedObjectTexture);
                }
            }
        }

        renderer.render();

        inputFrame.syncContainer.insertSync();
        if (isTrackedObjectRenderer) {
            currentTrackedFrame.syncContainer.insertSync();
        }
    }

    /**
     * Release all resources owned by this {@link GLFrameRendererAdapter}. After invoking this
     * method, this {@link GLFrameRendererAdapter} may no longer be used.
     */
    public void release() {
        tracker.unregisterListener(listener);

        if (inputFrame != null) {
            inputFrame.release();
            inputFrame = null;
        }

        if (currentTrackedFrame != null) {
            currentTrackedFrame.release();
            currentTrackedFrame = null;
        }

        inputTexture = 0;
        trackedObjectTexture = 0;
        isTrackedObjectRenderer = false;
        renderer = null;
        inputDirty = true;

        synchronized (lock) {
            trackedObjectsDirty = true;
            trackedObjects.clear();
            nextFrameTimestamp = -1;

            if (trackedObjectsFrame != null) {
                trackedObjectsFrame.release();
                trackedObjectsFrame = null;
            }

            if (nextTrackedFrame != null) {
                nextTrackedFrame.release();
                nextTrackedFrame = null;
            }
        }
    }

    private final ObjectTracker.EventListener listener = new ObjectTracker.FrameEventListener() {
        @Override
        public void onTrackerEvent(long frameTimestamp,
                Map<TrackedObject, TrackedObject.Position> positions) {
            synchronized (lock) {
                trackedObjectsDirty = true;

                if (trackedObjectsFrame != null) {
                    trackedObjectsFrame.release();
                    trackedObjectsFrame = null;
                }

                if (nextTrackedFrame != null) {
                    if (nextTrackedFrame.timestamp == frameTimestamp) {
                        trackedObjectsFrame = nextTrackedFrame.retain();
                    }

                    nextTrackedFrame.release();
                    nextTrackedFrame = null;
                }

                trackedObjects.clear();
                trackedObjects.putAll(positions);
            }
        }

        @Override
        public void onFrameProcessed(long frameTimestamp) {
            synchronized (lock) {
                nextFrameTimestamp = frameTimestamp;
            }
        }
    };
}

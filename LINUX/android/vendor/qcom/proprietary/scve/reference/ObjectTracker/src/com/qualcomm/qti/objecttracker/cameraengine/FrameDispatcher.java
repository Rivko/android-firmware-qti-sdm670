/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine;

import java.util.LinkedHashSet;
import java.util.concurrent.atomic.AtomicInteger;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;

import android.hardware.Camera;
import android.util.Log;

/**
 * This class implements a camera preview frame dispatcher, which allows sharing a single frame
 * of NV21 data from the camera with multiple frame sinks, without requiring any buffer copies. It
 * handles all buffer and frame sink management.
 */
@SuppressWarnings("deprecation")
public class FrameDispatcher {
    private static final String TAG = "FrameDispatcher";
    private static final int NUM_FRAME_BUFFERS = 3;

    private final AtomicInteger frameGeneration = new AtomicInteger();

    // These fields may be accessed from arbitrary thread contexts, and must be synchronized on
    // the lock object
    private final Object lock = new Object();
    private Camera camera;
    private final LinkedHashSet<FrameSink> frameSinks = new LinkedHashSet<FrameSink>();

    // These fields are written only by the FrameDispatcher thread. they may be safely read there
    // without synchronization, but all other accesses require protection via lock.
    private boolean running;
    private CoordinateSpace cameraSpace;

    /**
     * Construct a {@link FrameDispatcher}
     */
    /*package*/ FrameDispatcher() {
    }

    /**
     * Add a {@link FrameSink} to which this class will dispatch {@link Frame}s. If the dispatcher
     * is running, then {@link FrameSink#onInitialize(int, int)} will be invoked immediately on this
     * sink.
     * @param sink {@link FrameSink} to add
     * @throws IllegalArgumentException if sink is null
     */
    public void addFrameSink(FrameSink sink) {
        if (sink == null) {
            throw new IllegalArgumentException("sink cannot be null");
        }

        boolean notify;
        CoordinateSpace cameraSpace;

        synchronized (lock) {
            frameSinks.add(sink);
            notify = running;
            cameraSpace = this.cameraSpace;
        }

        if (notify) {
            sink.onInitialize(cameraSpace);
        }
    }

    /**
     * Removes a {@link FrameSink} previously added via {@link #addFrameSink(FrameSink)}. If the
     * dispatcher is running, then {@link FrameSink#onRelease()} will be invoked immediately on this
     * sink.
     * @param sink {@link FrameSink} to remove
     * @throws IllegalArgumentException if sink is null
     */
    public void removeFrameSink(FrameSink sink) {
        if (sink == null) {
            throw new IllegalArgumentException("sink cannot be null");
        }

        boolean notify;

        synchronized (lock) {
            frameSinks.remove(sink);
            notify = running;
        }

        if (notify) {
            sink.onRelease();
        }
    }

    /**
     * Starts the {@link FrameDispatcher}. This will register callbacks with {@link Camera} and
     * notify sinks that {@link Frame}s will be forthcoming. It should be called after the {@link
     * Camera}s preview frame size is configured.
     * @param camera Open {@link Camera} on which the dispatcher will operate
     * @param cameraSpace a {@link CoordinateSpace} describing the camera preview frames
     * @throws IllegalArgumentException if camera is null
     * @throws RuntimeException if the dispatcher is already running
     */
    /*package*/ void start(Camera camera, CoordinateSpace cameraSpace) {
        if (camera == null) {
            throw new IllegalArgumentException("camera cannot be null");
        }

        Log.v(TAG, "Frame dispatcher started");

        synchronized (lock) {
            if (running) {
                throw new IllegalArgumentException("FrameDispatcher already running");
            }
            running = true;
            this.camera = camera;
            this.cameraSpace = cameraSpace;

            for (FrameSink sink : frameSinks) {
                sink.onInitialize(cameraSpace);
            }
        }

        camera.setPreviewCallbackWithBuffer(cameraPreviewCallback);
        int frameSize = (cameraSpace.width * cameraSpace.height * 3) / 2; // Size of NV21 frame (in bytes)
        for (int i = 0; i < NUM_FRAME_BUFFERS; i++) {
            camera.addCallbackBuffer(new byte[frameSize]);
        }
    }

    /**
     * Stops the {@link FrameDispatcher}. This will unregister callbacks with the {@link Camera}
     * and notify sinks that no more {@link Frame}s are forthcoming.
     * @throws RuntimeException if the dispatcher is not running
     */
    /*package*/ void stop() {
        // N.B. incremement frame generation before clearing the preview callback, to avoid the race
        // where a frame buffer is added back to the queue after it is cleared.
        frameGeneration.incrementAndGet();
        camera.setPreviewCallbackWithBuffer(null);

        synchronized (lock) {
            if (!running) {
                throw new IllegalArgumentException("FrameDispatcher not running");
            }
            running = false;
            camera = null;
            cameraSpace = null;

            for (FrameSink sink : frameSinks) {
                sink.onRelease();
            }
        }

        Log.v(TAG, "Frame dispatcher stopped");
    }

    /**
     * Invoke when a {@link Frame} is no longer being used by any {@link FrameSink}s. It will take
     * the necessary steps to recycle or clean up the {@link Frame}.
     * <p><i>NOTE: may be invoked in arbitrary thread contexts</i>
     * @param frame a {@link Frame} to recycle or clean up
     */
    /*package*/ void releaseFrame(Frame frame) {
        if (frameGeneration.get() == frame.tag) {
            camera.addCallbackBuffer(frame.imageDataNV21);
        }
    }

    private final Camera.PreviewCallback cameraPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            Frame f = new Frame(cameraSpace.width, cameraSpace.height, System.nanoTime(), data,
                    FrameDispatcher.this, frameGeneration.get());

            synchronized (lock) {
                f.retain(frameSinks.size());
                for (FrameSink sink : frameSinks) {
                    sink.onFrame(f);
                }
            }

            f.release(); // release our reference to the Frame
        }
    };
}

/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.misc;

import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import com.qualcomm.qti.objecttracker.cameraengine.Frame;
import com.qualcomm.qti.objecttracker.cameraengine.FrameDispatcher;
import com.qualcomm.qti.objecttracker.cameraengine.FrameSink;

/**
 * This utility class implements a {@link HandlerThread} which is also a {@link FrameSink}.
 * Subclasses need only implement a few methods ({@link #doStart(int, int)}, {@link #doStop()}, and
 * {@link #doFrame(Frame)}) to create a thread which whose lifetime will be associated with the
 * {@link FrameDispatcher} to which it is subsequently attached.
 */
public abstract class ThreadedFrameSink implements FrameSink {
    private static final String TAG = "ThreadedFrameSink";

    private final String threadName;

    // These members are only safe to access on the UI thread (which is where the onInitialize and
    // onRelease FrameSink callbacks initiate)
    private HandlerThread thread;
    private Handler handler;

    /**
     * Construct a {@link ThreadedFrameSink}
     * @param threadName Name for the created thread
     */
    protected ThreadedFrameSink(String threadName) {
        this.threadName = threadName;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onInitialize(final CoordinateSpace cameraSpace) {
        thread = new HandlerThread(threadName);
        thread.start();
        handler = new Handler(thread.getLooper());
        handler.post(new Runnable() {
            @Override
            public void run() {
                doStart(cameraSpace);
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onRelease() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                doStop();
            }
        });
        thread.quitSafely();
        try {
            thread.join();
        } catch (InterruptedException e) {
            Log.i(TAG, "Interrupted while waiting for thread to terminate", e);
            Thread.currentThread().interrupt();
        }
        thread = null;
        handler = null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onFrame(final Frame frame) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                doFrame(frame);
            }
        });
    }

    /**
     * This method will be invoked (in the context of the enclosed thread) when notified to start
     * by the {@link FrameDispatcher} (via {@link FrameSink#onInitialize(int, int)})
     * @param cameraSpace A {@link CoordinateSpace} describing the camera frames from the
     *      {@link FrameDispatcher}
     */
    protected abstract void doStart(CoordinateSpace cameraSpace);

    /**
     * This method will be invoked (in the context of the enclosed thread) when notified to stop
     * by the {@link FrameDispatcher} (via {@link FrameSink#onRelease()})
     */
    protected abstract void doStop();

    /**
     * This method will be invoked (in the context of the enclosed thread) when notified of a new
     * {@link Frame} by the {@link FrameDispatcher} (via {@link FrameSink#onFrame(Frame)})
     * @param frame the {@link Frame} to process
     */
    protected abstract void doFrame(Frame frame);
}

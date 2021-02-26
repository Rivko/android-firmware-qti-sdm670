/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine;

import com.qualcomm.qti.objecttracker.misc.BaseReferenceCountedObject;

/**
 * This class contains a single frame of image data from the camera. It is a {@link
 * ReferenceCountedObject}; whomever creates the object is responsible for setting an initial
 * reference count (via {@link ReferenceCountedObject#reset(int)} based on the number of consumers
 * of the frame data. After it has been {@link #release()}d, it will be returned to the provided
 * {@link FrameDispatcher} for reuse or cleanup.
 */
public class Frame extends BaseReferenceCountedObject<Frame> {
    /** Width of frame, in px */
    public final int width;

    /** Height of frame, in px */
    public final int height;

    /** Timestamp the frame was received by the app (obtained from {@link System#nanoTime()}) */
    public final long timestamp;

    /**
     * A buffer containing the NV21 image data for this frame. The image data is assumed to be
     * packed (ie width == stride). The buffer will be at least (width * height * 1.5) bytes in
     * length.
     */
    public final byte[] imageDataNV21;

    private final FrameDispatcher dispatcher;
    /*package*/ final int tag;

    /**
     * Construct a {@link Frame}
     * @param width see {@link #width}
     * @param height see {@link #height}
     * @param timestamp see {@link #timestamp}
     * @param imageDataNV21 see {@link #imageDataNV21}
     * @param dispatcher {@link FrameDispatcher} to which this {@link Frame} should be returned
     *      when all {@link FrameSink}s are finished with it
     * @param tag an integer value which can be used for arbitrary object tracking purposes
     */
    /*package*/ Frame(int width, int height, long timestamp, byte[] imageDataNV21,
            FrameDispatcher dispatcher, int tag) {
        this.width = width;
        this.height = height;
        this.timestamp = timestamp;
        this.imageDataNV21 = imageDataNV21;
        this.dispatcher = dispatcher;
        this.tag = tag;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onReleased() {
        dispatcher.releaseFrame(this);
    }
}

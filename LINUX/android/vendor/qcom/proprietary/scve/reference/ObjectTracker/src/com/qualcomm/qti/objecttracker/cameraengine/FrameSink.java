/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.cameraengine;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;

/**
 * A class which wishes to receive preview {@link Frame}s from the camera should implement this
 * interface and register themselves with the {@link FrameDispatcher}.
 */
public interface FrameSink {
    /**
     * Invoked when the {@link FrameDispatcher} is starting. It is guaranteed to be invoked before
     * {@link #onFrame(Frame)} is invoked.
     * <p><i>Note: this method will be invoked in an arbitrary thread context</i>
     * @param cameraSpace A {@link CoordinateSpace} describing the camera frames which will be
     *      issued to this {@link FrameSink}
     */
    void onInitialize(CoordinateSpace cameraSpace);

    /**
     * Invoked when the {@link FrameDispatcher} is stopping. It is guaranteed that {@link
     * #onFrame(Frame)} will not be invoked after this method is invoked.
     * <p><i>Note: this method will be invoked in an arbitrary thread context</i>
     */
    void onRelease();

    /**
     * Invoked each time a new {@link Frame} is available from the {@link FrameDispatcher}.
     * @param frame A new {@link Frame}, containing the image data from the camera. When the sink
     * is done with the {@link Frame}, it should invoke the {@link Frame#release()} method. Once
     * all sinks have released the {@link Frame}, it will be returned to the camera for reuse.
     * <p><i>Note: this method will be invoked in an arbitrary thread context</i>
     */
    void onFrame(Frame frame);
}

/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TextureFormat;

/**
 * A class which wishes to receive camera preview {@link GLFrame}s from the camera should implement
 * this interface and register themselves with the {@link GLFrameTranslator}
 */
/*package*/ interface GLFrameSink {
    /**
     * Invoked when the {@link GLFrameSink} is starting. It is guaranteed to be invoked before
     * {@link #onFrame(GLFrame)} is invoked.
     * <p><i>Note: this method will be invoked in an arbitrary thread context</i>
     * @param cameraSpace A {@link CoordinateSpace} describing the camera frames encoded as
     *      {@link GLFrame} objects which will be issued to this {@link GLFrameSink}
     * @param textureFormat {@link TextureFormat} of the {@link GLFrame} textures provided by
     *      {@link #onFrame(GLFrame)}
     */
    void onInitialize(CoordinateSpace cameraSpace, TextureFormat textureFormat);

    /**
     * Invoked when the {@link GLFrameSink} is stopping. It is guaranteed that {@link
     * #onFrame(GLFrame)} will not be invoked after this method is invoked.
     * <p><i>Note: this method will be invoked in an arbitrary thread context</i>
     */
    void onRelease();

    /**
     * Invoked each time a new {@link GLFrame} is available from the {@link GLFrameSink}.
     * @param frame A new {@link GLFrame}, containing the image data from the camera. When the sink
     * is done with the {@link GLFrame}, it should invoke the {@link GLFrame#release()} method. Once
     * all sinks have released the {@link GLFrame}, it will be returned to the pool for reuse.
     * <p><i>Note: this method will be invoked in an arbitrary thread context</i>
     */
    void onFrame(GLFrame frame);
}

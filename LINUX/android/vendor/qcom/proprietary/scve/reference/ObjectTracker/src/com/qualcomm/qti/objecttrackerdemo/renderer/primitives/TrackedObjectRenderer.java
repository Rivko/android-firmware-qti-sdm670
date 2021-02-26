/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.util.Map;

import com.qualcomm.qti.objecttracker.tracker.TrackedObject;

/**
 * This interface extends {@link Renderer} to add support for renderers which receive the location
 * of tracked objects, as well as the frame against which object tracking was performed (which may
 * be several frames prior to the current frame)
 */
public interface TrackedObjectRenderer extends Renderer {
    /**
     * Provides the current set of {@link TrackedObject}s, as well as the frame against which
     * object tracking was performed
     * @param positions the current set of {@link TrackedObject}s and their corresponding
     *      {@link TrackedObject.Position}s. May be null, in which case this {@link Renderer} will
     *      assume no objects are currently being tracked.
     * @param transform A 4x4 affine transform to apply to texture coordinates referencing this
     *      texture. May be null, in which case an identity transform will be applied.
     * @param texture A set of valid GL texture handles, 1 per plane in the input images. For eg,
     *      {@link TextureFormat#RGBA} images are interleaved, and have only one plane, whereas
     *      {@link TextureFormat#NV21} images are semi-planar, having one plane for the Y component
     *      and another for the UV components
     * @throws IllegalArgumentException if transform is not null and not of length 16
     * @throws RuntimeException if {@link #init()} has not been invoked
     */
    void setTrackedObjects(Map<TrackedObject, TrackedObject.Position> positions, float[] transform,
            int... texture);
}

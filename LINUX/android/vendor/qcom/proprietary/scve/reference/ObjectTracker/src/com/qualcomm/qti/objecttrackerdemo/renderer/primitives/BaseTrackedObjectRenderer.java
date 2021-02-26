/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;

/**
 * This class extends {@link BaseRenderer} with the capability to listen for {@link ObjectTracker}
 * events, plus handling synchronization and notification of objects being acquired/released in
 * the context of the rendering thread.
 */
/*package*/ abstract class BaseTrackedObjectRenderer extends BaseRenderer
        implements TrackedObjectRenderer {
    private final HashSet<TrackedObject> acquired = new HashSet<TrackedObject>();
    private final HashSet<TrackedObject> released = new HashSet<TrackedObject>();
    private final LinkedHashMap<TrackedObject, TrackedObject.Position> tracked =
            new LinkedHashMap<TrackedObject, TrackedObject.Position>();
    private final Map<TrackedObject, TrackedObject.Position> trackedUpdate =
            new LinkedHashMap<TrackedObject, TrackedObject.Position>();
    private boolean trackedObjectsDirty;
    private int[] trackedObjectTexture;
    private float[] trackedObjectTextureTransform;

    /**
     * Construct a {@link BaseTrackedObjectRenderer}. See {@link BaseRenderer#BaseRenderer(int, int,
     * int, int, TextureFormat)} for a description of common parameters.
     */
    public BaseTrackedObjectRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
            TextureFormat textureFormat) {
        super(textureSpace, viewportSpace, textureFormat);
    }

    /** {@inheritDoc} */
    @Override
    public void setTrackedObjects(Map<TrackedObject, TrackedObject.Position> positions,
            float[] transform, int... texture) {
        if (transform != null && transform.length != 16) {
            throw new IllegalArgumentException("If non-null, transform must be a 4x4 matrix");
        } else if (!initialized) {
            throw new RuntimeException("Cannot invoke setTrackedObjects when uninitialized");
        }

        trackedObjectsDirty = true;

        trackedUpdate.clear();
        if (positions != null) {
            trackedUpdate.putAll(positions);
        }

        trackedObjectTexture = texture;
        trackedObjectTextureTransform = (transform != null ? transform : IDENTITY_TRANSFORM);
    }

    /** {@inheritDoc} */
    @Override
    protected void onInit() {
        super.onInit();

        trackedObjectsDirty = true;
        trackedObjectTexture = null;
        trackedObjectTextureTransform = IDENTITY_TRANSFORM;
    }

    /** {@inheritDoc} */
    @Override
    protected void onRelease() {
        super.onRelease();

            trackedUpdate.clear();
        acquired.clear();
        released.clear();
        tracked.clear();
    }

    /** {@inheritDoc} */
    @Override
    protected void onPreRender() {
        super.onPreRender();

        if (trackedObjectsDirty) {
            if (trackedObjectTexture == null || trackedObjectTexture.length == 0) {
                throw new IllegalStateException("Cannot render without tracked object texture");
            }

            trackedObjectsDirty = false;

            acquired.clear();
            acquired.addAll(trackedUpdate.keySet());
            acquired.removeAll(tracked.keySet());
            if (!acquired.isEmpty()) {
                onTrackedObjectsAcquired(acquired);
        }

            released.clear();
            released.addAll(tracked.keySet());
            released.removeAll(trackedUpdate.keySet());
            if (!released.isEmpty()) {
                onTrackedObjectsReleased(released);
    }

            tracked.clear();
            tracked.putAll(trackedUpdate);
            onTrackedObjectsUpdate(tracked, trackedObjectTexture, trackedObjectTextureTransform);

            trackedUpdate.clear();
        }
    }

    /**
     * Subclasses should override this method to be notified when a new object is being tracked. It
     * will be invoked in the context of the rendering thread, directly before
     * {@link #onTrackedObjectsReleased(Collection)} is invoked.
     * @param trackedObjects {@link Collection} of {@link TrackedObject}s that have been newly
     *      acquired
     */
    protected abstract void onTrackedObjectsAcquired(Collection<TrackedObject> trackedObjects);

    /**
     * Subclasses should override this method to be notified when a tracked object is released. It
     * will be invoked in the context of the rendering thread, directly before
     * {@link #onTrackedObjectsUpdate(Collection)} is invoked.
     * @param trackedObjects {@link Collection} of {@link TrackedObject}s that have been newly
     *      released
     */
    protected abstract void onTrackedObjectsReleased(Collection<TrackedObject> trackedObjects);

    /**
     * Subclasses should override this method to be notified of the state & position of all tracked
     * objects. It will be invoked in the context of the rendering thread, directly before rendering
     * occurs. See {@link #onSetInputTexture(int, float[])} for a description of textures and
     * textureTransform.
     * @param positions {@link Map} of {@link TrackedObject}s and corresponding
     *      {@link TrackedObject.Position}s for tracked objects that are currently being tracked
     */
    protected abstract void onTrackedObjectsUpdate(
            Map<TrackedObject, TrackedObject.Position> positions,
            int[] texture, float[] textureTransform);
}

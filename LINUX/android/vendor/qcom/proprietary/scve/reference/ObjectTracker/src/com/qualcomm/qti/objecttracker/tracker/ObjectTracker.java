/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.tracker;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;

import com.qualcomm.qti.objecttracker.cameraengine.Frame;
import com.qualcomm.qti.objecttracker.cameraengine.FrameDispatcher;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.ThreadedFrameSink;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject.Mode;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject.State;

import android.annotation.SuppressLint;
import android.graphics.Point;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

/**
 * This class provides an asynchronous object tracking service. it receives image {@link Frame}s
 * from the {@link FrameDispatcher}, and tracking requests from various other consumers around the
 * application. On each image frame, it will update the object tracking results and notify all
 * registered {@link EventListener}s of the change (if any).
 */
public class ObjectTracker extends ThreadedFrameSink {
    /**
     * Classes may implement this interface to receive notification of {@link ObjectTracker} events.
     * Events include both {@link TrackedObject} state changes and position changes.
     */
    public interface EventListener {
        /**
         * Invoked whenever one or more {@link TrackedObject}s change state or position
         * @param frameTimestamp the {@link Frame#timestamp} with which the trackedObject
         *      positions & states were generated
         * @param positions {@link Map} of all currently tracked objects and their position
         */
        void onTrackerEvent(long frameTimestamp,
                Map<TrackedObject, TrackedObject.Position> positions);
    }

    /**
     * Classes may implement this interface (which is an extension of the {@link EventListener}
     * interface) to additionally receive notifications about which {@link Frame} the
     * {@link ObjectTracker} is currently processing. The {@link #onFrameProcessing(long)} method
     * will be invoked on every frame processed by the {@link ObjectTracker}, whether or not it
     * results in any {@link TrackedObject} events. As such, it can be used to discard cached data
     * for older tracking events.
     */
    public interface FrameEventListener extends EventListener {
        /**
         * Invoked for each frame processed by the {@link ObjectTracker}, immediately before the
         * frame is processed. As such, it will always occur before all invocations of
         * {@link #onTrackerEvent(long, Collection)} (if any) for the specified frame.
         * @param frameTimestamp the {@link Frame#timestamp} of the {@link Frame} against which
         *      tracking results will next be reported
         */
        void onFrameProcessed(long frameTimestamp);
    }

    /**
     * This constant represents the maximum number of objects which can be tracked simultaneously
     */
    public static final int MAX_NUM_TRACKED_OBJECTS = ObjectTrackerBinding.MAX_NUM_TRACKED_OBJECTS;

    private static final String TAG = "ObjectTracker";
    private static final int CONFIDENCE_THRESHOLD = 10;

    private final OperationMode operationMode;
    private final Precision precision;

    // These members may be accessed from arbitrary thread contexts, so protect with lock
    private final Object lock = new Object();
    private Handler handler;
    private boolean processing;
    private final Rect imageRect = new Rect();
    private final ArrayList<TrackedObject> workingObjects = new ArrayList<TrackedObject>();
    private final LinkedHashMap<TrackedObject, TrackedObject.Position> workingPositions =
            new LinkedHashMap<TrackedObject, TrackedObject.Position>();
    private final ArrayList<TrackedObject> pendingObjects = new ArrayList<TrackedObject>();
    private final ArrayList<TrackedObject> errorObjects = new ArrayList<TrackedObject>();
    @SuppressLint("UseSparseArrays")
    private final HashMap<Integer, TrackedObject> trackedObjects =
            new HashMap<Integer, TrackedObject>();
    private final HashSet<EventListener> listeners = new HashSet<EventListener>();
    private long frameTimestamp;
    private int minRoiDimension;
    private int maxRoiDimension;

    // These members are only accessed from the tracker thread
    private ObjectTrackerBinding tracker;

    /**
     * Construct an {@link ObjectTracker}
     * @param operationMode the {@link OperationMode} to supply to the {@link ObjectTrackerBinding}
     * @param precision the {@link Precision} to supply to the {@link ObjectTrackerBinding}
     */
    public ObjectTracker(OperationMode operationMode, Precision precision) {
        super(TAG);
        this.operationMode = operationMode;
        this.precision = precision;
    }

    /**
     * Register a listener with the {@link ObjectTracker}. Upon registration, if the tracker is
     * currently running, the listeners {@link EventListener#onTrackerEvent(Collection)} will be
     * immediately invoked.
     * @param listener the {@link EventListener} to register
     */
    public void registerListener(EventListener listener) {
        synchronized (lock) {
            boolean added = listeners.add(listener);

            // If tracker is currently running, immediately invoke the new listener
            if (added && handler != null) {
                getTrackedObjects(workingObjects);
                TrackedObject.Position.getTrackedObjectPositions(workingObjects, workingPositions);
                if (!trackedObjects.isEmpty()) {
                    listener.onTrackerEvent(frameTimestamp, workingPositions);
                }
            }
        }
    }

    /**
     * Unregister a listener with the {@link ObjectTracker}. After unregistering, no further
     * events will be broadcast to this listener
     * @param listener the {@link EventListener} to unregister
     */
    public void unregisterListener(EventListener listener) {
        synchronized (lock) {
            listeners.remove(listener);
        }
    }

    /**
     * Gets the minimum width/height (for the current input image size) of the {@link Rect} which
     * may be provided to {@link #trackObject(Rect)}
     * @return the minimum width/height of the region of interest to track (via
     *      {@link #trackObject(Rect)})
     * @throws ObjectTrackerNotRunningException if the {@link ObjectTracker} is not running
     */
    public int getMinRoiDimension() {
        synchronized (lock) {
            if (handler == null) {
                throw new ObjectTrackerNotRunningException();
            }

            return minRoiDimension;
        }
    }

    /**
     * Gets the maximum width/height (for the current input image size) of the {@link Rect} which
     * may be provided to {@link #trackObject(Rect)}
     * @return the maximum width/height of the region of interest to track (via
     *      {@link #trackObject(Rect)})
     * @throws ObjectTrackerNotRunningException if the {@link ObjectTracker} is not running
     */
    public int getMaxRoiDimension() {
        synchronized (lock) {
            if (handler == null) {
                throw new ObjectTrackerNotRunningException();
            }

            return maxRoiDimension;
        }
    }

    /**
     * Request that the {@link ObjectTracker} track an object contained within the provided region
     * of interest {@link Rect}. On receipt of the next image frame, the tracker will attempt to
     * register the object.
     * <p><i>NOTE: tracking requests are asynchronous; to receive notifications of registration
     * success/failure, register an {@link EventListener} on this {@link ObjectTracker}<i>
     * @param r Object region of interest {@link Rect}
     * @return a {@link TrackedObject} representing this tracking request
     * @throws IllegalArgumentException if r is null, if its width/height are outside the range
     *      returned by {@link #getMinRoiDimension()}/{@link #getMaxRoiDimension()}, or if it is
     *      not entirely contained within the image frame
     * @throws ObjectTrackerNotRunningException if the {@link ObjectTracker} is not running
     */
    public TrackedObject trackObject(Rect r) {
        if (r == null || r.isEmpty()) {
            throw new IllegalArgumentException("r cannot be null or empty");
        }

        synchronized (lock) {
            if (handler == null) {
                throw new ObjectTrackerNotRunningException();
            } else if (!imageRect.contains(r)) {
                throw new IllegalArgumentException("r [" + r.toShortString() + "] must be " +
                        "entirely within the image [" + imageRect.toShortString() + "]");
            }

            int width = r.width();
            int height = r.height();
            if (width < minRoiDimension || width > maxRoiDimension ||
                    height < minRoiDimension || height > maxRoiDimension) {
                throw new IllegalArgumentException("Size of r (" + width + "," + height + ") is " +
                        "outside the allowed range of " + minRoiDimension + "-" + maxRoiDimension);
            }

            TrackedObject t = new TrackedObject(this, r);
            Log.v(TAG, "Requesting tracking on [" + t + "]");
            pendingObjects.add(t);
            return t;
        }
    }

    /**
     * Request that the {@link ObjectTracker} track an object indicated by the provided
     * {@link Point} within the object. On receipt of subsequent image frame(s), the tracker will
     * attempt to register the object. Only a single request to track an object by {@link Point}
     * will be processed at a time. If there are other {@link TrackedObject} by {@link Point} in
     * the {@link State#PENDING} state, this request will be delayed until all requests issued
     * prior to this one have moved out of the {@link State#PENDING} state. Since this may entail a
     * delay (with subsequent tracking errors if the image contents at the touched {@link Point}
     * have changed), it is recommended that callers check for this with {@link
     * #isTrackObjectByPointPending()}, and handle this case appropriately.
     * <p><i>NOTE: tracking requests are asynchronous; to receive notifications of registration
     * success/failure, register an {@link EventListener} on this {@link ObjectTracker}<i>
     * @param p {@link Point} within the object to register
     * @return a {@link TrackedObject} representing this tracking request
     * @throws IllegalArgumentException if p is null, or if it is not within the bounds of the
     *      image frame
     * @throws ObjectTrackerNotRunningException if the {@link ObjectTracker} is not running
     */
    public TrackedObject trackObject(Point p) {
        if (p == null) {
            throw new IllegalArgumentException("p cannot be null");
        }

        synchronized (lock) {
            if (handler == null) {
                throw new ObjectTrackerNotRunningException();
            } else if (!imageRect.contains(p.x, p.y)) {
                throw new IllegalArgumentException("p [" + p.toString() + "] must be within the" +
                        "bounds of the image [" + imageRect.toShortString() + "]");
            }

            TrackedObject t = new TrackedObject(this, p);
            Log.v(TAG, "Requesting tracking on [" + t + "]");
            pendingObjects.add(t);
            return t;
        }
    }

    /**
     * Request that the {@link ObjectTracker} stop tracking all tracked objects
     * <p><i>NOTE: untracking requests are asynchronous; to receive notifications when objects are
     * no longer being tracked, register an {@link EventListener} on this {@link ObjectTracker}<i>
     */
    public void untrackAllObjects() {
        synchronized (lock) {
            if (handler == null) {
                // Don't throw here; this makes cleanup easy on global app state changes, regardless
                // of whether the tracker is actually running
                return;
            }

            Log.v(TAG, "Stopping tracking all objects");

            handler.post(new Runnable() {
                @Override
                public void run() {
                    doUntrackAllObjects();
                }
            });
        }
    }

    /**
     * Request that the given {@link TrackedObject} stop being tracked
     * <p><i>NOTE: untracking requests are asynchronous; to receive notifications when objects are
     * no longer being tracked, register an {@link EventListener} on this {@link ObjectTracker}<i>
     * @param t {@link TrackedObject} to stop tracking
     * @throws IllegalArgumentException if t is null
     * @throws ObjectTrackerNotRunningException if the {@link ObjectTracker} is not running
     */
    /*package*/ void untrackObject(final TrackedObject t) {
        if (t == null) {
            throw new IllegalArgumentException("t cannot be null");
        }

        synchronized (lock) {
            if (handler == null) {
                throw new ObjectTrackerNotRunningException();
            }

            Log.v(TAG, "Stopping tracking of object [" + t + "]");

            handler.post(new Runnable() {
                @Override
                public void run() {
                    doUntrackObject(t);
                }
            });
        }
    }

    /**
     * Check if there is a {@link TrackedObject} requested via {@link #trackObject(Point)} pending;
     * only one such request may be pending at a time.
     * @return true if there is a {@link TrackedObject} requested via {@link #trackObject(Point)}
     *      pending, else false
     * @throws ObjectTrackerNotRunningException if the {@link ObjectTracker} is not running
     */
    public boolean isTrackObjectByPointPending() {
        synchronized (lock) {
            for (TrackedObject t : pendingObjects) {
                if (t.getMode() == Mode.TRACK_BY_POINT) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Populate the provided {@link Collection} with the current set of {@link TrackedObject}s
     * @param trackedObjects a {@link Collection} to which to add the current set of
     *      {@link TrackedObject}s
     * @throws IllegalArgumentException if trackedObjects is null
     */
    public void getTrackedObjects(Collection<TrackedObject> trackedObjects) {
        if (trackedObjects == null) {
            throw new IllegalArgumentException("trackedObjects may not be null");
        }

        trackedObjects.clear();

        synchronized (lock) {
            trackedObjects.addAll(pendingObjects);
            trackedObjects.addAll(errorObjects);
            trackedObjects.addAll(this.trackedObjects.values());
        }
    }

    /**
     * Return a {@link Collection} containing the current set of {@link TrackedObject}s
     * @return a {@link Collection} of {@link TrackedObject}s
     */
    public Collection<TrackedObject> getTrackedObjects() {
        ArrayList<TrackedObject> trackedObjects = new ArrayList<TrackedObject>();
        getTrackedObjects(trackedObjects);
        return trackedObjects;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onFrame(Frame frame) {
        synchronized (lock) {
            // Check if we are already processing a frame. If so, just drop this frame.
            if (processing) {
                frame.release();
                return;
            }
            processing = true;
        }

        super.onFrame(frame);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doStart(CoordinateSpace cameraSpace) {
        try {
            // N.B. for the preview frames from the camera, stride == width
            tracker = new ObjectTrackerBinding(operationMode, precision, cameraSpace.width,
                    cameraSpace.height, cameraSpace.width);
        } catch (InternalObjectTrackerException e) {
            throw new RuntimeException("Error starting object tracker", e);
        }

        synchronized (lock) {
            processing = false;
            resetTrackedObjectLists();
            handler = new Handler(Looper.myLooper());
            minRoiDimension = tracker.getMinRoiDimension();
            maxRoiDimension = tracker.getMaxRoiDimension();
            imageRect.set(0, 0, cameraSpace.width, cameraSpace.height);
            frameTimestamp = -1;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doStop() {
        synchronized (lock) {
            handler = null;
            doUntrackAllObjects();
            processing = false;
        }

        tracker.release();
        tracker = null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doFrame(Frame frame) {
        synchronized (lock) {
            boolean dirty = false;

            frameTimestamp = frame.timestamp;
            notifyListenersOfFrameProcessed();

            // First, process all pending requests
            boolean hasProcessedPendingTrackByPoint = false;
            for (TrackedObject t : pendingObjects) {
                try {
                    int id;
                    switch (t.getMode()) {
                    case TRACK_BY_RECT:
                        id = tracker.registerObject(frame.imageDataNV21, t.getInitialRect());
                        break;

                    case TRACK_BY_POINT:
                        // Only a single TRACK_BY_POINT may be processed at a time. Skip all
                        // subsequent TRACK_BY_POINT requests without processing them.
                        if (hasProcessedPendingTrackByPoint) {
                            continue;
                        }

                        id = tracker.registerObject(frame.imageDataNV21, t.getInitialPoint(),
                                t.getPendingFrameCount() == 0);
                        hasProcessedPendingTrackByPoint = true;
                        break;

                    default:
                        throw new RuntimeException("Unexpected tracker mode " + t.getMode().name());
                    }

                    if (id == ObjectTrackerBinding.PENDING_REGISTRATION) {
                        dirty |= t.processTrackerEvent(State.PENDING, null);
                        if (t.getPendingFrameCount() == TrackedObject.MAX_PENDING_FRAME_COUNT) {
                            dirty |= t.processTrackerEvent(State.UNABLE_TO_ACQUIRE_OBJECT, null);
                            errorObjects.add(t);
                        }
                    } else {
                        t.setTrackedObjectId(id);
                        dirty |= t.processTrackerEvent(State.ACQUIRED, new Rect());
                        trackedObjects.put(id, t);
                    }
                } catch (ObjectRegistrationFailedException e) {
                    Log.e(TAG, "Failed registering object [" + t + "]", e);
                    dirty |= t.processTrackerEvent(State.UNABLE_TO_ACQUIRE_OBJECT, null);
                    errorObjects.add(t);
                } catch (TooManyObjectsRegisteredException e) {
                    Log.e(TAG, "Max number of registered objects reached, failed registering [" +
                            t + "]");
                    dirty |= t.processTrackerEvent(State.UNABLE_TO_ACQUIRE_OBJECT, null);
                    errorObjects.add(t);
                } catch (InternalObjectTrackerException e) {
                    throw new RuntimeException("Error while registering object [" + t + "]", e);
                }
            }
            pendingObjects.removeAll(errorObjects);
            pendingObjects.removeAll(trackedObjects.values());

            // Second, run tracking on all objects (including newly registered objects)
            if (!trackedObjects.isEmpty()) {
                try {
                    for (ObjectTrackerBinding.Result result : tracker.trackObjects(
                            frame.imageDataNV21)) {
                        TrackedObject t = trackedObjects.get(result.id);
                        if (t == null) {
                            Log.e(TAG, "Got tracker result for unknown object [" + result.id + "]");
                            continue;
                        }

                        if (result.confidence >= CONFIDENCE_THRESHOLD && !result.pos.isEmpty()) {
                            dirty |= t.processTrackerEvent(State.ACQUIRED, result.pos);
                        } else {
                            dirty |= t.processTrackerEvent(State.LOST, null);
                        }
                    }
                } catch (ObjectTrackerException e) {
                    throw new RuntimeException("Error while tracking objects", e);
                }
            }

            if (dirty) {
                notifyListenersOfTrackedObjectStates();
            }

            processing = false;
        }

        frame.release();
    }

    // N.B. Runs in tracker thread context
    private void doUntrackAllObjects() {
        synchronized (lock) {
            // First, update the pending list. These objects need no cleanup.
            for (TrackedObject t : pendingObjects) {
                t.processTrackerEvent(State.CLOSED, null);
            }

            // Second, update the error list. These objects need no cleanup.
            for (TrackedObject t : errorObjects) {
                t.processTrackerEvent(State.CLOSED, null);
            }

            // Third, update the tracked object list and inform the tracker that these objects are
            // no longer being tracked.
            for (TrackedObject t : trackedObjects.values()) {
                try {
                    tracker.unregisterObject(t.getTrackedObjectId());
                } catch (InvalidObjectIdentifierException e) {
                    Log.e(TAG, "Tracker state inconsistent, unregistration of object [" + t +
                            "] failed. Squelching error.", e);
                } catch (InternalObjectTrackerException e) {
                    throw new RuntimeException("Error while unregistering object [" + t + "]", e);
                }
                t.processTrackerEvent(State.CLOSED, null);
            }

            // No need for a dirty flag, since we are untracking everything
            notifyListenersOfTrackedObjectStates();
            resetTrackedObjectLists();
        }
    }

    // N.B. Runs in tracker thread context
    private void doUntrackObject(TrackedObject trackedObject) {
        synchronized (lock) {
            // Locate the item to untrack. It may not be present if it is already closed. If
            // currently tracked, make sure to inform the tracker to stop tracking.
            if (trackedObjects.containsKey(trackedObject.getTrackedObjectId())) {
                try {
                    tracker.unregisterObject(trackedObject.getTrackedObjectId());
                } catch (InvalidObjectIdentifierException e) {
                    Log.e(TAG, "Tracker state inconsistent, unregistration of object [" +
                            trackedObject + "] failed. Squelching error.", e);
                } catch (InternalObjectTrackerException e) {
                    throw new RuntimeException("Object tracker error while unregistering object [" +
                            trackedObject + "]", e);
                }
            }

            boolean dirty = trackedObject.processTrackerEvent(State.CLOSED, null);

            // If object is newly closed, notify listeners
            if (dirty) {
                notifyListenersOfTrackedObjectStates();
            }

            trackedObjects.remove(trackedObject.getTrackedObjectId());
            pendingObjects.remove(trackedObject);
            errorObjects.remove(trackedObject);
        }
    }

    // N.B. Must be invoked with lock held
    private void resetTrackedObjectLists() {
        pendingObjects.clear();
        errorObjects.clear();
        trackedObjects.clear();
    }

    // N.B. Must be invoked with lock held
    private void notifyListenersOfTrackedObjectStates() {
        getTrackedObjects(workingObjects);
        if (!workingObjects.isEmpty()) {
            TrackedObject.Position.getTrackedObjectPositions(workingObjects, workingPositions);
            for (EventListener l : listeners) {
                l.onTrackerEvent(frameTimestamp, workingPositions);
            }
        }
    }

    // N.B. Must be invoked with lock held
    private void notifyListenersOfFrameProcessed() {
            for (EventListener l : listeners) {
            if (l instanceof FrameEventListener) {
                ((FrameEventListener)l).onFrameProcessed(frameTimestamp);
            }
        }
    }
}

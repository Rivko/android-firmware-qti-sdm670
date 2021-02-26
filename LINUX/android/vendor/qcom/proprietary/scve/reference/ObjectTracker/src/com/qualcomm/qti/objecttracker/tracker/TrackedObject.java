/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.tracker;

import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.Map;

import android.graphics.Point;
import android.graphics.Rect;
import android.util.Log;

/**
 * This class encapsulates a single tracked object. It provides a public view of the object to
 * {@link ObjectTracker.EventListener}s, and an internal API for the {@link ObjectTracker} class.
 */
public class TrackedObject {
    /**
     * The states that this {@link TrackedObject} may be in
     */
    public enum State {
        /**
         * The tracking request has not yet been processed.
         * Allowed transitions: {@link #ACQUIRED}, {@link #UNABLE_TO_ACQUIRE_OBJECT},
         * {@link #CLOSED}.
         */
        PENDING,

        /**
         * The tracker was unable to acquire the object for tracking.
         * Allowed transitions: {@link #CLOSED}.
         */
        UNABLE_TO_ACQUIRE_OBJECT,

        /**
         * The object was successfully acquired, and its current position is known.
         * Allowed transitions: {@link #LOST}, {@link #CLOSED}.
         *
         */
        ACQUIRED,

        /**
         * The object was successfully acquired, but its current position is not known.
         * Allowed transitions: {@link #ACQUIRED}, {@link #CLOSED}
         */
        LOST,

        /**
         * This object is no longer being tracked.
         * Allowed transitions: none
         */
        CLOSED
    }

    /**
     * The different registration modes supported by {@link TrackedObject}s
     */
    /*package*/ enum Mode {
        /** Initiate tracking via a region of interest {@link Rect} */
        TRACK_BY_RECT,

        /** Initiate tracking via a indicated {@link Point} within the object to track */
        TRACK_BY_POINT
    }

    /**
     * This class represents the instantaneous state of a {@link TrackedObject}
     */
    public static class Position {
        /** The position of the {@link TrackedObject} (as of the call to {@link #getPosition()}) */
        public final Rect position = new Rect();

        /** The state of the {@link TrackedObject} (as of the call to {@link #getPosition()}) */
        public State state = State.PENDING;

        /**
         * The prior state of the {@link TrackedObject} (as of the call to {@link #getPosition()})
         */
        public State lastState = State.PENDING;

        /**
         * This method is a helper to translate from a {@link Collection} of {@link TrackedObject}s
         * to a {@link Map} of {@link TrackedObject}s to their instantaneous {@link Position}
         * @param trackedObjects a {@link Collection} of {@link TrackedObject} for which to obtain
         *      {@link Position}s
         * @param positions a {@link Map} to receive the set of {@link TrackedObject}s and their
         *      corresponding {@link Position}s
         * @throws IllegalArgumentException if trackedObjects or positions are null
         */
        public static void getTrackedObjectPositions(Collection<TrackedObject> trackedObjects,
                Map<TrackedObject, TrackedObject.Position> positions) {
            if (trackedObjects == null) {
                throw new IllegalArgumentException("trackedObjects may not be null");
            } else if (positions == null) {
                throw new IllegalArgumentException("positions may not be null");
            }

            positions.clear();

            for (TrackedObject t : trackedObjects) {
                positions.put(t, t.getPosition());
            }
        }

        /**
         * This method is a varient of {@link #getTrackedObjectPositions(Collection, Map)} which
         * additionally allocates (and returns) the {@link Map}
         * @return a {@link Map} containing the set of {@link TrackedObject}s and their
         *      corresponding {@link Position}s
         * @see #getTrackedObjectPositions(Collection, Map)
         */
        public static Map<TrackedObject, TrackedObject.Position> getTrackedObjectPositions(
                Collection<TrackedObject> trackedObjects) {
            Map<TrackedObject, TrackedObject.Position> positions =
                    new LinkedHashMap<TrackedObject, TrackedObject.Position>();
            getTrackedObjectPositions(trackedObjects, positions);
            return positions;
        }

    }

    /**
     * The maximum number of frames that an object may be in the {@link State#PENDING} state before
     * it should be moved to the {@link State#UNABLE_TO_ACQUIRE_OBJECT} state.
     */
    /*package*/ static final int MAX_PENDING_FRAME_COUNT = 10;

    private static final String TAG = "TrackedObject";

    // No synchronization is required to access these members
    private final ObjectTracker tracker;
    private final Mode mode;
    private final Rect initialRect;
    private final Point initialPoint;

    // These variables may be read by any thread (but only modified by the tracker thread). Changes
    // from the tracker thread and reads from other threads should be synchronized.
    private final Object lock = new Object();
    private State state = State.PENDING;
    private State lastState = State.PENDING;
    private final Rect position = new Rect();

    // These variables are only accessed in the context of the tracker thread, and do not need
    // to be synchronized
    private int trackedObjectId = ObjectTrackerBinding.PENDING_REGISTRATION;
    private int pendingFrameCount;

    /**
     * Construct a {@link Mode#TRACK_BY_RECT} {@link TrackedObject}
     * @param tracker {@link ObjectTracker} which owns this {@link TrackedObject}
     * @param r Initial region of interest {@link Rect} for tracking
     */
    /*package*/ TrackedObject(ObjectTracker tracker, Rect r) {
        this.tracker = tracker;
        mode = Mode.TRACK_BY_RECT;
        initialRect = new Rect(r);
        initialPoint = null;
    }

    /**
     * Construct a {@link Mode#TRACK_BY_POINT} {@link TrackedObject}
     * @param tracker {@link ObjectTracker} which owns this {@link TrackedObject}
     * @param p Initial indicated {@link Point} within the object to track
     */
    /*package*/ TrackedObject(ObjectTracker tracker, Point p) {
        this.tracker = tracker;
        mode = Mode.TRACK_BY_POINT;
        initialPoint = new Point(p);
        initialRect = null;
    }

    /**
     * Get the registration {@link Mode} of this {@link TrackedObject}
     * @return the {@link Mode} of this {@link TrackedObject}
     */
    /*package*/ Mode getMode() {
        return mode;
    }

    /**
     * For {@link Mode#TRACK_BY_RECT} {@link TrackedObject}s, returns the initial region of interest
     * {@link Rect}
     * @return the initial region of interest {@link Rect}
     * @throws RuntimeException if the mode is not {@link Mode#TRACK_BY_RECT}
     */
    /*package*/ Rect getInitialRect() {
        if (mode != Mode.TRACK_BY_RECT) {
            throw new RuntimeException("Can only be invoked in TRACK_BY_RECT mode");
        }
        return initialRect;
    }

    /**
     * For {@link Mode#TRACK_BY_POINT} {@link TrackedObject}s, returns the initial indicated
     * {@link Point}
     * @return the initial indicated {@link Point}
     * @throws RuntimeException if the mode is not {@link Mode#TRACK_BY_POINT}
     */
    /*package*/ Point getInitialPoint() {
        if (mode != Mode.TRACK_BY_POINT) {
            throw new RuntimeException("Can only be invoked in TRACK_BY_POINT mode");
        }
        return initialPoint;
    }

    // TODO: comment
    public Position getPosition() {
        Position p = new Position();
        getPosition(p);
        return p;
    }

    // TODO: comment
    public void getPosition(Position p) {
        synchronized (lock) {
            p.position.set(position);
            p.state = state;
            p.lastState = lastState;
        }
    }

    /**
     * Request that the {@link ObjectTracker} stop tracking this object. This will move the object
     * to the {@link State#CLOSED} state (if it is not in that state already). After closing,
     * {@link ObjectTracker.EventListener}s will receive a single final notification of an event
     * for that object, after which subsequent events will not include this {@link TrackedObject}.
     */
    public void stopTracking() {
        synchronized (lock) {
            if (state == State.CLOSED) {
                return;
            }
        }

        try {
            tracker.untrackObject(this);
        } catch (ObjectTrackerNotRunningException e) {
            // The state would normally be closed if the ObjectTracker is not running, but it's
            // possible that ObjectTracker could be stopped between releasing lock and calling
            // untrackObject. We can't hold lock across the call to untrackObject, since that can
            // lead to deadlock if ObjectTracker is holding its own lock and calls into
            // TrackedObject on another thread. Just ignore the ObjectTrackerNotRunningException
            // here, since if the ObjectTracker was stopped all tracked objects will be closed
            // automatically.
        }
    }

    /**
     * Sets the tracked object identifier for this {@link TrackedObject}
     * <p><i>NOTE: this should only be invoked by the {@link ObjectTracker}, from the tracker
     * thread</i>
     * @param id Unique identifier for this {@link TrackedObject}, as obtained from an
     *      {@link ObjectTrackerBinding}
     * @throws RuntimeException if the current state is not {@link State#PENDING}
     */
    /*package*/ void setTrackedObjectId(int id) {
        if (state != State.PENDING) {
            throw new RuntimeException("Can only set tracked object ID when in pending state");
        }
        trackedObjectId = id;
    }

    /**
     * Gets the unique identifier of this {@link TrackedObject}
     * <p><i>NOTE: this should only be invoked by the {@link ObjectTracker}, from the tracker
     * thread</i>
     * @return the unique identifier previously set by {@link #setTrackedObjectId(int)}
     */
    /*package*/ int getTrackedObjectId() {
        return trackedObjectId;
    }

    /**
     * Gets the number of frames that this {@link TrackedObject} has been in {@link State#PENDING}
     * <p><i>NOTE: this should only be invoked by the {@link ObjectTracker}, from the tracker
     * thread</i>
     * @return
     */
    /*package*/ int getPendingFrameCount() {
        return pendingFrameCount;
    }

    /**
     * This method should be invoked by the {@link ObjectTracker} after each image frame, with the
     * raw tracking results for this object against that frame
     * <p><i>NOTE: this should only be invoked by the {@link ObjectTracker}, from the tracker
     * thread</i>
     * @param newState the {@link State} value produced by the {@link ObjectTracker} for this
     *      object
     * @param newPosition the position {@link Rect} produced by the {@link ObjectTracker} for this
     *      object
     * @return true if the {@link TrackedObject} state or position was changed by this event
     */
    /*package*/ boolean processTrackerEvent(State newState, Rect newPosition) {
        if (newState == null) {
            throw new IllegalArgumentException("newState cannot be null");
        } else if (newState == State.ACQUIRED && newPosition == null) {
            throw new IllegalArgumentException("newPosition cannot be null if newState==ACQUIRED");
        } else if (!validateStateTransition(newState)) {
            throw new RuntimeException("Invalid state transition for object [" + trackedObjectId +
                    "] (from [" + state + "] to [" + newState + "])");
        }

        synchronized (lock) {
            boolean dirty = false;

            if (state != newState) {
                Log.v(TAG, "Object [" + trackedObjectId + "] changed state to [" + newState +
                        "] (was [" + state + "])");
                lastState = state;
                state = newState;
                dirty = true;
            }

            if (state == State.PENDING && ++pendingFrameCount >= MAX_PENDING_FRAME_COUNT) {
                state = State.UNABLE_TO_ACQUIRE_OBJECT;
                dirty = true;
            } else if (state == State.ACQUIRED && !position.equals(newPosition)) {
                position.set(newPosition);
                dirty = true;
            } else if (state == State.CLOSED) {
                position.setEmpty();
            }

            return dirty;
        }
    }

    private boolean validateStateTransition(State newState) {
        // Always valid to stay in same state
        if (newState == state) {
            return true;
        }

        switch (state) {
        case PENDING:
            return (newState == State.UNABLE_TO_ACQUIRE_OBJECT || newState == State.ACQUIRED ||
                    newState == State.CLOSED);
        case UNABLE_TO_ACQUIRE_OBJECT:
            return (newState == State.CLOSED);
        case ACQUIRED:
            return ((newState == State.LOST || newState == State.CLOSED) &&
                    trackedObjectId != ObjectTrackerBinding.PENDING_REGISTRATION);
        case LOST:
            return ((newState == State.ACQUIRED || newState == State.CLOSED) &&
                    trackedObjectId != ObjectTrackerBinding.PENDING_REGISTRATION);
        case CLOSED:
            // No valid transitions from the CLOSED state
            return false;
        default:
            throw new RuntimeException("Unexpected state value");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        if (trackedObjectId != ObjectTrackerBinding.PENDING_REGISTRATION) {
            return Integer.toString(trackedObjectId);
        }

        switch (mode) {
        case TRACK_BY_RECT:
            return "Rect(" + initialRect.left + "," + initialRect.top + "," + initialRect.right +
                    "," + initialRect.bottom + ")";

        case TRACK_BY_POINT:
            return "Point(" + initialPoint.x + "," + initialPoint.y + ")";

        default:
            throw new RuntimeException("Unexpected mode " + mode.name());
        }
    }
}

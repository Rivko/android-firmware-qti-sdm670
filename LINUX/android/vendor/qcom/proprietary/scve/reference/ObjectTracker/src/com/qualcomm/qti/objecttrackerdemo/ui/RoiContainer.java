/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.ui;

import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.CoordinateTranslator;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttrackerdemo.R;

import android.annotation.SuppressLint;
import android.app.Fragment;
import android.graphics.PointF;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

/**
 * The {@link RoiContainer} fragment encapsulates all the logic necessary to control a set of
 * {@link RoiView}s (and their corresponding regions of interest). It collaborates closely with
 * {@link RoiView} to manage regions of interest from the {@link ObjectTracker}.
 */
public class RoiContainer extends Fragment {
    /**
     * The shape of ROI which should be used
     */
    public enum Shape {
        RECTANGLE,
        OVAL
    }

    private static final String TAG = "RoiContainer";
    private static final int MAX_ROI_COUNT = ObjectTracker.MAX_NUM_TRACKED_OBJECTS;

    private final CoordinateTranslator coordinateTranslator = new CoordinateTranslator();
    private ObjectTracker objectTracker;
    private FrameLayout roiFrame;
    private TextView roiTarget;
    private final Set<RoiView> availableRoiViews = new HashSet<RoiView>(MAX_ROI_COUNT);
    private Shape shape = Shape.RECTANGLE;

    /**
     * Initialize this {@link RoiContainer} with required attachments
     * @param tracker {@link ObjectTracker} to attach
     * @throws IllegalArgumentException if tracker is null
     * @throws RuntimeException if {@link #attachObjectTracker(ObjectTracker)} has already been
     *      invoked
     */
    public void init(ObjectTracker tracker) {
        if (tracker == null) {
            throw new IllegalArgumentException("tracker may not be null");
        } else if (coordinateTranslator == null) {
            throw new IllegalArgumentException("coordinateTranslator may not be null");
        } else if (objectTracker != null) {
            throw new RuntimeException("init already invoked");
        }

        objectTracker = tracker;
    }

    /**
     * Set the camera's coordinate space for this {@link RoiContainer}
     * @param cameraSpace a {@link CoordinateSpace} describing the properties of the camera's
     *      coordinate space
     * @throws IllegalArgumentException if cameraSpace is null
     */
    public void setCameraCoordinateSpace(CoordinateSpace cameraSpace) {
        if (cameraSpace == null) {
            throw new IllegalArgumentException("cameraSpace cannot be null");
        }
        coordinateTranslator.setSourceCoordinateSpace(cameraSpace);
    }

    /**
     * Set the ROI shape
     * @param shape a {@link Shape} for the ROI
     * @throws IllegalArgumentException if shape is null
     */
    public void setRoiShape(Shape shape) {
        if (shape == null) {
            throw new IllegalArgumentException("shape may not be null");
        }

        this.shape = shape;

        if (roiFrame != null) {
            for (int i = 0; i < roiFrame.getChildCount(); i++) {
                ((RoiView)roiFrame.getChildAt(i)).setRoiShape(shape);
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_roi_container, container, false);

        roiFrame = (FrameLayout)view.findViewById(R.id.roi_frame);
        roiFrame.addOnLayoutChangeListener(roiFrameLayoutChangeListener);
        roiFrame.setOnTouchListener(roiFrameTouchListener);

        roiTarget = (TextView)view.findViewById(R.id.roi_target);
        roiTarget.setOnTouchListener(roiTargetTouchListener);
        updateRoiTargetState();

        for (int i = 0; i < MAX_ROI_COUNT; i++) {
            RoiView roi = (RoiView)inflater.inflate(R.layout.view_roi, roiFrame, false);
            Log.v(TAG, "Created ROI [" + roi.toString() + "]");
            roi.attachToRoiContainer(RoiContainer.this);
            roi.setEnabled(false);
            roiFrame.addView(roi);
            availableRoiViews.add(roi);
        }

        setRoiShape(shape);
        updateRoiTargetState();

        return view;
    }

    /**
     * This method is provided to allow destruction of {@link RoiView}s created by this container
     * @param roi {@link RoiView} to destroy
     */
    /*package*/ void destroyRoiView(RoiView roi) {
        if (roiFrame.indexOfChild(roi) == -1) {
            return;
        }

        Log.v(TAG, "Deactivating ROI [" + roi.toString() + "]");
        roi.setEnabled(false);
        roiFrame.invalidate();
        availableRoiViews.add(roi);
        updateRoiTargetState();
    }

    /**
     * Get the {@link CoordinateTranslator}. {@link RoiView}s use this to map locations in the
     * camera frame to the display, and vice-versa.
     * @return a {@link CoordinateTranslator}
     */
    /*package*/ CoordinateTranslator getCoordinateTranslator() {
        return coordinateTranslator;
    }

    /**
     * Get the ROI target. {@link RoiView}s use this to retrieve the location that they can be
     * dropped to request destruction.
     * @return the ROI target {@link View}
     */
    /*package*/ View getRoiTarget() {
        return roiTarget;
    }

    /**
     * Get the {@link ObjectTracker} attached to this {@link RoiContainer}
     * @return the {@link ObjectTracker} attached to this {@link RoiContainer}
     */
    /*package*/ ObjectTracker getObjectTracker() {
        return objectTracker;
    }

    private void updateRoiTargetState() {
        int remaining = availableRoiViews.size();
        roiTarget.setText(Integer.toString(remaining));
        roiTarget.getBackground().setLevel(remaining);
    }

    private final View.OnTouchListener roiTargetTouchListener = new View.OnTouchListener() {
        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            int available = availableRoiViews.size();
            if (event.getActionMasked() == MotionEvent.ACTION_DOWN && available > 0) {
                Iterator<RoiView> it = availableRoiViews.iterator();
                RoiView roi = it.next();
                it.remove();

                Log.v(TAG, "Activating ROI [" + roi.toString() + "] for drag");
                roi.enableForDrag();
                roi.bringToFront();
                updateRoiTargetState();
            }

            // N.B. Always return false, even if a new RoiView was created. This will allow the
            // touch event to propagate down to the selected ROI (if any).
            return false;
        }
    };

    private final View.OnTouchListener roiFrameTouchListener = new View.OnTouchListener() {
        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            boolean handled = false;

            int available = availableRoiViews.size();
            if (event.getActionMasked() == MotionEvent.ACTION_DOWN && available > 0 &&
                    !objectTracker.isTrackObjectByPointPending()) {
                Iterator<RoiView> it = availableRoiViews.iterator();
                RoiView roi = it.next();
                it.remove();

                Log.v(TAG, "Activating ROI [" + roi.toString() + "] for touch");
                roi.enableForTrackByTouch(new PointF(event.getX(), event.getY()));
                roi.bringToFront();
                updateRoiTargetState();
                handled = true;
            }

            return handled;
        }
    };

    private final View.OnLayoutChangeListener roiFrameLayoutChangeListener =
            new View.OnLayoutChangeListener() {
        @Override
        public void onLayoutChange(View v, int left, int top, int right,
                int bottom, int oldLeft, int oldTop, int oldRight, int oldBottom) {
            coordinateTranslator.setDestinationCoordinateSpace(
                    CoordinateSpace.forDisplay(right - left, bottom - top, v.getDisplay()));
        }
    };
}

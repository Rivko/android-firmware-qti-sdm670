/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.ui;

import java.util.Map;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.CoordinateTranslator;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject.State;
import com.qualcomm.qti.objecttrackerdemo.R;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.graphics.drawable.LevelListDrawable;
import android.os.Vibrator;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View;
import android.widget.ImageView;

/**
 * The RoiView represents a tracked region of interest. It both allows the user to select a ROI and
 * displays ROI updates from the {@link ObjectTracker}
 */
public class RoiView extends ImageView {
    private static final int VIBRATION_DURATION_MS = 80;

    private static final int[] STATE_PENDING = new int[] { R.attr.state_acquiring };
    private static final int[] STATE_UNABLE_TO_ACQUIRE = new int[] { R.attr.state_acquiring, R.attr.state_acquisition_failed };
    private static final int[] STATE_TRACKING = new int[] { R.attr.state_acquired, R.attr.state_tracking };
    private static final int[] STATE_LOST = new int[] { R.attr.state_acquired };
    private static final int[] STATE_CLOSED = new int[] { };

    private final int roiTouchSlop;
    private final int roiTargetTouchSlop;
    private final Vibrator vibrator;
    private final ModifiableInsetLayerDrawable insetDrawable;
    private final CombinedOnGestureListener gestureListener;
    private final GestureDetector translateGestureDetector;
    private final ScaleGestureDetector scaleGestureDetector;
    private ObjectTracker objectTracker;
    private RoiContainer roiContainer;
    private CoordinateTranslator coordinateTranslator;
    private View roiTarget;
    private int minTranslatedSize;
    private int maxTranslatedSize;
    private int minSize;
    private int maxSize;
    private final RectF position = new RectF();
    private final RectF roiTargetPosition = new RectF();
    private TrackedObject trackedObject;
    private TrackedObject.Position trackedObjectPosition;
    private final Rect intermediateRect = new Rect(); // working Rect for intermediate calculations
    private final RectF intermediateRectF = new RectF(); // working RectF

    /**
     * Construct a {@link RoiView}. See {@link RoiView#RoiView(Context, AttributeSet, int)} for a
     * description of parameters.
     */
    public RoiView(Context context) {
        this(context, null, 0);
    }

    /**
     * Construct a {@link RoiView}. See {@link RoiView#RoiView(Context, AttributeSet, int)} for a
     * description of parameters.
     */
    public RoiView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Construct a {@link RoiView}
     * @param context Application {@link Context} in which this {@link RoiView} is being created
     * @param attrs {@link AttributeSet} describing the properties of the {@link RoiView}
     * @param defStyle Default style identifier for this {@link RoiView}
     */
    public RoiView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        vibrator = (Vibrator)context.getSystemService(Context.VIBRATOR_SERVICE);

        LevelListDrawable roiDrawable = new LevelListDrawable();
        roiDrawable.addLevel(0, 0, context.getResources().getDrawable(R.drawable.roi));
        roiDrawable.addLevel(1, 1, context.getResources().getDrawable(R.drawable.roi_oval));
        insetDrawable = new ModifiableInsetLayerDrawable(new Drawable[] { roiDrawable } );
        setImageDrawable(insetDrawable);

        gestureListener = new CombinedOnGestureListener();
        translateGestureDetector = new GestureDetector(context, gestureListener);
        scaleGestureDetector = new ScaleGestureDetector(context, gestureListener);

        Resources res = getResources();
        roiTouchSlop = res.getDimensionPixelSize(R.dimen.roi_touch_slop);
        roiTargetTouchSlop = res.getDimensionPixelSize(R.dimen.roi_container_touch_slop);
    }

    /**
     * Attach an {@link RoiView} to its parent {@link RoiContainer}
     * @param container Parent container of this {@link RoiView}
     */
    public void attachToRoiContainer(RoiContainer container) {
        if (container == null) {
            throw new IllegalArgumentException("container may not be null");
        }

        roiContainer = container;
    }

    /**
     * See {@link RoiContainer#setRoiShape(Shape)}
     * @param shape the {@link RoiContainer.Shape} to set for this ROI
     */
    /*package*/ void setRoiShape(RoiContainer.Shape shape) {
        switch (shape) {
        case RECTANGLE:
            insetDrawable.setLevel(0);
            break;

        case OVAL:
            insetDrawable.setLevel(1);
            break;

        default:
            throw new IllegalStateException("Unexpected ROI container shape");
        }
    }

    /** {@inheritDoc} */
    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);

        if (!enabled) {
            minTranslatedSize = 0;
            maxTranslatedSize = 0;
            minSize = 0;
            maxSize = 0;
            position.set(0, 0, 0, 0);
            onPositionChanged();

            resetTracking();
        }
    }

    /**
     * Enable this {@link RoiView} for tracking by dragging and dropping a ROI rect. The ROI will
     * initially be positioned beneath the {@link RoiContainer}.
     */
    /*package*/ void enableForDrag() {
        setEnabled(true);

        calculateRoiLimits();

            float centerX = roiTargetPosition.centerX();
            float centerY = roiTargetPosition.centerY();
            int defaultSize = (maxSize - minSize) / 2;
            position.set(centerX - defaultSize, centerY - defaultSize,
                    centerX + defaultSize, centerY + defaultSize);
        onPositionChanged();
        }

    /**
     * Enable this {@link RoiView} for tracking by automatically acquiring an object beneath a
     * touched point. The ROI will be initially be positioned at the touched point.
     * @param initialPos the initial position of this {@link RoiView} within its parent
     */
    /*package*/ void enableForTrackByTouch(PointF initialPos) {
        setEnabled(true);

        calculateRoiLimits();

        position.set(initialPos.x - minSize / 2f, initialPos.y - minSize / 2f,
                initialPos.x + minSize / 2f, initialPos.y + minSize / 2f);
        onPositionChanged();

        startTrackingAtTouch(initialPos);
    }

    private void calculateRoiLimits() {
        minTranslatedSize = objectTracker.getMinRoiDimension();
        position.set(0, 0, minTranslatedSize, minTranslatedSize);
        coordinateTranslator.mapRect(position, position);
        minSize = (int)Math.ceil(position.width());

        maxTranslatedSize = objectTracker.getMaxRoiDimension();
        position.set(0, 0, maxTranslatedSize, maxTranslatedSize);
        coordinateTranslator.mapRect(position, position);
        maxSize = (int)Math.floor(position.width());
    }

    private void onPositionChanged() {
        insetDrawable.setLayerInset(0, (int)position.left, (int)position.top,
                (int)(getWidth() - position.right), (int)(getHeight() - position.bottom));
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        roiTarget = roiContainer.getRoiTarget();
        objectTracker = roiContainer.getObjectTracker();
        coordinateTranslator = roiContainer.getCoordinateTranslator();

        roiTarget.addOnLayoutChangeListener(roiTargetLayoutChangeListener);
        roiTargetLayoutChangeListener.onLayoutChange(roiTarget, roiTarget.getLeft(),
                roiTarget.getTop(), roiTarget.getRight(), roiTarget.getBottom(), 0, 0, 0, 0);

        objectTracker.registerListener(objectTrackerListener);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        resetTracking();

        objectTracker.unregisterListener(objectTrackerListener);
        roiTarget.removeOnLayoutChangeListener(roiTargetLayoutChangeListener);

        roiTarget = null;
        objectTracker = null;
        coordinateTranslator = null;
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        onPositionChanged();
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (!isEnabled()) {
            return false;
        }

        translateGestureDetector.onTouchEvent(event);
        scaleGestureDetector.onTouchEvent(event);

        // N.B. we should only ever get an ACTION_UP if we had previously started a translate/scale
        if (event.getActionMasked() == MotionEvent.ACTION_UP) {
            gestureListener.onUp(event);
        }

        return (gestureListener.mode != CombinedOnGestureListener.NONE);
    }

    @Override
    public int[] onCreateDrawableState(int extraSpace) {
        final int[] drawableState = super.onCreateDrawableState(extraSpace + 2);

        switch (trackedObjectPosition != null ? trackedObjectPosition.state : State.PENDING) {
        case PENDING:
            mergeDrawableStates(drawableState, STATE_PENDING);
            break;

        case UNABLE_TO_ACQUIRE_OBJECT:
            mergeDrawableStates(drawableState, STATE_UNABLE_TO_ACQUIRE);
            break;

        case ACQUIRED:
            mergeDrawableStates(drawableState, STATE_TRACKING);
            break;

        case LOST:
            mergeDrawableStates(drawableState, STATE_LOST);
            break;

        case CLOSED:
        default:
            mergeDrawableStates(drawableState, STATE_CLOSED);
            break;
        }

        return drawableState;
    }

    private void startTrackingAtCurrentPosition() {
        CoordinateSpace cameraSpace = coordinateTranslator.getSourceCoordinateSpace();

        // Translate from on-screen coordinates to camera frame coordinates, and clip to image frame
        coordinateTranslator.mapRectInverse(position, intermediateRectF);
        intermediateRectF.roundOut(intermediateRect);
        intermediateRect.intersect(0, 0, cameraSpace.width, cameraSpace.height);

        final int width = intermediateRect.width();
        final int height = intermediateRect.height();

        // Ensure that width and height after translation are within the allowed range
        if (width < minTranslatedSize) {
            intermediateRect.right += (minTranslatedSize - width);
        } else if (width > maxTranslatedSize) {
            intermediateRect.right -= width - maxTranslatedSize;
        }

        if (height < minTranslatedSize) {
            intermediateRect.bottom += (minTranslatedSize - height);
        } else if (height > maxTranslatedSize) {
            intermediateRect.bottom -= height - maxTranslatedSize;
        }

        // Ensure that the region of interest is entirely contained within the frame
        // TODO: tracking fails if ROI intersects right or bottom edge. Suspect an engine bug.
        if (intermediateRect.left < 0) {
            intermediateRect.offset(-intermediateRect.left, 0);
        } else if (intermediateRect.right > cameraSpace.width) {
            intermediateRect.offset(cameraSpace.width - intermediateRect.right, 0);
        }

        if (intermediateRect.top < 0) {
            intermediateRect.offset(0, -intermediateRect.top);
        } else if (intermediateRect.bottom > cameraSpace.height) {
            intermediateRect.offset(0, cameraSpace.height - intermediateRect.bottom);
        }

        // Start tracking the (sanitized) region of interest
        trackedObject = objectTracker.trackObject(intermediateRect);
        refreshDrawableState();
    }

    private void startTrackingAtTouch(PointF touchPos) {
        CoordinateSpace cameraSpace = coordinateTranslator.getSourceCoordinateSpace();

        // Translate from on-screen coordinates to camera frame coordinates, and clip to image frame
        PointF intermediateTouchPos = coordinateTranslator.mapPointInverse(touchPos);

        if (intermediateTouchPos.x < 0f) {
            intermediateTouchPos.x = 0f;
        } else if (intermediateTouchPos.x >= cameraSpace.width) {
            intermediateTouchPos.x = Math.nextAfter(cameraSpace.width, Double.NEGATIVE_INFINITY);
        }

        if (intermediateTouchPos.y < 0f) {
            intermediateTouchPos.y = 0f;
        } else if (intermediateTouchPos.y >= cameraSpace.height) {
            intermediateTouchPos.y = Math.nextAfter(cameraSpace.height, Double.NEGATIVE_INFINITY);
        }

        trackedObject = objectTracker.trackObject(new Point((int)intermediateTouchPos.x,
                (int)intermediateTouchPos.y));
        refreshDrawableState();
    }

    private void resetTracking() {
        if (trackedObject != null) {
            trackedObject.stopTracking();
            trackedObject = null;
            trackedObjectPosition = null;
            refreshDrawableState();
        }
    }

    private void updateTracking(Map<TrackedObject, TrackedObject.Position> positions) {
        if (trackedObject != null) {
            trackedObjectPosition = positions.get(trackedObject);

            refreshDrawableState();

            // N.B. It's possible that a just-created tracked object has not yet been processed by
            // the object tracker, and thus no position was reported for it.
            switch (trackedObjectPosition != null ? trackedObjectPosition.state : State.PENDING) {
            case ACQUIRED:
                position.set(trackedObjectPosition.position);
                coordinateTranslator.mapRect(position, position);
                onPositionChanged();
                break;

            case CLOSED:
                roiContainer.destroyRoiView(this);
                break;

            case PENDING:
            case UNABLE_TO_ACQUIRE_OBJECT:
            case LOST:
            default:
                // Do nothing
                break;
            }
        }
    }

    private final ObjectTracker.EventListener objectTrackerListener =
            new ObjectTracker.EventListener() {
        @Override
        public void onTrackerEvent(long frameTimestamp,
                final Map<TrackedObject, TrackedObject.Position> positions) {
            post(new Runnable() {
            @Override
            public void run() {
                    updateTracking(positions);
                }
            });
            }
        };

    private final OnLayoutChangeListener roiTargetLayoutChangeListener =
            new OnLayoutChangeListener() {
        @Override
        public void onLayoutChange(View v, int left, int top, int right,
                int bottom, int oldLeft, int oldTop, int oldRight, int oldBottom) {
            roiTargetPosition.set(roiTarget.getLeft(), roiTarget.getTop(), roiTarget.getRight(),
                    roiTarget.getBottom());
            roiTargetPosition.inset(-roiTargetTouchSlop, -roiTargetTouchSlop);
        }
    };

    private class CombinedOnGestureListener extends SimpleOnGestureListener
            implements OnScaleGestureListener {
        // N.B. Can't use enums in non-static inner classes
        public static final int NONE = 0;
        public static final int TRANSLATING = 1;
        public static final int SCALING = 2;

        private int mode;
        private boolean insideRoiTarget;

        public void onUp(MotionEvent e) {
            mode = NONE;

            if (roiTargetPosition.contains(e.getX(), e.getY())) {
                roiContainer.destroyRoiView(RoiView.this);
            } else {
                startTrackingAtCurrentPosition();
            }
        }

        @Override
        public boolean onDown(MotionEvent e) {
            intermediateRectF.set(position);
            intermediateRectF.inset(-roiTouchSlop, -roiTouchSlop);
            final float touchX = e.getX();
            final float touchY = e.getY();

            if (intermediateRectF.contains(touchX, touchY)) {
                mode = TRANSLATING;
                bringToFront();
                resetTracking();
                insideRoiTarget = roiTargetPosition.contains(touchX, touchY);

                if (clampSize()) {
                    // Ensure position includes the touch point
                    if (!position.contains(touchX, touchY)) {
                        if (position.left > touchX) {
                            position.offset(touchX - position.left, 0);
                        } else if (position.right < touchX) {
                            position.offset(touchX - position.right, 0);
                        }

                        if (position.top > touchY) {
                            position.offset(0, touchY - position.top);
                        } else if (position.bottom < touchY) {
                            position.offset(0, touchY - position.bottom);
                        }
                    }

                    onPositionChanged();
                }
            }
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2,
                float distanceX, float distanceY) {
            if (mode == TRANSLATING) {
                position.offset(-distanceX, -distanceY);
                onPositionChanged();

                boolean wasInsideRoiTarget = insideRoiTarget;
                insideRoiTarget = roiTargetPosition.contains(e2.getX(), e2.getY());
                if (insideRoiTarget && !wasInsideRoiTarget) {
                    vibrator.vibrate(VIBRATION_DURATION_MS);
                }
            }
            return true;
        }

        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            if (mode == TRANSLATING) {
                mode = SCALING;
            }
            return true;
        }

        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
            if (mode == SCALING) {
                mode = TRANSLATING;
            }
        }

        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            if (mode == SCALING) {
                position.inset(
                        position.width() * (1f - detector.getCurrentSpanX() /
                        detector.getPreviousSpanX()) / 2f,
                        position.height() * (1f - detector.getCurrentSpanY() /
                        detector.getPreviousSpanY()) / 2f);
                clampSize();
                onPositionChanged();
            }
            return true;
        }

        private boolean clampSize() {
            boolean clamped = false;

            if (position.width() < minSize) {
                position.inset(-(minSize - position.width()) / 2f, 0);
                clamped = true;
            } else if (position.width() > maxSize) {
                position.inset((position.width() - maxSize) / 2f, 0);
                clamped = true;
            }

            if (position.height() < minSize) {
                position.inset(0, -(minSize - position.height()) / 2f);
                clamped = true;
            } else if (position.height() > maxSize) {
                position.inset(0, (position.height() - maxSize) / 2f);
                clamped = true;
            }

            return clamped;
        }
    }

    // N.B. Standard LayerDrawable has a bug; setLayerInset doesn't notify the child drawable
    // of a bounds change to account for the new inset. Subclass and fix.
    private static class ModifiableInsetLayerDrawable extends LayerDrawable {
        public ModifiableInsetLayerDrawable(Drawable[] layers) {
            super(layers);
        }

        @Override
        public void setLayerInset(int index, int l, int t, int r, int b) {
            super.setLayerInset(index, l, t, r, b);
            onBoundsChange(getBounds());
        }
    }
}

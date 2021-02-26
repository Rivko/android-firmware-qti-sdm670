/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import java.util.ArrayList;
import java.util.List;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.PorterDuff.Mode;
import android.graphics.PorterDuffXfermode;
import android.os.Bundle;
import android.os.Parcelable;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View;
import android.widget.ImageView;

import com.qualcomm.qti.qces.imageedit.jnilib.SoftSegmentationJNILib;
import com.qualcomm.qti.qces.imageedit.softcut.R;

/**
 * This class the is mask to track user inputs for segmentation
 */
public class MaskView extends View {

    /**
     * Classes may implement this interface to receive notifications when stroke has been started or
     * stopped
     */
    public interface OnStrokeListener {
        /**
         * This method will be invoked when a stroke is started or completed
         * startPoint: true if starting point, false if ending point
         * x, y position within the bitmap
         */
        void onStrokeStartPoint(boolean startPoint, float x, float y);
    }

    public static final int MASK_ALPHA_VIEW = 0;
    public static final int HARD_CUT_RESULT_MASK_VIEW = 1;

    private static final int ALPHA_SOFT_EDGE_COLOR = Color.argb(
            SoftSegmentationJNILib.MASK_UNKNOWN, 0, 0, 0);
    private static final int ALPHA_FOREGROUND_COLOR = Color.argb(
            SoftSegmentationJNILib.MASK_FOREGROUND, 0, 0, 0);
    private static final int ALPHA_BACKGROUND_COLOR = Color.argb(
            SoftSegmentationJNILib.MASK_BACKGROUND, 0, 0, 0);

    private static final int SOFT_EDGE_COLOR = Color.argb(90, 255, 255, 0);
    private static final int FOREGROUND_COLOR = Color.argb(90, 0, 255, 0); // Green
    private static final int BACKGROUND_COLOR = Color.argb(90, 255, 0, 0); // Red

    private static final float TOUCH_TOLERANCE = 4;

    private final Matrix displayMatrix = new Matrix();
    private final Matrix displayMatrixInverse = new Matrix();

    private OnStrokeListener strokeListener;

    private Canvas maskCanvas;
    private Bitmap maskBitmap;

    private Canvas drawCanvas;
    private Bitmap drawBitmap;

    private Paint foregroundPaint;
    private Paint backgroundPaint;
    private Paint softEdgesPaint;

    private Paint foregroundPaintColor;
    private Paint backgroundPaintColor;
    private Paint softEdgesPaintColor;

    private float mattingStrokeWidth;

    private SegmentationState segmentationState = SegmentationState.FOREGROUND;
    private ImageView[] companionViews;

    private Stroke stroke;
    private ArrayList<Stroke> strokes;
    private ArrayList<Stroke> undoStrokes;

    /*
     * Member variables for Zoom & Pan operations
     */
    // public ImageView imageView;
    private final float maxZoomFactor = 10f;
    private GestureDetector translateGestureDetector;
    private ScaleGestureDetector scaleGestureDetector;
    private int viewportWidth;
    private int viewportHeight;
    private int contentWidth;
    private int contentHeight;
    private float minScale;
    private float maxScale;
    private float centerX;
    private float centerY;

    private float scale;
    private float initialScale = 0;

    @Override
    public Parcelable onSaveInstanceState() {

        Bundle bundle = new Bundle();
        bundle.putParcelable("instanceState", super.onSaveInstanceState());
        // ... save everything
        bundle.putParcelableArrayList("arraylist_strokes", strokes);
        bundle.putParcelableArrayList("arraylist_undostrokes", undoStrokes);
        bundle.putSerializable("segmentation_state", segmentationState);
        return bundle;
    }

    @Override
    public void onRestoreInstanceState(Parcelable state) {

        if (state instanceof Bundle) {
            Bundle bundle = (Bundle) state;
            // ... load everything
            this.strokes = bundle.getParcelableArrayList("arraylist_strokes");
            this.undoStrokes = bundle.getParcelableArrayList("arraylist_undostrokes");
            this.segmentationState = (SegmentationState) bundle
                    .getSerializable("segmentation_state");

            state = bundle.getParcelable("instanceState");
        }
        super.onRestoreInstanceState(state);
    }

    public boolean didUserZoom(){
        if (scale == initialScale)
            return false;
        else
            return true;
    }

    /**
     * Create a MaskView
     *
     * @param context The application context
     */
    public MaskView(Context context) {
        this(context, null, 0);
    }

    /**
     * Create a MaskView
     *
     * @param context      The application context
     * @param attributeSet Attribute set to grab custom attributes
     */
    public MaskView(Context context, AttributeSet attributeSet) {
        this(context, attributeSet, 0);
    }

    /**
     * Create a MaskView
     *
     * @param context      the application context
     * @param attributeSet {@link AttributeSet} from which to load attributes
     * @param defStyle     Style identifier
     */
    public MaskView(Context context, AttributeSet attributeSet, int defStyle) {
        super(context, attributeSet, defStyle);

        // spec says default is medium stroke
        mattingStrokeWidth = getResources().getDimension(R.dimen.stroke_width_medium);

        strokes = new ArrayList<Stroke>();
        undoStrokes = new ArrayList<Stroke>();

        initializeForegroundPaint();
        initializeBackgroundPaint();
        initializeSoftEdgesPaint();
        initializeForegroundPaintColor();
        initializeBackgroundPaintColor();
        initializeSoftEdgesPaintColor();

        translateGestureDetector = new GestureDetector(getContext(), translateGestureListener);
        scaleGestureDetector = new ScaleGestureDetector(getContext(), scaleGestureListener);
        setOnTouchListener(touchListener);

    }

    /*
     * This paint is for the non-visible mask
     */
    private void initializeSoftEdgesPaint() {
        softEdgesPaint = new Paint();
        softEdgesPaint.setColor(ALPHA_SOFT_EDGE_COLOR);
        softEdgesPaint.setStyle(Paint.Style.STROKE);
        softEdgesPaint.setStrokeJoin(Paint.Join.ROUND);
        softEdgesPaint.setStrokeCap(Paint.Cap.ROUND);
        softEdgesPaint.setXfermode(new PorterDuffXfermode(Mode.SRC));
    }

    /*
     * This paint is for the non-visible mask
     */
    private void initializeForegroundPaint() {
        foregroundPaint = new Paint();
        foregroundPaint.setColor(ALPHA_FOREGROUND_COLOR);
        foregroundPaint.setStyle(Paint.Style.STROKE);
        foregroundPaint.setStrokeJoin(Paint.Join.ROUND);
        foregroundPaint.setStrokeCap(Paint.Cap.ROUND);
        foregroundPaint.setXfermode(new PorterDuffXfermode(Mode.SRC));
    }

    /*
     * This paint is for the non-visible mask
     */
    private void initializeBackgroundPaint() {
        backgroundPaint = new Paint();
        backgroundPaint.setColor(ALPHA_BACKGROUND_COLOR);
        backgroundPaint.setStyle(Paint.Style.STROKE);
        backgroundPaint.setStrokeJoin(Paint.Join.ROUND);
        backgroundPaint.setStrokeCap(Paint.Cap.ROUND);
        backgroundPaint.setXfermode(new PorterDuffXfermode(Mode.SRC));
    }

    /*
     * This paint is for the visible canvas
     */
    private void initializeSoftEdgesPaintColor() {
        softEdgesPaintColor = new Paint(Paint.ANTI_ALIAS_FLAG);
        softEdgesPaintColor.setColor(SOFT_EDGE_COLOR);
        softEdgesPaintColor.setStyle(Paint.Style.STROKE);
        softEdgesPaintColor.setStrokeJoin(Paint.Join.ROUND);
        softEdgesPaintColor.setStrokeCap(Paint.Cap.ROUND);
    }

    /*
     * This paint is for the visible canvas
     */
    private void initializeForegroundPaintColor() {
        foregroundPaintColor = new Paint(Paint.ANTI_ALIAS_FLAG);
        foregroundPaintColor.setColor(FOREGROUND_COLOR);
        foregroundPaintColor.setStyle(Paint.Style.STROKE);
        foregroundPaintColor.setStrokeJoin(Paint.Join.ROUND);
        foregroundPaintColor.setStrokeCap(Paint.Cap.ROUND);

    }

    /*
     * This paint is for the visible canvas
     */
    private void initializeBackgroundPaintColor() {
        backgroundPaintColor = new Paint(Paint.ANTI_ALIAS_FLAG);
        backgroundPaintColor.setColor(BACKGROUND_COLOR);
        backgroundPaintColor.setStyle(Paint.Style.STROKE);
        backgroundPaintColor.setStrokeJoin(Paint.Join.ROUND);
        backgroundPaintColor.setStrokeCap(Paint.Cap.ROUND);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        updateDisplayMatrix();
    }

    // Calculate the transformation from display coordinates to mask coordinates
    private void updateDisplayMatrix() {
        if (maskBitmap == null) {
            viewportWidth = 0;
            viewportHeight = 0;
            displayMatrix.reset();
            displayMatrixInverse.reset();
            return;
        }

        if (viewportWidth == getWidth() && viewportHeight == getHeight() &&
                contentWidth == maskBitmap.getWidth() && contentHeight == maskBitmap.getHeight()) {
            // if width x height dont change then do not call setContentSize i.e do not reset the
            // scale and translation to default, it should reset only when orientation changes
            return;
        }
        viewportWidth = getWidth();
        viewportHeight = getHeight();

        setContentSize(maskBitmap.getWidth(), maskBitmap.getHeight());
    }

    /**
     * Set the mask bitmap. This bitmap is what the canvas will build upon.
     *
     * @param maskBitmap The mask bitmap you want the mask to wrap
     */
    void setMaskBitmap(Bitmap maskBitmap) {
        this.maskBitmap = maskBitmap;

        maskCanvas = new Canvas();
        drawCanvas = new Canvas();

        if (maskBitmap != null) {
            //Util.printBitmap("maskBitmap on init", maskBitmap);

            drawBitmap = Bitmap.createBitmap(maskBitmap.getWidth(), maskBitmap.getHeight(),
                    Bitmap.Config.ARGB_8888);
            drawCanvas.setBitmap(drawBitmap);

            maskCanvas.setBitmap(maskBitmap);

        }
        updateDisplayMatrix();
        invalidate();
    }

    /**
     * Handle the drawing of the paths
     *
     * @param canvas The canvas that the paths will be drawn on
     */
    @Override
    protected void onDraw(Canvas c) {
        super.onDraw(c);

        if (drawBitmap != null) {

            // we must clear the drawBitmap on each pass otherwise translucent strokes stack up
            drawBitmap.eraseColor(Color.TRANSPARENT);

            int saveCount = c.save();
            c.concat(displayMatrix);

            drawStrokes(drawCanvas, true);

            // draw the drawBitmap to the real canvas
            c.drawBitmap(drawBitmap, 0, 0, null);

            c.restoreToCount(saveCount);
        }

    }

    private void drawStrokes(Canvas canvas, boolean forDisplay) {
        if (strokes != null && !strokes.isEmpty()) {
            List<Stroke> paintStrokes = strokes;

            // If rendering to display and state is SOFT_EDGES, only paint an
            // in-progress stroke
            if (forDisplay && segmentationState == SegmentationState.SOFT_EDGES) {
                final int count = paintStrokes.size();
                paintStrokes = paintStrokes.subList((count >= 1 && stroke != null ? count - 1
                        : count), count);
            }

            for (Stroke stroke : paintStrokes) {
                Paint p = (forDisplay ? getDisplayPaint(stroke.getType()) : getMaskPaint(stroke
                        .getType()));
                p.setStrokeWidth(stroke.getWidth());

                canvas.drawPath(stroke.getPath(), p);


            }
        }

    }

    /**
     * Attach a set of companion {@link View}s to this {@link MaskView}. When this view is zoomed,
     * the companion views will be zoomed by an equal amount
     *
     * @param views a set of {@link View}s to zoom in conjunction with this {@link View}
     */
    void attachCompanionViews(ImageView... views) {
        companionViews = views;
    }

    /*
     * Below logic is for Zoom and Pan
     */

    /**
     * Sets the size of the content. This will set an initial scale & translation such that the
     * entire contents fit within the {@link View}.
     *
     * @param width  Width of the {@link View}s content, in px
     * @param height Height of the {@link View}s content, in px
     * @throws IllegalArgumentException if width or height are < 0
     */
    public void setContentSize(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be non-negative");
        }
        contentWidth = width;
        contentHeight = height;
        resetTranslateScaleState();
    }

    /**
     * Get the current X translation of the content.
     * <p/>
     * <i>NOTE: this translation value applies before scaling</i>
     *
     * @return the current X translation
     */
    public float getTranslationX() {
        return centerX - viewportWidth / scale / 2f;
    }

    /**
     * Get the current Y translation of the content.
     * <p/>
     * <i>NOTE: this translation value applies before scaling</i>
     *
     * @return the current Y translation
     */
    public float getTranslationY() {
        return centerY - viewportHeight / scale / 2f;
    }

    /**
     * Get the current scale of the content
     *
     * @return the current scale
     */
    public float getScale() {
        return scale;
    }

    private void resetTranslateScaleState() {
        if (contentWidth != 0 && contentHeight != 0 && viewportWidth != 0 && viewportHeight != 0) {
            minScale = Math.min((float) viewportWidth / (float) contentWidth, (float) viewportHeight /
                    (float) contentHeight);
        } else {
            minScale = 1f;
        }

        maxScale = minScale * maxZoomFactor;
        applyTranslateScale(contentWidth / 2f, contentHeight / 2f, minScale);
    }

    private void applyTranslateScale(float x, float y, float s) {
        if (s < minScale) {
            s = minScale;
        } else if (s > maxScale) {
            s = maxScale;
        }
        scale = s;
        if (initialScale == 0)
            initialScale = s;

        x = clampOffsetToValidRange(x, viewportWidth, contentWidth, scale);
        y = clampOffsetToValidRange(y, viewportHeight, contentHeight, scale);

        centerX = x;
        centerY = y;

        onTranslateScaleChanged(getTranslationX(), getTranslationY(), scale);
    }

    private static float clampOffsetToValidRange(float offset, int viewportSize, int contentSize,
                                                 float scale) {
        float minOffset = Math.min(contentSize, viewportSize / scale) / 2f;
        if (offset < minOffset) {
            return minOffset;
        } else if (offset > contentSize - minOffset) {
            return contentSize - minOffset;
        } else {
            return offset;
        }
    }

    protected void onTranslateScaleChanged(float x, float y, float scale) {
        displayMatrix.reset();
        displayMatrixInverse.reset();
        displayMatrix.postTranslate(-x, -y);
        displayMatrix.postScale(scale, scale);
        displayMatrix.invert(displayMatrixInverse);

        if (companionViews != null) {
            for (ImageView v : companionViews) {
                v.setImageMatrix(displayMatrix);
            }
        }

        invalidate();
    }

    private final OnTouchListener touchListener = new OnTouchListener() {
        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            translateGestureDetector.onTouchEvent(event);
            scaleGestureDetector.onTouchEvent(event);
            if (event.getActionMasked() == MotionEvent.ACTION_UP) {
                onUp(event);
            }
            return true;
        }
    };

    private static final int MODE_NONE = 0;
    private static final int MODE_TRANSLATE = 1;
    private static final int MODE_SCALE = 2;
    private static final int MODE_PAN = 3;
    private int mode;

    private final PointF lastScrollPosition = new PointF();
    private final float[] pos = new float[2];

    public void onUp(MotionEvent e) {
        if (mode == MODE_TRANSLATE) {
            updatePos(e);
            endStroke(pos[0], pos[1]);
        }
        mode = MODE_NONE;
    }

    private void updatePos(MotionEvent e) {
        lastScrollPosition.set(e.getX(), e.getY());
        pos[0] = lastScrollPosition.x;
        pos[1] = lastScrollPosition.y;
        displayMatrixInverse.mapPoints(pos);
    }

    private final OnGestureListener translateGestureListener = new SimpleOnGestureListener() {

        @Override
        public boolean onDown(MotionEvent e) {
            setPressed(true);
            mode = MODE_NONE;
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (e2.getPointerCount() >= 2) {
                if (stroke != null) {
                    // remove any in-progress 1-finger stroke if the user 2-finger zooms
                    strokes.remove(strokes.size() - 1);
                    stroke = null;
                }
                mode = MODE_PAN;
                final float scale = getScale();
                applyTranslateScale(centerX + distanceX / scale, centerY + distanceY / scale, scale);
            } else if (mode == MODE_NONE && e2.getPointerCount() == 1) {
                mode = MODE_TRANSLATE;
                updatePos(e1);
                startStroke(pos[0], pos[1]);
            }

            if (mode == MODE_TRANSLATE) {
                pos[0] = e2.getX();
                pos[1] = e2.getY();
                float dx = pos[0] - lastScrollPosition.x;
                float dy = pos[1] - lastScrollPosition.y;
                if (Math.sqrt(dx * dx + dy * dy) >= TOUCH_TOLERANCE) {
                    displayMatrixInverse.mapPoints(pos);
                    strokeTo(pos[0], pos[1]);
                }
                // invalidate();
            }
            return true;
        }

        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            setPressed(false);
            performClick();
            return true;
        }
    };

    private final OnScaleGestureListener scaleGestureListener = new OnScaleGestureListener() {
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            return true;
        }

        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
        }

        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            float sf = detector.getScaleFactor();
            float focusX = detector.getFocusX() - viewportWidth / 2f;
            float focusY = detector.getFocusY() - viewportHeight / 2f;
            applyTranslateScale(centerX + (sf - 1f) * focusX / scale, centerY + (sf - 1f) * focusY /
                    scale, scale * sf);
            return true;
        }
    };

    // End of logic for zoom and pan


    private void startStroke(float x, float y) {
        int strokeType;
        float strokeWidth;

        if (strokeListener != null)
            strokeListener.onStrokeStartPoint(true, x, y);

        switch (segmentationState) {
            case FOREGROUND:
                strokeType = Stroke.FOREGROUND;
                strokeWidth = getResources().getDimension(R.dimen.stroke_width_default);
                break;

            case BACKGROUND:
                strokeType = Stroke.BACKGROUND;
                strokeWidth = getResources().getDimension(R.dimen.stroke_width_default);
                break;

            case SOFT_EDGES:
                strokeType = Stroke.SOFT_EDGE;
                strokeWidth = mattingStrokeWidth;
                break;

            default:
                throw new RuntimeException("Unexpected state [" + segmentationState + "]");
        }

        float width = strokeWidth / getScale();

        if (width < 1f) {
            width = 1f;
        }
        stroke = new Stroke(strokeType, width);
        stroke.pathMoveTo(x, y);
        strokes.add(stroke);

        invalidate();
    }

    private void strokeTo(float x, float y) {
        stroke.pathLineTo(x, y);
        invalidate();
    }

    private void endStroke(float x, float y) {
        // set the stroke into the mask canvas
        Paint p = getMaskPaint(stroke.getType());
        p.setStrokeWidth(stroke.getWidth());
        maskCanvas.drawPath(stroke.getPath(), p);

        invalidate();

        //stroke = null;

        //Util.printBitmap("maskBitmap after endStroke", maskBitmap);

        if (strokeListener != null)
            strokeListener.onStrokeStartPoint(false, x, y);

        stroke = null;

    }

    public void redrawCurrentStrokeType(int strokeType) {

        // set the stroke into the mask canvas
        Paint p = getMaskPaint(strokeType);
        p.setStrokeWidth(stroke.getWidth());
        maskCanvas.drawPath(stroke.getPath(), p);
    }

    private Paint getDisplayPaint(int strokeType) {

        switch (strokeType) {
            case Stroke.FOREGROUND:
                return foregroundPaintColor;

            case Stroke.BACKGROUND:
                return backgroundPaintColor;

            case Stroke.SOFT_EDGE:
                return softEdgesPaintColor;

            default:
                throw new RuntimeException("Unexpected stroke type [" + strokeType + "]");
        }

    }

    private Paint getMaskPaint(int strokeType) {
        switch (strokeType) {
            case Stroke.FOREGROUND:
                return foregroundPaint;

            case Stroke.BACKGROUND:
                return backgroundPaint;

            case Stroke.SOFT_EDGE:
                return softEdgesPaint;

            default:
                throw new RuntimeException("Unexpected stroke type [" + strokeType + "]");
        }
    }

    void setStrokeListener(OnStrokeListener listener) {
        this.strokeListener = listener;
    }

    /**
     * Set the current state of segmentation
     *
     * @param segmentationState The current state of segmentation FOREGROUND/BACKGROUND/SOFT_EDGES
     */
    void setState(SegmentationState segmentationState) {
        this.segmentationState = segmentationState;
    }

    /**
     * Get the current state of segmentation
     *
     * @return segmentationState The current state of segmentation FOREGROUND/BACKGROUND/SOFT_EDGES
     */
    SegmentationState getSegmentationState() {
        return this.segmentationState;
    }

    void clearSegmentationStrokes() {
        strokes.clear();
        invalidate();
    }

    void redrawStrokes() {

        if (maskCanvas != null) {
            drawStrokes(maskCanvas, false);
            drawStrokes(drawCanvas, true);

            if (segmentationState != SegmentationState.SOFT_EDGES)
                invalidate();
        }

    }

    void removeLastStroke() {
        undoStrokes.add(strokes.get(strokes.size() - 1));
        strokes.remove(strokes.size() - 1);
        redrawStrokes();
    }

    void putBackLastUndoStroke() {
        strokes.add(undoStrokes.get(undoStrokes.size() - 1));
        undoStrokes.remove(undoStrokes.size() - 1);
        redrawStrokes();
    }

    int getBackgroundStrokesCount() {
        int backgroundStrokeCount = 0;
        for (Stroke stroke : strokes) {
            if (stroke.getType() == Stroke.BACKGROUND) {
                backgroundStrokeCount++;
            }
        }
        return backgroundStrokeCount;
    }

    int getForegroundStrokesCount() {
        int foregroundStrokeCount = 0;
        for (Stroke stroke : strokes) {
            if (stroke.getType() == Stroke.FOREGROUND) {
                foregroundStrokeCount++;
            }
        }
        return foregroundStrokeCount;
    }

    void setMattingStrokeSize(float strokeWidth) {
        this.mattingStrokeWidth = strokeWidth;
    }

    int getStrokeCount() {
        return strokes.size();
    }

    int getUndoStrokeCount() {
        return undoStrokes.size();
    }

    void clearUndoStrokes() {
        undoStrokes.clear();
    }

    int peekUndoStrokesSegmentationState() {
        int topUndoStrokeSegmentationState = -1;

        if (getUndoStrokeCount() > 0) {
            topUndoStrokeSegmentationState = undoStrokes.get(getUndoStrokeCount() - 1).getType();
        }
        return topUndoStrokeSegmentationState;
    }

    int peekStrokesSegmentationState() {
        int topStrokeSegmentationState = -1;

        if (getStrokeCount() > 0) {
            topStrokeSegmentationState = strokes.get(getStrokeCount() - 1).getType();
        }
        return topStrokeSegmentationState;
    }

    /*
     * This function is called when done is clicked at enhance edges, it manually adds a
     * small(invisible) stroke if no strokes are drawn before clicking done, to mask the below bug
     * this should be removed later after fixing the problem in right way.
     */

    /*
     * BUG TITLE : softcut done without making any softcut stroke results in mask-only image when
     * effects activity comes up.
     */
    public void doneClicked() {
        if (getStrokeCount() == 0 && segmentationState == SegmentationState.SOFT_EDGES) {
            float pos[] = new float[2];
            pos[0] = viewportWidth / 2;
            pos[1] = viewportHeight / 2;
            displayMatrixInverse.mapPoints(pos);
            startStroke(pos[0], pos[1]);

            pos[0] = pos[0] + 0.1f;
            pos[1] = pos[1] + 0.1f;
            displayMatrixInverse.mapPoints(pos);

            strokeTo(pos[0], pos[1]);
            endStroke(0, 0);
        }
    }
}

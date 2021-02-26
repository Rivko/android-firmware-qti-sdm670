/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import java.util.ArrayList;
import java.util.List;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PathMeasure;
import android.graphics.PointF;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View;
import android.widget.ImageView;

import com.qualcomm.qti.qces.imageedit.softcut.R;

/**
 * This class the is mask to track user inputs for segmentation
 */
public class RemovalMaskView extends View {

    private static final String TAG = RemovalMaskView.class.getSimpleName();

    /**
     * Classes may implement this interface to receive notifications when the mask being operated on
     * by this {@link RemovalMaskView} has been updated
     */
    public interface OnMaskUpdatedListener {
        /**
         * This method will be invoked when the mask is updated
         * @param stroke
         */
        void onMaskUpdated(RemovalStroke stroke);

    }

    public static final int REMOVAL_ALPHA_VIEW = 0;
    public static final int REMOVAL_RESULT_MASK_VIEW = 1;
    private final int MIN_PATH_LENGTH = 25;

    private static final float TOUCH_TOLERANCE = 4;

    private final Matrix displayMatrix = new Matrix();
    private final Matrix displayMatrixInverse = new Matrix();

    private OnMaskUpdatedListener listener;
    private Canvas canvas;
    private Bitmap bitmap;

    private Paint removalPaintColor;
    private float removalStrokeWidth;

    private ImageView[] companionViews;

    private RemovalStroke stroke;

    private ArrayList<RemovalStroke> strokes;

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
    Matrix globalMatrix;

    /**
     * Create a MaskView
     * @param context The application context
     */
    public RemovalMaskView(Context context) {
        this(context, null, 0);
    }

    /**
     * Create a MaskView
     * @param context The application context
     * @param attributeSet Attribute set to grab custom attributes
     */
    public RemovalMaskView(Context context, AttributeSet attributeSet) {
        this(context, attributeSet, 0);
    }

    /**
     * Create a MaskView
     * @param context the application context
     * @param attributeSet {@link AttributeSet} from which to load attributes
     * @param defStyle Style identifier
     */
    @SuppressLint("ClickableViewAccessibility")
    public RemovalMaskView(Context context, AttributeSet attributeSet, int defStyle) {
        super(context, attributeSet, defStyle);
        // default is medium stroke
        removalStrokeWidth = getResources().getDimension(R.dimen.stroke_width_medium);

        strokes = new ArrayList<RemovalStroke>();

        initializeRemovalPaintColor();

        translateGestureDetector = new GestureDetector(getContext(), translateGestureListener);
        scaleGestureDetector = new ScaleGestureDetector(getContext(), scaleGestureListener);

        setOnTouchListener(touchListener);

    }

    private void initializeRemovalPaintColor() {
        removalPaintColor = new Paint(Paint.ANTI_ALIAS_FLAG);
        removalPaintColor.setColor(getResources().getColor(R.color.removal_color));
        removalPaintColor.setStyle(Paint.Style.STROKE);
        removalPaintColor.setStrokeJoin(Paint.Join.ROUND);
        removalPaintColor.setStrokeCap(Paint.Cap.ROUND);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        updateDisplayMatrix();
    }

    // Calculate the transformation from display coordinates to mask coordinates
    private void updateDisplayMatrix() {
        if (bitmap == null) {
            viewportWidth = 0;
            viewportHeight = 0;
            displayMatrix.reset();
            displayMatrixInverse.reset();
            return;
        }

        if (viewportWidth == getWidth() && viewportHeight == getHeight() &&
                contentWidth == bitmap.getWidth() && contentHeight == bitmap.getHeight()) {
            // if width x height dont change then do not call setContentSize i.e do not reset the
            // scale and translation to default, it should reset only when orientation changes
            return;
        }
        viewportWidth = getWidth();
        viewportHeight = getHeight();

        setContentSize(bitmap.getWidth(), bitmap.getHeight());
    }

    /**
     * Handle the drawing of the paths
     * @param canvas The canvas that the paths will be drawn on
     */
    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (bitmap != null) {
            int saveCount = canvas.save();
            canvas.concat(displayMatrix);
            drawStrokes(canvas);
            canvas.restoreToCount(saveCount);
        }
    }

    private void drawStrokes(Canvas canvas) {
        if (strokes != null && !strokes.isEmpty()) {
            List<RemovalStroke> paintStrokes = strokes;

            // only paint an in-progress stroke
            final int count = paintStrokes.size();
            paintStrokes = paintStrokes.subList((count >= 1 && stroke != null ? count - 1 : count),
                    count);

            for (RemovalStroke stroke : paintStrokes) {
                Paint p = removalPaintColor;
                p.setStrokeWidth(stroke.getWidth());
                canvas.drawPath(stroke.getPath(), p);
            }
        }

    }

    /**
     * Attach a set of companion {@link View}s to this {@link RemovalMaskView}. When this view is
     * zoomed, the companion views will be zoomed by an equal amount
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
     * @param width Width of the {@link View}s content, in px
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
     * <p>
     * <i>NOTE: this translation value applies before scaling</i>
     * @return the current X translation
     */
    public float getTranslationX() {
        return centerX - viewportWidth / scale / 2f;
    }

    /**
     * Get the current Y translation of the content.
     * <p>
     * <i>NOTE: this translation value applies before scaling</i>
     * @return the current Y translation
     */
    public float getTranslationY() {
        return centerY - viewportHeight / scale / 2f;
    }

    /**
     * Get the current scale of the content
     * @return the current scale
     */
    public float getScale() {
        return scale;
    }

    private void resetTranslateScaleState() {
        if (contentWidth != 0 && contentHeight != 0 && viewportWidth != 0 && viewportHeight != 0) {
            minScale = Math.min((float)viewportWidth / (float)contentWidth, (float)viewportHeight /
                    (float)contentHeight);
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

    public void onUp(MotionEvent e) {
        if (mode == MODE_TRANSLATE) {
            endStroke();
        }
        mode = MODE_NONE;
    }

    private final OnGestureListener translateGestureListener = new SimpleOnGestureListener() {

        private final PointF lastScrollPosition = new PointF();
        private final float[] pos = new float[2];

        @Override
        public boolean onDown(MotionEvent e) {
            setPressed(true);
            mode = MODE_NONE;
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (e2.getPointerCount() >= 2) {
                clearStroke();
                mode = MODE_PAN;
                final float scale = getScale();
                applyTranslateScale(centerX + distanceX / scale, centerY + distanceY / scale, scale);
            } else if (mode == MODE_NONE && e2.getPointerCount() == 1) {
                mode = MODE_TRANSLATE;
                lastScrollPosition.set(e1.getX(), e1.getY());
                pos[0] = lastScrollPosition.x;
                pos[1] = lastScrollPosition.y;
                displayMatrixInverse.mapPoints(pos);
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

        float width = removalStrokeWidth / getScale();

        if (width < 1f) {
            width = 1f;
        }
        stroke = new RemovalStroke(width);
        stroke.pathMoveTo(x, y);
        strokes.add(stroke);

        invalidate();
    }

    private void strokeTo(float x, float y) {

        stroke.pathLineTo(x, y);
        invalidate();

    }

    public RemovalStroke getStroke() {
        return stroke;
    }

    private void endStroke() {

        // we erase the path if it is too small
        PathMeasure pm = new PathMeasure(stroke.getPath(), false);
        if (pm.getLength() < MIN_PATH_LENGTH) {
            deleteLastStroke();
        } else {
            // otherwise proceed by updating the screen
            Paint p = removalPaintColor;
            p.setStrokeWidth(stroke.getWidth());
            canvas.drawPath(stroke.getPath(), p);
            invalidate();

            if (listener != null) {
                listener.onMaskUpdated(stroke);
            }
        }
    }

    void setOnMaskUpdatedListener(OnMaskUpdatedListener listener) {
        this.listener = listener;
    }

    /**
     * Set the mask bitmap. This bitmap is what the canvas will build upon.
     * @param maskBitmap The mask bitmap you want the mask to wrap
     */
    void setMaskBitmap(Bitmap maskBitmap) {
        this.bitmap = maskBitmap;
        canvas = new Canvas();
        if (bitmap != null) {
            canvas.setBitmap(bitmap);
        }
        updateDisplayMatrix();
        invalidate();
    }

    void redrawStrokes() {
        if (canvas != null) {
            drawStrokes(canvas);
        }
    }

    void setMattingStrokeSize(float strokeWidth) {
        this.removalStrokeWidth = strokeWidth;
    }

    int getStrokeCount() {
        return strokes.size();
    }

    void clearStroke() {
        stroke = null;
        invalidate();
    }

    public void deleteLastStroke() {
        strokes.remove(strokes.size() - 1);
        clearStroke();
    }

}

/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import java.util.ArrayList;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.GradientDrawable.Orientation;
import android.util.Log;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.CanvasView;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.RecognizedRect;
import com.qualcomm.qti.sta.data.Utils;

/**
 * Drawable to show the background with OCR Bitmap
 */
class ResultImageDrawable extends Drawable {
    private static final String LOG_TAG = "ResultImageDrawable";

    private class FeatheredRect {

        private final GradientDrawable[] corner = new GradientDrawable[4];
        private final GradientDrawable[] edge = new GradientDrawable[4];

        private FeatheredRect() {
            for (int i = 0; i < corner.length; i++) {
                corner[i] = new GradientDrawable(Orientation.TL_BR, new int[] { Color.TRANSPARENT,
                        Color.BLACK, Color.TRANSPARENT });
                corner[i].setGradientType(GradientDrawable.RADIAL_GRADIENT);
            }
            edge[0] = new GradientDrawable(Orientation.BOTTOM_TOP, new int[] { Color.BLACK,
                    Color.TRANSPARENT });
            edge[1] = new GradientDrawable(Orientation.RIGHT_LEFT, new int[] { Color.BLACK,
                    Color.TRANSPARENT });
            edge[2] = new GradientDrawable(Orientation.LEFT_RIGHT, new int[] { Color.BLACK,
                    Color.TRANSPARENT });
            edge[3] = new GradientDrawable(Orientation.TOP_BOTTOM, new int[] { Color.BLACK,
                    Color.TRANSPARENT });
        }

        private void initialize(float left, float top, float right, float bottom, float radius) {
            // corners
            RectF cornerLocationRect = new RectF(left - radius, top - radius, right - radius,
                    bottom - radius);
            addCircularGradient(0, cornerLocationRect.left, cornerLocationRect.top, 1.0f, 1.0f,
                    radius);
            addCircularGradient(1, cornerLocationRect.right, cornerLocationRect.top, 0.0f, 1.0f,
                    radius);
            addCircularGradient(2, cornerLocationRect.left, cornerLocationRect.bottom, 1.0f, 0.0f,
                    radius);
            addCircularGradient(3, cornerLocationRect.right, cornerLocationRect.bottom, 0.0f, 0.0f,
                    radius);

            // top edge
            float edgeLeft = left + radius;
            float edgeTop = top - radius;
            float edgeRight = right - radius;
            float edgeBottom = top;
            edge[0].setBounds((int) edgeLeft, (int) edgeTop, (int) edgeRight, (int) edgeBottom);

            // left edge
            edgeLeft = left - radius;
            edgeTop = top + radius;
            edgeRight = left;
            edgeBottom = bottom - radius;
            edge[1].setBounds((int) edgeLeft, (int) edgeTop, (int) edgeRight, (int) edgeBottom);

            // right edge
            edgeLeft = right;
            edgeTop = top + radius;
            edgeRight = right + radius;
            edgeBottom = bottom - radius;
            edge[2].setBounds((int) edgeLeft, (int) edgeTop, (int) edgeRight, (int) edgeBottom);

            // bottom edge
            edgeLeft = left + radius;
            edgeTop = bottom;
            edgeRight = right - radius;
            edgeBottom = bottom + radius;
            edge[3].setBounds((int) edgeLeft, (int) edgeTop, (int) edgeRight, (int) edgeBottom);
        }

        private void addCircularGradient(int index, float cornerLeft, float cornerTop,
                float anchorX, float anchorY, float radius) {
            float cornerRight = cornerLeft + radius * 2;// half circle only
            float cornerBottom = cornerTop + radius * 2;// half circle only

            corner[index].setGradientRadius(radius * 2);
            corner[index].setGradientCenter(anchorX, anchorY);
            corner[index].setBounds((int) cornerLeft, (int) cornerTop, (int) cornerRight,
                    (int) cornerBottom);
        }

        private void draw(Canvas canvas) {
            for (int i = 0; i < corner.length; i++) {
                corner[i].draw(canvas);
            }
            for (int i = 0; i < edge.length; i++) {
                edge[i].draw(canvas);
            }
        }
    }

    private class InitialLoader {
        private float atX;
        private float atY;
        private float scale;

        private void set(float scale, float x, float y) {
            this.scale = scale;
            atX = x;
            atY = y;
        }

        private void draw(Canvas canvas) {
            canvas.translate(atX, atY);
            canvas.scale(scale, scale);
            canvas.drawBitmap(markedAreasImage, 0, 0, imagePaint);
        }
    }

    private InitialLoader initialLoader = new InitialLoader();

    private final Bitmap markedAreasImage;

    private final Paint imagePaint;

    private boolean haveOpacity = false;
    private int opacity = 0;

    private FeatheredRect featheredRect = new FeatheredRect();
    private final CanvasView canvasView;

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param bitmap
     *            snapshot from ROI which needs to be shown as background image
     * @param dataList
     *            parsed out data list
     * @param dataId
     *            current data id
     * @param canvasView
     *            image canvas view
     * @param imageScale
     *            original image scale
     */
    ResultImageDrawable(Context context, Bitmap bitmap, ArrayList<RecognizedData> dataList,
            int dataId, CanvasView canvasView, float imageScale) {
        this.canvasView = canvasView;
        markedAreasImage = bitmap;

        int dimmedColor = context.getResources().getColor(R.color.ocr_result_dimmed);
        float maxRoundRadius = context.getResources().getDimension(
                R.dimen.max_recognized_rectangle_round_radius);
        float minRoundRadius = context.getResources().getDimension(
                R.dimen.min_recognized_rectangle_round_radius);
        float rectPadding = context.getResources().getDimension(
                R.dimen.recognized_rectangle_padding);
        float radiusRatio = Utils.getFloatFromDimen(context.getResources(),
                R.dimen.recognized_rectangle_round_radius_ratio);

        imagePaint = new Paint(Paint.ANTI_ALIAS_FLAG);

        // create mask image
        Paint maskPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        Bitmap maskImage = Bitmap.createBitmap(markedAreasImage.getWidth(), markedAreasImage.getHeight(),
                Config.ARGB_8888);
        Canvas maskCanvas = new Canvas(maskImage);

        maskPaint.setColor(dimmedColor);
        maskCanvas.drawRect(0, 0, markedAreasImage.getWidth(), markedAreasImage.getHeight(), maskPaint);

        maskPaint.setColor(Color.BLACK);
        for (RecognizedData data : dataList) {
            if (data.id == dataId) {
                for (RecognizedRect dataRect : data.getRectList()) {
                    float roundRadius = calculateRoundRadius(dataRect.height() * imageScale,
                            maxRoundRadius, minRoundRadius, radiusRatio);

                    float angle = (float) dataRect.getRotation();// get the angle
                    int saveCount = maskCanvas.save();
                    Point center = dataRect.getCenter();
                    maskCanvas.translate(center.x * imageScale, center.y * imageScale);
                    maskCanvas.rotate(angle);
                    RectF rect = dataRect.getCenteredAtOrigin(rectPadding, imageScale);
                    featheredRect.initialize(rect.left, rect.top, rect.right, rect.bottom,
                            roundRadius);
                    featheredRect.draw(maskCanvas);
                    maskCanvas.restoreToCount(saveCount);
                }
            }
        }

        for (RecognizedData data : dataList) {
            if (data.id == dataId) {
                for (RecognizedRect dataRect : data.getRectList()) {
                    float angle = (float) dataRect.getRotation();// get the angle
                    // rotate the canvas
                    int saveCount = maskCanvas.save();
                    Point center = dataRect.getCenter();
                    maskCanvas.translate(center.x * imageScale, center.y * imageScale);
                    maskCanvas.rotate(angle);
                    Log.d(LOG_TAG, "Angle for " + data.getText() + ": " + angle);
                    RectF rect = dataRect.getCenteredAtOrigin(rectPadding, imageScale);
                    float roundRadius = calculateRoundRadius(dataRect.height() * imageScale,
                            maxRoundRadius, minRoundRadius, radiusRatio);
                    maskCanvas.drawRoundRect(rect, roundRadius, roundRadius, maskPaint);
                    maskCanvas.restoreToCount(saveCount);
                }
            }
        }

        // create snapshot image
        Canvas mCanvas = new Canvas(markedAreasImage);
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));
        mCanvas.drawBitmap(maskImage, 0, 0, paint);

        paint.setXfermode(null);

    }

    private float calculateRoundRadius(float height, float maxRoundRadius, float minRoundRadius,
            float radiusRatio) {
        float radius = height * radiusRatio;
        if (radius < minRoundRadius) {
            radius = minRoundRadius;
        } else if (radius > maxRoundRadius) {
            radius = maxRoundRadius;
        }
        //Log.d(LOG_TAG, "Radius for height: " + height + " = " +  radius);
        return radius;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBoundsChange(Rect bounds) {
        if (initialLoader != null) {
            float horizontalScale = (float) bounds.width() / markedAreasImage.getWidth();
            float verticalScale = (float) bounds.height() / markedAreasImage.getHeight();
            float scalefactor = Math.min(horizontalScale, verticalScale);

            float atX = (bounds.width() - markedAreasImage.getWidth() * scalefactor) / 2;
            float atY = (bounds.height() - markedAreasImage.getHeight() * scalefactor) / 2;

            initialLoader.set(scalefactor, atX, atY);
        }
    }

    /**
     * gets the image start y
     *
     * @return image start y
     */
    public float getImageStartY() {
        if (initialLoader != null) {
            return initialLoader.atY;
        }
        return 0;
    }

    /**
     * Sets the image start y
     *
     * @param y
     *            image start y
     */
    public void setImageY(float y) {
        if (initialLoader != null) {
            initialLoader.atY = y;
            invalidateSelf();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void draw(Canvas canvas) {
        int saveCount = canvas.save();
        if (initialLoader != null) {
            initialLoader.draw(canvas);

        } else {
            canvas.scale(canvasView.getScale(), canvasView.getScale());
            canvas.drawBitmap(markedAreasImage, canvasView.getTranslateX(),
                    canvasView.getTranslateY(), imagePaint);
        }
        canvas.restoreToCount(saveCount);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setAlpha(int alpha) {
        imagePaint.setAlpha(alpha);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setColorFilter(ColorFilter filter) {
        imagePaint.setColorFilter(filter);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getOpacity() {
        if (haveOpacity) {
            return opacity;
        }
        opacity = Drawable.resolveOpacity(opacity, imagePaint.getAlpha());

        haveOpacity = true;
        return opacity;
    }

    /**
     * Enables the touch event on the drawable
     */
    public void enableTouchEvents() {
        initialLoader = null;
        canvasView.setDisplaySize(getBounds().width(), getBounds().height());
    }
}

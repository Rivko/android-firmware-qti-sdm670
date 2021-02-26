/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.util.Log;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.device.CoordinateSpace;
import com.qualcomm.qti.sta.device.CoordinateTranslator;

/**
 * Class for displaying ROI
 */
public class ROIDrawable extends Drawable {
    private static final String LOG_TAG = "ROIDrawable";

    private enum FrameType {
        CONTACT_CARD, FLYER;
    };

    // grid
    private final float gridCellSize;
    private float[] linesAtX = null;
    private float[] linesAtY = null;
    private final Paint gridPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    // background
    private final RectF leftRect = new RectF();
    private final RectF topRect = new RectF();
    private final RectF rightRect = new RectF();
    private final RectF bottomRect = new RectF();
    private final Paint backgroundPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    // frame
    private final RectF frame = new RectF();
    private final RectF[] frameLines = new RectF[8];
    private final Paint framePaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    private final float defaultFrameOffset;
    private final float contactCardRatio;
    private final float maxBusinessCardFrameHeight;
    private FrameType frameType = FrameType.CONTACT_CARD;

    // frame handlers
    private final Paint handlerPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final float handlerSize;
    private final RectF leftHandler = new RectF();
    private final RectF topHandler = new RectF();
    private final RectF rightHandler = new RectF();
    private final RectF bottomHandler = new RectF();

    private final float minFrameWidth;
    private final float minFrameHeight;

    private final float frameStroke;
    private final float frameThickStroke;

    private boolean haveOpacity = false;
    private int opacity = 0;

    private float frameWidthLocked = 0;
    private float frameHeightLocked = 0;

    /**
     * Constructor for the ROI Drawable
     *
     * @param resources
     *            application resources
     */
    /* package */ROIDrawable(Resources resources) {
        minFrameWidth = resources.getDimension(R.dimen.min_frame_width);
        minFrameHeight = resources.getDimension(R.dimen.min_frame_height);
        frameStroke = resources.getDimension(R.dimen.roi_frame_stroke);
        frameThickStroke = resources.getDimension(R.dimen.roi_frame_thick_stroke);
        handlerSize = resources.getDimension(R.dimen.handler_size);
        gridCellSize = resources.getDimension(R.dimen.roi_grid_size);
        defaultFrameOffset = resources.getDimension(R.dimen.default_roi_screen_offset);
        contactCardRatio = Utils.getFloatFromDimen(resources, R.dimen.business_card_aspect_ratio);
        maxBusinessCardFrameHeight = resources.getDimension(R.dimen.max_roi_business_card_height);

        int backgroundColor = resources.getColor(R.color.faded_background);
        int frameColor = resources.getColor(R.color.roi_frame);
        int gridColor = resources.getColor(R.color.grid);

        backgroundPaint.setColor(backgroundColor);
        handlerPaint.setStyle(Paint.Style.STROKE);
        handlerPaint.setColor(frameColor);
        handlerPaint.setStrokeWidth(frameStroke);

        framePaint.setStyle(Paint.Style.STROKE);
        framePaint.setColor(frameColor);
        framePaint.setStrokeWidth(frameStroke);

        gridPaint.setStyle(Paint.Style.STROKE);
        gridPaint.setColor(gridColor);
        gridPaint.setStrokeWidth(0);

        for (int i = 0; i < frameLines.length; i++) {
            frameLines[i] = new RectF();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBoundsChange(Rect bounds) {
        calculateGridParameters(bounds);

        float width = bounds.width() - defaultFrameOffset * 2;
        float height = (frameType == FrameType.CONTACT_CARD) ? bounds.width() * contactCardRatio
                : bounds.height() - defaultFrameOffset * 2;

        if (height > maxBusinessCardFrameHeight) {
            height = maxBusinessCardFrameHeight;
            width = maxBusinessCardFrameHeight / contactCardRatio;
        }
        updatePositions(bounds, width, height);
    }

    private void calculateGridParameters(Rect viewArea) {
        float xParts = viewArea.width() / gridCellSize;
        int xLines = (int) xParts;
        if (xParts > (float) xLines) {
            xLines++;
        }

        float yParts = viewArea.height() / gridCellSize;
        int yLines = (int) yParts;
        if (yParts > (float) yLines) {
            yLines++;
        }

        linesAtX = new float[xLines];
        linesAtY = new float[yLines];

        for (int i = 0; i < xLines; i++) {
            linesAtX[i] = (i + 1) * gridCellSize;
        }

        for (int i = 0; i < yLines; i++) {
            linesAtY[i] = (i + 1) * gridCellSize;
        }
    }

    private void updatePositions(Rect viewArea, float width, float height) {
        if (width == frame.width() && height == frame.height()) {
            // Log.d(LOG_TAG, "Already at correct size");
            return;
        }
        float left = viewArea.centerX() - width / 2;
        float top = viewArea.centerY() - height / 2;
        float right = viewArea.centerX() + width / 2;
        float bottom = viewArea.centerY() + height / 2;

        frame.set(left, top, right, bottom);

        // update background
        topRect.set(0, 0, viewArea.width(), frame.top);
        leftRect.set(0, frame.top, frame.left, frame.bottom);
        rightRect.set(frame.right, frame.top, viewArea.width(), frame.bottom);
        bottomRect.set(0, frame.bottom, viewArea.width(), viewArea.height());

        // update handlers
        float handlerCenterX = handlerSize / 2;
        float handlerCenterY = handlerSize / 2;
        setHandler(topHandler, frame.centerX() - handlerCenterX, frame.top - handlerCenterY);
        setHandler(leftHandler, frame.left - handlerCenterX, frame.centerY() - handlerCenterY);
        setHandler(rightHandler, frame.right - handlerCenterX, frame.centerY() - handlerCenterY);
        setHandler(bottomHandler, frame.centerX() - handlerCenterX, frame.bottom - handlerCenterY);

        // update frame lines
        frameLines[0].set(frame.left, frame.top, topHandler.left, frame.top);
        frameLines[1].set(topHandler.right, frame.top, frame.right, frame.top);
        frameLines[2].set(frame.left, frame.top, frame.left, leftHandler.top);
        frameLines[3].set(frame.left, leftHandler.bottom, frame.left, frame.bottom);
        frameLines[4].set(frame.right, frame.top, frame.right, rightHandler.top);
        frameLines[5].set(frame.right, rightHandler.bottom, frame.right, frame.bottom);
        frameLines[6].set(frame.left, frame.bottom, bottomHandler.left, frame.bottom);
        frameLines[7].set(bottomHandler.right, frame.bottom, frame.right, frame.bottom);

        invalidateSelf();
    }

    private void setHandler(RectF handlerRect, float left, float top) {
        handlerRect.set(left, top, left + handlerSize, top + handlerSize);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void draw(Canvas canvas) {
        Rect viewArea = getBounds();
        // draw grid
        for (int i = 0; i < linesAtX.length; i++) {
            canvas.drawLine(linesAtX[i], 0, linesAtX[i], viewArea.height(), gridPaint);
        }
        for (int i = 0; i < linesAtY.length; i++) {
            canvas.drawLine(0, linesAtY[i], viewArea.width(), linesAtY[i], gridPaint);
        }

        // draw background
        canvas.drawRect(topRect, backgroundPaint);
        canvas.drawRect(leftRect, backgroundPaint);
        canvas.drawRect(rightRect, backgroundPaint);
        canvas.drawRect(bottomRect, backgroundPaint);

        // draw frame lines and handlers
        for (RectF line : frameLines) {
            canvas.drawLine(line.left, line.top, line.right, line.bottom, framePaint);
        }
        canvas.drawRect(topHandler, handlerPaint);
        canvas.drawRect(leftHandler, handlerPaint);
        canvas.drawRect(rightHandler, handlerPaint);
        canvas.drawRect(bottomHandler, handlerPaint);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setAlpha(int alpha) {
        backgroundPaint.setAlpha(alpha);
        framePaint.setAlpha(alpha);
        handlerPaint.setAlpha(alpha);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setColorFilter(ColorFilter filter) {
        backgroundPaint.setColorFilter(filter);
        framePaint.setColorFilter(filter);
        handlerPaint.setColorFilter(filter);
    }

    /**
     * {@inheritDoc}
     */
    public int getOpacity() {
        if (haveOpacity) {
            return opacity;
        }
        opacity = Drawable.resolveOpacity(opacity, backgroundPaint.getAlpha());
        opacity = Drawable.resolveOpacity(opacity, framePaint.getAlpha());
        opacity = Drawable.resolveOpacity(opacity, handlerPaint.getAlpha());

        haveOpacity = true;
        return opacity;
    }

    /**
     * Unlock the view
     */
    /* package */void unlockView() {

    }

    /**
     * lock the view
     */
    /* package */void lockView() {
        frameWidthLocked = frame.width();
        frameHeightLocked = frame.height();
    }

    private float validateWidth(float rawWidth) {
        float validWidth = rawWidth;
        if (validWidth < minFrameWidth) {
            validWidth = minFrameWidth;
        } else if (validWidth > getBounds().width()) {
            validWidth = getBounds().width();
        }
        return validWidth;
    }

    private float validateHeight(float rawHeight) {
        float validHeight = rawHeight;
        if (validHeight < minFrameHeight) {
            validHeight = minFrameHeight;
        } else if (validHeight > getBounds().height()) {
            validHeight = getBounds().height();
        }
        return validHeight;
    }

    /**
     * Zooms the ROI with scale factor
     *
     * @param scale
     *            scale factor
     */
    /* package */void zoomBy(float scale) {
        float newWidth = validateWidth(frameWidthLocked * scale);
        float newHeight = validateHeight(frameHeightLocked * scale);
        updatePositions(getBounds(), newWidth, newHeight);
    }

    /**
     * drag the roi
     *
     * @param start
     *            starting point where user started dragging
     * @param from
     *            point from where device started recording dragging
     * @param toX
     *            drag x distance
     * @param toY
     *            drag y distance
     */
    /* package */void drag(PointF start, PointF from, float toX, float toY) {
        float midX = getBounds().centerX();
        float midY = getBounds().centerY();

        float yDistance = 0;
        if (start.y < midY) {
            yDistance = from.y - toY;
        } else {
            yDistance = -(from.y - toY);
        }
        float xDistance = 0;
        if (start.x < midX) {
            xDistance = from.x - toX;
        } else {
            xDistance = -(from.x - toX);
        }
        float newWidth = validateWidth(frame.width() + xDistance * 2);
        float newHeight = validateHeight(frame.height() + yDistance * 2);
        updatePositions(getBounds(), newWidth, newHeight);
    }

    /**
     * show/hides the UI related to taking snapshot
     *
     * @param enable
     *            true if ui needs to be shown, false otherwise
     */
    public void enableSnapshotUI(boolean enable) {
        framePaint.setStrokeWidth(enable ? frameThickStroke : frameStroke);
        invalidateSelf();
    }

    /**
     * gets the roi frame rectangle
     *
     * @return frame rectangle
     */
    public RectF getEnclosedArea() {
        return frame;
    }

    /**
     * return back a cropped image
     *
     * @param source
     *            full source image
     * @param previewRect
     *            rectangle area on preview
     * @param pictureSpace
     *            picture coordinate space
     * @param previewSpace
     *            preview coordinate space
     * @return cropped image portion as marked by ROI frame
     */
    /* package */Bitmap getCroppedImage(Bitmap source, RectF previewRect,
            CoordinateSpace previewSpace, CoordinateSpace pictureSpace) {

        CoordinateTranslator coordinateTranslator = new CoordinateTranslator();
        coordinateTranslator.setSourceCoordinateSpace(previewSpace);
        coordinateTranslator.setDestinationCoordinateSpace(pictureSpace);

        RectF mappedRect = new RectF();
        coordinateTranslator.mapRect(previewRect, mappedRect);

        if (mappedRect.width() > source.getWidth() || mappedRect.height() > source.getHeight()) {
            Log.d(LOG_TAG, "Invalid mappedRect: " + mappedRect.left + "," + mappedRect.top + ","
                    + mappedRect.right + "," + mappedRect.bottom);
            return null;
        }
        Log.d(LOG_TAG, "mappedRect: " + mappedRect.left + "," + mappedRect.top + ","
                + mappedRect.right + "," + mappedRect.bottom);
        return Bitmap.createBitmap(source, (int) mappedRect.left, (int) mappedRect.top,
                (int) mappedRect.width(), (int) mappedRect.height());
    }
}

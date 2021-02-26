/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.data;

import android.graphics.Bitmap;
import android.graphics.Rect;

/**
 * This class holds all the information for KeyFrame
 *
 */
public class KeyFrameInfo {

    private Bitmap bitmap;
    private int keyFrameId;

    private int keyFrameScaledTrackingPositionX;
    private int keyFrameScaledTrackingPositionY;

    // Rect area for each frame with tracking position offset
    private Rect keyFrameRect;
    // Rect area should be displayed on the screen.
    // It is the common overlap area between frames.
    private Rect keyFrameDisplayRect = new Rect(0, 0, 0, 0);

    /**
     * Constructor, construct a keyframe object
     * @param bitmap Bitmap
     * @param keyFrameId int
     */
    public KeyFrameInfo(Bitmap bitmap, int keyFrameId, Rect rect, int scaledPositionX, int scaledPositionY) {
        this.bitmap = bitmap;
        this.keyFrameId = keyFrameId;
        this.keyFrameRect = rect;
        this.keyFrameDisplayRect = new Rect(0, 0, 0, 0);

        this.keyFrameScaledTrackingPositionX = scaledPositionX;
        this.keyFrameScaledTrackingPositionY = scaledPositionY;
    }

    /**
     * Set the current frame rect
     * @param rect Rect
     */
    public void setKeyFrameRect(Rect rect) {
        this.keyFrameRect = rect;
    }

    /**
     * Return current key frame id
     * @return key frame id int
     */
    public int getKeyFrameId() {
        return this.keyFrameId;
    }

    /**
     * Return current key frame bitmap
     * @return key frame bitmap
     */
    public Bitmap getKeyFrameBitmap() {
        return this.bitmap;
    }

    /**
     * Return key frame Rect
     * @return key frame rect
     */
    public Rect getKeyFrameRect() {
        return this.keyFrameRect;
    }

    /**
     * Set the key frame display rect
     * @param rect Rect
     */
    public void setKeyFrameDisplayRect(Rect rect) {
        this.keyFrameDisplayRect.set(rect);
    }

    /**
     * Return the key frame display rect
     * @return keyFrameDisplayRect Rect
     */
    public Rect getKeyFrameDisplayRect() {
        return this.keyFrameDisplayRect;
    }

    /**
     * Set the key frame tracking position x value scaled accordingly
     * @param positionX int
     */
    public void setKeyFrameScaledPositionX(int positionX) {
        this.keyFrameScaledTrackingPositionX = positionX;
    }

    /**
     * Set the key frame tracking position y value scaled accordingly
     * @param positionY int
     */
    public void setKeyFrameScaledPositionY(int positionY) {
        this.keyFrameScaledTrackingPositionY = positionY;
    }

    /**
     * Return key frame scaled tracking position x value
     * @return keyFrameScaledTrackingPositionX
     */
    public int getKeyFrameScaledPositionX() {
        return this.keyFrameScaledTrackingPositionX;
    }

    /**
     * Return key frame tracking position y value
     * @return keyFrameScaledTrackingPositionY
     */
    public int getKeyFrameScaledPositionY() {
        return this.keyFrameScaledTrackingPositionY;
    }
}

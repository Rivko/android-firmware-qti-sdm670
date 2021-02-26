/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import android.graphics.Point;
import android.graphics.RectF;

/**
 * Rectangle for the recognized data. This class has 4 points: vertex 1 is
 * top-left, vertex 2 is top-right, vertex 3 is bottom-right, vertex 4 is
 * bottom-left
 */
public class RecognizedRect {
    //private static final String LOG_TAG = "RecognizedRect";

    /**
     * Class to store information on comparing two lines
     */
    public static class LinesComparisonData {
        public final float dx;
        public final float dy;
        public final boolean horizontallyInOrder;
        public final boolean verticallyInOrder;

        private LinesComparisonData(float dx, float dy, boolean horizontallyInOrder,
                boolean verticallyInOrder) {
            this.dx = dx;
            this.dy = dy;
            this.horizontallyInOrder = horizontallyInOrder;
            this.verticallyInOrder = verticallyInOrder;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public String toString() {
            return "dx,dy=" + dx + "," + dy + " ,ho:" + horizontallyInOrder + ", vo:"
                    + verticallyInOrder;
        }
    }

    private final Point vertex1;// top-left
    private final Point vertex2;// top-right
    private final Point vertex3;// bottom-right
    private final Point vertex4;// bottom-left

    private Point center;
    private int rectWidth;
    private int rectHeight;
    private double rectRotation;

    /**
     * Constructor for the class
     *
     * @param v1x
     *            vertex 1 x value
     * @param v1y
     *            vertex 1 y value
     * @param v2x
     *            vertex 2 x value
     * @param v2y
     *            vertex 2 y value
     * @param v3x
     *            vertex 3 x value
     * @param v3y
     *            vertex 3 y value
     * @param v4x
     *            vertex 4 x value
     * @param v4y
     *            vertex 4 y value
     */
    public RecognizedRect(int v1x, int v1y, int v2x, int v2y, int v3x, int v3y, int v4x, int v4y) {
        vertex1 = new Point(v1x, v1y);
        vertex2 = new Point(v2x, v2y);
        vertex3 = new Point(v3x, v3y);
        vertex4 = new Point(v4x, v4y);

        rectWidth = (int) Math.hypot(vertex1.x - vertex2.x, vertex1.y - vertex2.y);
        rectHeight = (int) Math.hypot(vertex1.x - vertex4.x, vertex1.y - vertex4.y);
        center = new Point((vertex1.x + vertex3.x) / 2, (vertex1.y + vertex3.y) / 2);

        int base = vertex2.x - vertex1.x;
        int altitude = vertex2.y - vertex1.y;
        double angleValue = (double) altitude / (double) base;

        rectRotation = Math.toDegrees(Math.atan(angleValue));
    }

    /**
     * Constructor for the class
     *
     * @param vtx1
     *            vertex 1 information
     * @param vtx2
     *            vertex 2 information
     * @param vtx3
     *            vertex 3 information
     * @param vtx4
     *            vertex 4 information
     */
    public RecognizedRect(Point vtx1, Point vtx2, Point vtx3, Point vtx4) {
        this(vtx1.x, vtx1.y, vtx2.x, vtx2.y, vtx3.x, vtx3.y, vtx4.x, vtx4.y);
    }

    /**
     * get the height of the rect
     *
     * @return height of the rect
     */
    public int height() {
        return rectHeight;
    }

    /**
     * get the width of the rect
     *
     * @return height of the rect
     */
    public int width() {
        return rectWidth;
    }

    /**
     * get the rectangle assuming the center at origin
     *
     * @param padding
     *            padding needed for the rectangle
     * @param scale
     *            scale for the rect
     * @return centered rect
     */
    public RectF getCenteredAtOrigin(float padding, float scale) {
        return new RectF((-rectWidth / 2 - padding) * scale, (-rectHeight / 2 - padding) * scale,
                (rectWidth / 2 + padding) * scale, (rectHeight / 2 + padding) * scale);
    }

    /**
     * gets the vertical distance between two lines second placed below first.
     *
     * @param rect1
     *            first line rectangle
     * @param rect2
     *            second line rectangle
     * @return line distance
     */
    public static int getMultiLineDy(RecognizedRect rect1, RecognizedRect rect2) {
        return Math.abs(rect2.vertex1.y - rect1.vertex1.y);
    }

    /**
     * Compares two lines assuming lines are placed at same horizontal level
     *
     * @param leftRect
     *            left line rectangle
     * @param rightRect
     *            Right line rectangle
     * @return comparison information
     */
    public static LinesComparisonData getSameLineComparison(RecognizedRect leftRect,
            RecognizedRect rightRect) {
        boolean horizontallyInOrder = true;
        boolean verticallyInOrder = true;
        float dx, dy;
        if (rightRect.vertex1.x == rightRect.vertex4.x) {// no angle
            dx = Math.abs(rightRect.vertex1.x - leftRect.vertex2.x);
            dy = Math.abs(rightRect.vertex1.y - leftRect.vertex2.y);

            if (leftRect.vertex1.x > rightRect.vertex1.x) {
                horizontallyInOrder = false;
            }
            if (leftRect.vertex1.y > rightRect.vertex1.y) {
                verticallyInOrder = false;
            }

        } else {
            Point rightRectVtx1, rightRectVtx4, leftRectVtx2;
            if (leftRect.vertex1.x < rightRect.vertex1.x) {
                rightRectVtx1 = rightRect.vertex1;
                rightRectVtx4 = rightRect.vertex4;
                leftRectVtx2 = leftRect.vertex2;

            } else {
                rightRectVtx1 = leftRect.vertex1;
                rightRectVtx4 = leftRect.vertex4;
                leftRectVtx2 = rightRect.vertex2;
                horizontallyInOrder = false;
            }

            float slope1 = (float) (rightRectVtx4.y - rightRectVtx1.y)
                    / (float) (rightRectVtx4.x - rightRectVtx1.x);
            float constant1 = (float) (rightRectVtx4.y - slope1 * rightRectVtx4.x);

            float slope2 = -1.0f / slope1;
            float constant2 = (float) (leftRectVtx2.y - slope2 * leftRectVtx2.x);

            float intersectX = (constant2 - constant1) / (slope1 - slope2);
            float intersectY = ((slope1 * constant2) - (slope2 * constant1)) / (slope1 - slope2);

            //Log.d(LOG_TAG, "Intersect " + intersectX + "," + intersectY);
            dx = (float) Math.hypot(leftRectVtx2.x - intersectX, leftRectVtx2.y - intersectY);
            dy = (float) Math.hypot(rightRectVtx1.x - intersectX, rightRectVtx1.y - intersectY);

            if (intersectY > rightRectVtx1.y) {
                verticallyInOrder = false;
            }
        }

        return new LinesComparisonData(dx, dy, horizontallyInOrder, verticallyInOrder);
    }

    /**
     * Compares two lines assuming second line is placed below first line
     *
     * @param firstLineRect
     *            first line rectangle
     * @param secondLineRect
     *            second line rectangle
     * @return comparison information
     */
    public static LinesComparisonData getMultilineComparison(RecognizedRect firstLineRect,
            RecognizedRect secondLineRect) {
        boolean horizontallyInOrder = true;
        boolean verticallyInOrder = true;
        float dx, dy;

        dx = Math.abs(secondLineRect.vertex1.x - firstLineRect.vertex4.x);
        dy = Math.abs(secondLineRect.vertex1.y - firstLineRect.vertex4.y);

        if (secondLineRect.vertex1.x < firstLineRect.vertex4.x) {
            horizontallyInOrder = false;
        }
        if (secondLineRect.vertex1.y < firstLineRect.vertex4.y) {
            verticallyInOrder = false;
        }
        return new LinesComparisonData(dx, dy, horizontallyInOrder, verticallyInOrder);
    }

    /**
     * Merges two lines rectangles
     *
     * @param rect1
     *            first line rectangle
     * @param rect2
     *            second line rectangle
     * @return merged rectangle
     */
    public static RecognizedRect getMergedHorizontally(RecognizedRect rect1, RecognizedRect rect2) {
        RecognizedRect leftRect, rightRect;
        if (rect1.vertex1.x < rect2.vertex1.x) {
            leftRect = rect1;
            rightRect = rect2;
        } else {
            leftRect = rect2;
            rightRect = rect1;
        }

        return new RecognizedRect(leftRect.vertex1, rightRect.vertex2, rightRect.vertex3,
                leftRect.vertex4);
    }

    /**
     * gets the rectangle rotation
     *
     * @return angle rotation in degree
     */
    public double getRotation() {
        return rectRotation;
    }

    /**
     * gets the center point of the rectangle
     *
     * @return center point
     */
    public Point getCenter() {
        return center;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return "v1(" + vertex1.x + "," + vertex1.y + ") v2(" + vertex2.x + "," + vertex2.y
                + ") v3(" + vertex3.x + "," + vertex3.y + ") v4(" + vertex4.x + "," + vertex4.y
                + ")";
    }
}

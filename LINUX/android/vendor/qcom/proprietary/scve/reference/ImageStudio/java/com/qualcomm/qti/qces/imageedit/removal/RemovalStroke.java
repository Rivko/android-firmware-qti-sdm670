/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import java.util.ArrayList;

import android.graphics.Path;
import android.os.Parcel;
import android.os.Parcelable;

public class RemovalStroke implements Parcelable {

    public static final int SOFT_EDGE = 2;
    public static final float SMALL_MATTING_STROKE = 20f;
    public static final float MEDIUM_MATTING_STROKE = 40f;
    public static final float LARGE_MATTING_STROKE = 60f;

    private float width;

    private final Path path = new Path();
    private final ArrayList<Integer> intArrayList = new ArrayList<Integer>();

    public RemovalStroke(float width) {
        this.width = width;
    }

    private RemovalStroke(Parcel in) {
        recreatePathObject(in.createIntArray());
        this.width = in.readFloat();
    }

    public static final Parcelable.Creator<RemovalStroke> CREATOR = new Parcelable.Creator<RemovalStroke>() {
        public RemovalStroke createFromParcel(Parcel in) {
            return new RemovalStroke(in);
        }

        @Override
        public RemovalStroke[] newArray(int size) {
            return new RemovalStroke[size];
        }
    };

    @Override
    public void writeToParcel(Parcel dest, int flags) {

        int[] intArray = new int[intArrayList.size()];
        int i = 0;
        for (Integer mInteger : intArrayList) {
            intArray[i++] = mInteger;
        }
        dest.writeIntArray(intArray);

        dest.writeFloat(width);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    void pathMoveTo(float x, float y) {
        path.moveTo(x, y);
        intArrayList.add((int)x);
        intArrayList.add((int)y);
    }

    void pathLineTo(float x, float y) {
        path.lineTo(x, y);
        intArrayList.add((int)x);
        intArrayList.add((int)y);
    }

    private void recreatePathObject(int[] intArray) {
        int i = 0;
        if (intArray.length >= 2) {
            path.moveTo(intArray[i], intArray[i + 1]);
            i += 2;
            while (i < intArray.length) {
                path.lineTo(intArray[i], intArray[i + 1]);
                i += 2;
            }
        }
    }

    public Path getPath() {
        return path;
    }

    public float getWidth() {
        return width;
    }

    public void setWidth(float width) {
        this.width = width;
    }
}

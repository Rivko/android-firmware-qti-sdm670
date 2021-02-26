/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import java.util.ArrayList;
import java.util.ListIterator;

import android.graphics.Path;
import android.os.Parcel;
import android.os.Parcelable;

public class Stroke implements Parcelable {

    public static final int FOREGROUND = 0;
    public static final int BACKGROUND = 1;
    public static final int SOFT_EDGE = 2;

    public static final float SMALL_MATTING_STROKE = 20f;
    public static final float MEDIUM_MATTING_STROKE = 40f;
    public static final float LARGE_MATTING_STROKE = 60f;

    private int type;
    private float width;

    private final Path path = new Path();
    private ArrayList<Integer> intArrayList = new ArrayList<Integer>();

    public Stroke(int type, float width) {
        this.type = type;
        this.width = width;
    }

    private Stroke(Parcel in) {
        this.type = in.readInt();
        recreatePathObject(in.createIntArray());
        this.width = in.readFloat();
    }

    public static final Parcelable.Creator<Stroke> CREATOR = new Parcelable.Creator<Stroke>() {
        public Stroke createFromParcel(Parcel in) {
            return new Stroke(in);
        }

        @Override
        public Stroke[] newArray(int size) {
            return new Stroke[size];
        }
    };

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(type);
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
        // recreate intArrayList from the parceled int[] intArray
        for (int i = 0; i < intArray.length; i++) {
            intArrayList.add(intArray[i]);
        }
        recreatePathObjectFromIntArrayList();
    }

    private void recreatePathObjectFromIntArrayList() {
        if (intArrayList.size() >= 2) {
            ListIterator<Integer> intArrayIterator = intArrayList.listIterator();
            path.moveTo(intArrayIterator.next(), intArrayIterator.next());
            while (intArrayIterator.hasNext()) {
                path.lineTo(intArrayIterator.next(), intArrayIterator.next());
            }
        }
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
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

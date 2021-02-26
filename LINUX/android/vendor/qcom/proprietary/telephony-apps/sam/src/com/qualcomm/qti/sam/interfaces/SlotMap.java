/******************************************************************************
 * @file    SlotMap.java
 *
 * @brief   SlotMap parcel definition
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.sam.interfaces;

import android.util.Log;
import android.os.Parcelable;
import android.os.Parcel;

public class SlotMap implements Parcelable {
    private int[] mSlotMap;

    public SlotMap() {
    }

    public void setSlotMap(int[] slotMap) {
        mSlotMap = slotMap;
        if (slotMap != null) {
            mSlotMap = slotMap.clone();
        }
    }

    public int[] getSlotMap() {
        return mSlotMap.clone();
    }

    public SlotMap (Parcel in){
        int arrayLength;
        arrayLength = in.readInt();
        if (arrayLength != 0) {
            mSlotMap = new int[arrayLength];
            in.readIntArray(mSlotMap);
        } else {
            mSlotMap = null;
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public void readFromParcel(Parcel in){
        int arrayLength;
        arrayLength = in.readInt();
        if (arrayLength != 0) {
            mSlotMap = new int[arrayLength];
            in.readIntArray(mSlotMap);
        } else {
            mSlotMap = null;
        }
    }

    @Override
    public void writeToParcel(Parcel dest, int flags){
        if (mSlotMap != null) {
            dest.writeInt(mSlotMap.length);
            dest.writeIntArray(mSlotMap);
        } else {
            dest.writeInt(0);
        }
    }

    public static final Parcelable.Creator<SlotMap> CREATOR =
                                        new Parcelable.Creator<SlotMap>() {

        public SlotMap createFromParcel(Parcel in) {
            return new SlotMap(in);
        }

        public SlotMap[] newArray(int size) {
            return new SlotMap[size];
        }
    };
}


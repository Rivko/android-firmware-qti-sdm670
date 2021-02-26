/******************************************************************************
 * @file    LteDirectDiscoveryCapability.aidl
 * @brief   Parcelable class for Lte Direct Discovery capability
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.ltedirect.internal;

import android.os.Parcel;
import android.os.Parcelable;

/*
 * Contains device LTE-D capability of SUB1 and SUB2
 */
public final class LteDirectDiscoveryCapability implements Parcelable {
    public static final int SUB1 = 0;
    public static final int SUB2 = 1;
    public static final int SUB3 = 2;

    public static final int CAPABILITY_DISABLED = 0;
    public static final int CAPABILITY_ENABLED  = 1;

    public int subCount;
    public int capability[];  //capability[subid] will contains the lted capability of the subscription

    public static final Parcelable.Creator<LteDirectDiscoveryCapability> CREATOR =
        new Parcelable.Creator<LteDirectDiscoveryCapability>() {
            public LteDirectDiscoveryCapability createFromParcel(Parcel in) {
                return new LteDirectDiscoveryCapability(in);
            }

            public LteDirectDiscoveryCapability[] newArray(int size) {
                return new LteDirectDiscoveryCapability[size];
            }
        };

    public LteDirectDiscoveryCapability() {
    }

    private LteDirectDiscoveryCapability(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(subCount);
        out.writeIntArray(capability);
    }

    public void readFromParcel(Parcel in) {
        subCount = in.readInt();
        capability = new int[subCount];
        in.readIntArray(capability);
    }

    public int describeContents() {
        return 0;
    }
}


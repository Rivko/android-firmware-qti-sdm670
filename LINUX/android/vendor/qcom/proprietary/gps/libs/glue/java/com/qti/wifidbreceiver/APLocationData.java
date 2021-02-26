/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wifidbreceiver;

import android.os.Parcel;
import android.os.Parcelable;

public final class APLocationData implements Parcelable {
    public String mMacAddress;
    public float mLatitude;
    public float mLongitude;
    public float mMaxAntenaRange;
    public float mHorizontalError;

    public int mReliability;

    public static final int AP_LOC_WITH_LAT_LON         = 0x0;
    public static final int AP_LOC_MAR_VALID            = 0x1;
    public static final int AP_LOC_HORIZONTAL_ERR_VALID = 0x2;
    public static final int AP_LOC_RELIABILITY_VALID    = 0x4;
    public int mValidBits;

    public APLocationData() {

    }

    public static final Parcelable.Creator<APLocationData> CREATOR =
        new Parcelable.Creator<APLocationData>() {
        public APLocationData createFromParcel(Parcel in) {
            return new APLocationData(in);
        }

        public APLocationData[] newArray(int size) {
            return new APLocationData[size];
        }
    };

    private APLocationData(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeString(mMacAddress);
        out.writeFloat(mLatitude);
        out.writeFloat(mLongitude);
        out.writeFloat(mMaxAntenaRange);
        out.writeFloat(mHorizontalError);
        out.writeInt(mReliability);
        out.writeInt(mValidBits);
    }

    public void readFromParcel(Parcel in) {
        mMacAddress = in.readString();
        mLatitude = in.readFloat();
        mLongitude = in.readFloat();
        mMaxAntenaRange = in.readFloat();
        mHorizontalError = in.readFloat();
        mReliability = in.readInt();
        mValidBits = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

}

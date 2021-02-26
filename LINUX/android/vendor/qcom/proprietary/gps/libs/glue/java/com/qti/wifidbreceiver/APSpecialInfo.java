/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wifidbreceiver;

import android.os.Parcel;
import android.os.Parcelable;

public final class APSpecialInfo implements Parcelable {
    public String mMacAddress;
    public int mInfo;

    public APSpecialInfo() {

    }

    public static final Parcelable.Creator<APSpecialInfo> CREATOR =
        new Parcelable.Creator<APSpecialInfo>() {
        public APSpecialInfo createFromParcel(Parcel in) {
            return new APSpecialInfo(in);
        }

        public APSpecialInfo[] newArray(int size) {
            return new APSpecialInfo[size];
        }
    };

    private APSpecialInfo(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeString(mMacAddress);
        out.writeInt(mInfo);
    }

    public void readFromParcel(Parcel in) {
        mMacAddress = in.readString();
        mInfo = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

}


/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wifidbreceiver;

import android.os.Parcel;
import android.os.Parcelable;

public final class APInfo implements Parcelable {
    public String mMacAddress;
    // use 0 if cell info not available, fill 0 to all IDs
    public int mCellType;
    public int mCellRegionID1;
    public int mCellRegionID2;
    public int mCellRegionID3;
    public int mCellRegionID4;
    // use null if SSID not available
    public byte[] mSSID;

    public APInfo() {

    }

    public static final Parcelable.Creator<APInfo> CREATOR =
        new Parcelable.Creator<APInfo>() {
        public APInfo createFromParcel(Parcel in) {
            return new APInfo(in);
        }

        public APInfo[] newArray(int size) {
            return new APInfo[size];
        }
    };

    private APInfo(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeString(mMacAddress);
        out.writeInt(mCellType);
        out.writeInt(mCellRegionID1);
        out.writeInt(mCellRegionID2);
        out.writeInt(mCellRegionID3);
        out.writeInt(mCellRegionID4);
        out.writeByteArray(mSSID);
    }

    public void readFromParcel(Parcel in) {
        mMacAddress = in.readString();
        mCellType = in.readInt();
        mCellRegionID1 = in.readInt();
        mCellRegionID2 = in.readInt();
        mCellRegionID3 = in.readInt();
        mCellRegionID4 = in.readInt();
        mSSID = in.createByteArray();
    }

    public int describeContents() {
        return 0;
    }

}

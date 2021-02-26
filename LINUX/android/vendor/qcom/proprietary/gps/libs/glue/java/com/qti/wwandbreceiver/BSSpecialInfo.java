/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wwandbreceiver;

import android.os.Parcel;
import android.os.Parcelable;

public final class BSSpecialInfo implements Parcelable {
    public int mCellType;
    public int mCellRegionID1;
    public int mCellRegionID2;
    public int mCellRegionID3;
    public int mCellRegionID4;
    public int mInfo;

    public BSSpecialInfo() {

    }

    public static final Parcelable.Creator<BSSpecialInfo> CREATOR =
            new Parcelable.Creator<BSSpecialInfo>() {
        public BSSpecialInfo createFromParcel(Parcel in) {
            return new BSSpecialInfo(in);
        }

        public BSSpecialInfo[] newArray(int size) {
            return new BSSpecialInfo[size];
        }
    };

    private BSSpecialInfo(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mCellType);
        out.writeInt(mCellRegionID1);
        out.writeInt(mCellRegionID2);
        out.writeInt(mCellRegionID3);
        out.writeInt(mCellRegionID4);
        out.writeInt(mInfo);
    }

    public void readFromParcel(Parcel in) {
        mCellType = in.readInt();
        mCellRegionID1 = in.readInt();
        mCellRegionID2 = in.readInt();
        mCellRegionID3 = in.readInt();
        mCellRegionID4 = in.readInt();
        mInfo = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

}


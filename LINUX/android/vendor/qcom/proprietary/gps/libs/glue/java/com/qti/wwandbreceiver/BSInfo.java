/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wwandbreceiver;

import android.os.Parcel;
import android.os.Parcelable;

public final class BSInfo implements Parcelable {
    // use 0 if cell info not available, fill 0 to all IDs
    public int mCellType;
    public int mCellRegionID1;
    public int mCellRegionID2;
    public int mCellRegionID3;
    public int mCellRegionID4;

    public BSInfo() {

    }

    public static final Parcelable.Creator<BSInfo> CREATOR =
            new Parcelable.Creator<BSInfo>() {
        public BSInfo createFromParcel(Parcel in) {
            return new BSInfo(in);
        }

        public BSInfo[] newArray(int size) {
            return new BSInfo[size];
        }
    };

    private BSInfo(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mCellType);
        out.writeInt(mCellRegionID1);
        out.writeInt(mCellRegionID2);
        out.writeInt(mCellRegionID3);
        out.writeInt(mCellRegionID4);
    }

    public void readFromParcel(Parcel in) {
        mCellType = in.readInt();
        mCellRegionID1 = in.readInt();
        mCellRegionID2 = in.readInt();
        mCellRegionID3 = in.readInt();
        mCellRegionID4 = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

}

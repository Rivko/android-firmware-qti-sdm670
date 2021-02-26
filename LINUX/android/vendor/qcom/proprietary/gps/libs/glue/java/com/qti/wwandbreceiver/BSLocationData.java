/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wwandbreceiver;

import android.os.Parcel;
import android.os.Parcelable;

public final class BSLocationData implements Parcelable {
    public int mCellType;
    public int mCellRegionID1;
    public int mCellRegionID2;
    public int mCellRegionID3;
    public int mCellRegionID4;

    public float mLatitude;
    public float mLongitude;

    public static final int BS_LOC_WITH_LAT_LON = 0x0;

    public static final int BS_LOC_HORIZONTAL_COV_RADIUS_VALID = 0x1;
    public static final int BS_LOC_HORIZONTAL_CONFIDENCE_VALID = 0x2;
    public static final int BS_LOC_HORIZONTAL_RELIABILITY_VALID = 0x4;
    public static final int BS_LOC_ALTITUDE_VALID = 0x8;
    public static final int BS_LOC_ALTITUDE_UNCERTAINTY_VALID = 0x10;
    public static final int BS_LOC_ALTITUDE_CONFIDENCE_VALID = 0x20;
    public static final int BS_LOC_ALTITUDE_RELIABILITY_VALID = 0x40;

    public int mValidBits;

    public float mHorizontalCoverageRadius;
    public int mHorizontalConfidence;
    public int mHorizontalReliability;
    public float mAltitude;
    public float mAltitudeUncertainty;
    public float mAltitudeConfidence;
    public int mAltitudeReliability;

    public BSLocationData() {

    }

    public static final Parcelable.Creator<BSLocationData> CREATOR =
        new Parcelable.Creator<BSLocationData>() {
        public BSLocationData createFromParcel(Parcel in) {
            return new BSLocationData(in);
        }

        public BSLocationData[] newArray(int size) {
            return new BSLocationData[size];
        }
    };

    private BSLocationData(Parcel in) {
        readFromParcel(in);
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mCellType);
        out.writeInt(mCellRegionID1);
        out.writeInt(mCellRegionID2);
        out.writeInt(mCellRegionID3);
        out.writeInt(mCellRegionID4);
        out.writeFloat(mLatitude);
        out.writeFloat(mLongitude);
        out.writeInt(mValidBits);
        out.writeFloat(mHorizontalCoverageRadius);
        out.writeInt(mHorizontalConfidence);
        out.writeInt(mHorizontalReliability);
        out.writeFloat(mAltitude);
        out.writeFloat(mAltitudeUncertainty);
        out.writeFloat(mAltitudeConfidence);
        out.writeInt(mAltitudeReliability);
    }

    public void readFromParcel(Parcel in) {
        mCellType = in.readInt();
        mCellRegionID1 = in.readInt();
        mCellRegionID2 = in.readInt();
        mCellRegionID3 = in.readInt();
        mCellRegionID4 = in.readInt();
        mLatitude = in.readFloat();
        mLongitude = in.readFloat();
        mValidBits = in.readInt();
        mHorizontalCoverageRadius = in.readFloat();
        mHorizontalConfidence = in.readInt();
        mHorizontalReliability = in.readInt();
        mAltitude = in.readFloat();
        mAltitudeUncertainty = in.readFloat();
        mAltitudeConfidence = in.readFloat();
        mAltitudeReliability = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

}

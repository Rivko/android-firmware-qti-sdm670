/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qti.geofence;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.Bundle;
import android.util.Log;

public class GeofenceData implements Parcelable {
    private static String TAG = "GeofenceData";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    public static final int LATITUDE_IS_SET = 0x01;
    public static final int LONGITUDE_IS_SET = 0x02;
    public static final int RADIUS_IS_SET = 0x04;
    public static final int TRANSITION_TYPES_IS_SET = 0x08;
    public static final int CONFIDENCE_IS_SET = 0x10;
    public static final int DWELL_NOTIFY_IS_SET = 0x20;
    public static final int RESPONSIVENESS_IS_SET = 0x40;

    public enum GeofenceTransitionTypes {
        UNKNOWN(0), ENTERED_ONLY(1), EXITED_ONLY(2), ENTERED_AND_EXITED(3);

        private final int mValue;
        GeofenceTransitionTypes(int val) { this.mValue = val; }
        public int getValue() { return mValue; }
    };

    public enum GeofenceConfidence {
        LOW(1), MEDIUM(2), HIGH(3);

        private final int mValue;
        GeofenceConfidence(int val) { this.mValue = val; }
        public int getValue() { return mValue; }
    };

    public enum DwellTypes {
        UNKNOWN(0), DWELL_TYPE_INSIDE(1), DWELL_TYPE_OUTSIDE(2),
        DWELL_TYPE_INSIDE_OUTSIDE(3);

        private final int mValue;
        DwellTypes(int val) { this.mValue = val; }
        public int getValue() { return mValue; }
    }

    private int mNotifyResponsiveness;
    private double mLatitude;
    private double mLongitude;
    private double mRadius;
    private GeofenceTransitionTypes mTransitionType;
    private GeofenceConfidence mConfidence;
    private DwellTypes mDwellType;
    private int mDwellTime;
    private String mAppTextData;
    private Bundle mAppBundleData;
    private int mGeofenceId;
    private boolean mIsPaused;

    public GeofenceData(Parcel source) {
        mNotifyResponsiveness = source.readInt();
        mLatitude = source.readDouble();
        mLongitude = source.readDouble();
        mRadius = source.readDouble();

        try {
            mTransitionType = GeofenceTransitionTypes.valueOf(source.readString());
        } catch (IllegalArgumentException x) {
            mTransitionType = null;
        }

        try {
            mConfidence = GeofenceConfidence.valueOf(source.readString());
        } catch (IllegalArgumentException x) {
            mConfidence = null;
        }

        try {
            mDwellType = DwellTypes.valueOf(source.readString());
        } catch (IllegalArgumentException x) {
            mDwellType = null;
        }
        mDwellTime = source.readInt();

        try {
            mAppTextData = source.readString();
        } catch (IllegalArgumentException x) {
            mAppTextData = null;
        }

        mAppBundleData = source.readBundle();

        mGeofenceId = source.readInt();
        mIsPaused = source.readBoolean();
    }

    public GeofenceData(int notifyResponsiveness, double latitude,
        double longitude, double radius, int transitionType,
        int confidence, int dwellType, int dwellTime,
        String appTextData, Bundle appBundleData, int geofenceId) {
            mNotifyResponsiveness = notifyResponsiveness;
            mLatitude = latitude;
            mLongitude = longitude;
            mRadius = radius;
            setTransitionType(transitionType);
            setConfidence(confidence);
            setDwellType(dwellType);
            mDwellTime = dwellTime;
            mAppTextData = appTextData;
            mAppBundleData = appBundleData;
            mGeofenceId = geofenceId;
            mIsPaused = false;
    }

    public int getNotifyResponsiveness() {
        return mNotifyResponsiveness;
    }

    public double getLatitude() {
        return mLatitude;
    }

    public double getLongitude(){
        return mLongitude;
    }

    public double getRadius() {
        return mRadius;
    }

    public GeofenceTransitionTypes getTransitionType() {
        return mTransitionType;
    }

    public GeofenceConfidence getConfidence() {
        return mConfidence;
    }

    public DwellTypes getDwellType() {
        return mDwellType;
    }

    public int getDwellTime() {
        return mDwellTime;
    }

    public String getAppTextData() {
        return mAppTextData;
    }

    public Bundle getAppBundleData() {
        return mAppBundleData;
    }

    public int getGeofenceId() {
        return mGeofenceId;
    }

    public boolean isPaused() {
        return mIsPaused;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public void setLatitude(double latitude) {
        mLatitude = latitude;
    }

    public void setLongitude(double longitude) {
        mLongitude = longitude;
    }

    public void setRadius(double radius) {
        mRadius = radius;
    }

    public void setNotifyResponsiveness(int notifyResponsiveness) {
        mNotifyResponsiveness = notifyResponsiveness;
    }

    public void setTransitionType(int transitionType) {
        mTransitionType = GeofenceTransitionTypes.UNKNOWN;
        for (GeofenceTransitionTypes type : GeofenceTransitionTypes.values()) {
            if (type.getValue() == transitionType) {
                mTransitionType = type;
                break;
            }
        }
    }

    public void setConfidence(int confidence) {
        mConfidence = GeofenceConfidence.LOW;
        for (GeofenceConfidence type : GeofenceConfidence.values()) {
            if (type.getValue() == confidence) {
                mConfidence = type;
                break;
            }
        }
    }

    public void setDwellType(int dwellType) {
        mDwellType = DwellTypes.UNKNOWN;
        for (DwellTypes type : DwellTypes.values()) {
            if (type.getValue() == dwellType) {
                mDwellType = type;
                break;
            }
        }
    }

    public void setDwellTime(int dwellTime) {
        mDwellTime = dwellTime;
    }

    public void setGeofenceId(int gfId) {
        mGeofenceId = gfId;
    }

    public void setPausedStatus(boolean isPaused) {
        mIsPaused = isPaused;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        if (VERBOSE) {
            Log.v(TAG, "in GeofenceData: writeToParcel()" +
               "; responsiveness is " + mNotifyResponsiveness +
               "; latitude is " + mLatitude +
               "; longitude is " + mLongitude +
               "; radius is " + mRadius +
               "; transitionTypes is " + mTransitionType.name() +
               "; confidence is " + mConfidence.name() +
               "; dwellTimeMask is " + mDwellType.name() +
               "; dwellTime is " + mDwellTime +
               "; AppTextData is " + mAppTextData +
               "; Geofence id is " + mGeofenceId);
        }

        dest.writeInt(mNotifyResponsiveness);
        dest.writeDouble(mLatitude);
        dest.writeDouble(mLongitude);
        dest.writeDouble(mRadius);
        dest.writeString((null == mTransitionType) ? "" : mTransitionType.name());
        dest.writeString((null == mConfidence) ? "" : mConfidence.name());
        dest.writeString((null == mDwellType) ? "" : mDwellType.name());
        dest.writeInt(mDwellTime);
        dest.writeString(mAppTextData);
        dest.writeBundle(mAppBundleData);
        dest.writeInt(mGeofenceId);
        dest.writeBoolean(mIsPaused);
    }

    public static final Parcelable.Creator<GeofenceData> CREATOR =
            new Parcelable.Creator<GeofenceData>() {
        @Override
        public GeofenceData createFromParcel(Parcel source) {
             return new GeofenceData(source);
        }
        @Override
        public GeofenceData[] newArray(int size) {
            return new GeofenceData[size];
        }
    };
};

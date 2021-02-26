/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =============================================================================*/

package com.qti.debugreport;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.Bundle;
import android.util.Log;

/** @addtogroup IZatLocationReport
@{ */

/**
 * Class IZatLocationReport.
 *
 * This class contains location information.
 */
public class IZatLocationReport implements Parcelable {
    private static String TAG = "IZatLocationReport";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

/** @} */ /* end_addtogroup IZatLocationReport */

/** @addtogroup IZatLocationReport
@{ */

    /**
     * Enum of IZat location source values.
     */
    public enum IzatLocationSource{
        POSITION_SOURCE_UNKNOWN(0),
        POSITION_SOURCE_CPI(1),
        POSITION_SOURCE_REFERENCE_LOCATION(2),
        POSITION_SOURCE_TLE(3);

        private final int mValue;
        private IzatLocationSource(int value)  {
            this.mValue = value;
        }

        public int getValue() {
            return mValue;
        }
    };

    private int mValidityBit;
    private double mLatitude;
    private double mLongitude;
    private float mAccuracy;
    private double mAltitude;
    private float mAltitudeUncertainity;
    private IzatLocationSource mSource;
    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;

    private static final int HAS_HORIZONTAL_COMPONENT = 1;
    private static final int HAS_VERTICAL_COMPONENT = 2;
    private static final int HAS_SOURCE = 4;

    public IZatLocationReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported, int validityMask,
        double lat, double lon, float horzAccuracy,
        double alt, float altUnc, int source) {

        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;
        mValidityBit = validityMask;

        if ((mValidityBit & HAS_HORIZONTAL_COMPONENT) != 0) {
            mLatitude = lat;
            mLongitude = lon;
            mAccuracy = horzAccuracy;
        }

        if ((mValidityBit & HAS_VERTICAL_COMPONENT) != 0) {
            mAltitude = alt;
            mAltitudeUncertainity = altUnc;
        }

        if ((mValidityBit & HAS_SOURCE) != 0) {
            /* Values coming from modem are
               EpiSrc = 1: Coarse Position
               EpiSrc = 2: Reference Location
               EpiSrc = 3: TLE Position*/
            try {
                mSource = IzatLocationSource.values()[source];
            } catch (ArrayIndexOutOfBoundsException e) {
                mSource = IzatLocationSource.POSITION_SOURCE_UNKNOWN;
            }
        }
    }

    public IZatLocationReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mValidityBit = source.readInt();
        if ((mValidityBit & HAS_HORIZONTAL_COMPONENT) != 0) {
            mLatitude = source.readDouble();
            mLongitude = source.readDouble();
            mAccuracy = source.readFloat();
        }

        if ((mValidityBit & HAS_VERTICAL_COMPONENT) != 0) {
            mAltitude = source.readDouble();
            mAltitudeUncertainity = source.readFloat();
        }

        if ((mValidityBit & HAS_SOURCE) != 0) {
            try {
                mSource = IzatLocationSource.values()[source.readInt()];
            } catch (ArrayIndexOutOfBoundsException e) {
                mSource = IzatLocationSource.POSITION_SOURCE_UNKNOWN;
            }
        }
    }

    /**
     * Checks whether the horizontal component of the fix (i.e., latitude, longitude, and horizontal accuracy)
     * is available.
     *
     * @return A Boolean value to indicate whether the horizontal component is set.
     */
    public boolean hasHorizontalFix() {
        return ((mValidityBit & HAS_HORIZONTAL_COMPONENT) != 0 );
    }

    /**
     * Checks whether the vertical component of the fix (i.e., altitude and altitude uncertainity)
     * is available.
     *
     * @return A Boolean value to indicate whether the vertical component is set.
     */
    public boolean hasVerticalFix() {
        return ((mValidityBit & HAS_VERTICAL_COMPONENT) != 0);
    }

    /**
     * Checks whether the source information is set.
     *
     * @return A Boolean value to indicate whether the source is set.
     */
    public boolean hasSource() {
        return ((mValidityBit & HAS_SOURCE) != 0);
    }


    /**
     * Gets the latitude.
     *
     * @return The latitude in radians.
     */
    public double getLatitude() {
        return mLatitude;
    }

    /**
     * Gets the longitude.
     *
     * @return The longitude in radians.
     */
    public double getLongitude() {
        return mLongitude;
    }

    /**
     * Gets the altitude.
     *
     * @return The altitude in radians.
     */
    public double getAltitude() {
        return mAltitude;
    }

    /**
     * Gets the horizontal accuracy.
     *
     * @return The horizontal accuracy of the fix in meters.
     */
    public float getAccuracy() {
        return mAccuracy;
    }

    /**
     * Gets the altitude uncertainity.
     *
     * @return The altitude uncertainity of the fix in meters.
     */
    public float getAltitudeUncertainity() {
        return mAltitudeUncertainity;
    }

    /**
     * Gets the source of the fix.
     *
     * @return The source of the fix; see {@link IzatLocationSource}.
     */
    public IzatLocationSource getSource() {
        return mSource;
    }

    /**
     * Gets the UTC time of when the data was last updated or changed.
     *
     * @return Returns a UTC time as {@link IZatUtcSpec}.
    */
    public IZatUtcSpec getUTCTimestamp() {
        return mUtcTimeLastUpdated;
    }

    /**
     * Gets the UTC time of when the data was last reported.
     *
     * @return Returns a UTC time as {@link IZatUtcSpec}.
    */
    public IZatUtcSpec getLastReportedUTCTime() {
        return mUtcTimeLastReported;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(mUtcTimeLastUpdated, 0);
        dest.writeParcelable(mUtcTimeLastReported, 0);

        dest.writeInt(mValidityBit);
        if ((mValidityBit & HAS_HORIZONTAL_COMPONENT) != 0) {
            dest.writeDouble(mLatitude);
            dest.writeDouble(mLongitude);
            dest.writeFloat(mAccuracy);
        }

        if ((mValidityBit & HAS_VERTICAL_COMPONENT) != 0) {
            dest.writeDouble(mAltitude);
            dest.writeFloat(mAltitudeUncertainity);
        }

        if ((mValidityBit & HAS_SOURCE) != 0) {
            dest.writeInt(mSource.getValue());
        }
    }

/** @cond */

    public static final Parcelable.Creator<IZatLocationReport> CREATOR =
        new Parcelable.Creator<IZatLocationReport>() {
        @Override
        public IZatLocationReport createFromParcel(Parcel source) {
            return new IZatLocationReport(source);
        }
        @Override
        public IZatLocationReport[] newArray(int size) {
            return new IZatLocationReport[size];
        }

/** @endcond */

    };
};
/** @} */ /* end_addtogroup IZatLocationReport */

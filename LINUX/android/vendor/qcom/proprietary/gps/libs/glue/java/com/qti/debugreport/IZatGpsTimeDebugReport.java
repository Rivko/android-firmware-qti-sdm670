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

import java.lang.ArrayIndexOutOfBoundsException;
import java.lang.String;

/** @addtogroup IZatGpsTimeDebugReport
@{ */

/**
 * Class IZatGpsTimeDebugReport.
 *
 * This class contains GPS time information.
 */
public class IZatGpsTimeDebugReport implements Parcelable {
    private static String TAG = "IZatGpsTimeReport";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

/** @} */ /* end_addtogroup IZatGpsTimeDebugReport */

/** @addtogroup IZatGpsTimeDebugReport
@{ */

    /**
    * Enumeration of IZat time source values.
    */
    public enum IZatTimeSource {
        TIME_SOURCE_ESTIMATE_INVALID (0),
        TIME_SOURCE_ESTIMATE_NETWORK_TIME_TRANSFER (1),
        TIME_SOURCE_ESTIMATE_NETWORK_TIME_TAGGING (2),
        TIME_SOURCE_ESTIMATE_EXTERNAL_INPUT (3),
        TIME_SOURCE_ESTIMATE_TOW_DECODE (4),
        TIME_SOURCE_ESTIMATE_TOW_CONFIRMED (5),
        TIME_SOURCE_ESTIMATE_TOW_AND_WEEK_CONFIRMED (6),
        TIME_SOURCE_ESTIMATE_TIME_ALIGNMENT (7),
        TIME_SOURCE_ESTIMATE_NAV_SOLUTION (8),
        TIME_SOURCE_ESTIMATE_SOLVE_FOR_TIME (9),
        TIME_SOURCE_ESTIMATE_GLO_TOD_DECODE (10),
        TIME_SOURCE_ESTIMATE_TIME_CONVERSION (11),
        TIME_SOURCE_ESTIMATE_SLEEP_CLOCK (12) ,
        TIME_SOURCE_ESTIMATE_SLEEP_CLOCK_TIME_TRANSFER (13),
        TIME_SOURCE_ESTIMATE_UNKNOWN (14),
        TIME_SOURCE_ESTIMATE_WCDMA_SLEEP_TIME_TAGGING (15),
        TIME_SOURCE_ESTIMATE_GSM_SLEEP_TIME_TAGGING (16),
        TIME_SOURCE_ESTIMATE_GAL_TOW_DECODE (17),
        TIME_SOURCE_ESTIMATE_BDS_SOW_DECODE (18),
        TIME_SOURCE_ESTIMATE_QZSS_TOW_DECODE (19);

        private final int mValue;
        private IZatTimeSource(int value)  {
            this.mValue = value;
        }

        public int getValue() {
            return mValue;
        }
    };

    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;
    private int mGpsWeek;
    private long mGpsTimeOfWeekInMs;
    private boolean mTimeValid;
    private IZatTimeSource mTimeSource;
    private int mTimeUncertainity;

    private int mClockFrequencyBias;
    private int mClockFrequencyBiasUncertainity;

    public IZatGpsTimeDebugReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported,
        int gpsWeek, long gpsTimeOfweekInMs, boolean timeValid, int timeSource,
        int timeUncertainity, int clockfrequencyBias,
        int clockfrequencyBiasUncertainity) {

        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;

        mGpsWeek = gpsWeek;
        mGpsTimeOfWeekInMs= gpsTimeOfweekInMs;

        mTimeValid = timeValid;
        try {
            mTimeSource = IZatTimeSource.values()[timeSource];
        } catch (ArrayIndexOutOfBoundsException e) {
            mTimeSource = IZatTimeSource.TIME_SOURCE_ESTIMATE_INVALID;
        }
        mTimeUncertainity = timeUncertainity;

        mClockFrequencyBias = clockfrequencyBias;
        mClockFrequencyBiasUncertainity = clockfrequencyBiasUncertainity;
    }

    public IZatGpsTimeDebugReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mGpsWeek = source.readInt();
        mGpsTimeOfWeekInMs= source.readLong();
        mTimeValid = (source.readInt() == 1);
        mTimeSource = IZatTimeSource.values()[source.readInt()];
        mTimeUncertainity = source.readInt();

        mClockFrequencyBias = source.readInt();
        mClockFrequencyBiasUncertainity = source.readInt();
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


    /**
    * Gets the GPS week.
    *
    * @return Returns the GPS week.
    */
    public int getGpsWeek() {
        return mGpsWeek;
    }

    /**
    * Gets the GPS time of week in ms.
    *
    * @return Returns the GPS time of week in ms.
    */
    public long getGpsTimeOfWeek() {
        return mGpsTimeOfWeekInMs;
    }

    /**
    * Queries whether the time is valid.
    *
    * @return Boolean value; TRUE if the time is valid, FALSE if the time is not valid.
    */
    public boolean IsTimeValid() {
        return mTimeValid;
    }

    /**
    * Gets the time source.
    *
    * @return Time source of {@link IZatTimeSource} type.
    */
    public IZatTimeSource getTimeSource() {
        return mTimeSource;
    }

    /**
    * Gets the time uncertainity.
    *
    * @return Time uncertainity in microseconds.
    */
    public int getTimeUncertainity() {
        return mTimeUncertainity;
    }

   /**
   * Gets the clock frequency bias.
   *
   * @return Clock frequency bias in ppb.
   */
   public int getClockFrequencyBias() {
        return mClockFrequencyBias;
    }


    /**
    * Gets the clock frequency bias uncertainity.
    *
    * @return Clock frequency bias uncertainity in ppb.
    */
    public int getClockFrequencyBiasUncertainity() {
        return mClockFrequencyBiasUncertainity;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(mUtcTimeLastUpdated, 0);
        dest.writeParcelable(mUtcTimeLastReported, 0);

        dest.writeInt(mGpsWeek);
        dest.writeLong(mGpsTimeOfWeekInMs);
        dest.writeInt(mTimeValid ? 1 : 0);
        dest.writeInt(mTimeSource.getValue());
        dest.writeInt(mTimeUncertainity);

        dest.writeInt(mClockFrequencyBias);
        dest.writeInt(mClockFrequencyBiasUncertainity);
    }

/** @cond */

    public static final Parcelable.Creator<IZatGpsTimeDebugReport> CREATOR =
            new Parcelable.Creator<IZatGpsTimeDebugReport>() {
        @Override
        public IZatGpsTimeDebugReport createFromParcel(Parcel source) {
             return new IZatGpsTimeDebugReport(source);
        }
        @Override
        public IZatGpsTimeDebugReport[] newArray(int size) {
            return new IZatGpsTimeDebugReport[size];
        }
/** @endcond */
    };
};
/** @} */ /* end_addtogroup IZatGpsTimeDebugReport */

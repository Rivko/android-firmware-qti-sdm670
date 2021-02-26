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

/** @addtogroup IZatFixStatusDebugReport
@{ */

/**
 * Class IZatFixStatusDebugReport.

 * This class contains the status of position fixes.
 * It also contains failure reasons for not getting a final fix.
 */
public class IZatFixStatusDebugReport implements Parcelable {
    private static String TAG = "IZatFixStatus";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    private static final int IS_FINAL_FIX_SUCCESSFUL = 1;
    private static final int IS_TOO_FEW_SV_SEEN = 2;
    private static final int IS_HEPE_CHECK_FAIL = 4;
    private static final int IS_VERY_LOW_RELAIBILITY = 8;

/** @} */ /* end_addtogroup IZatFixStatusDebugReport */

/** @addtogroup IZatFixStatusDebugReport
@{ */

    /**
    * Enum of the IZat fix status.
    */
    public enum IzatFixStatus {
        FINAL_FIX_SUCCESSFUL(0), TOO_FEW_SV(1), HEPE_CHECK_FAIL(2), VERY_LOW_RELAIBILITY_FIX(3);

        private final int mFixStatus;
        private IzatFixStatus(int status) {
            mFixStatus = status;
        }

        public int getValue() {
            return mFixStatus;
        }
    };

/** @cond */
    private int mFixStatusMask;
    private IzatFixStatus mFixStatus;
    private long mHepeLimit;
    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;
/** @endcond */

    public IZatFixStatusDebugReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported, int fixStatusMask, long hepeLimit) {

        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;

        mFixStatusMask = fixStatusMask;
        if ((mFixStatusMask & IS_FINAL_FIX_SUCCESSFUL) != 0) {
            mFixStatus = IzatFixStatus.values()[0];
        } else if ((mFixStatusMask & IS_TOO_FEW_SV_SEEN) != 0) {
            mFixStatus = IzatFixStatus.values()[1];
        } else if ((mFixStatusMask & IS_HEPE_CHECK_FAIL) != 0) {
            mFixStatus = IzatFixStatus.values()[2];
        } else if ((mFixStatusMask & IS_VERY_LOW_RELAIBILITY) != 0) {
            mFixStatus = IzatFixStatus.values()[3];
        }

        mHepeLimit = hepeLimit;
    }

    public IZatFixStatusDebugReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mFixStatusMask = source.readInt();
        if ((mFixStatusMask & IS_FINAL_FIX_SUCCESSFUL) != 0) {
            mFixStatus = IzatFixStatus.values()[0];
        } else if ((mFixStatusMask & IS_TOO_FEW_SV_SEEN) != 0) {
            mFixStatus = IzatFixStatus.values()[1];
        } else if ((mFixStatusMask & IS_HEPE_CHECK_FAIL) != 0) {
            mFixStatus = IzatFixStatus.values()[2];
        } else if ((mFixStatusMask & IS_VERY_LOW_RELAIBILITY) != 0) {
            mFixStatus = IzatFixStatus.values()[3];
        }


        mHepeLimit = source.readLong();
    }

/** @} */ /* end_addtogroup IZatFixStatusDebugReport */

/** @addtogroup IZatFixStatusDebugReport
@{ */

    /**
     * Gets the fix status.
     *
     * @return Returns a {@link IzatFixStatus} type.
    */
    public IzatFixStatus getFixStatus() {
        return mFixStatus;
    }


    /**
     * Gets the HEPE threshold.
     *
     * @return Returns the HEPE threshold.
    */
    public long getHEPELimit() {
        return mHepeLimit;
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

        dest.writeInt(mFixStatusMask);
        dest.writeLong(mHepeLimit);
    }

/** @cond */

    public static final Parcelable.Creator<IZatFixStatusDebugReport> CREATOR =
            new Parcelable.Creator<IZatFixStatusDebugReport>() {
        @Override
        public IZatFixStatusDebugReport createFromParcel(Parcel source) {
             return new IZatFixStatusDebugReport(source);
        }
        @Override
        public IZatFixStatusDebugReport[] newArray(int size) {
            return new IZatFixStatusDebugReport[size];
        }

/** @endcond */

    };
};
/** @} */ /* end_addtogroup IZatFixStatusDebugReport */

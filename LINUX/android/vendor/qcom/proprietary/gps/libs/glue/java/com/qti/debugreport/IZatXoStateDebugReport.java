/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qti.debugreport;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.Bundle;

import java.lang.ArrayIndexOutOfBoundsException;
import java.lang.String;

/** @addtogroup IZatXoStateDebugReport
@{ */

/**
 * Class IZatXoStateDebugReport.
 *
 * This class contains crystal oscillator (XO) state information.
 */
public class IZatXoStateDebugReport implements Parcelable {
    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;
    private IZatXoState mXoState;

    public IZatXoStateDebugReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported, int xoState) {
        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;

        try {
            mXoState = IZatXoState.values()[xoState];
        } catch (ArrayIndexOutOfBoundsException e) {
            mXoState = IZatXoState.FAILED;
        }
    }

    public IZatXoStateDebugReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mXoState = IZatXoState.values()[source.readInt()];
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
    * Gets the XO state.
    *
    * @return Object of the {@link IZatXoState} type.
    */
    public IZatXoState getXoState() {
        return mXoState;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(mUtcTimeLastUpdated, 0);
        dest.writeParcelable(mUtcTimeLastReported, 0);
        dest.writeInt(mXoState.getValue());
    }
/** @cond */
    public static final Parcelable.Creator<IZatXoStateDebugReport> CREATOR =
            new Parcelable.Creator<IZatXoStateDebugReport>() {
        @Override
        public IZatXoStateDebugReport createFromParcel(Parcel source) {
             return new IZatXoStateDebugReport(source);
        }
        @Override
        public IZatXoStateDebugReport[] newArray(int size) {
            return new IZatXoStateDebugReport[size];
        }
/** @endcond */

    };

/** @} */ /* end_addtogroup IZatXoStateDebugReport */

/** @addtogroup IZatXoStateDebugReport
@{ */

    /**
     * Enumeration of XO state values.
     */
    public enum IZatXoState {
        FAILED(0),
        NOT_CAL(1),
        FAC1(2),
        DEFAULT(3),
        WIDE_BINS(4),
        COARSE(5),
        IFC(6),
        FINE(7),
        FT1(8),
        OLD_RGS(9),
        INTERP(10),
        FT2(11),
        EXACT(12),
        RGS(13),
        RGS_RECENT(14),
        VCO_LAST(15 );

        private final int mValue;

        private IZatXoState(int value) {
            mValue = value;
        }

        public int getValue() {
            return mValue;
        }
    };
};
/** @} */ /* end_addtogroup IZatXoStateDebugReport */

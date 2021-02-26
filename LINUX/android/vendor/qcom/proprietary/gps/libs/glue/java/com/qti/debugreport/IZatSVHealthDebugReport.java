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

import java.util.List;
import java.util.ArrayList;

/** @addtogroup IZatSVHealthDebugReport
@{ */

/**
 * Class IZatSVHealthDebugReport.
 *
 * This class contains the health information of each
 * satellite in each GNSS constellation.
 */
public class IZatSVHealthDebugReport implements Parcelable {
    private static String TAG = "IZatSVHealthReport";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

/** @} */ /* end_addtogroup IZatSVHealthDebugReport */

/** @addtogroup IZatSVHealthDebugReport
@{ */

    /** Enumeration of SV health state values. */
    public enum IzatSVHealthState {
        SV_HEALTH_UNKNOWN(0),
        SV_HEALTH_GOOD(1),
        SV_HEALTH_BAD(2);

        private final int mValue;
        private IzatSVHealthState(int value) {
            mValue = value;
        }
        public int getValue() {
            return mValue;
        }
    };

    private List<IzatSVHealthState> mGpsSVHealthState = new ArrayList<IzatSVHealthState> ();
    private List<IzatSVHealthState> mGlonassSVHealthState = new ArrayList<IzatSVHealthState> ();
    private List<IzatSVHealthState> mBdsSVHealthState = new ArrayList<IzatSVHealthState> ();
    private List<IzatSVHealthState> mGalSVHealthState = new ArrayList<IzatSVHealthState> ();
    private List<IzatSVHealthState> mQzssSVHealthState = new ArrayList<IzatSVHealthState> ();

    private int mGpsGoodMask, mGpsBadMask, mGpsUnknownMask;
    private int mGlonassGoodMask, mGlonassBadMask, mGlonassUnknownMask;
    private long mBdsGoodMask, mBdsBadMask, mBdsUnknownMask;
    private long mGalGoodMask, mGalBadMask, mGalUnknownMask;
    private byte mQzssGoodMask, mQzssBadMask, mQzssUnknownMask;

    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;

    /**
     * Gets the satellite health state for GPS satellites.
     *
     * @return List of {@link IzatSVHealthState} for each satellite.
     */
    public List<IzatSVHealthState>getSVHealthForGPS() {
        return mGpsSVHealthState;
    }

    /**
     * Gets the satellite health state for GLONASS satellites.
     *
     * @return List of {@link IzatSVHealthState} for each satellite.
     */
    public List<IzatSVHealthState>getSVHealthForGlonass() {
        return mGlonassSVHealthState;
    }

    /**
     * Gets the satellite health state for BDS satellites.
     *
     * @return List of {@link IzatSVHealthState} for each satellite.
     */
    public List<IzatSVHealthState>getSVHealthForBDS() {
        return mBdsSVHealthState;
    }

    /**
     * Gets the satellite health state for GAL satellites.
     *
     * @return List of {@link IzatSVHealthState} for each satellite.
     */
    public List<IzatSVHealthState>getSVHealthForGal() {
        return mGalSVHealthState;
    }

    /**
     * Gets the satellite health state for QZSS satellites.
     *
     * @return List of {@link IzatSVHealthState} for each satellite.
     */
    public List<IzatSVHealthState>getSVHealthForQzss() {
        return mQzssSVHealthState;
    }

    public IZatSVHealthDebugReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported,
        int gpsGoodMask, int gpsBadMask, int gpsUnknownMask,
        int glonassGoodMask, int glonassBadMask, int glonassUnknownMask,
        long bdsGoodMask, long bdsBadMask, long bdsUnknownMask,
        long galGoodMask, long galBadMask, long galUnknownMask,
        byte qzssGoodMask, byte qzssBadMask, byte qzssUnknownMask) {

        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;

        mGpsGoodMask = gpsGoodMask;
        mGpsBadMask = gpsBadMask;
        mGpsUnknownMask = gpsUnknownMask;
        mGlonassGoodMask = glonassGoodMask;
        mGlonassBadMask = glonassBadMask;
        mGlonassUnknownMask = glonassUnknownMask;
        mBdsGoodMask = bdsGoodMask;
        mBdsBadMask = bdsBadMask;
        mBdsUnknownMask = bdsUnknownMask;
        mGalGoodMask = galGoodMask;
        mGalBadMask = galBadMask;
        mGalUnknownMask = galUnknownMask;
        mQzssGoodMask = qzssGoodMask;
        mQzssBadMask = qzssBadMask;
        mQzssUnknownMask = qzssUnknownMask;

        fillSVHealthList(mGpsSVHealthState, gpsGoodMask, gpsBadMask, gpsUnknownMask);
        fillSVHealthList(mGlonassSVHealthState, glonassGoodMask, glonassBadMask, glonassUnknownMask);
        fillSVHealthList(mBdsSVHealthState, bdsGoodMask, bdsBadMask, bdsUnknownMask);
        fillSVHealthList(mGalSVHealthState, galGoodMask, galBadMask, galUnknownMask);
        fillSVHealthList(mQzssSVHealthState, qzssGoodMask, qzssBadMask, qzssUnknownMask);
    }

    public IZatSVHealthDebugReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mGpsGoodMask = source.readInt();
        mGpsBadMask = source.readInt();
        mGpsUnknownMask = source.readInt();
        mGlonassGoodMask = source.readInt();
        mGlonassBadMask = source.readInt();
        mGlonassUnknownMask = source.readInt();

        mBdsGoodMask = source.readLong();
        mBdsBadMask = source.readLong();
        mBdsUnknownMask = source.readLong();
        mGalGoodMask = source.readLong();
        mGalBadMask = source.readLong();
        mGalUnknownMask = source.readLong();

        mQzssGoodMask = source.readByte();
        mQzssBadMask =  source.readByte();
        mQzssUnknownMask = source.readByte();

        fillSVHealthList(mGpsSVHealthState, mGpsGoodMask, mGpsBadMask, mGpsUnknownMask);
        fillSVHealthList(mGlonassSVHealthState, mGlonassGoodMask, mGlonassBadMask, mGlonassUnknownMask);
        fillSVHealthList(mBdsSVHealthState, mBdsGoodMask, mBdsBadMask, mBdsUnknownMask);
        fillSVHealthList(mGalSVHealthState, mGalGoodMask, mGalBadMask, mGalUnknownMask);
        fillSVHealthList(mQzssSVHealthState, mQzssGoodMask, mQzssBadMask, mQzssUnknownMask);
    }

    private void fillSVHealthList(List<IzatSVHealthState> listSVHealth, int goodMask, int badMask, int unknowMask) {
        for (int i = 0; i < 32; i++) {
            if ((1 << i & goodMask) != 0) {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_GOOD);
            } else if ((1 << i & badMask) != 0) {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_BAD);
            } else {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_UNKNOWN);
            }
        }
    }

    private void fillSVHealthList(List<IzatSVHealthState> listSVHealth, long goodMask, long badMask, long unknowMask) {
        for (long i = 0; i < 64; i++) {
            if ((1L << i & goodMask) != 0) {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_GOOD);
            } else if ((1L << i & badMask) != 0) {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_BAD);
            } else {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_UNKNOWN);
            }
        }
    }

    private void fillSVHealthList(List<IzatSVHealthState> listSVHealth, byte goodMask, byte badMask, byte unknowMask) {
        for (byte i = 0; i < 8; i++) {
            if ((1 << i & goodMask) != 0) {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_GOOD);
            } else if ((1 << i & badMask) != 0) {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_BAD);
            } else {
                listSVHealth.add(IzatSVHealthState.SV_HEALTH_UNKNOWN);
            }
        }
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

        dest.writeInt(mGpsGoodMask);
        dest.writeInt(mGpsBadMask);
        dest.writeInt(mGpsUnknownMask);
        dest.writeInt(mGlonassGoodMask);
        dest.writeInt(mGlonassBadMask);
        dest.writeInt(mGlonassUnknownMask);

        dest.writeLong(mBdsGoodMask);
        dest.writeLong(mBdsBadMask);
        dest.writeLong(mBdsUnknownMask);
        dest.writeLong(mGalGoodMask);
        dest.writeLong(mGalBadMask);
        dest.writeLong(mGalUnknownMask);

        dest.writeByte(mQzssGoodMask);
        dest.writeByte(mQzssBadMask);
        dest.writeByte(mQzssUnknownMask);
    }

/** @cond */

    public static final Parcelable.Creator<IZatSVHealthDebugReport> CREATOR =
        new Parcelable.Creator<IZatSVHealthDebugReport>() {
        @Override
        public IZatSVHealthDebugReport createFromParcel(Parcel source) {
            return new IZatSVHealthDebugReport(source);
        }
        @Override
        public IZatSVHealthDebugReport[] newArray(int size) {
            return new IZatSVHealthDebugReport[size];
        }

/** @endcond */

    };
};
/** @} */ /* end_addtogroup IZatSVHealthDebugReport */

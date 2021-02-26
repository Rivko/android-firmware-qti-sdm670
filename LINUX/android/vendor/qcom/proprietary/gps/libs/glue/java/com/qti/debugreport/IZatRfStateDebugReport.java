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

/** @addtogroup IZatRfStateDebugReport
@{ */

/**
 * Class IZatRfStateDebugReport.
 *
 * This class contains the RF state and parameters.
 */
public class IZatRfStateDebugReport implements Parcelable {
    private static String TAG = "IZatRfStateReport";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;
    private int mPGAGain;
    private long mADCAmplitudeI;
    private long mADCAmplitudeQ;
    private long mJammerMetricGPS;
    private long mJammerMetricGlonass;
    private long mJammerMetricBds;
    private long mJammerMetricGal;
    private long mErrorRecovery;
    private long mGPSBPAmpI;
    private long mGPSBPAmpQ;
    private long mGLOBPAmpI;
    private long mGLOBPAmpQ;
    private long mBDSBPAmpI;
    private long mBDSBPAmpQ;
    private long mGALBPAmpI;
    private long mGALBPAmpQ;

    public IZatRfStateDebugReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported,
        int pgaGain, long adcAmplI, long adcAmplQ,
        long jammermetricGps, long jammermetricGlonass,
        long jammermetricBds, long jammermetricGal,
        long gpsBpAmpI, long gpsBpAmpQ, long gloBpAmpI, long gloBpAmpQ,
        long bdsBpAmpI, long bdsBpAmpQ, long galBpAmpI, long galBpAmpQ) {

        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;

        mPGAGain = pgaGain;
        mADCAmplitudeI = adcAmplI;
        mADCAmplitudeQ = adcAmplQ;

        mJammerMetricGPS = jammermetricGps;
        mJammerMetricGlonass = jammermetricGlonass;
        mJammerMetricBds = jammermetricBds;
        mJammerMetricGal = jammermetricGal;

        mGPSBPAmpI = gpsBpAmpI;
        mGPSBPAmpQ = gpsBpAmpQ;
        mGLOBPAmpI = gloBpAmpI;
        mGLOBPAmpQ = gloBpAmpQ;
        mBDSBPAmpI = bdsBpAmpI;
        mBDSBPAmpQ = bdsBpAmpQ;
        mGALBPAmpI = galBpAmpI;
        mGALBPAmpQ = galBpAmpQ;
    }

    public IZatRfStateDebugReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mPGAGain = source.readInt();
        mADCAmplitudeI = source.readLong();
        mADCAmplitudeQ = source.readLong();

        mJammerMetricGPS = source.readLong();
        mJammerMetricGlonass = source.readLong();
        mJammerMetricBds = source.readLong();
        mJammerMetricGal = source.readLong();

        mGPSBPAmpI = source.readLong();
        mGPSBPAmpQ = source.readLong();
        mGLOBPAmpI = source.readLong();
        mGLOBPAmpQ = source.readLong();
        mBDSBPAmpI = source.readLong();
        mBDSBPAmpQ = source.readLong();
        mGALBPAmpI = source.readLong();
        mGALBPAmpQ = source.readLong();
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
    * Gets the GNSS RF gain.
    *
    * @return The GNSS RF gain.
    */
    public int getPGAGain() {
        return mPGAGain;
    }


    /**
    * Gets the ADC amplitude I.
    *
    * @return The ADC amplitude I.
    */
    public long getADCAmplitudeI() {
        return mADCAmplitudeI;
    }

    /**
    * Gets the ADC amplitude Q.
    *
    * @return The ADC amplitude Q.
    */
    public long getADCAmplitudeQ() {
        return mADCAmplitudeQ;
    }

    /**
    * Gets the jammer metric for GPS.
    *
    * @return The GPS jammer metric.
    */
    public long getJammerMetricGPS() {
        return mJammerMetricGPS;
    }

    /**
    * Gets the jammer metric for GLONASS.
    *
    * @return The GLONASS jammer metric.
    */
    public long getJammerMetricGlonass() {
        return mJammerMetricGlonass;
    }

    /**
    * Gets the jammer metric for Beidou.
    *
    * @return The Beidou jammer metric.
    */
    public long getJammerMetricBds() {
        return mJammerMetricBds;
    }

    /**
    * Gets the jammer metric for GAL.
    *
    * @return The GAL jammer metric.
    */
    public long getJammerMetricGal() {
        return mJammerMetricGal;
    }

    /**
    * Gets the GPS baseband processor amplitude I.
    *
    * @return The GPS baseband processor amplitude I.
    */
    public long getGPSBPAmpI() {
        return  mGPSBPAmpI;
    }

    /**
    * Gets the GPS baseband processor amplitude Q.
    *
    * @return The GPS baseband processor amplitude Q.
    */
    public long getGPSBPAmpQ() {
        return mGPSBPAmpQ;
    }

    /**
    * Gets the GLONASS baseband processor amplitude I.
    *
    * @return The GLONASS baseband processor amplitude I.
    */
    public long getGLOBPAmpI() {
        return  mGLOBPAmpI;
    }

    /**
    * Gets the GLONASS baseband processor amplitude Q.
    *
    * @return The GLONASS basedband processor amplitude Q.
    */
    public long getGLOBPAmpQ() {
        return mGLOBPAmpQ;
    }

    /**
    * Gets the BDS baseband processor amplitude I.
    *
    * @return The BDS baseband processor amplitude I.
    */
    public long getBDSBPAmpI() {
        return  mBDSBPAmpI;
    }

    /**
    * Gets the BDS baseband processor amplitude Q.
    *
    * @return The BDS baseband processor amplitude Q.
    */
    public long getBDSBPAmpQ() {
        return mBDSBPAmpQ;
    }

    /**
    * Get the GAL baseband processor amplitude I.
    *
    * @return The GAL baseband processor amplitude I.
    */
    public long getGALBPAmpI() {
        return  mGALBPAmpI;
    }

    /**
    * Gets the GAL baseband processor amplitude Q.
    *
    * @return The GAL baseband processor amplitude Q.
    */
    public long getGALBPAmpQ() {
        return mGALBPAmpQ;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(mUtcTimeLastUpdated, 0);
        dest.writeParcelable(mUtcTimeLastReported, 0);

        dest.writeInt(mPGAGain);
        dest.writeLong(mADCAmplitudeI);
        dest.writeLong(mADCAmplitudeQ);

        dest.writeLong(mJammerMetricGPS);
        dest.writeLong(mJammerMetricGlonass);
        dest.writeLong(mJammerMetricBds);
        dest.writeLong( mJammerMetricGal);

        dest.writeLong(mGPSBPAmpI);
        dest.writeLong(mGPSBPAmpQ);
        dest.writeLong(mGLOBPAmpI);
        dest.writeLong(mGLOBPAmpQ);
        dest.writeLong(mBDSBPAmpI);
        dest.writeLong(mBDSBPAmpQ);
        dest.writeLong(mGALBPAmpI);
        dest.writeLong(mGALBPAmpQ);
    }

/** @cond */

    public static final Parcelable.Creator<IZatRfStateDebugReport> CREATOR =
            new Parcelable.Creator<IZatRfStateDebugReport>() {
        @Override
        public IZatRfStateDebugReport createFromParcel(Parcel source) {
             return new IZatRfStateDebugReport(source);
        }
        @Override
        public IZatRfStateDebugReport[] newArray(int size) {
            return new IZatRfStateDebugReport[size];
        }

/** @endcond */

    };
};
/** @} */ /* end_addtogroup IZatRfStateDebugReport */

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

/** @addtogroup IZatXTRADebugReport
@{ */

/**
 * Class IZatXTRADebugReport.
 *
 * This class contains the XTRA data validity and the
 *  age of the XTRA data for each GNSS constellation.
 */
public class IZatXTRADebugReport implements Parcelable {
    private static String TAG = "IZatXTRAReport";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

/** @} */ /* end_addtogroup IZatXTRADebugReport */

/** @addtogroup IZatXTRADebugReport
@{ */

    /**
    * Class GpsXtraValidityInfo.
    *
    * This class contains the age and the validity
    * of the XTRA data for GPS.
    * <ul>
    * <li> Age of the XTRA data is in seconds </li>
    * <li> XTRA validity is returned as an int mask value. Each bit represents the
    * XTRA data state of a satellite; 0 indicates that XTRA is not present
    * and 1 indicates that XTRA is present for that satellite.</li>
    * </ul>
    *
    */
    public class GpsXtraValidityInfo {
        private  int mGpsXtraAge;
        private  int mGpsXtraValidity;

      public int getXtraAge() {
        return mGpsXtraAge;
      }

      public int getGpsXtraValidity() {
        return mGpsXtraValidity;
      }
    }

/** @} */ /* end_addtogroup IZatXTRADebugReport */

/** @addtogroup IZatXTRADebugReport
@{ */

    /**
    * Class GlonassXtraValidityInfo.
    *
    * This class contains the age and the validity
    * of the XTRA data for GLONASS.
    * <ul>
    * <li> Age of the XTRA data is in seconds </li>
    * <li> XTRA validity is returned as an int mask value. Each bit represents the
    * XTRA data state of a satellite; 0 indicates that XTRA is not present
    * and 1 indicates that XTRA is present for that satellite.</li>
    * </ul>
    *
    */
    public class GlonassXtraValidityInfo {
        private int mGlonassXtraAge;
        private int  mGlonassXtraValidity;

        public int getXtraAge() {
            return mGlonassXtraAge;
        }

        public int getXtraValidity() {
            return mGlonassXtraValidity;
        }

    }

/** @} */ /* end_addtogroup IZatXTRADebugReport */

/** @addtogroup IZatXTRADebugReport
@{ */

    /**
    * Class BdsXtraValidityInfo.
    *
    * This class contains the age and the validity
    * of the XTRA data for BDS.
    * <ul>
    * <li> Age of the XTRA data is in seconds </li>
    * <li> XTRA validity is returned as a long mask value. Each bit represents the
    * XTRA data state of a satellite; 0 indicates that XTRA is not present
    * and 1 indicates that XTRA is present for that satellite.</li>
    * </ul>
    *
    */
    public class BdsXtraValidityInfo {
        private int mBdsXtraAge;
        private long mBdsXtraValidity;

      public int getXtraAge() {
        return mBdsXtraAge;
      }

      public long getXtraValidity() {
        return mBdsXtraValidity;
      }
    }

/** @} */ /* end_addtogroup IZatXTRADebugReport */

/** @addtogroup IZatXTRADebugReport
@{ */

    /**
    * Class GalXtraValidityInfo.
    *
    * This class contains the age and the validity
    * of the XTRA data for GAL.
    * <ul>
    * <li> Age of the XTRA data is in seconds </li>
    * <li> XTRA validity is returned as a long mask value. Each bit represents the
    * XTRA data state of a satellite; 0 indicates that XTRA is not present
    * and 1 indicates that XTRA is present for that satellite.</li>
    * </ul>
    *
    */
    public class GalXtraValidityInfo {
        private int mGalXtraAge;
        private long mGalXtraValidity;

      public int getXtraAge() {
        return mGalXtraAge;
      }

      public long getXtraValidity() {
        return mGalXtraValidity;
      }
    }

/** @} */ /* end_addtogroup IZatXTRADebugReport */

/** @addtogroup IZatXTRADebugReport
@{ */

    /**
    * Class QzssXtraValidityInfo.
    *
    * This class contains the age and the validity
    * of the XTRA data for QZSS.
    * <ul>
    * <li> Age of the XTRA data is in seconds </li>
    * <li> XTRA validity is returned as an int mask value. Each bit represents the
    * XTRA data state of a satellite; 0 indicates that XTRA is not present
    * and 1 indicates that XTRA is present for that satellite.</li>
    * </ul>
    *
    */
    public class QzssXtraValidityInfo {
        private int mQzssXtraAge;
        private byte mQzssXtraValidity;

      public int getXtraAge() {
        return mQzssXtraAge;
      }

      public byte getXtraValidity() {
        return mQzssXtraValidity;
      }
    }

/** @cond */

    private GpsXtraValidityInfo mGpsXtraValidityInfo;
    private GlonassXtraValidityInfo mGlonassXtraValidityInfo;
    private BdsXtraValidityInfo mBdsXtraValidityInfo;
    private GalXtraValidityInfo mGalXtraValidityInfo;
    private QzssXtraValidityInfo mQzssXtraValidityInfo;
    private byte mValidityMask;
    private IZatUtcSpec mUtcTimeLastUpdated, mUtcTimeLastReported;

    private static final int GPS_XTRA_DATA_AVAILABLE = 1;
    private static final int GLONASS_XTRA_DATA_AVAILABLE = 2;
    private static final int BDS_XTRA_DATA_AVAILABLE = 4;
    private static final int GAL_XTRA_DATA_AVAILABLE = 8;
    private static final int QZSS_XTRA_DATA_AVAILABLE = 16;

/** @endcond */

    public IZatXTRADebugReport(IZatUtcSpec utcTimeLastUpdated,
        IZatUtcSpec utcTimeLastReported,
        byte validityMask,  int gpsXtraValidity, int gpsXtraAge,
        int glonassXtraValidity, int glonassXtraAge, long bdsXtraValidity, int bdsXtraAge,
        long galXtraValidity, int galXtraAge, byte qzssXtraValidity, int qzssXtraAge) {

        mUtcTimeLastUpdated = utcTimeLastUpdated;
        mUtcTimeLastReported = utcTimeLastReported;

        mValidityMask = validityMask;

        if ((mValidityMask & GPS_XTRA_DATA_AVAILABLE) != 0 ) {
            mGpsXtraValidityInfo = new GpsXtraValidityInfo();
            mGpsXtraValidityInfo.mGpsXtraValidity = gpsXtraValidity;
            mGpsXtraValidityInfo.mGpsXtraAge = gpsXtraAge;
        }

        if ((mValidityMask & GLONASS_XTRA_DATA_AVAILABLE) != 0 ) {
            mGlonassXtraValidityInfo = new GlonassXtraValidityInfo();
            mGlonassXtraValidityInfo.mGlonassXtraValidity = glonassXtraValidity;
            mGlonassXtraValidityInfo.mGlonassXtraAge = glonassXtraAge;
        }

        if ((mValidityMask & BDS_XTRA_DATA_AVAILABLE) != 0 ) {
            mBdsXtraValidityInfo = new BdsXtraValidityInfo();
            mBdsXtraValidityInfo.mBdsXtraValidity = bdsXtraValidity;
            mBdsXtraValidityInfo.mBdsXtraAge = bdsXtraAge;
        }

        if ((mValidityMask & GAL_XTRA_DATA_AVAILABLE) != 0 ) {
            mGalXtraValidityInfo = new GalXtraValidityInfo();
            mGalXtraValidityInfo.mGalXtraValidity = galXtraValidity;
            mGalXtraValidityInfo.mGalXtraAge = galXtraAge;
        }

        if ((mValidityMask & QZSS_XTRA_DATA_AVAILABLE) != 0 ) {
            mQzssXtraValidityInfo = new QzssXtraValidityInfo();
            mQzssXtraValidityInfo.mQzssXtraValidity = qzssXtraValidity;
            mQzssXtraValidityInfo.mQzssXtraAge = qzssXtraAge;
        }
    }

    public IZatXTRADebugReport(Parcel source) {
        mUtcTimeLastUpdated = source.readParcelable(IZatUtcSpec.class.getClassLoader());
        mUtcTimeLastReported = source.readParcelable(IZatUtcSpec.class.getClassLoader());

        mValidityMask = source.readByte();

        if ((mValidityMask & GPS_XTRA_DATA_AVAILABLE) != 0 ) {
            mGpsXtraValidityInfo = new GpsXtraValidityInfo();
            mGpsXtraValidityInfo.mGpsXtraValidity = source.readInt();
            mGpsXtraValidityInfo.mGpsXtraAge = source.readInt();
        }

       if ((mValidityMask & GLONASS_XTRA_DATA_AVAILABLE) != 0 ) {
            mGlonassXtraValidityInfo = new GlonassXtraValidityInfo();
            mGlonassXtraValidityInfo.mGlonassXtraValidity = source.readInt();
            mGlonassXtraValidityInfo.mGlonassXtraAge = source.readInt();
        }

        if ((mValidityMask & BDS_XTRA_DATA_AVAILABLE) != 0 ) {
            mBdsXtraValidityInfo = new BdsXtraValidityInfo();
            mBdsXtraValidityInfo.mBdsXtraValidity = source.readLong();
            mBdsXtraValidityInfo.mBdsXtraAge = source.readInt();
        }

        if ((mValidityMask & GAL_XTRA_DATA_AVAILABLE) != 0 ) {
            mGalXtraValidityInfo = new GalXtraValidityInfo();
            mGalXtraValidityInfo.mGalXtraValidity = source.readLong();
            mGalXtraValidityInfo.mGalXtraAge = source.readInt();
        }

        if ((mValidityMask & QZSS_XTRA_DATA_AVAILABLE) != 0 ) {
            mQzssXtraValidityInfo = new QzssXtraValidityInfo();
            mQzssXtraValidityInfo.mQzssXtraValidity = source.readByte();
            mQzssXtraValidityInfo.mQzssXtraAge = source.readInt();
        }
    }

/** @} */ /* end_addtogroup IZatXTRADebugReport */

/** @addtogroup IZatXTRADebugReport
@{ */

    /**
     * Checks whether XTRA data for GPS is available.
     *
     * @return Boolean value to indicate whether XTRA data is available.
     */
    public boolean hasGpsXtraInfo() {
        return ((mValidityMask & GPS_XTRA_DATA_AVAILABLE) != 0);
    }

    /**
     * Checks whether XTRA data for GLONASS is available.
     *
     * @return Boolean value to indicate whether XTRA data is available.
     */
    public boolean hasGlonassXtraInfo() {
        return ((mValidityMask & GLONASS_XTRA_DATA_AVAILABLE) != 0);
    }

    /**
     * Checks whether XTRA data for BDS is available.
     *
     * @return Boolean value to indicate whether XTRA data is available.
     */
    public boolean hasBdsXtraInfo() {
        return ((mValidityMask & BDS_XTRA_DATA_AVAILABLE) != 0);
    }

    /**
     * Checks whether XTRA data for GAL is available.
     *
     * @return Boolean value to indicate whether XTRA data is available.
     */
    public boolean hasGalXtraInfo() {
        return ((mValidityMask & GAL_XTRA_DATA_AVAILABLE) != 0);
    }
    /**
     * Checks whether XTRA data for QZSS is available.
     *
     * @return Boolean value to indicate whether XTRA data is available.
     */
    public boolean hasQzssXtraInfo() {
        return ((mValidityMask & QZSS_XTRA_DATA_AVAILABLE) != 0);
    }

    /**
     * Gets the XTRA data information for GPS satellites.
     *
     * @return An object of the {@link GpsXtraValidityInfo} class is returned.
     */
    public  GpsXtraValidityInfo getXtraDataValidityForGPS() {
        return mGpsXtraValidityInfo;
    }

    /**
     * Gets the XTRA data information for GLONASS satellites.
     *
     * @return An object of the {@link GlonassXtraValidityInfo} class is returned.
     */
    public  GlonassXtraValidityInfo getXtraDataValidityForGlonass() {
        return mGlonassXtraValidityInfo;
    }

    /**
     * Gets the XTRA data information for BDS satellites.
     *
     * @return An object of the {@link BdsXtraValidityInfo} class is returned.
     */
    public  BdsXtraValidityInfo getXtraDataValidityForBDS() {
        return mBdsXtraValidityInfo;
    }

    /**
     * Gets the XTRA data information for GAL satellites.
     *
     * @return An object of the {@link GalXtraValidityInfo} class is returned.
     */
    public  GalXtraValidityInfo getXtraDataValidityForGal() {
        return mGalXtraValidityInfo;
    }

    /**
     * Gets the XTRA data information for QZSS satellites.
     *
     * @return An object of the {@link QzssXtraValidityInfo} class is returned.
     */
    public  QzssXtraValidityInfo getXtraDataValidityForQzss() {
        return mQzssXtraValidityInfo;
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

        dest.writeByte(mValidityMask);

        if ((mValidityMask & GPS_XTRA_DATA_AVAILABLE) != 0 ) {
            dest.writeInt(mGpsXtraValidityInfo.mGpsXtraValidity);
            dest.writeInt(mGpsXtraValidityInfo.mGpsXtraAge);
        }

       if ((mValidityMask & GLONASS_XTRA_DATA_AVAILABLE) != 0 ) {
            dest.writeInt(mGlonassXtraValidityInfo.mGlonassXtraValidity);
           dest.writeInt( mGlonassXtraValidityInfo.mGlonassXtraAge);
        }

        if ((mValidityMask & BDS_XTRA_DATA_AVAILABLE) != 0 ) {
            dest.writeLong(mBdsXtraValidityInfo.mBdsXtraValidity);
            dest.writeInt(mBdsXtraValidityInfo.mBdsXtraAge);
        }

        if ((mValidityMask & GAL_XTRA_DATA_AVAILABLE) != 0) {
            dest.writeLong(mGalXtraValidityInfo.mGalXtraValidity);
            dest.writeInt(mGalXtraValidityInfo.mGalXtraAge);
        }

        if ((mValidityMask & QZSS_XTRA_DATA_AVAILABLE) != 0 ) {
            dest.writeByte(mQzssXtraValidityInfo.mQzssXtraValidity);
            dest.writeInt(mQzssXtraValidityInfo.mQzssXtraAge);
        }
    }

/** @cond */

    public static final Parcelable.Creator<IZatXTRADebugReport> CREATOR =
            new Parcelable.Creator<IZatXTRADebugReport>() {
        @Override
        public IZatXTRADebugReport createFromParcel(Parcel source) {
             return new IZatXTRADebugReport(source);
        }
        @Override
        public IZatXTRADebugReport[] newArray(int size) {
            return new IZatXTRADebugReport[size];
        }

/** @endcond */

    };
};
/** @} */ /* end_addtogroup IZatXTRADebugReport */

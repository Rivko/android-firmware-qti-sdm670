/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import java.util.List;

import android.content.Context;
import android.location.Location;

/*
 * <p>Copyright (c) 2017 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * <br/>
 * <p><b>IZatWiFiDBReceiver</b> interface @version 1.0.0 </p>
 */

/** @addtogroup IZatWiFiDBReceiver
@{ */

 /** APIs for
 * injecting a Wi-Fi AP location database to a Qualcomm Location framework.
 *
 */
public abstract class IZatWiFiDBReceiver {
    protected final IZatWiFiDBReceiverResponseListener mResponseListener;

    /**
     * Constructor - IZatWiFiDBReceiver.
     *
     * @param listener Listener to receive Wi-Fi DB receiver
     *         responses. This parameter cannot be NULL, otherwise
     *         a {@link IZatIllegalArgumentException} is
     *         thrown.
     * @throws IZatIllegalArgumentException The listener parameter is NULL.
     * @return
     * None.
     */
    protected IZatWiFiDBReceiver(IZatWiFiDBReceiverResponseListener listener)
                                throws IZatIllegalArgumentException {
        if(null == listener) {
            throw new IZatIllegalArgumentException("Unable to obtain IZatWiFiDBReceiver instance");
        }
        mResponseListener = listener;
    }

    /**
     * Requests a list of access points.
     * <p>
     * This function enables Wi-Fi database providers to request a list of APs
     * that require location information.
     * </p>
     *
     * @param expire_in_days Number of days until
     *                        the associated location of an AP to be
     *                        fetched by this request (if
     *                        available) will expire. Optional parameter.
     *                        If 0 is provided, only APs that have an
     *                        already expired location or no
     *                        location associated with it will be
     *                        fetched.
     * @return
     * None.
     */
    public abstract void requestAPList(int expire_in_days);

    /**
     * Requests a Wi-Fi database update.
     * <p>
     * This function enables a Wi-Fi database provider to insert a list of APs
     * with location information.
     * </p>
     *
     * @param location_data Location information of access points.
     *                      If not available, a NULL/empty list can be
     *                      provided.
     * @param special_info Special information on an access point. If
     *                     not available, a NULL/empty list can be
     *                     provided.
     * @param days_valid Number of days for which location_data and
     *                   special_info will be valid. Optional
     *                   parameter. Defaults to 15 days if 0 is
     *                   provided.
     *
     * @return
     * None.
     */
    public abstract void pushWiFiDB(List<IZatAPLocationData> location_data,
                                    List<IZatAPSpecialInfo> special_info,
                                    int days_valid);

/** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * Interface class IZatWiFiDBReceiverResponseListener.
     *
     * This interface
     * receives responses from a Wi-Fi database receiver in a Qualcomm
     * Location framework.
     */
    public interface IZatWiFiDBReceiverResponseListener {
        /**
         * Response to an AP list request.
         * <p>
         * This API is called by the underlying service back
         * to applications when a list of APs is available.
         * Applications should implement this interface.</p>
         *
         * @param ap_list List of APs.
         *
         * @return
         * None.
         */
        void onAPListAvailable(List<IZatAPInfo> ap_list);

        /**
         * Response for an AP location injection request.
         * <p>
         * This API is called by the underlying service back
         * to applications when an AP location injection completes.
         * Applications should implement this interface.</p>
         *
         * @param is_success Injection of AP locations success or
         *                   failure.
         * @param error Error details if the AP location injection
         *                   failed.
         *
         * @return
         * None.
         */
        void onStatusUpdate(boolean is_success, String error);

        /**
         * Service request to a Wi-Fi database provider.
         * <p>
         * This API is called by the underlying service back
         * to applications when they need service. Applications should
         * implement this interface.</p>
         *
         * @return
         * None.
         */
        void onServiceRequest();
    }

    /** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * Class IZatAPInfo.
     */
    public static class IZatAPInfo {
        String mMacAddress;
        IZatAPInfoExtra mExtra;

        /**
         * IZatAPInfo constructor.
         *
         * @param mac MAC address if the Wi-Fi access point is in the form of a
         *            six-byte MAC address: {@code XX:XX:XX:XX:XX:XX}
         * @param extra Extra context information for this AP.
         *
         * @return
         * None.
         */
        public IZatAPInfo(String mac, IZatAPInfoExtra extra) {
            mMacAddress = mac;
            mExtra = extra;
        }

        /**
         * Gets the MAC address of this AP.
         *
         * @return String in the form of a six-byte MAC address: {@code
         *         XX:XX:XX:XX:XX:XX}
         */
        public String getMacAddress() {
            return mMacAddress;
        }

        /**
         * Gets extra context information on this AP.
         *
         * @return IZatAPInfoExtra.
         */
        public IZatAPInfoExtra getExtra() {
            return mExtra;
        }

        /**
         * Checks whether extra information for this AP is available
         *
         * @return Boolean TRUE or FALSE.
         */
        public boolean isExtraAvailable() {
            return (null == mExtra ? false : mExtra.isAvailable());
        }
    }

/** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * Class IZatAPInfoExtra.
     */
    public static class IZatAPInfoExtra {
        IZatCellInfo mCellInfo;
        IZatAPSSIDInfo mSSID;

        /**
         *  Constructor - IZatAPInfoExtra.
         *
         * @param cellInfo Cell information in which this AP is observed.
         * @param ssid SSID of this access point.
         *
         * @return
         * None.
         */
        public IZatAPInfoExtra(IZatCellInfo cellInfo, IZatAPSSIDInfo ssid) {
            mCellInfo = cellInfo;
            mSSID = ssid;
        }

        /**
         * Gets the cell information.
         * <p>
         *  Gets the cell information in which this AP is observed. This information
         *  is currently not supported.</p>
         *
         * @return IZatCellInfo.
         */
        public IZatCellInfo getCellInfo() {
            return mCellInfo;
        }

        /**
         * Gets the SSID.
         * <p>
         *  Returns the service set identifier (SSID) of this AP.</p>
         *
         * @return IZatAPSSIDInfo.
         */
        public IZatAPSSIDInfo getSSID() {
            return mSSID;
        }

        /**
         * Checks whether extra context information is available for this AP.
         *
         * @return Boolean TRUE or FALSE.
         */
        public boolean isAvailable() {
            return ((null != mSSID) || (null != mCellInfo));
        }
    }

/** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * Class IZatCellInfo.
     */
    public static class IZatCellInfo {
        public final int mRegionID1;
        public final int mRegionID2;
        public final int mRegionID3;
        public final int mRegionID4;

        /**
         * Enumeration of IZat cell types.
         */
        public enum IZatCellTypes {
            GSM,     /**< GSM cell type. */
            CDMA,    /**< CDMA cell type. */
            WCDMA,   /**< WCDMA cell type. */
            LTE      /**< LTE cell type. */
        }
        public final IZatCellTypes mType;

        /**
         * Constructor - IZatCellInfo.
         *
         * @param regionID1 Mobile country code (MCC). For CDMA set to 0.
         * @param regionID2 Mobile network code (MNC). For CDMA, set to
         *                  system ID (SID). For WCDMA, set to 0 if not
         *                  available.
         * @param regionID3 GSM: Local area code (LAC) \n
         *                  WCDMA: LAC; set to 0 if not available \n
         *                  CDMA: Network ID (NID) \n
         *                  LTE: Tracking area code (TAC); set to 0 if not available
         * @param regionID4 Cell ID (CID); for CDMA, set to base station
         *                  ID (BSID).
         * @param type Cell type.
         *
         * @return
         * None.
         */
        public IZatCellInfo(int regionID1, int regionID2, int regionID3,
                            int regionID4, IZatCellTypes type) {
            mRegionID1 = regionID1;
            mRegionID2 = regionID2;
            mRegionID3 = regionID3;
            mRegionID4 = regionID4;
            mType = type;
        }
    }

/** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * Class IZatAPSSIDInfo.
     */
    public static class IZatAPSSIDInfo {

        public final byte[] mSSID;

        /**
         * Contructor - IZatAPSSIDInfo.
         *
         * @param ssid Available bytes in the AP's SSID. This can be
         *             smaller than the actual SSID size. The number of
         *             available bytes from the SSID is indicated by
         *             validBytesCount.
         * @param validBytesCount Actual number of AP SSID bytes
         *                         that are available in the SSID.
         *
         * @return
         * None.
         */
        public IZatAPSSIDInfo(byte[] ssid, short validBytesCount) {
            mSSID = new byte[validBytesCount];
        }
    }

/** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * Class IZatAPLocationData.
     */
    public static class IZatAPLocationData {

        String mMacAddress;
        float mLatitude;
        float mLongitude;
        float mMaxAntenaRange;
        float mHorizontalError;

        /**
         * Enumeration of IZat reliability types.
         */
        public enum IZatReliablityTypes {
            VERY_LOW,    /**< The probability of a position outlier is
                              1 in one hundred or even more likely. */
            LOW,         /**< The probability of a position outlier is about
                              1 in one thousand. */
            MEDIUM,      /**< The probability of a position outlier is about
                              1 in 100 thousand.
            HIGH,        /**< The probability of a position outlier is about
                              1 in 10 million.
            VERY_HIGH    /**< The probability of a position outlier is about
                              1 in a thousand million. \n
                              Until sufficient experience is obtained, the reliability
                              input value should remain unset or set to LOW. */
        }
        IZatReliablityTypes mReliability;

        public static final int IZAT_AP_LOC_WITH_LAT_LON         = 0x0;
        public static final int IZAT_AP_LOC_MAR_VALID            = 0x1;
        public static final int IZAT_AP_LOC_HORIZONTAL_ERR_VALID = 0x2;
        public static final int IZAT_AP_LOC_RELIABILITY_VALID    = 0x4;
        int mValidBits;

        /**
         * Constructor - IZatAPLocationData.
         *
         * @param mac MAC address in the form of a six-byte MAC address string: \n
         *         {@code XX:XX:XX:XX:XX:XX}
         * @param latitude Latitude of the AP.
         * @param longitude Longitude of the AP.
         * @return
         * None.
         */
        public IZatAPLocationData(String mac, float latitude, float longitude) {
            mMacAddress = mac;
            mLatitude = latitude;
            mLongitude = longitude;
            mValidBits = IZAT_AP_LOC_WITH_LAT_LON;
        }

        /**
         * Sets the latitude.
         *
         * @param latitude Latitude of the AP.
         *
         * @return
         * None.
         */
        public void setLatitude(float latitude) {
            mLatitude = latitude;
        }

        /**
         * Sets the longitude.
         *
         * @param longitude Longitude of the AP.
         *
         * @return
         * None.
         */
        public void setLongitude(float longitude) {
            mLongitude = longitude;
        }


        /**
         * Sets the maximum antenna range of the AP.
         *
         * @param mar Maximum antenna range of the AP.
         *
         * @return
         * None.
         */
        public void setMaxAntenaRange(float mar) {
            mMaxAntenaRange = mar;
            mValidBits |= IZAT_AP_LOC_MAR_VALID;
        }

        /**
         * Sets the horizontal error.
         *
         * @param he Horizontal error.
         *
         * @return
         * None.
         */
        public void setHorizontalError(float he) {
            mHorizontalError = he;
            mValidBits |= IZAT_AP_LOC_HORIZONTAL_ERR_VALID;
        }

        /**
         * Sets the reliability of the provided location information.
         *
         * @param reliability Reliability values; see enum IZatReliablityTypes.
         *
         * @return
         * None.
         */
        public void setReliability(IZatReliablityTypes reliability) {
            mReliability = reliability;
            mValidBits |= IZAT_AP_LOC_RELIABILITY_VALID;
        }

        /**
         * Gets the MAC address.
         * <p>
         *  Gets the MAC address in the form of a six-byte MAC address string:
         *  {@code XX:XX:XX:XX:XX:XX}
         * @return
         * MAC address string.
         */
        public String getMacAddress() {
            return mMacAddress;
        }

        /**
         * Gets the latitude.
         *
         * @return
         * Latitude as a float value.
         */
        public float getLatitude() {
            return mLatitude;
        }

        /**
         * Gets the longitude.
         *
         * @return
         * Longitude as a float value.
         */
        public float getLongitude() {
            return mLongitude;
        }

        /**
         * Gets the maximum antenna range.
         *
         * @throws IZatStaleDataException Range data is stale.
         * @return
         * Maximum antenna range as a float value.
         */
        public float getMaxAntenaRange() throws IZatStaleDataException {
            if(0 == (IZAT_AP_LOC_MAR_VALID & mValidBits)) {
                throw new IZatStaleDataException("Maximum Antena Range information is not valid");
            }
            return mMaxAntenaRange;
        }

        /**
         * Gets the horizontal error.
         *
         * @throws IZatStaleDataException Error data is stale.
         * @return
         * Horizontal error as a float value.
         */
        public float getHorizontalError() throws IZatStaleDataException {
            if(0 == (IZAT_AP_LOC_HORIZONTAL_ERR_VALID & mValidBits)) {
                throw new IZatStaleDataException("Horizontal error information is not valid");
            }
            return mHorizontalError;
        }

        /**
         * Gets the reliability.
         *
         * @throws IZatStaleDataException  Reliability data is stale.
         * @return
         * A value from IZatReliablityTypes.
         */
        public IZatReliablityTypes getReliability() throws IZatStaleDataException {
            if(0 == (IZAT_AP_LOC_RELIABILITY_VALID & mValidBits)) {
                throw new IZatStaleDataException("Reliability information is not valid");
            }
            return mReliability;
        }

    }

/** @} */ /* end_addtogroup IZatWiFiDBReceiver */

/** @addtogroup IZatWiFiDBReceiver
@{ */

    /**
     * class IZatAPSpecialInfo.
     */
    public static class IZatAPSpecialInfo {

        public final String mMacAddress;

        /**
         * Enumeration of IZatAPSpecialInfoTypes.
         */
        public enum IZatAPSpecialInfoTypes {
          /** No information is available.
           */
            NO_INFO_AVAILABLE,
          /** Moving AP.
           */
            MOVING_AP
        }
        public final IZatAPSpecialInfoTypes mInfo;


        /**
         * Constructor - IZatAPSpecialInfo.
         *
         * @param mac MAC address in the form of a six-byte MAC address string:
         *         {@code XX:XX:XX:XX:XX:XX}
         * @param info Information on this AP.
         * @return
         * None.
         */
        public IZatAPSpecialInfo(String mac, IZatAPSpecialInfoTypes info) {
            mMacAddress = mac;
            mInfo = info;
        }
    }

}
/** @} */ /* end_addtogroup IZatWiFiDBReceiver */
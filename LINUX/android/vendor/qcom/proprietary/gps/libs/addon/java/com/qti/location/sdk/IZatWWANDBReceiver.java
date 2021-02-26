/* ======================================================================
*  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
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
 * <p><b>IZatWWANDBReceiver</b> interface @version 1.0.0 <p>
 */

/** @addtogroup IZatWWANDBReceiver
@{ */

/** API for
 * injecting a WWAN Cell Location database to a Qualcomm Location framework.
 */
public abstract class IZatWWANDBReceiver {
    protected final IZatWWANDBReceiverResponseListener mResponseListener;

    /**
     * <p>
     * Constructor - IZatWWANDBReceiver. </p>
     *
     * @param listener Listener to receive WWAN DB Receiver
     *         responses. This parameter cannot be NULL, otherwise
     *         a {@link IZatIllegalArgumentException} is
     *         thrown.
     * @throws IZatIllegalArgumentException The listener parameter is NULL.
     * @return
     * None.
     */
    protected IZatWWANDBReceiver(IZatWWANDBReceiverResponseListener listener)
                                throws IZatIllegalArgumentException {
        if(null == listener) {
            throw new IZatIllegalArgumentException("Unable to obtain IZatWWANDBReceiver instance");
        }
        mResponseListener = listener;
    }

    /**
     * Requests list of base stations.
     * <p>
     * This enables WWAN database providers to request a list of BSs
     * that require location information.
     * </p>
     *
     * @param expire_in_days Number of days until
     *                        the associated location of a BS (if
     *                        available) will expire.
     *                        Optional parameter.
     *                        If 0 is provided, only BSs that have an
     *                        already expired location or no
     *                        location associated with it will be
     *                        fetched.
     * @return
     * None.
     */
    public abstract void requestBSList(int expire_in_days);

    /**
     * Requests a WWAN DB update.
     * <p>
     * This enables a WWAN database provider to insert a list of BSs
     * with location information.
     * </p>
     *
     * @param location_data Location information of base stations.
     *                          If not available, a NULL/empty list can be
     *                          provided.
     * @param special_info Special information on a base station. If
     *                         not available, a NULL/empty list can be
     *                         provided.
     * @param days_valid  Number of days for which location_data and
     *                   special_info will be valid. Optional
     *                   parameter. Defaults to 15 days if 0 is
     *                   provided.
     * @return
     * None.
     */
    public abstract void pushWWANDB(List<IZatBSLocationData> location_data,
                                    List<IZatBSSpecialInfo> special_info,
                                    int days_valid);

/** @} */ /* end_addtogroup IZatWWANDBReceiver */

/** @addtogroup IZatWWANDBReceiver
@{ */

    /**
     * Interface class IZatWWANDBReceiverResponseListener.
     *
     * <p>This interface
     * receives responses from a WWAN database receiver in a Qualcomm
     * Location framework.</p>
     */
    public interface IZatWWANDBReceiverResponseListener {
        /**
         * Response to a BS list request.
         * <p>
         * This API is called by the underlying service back
         * to applications when a list of BSs is available.
         * Applications should implement this interface.</p>
         *
         * @param bs_list List of BSs.
         *
         * @return
         * None.
         */
        void onBSListAvailable(List<IZatBSInfo> bs_list);

        /**
         * Response for a BS location injection request.
         * <p>
         * This API is called by the underlying service back
         * to applications when a BS location injection completes.
         * Applications should implement this interface.</p>
         *
         * @param is_success Injection of BS locations success or
         *                        failure.
         * @param error Error details if the BS location injection
         *                   failed.
         *
         * @return
         * None.
         */
        void onStatusUpdate(boolean is_success, String error);

        /**
         * Service request to a WWAN DB provider.
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

/** @} */ /* end_addtogroup IZatWWANDBReceiver */

/** @addtogroup IZatWWANDBReceiver
@{ */

    /**
     * Class IZatBSInfo.
     */
    public static class IZatBSInfo {
        IZatBSCellInfo mCellInfo;

        /**
         * IZatBSInfo constructor.
         *
         * @param cellInfo Cell information for this BS.
         *
         * @return
         * None.
         */
        public IZatBSInfo(IZatBSCellInfo cellInfo) {
            mCellInfo = cellInfo;
        }

        /**
         * Gets cell information.
         * <p>
         *  Gets the cell information in which this BS is observed. This information
         *  is no longer supported.</p>
         *
         * @return IZatBSCellInfo.
         */
        public IZatBSCellInfo getCellInfo() {
            return mCellInfo;
        }
    }

/** @} */ /* end_addtogroup IZatWWANDBReceiver */

/** @addtogroup IZatWWANDBReceiver
@{ */

    /**
     * Class IZatBSCellInfo.
     */
    public static class IZatBSCellInfo {
        public final int mRegionID1;
        public final int mRegionID2;
        public final int mRegionID3;
        public final int mRegionID4;

        /**
         * Enumeration of BS cell types.
         */
        public enum IZatBSCellTypes {
            GSM,     /**< GSM cell type. */
            CDMA,    /**< CDMA cell type. */
            WCDMA,   /**< WCDMA cell type. */
            LTE      /**< LTE cell type. */
        }
        public final IZatBSCellTypes mType;

        /**
         * Constructor - IZatBSCellInfo.
         *
         * @param regionID1 Mobile country code (MCC). For CDMA set to 0.
         * @param regionID2 Mobile network code (MNC). For CDMA, set to
         *                      system ID (SID). For WCDMA, set to 0 if not
         *                      available.
         * @param regionID3 GSM: Local area code (LAC) \n
         *                      WCDMA: LAC; set to 0 if not available \n
         *                      CDMA: Network ID (NID) \n
         *                      LTE: Tracking area code (TAC); set to 0 if not available
         * @param regionID4 Cell ID (CID); for CDMA, set to Base Station
         *                      ID (BSID).
         * @param type Cell type.
         */
        public IZatBSCellInfo(int regionID1, int regionID2, int regionID3,
                              int regionID4, IZatBSCellTypes type) {
            mRegionID1 = regionID1;
            mRegionID2 = regionID2;
            mRegionID3 = regionID3;
            mRegionID4 = regionID4;
            mType = type;
        }
    }

/** @} */ /* end_addtogroup IZatWWANDBReceiver */

/** @addtogroup IZatWWANDBReceiver
@{ */

    /**
     * Class IZatBSLocationData.
     */
    public static class IZatBSLocationData {

        int mCellType;
        int mCellRegionID1;
        int mCellRegionID2;
        int mCellRegionID3;
        int mCellRegionID4;

        float mLatitude;
        float mLongitude;

        /**
         * Enumeration of reliablity types.
         */
        public enum IZatReliablityTypes {
            VERY_LOW,    /**< The probability of a position outlier is 1
                              1 in one hundred or even more likely. */
            LOW,         /**< The probability of a position outlier is about
                              1 in one thousand. */
            MEDIUM,      /**< The probability of a position outlier is about
                              1 in 100 thousand. */
            HIGH,        /**< The  probability of a position outlier is about
                              1 in 10 million. */
            VERY_HIGH    /**< The probability of a position outlier is about 1 in
                              1 in a thousand million. \n
                              Until sufficient experience is obtained, the reliability
                              input value should remain unset or set to LOW. */
        }

        // BS Location masks
        public static final int IZAT_BS_LOC_WITH_LAT_LON = 0x0;
        public static final int IZAT_BS_LOC_HORIZONTAL_COV_RADIUS_VALID = 0x1;
        public static final int IZAT_BS_LOC_HORIZONTAL_CONFIDENCE_VALID = 0x2;
        public static final int IZAT_BS_LOC_HORIZONTAL_RELIABILITY_VALID = 0x4;
        public static final int IZAT_BS_LOC_ALTITUDE_VALID = 0x8;
        public static final int IZAT_BS_LOC_ALTITUDE_UNCERTAINTY_VALID = 0x10;
        public static final int IZAT_BS_LOC_ALTITUDE_CONFIDENCE_VALID = 0x20;
        public static final int IZAT_BS_LOC_ALTITUDE_RELIABILITY_VALID = 0x40;
        int mValidBits;

        float mHorizontalCoverageRadius;
        int mHorizontalConfidence;
        IZatReliablityTypes mHorizontalReliability;
        float mAltitude;
        float mAltitudeUncertainty;
        int mAltitudeConfidence;
        IZatReliablityTypes mAltitudeReliability;

        /**
         * Constructor - IZatBSLocationData.
         *
         * @param cellType Cell type.
         * @param cellRegionID1 Cell region ID 1.
         * @param cellRegionID2 Cell region ID 2.
         * @param cellRegionID3 Cell region ID 3.
         * @param cellRegionID4 Cell region ID 4.
         * @param latitude Latitude of the BS.
         * @param longitude Longitude of the BS.
         */
        public IZatBSLocationData(int cellType,
                                  int cellRegionID1, int cellRegionID2,
                                  int cellRegionID3, int cellRegionID4,
                                  float latitude, float longitude) {

            mCellType = cellType;
            mCellRegionID1 = cellRegionID1;
            mCellRegionID2 = cellRegionID2;
            mCellRegionID3 = cellRegionID3;
            mCellRegionID4 = cellRegionID4;
            mLatitude = latitude;
            mLongitude = longitude;
            mValidBits = IZAT_BS_LOC_WITH_LAT_LON;
        }

        /**
         * Sets the cell type.
         *
         * @param ct Cell type.
         *
         * @return None.
         */
        public void setCellType(int ct) {
            mCellType = ct;
        }

        /**
         * Sets the latitude
         *
         * @param latitude Latitude of the BS.
         *
         * @return None.
         */
        public void setLatitude(float latitude) {
            mLatitude = latitude;
        }

        /**
         * Sets the longitude.
         *
         * @param longitude Longitude of the BS.
         *
         * @return None.
         */
        public void setLongitude(float longitude) {
            mLongitude = longitude;
        }

        /**
         * Sets the horizontal coverage radius.
         *
         * @param hcr Horizontal coverage radius.
         *
         * @return None.
         */
        public void setHorizontalCoverageRadius(float hcr) {
            mHorizontalCoverageRadius = hcr;
            mValidBits |= IZAT_BS_LOC_HORIZONTAL_COV_RADIUS_VALID;
        }

        /**
         * Sets the horizontal confidence of the location information provided.
         *  Range 0-100
         *  </p>
         * @param hc Horizontal confidence. Range: 0 to 100.
         *
         * @return None.
         */
        public void setHorizontalConfidence(int hc) {
            mHorizontalConfidence = hc;
            mValidBits |= IZAT_BS_LOC_HORIZONTAL_CONFIDENCE_VALID;
        }

        /**
         * Sets the horizontal reliability. See enum IZatReliablityTypes.
         *
         * @param reliability Horizontal reliability.
         *
         * @return None.
         */
        public void setHorizontalReliability(IZatReliablityTypes reliability) {
            mHorizontalReliability = reliability;
            mValidBits |= IZAT_BS_LOC_HORIZONTAL_RELIABILITY_VALID;
        }

        /**
         * Sets the altitude.
         *
         * @param alt Altitude.
         *
         * @return None.
         */
        public void setAltitude(float alt) {
            mAltitude = alt;
            mValidBits |= IZAT_BS_LOC_ALTITUDE_VALID;
        }

        /**
         * Sets the altitude uncertainty.
         *
         * @param alu Altitude uncertainty.
         *
         * @return None.
         */
        public void setAltitudeUncertainty(float alu) {
            mAltitudeUncertainty = alu;
            mValidBits |= IZAT_BS_LOC_ALTITUDE_UNCERTAINTY_VALID;
        }

        /**
         * Sets the altitude confidence.
         *
         * @param alc Altitude confidence.
         *
         * @return None.
         */
        public void setAltitudeConfidence(int alc) {
            mAltitudeConfidence = alc;
            mValidBits |= IZAT_BS_LOC_ALTITUDE_CONFIDENCE_VALID;
        }

        /**
         * Sets the altitude reliability. See enum IZatReliablityTypes.
         *
         * @param reliability Altitude reliability.
         *
         * @return None.
         */
        public void setAltitudeReliability(IZatReliablityTypes reliability) {
            mAltitudeReliability = reliability;
            mValidBits |= IZAT_BS_LOC_ALTITUDE_RELIABILITY_VALID;
        }

        /**
         * Gets the cell type.
         *
         * @return Cell type as an integer.
         */
        public int getCellType() {
            return mCellType;
        }

        /**
         * Gets cell region ID 1.
         *
         * @return CellRegionID1 as an integer.
         */
        public int getCellRegionID1() {
            return mCellRegionID1;
        }

        /**
         * Gets cell region ID 2.
         *
         * @return CellRegionID2 as an integer.
         */
        public int getCellRegionID2() {
            return mCellRegionID2;
        }

        /**
         * Gets cell region ID 3.
         *
         * @return CellRegionID3 as an integer.
         */
        public int getCellRegionID3() {
            return mCellRegionID3;
        }

        /**
         * Gets cell region ID 4.
         *
         * @return CellRegionID4 as an integer.
         */
        public int getCellRegionID4() {
            return mCellRegionID4;
        }

        /**
         * Gets the latitude.
         *
         * @return Latitude as a float value.
         */
        public float getLatitude() {
            return mLatitude;
        }

        /**
         * Gets the longitude.
         *
         * @return Longitude as a float value.
         */
        public float getLongitude() {
            return mLongitude;
        }

        /**
         * Gets the horizontal coverage radius.
         *
         * @throws IZatStaleDataException Horizontal coverage information is not valid.
         * @return Horizontal coverage radius as a float value.
         */
        public float getHorizontalCoverageRadius() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_HORIZONTAL_COV_RADIUS_VALID & mValidBits)) {
                throw new IZatStaleDataException("Horizontal coverage information is not valid");
            }
            return mHorizontalCoverageRadius;
        }

        /**
         * Gets the horizontal confidence.
         *
         * @throws IZatStaleDataException Horizontal confidence information is not valid.
         * @return Horizontal confidence as an integer.
         */
        public int getHorizontalConfidence() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_HORIZONTAL_CONFIDENCE_VALID & mValidBits)) {
                throw new IZatStaleDataException("Horizontal confidence information is not valid");
            }
            return mHorizontalConfidence;
        }

        /**
         * Gets the horizontal reliability.
         *
         * @throws IZatStaleDataException Horizontal reliability information is not valid.
         * @return Horizontal reliability. See enum IZatReliablityTypes.
         */
        public IZatReliablityTypes getHorizontalReliability() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_HORIZONTAL_RELIABILITY_VALID & mValidBits)) {
                throw new IZatStaleDataException("Horizontal reliability information is not valid");
            }
            return mHorizontalReliability;
        }

        /**
         * Gets the altitude.
         *
         * @throws IZatStaleDataException Altitude information is not valid.
         * @return Altitude as a float value.
         */
        public float getAltitude() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_ALTITUDE_VALID & mValidBits)) {
                throw new IZatStaleDataException("Altitude information is not valid");
            }
            return mAltitude;
        }

        /**
         * Gets the altitude uncertainty.
         *
         * @throws IZatStaleDataException Altitude uncertainty information is not valid.
         * @return Altitude uncertainty as a float value.
         */
        public float getAltitudeUncertainty() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_ALTITUDE_UNCERTAINTY_VALID & mValidBits)) {
                throw new IZatStaleDataException("Altitude uncertainty information is not valid");
            }
            return mAltitudeUncertainty;
        }

        /**
         * Gets the altitude confidence.
         *
         * @throws IZatStaleDataException Altitude confidence information is not valid.
         * @return Altitude confidence as an integer.
         */
        public int getAltitudeConfidence() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_ALTITUDE_CONFIDENCE_VALID & mValidBits)) {
                throw new IZatStaleDataException("Altitude confidence information is not valid");
            }
            return mAltitudeConfidence;
        }

        /**
         * Gets the altitude reliability.
         *
         * @throws IZatStaleDataException Altitude reliability information is not valid.
         * @return Altitude reliability. See enum IZatReliablityTypes.
         */
        public IZatReliablityTypes getAltitudeReliability() throws IZatStaleDataException {
            if(0 == (IZAT_BS_LOC_ALTITUDE_RELIABILITY_VALID & mValidBits)) {
                throw new IZatStaleDataException("Altitude reliability information is not valid");
            }
            return mAltitudeReliability;
        }
    }

/** @} */ /* end_addtogroup IZatWWANDBReceiver */

/** @addtogroup IZatWWANDBReceiver
@{ */

    /**
     * Class IZatBSSpecialInfo.
     */
    public static class IZatBSSpecialInfo {

        int mCellType;
        int mCellRegionID1;
        int mCellRegionID2;
        int mCellRegionID3;
        int mCellRegionID4;

        /**
         * Enumeration of BS special information types.
         */
        public enum IZatBSSpecialInfoTypes {
            NO_INFO_AVAILABLE,    /**< No information is available. */
            MOVING_BS             /**< Moving BS. */
        }
        public final IZatBSSpecialInfoTypes mInfo;


        /**
         * Constructor - IZatBSSpecialInfo.
         *
         * @param cellType Cell type.
         * @param cellRegionID1 Cell region ID 1.
         * @param cellRegionID2 Cell region ID 2.
         * @param cellRegionID3 Cell region ID 3.
         * @param cellRegionID4 Cell region ID 4.
         * @param info Information on this BS.
         *
         * @return None.
         *
         */
        public IZatBSSpecialInfo(int cellType,
                                 int cellRegionID1, int cellRegionID2,
                                 int cellRegionID3, int cellRegionID4,
                                 IZatBSSpecialInfoTypes info) {
            mCellType = cellType;
            mCellRegionID1 = cellRegionID1;
            mCellRegionID2 = cellRegionID2;
            mCellRegionID3 = cellRegionID3;
            mCellRegionID4 = cellRegionID4;
            mInfo = info;
        }

        /**
         * Gets the cell type.
         *
         * @return Cell type as an integer.
         */
        public int getCellType() {
            return mCellType;
        }

        /**
         * Gets cell region ID 1.
         *
         * @return CellRegionID1 as an integer.
         */
        public int getCellRegionID1() {
            return mCellRegionID1;
        }

        /**
         * Gets cell region ID 2.
         *
         * @return CellRegionID2 as an integer.
         */
        public int getCellRegionID2() {
            return mCellRegionID2;
        }

        /**
         * Gets cell region ID 3.
         *
         * @return CellRegionID3 as an integer.
         */
        public int getCellRegionID3() {
            return mCellRegionID3;
        }

        /**
         * Gets cell region ID 4.
         *
         * @return CellRegionID4 as an integer
         */
        public int getCellRegionID4() {
            return mCellRegionID4;
        }
    }

}
/** @} */ /* end_addtogroup IZatWWANDBReceiver */

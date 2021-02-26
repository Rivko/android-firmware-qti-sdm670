/* ======================================================================
*  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/** @addtogroup IZatTestService
@{ */

/**
 * Class IZatTestService. (FOR INTERNAL USE ONLY)
 *
 */
public interface IZatTestService {
    /* Aiding data flags */
    static final long GNSS_AIDING_DATA_POSITION             = (1L << 1);
    static final long GNSS_AIDING_DATA_TIME                 = (1L << 2);
    static final long GNSS_AIDING_DATA_IONO                 = (1L << 3);
    static final long GNSS_AIDING_DATA_UTC                  = (1L << 4);
    static final long GNSS_AIDING_DATA_HEALTH               = (1L << 5);
    static final long GNSS_AIDING_DATA_SADATA               = (1L << 6);
    static final long GNSS_AIDING_DATA_RTI                  = (1L << 7);
    static final long GNSS_AIDING_DATA_FREQ_BIAS_EST        = (1L << 8);
    static final long GNSS_AIDING_DATA_CELLDB_INFO          = (1L << 9);
    // GPS
    static final long GNSS_AIDING_DATA_GPS_EPHEMERIS        = (1L << 10);
    static final long GNSS_AIDING_DATA_GPS_ALMANAC          = (1L << 11);
    static final long GNSS_AIDING_DATA_GPS_SVDIR            = (1L << 12);
    static final long GNSS_AIDING_DATA_GPS_SVSTEER          = (1L << 13);
    static final long GNSS_AIDING_DATA_GPS_TIME             = (1L << 14);
    static final long GNSS_AIDING_DATA_GPS_ALMANAC_CORR     = (1L << 15);
    // GLONASS
    static final long GNSS_AIDING_DATA_GLO_EPHEMERIS        = (1L << 16);
    static final long GNSS_AIDING_DATA_GLO_ALMANAC          = (1L << 17);
    static final long GNSS_AIDING_DATA_GLO_SVDIR            = (1L << 18);
    static final long GNSS_AIDING_DATA_GLO_SVSTEER          = (1L << 19);
    static final long GNSS_AIDING_DATA_GLO_TIME             = (1L << 20);
    static final long GNSS_AIDING_DATA_GLO_ALMANAC_CORR     = (1L << 21);
    // BeiDou
    static final long GNSS_AIDING_DATA_BDS_EPHEMERIS        = (1L << 22);
    static final long GNSS_AIDING_DATA_BDS_ALMANAC          = (1L << 23);
    static final long GNSS_AIDING_DATA_BDS_SVDIR            = (1L << 24);
    static final long GNSS_AIDING_DATA_BDS_SVSTEER          = (1L << 25);
    static final long GNSS_AIDING_DATA_BDS_TIME             = (1L << 26);
    static final long GNSS_AIDING_DATA_BDS_ALMANAC_CORR     = (1L << 27);

    static final long GNSS_AIDING_DATA_ALL                  = ~0L;
/** @} */
/** @addtogroup IZatTestService
@{ */

    /**
     * Deletes GNSS aiding data.
     * <p>
     * This API deletes any aiding data obtained by GNSS providers.
     * </p>
     *
     * @param flags  Mask of aiding data elements to delete. This parameter
     *         can not be 0, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown. \n
     *         Possible values (one or more): \n
     *         {@link #GNSS_AIDING_DATA_POSITION}, \n
     *         {@link #GNSS_AIDING_DATA_TIME}, \n
     *         {@link #GNSS_AIDING_DATA_IONO}, \n
     *         {@link #GNSS_AIDING_DATA_UTC}, \n
     *         {@link #GNSS_AIDING_DATA_HEALTH}, \n
     *         {@link #GNSS_AIDING_DATA_SADATA}, \n
     *         {@link #GNSS_AIDING_DATA_RTI}, \n
     *         {@link #GNSS_AIDING_DATA_FREQ_BIAS_EST}, \n
     *         {@link #GNSS_AIDING_DATA_CELLDB_INFO}, \n
     *         {@link #GNSS_AIDING_DATA_GPS_EPHEMERIS}, \n
     *         {@link #GNSS_AIDING_DATA_GPS_ALMANAC}, \n
     *         {@link #GNSS_AIDING_DATA_GPS_SVDIR}, \n
     *         {@link #GNSS_AIDING_DATA_GPS_SVSTEER}, \n
     *         {@link #GNSS_AIDING_DATA_GPS_TIME}, \n
     *         {@link #GNSS_AIDING_DATA_GPS_ALMANAC_CORR}, \n
     *         {@link #GNSS_AIDING_DATA_GLO_EPHEMERIS}, \n
     *         {@link #GNSS_AIDING_DATA_GLO_ALMANAC}, \n
     *         {@link #GNSS_AIDING_DATA_GLO_SVDIR}, \n
     *         {@link #GNSS_AIDING_DATA_GLO_SVSTEER}, \n
     *         {@link #GNSS_AIDING_DATA_GLO_TIME}, \n
     *         {@link #GNSS_AIDING_DATA_GLO_ALMANAC_CORR}, \n
     *         {@link #GNSS_AIDING_DATA_BDS_EPHEMERIS}, \n
     *         {@link #GNSS_AIDING_DATA_BDS_ALMANAC}, \n
     *         {@link #GNSS_AIDING_DATA_BDS_SVDIR}, \n
     *         {@link #GNSS_AIDING_DATA_BDS_SVSTEER}, \n
     *         {@link #GNSS_AIDING_DATA_BDS_TIME}, \n
     *         {@link #GNSS_AIDING_DATA_BDS_ALMANAC_CORR}, \n
     *         {@link #GNSS_AIDING_DATA_ALL}
     *
     * @throws IZatIllegalArgumentException The flags parameter is 0.
     *
     * @return
     * None.
     */
    void deleteAidingData(long flags)
                          throws IZatIllegalArgumentException;

    /**
     * Enables or disable the XTRA client throttle.
     * <p>
     * This API enables applications to enable/disable the XTRA client throttle.
     * </p>
     *
     * @param enabled TRUE to enable the XTRA throttle;
                       FALSE to disable the XTRA throttle.
     *
     * @return
     * None.
     */
    void updateXtraThrottle(boolean enabled);

    /**
     * Starts listening for changes in the maximum power allocated by FLP.
     * <p>
     * This API enables applications to stay informed of the maximum power
     * allocated by FLP.
     * Applications are notified of a power change via a callback
     * for a foreground or background session.
     * If the registration succeeds, the callback returns the current
     * maximum power allocated by FLP.</p>
     *
     * @param callback Callback to receive power changes.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException  The callback parameter is NULL.
     *
     * @return
     * None.
     */
    void registerForMaxPowerAllocatedChange(IFlpMaxPowerAllocatedCallback callback)
                                            throws IZatIllegalArgumentException;

    /**
     * Stops listening for changes in the maximum power allocated by FLP.
     * <p>
     * This API enables applications to stop listening for changes in the maximum
     * power allocated by FLP.
     * If the callback is not registered, this function
     * just returns. </p>
     *
     * @param callback Callback used to register for receiving
     *         changes in the maximum power allocated by FLP.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException The callback parameter is NULL.
     *
     * @return
     * None.
     */
    void deregisterForMaxPowerAllocatedChange(IFlpMaxPowerAllocatedCallback callback)
                                              throws IZatIllegalArgumentException;

    /**
     * Interface class IFlpMaxPowerAllocatedCallback.
     * @cond
     * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
     * <p>All Rights Reserved.</p>
     * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
     * @endcond
     */
    interface IFlpMaxPowerAllocatedCallback {
        /**
         * Maximum power allocated callback.
         * <p>
         * This API is called by the underlying service back
         * to applications when the maximum allocated power changes.
         * Applications should implement this interface.</p>
         *
         * @param power_mW Maximum power allocated in mW.
         *
         * @return
         * None.
         */
        void onMaxPowerAllocatedChanged(double power_mW);
    }
}
    /** @} */  /* end_addtogroup IZatTestService */

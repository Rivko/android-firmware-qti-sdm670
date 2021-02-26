/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.content.Context;
import android.os.Bundle;

import com.qti.debugreport.IZatDebugConstants;
import java.util.ArrayList;

/** @addtogroup IZatDebugReportingService
    @{ */

public interface IZatDebugReportingService {
    /**
     * Registers to start receiving IZat debug reports.
     * <p>
     * This function enables applications to start receiving IZat debug
     * reports at 1 Hz. If there is already a callback registered,
     * the new one one will replace the existing one. </p>
     *
     * @param debugReportCb Callback to receive the debug report.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException The debugReportCb parameter is NULL.
     *
     * @return
     * None.
     */

    void registerForDebugReports(IZatDebugReportCallback debugReportCb)
                                      throws IZatIllegalArgumentException;

        /**
     * Deregisters to stop receiving IZat debugs reports.
     * <p>
     * This function enables applications to stop getting debug reports.</p>
     *
     * @param debugReportCb Callback to stop receiving debug reports.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException The debugReportCb parameter is NULL.
     *
     * @return
     * None.
     */

    void deregisterForDebugReports(IZatDebugReportCallback debugReportCb)
                                        throws IZatIllegalArgumentException;

    /**
     * Synchronous call to receive an instant debug report.
     * <p> The application can make this call to receive an instant
     * debug report at any time. </p>
     *
     * @return A <a href="https://developer.android.com/reference/android/os/Bundle.html">Bundle</a> type.
     * The bundle object can contain the following fields:
     *<ul>
     *    <li> IZAT_DEBUG_EXTERNAL_POSITION_INJECTION_KEY
     *         {@link java.util.ArrayList}
     *         IZatLocationReport </li>
     *    <li> IZAT_DEBUG_BEST_POSITION_KEY
     *         {@link java.util.ArrayList}
     *         IZatLocationReport </li>
     *    <li> IZAT_DEBUG_EPH_STATUS_KEY
     *         {@link java.util.ArrayList}
     *         IZatEphmerisDebugReport </li>
     *    <li> IZAT_DEBUG_XTRA_STATUS_KEY
     *         {@link java.util.ArrayList}
     *         IZatXTRADebugReport </li>
     *    <li> IZAT_DEBUG_SV_HEALTH_KEY
     *         {@link java.util.ArrayList}
     *         IZatSVHealthDebugReport </li>
     *    <li> IZAT_DEBUG_PDR_INFO_KEY
     *         {@link java.util.ArrayList}
     *         IZatPDRDebugReport </li>
     *    <li> IZAT_DEBUG_FIX_STATUS_KEY
     *         {@link java.util.ArrayList}
     *         IZatFixStatusDebugReport </li>
     *    <li> IZAT_DEBUG_GPS_TIME_KEY
     *         {@link java.util.ArrayList}
     *         IZatGpsTimeDebugReport </li>
     *    <li> IZAT_DEBUG_RF_STATE_KEY
     *         {@link java.util.ArrayList}
     *         IZatRfStateDebugReport </li>
     *    <li> IZAT_DEBUG_XO_STATE_KEY
     *         {@link java.util.ArrayList}
     *         IZatXoStateDebugReport </li>
     *    <li> IZAT_DEBUG_LAST_ERROR_RECOVERIES_KEY
     *         {@link java.util.ArrayList}
     *         IZatUtcSpec </li>
     * </ul>
     */

    Bundle getDebugReport();

/** @} */ /* end_addtogroup IZatDebugReportingService */

/** @addtogroup IZatDebugReportingService
    @{ */

    interface IZatDebugReportCallback {
    /**
     * Debug report callback.
     * <p>
     * This API is called by the underlying service back
     * to applications when a debug report is available.
     * Applications should implement this interface.</p>
     *
     * @param debugReport <a href="https://developer.android.com/reference/android/os/Bundle.html">Bundle</a> type.
     * The bundle object can contain the following fields:
     *<ul>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_EXTERNAL_POSITION_INJECTION_KEY,
     *          {@link com.qti.debugreport.IZatLocationReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_BEST_POSITION_KEY,
     *          {@link com.qti.debugreport.IZatLocationReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_EPH_STATUS_KEY,
     *          {@link com.qti.debugreport.IZatEphmerisDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_XTRA_STATUS_KEY,
     *          {@link com.qti.debugreport.IZatXTRADebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_SV_HEALTH_KEY,
     *          {@link com.qti.debugreport.IZatSVHealthDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_PDR_INFO_KEY,
     *          {@link com.qti.debugreport.IZatPDRDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_FIX_STATUS_KEY,
     *          {@link com.qti.debugreport.IZatFixStatusDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_GPS_TIME_KEY,
     *          {@link com.qti.debugreport.IZatGpsTimeDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_RF_STATE_KEY,
     *          {@link com.qti.debugreport.IZatRfStateDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_XO_STATE_KEY,
     *          {@link com.qti.debugreport.IZatXoStateDebugReport}) </li>
     *    <li> ({@link com.qti.debugreport.IZatDebugConstants} IZAT_DEBUG_LAST_ERROR_RECOVERIES_KEY,
     *          {@link com.qti.debugreport.IZatUtcSpec}) </li>
     * </ul>
     */

    void onDebugReportAvailable(Bundle debugReport);
    }
}
/** @} */ /* end_addtogroup IZatDebugReportingService */

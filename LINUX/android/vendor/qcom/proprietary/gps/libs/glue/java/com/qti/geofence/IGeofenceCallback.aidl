/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.geofence;

import android.location.Location;

oneway interface IGeofenceCallback {
    void onTransitionEvent(in int geofenceHwId,
                           in int event,
                           in Location location);

    void onRequestResultReturned(in int geofenceHwId,
                                 in int requestType,
                                 in int result);

    void onEngineReportStatus(in int status,
                              in Location location);
}

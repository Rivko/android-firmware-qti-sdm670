/* ======================================================================
*  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.geofence;

import java.com.qti.geofence.IGeofenceCallback;
import java.com.qti.geofence.GeofenceData;
import android.app.PendingIntent;

interface IGeofenceService {

    void registerCallback (in IGeofenceCallback callback);

    void unregisterCallback (in IGeofenceCallback callback);

    int addGeofence (in double latitude,
                     in double longitude,
                     in double radius,
                     in int transitionTypes,
                     in int responsiveness,
                     in int confidence,
                     in int dwellTime,
                     in int dwellTimeMask);

    void removeGeofence (in int geofenceId);

    void updateGeofence (in int geofenceId,
                         in int transitionTypes,
                         in int responsiveness);

    void pauseGeofence (in int geofenceId);

    void resumeGeofence (in int geofenceId);

    int addGeofenceObj(in GeofenceData gfData);

    void registerPendingIntent(in PendingIntent notifyIntent);

    void unregisterPendingIntent(in PendingIntent notifyIntent);

    void recoverGeofences(inout List<GeofenceData> gfList);

    void updateGeofenceData (in int geofenceId,
                             in double latitude,
                             in double longitude,
                             in double radius,
                             in int transitionTypes,
                             in int responsiveness,
                             in int confidence,
                             in int dwellTime,
                             in int dwellTimeMask,
                             in int updatedFieldsFlags);
}

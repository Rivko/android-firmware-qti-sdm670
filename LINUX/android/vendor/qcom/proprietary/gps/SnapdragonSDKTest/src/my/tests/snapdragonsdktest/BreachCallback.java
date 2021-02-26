/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import android.util.Log;
import android.location.Location;
import com.qti.location.sdk.IZatGeofenceService;

public class BreachCallback implements IZatGeofenceService.IZatGeofenceCallback {
    private static String TAG = "BreachCallbackInApp";

    public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
            int event,
            Location location) {
        Log.v(TAG, "got breach event in app, handle.obj is "+ handle.getContext());
        if (location != null) {
            Log.v(TAG, "The breach location is " +
                    location.getLatitude()+"/"+
                    location.getLongitude());
        } else {
            Log.v(TAG, "The breach location is Null");
        }
    }

    public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
            int requestType,
            int errorCode) {
        Log.v(TAG, "got onRequestFailed in app, handle.obj is "+
                handle.getContext() +"; requestType is " + requestType +
                "; errorCode is " +errorCode);
    }

    public void onEngineReportStatus(int status,
            Location location) {
        Log.v(TAG, "got geofence status in app, status is "+status);
        if (location != null) {
            Log.v(TAG, "The last location is " +location.getLatitude()+
                   "/"+location.getLongitude());
        } else {
            Log.v(TAG, "The last location is Null");
        }
    }
}
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import android.util.Log;
import android.location.Location;
import com.qti.location.sdk.IZatFlpService;

public class LocationCallback implements IZatFlpService.IFlpLocationCallback {
    private static String TAG = "LocationCallbackInApp";
    private static final Object sCallBacksLock = new Object();
    private String mSessionName;

    public LocationCallback(String sessionName) {
        mSessionName = sessionName;
    }

    public void onLocationAvailable(Location[] locations) {
        synchronized(sCallBacksLock) {
            Log.v(TAG, "Got batched fixes for session:" + mSessionName);

            for (Location loc : locations) {
                Log.v(TAG, "Latitude:" + loc.getLatitude() +
                        " Longitude:" + loc.getLongitude() +
                        " Accuracy:" + loc.getAccuracy() +
                        " UTC Time:" + loc.getTime() +
                        " Altitude:" + loc.getAltitude() +
                        " Vertical Uncertainity:" + loc.getVerticalAccuracyMeters() +
                        " Speed:" + loc.getSpeed() +
                        " Speed Accuracy:" + loc.getSpeedAccuracyMetersPerSecond() +
                        " Bearing:" + loc.getBearing() +
                        " Bearing Accuracy" + loc.getBearingAccuracyDegrees());
            }
        }
    }
}
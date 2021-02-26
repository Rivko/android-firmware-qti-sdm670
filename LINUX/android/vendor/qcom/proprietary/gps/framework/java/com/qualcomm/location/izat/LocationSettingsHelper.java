/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;
import android.util.Log;

import java.util.HashSet;
import java.util.Iterator;

public class LocationSettingsHelper {
    private static final String TAG = "LocationSettingsHelper";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private static LocationSettingsHelper sInstance = null;

    private final LocationManager mLocationManager;
    private HashSet<ILocationSettingsCallback> mLocationSettingsCallbackSet =
            new HashSet<ILocationSettingsCallback>();

    public boolean mIsLocationSettingsOn;

    public static synchronized LocationSettingsHelper getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new LocationSettingsHelper(ctx);
        }

        return sInstance;
    }

    public interface ILocationSettingsCallback {
        void onLocationSettingsChange(boolean locationSettingsIsOn);
    }

    public void registerForLocationSettingsCallback(
            ILocationSettingsCallback locationSettingsCallback) {
        mLocationSettingsCallbackSet.add(locationSettingsCallback);
    }

    public void deregisterForLocationSettingsCallback(
            ILocationSettingsCallback locationSettingsCallback) {
        mLocationSettingsCallbackSet.remove(locationSettingsCallback);
    }

    private LocationSettingsHelper(Context ctx) {
        mLocationManager = (LocationManager) ctx.getSystemService(Context.LOCATION_SERVICE);
        ctx.registerReceiver(mLocationSettingsReceiver,
                             new IntentFilter(LocationManager.PROVIDERS_CHANGED_ACTION));
        mIsLocationSettingsOn = isLocationSettingsOn();
    }

    private boolean isLocationSettingsOn() {
        boolean isGpsProviderOn = mLocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        boolean isNetworkProviderOn =
                mLocationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        if (VERBOSE) {
            Log.v(TAG, "isLocationSettingsOn isGpsProviderOn: " + isGpsProviderOn +
                       " isNetworkProviderOn: " + isNetworkProviderOn);
        }

        return isGpsProviderOn || isNetworkProviderOn;
    }

    private BroadcastReceiver mLocationSettingsReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals("android.location.PROVIDERS_CHANGED")) {
                synchronized (this) {
                    boolean isLocationSettingsOnNow = isLocationSettingsOn();
                    if (isLocationSettingsOnNow != mIsLocationSettingsOn) {
                        mIsLocationSettingsOn = isLocationSettingsOnNow;
                        Iterator<ILocationSettingsCallback> it =
                                mLocationSettingsCallbackSet.iterator();
                        while (it.hasNext()) {
                            try {
                                ILocationSettingsCallback locationSettingsCallback = it.next();
                                locationSettingsCallback.onLocationSettingsChange(
                                        mIsLocationSettingsOn);
                            } catch (Exception e) {
                                Log.e(TAG, "Invalid locationSettingsCallback.");
                                it.remove();
                            }
                        }
                    }
                }
            }
        }
    };
}

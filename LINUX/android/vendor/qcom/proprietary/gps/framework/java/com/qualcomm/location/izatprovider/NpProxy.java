/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izatprovider;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.lang.NullPointerException;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.Properties;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.ServiceConnection;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationRequest;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.DeadObjectException;
import android.os.WorkSource;
import android.os.UserHandle;
import android.util.Log;

import com.android.internal.location.ILocationProvider;
import com.android.internal.content.PackageMonitor;
import com.android.internal.location.ProviderRequest;

import com.qualcomm.location.izatprovider.IzatProvider.IzatProviderContext;

// Wraps OSNP service
public class NpProxy implements Handler.Callback {

    static {
        Properties prop = new Properties();
        try {
            File file = new File("/vendor/etc/izat.conf");
            if (!file.exists()) {
                file = new File("/etc/izat.conf");
            }
            FileInputStream stream = new FileInputStream(file);
            prop.load(stream);
            stream.close();
        } catch (IOException ioe) {
        }

        mDefaultServicePackageName = prop.getProperty("OSNLP_PACKAGE");
        mDefaultServiceActionName = prop.getProperty("OSNLP_ACTION");
        mRegionServicePackageName = prop.getProperty("REGION_OSNLP_PACKAGE");
        mRegionServiceActionName = prop.getProperty("REGION_OSNLP_ACTION");
        mActualServicePackageName = null;
        mActualServiceActionName = null;

        nativeOnClassLoad();
    }

    void enable() {
        log("NpProxy enable");
        try {
            mNpHandle.enable();
        } catch (DeadObjectException doe) {
            mNpHandle = null;
            Log.w(TAG, doe);
        } catch (RemoteException e) {
            Log.w(TAG, e);
        } catch (NullPointerException npe) {
            Log.w(TAG, npe);
        }
    }

    private boolean checkPackageExists(String packageName)
    {
        boolean exists = (packageName != null && !packageName.isEmpty());

        if (exists) {
            try {
                String info = mContext.getPackageManager()
                                      .getPackageInfo(packageName, 0)
                                      .versionName;
                exists = (info != null || !info.isEmpty());
            } catch (NameNotFoundException e) {
                exists = false;
            }
        }

        return exists;
    }

    private void updateServiceBinding(boolean forceRebind) {
        String curPackage = mActualServicePackageName;
        boolean useRegional = mDeviceInRegion;
        boolean hasDefaultNp = checkPackageExists(mDefaultServicePackageName);
        boolean hasRegionNp = checkPackageExists(mRegionServicePackageName);

        log("hasRegionNp = " + hasRegionNp + "; hasDefaultNp = " +
            hasDefaultNp);

        // only having default NP doesn't mean we could use it. If device is in
        // the reagion where it can not be used, i.e if (useRegional == true),
        // having default NP does no good. So hasAndAllowDefaultNp means we have
        // default NP AND we can use it.
        boolean hasAndAllowDefaultNp = (hasDefaultNp && !useRegional);

        // prefer default NP if we can use it
        if (hasAndAllowDefaultNp) {
            mActualServicePackageName = mDefaultServicePackageName;
            mActualServiceActionName = mDefaultServiceActionName;
        } else if (hasRegionNp) {
            mActualServicePackageName = mRegionServicePackageName;
            mActualServiceActionName = mRegionServiceActionName;
        } else {
            // no NLP we can use
            mActualServicePackageName = null;
            mActualServiceActionName = null;
            Log.w(TAG, "Device has no NLP service to use: OSNLP_PACKAGE - " +
                  mDefaultServicePackageName + " / OSNLP_ACTION - " +
                  mDefaultServiceActionName + "does" + (hasDefaultNp ? "" : " not") +
                  " exist; REGION_OSNLP_PACKAGE - " + mRegionServicePackageName +
                  " / REGION_OSNLP_ACTION - " + mRegionServiceActionName + "does" +
                  (hasRegionNp ? "" : " not") + " exist. Regional NlP is" +
                  (useRegional ? "" : " not") + " mandatory.");
        }

        if (forceRebind || mActualServicePackageName != curPackage) {
            if (curPackage != null) {
                unbindServiceTry();
            }

            if (mActualServicePackageName != null) {
                log("binding to service in " + mActualServicePackageName);

                Intent intent = new Intent(mActualServiceActionName);
                intent.setPackage(mActualServicePackageName);
                mContext.bindServiceAsUser(intent, mServiceConnection,
                                           (Context.BIND_AUTO_CREATE |
                                            Context.BIND_NOT_FOREGROUND |
                                            Context.BIND_NOT_VISIBLE),
                                           UserHandle.CURRENT);
            }
        }
    }

    private void unbindServiceTry() {
        try {
            mContext.unbindService(mServiceConnection);
            mNpHandle = null;
        } catch (Exception e) {
            log("unbindServiceTry - " + e.getMessage());
        }
    }

    private void setRequestTry() {
        if (null != mRequests) {
            try {
                mWorkSource.clear();
                if (mRequests.reportLocation) {
                    if (null != mIzatProviderContext.getNetworkProviderInner(mContext)) {
                        WorkSource ws = mIzatProviderContext
                                .getNetworkProviderInner(mContext).getWorkSource();
                        if (null != ws) {
                            mWorkSource.add(ws);
                        }
                    }

                    if (null != mIzatProviderContext.getFusedProviderInner(mContext)) {
                        WorkSource ws = mIzatProviderContext
                                .getFusedProviderInner(mContext).getWorkSource();
                        if (null != ws) {
                            mWorkSource.add(ws);
                        }
                    }
                }
                mNpHandle.setRequest(mRequests, mWorkSource);
                if (!mRequests.reportLocation) {
                    mRequests = null;
                }
            } catch (DeadObjectException doe) {
                mNpHandle = null;
                Log.w(TAG, doe);
            } catch (RemoteException e) {
                Log.w(TAG, e);
            } catch (NullPointerException npe) {
                Log.w(TAG, npe);
            }
        }
    }

    static void screenLocations(boolean screenOn)
    {
        log("Screen location status: " + screenOn);
        mLocationScreenOn.set(screenOn);
    }

    NpProxy(Context context, Looper looper, IzatProviderContext izatProviderContext) {
        mContext = context;
        mIzatProviderContext = izatProviderContext;
        mHandler = new Handler(looper, this);
        mHandler.sendEmptyMessage(MSG_INIT);
        mDeviceInRegion = false;
        mRequests = null;
    }

    void destroy() {
        unbindServiceTry();
        mRequests = null;
        mLocationManager.removeUpdates(mLocationListener);
        mPackageMonitor.unregister();
        nativeOnInstanceDeinit();
    }

    // Methods Called by native code
    private void onSetRequest(long interval, float smallestDisplacement, int numUpdates) {
        LocationRequest locationRequest = LocationRequest
                .createFromDeprecatedProvider(LocationManager.NETWORK_PROVIDER,
                                              interval, smallestDisplacement,
                                              numUpdates == 1);
        ProviderRequest providerRequest = new ProviderRequest ();
        providerRequest.interval = interval;
        providerRequest.reportLocation = true;
        providerRequest.locationRequests.add (locationRequest);

        mHandler.obtainMessage(MSG_SET_REQUEST, providerRequest).sendToTarget();
    }

    private void onStopRequest() {
        mHandler.obtainMessage(MSG_SET_REQUEST, new ProviderRequest()).sendToTarget();
    }

    private boolean clearMarkerIfForScreening(Location location) {
        Bundle extras = location.getExtras();
        boolean marked = (extras != null &&
                          extras.containsKey ("com.qualcomm.location.nlp:screen"));

        if (marked) {
            extras.remove("com.qualcomm.location.nlp:screen");
        }

        return marked;
    }

    static private void log(String msg) {
        if (DEBUG) Log.d(TAG, msg);
    }

    // Message handler
    public boolean handleMessage(Message msg) {
        int msgID = msg.what;
        log("handleMessage what - " + msgID);

        switch (msgID) {
        case MSG_INIT:
        {
            mLocationManager = (LocationManager)mContext
                    .getSystemService(Context.LOCATION_SERVICE);

            // Register package monitor
            mPackageMonitor.register(mContext, null, UserHandle.ALL, true);

            LocationRequest request = LocationRequest
                    .createFromDeprecatedProvider(LocationManager.PASSIVE_PROVIDER,
                                                  0, 0, false);
            request.setHideFromAppOps (true);
            try {
                mLocationManager.requestLocationUpdates(request, mLocationListener,
                                                        mHandler.getLooper());
            } catch (RuntimeException e) {
                Log.e(TAG, "Cannot request for passive location updates");
            }

            updateServiceBinding(false);

            nativeOnInstanceInit();
            break;
        }
        case MSG_CONNECTED:
        {
            enable();
            setRequestTry();
            break;
        }
        case MSG_SET_REQUEST:
        {
            mRequests = (ProviderRequest)msg.obj;
            setRequestTry();
            break;
        }
        case MSG_LOCATION_CHANGED:
        {
            int source = -1;
            Location location = (Location)msg.obj;

            if (location != null) {

                log("Location: " + location.toString());

                Bundle extras = location.getExtras();
                if (extras != null) {
                    String strSource = extras.getString(IzatProvider.NLP_TYPE_KEY, "");
                    if (strSource != null) {
                        if (strSource.equalsIgnoreCase("cell")) {
                            source = 0;
                        } else if (strSource.equalsIgnoreCase("wifi")) {
                            source = 1;
                        }
                    }
                }

                nativeOnLocationChanged(true, location.getTime(),
                                        true, location.getElapsedRealtimeNanos(),
                                        true, location.getLatitude(),
                                        true, location.getLongitude(),
                                        location.hasAccuracy(), location.getAccuracy(),
                                        location.hasVerticalAccuracy(),
                                        location.getVerticalAccuracyMeters(),
                                        location.hasAltitude(), location.getAltitude(),
                                        location.hasBearing(), location.getBearing(),
                                        location.hasSpeed(), location.getSpeed(),
                                        source != -1, source);
            }
            break;
        }
        case MSG_REGIONAL_NP_UPDATE:
            boolean deviceInRegion = (msg.arg1 == 1);
            if (mDeviceInRegion != deviceInRegion) {
                mDeviceInRegion = deviceInRegion;
                updateServiceBinding(false);
            }
            break;
        case MSG_NP_UPDATE:
            updateServiceBinding(true);
            break;
        default:
            Log.w(TAG, "Unhandled Message " + msg.what);
        }
        return true;
    }


    // native method declarations.
    private static native void nativeOnClassLoad();
    private native void nativeOnInstanceInit();
    private native void nativeOnInstanceDeinit();
    private native void nativeOnLocationChanged(boolean hasTime, long time,
                                                boolean hasTimeNanos, long timeNanos,
                                                boolean hasLatitude, double latitude,
                                                boolean hasLongitude, double longitude,
                                                boolean hasAccuracy, float accuracy,
                                                boolean hasVerticalAccuracy, float verticalAccuracy,
                                                boolean hasAltitude, double altitude,
                                                boolean hasBearing, float bearing,
                                                boolean hasSpeed, float speed,
                                                boolean hasSource, int source);

    // Constant data members
    private static final int        MSG_INIT                  = 1;
    private static final int        MSG_CONNECTED             = 2;
    private static final int        MSG_SET_REQUEST           = 3;
    private static final int        MSG_LOCATION_CHANGED      = 4;
    private static final int        MSG_REGIONAL_NP_UPDATE    = 5;
    private static final int        MSG_NP_UPDATE             = 8;

    private static final String     TAG   = "NpProxy";
    private static final boolean    DEBUG = Log.isLoggable (TAG, Log.DEBUG);

    // static class members
    private static AtomicBoolean    mLocationScreenOn   = new AtomicBoolean(false);
    // data members
    private Handler                 mHandler;
    private Context                 mContext;
    private IzatProviderContext     mIzatProviderContext;
    private WorkSource              mWorkSource         = new WorkSource();
    private LocationManager         mLocationManager;
    private boolean                 mDeviceInRegion;
    private ILocationProvider       mNpHandle;
    private ProviderRequest         mRequests;

    // initialized in the static clause, per izat.conf
    private static String           mDefaultServicePackageName;
    private static String           mDefaultServiceActionName;
    private static String           mRegionServicePackageName;
    private static String           mRegionServiceActionName;
    // assigned in updateServiceBinding(), when a new service is bound
    private static String           mActualServicePackageName;
    private static String           mActualServiceActionName;

    // Implemented Callback objects
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder binder) {
            mNpHandle = ILocationProvider.Stub.asInterface(binder);
            mHandler.obtainMessage(MSG_CONNECTED, binder).sendToTarget();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
        }
    };


    private LocationListener mLocationListener = new LocationListener() {
        @Override
        public void onLocationChanged (Location location) {
            if (LocationManager.NETWORK_PROVIDER.equals(location.getProvider()) &&
                (!mLocationScreenOn.get() || clearMarkerIfForScreening(location))) {
                mHandler.obtainMessage(MSG_LOCATION_CHANGED, location).sendToTarget();
            }
        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
        }
        @Override
        public void onProviderEnabled(String provider) {
        }
        @Override
        public void onProviderDisabled(String provider) {
        }
    };

    private final PackageMonitor mPackageMonitor = new PackageMonitor() {
        @Override
        public void onPackageUpdateFinished(String packageName, int uid) {
            if (packageName != null && packageName.equals(mActualServicePackageName)) {
                mHandler.sendEmptyMessage(MSG_NP_UPDATE);
            }
        }

        @Override
        public void onPackageAdded(String packageName, int uid) {
            if (packageName != null &&
                (packageName.equals(mDefaultServicePackageName) ||
                 packageName.equals(mRegionServicePackageName))) {
                // we want to possibly update the NP package if the other pack
                // is added.
                mHandler.sendEmptyMessage(MSG_NP_UPDATE);
            }
        }

        @Override
        public void onPackageRemoved(String packageName, int uid) {
            if (packageName != null && packageName.equals(mActualServicePackageName)) {
                mHandler.sendEmptyMessage(MSG_NP_UPDATE);
            }
        }

        @Override
        public boolean onPackageChanged(String packageName, int uid, String[] components) {
            if (packageName != null && packageName.equals(mActualServicePackageName)) {
                mHandler.sendEmptyMessage(MSG_NP_UPDATE);
            }
            return super.onPackageChanged(packageName, uid, components);
        }
    };

    public void setRegionalNpRegulate(boolean regulated) {
        mHandler.obtainMessage(MSG_REGIONAL_NP_UPDATE, regulated ? 1 : 0, 0).sendToTarget();
    }

}

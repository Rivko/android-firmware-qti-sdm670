/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.geofence;

import android.content.Intent;
import android.content.Context;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.location.Location;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.Bundle;

import android.util.Log;

import android.app.PendingIntent;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.qti.geofence.*;
import com.qualcomm.location.izat.DataPerPackageAndUser;
import com.qualcomm.location.izat.CallbackData;


public class GeofenceServiceProvider {
    private static final String TAG = "GeofenceServiceProvider";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private static final int GEOFENCE_RESULT_SUCCESS = 0;
    private static final int GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES = -100;
    private static final int GEOFENCE_RESULT_ERROR_ID_EXISTS = -101;
    private static final int GEOFENCE_RESULT_ERROR_ID_UNKNOWN = -102;
    private static final int GEOFENCE_RESULT_ERROR_INVALID_TRANSITION = -103;
    private static final int GEOFENCE_RESULT_ERROR_GENERIC = -149;

    private static final int GEOFENCE_REQUEST_TYPE_ADD = 1;
    private static final int GEOFENCE_REQUEST_TYPE_REMOVE = 2;
    private static final int GEOFENCE_REQUEST_TYPE_PAUSE = 3;
    private static final int GEOFENCE_REQUEST_TYPE_RESUME = 4;
    private static int sGeofenceId = 1;
    private static final Object sCallBacksLock = new Object();
    private PendingIntent mPendingIntent;
    private final Context mContext;

    private class ClientGeofenceData extends CallbackData {
        private IGeofenceCallback mCallback;
        private PendingIntent mPendingIntent;
        private Map<Integer, GeofenceData> mGeofenceMap;

        public ClientGeofenceData(IGeofenceCallback callback) {
            mCallback = callback;
            super.mCallback = callback;
            mGeofenceMap = new HashMap<Integer, GeofenceData> ();
        }

        public ClientGeofenceData(PendingIntent notifyIntent) {
            mPendingIntent = notifyIntent;
            mGeofenceMap = new HashMap<Integer, GeofenceData> ();
        }
    }

    private DataPerPackageAndUser<ClientGeofenceData> mDataPerPackageAndUser;

    private RemoteCallbackList<IGeofenceCallback> mGeofenceCallbacks
        = new RemoteCallbackList<IGeofenceCallback>();

    public static GeofenceServiceProvider sInstance = null;
    public static GeofenceServiceProvider getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new GeofenceServiceProvider(ctx);
        }
        return sInstance;
    }

    public GeofenceServiceProvider(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "GeofenceServiceProvider construction");
        }

        mContext = ctx;
        mDataPerPackageAndUser = new DataPerPackageAndUser<ClientGeofenceData>(mContext,
                new UserChanged());

        native_geofence_init();

        PackageRemovedReceiver pkgRemovedReceiver = new PackageRemovedReceiver();
        IntentFilter filter = new IntentFilter(Intent.ACTION_PACKAGE_REMOVED);
        filter.addDataScheme("package");
        ctx.registerReceiver(pkgRemovedReceiver, filter);
    }

    /* Remote binder */
    private final IGeofenceService.Stub mBinder = new IGeofenceService.Stub() {

        public void registerCallback(final IGeofenceCallback callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            synchronized (sCallBacksLock) {
                ClientGeofenceData clData = mDataPerPackageAndUser.getData();
                if (null == clData) {
                    clData = new ClientGeofenceData(callback);
                    clData.mCallback = callback;

                    mDataPerPackageAndUser.setData(clData);
                } else {
                    if (null != clData.mCallback) {
                        mGeofenceCallbacks.unregister(clData.mCallback);
                    }
                    clData.mCallback = callback;
                }
                mGeofenceCallbacks.register(callback);
            }
            try {
                callback.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                    @Override
                    public void binderDied() {
                        synchronized (sCallBacksLock) {
                            ClientGeofenceData clData =
                                    mDataPerPackageAndUser.getDataByCallback(callback);

                            if (null != clData) {
                                clData.mCallback = null;
                                if (null == clData.mPendingIntent) {
                                    if (VERBOSE) {
                                        Log.d(TAG, "Client died, no intent:" +
                                              mDataPerPackageAndUser.getPackageName(null) +
                                              " remove all geofences");
                                    }
                                    removeAllGeofences(clData);
                                } else {
                                    if (VERBOSE) {
                                        Log.d(TAG, "Client died:" +
                                              mDataPerPackageAndUser.getPackageName(null) +
                                              " notify on breach by intent");
                                    }
                                }
                            } else {
                                Log.e(TAG, "Client died but no clientData found!: " +
                                      mDataPerPackageAndUser.getPackageName(null));
                            }

                            mGeofenceCallbacks.unregister(callback);
                        }
                    }
                }, 0);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed unregister geofence cb", e);
            }
        }

        public void unregisterCallback(IGeofenceCallback callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            synchronized (sCallBacksLock) {
                removeAllGeofences(mDataPerPackageAndUser.getData());
                mGeofenceCallbacks.unregister(callback);
            }
        }

        public void registerPendingIntent(final PendingIntent notifyIntent) {
            if (notifyIntent == null) {
                Log.e(TAG,
                    "in registerPendingIntent() notifyIntent is null");
                return;
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null == clData) {
                clData = new ClientGeofenceData(notifyIntent);
                mDataPerPackageAndUser.setData(clData, notifyIntent);
            } else {
                clData.mPendingIntent = notifyIntent;
            }
        }

        public void unregisterPendingIntent(PendingIntent notifyIntent) {
            if (notifyIntent == null) {
                Log.e(TAG,
                    "in unregisterPendingIntent() notifyIntent is null");
                return;
            }


            String callingPackageAndUser = notifyIntent.getCreatorPackage();
            if (VERBOSE) {
                Log.d(TAG,
                    "unregisterPendingIntent() for package:" + callingPackageAndUser);
            }
            removeAllGeofences(mDataPerPackageAndUser.getData());
        }

        public int addGeofence(double latitude,
                               double longitude,
                               double radius, // in meters
                               int transitionTypes,
                               int responsiveness, // in milliseconds
                               int confidence,
                               int dwellTime, // in seconds
                               int dwellTimeMask) {

            final int geofenceId = sGeofenceId++;

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                GeofenceData gfData = new GeofenceData(responsiveness, latitude, longitude,
                    radius, transitionTypes, confidence, dwellTimeMask, dwellTime,
                    null, null, geofenceId);
                synchronized (clData.mGeofenceMap) {
                    clData.mGeofenceMap.put(geofenceId, gfData);
                }
            }

            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): addGeofence()" +
                           "; Calling package is " +
                           mDataPerPackageAndUser.getPackageName(null) +
                           "; geofenceId is " + geofenceId +
                           "; latitude is " + latitude +
                           "; longitude is " + longitude +
                           "; radius is " + radius +
                           "; transitionTypes is " + transitionTypes +
                           "; responsiveness is " + responsiveness +
                           "; confidence is " + confidence +
                           "; dwellTime is " + dwellTime +
                           "; dwellTimeMask is " + dwellTimeMask);
            }
            native_add_geofence(geofenceId, latitude, longitude, radius,
                                transitionTypes, responsiveness, confidence,
                                dwellTime, dwellTimeMask);
            return geofenceId;
        }

        public int addGeofenceObj(GeofenceData gfData) {
            if (gfData == null) {
                Log.e(TAG,
                    "in addGeofence() gfData is null");
                return -1;
            }

            double latitude = gfData.getLatitude();
            double longitude = gfData.getLongitude();
            double radius = gfData.getRadius();
            int transitionTypes = gfData.getTransitionType().getValue();
            int responsiveness = gfData.getNotifyResponsiveness();
            int confidence = gfData.getConfidence().getValue();
            int dwellTime = gfData.getDwellTime();
            int dwellTimeMask = gfData.getDwellType().getValue();
            Object appBundleData = gfData.getAppBundleData();
            String appTextData = gfData.getAppTextData();

            final int geofenceId = sGeofenceId++;

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                // geofence Id is provided by service, hence set it here.
                gfData.setGeofenceId(geofenceId);
                synchronized (clData.mGeofenceMap) {
                    clData.mGeofenceMap.put(geofenceId, gfData);
                }
            }

            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): addGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId +
                        "; latitude is " + latitude +
                        "; longitude is " + longitude +
                        "; radius is " + radius +
                        "; transitionTypes is " + transitionTypes +
                        "; responsiveness is " + responsiveness +
                        "; confidence is " + confidence +
                        "; dwellTime is " + dwellTime +
                        "; dwellTimeMask is " + dwellTimeMask +
                        "; appTextData is " + appTextData);
            }
            native_add_geofence(geofenceId, latitude, longitude, radius,
                                transitionTypes, responsiveness, confidence,
                                dwellTime, dwellTimeMask);
            return geofenceId;
        }

        public void removeGeofence(int geofenceId) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): removeGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                synchronized (clData.mGeofenceMap) {
                    clData.mGeofenceMap.remove(Integer.valueOf(geofenceId));
                }
            }

            native_remove_geofence(geofenceId);
        }

        public void updateGeofence(int geofenceId,
                                   int transitionTypes,
                                   int notifyResponsiveness) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): updateGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    gfData.setTransitionType(transitionTypes);
                    gfData.setNotifyResponsiveness(notifyResponsiveness);
                }
            }

            native_update_geofence(geofenceId,
                                   transitionTypes,
                                   notifyResponsiveness);
        }

        public void updateGeofenceData(int geofenceId,
                                       double latitude,
                                       double longitude,
                                       double radius,
                                       int transitionTypes,
                                       int responsiveness,
                                       int confidence,
                                       int dwellTime,
                                       int dwellTimeMask,
                                       int updatedFieldsFlags) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): updateGeofenceData()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId +
                        "; latitude is " + latitude +
                        "; longitude is " + longitude +
                        "; radius is " + radius +
                        "; transitionTypes is " + transitionTypes +
                        "; responsiveness is " + responsiveness +
                        "; confidence is " + confidence +
                        "; dwellTime is " + dwellTime +
                        "; dwellTimeMask is " + dwellTimeMask +
                        "; updatedFieldsFlags is " + updatedFieldsFlags);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData =
                        clData.mGeofenceMap.get(Integer.valueOf(geofenceId));

                    if ((updatedFieldsFlags & GeofenceData.TRANSITION_TYPES_IS_SET) != 0) {
                        gfData.setTransitionType(transitionTypes);
                    }
                    if ((updatedFieldsFlags & GeofenceData.RESPONSIVENESS_IS_SET) != 0) {
                        gfData.setNotifyResponsiveness(responsiveness);
                    }
                    if ((updatedFieldsFlags & GeofenceData.LATITUDE_IS_SET) != 0) {
                        gfData.setLatitude(latitude);
                    }
                    if ((updatedFieldsFlags & GeofenceData.LONGITUDE_IS_SET) != 0) {
                        gfData.setLongitude(longitude);
                    }
                    if ((updatedFieldsFlags & GeofenceData.RADIUS_IS_SET) != 0) {
                        gfData.setRadius(radius);
                    }
                    if ((updatedFieldsFlags & GeofenceData.TRANSITION_TYPES_IS_SET) != 0) {
                        gfData.setTransitionType(transitionTypes);
                    }
                    if ((updatedFieldsFlags & GeofenceData.CONFIDENCE_IS_SET) != 0) {
                        gfData.setConfidence(confidence);
                    }
                    if ((updatedFieldsFlags & GeofenceData.DWELL_NOTIFY_IS_SET) != 0) {
                        gfData.setDwellTime(dwellTime);
                        gfData.setDwellType(dwellTimeMask);
                    }

                    native_remove_geofence(geofenceId);
                    native_add_geofence(geofenceId, latitude, longitude, radius,
                            transitionTypes, responsiveness, confidence,
                            dwellTime, dwellTimeMask);

                    if (gfData.isPaused()) {
                        native_pause_geofence(geofenceId);
                    }

                } else {
                    Log.e(TAG, "Geofence id not found: " + geofenceId);
                }
            }
        }

        public void pauseGeofence(int geofenceId) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): pauseGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    gfData.setPausedStatus(true);
                    native_pause_geofence(geofenceId);
                }
            }
        }

        public void resumeGeofence(int geofenceId) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): resumeGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData.GeofenceTransitionTypes transitionTypes;
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    transitionTypes = gfData.getTransitionType();
                    gfData.setPausedStatus(false);

                    // resume the geofence with the original transition type
                    native_resume_geofence(geofenceId, transitionTypes.getValue());
                }
            }
        }

        public void recoverGeofences(List<GeofenceData> gfList) {
             if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): recoverGeofences()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null));
             }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                gfList.addAll(clData.mGeofenceMap.values());
             }
        }
    };

    private void reportGeofenceTransition(int geofenceId,
                                          int transition,
                                          Location location) {
        if (VERBOSE) {
            Log.d(TAG, "reportGeofenceTransition id : " + geofenceId +
                    "; transition : " + transition);
        }
        synchronized (sCallBacksLock) {
            for (ClientGeofenceData clData : mDataPerPackageAndUser.getAllData()) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    if (gfData.isPaused()) {
                        if (VERBOSE) {
                            Log.w(TAG, "Breach event with paused geofence: " + geofenceId);
                        }
                        break;
                    }

                    if (VERBOSE) {
                        Log.d(TAG, "Sending breach event to: " + geofenceId);
                    }

                    try {
                        if (clData.mPendingIntent != null) {
                            Intent pdIntent = new Intent();
                            String gfCtxStringData = gfData.getAppTextData();
                            if (gfCtxStringData != null) {
                                pdIntent.putExtra("context-data", gfCtxStringData);
                            } else {
                                Bundle gfCtxBundleData = gfData.getAppBundleData();
                                if (gfCtxBundleData != null) {
                                    pdIntent.putExtra("context-data", gfCtxBundleData);
                                }
                            }
                            pdIntent.putExtra("transition-location", location);
                            pdIntent.putExtra("transition-event", transition);

                            clData.mPendingIntent.send(mContext, 0, pdIntent);
                        }
                    } catch (PendingIntent.CanceledException e) {
                        clData.mPendingIntent = null;
                    }

                    try {
                        if (clData.mCallback != null) {
                            clData.mCallback.onTransitionEvent(geofenceId,
                                                               transition,
                                                               location);
                        }
                    } catch (RemoteException e) {
                        clData.mCallback = null;
                    }

                    if ((clData.mPendingIntent == null) &&
                        (clData.mCallback == null)) {
                        removeAllGeofences(clData);
                    }

                    break;
                }
            };
        }
    }

    private void reportGeofenceRequestStatus(int requestType,
                                             int geofenceId,
                                             int status) {
        if (VERBOSE) {
            Log.d(TAG, "reportGeofenceRequestStatus requestType: " +
                    requestType + "; id : " + geofenceId +
                    "; status : " + status);
        }
        synchronized (sCallBacksLock) {
            for (ClientGeofenceData clData : mDataPerPackageAndUser.getAllData()) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    try {
                        clData.mCallback.onRequestResultReturned(geofenceId,
                                                                 requestType,
                                                                 status);

                        if ((GEOFENCE_REQUEST_TYPE_ADD == requestType) &&
                                (GEOFENCE_RESULT_SUCCESS != status)) {
                            synchronized (clData.mGeofenceMap) {
                                clData.mGeofenceMap.remove(Integer.valueOf(geofenceId));
                            }
                        }
                    } catch (RemoteException e) {
                        // do nothing
                    }
                    return;
                }
            };
        }
    }

    private void reportGeofenceStatus(int status,
                                      Location location) {
        if (VERBOSE) {
            Log.d(TAG, "reportGeofenceStatus - status : " + status);
        }
        synchronized (sCallBacksLock) {
            int index = mGeofenceCallbacks.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mGeofenceCallbacks.getBroadcastItem(i).onEngineReportStatus(status, location);
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mGeofenceCallbacks.finishBroadcast();
        }
    }

    private void removeAllGeofences(ClientGeofenceData clData)
    {
        if (VERBOSE) {
            Log.d(TAG, "removing all geofences for package and user: " +
                    clData.mPackageName);
        }
        synchronized (clData.mGeofenceMap) {
            for(Integer key : clData.mGeofenceMap.keySet()) {
                native_remove_geofence(key);
            }
            clData.mGeofenceMap.clear();
        }

        mDataPerPackageAndUser.removeData(clData);
    }

    public IGeofenceService getGeofenceBinder() {
        return mBinder;
    }

    class PackageRemovedReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context conext, Intent intent) {
            synchronized (sCallBacksLock)  {
                Log.d(TAG, "Package uninstalled, removing its geofences: " +
                      intent.getData().getSchemeSpecificPart());
                for (ClientGeofenceData clData:
                        mDataPerPackageAndUser.getAllDataPerPackageName(
                        intent.getData().getSchemeSpecificPart())) {
                    removeAllGeofences(clData);
                }
            }
        }
    }

    class UserChanged implements DataPerPackageAndUser.UserChangeListener<ClientGeofenceData> {
        @Override
        public void onUserChange(Map<String, ClientGeofenceData> prevUserData,
                                 Map<String, ClientGeofenceData> currentUserData) {
            if (VERBOSE) {
                Log.d(TAG, "Active user has changed, updating geofences...");
            }

            // Remove prevUser geofences
            for (ClientGeofenceData geofenceData: prevUserData.values()) {
                synchronized (geofenceData.mGeofenceMap) {
                    for(GeofenceData geofence: geofenceData.mGeofenceMap.values()) {
                        if (VERBOSE) {
                            Log.d(TAG, "Removing geofence id: " + geofence.getGeofenceId());
                        }
                        native_remove_geofence(geofence.getGeofenceId());
                    }
                }
            }

            // Add back current user Geofences
            for (ClientGeofenceData geofenceData: currentUserData.values()) {
                synchronized (geofenceData.mGeofenceMap) {
                    for(GeofenceData geofence: geofenceData.mGeofenceMap.values()) {
                        if (VERBOSE) {
                            Log.d(TAG, "Adding geofence id: " + geofence.getGeofenceId());
                        }
                        native_add_geofence(geofence.getGeofenceId(),
                                geofence.getLatitude(), geofence.getLongitude(),
                                geofence.getRadius(),
                                geofence.getTransitionType().getValue(),
                                geofence.getNotifyResponsiveness(),
                                geofence.getConfidence().getValue(),
                                geofence.getDwellTime(),
                                geofence.getDwellType().getValue());
                    }
                }
            }
        }
    }

    static {
        native_geofence_class_init();
    }

    private static native void native_geofence_class_init();
    private native void native_geofence_init();
    private native void native_add_geofence(int geofenceId,
                                            double latitude,
                                            double longitude,
                                            double radius,
                                            int transitionTypes,
                                            int notificationResponsivenes,
                                            int confidence,
                                            int dwellTime,
                                            int dwellTimeMask);
    private native void native_remove_geofence(int geofenceId);
    private native void native_update_geofence(int geofenceId,
                                               int transitionTypes,
                                               int notifyResponsiveness);
    private native void native_pause_geofence(int geofenceId);
    private native void native_resume_geofence(int geofenceId,
                                               int transitionTypes);
}

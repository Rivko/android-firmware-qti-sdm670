/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.flp;

import android.content.Context;
import android.location.Location;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.util.Pair;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;

import com.qti.flp.*;
import com.qualcomm.location.izat.CallbackData;
import com.qualcomm.location.izat.DataPerPackageAndUser;
import com.qualcomm.location.izat.LocationSettingsHelper;

public class FlpServiceProvider implements LocationSettingsHelper.ILocationSettingsCallback {
    private static final String TAG = "FlpServiceProvider";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private static final int FLP_RESULT_SUCCESS = 0;
    private static final int FLP_RESULT_ERROR = -1;
    private static final int LOCATION_REPORT_ON_FULL_INDICATION = 1;
    private static final int LOCATION_REPORT_ON_FIX_INDICATION = 2;
    private static final int LOCATION_REPORT_ON_QUERY = 4;
    private static final int LOCATION_REPORT_ON_INDICATIONS = 8; // for legacy
    private static final int FLP_PASSIVE_LISTENER_HW_ID = -1;
    private static final Object sCallBacksLock = new Object();
    private static final Object sStatusCallbacksLock = new Object();
    private static final Object sLocationSettingsLock = new Object();
    private final Context mContext;
    private final LocationSettingsHelper mLocationSettingsHelper;
    private final DataPerPackageAndUser<FlpUserData> mDataPerPackageAndUser;

    private static final int FLP_SESSION_BACKGROUND = 1;
    private static final int FLP_SESSION_FOREGROUND = 2;
    private static final int FLP_SESSION_PASSIVE = 4;

    private static final int FLP_BG_NOTIFICATION_ROUTINE = 1;
    private static final int FLP_BG_NOTIFICATION_OUTDOOR_TRIP = 3;

    private static final int BATCHING_STATUS_TRIP_COMPLETED = 0;
    private static final int BATCHING_STATUS_POSITION_AVAILABE = 1;
    private static final int BATCHING_STATUS_POSITION_UNAVAILABLE = 2;

    private static final int BATCHING_MODE_ROUTINE = 0;
    private static final int BATCHING_MODE_OUTDOOR_TRIP = 1;

    private static final long MilliToNano(long millis) {
        return (millis > (Long.MAX_VALUE/1000000)) ? Long.MAX_VALUE : millis*1000000;
    }

    private class BgSessionData {
        private int mBgNotificationType;
        private long mSessionStartTime;

        BgSessionData() {
            mBgNotificationType = FLP_BG_NOTIFICATION_ROUTINE;
            mSessionStartTime = 0;
        }
    }


    // Used for data transfer into native layer.
    // Note the period difference in units: nanoseconds in native layer, milliseconds above.
    private class FlpSessionData {
        private int mId;
        private int mFlags;
        private long mPeriodNs;
        private int mDistanceIntervalMps;
        private long mTripDistanceM;

        FlpSessionData(int id,
                       int flags,
                       long period_ms,
                       int distance_interval_mps,
                       long trip_distance_m) {
            mId = id;
            mFlags = flags;
            mPeriodNs = MilliToNano(period_ms);
            mDistanceIntervalMps = distance_interval_mps;
            mTripDistanceM = trip_distance_m;
        }
    }

    private RemoteCallbackList<ILocationCallback> mCallbacksForBg
            = new RemoteCallbackList<ILocationCallback>();
    private RemoteCallbackList<ILocationCallback> mCallbacksForFg
            = new RemoteCallbackList<ILocationCallback>();
    private RemoteCallbackList<ILocationCallback> mCallbacksForPassive
            = new RemoteCallbackList<ILocationCallback>();
    private RemoteCallbackList<IMaxPowerAllocatedCallback> mMaxPowerCallbacks
            = new RemoteCallbackList<IMaxPowerAllocatedCallback>();
    private RemoteCallbackList<ISessionStatusCallback> mCallbacksForStatus
            = new RemoteCallbackList<ISessionStatusCallback>();

    // Used to store FLP data related to each user.
    private class FlpUserData extends CallbackData {
        private Map<Integer, BgSessionData> mBgSessionMap = new HashMap<Integer, BgSessionData>();
        private Map<Integer, FlpSessionData> mFlpSessionMap =
                new HashMap<Integer, FlpSessionData>();

        private Queue<Pair<ILocationCallback,Long>> mQueryCbQueue
                = new LinkedList<Pair<ILocationCallback,Long>>();

        // Backup copy to allow batch register/deregister on the corresponding RemoteCallbackList
        private Map<ILocationCallback, Long> mCallbacksForBgMap =
                new HashMap<ILocationCallback, Long>();
        private Map<ILocationCallback, Long> mCallbacksForFgMap =
                new HashMap<ILocationCallback, Long>();
        private Map<ILocationCallback, Long> mCallbacksForPassiveMap =
                new HashMap<ILocationCallback, Long>();
        private Map<ISessionStatusCallback, Long> mCallbacksForStatusMap
                = new HashMap<ISessionStatusCallback, Long>();

        private List<IMaxPowerAllocatedCallback> mMaxPowerCallbacksList
                = new ArrayList<IMaxPowerAllocatedCallback>();
    }

    // For FLP only 1 instance per user.
    // Will be switched accordingly on user changes.
    private FlpUserData mFlpUserData = new FlpUserData();

    public static FlpServiceProvider sInstance = null;
    public static FlpServiceProvider getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new FlpServiceProvider(ctx);
        }
        return sInstance;
    }
    private int mFlpFeaturMasks = -1;
    private boolean mIsLocationSettingsOn;

    public FlpServiceProvider(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "FlpServiceProvider construction");
        }
        mContext = ctx;
        if (native_flp_init() != FLP_RESULT_SUCCESS) {
            Log.e(TAG, "native flp init failed");
        }
        mLocationSettingsHelper = LocationSettingsHelper.getInstance(ctx);
        mLocationSettingsHelper.registerForLocationSettingsCallback(this);
        mIsLocationSettingsOn = mLocationSettingsHelper.mIsLocationSettingsOn;

        mDataPerPackageAndUser = new DataPerPackageAndUser<FlpUserData>(mContext,
                new UserChanged());
        mDataPerPackageAndUser.useCommonPackage();
        mDataPerPackageAndUser.setData(mFlpUserData);
    }

    public void onLocationSettingsChange(boolean locationSettingsIsOn) {
        synchronized (sLocationSettingsLock) {
            mIsLocationSettingsOn = locationSettingsIsOn;
            if (mIsLocationSettingsOn) {
                startFlpSessions();
            } else {
                stopFlpSessions();
            }
        }
    }

    private void startFlpSessions() {
        for (FlpSessionData flpSessionData : mFlpUserData.mFlpSessionMap.values()) {
            native_flp_start_session(flpSessionData.mId,
                    flpSessionData.mFlags,
                    flpSessionData.mPeriodNs,
                    flpSessionData.mDistanceIntervalMps,
                    flpSessionData.mTripDistanceM);
            if (VERBOSE) {
                Log.d(TAG, "Starting flp session id: " + flpSessionData.mId);
            }
        }
    }

    private void stopFlpSessions() {
        for (FlpSessionData flpSessionData : mFlpUserData.mFlpSessionMap.values()) {
            native_flp_stop_session(flpSessionData.mId);
            if (VERBOSE) {
                Log.d(TAG, "Stoping flp session id: " + flpSessionData.mId);
            }
        }
    }

    /* Remote binder */
    private final IFlpService.Stub mBinder = new IFlpService.Stub() {
        public void registerCallback(final int sessionType,
                                     final int id,
                                     final ILocationCallback cb,
                                     final long sessionStartTime) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): registerCallback()," +
                           " sessionType is " + sessionType + "; id is " + id +
                           "; sessionStartTime is " + sessionStartTime +
                           "; cb:" + cb);
            }
            if (cb != null) {
                if (mCallbacksForBg != null &&
                    mCallbacksForFg != null &&
                    mCallbacksForPassive != null) {
                    switch (sessionType) {
                        case FLP_SESSION_BACKGROUND:
                            synchronized (sCallBacksLock) {
                                mCallbacksForBg.register(cb, id);
                                mFlpUserData.mCallbacksForBgMap.put(cb, new Long(id));

                                BgSessionData bgSessData;
                                if (mFlpUserData.mBgSessionMap.containsKey(id)) {
                                    bgSessData = mFlpUserData.mBgSessionMap.get(id);
                                } else {
                                    bgSessData = new BgSessionData();
                                }

                                bgSessData.mSessionStartTime = sessionStartTime;
                                mFlpUserData.mBgSessionMap.put(id, bgSessData);
                            }
                            try {
                                cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                                    @Override
                                    public void binderDied() {
                                        synchronized(sCallBacksLock) {
                                            mCallbacksForBg.unregister(cb);
                                            mFlpUserData.mCallbacksForBgMap.remove(cb);
                                            mFlpUserData.mBgSessionMap.remove(id);
                                            stopFlpSession(id);
                                        }
                                    }
                                }, 0);
                            } catch (RemoteException e) {
                                throw new RuntimeException("Failed clean up flp sessions", e);
                            }
                            break;
                        case FLP_SESSION_FOREGROUND:
                            synchronized (sCallBacksLock) {
                                mCallbacksForFg.register(cb, sessionStartTime);
                                mFlpUserData.mCallbacksForFgMap.put(cb, sessionStartTime);
                            }
                            try {
                                cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                                    @Override
                                    public void binderDied() {
                                        synchronized(sCallBacksLock) {
                                            mCallbacksForFg.unregister(cb);
                                            mFlpUserData.mCallbacksForFgMap.remove(cb);
                                            stopFlpSession(id);
                                        }
                                    }
                                }, 0);
                            } catch (RemoteException e) {
                                throw new RuntimeException("Failed clean up flp sessions", e);
                            }
                            break;
                        case FLP_SESSION_PASSIVE:
                            synchronized (sCallBacksLock) {
                                mCallbacksForPassive.register(cb, sessionStartTime);
                                mFlpUserData.mCallbacksForPassiveMap.put(cb, sessionStartTime);
                            }
                            try {
                                cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                                    @Override
                                    public void binderDied() {
                                        synchronized(sCallBacksLock) {
                                            mCallbacksForPassive.unregister(cb);
                                            mFlpUserData.mCallbacksForPassiveMap.remove(cb);
                                            if (id != FLP_PASSIVE_LISTENER_HW_ID) {
                                                stopFlpSession(id);
                                            }
                                        }
                                    }
                                }, 0);
                            } catch (RemoteException e) {
                                throw new RuntimeException("Failed unregister passive cb", e);
                            }
                            break;
                        default:
                            Log.e(TAG, "unknown sessionType");
                            break;
                    }
                } else {
                    Log.e(TAG, "one of the callback list is not created");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public void unregisterCallback(final int sessionType,
                                       final ILocationCallback cb) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): unregisterCallback(): cb:" + cb);
            }
            if (cb != null) {
                if (mCallbacksForBg != null &&
                    mCallbacksForFg != null &&
                    mCallbacksForPassive != null) {
                    synchronized (sCallBacksLock) {
                        switch (sessionType) {
                            case FLP_SESSION_BACKGROUND:
                                mCallbacksForBg.unregister(cb);
                                mFlpUserData.mCallbacksForBgMap.remove(cb);
                                break;
                            case FLP_SESSION_FOREGROUND:
                                mCallbacksForFg.unregister(cb);
                                mFlpUserData.mCallbacksForFgMap.remove(cb);
                                break;
                            case FLP_SESSION_PASSIVE:
                                mCallbacksForPassive.unregister(cb);
                                mFlpUserData.mCallbacksForPassiveMap.remove(cb);
                                break;
                            default:
                                Log.e(TAG, "unknown sessionType");
                                break;
                        }
                    }
                } else {
                    Log.e(TAG, "one of the callback list is not created");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public void registerForSessionStatus(final int id, final ISessionStatusCallback cb) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): registerForSessionStatus() cb:" + cb);
            }
            if (cb != null) {
                if (mCallbacksForStatus != null) {
                    synchronized (sStatusCallbacksLock) {
                        mCallbacksForStatus.register(cb, id);
                        mFlpUserData.mCallbacksForStatusMap.put(cb, new Long(id));
                    }
                    try {
                        cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                            @Override
                            public void binderDied() {
                                synchronized(sStatusCallbacksLock) {
                                    mCallbacksForStatus.unregister(cb);
                                    mFlpUserData.mCallbacksForStatusMap.remove(cb);
                                }
                            }
                        }, 0);
                    } catch (RemoteException e) {
                        throw new RuntimeException("Failed clean up flp sessions", e);
                    }
                } else {
                      Log.e(TAG, "cb is null");
                }
            }
        }

        public void unregisterForSessionStatus(final ISessionStatusCallback cb) {
            if (VERBOSE) {
                Log.d(TAG, "in FlpService.Stub unregisterForSessionStatus() cb = : " + cb);
            }
            if (cb != null) {
                if (mCallbacksForStatus != null) {
                    synchronized(sStatusCallbacksLock) {
                        mCallbacksForStatus.unregister(cb);
                        mFlpUserData.mCallbacksForStatusMap.remove(cb);
                    }
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public int getAllSupportedFeatures() {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): getAllSupportedFeatures()");
            }
            if (mFlpFeaturMasks == -1) {
                mFlpFeaturMasks = native_flp_get_all_supported_features();
            }
            return mFlpFeaturMasks;
        }

        public int startFlpSession(int id,
                                   int flags,
                                   long period_ms,
                                   int distance_interval_mps,
                                   long trip_distance_m) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): startFlpSession()" +
                           "; id is " + id +
                           "; period_ms is " + period_ms +
                           "; flags is " + flags +
                           "; distance interval is " + distance_interval_mps +
                           "; trip distance is" + trip_distance_m);
            }

            // BgSessionMap only cares about BG sessions i.e. routine / trip
            synchronized(sCallBacksLock) {
                if ((flags == FLP_BG_NOTIFICATION_ROUTINE) ||
                    (flags == FLP_BG_NOTIFICATION_OUTDOOR_TRIP)) {
                    BgSessionData bgSessData = mFlpUserData.mBgSessionMap.get(id);

                    if (bgSessData != null) {
                        bgSessData.mBgNotificationType = flags;
                    } else {
                        Log.e(TAG, "No registered callback for this session id.");
                    }
                }
            }

            FlpSessionData flpSessionData = new FlpSessionData(id,
                                                               flags,
                                                               period_ms,
                                                               distance_interval_mps,
                                                               trip_distance_m);
            int result;
            synchronized (sLocationSettingsLock) {
                mFlpUserData.mFlpSessionMap.put(id, flpSessionData);
                if (!mIsLocationSettingsOn) {
                    if (VERBOSE) {
                        Log.d(TAG, "Location setting is OFF, storing FLP session data");
                    }
                    result = FLP_RESULT_SUCCESS;
                } else {
                    result = native_flp_start_session(flpSessionData.mId,
                                                      flpSessionData.mFlags,
                                                      flpSessionData.mPeriodNs,
                                                      flpSessionData.mDistanceIntervalMps,
                                                      flpSessionData.mTripDistanceM);
                }
            }

            return result;
        }

        public int updateFlpSession(int id,
                                    int flags,
                                    long period_ms,
                                    int distance_interval_mps,
                                    long trip_distance_m) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): updateFlpSession()" +
                           "; id is " + id +
                           "; period_ms is " + period_ms +
                           "; flags is " + flags +
                           "; distance_ms is " + distance_interval_mps +
                           "; trip distance " + trip_distance_m);
            }

            // BgSessionMap only cares about BG sessions i.e. routine / trip
            synchronized(sCallBacksLock) {
                BgSessionData bgSessData = mFlpUserData.mBgSessionMap.get(id);

                if ((flags == FLP_BG_NOTIFICATION_ROUTINE) ||
                    (flags == FLP_BG_NOTIFICATION_OUTDOOR_TRIP)) {
                    if (bgSessData != null) {
                        bgSessData.mBgNotificationType = flags;
                    } else {
                        // may be the update is happening from a foreground session,
                        // hence callback will be registered after the update call
                        bgSessData = new BgSessionData();
                        bgSessData.mBgNotificationType = flags;
                        mFlpUserData.mBgSessionMap.put(id, bgSessData);
                    }
                }
            }

            if (!mFlpUserData.mFlpSessionMap.containsKey(id)) {
                Log.e(TAG, "Invalid FlpSession id: " + id);
                return FLP_RESULT_ERROR;
            }

            int result;
            FlpSessionData flpSessionData = mFlpUserData.mFlpSessionMap.get(id);
            synchronized (sLocationSettingsLock) {
                flpSessionData.mId = id;
                flpSessionData.mFlags = flags;
                flpSessionData.mPeriodNs = MilliToNano(period_ms);
                flpSessionData.mDistanceIntervalMps = distance_interval_mps;
                flpSessionData.mTripDistanceM = trip_distance_m;

                if (!mIsLocationSettingsOn) {
                    result =  FLP_RESULT_SUCCESS;
                } else {
                    result =  native_flp_update_session(flpSessionData.mId,
                                                        flpSessionData.mFlags,
                                                        flpSessionData.mPeriodNs,
                                                        flpSessionData.mDistanceIntervalMps,
                                                        flpSessionData.mTripDistanceM);
                }
            }

            return result;
        }

        public int stopFlpSession(int id) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): stopFlpSession(); id is " + id);
            }

            synchronized(sCallBacksLock) {
                mFlpUserData.mBgSessionMap.remove(id);
            }

            if (!mFlpUserData.mFlpSessionMap.containsKey(id)) {
                Log.e(TAG, "Invalid FlpSession id: " + id);
                return FLP_RESULT_ERROR;
            }

            int result;
            synchronized (sLocationSettingsLock) {
                if (!mIsLocationSettingsOn) {
                    result = FLP_RESULT_SUCCESS;
                } else {
                    result = native_flp_stop_session(id);
                }

                mFlpUserData.mFlpSessionMap.remove(id);
            }

            return result;
        }

        public int pullLocations(final ILocationCallback cb,
                                 final long sessionStartTime,
                                 final int id) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): pullLocations(), sessionStartTime is "
                           + sessionStartTime);
            }
            if (cb == null) {
                Log.e(TAG, "in IFlpService.Stub(): cb is null.");
                return FLP_RESULT_ERROR;
            }

            if (!mFlpUserData.mFlpSessionMap.containsKey(id)) {
                Log.e(TAG, "Invalid FlpSession id: " + id);
                return FLP_RESULT_ERROR;
            }

            int result;
            synchronized (sLocationSettingsLock) {
                if (!mIsLocationSettingsOn) {
                    result = FLP_RESULT_SUCCESS;
                } else {
                    synchronized (sCallBacksLock) {
                        // save the cb
                        mFlpUserData.mQueryCbQueue.add(
                                new Pair<ILocationCallback, Long>(cb, sessionStartTime));
                    }
                    result = native_flp_get_all_locations(id);
                }
            }

            return result;
        }
    };

    /* Remote binder */
    private final ITestService.Stub mTestingBinder = new ITestService.Stub() {
        public void deleteAidingData(long flags) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): deleteAidingData(). Flags: " + flags);
            }
            native_flp_delete_aiding_data(flags);
        }

        public void updateXtraThrottle(boolean enabled) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): updateXtraThrottle(). enabled: " + enabled);
            }
            native_flp_update_xtra_throttle(enabled);
        }

        public void registerMaxPowerChangeCallback(final IMaxPowerAllocatedCallback cb) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): registerMaxPowerChangeCallback()");
            }
            if (cb != null) {
                if (mMaxPowerCallbacks != null) {
                    mMaxPowerCallbacks.register(cb);
                    mFlpUserData.mMaxPowerCallbacksList.add(cb);

                    try {
                        cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                            @Override
                            public void binderDied() {
                                mMaxPowerCallbacks.unregister(cb);
                                mFlpUserData.mMaxPowerCallbacksList.remove(cb);
                            }
                        }, 0);
                        native_flp_get_max_power_allocated_in_mw();
                    } catch (RemoteException e) {
                        throw new RuntimeException("Failed clean up", e);
                    }
                } else {
                    Log.e(TAG, "mMaxPowerCallbacks is null");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public void unregisterMaxPowerChangeCallback(IMaxPowerAllocatedCallback cb) {
            if (VERBOSE) {
                Log.d(TAG, "in IFlpService.Stub(): unregisterMaxPowerChangeCallback()");
            }
            if (cb != null) {
                if (mMaxPowerCallbacks != null) {
                    mMaxPowerCallbacks.unregister(cb);
                } else {
                    Log.e(TAG, "mMaxPowerCallbacks is null");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }
    };

    private void onLocationReport(Location[] locations, int reportTrigger, int batchingMode) {
        if (VERBOSE) {
            Log.v(TAG, "entering onLocationReport() reportTrigger is " + reportTrigger +
                       "; Batching Mode is " + batchingMode +
                       "; and the first timestamp is " + locations[0].getTime());
        }
        if (reportTrigger == LOCATION_REPORT_ON_FULL_INDICATION) {
            // Broadcast to all batching callbacks the new value.
            synchronized (sCallBacksLock) {
                int index = mCallbacksForBg.beginBroadcast();
                for (int i = 0; i < index; i++) {
                    try {
                        int sessionId = (Integer) mCallbacksForBg.getBroadcastCookie(i);
                        BgSessionData bgSessData = mFlpUserData.mBgSessionMap.get(sessionId);
                        if (bgSessData == null) {
                            Log.e(TAG, "No Background session data found for id:" + index);
                            continue;
                        }
                        long sessionStartTime = (long) bgSessData.mSessionStartTime;

                        if (VERBOSE) {
                            Log.v(TAG, "in the mCallbacksForBg loop : " + i +
                                "; timestamp is " + sessionStartTime +
                                "; notification Type is " + bgSessData.mBgNotificationType);
                        }
                        if (((bgSessData.mBgNotificationType == FLP_BG_NOTIFICATION_ROUTINE) &&
                            (batchingMode == BATCHING_MODE_ROUTINE)) ||
                            (bgSessData.mBgNotificationType == FLP_BG_NOTIFICATION_OUTDOOR_TRIP) &&
                            (batchingMode == BATCHING_MODE_OUTDOOR_TRIP)) {
                            if (sessionStartTime<=locations[0].getTime()) {
                                // return the whole batch
                                if (VERBOSE) {
                                    Log.v(TAG, "return whole batch");
                                }
                                mCallbacksForBg.getBroadcastItem(i).onLocationAvailable(locations);
                            } else if (sessionStartTime > locations[locations.length-1].getTime()) {
                                if (VERBOSE) {
                                    Log.v(TAG, "no need to return");
                                }
                            } else {
                                // find the offset
                                int offset = getOffset(sessionStartTime, locations);
                                Location[] newResult = new Location[locations.length - offset];
                                System.arraycopy(locations,
                                                 offset,
                                                 newResult,
                                                 0,
                                                 locations.length - offset);
                                mCallbacksForBg.getBroadcastItem(i).onLocationAvailable(newResult);
                            }
                        }
                    } catch (RemoteException e) {
                    // The RemoteCallbackList will take care of removing
                    // the dead object.
                    }
                }
                mCallbacksForBg.finishBroadcast();
            }
         } else if (reportTrigger == LOCATION_REPORT_ON_FIX_INDICATION) {
            // Broadcast to all tracking callbacks the new value.
            synchronized (sCallBacksLock) {
                int index = mCallbacksForFg.beginBroadcast();
                for (int i = 0; i < index; i++) {
                    try {
                        long sessionStartTime = (long) mCallbacksForFg.getBroadcastCookie(i);
                        if (VERBOSE) {
                            Log.v(TAG, "in the mCallbacksForFg loop : " + i
                                       + "; cd timestamp is" + sessionStartTime);
                        }
                        if (sessionStartTime <= locations[0].getTime()) {
                            // return the whole batch
                            if (VERBOSE) {
                                Log.v(TAG, "return whole batch");
                            }
                            mCallbacksForFg.getBroadcastItem(i).onLocationAvailable(locations);
                        } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                            if (VERBOSE) {
                                Log.v(TAG, "no need to return");
                            }
                        } else {
                            // find the offset
                            int offset = getOffset(sessionStartTime, locations);
                            Location[] newResult = new Location[locations.length - offset];
                            System.arraycopy(locations,
                                             offset,
                                             newResult,
                                             0,
                                             locations.length - offset);
                            mCallbacksForFg.getBroadcastItem(i).onLocationAvailable(newResult);
                        }
                    } catch (RemoteException e) {
                        // The RemoteCallbackList will take care of removing
                        // the dead object.
                    }
                }
                mCallbacksForFg.finishBroadcast();
            }
        } else if (reportTrigger == LOCATION_REPORT_ON_QUERY) {
            synchronized (sCallBacksLock) {
                if (!mFlpUserData.mQueryCbQueue.isEmpty()) {
                    Pair<ILocationCallback, Long> cbPair = mFlpUserData.mQueryCbQueue.remove();
                    if (cbPair != null) {
                        try {
                            // check the timestamp, find the offset
                            ILocationCallback callback = cbPair.first;
                            long sessionStartTime = cbPair.second;
                            if (VERBOSE) {
                                Log.v(TAG, "calling callback for" +
                                           " pulling, sessionStartTime is " +
                                            sessionStartTime);
                            }
                            if (sessionStartTime <= locations[0].getTime()) {
                                // return the whole batch
                                if (VERBOSE) {
                                    Log.v(TAG, "return whole batch");
                                }
                                callback.onLocationAvailable(locations);
                            } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                                if (VERBOSE) {
                                    Log.v(TAG, "no need to return");
                                }
                            } else {
                                int offset = getOffset(sessionStartTime, locations);
                                Location[] newResult = new Location[locations.length - offset];
                                System.arraycopy(locations,
                                                 offset,
                                                 newResult,
                                                 0,
                                                 locations.length - offset);
                                callback.onLocationAvailable(newResult);
                            }
                            // update the timestamp of the callback
                            if (mCallbacksForBg.unregister(callback)) {
                                mCallbacksForBg.register(callback, System.currentTimeMillis());
                                mFlpUserData.mCallbacksForBgMap.put(callback,
                                        System.currentTimeMillis());
                            }
                            if (mCallbacksForFg.unregister(callback)) {
                                mCallbacksForFg.register(callback, System.currentTimeMillis());
                                mFlpUserData.mCallbacksForFgMap.put(callback,
                                        System.currentTimeMillis());
                            }
                            if (mCallbacksForPassive.unregister(callback)) {
                                mCallbacksForPassive.register(callback, System.currentTimeMillis());
                                mFlpUserData.mCallbacksForPassiveMap.put(callback,
                                        System.currentTimeMillis());
                            }
                        } catch (RemoteException e) {
                            // The RemoteCallbackList will take care of removing
                            // the dead object.
                        }
                    }
                } else {
                    Log.e(TAG, "no available callback on query");
                }
            }
        } else if (reportTrigger == LOCATION_REPORT_ON_INDICATIONS) {
            /*
               For legacy behaviour, the callback are in the passive
               listeners already, so do nothing here.
            */
        } else {
            Log.e(TAG, "unknown reportTrigger");
        }

        // passive listeners
        if (mCallbacksForPassive.getRegisteredCallbackCount() > 0) {
            // Broadcast to all passive listeners
            synchronized (sCallBacksLock) {
                int index = mCallbacksForPassive.beginBroadcast();
                for (int i = 0; i < index; i++) {
                    try {
                        long sessionStartTime = (long) mCallbacksForPassive.getBroadcastCookie(i);
                        if (VERBOSE) {
                            Log.v(TAG, "in the mCallbacksForPassive loop : " + i
                                       + "; cd timestamp is" + sessionStartTime);
                        }
                        if (sessionStartTime<=locations[0].getTime()) {
                            // return the whole batch
                            if (VERBOSE) {
                                Log.v(TAG, "return whole batch");
                            }
                            mCallbacksForPassive.getBroadcastItem(i).onLocationAvailable(locations);
                        } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                            if (VERBOSE) {
                                Log.v(TAG, "no need to return");
                            }
                        } else {
                            // find the offset
                            int offset = getOffset(sessionStartTime, locations);
                            Location[] newResult = new Location[locations.length - offset];
                            System.arraycopy(locations,
                                             offset,
                                             newResult,
                                             0,
                                             locations.length - offset);
                            mCallbacksForPassive.getBroadcastItem(i).onLocationAvailable(newResult);
                        }
                    } catch (RemoteException e) {
                        // The RemoteCallbackList will take care of removing
                        // the dead object.
                    }
                }
                mCallbacksForPassive.finishBroadcast();
            }
        }
    }

    private void onBatchingStatusCb(int batchingStatus, int[] completedTripClientIds) {
        if (VERBOSE) {
            Log.d(TAG, "entering onBatchingStatusCb batchingStatus :" + batchingStatus);
        }
        synchronized(sStatusCallbacksLock) {
            List<Integer> completedTripClientList = null;

            if (completedTripClientIds != null) {
                completedTripClientList = new ArrayList<Integer>(completedTripClientIds.length);
                for (int index = 0; index < completedTripClientIds.length; index++) {
                    completedTripClientList.add(completedTripClientIds[index]);
                }
            }

            int broadcastCount = mCallbacksForStatus.beginBroadcast();
            for (int broadcastIndex = 0; broadcastIndex < broadcastCount; broadcastIndex++) {
                try {
                    if (batchingStatus != BATCHING_STATUS_TRIP_COMPLETED) {
                        mCallbacksForStatus.getBroadcastItem(broadcastIndex).onBatchingStatusCb(
                                batchingStatus);
                    } else {
                        int sessionId =
                                (int) mCallbacksForStatus.getBroadcastCookie(broadcastIndex);
                        if ((completedTripClientList != null) &&
                                (completedTripClientList.contains(sessionId))) {
                            mCallbacksForStatus.getBroadcastItem(broadcastIndex).onBatchingStatusCb(
                                    batchingStatus);
                        }
                    }
                } catch(RemoteException e) {
                  // The RemoteCallbackList will take care of removing
                  // the dead object.
                }
            }
            mCallbacksForStatus.finishBroadcast();
        }
    }

    private void onMaxPowerAllocatedChanged(int power_mW) {
        if (VERBOSE) {
            Log.d(TAG, "entering onMaxPowerAllocatedChanged() power: " + power_mW);
        }
        // Broadcast to all clients the new value.
        int index = mMaxPowerCallbacks.beginBroadcast();
        for (int i = 0; i < index; i++) {
            if (VERBOSE) {
                Log.v(TAG, "in the mMaxPowerCallbacks loop : " + i);
            }
            try {
                IMaxPowerAllocatedCallback cb =
                    mMaxPowerCallbacks.getBroadcastItem(i);
                cb.onMaxPowerAllocatedChanged((double)power_mW);
            } catch (RemoteException e) {
                // The RemoteCallbackList will take care of removing
                // the dead object.
            }
        }
        mMaxPowerCallbacks.finishBroadcast();
    }

    class UserChanged implements DataPerPackageAndUser.UserChangeListener<FlpUserData> {
        @Override
        public void onUserChange(Map<String, FlpUserData> prevUserData,
                                 Map<String, FlpUserData> currentUserData) {
            if (VERBOSE) {
                Log.d(TAG, "Active user has changed, updating FLP callbacks...");
            }

            synchronized (sLocationSettingsLock) {
                // New user switched may be first time running
                if (currentUserData.isEmpty()) {
                    Log.d(TAG, "FLP data for new user is empty, creating new instance.");
                    // mDataPerPackageAndUser.setData would dead-lock in this case
                    currentUserData.put(mDataPerPackageAndUser.getPackageName(null),
                            new FlpUserData());
                }
                FlpUserData currentFlpUserData =
                        currentUserData.get(mDataPerPackageAndUser.getPackageName(null)) ;

                synchronized (sCallBacksLock) {
                    // Unregister all previous callbacks
                    for (ILocationCallback callback : mFlpUserData.mCallbacksForFgMap.keySet()) {
                        mCallbacksForFg.unregister(callback);
                    }

                    for (ILocationCallback callback : mFlpUserData.mCallbacksForBgMap.keySet()) {
                        mCallbacksForBg.unregister(callback);
                    }

                    for (ILocationCallback callback :
                            mFlpUserData.mCallbacksForPassiveMap.keySet()) {
                        mCallbacksForPassive.unregister(callback);
                    }
                }

                synchronized (sStatusCallbacksLock) {
                    for (ISessionStatusCallback callback :
                            mFlpUserData.mCallbacksForStatusMap.keySet()) {
                        mCallbacksForStatus.unregister(callback);
                    }
                }

                for (IMaxPowerAllocatedCallback callback : mFlpUserData.mMaxPowerCallbacksList) {
                    mMaxPowerCallbacks.unregister(callback);
                }

                // Remove any pending callback queue. Wont make sense any more after user switch
                mFlpUserData.mQueryCbQueue.clear();

                if (mIsLocationSettingsOn) {
                    stopFlpSessions();
                }

                // Update to new current user
                mFlpUserData = currentFlpUserData;

                if (mIsLocationSettingsOn) {
                    startFlpSessions();
                }

                synchronized (sCallBacksLock) {
                    // Re-register all previous callbacks
                    for (ILocationCallback callback : mFlpUserData.mCallbacksForFgMap.keySet()) {
                        mCallbacksForFg.register(callback,
                                mFlpUserData.mCallbacksForFgMap.get(callback));
                    }

                    for (ILocationCallback callback : mFlpUserData.mCallbacksForBgMap.keySet()) {
                        mCallbacksForBg.register(callback,
                                mFlpUserData.mCallbacksForBgMap.get(callback));
                    }

                    for (ILocationCallback callback :
                            mFlpUserData.mCallbacksForPassiveMap.keySet()) {
                        mCallbacksForPassive.register(callback,
                                mFlpUserData.mCallbacksForPassiveMap.get(callback));
                    }
                }

                synchronized (sStatusCallbacksLock) {
                    for (ISessionStatusCallback callback :
                            mFlpUserData.mCallbacksForStatusMap.keySet()) {
                        mCallbacksForStatus.register(callback,
                                mFlpUserData.mCallbacksForPassiveMap.get(callback));
                    }
                }

                for (IMaxPowerAllocatedCallback callback : mFlpUserData.mMaxPowerCallbacksList) {
                    mMaxPowerCallbacks.register(callback);
                }
            }
        }

    }

    private int getOffset (long sessionStartTime, Location[] locations) {
        int offset = -1, left = 0, right = locations.length-1;
        while(left <= right) {
            int mid = (left+right)/2;
            // found the exactly match
            if (sessionStartTime ==
                locations[mid].getTime()) {
                offset = mid;
                break;
            }
            if (sessionStartTime >
                locations[mid].getTime()) {
                left = mid + 1;
            }
            if (sessionStartTime <
                locations[mid].getTime()) {
                right = mid - 1;
            }
        }
        if (offset == -1) {
            offset = left;
        }

        if (VERBOSE) {
           Log.v(TAG, "offset : " + offset);
        }
        return offset;
    }

    public IFlpService getFlpBinder() {
        return mBinder;
    }

    public ITestService getFlpTestingBinder() {
        return mTestingBinder;
    }

    static {
        native_flp_class_init();
    }

    private static native void native_flp_class_init();
    private native int native_flp_init();
    private native int native_flp_get_all_supported_features();
    private native int native_flp_start_session(int id,
                                                int flags,
                                                long period_ns,
                                                int distance_interval_mps,
                                                long trip_distance_m);
    private native int native_flp_update_session(int id,
                                                 int flags,
                                                 long period_ns,
                                                 int distance_interval_mps,
                                                 long trip_distance_m);
    private native int native_flp_stop_session(int id);
    private native int native_flp_get_all_locations(int id);
    private native void native_flp_delete_aiding_data(long flags);
    private native void native_flp_update_xtra_throttle(boolean enabled);
    private native void native_flp_get_max_power_allocated_in_mw();
}

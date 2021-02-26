/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.wwandbreceiver;

import android.content.Intent;
import android.location.Location;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.qti.wwandbreceiver.*;

public class WWANDBReceiver {
    private static final String TAG = "WWANDBReceiver";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    private static final int WWANDBRECV_MAX_ALLOWED_PROVIDERS = 1;

    private static final Object sCallBacksLock = new Object();

    private RemoteCallbackList<IWWANDBReceiverResponseListener> mWWANDBReceiverResponseListeners
            = new RemoteCallbackList<IWWANDBReceiverResponseListener>();

    public static WWANDBReceiver sInstance = null;
    public static WWANDBReceiver getInstance() {
        if (sInstance == null) {
            sInstance = new WWANDBReceiver();
        }
        return sInstance;
    }

    public WWANDBReceiver() {
        if (VERBOSE) {
            Log.d(TAG, "WWANDBReceiver construction");
        }
        native_wwan_db_receiver_init();
    }

    /* Remote binder */
    private final IWWANDBReceiver.Stub mBinder = new IWWANDBReceiver.Stub() {

        public boolean registerResponseListener(final IWWANDBReceiverResponseListener callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return false;
            }
            boolean isCallbackAdded = false;

            synchronized (sCallBacksLock) {
                if (WWANDBRECV_MAX_ALLOWED_PROVIDERS >
                    mWWANDBReceiverResponseListeners.getRegisteredCallbackCount()) {
                    isCallbackAdded = mWWANDBReceiverResponseListeners.register(callback);
                }
            }

            if (isCallbackAdded) {
                try {
                    callback.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                        @Override
                        public void binderDied() {
                            synchronized (sCallBacksLock) {
                                mWWANDBReceiverResponseListeners.unregister(callback);
                            }
                        }
                    }, 0);
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed unregister WWAN DB provider cb", e);
                }
            } else {
                synchronized (sCallBacksLock) {
                    mWWANDBReceiverResponseListeners.beginBroadcast();
                    isCallbackAdded =
                            (mWWANDBReceiverResponseListeners.getBroadcastItem(0) == callback);
                    mWWANDBReceiverResponseListeners.finishBroadcast();
                }
            }
            return isCallbackAdded;
        }

        public void removeResponseListener(final IWWANDBReceiverResponseListener callback) {
            if (null == callback) {
                Log.e(TAG, "callback is null");
                return;
            }
            synchronized (sCallBacksLock) {
                mWWANDBReceiverResponseListeners.unregister(callback);
            }
        }


        public int requestBSList(int expireInDays) {
            if (VERBOSE) {
                Log.d(TAG, "in IWWANDBReceiver.Stub(): requestBSList()");
            }
            return native_wwan_db_receiver_request_bs_list(expireInDays);
        }

        public int pushWWANDB(List<BSLocationData> locData,
                              List<BSSpecialInfo> splData,
                              int daysValid) {
            if (VERBOSE) {
                Log.d(TAG, "in IWWANDBReceiver.Stub(): pushWWANDB() ");
            }
            BSLocationData[] locs = locData.toArray(new BSLocationData[0]);

            BSSpecialInfo[] spls = splData.toArray(new BSSpecialInfo[0]);
            return native_wwan_db_receiver_push_ad(locs, spls, daysValid);
        }
    };

    private void onBSListAvailable(BSInfo[] bsInfoList) {
        if (VERBOSE) {
            Log.d(TAG, "onBSListAvailable");
        }
        List bsList = new ArrayList<BSInfo>();
        try {
            bsList.addAll(Arrays.asList(bsInfoList));
        } catch (NullPointerException e) {
            Log.d(TAG, "onBSListAvailable: BS List is NULL");
        }
        synchronized (sCallBacksLock) {
            int index = mWWANDBReceiverResponseListeners.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mWWANDBReceiverResponseListeners.getBroadcastItem(i).onBSListAvailable(bsList);
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mWWANDBReceiverResponseListeners.finishBroadcast();
        }
    }

    private void onStatusUpdate(boolean isSuccess, String error) {
        if (VERBOSE) {
            Log.d(TAG, "onStatusUpdate");
        }
        synchronized (sCallBacksLock) {
            int index = mWWANDBReceiverResponseListeners.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mWWANDBReceiverResponseListeners.getBroadcastItem(i).onStatusUpdate(isSuccess,
                                                                                        error);
                    if (VERBOSE) {
                        Log.d(TAG, "onStatusUpdate: send update to listener");
                    }
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mWWANDBReceiverResponseListeners.finishBroadcast();
        }
    }

    private void onServiceRequest() {
        if (VERBOSE) {
            Log.d(TAG, "onServiceRequest");
        }
        synchronized (sCallBacksLock) {
            int index = mWWANDBReceiverResponseListeners.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mWWANDBReceiverResponseListeners.getBroadcastItem(i).onServiceRequest();
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mWWANDBReceiverResponseListeners.finishBroadcast();
        }
    }

    public IWWANDBReceiver getWWANDBReceiverBinder() {
        return mBinder;
    }

    protected void finalize() throws Throwable {
        native_wwan_db_receiver_deinit();
    }

    static {
        native_wwan_db_receiver_class_init();
    }

    private static native void native_wwan_db_receiver_class_init();
    private native void native_wwan_db_receiver_init();
    private native void native_wwan_db_receiver_deinit();
    private native int native_wwan_db_receiver_request_bs_list(int expireInDays);
    private native int native_wwan_db_receiver_push_ad(BSLocationData[] locData,
                                                       BSSpecialInfo[] splData,
                                                       int daysValid);
}

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.wifidbreceiver;

import android.content.Intent;
import android.location.Location;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.qti.wifidbreceiver.*;

public class WiFiDBReceiver {
    private static final String TAG = "WiFiDBReceiver";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);

    private static final int WIFIDBRECV_MAX_ALLOWED_PROVIDERS = 1;

    private static final Object sCallBacksLock = new Object();

    private RemoteCallbackList<IWiFiDBReceiverResponseListener> mWiFiDBReceiverResponseListeners
        = new RemoteCallbackList<IWiFiDBReceiverResponseListener>();

    public static WiFiDBReceiver sInstance = null;
    public static WiFiDBReceiver getInstance() {
        if (sInstance == null) {
            sInstance = new WiFiDBReceiver();
        }
        return sInstance;
    }

    public WiFiDBReceiver() {
        if (VERBOSE_DBG) {
            Log.d(TAG, "WiFiDBReceiver construction");
        }
        native_wifi_db_receiver_init();
    }

    /* Remote binder */
    private final IWiFiDBReceiver.Stub mBinder = new IWiFiDBReceiver.Stub() {

        public boolean registerResponseListener(final IWiFiDBReceiverResponseListener callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return false;
            }
            boolean isCallbackAdded = false;

            synchronized (sCallBacksLock) {
                if(WIFIDBRECV_MAX_ALLOWED_PROVIDERS >
                   mWiFiDBReceiverResponseListeners.getRegisteredCallbackCount()) {
                    isCallbackAdded = mWiFiDBReceiverResponseListeners.register(callback);
                }
            }

            if(isCallbackAdded) {
                try {
                    callback.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                        @Override
                        public void binderDied() {
                            synchronized (sCallBacksLock) {
                                mWiFiDBReceiverResponseListeners.unregister(callback);
                            }
                        }
                    }, 0);
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed unregister WiFi DB provider cb", e);
                }
            } else {
                synchronized (sCallBacksLock) {
                    mWiFiDBReceiverResponseListeners.beginBroadcast();
                    isCallbackAdded =
                            (mWiFiDBReceiverResponseListeners.getBroadcastItem(0) == callback);
                    mWiFiDBReceiverResponseListeners.finishBroadcast();
                }
            }
            return isCallbackAdded;
        }

        public void removeResponseListener(final IWiFiDBReceiverResponseListener callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            synchronized (sCallBacksLock) {
                mWiFiDBReceiverResponseListeners.unregister(callback);
            }
        }


        public int requestAPList(int expireInDays) {

            if (VERBOSE_DBG) {
                Log.d(TAG, "in IWiFiDBReceiver.Stub(): requestAPList()");
            }
            return native_wifi_db_receiver_request_ap_list(expireInDays);
        }

        public int pushWiFiDB(List<APLocationData> locData,
                              List<APSpecialInfo> splData,
                              int daysValid) {
            if (VERBOSE_DBG) {
                Log.d(TAG, "in IWiFiDBReceiver.Stub(): pushWiFiDB() ");
            }
            APLocationData[] locs = locData.toArray(new APLocationData[0]);

            APSpecialInfo[] spls = splData.toArray(new APSpecialInfo[0]);
            return native_wifi_db_receiver_push_ad(locs, spls, daysValid);
        }
    };

    private void onAPListAvailable(APInfo[] apInfoList) {
        if (VERBOSE_DBG) {
            Log.d(TAG, "onAPListAvailable");
        }
        List apList = new ArrayList<APInfo>();
        try {
            apList.addAll(Arrays.asList(apInfoList));
        } catch (NullPointerException e) {
            Log.d(TAG, "onAPListAvailable: AP List is NULL");
        }
        synchronized (sCallBacksLock) {
            int index = mWiFiDBReceiverResponseListeners.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mWiFiDBReceiverResponseListeners.getBroadcastItem(i).onAPListAvailable(apList);
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mWiFiDBReceiverResponseListeners.finishBroadcast();
        }
    }

    private void onStatusUpdate(boolean isSuccess, String error) {
        if (VERBOSE_DBG) {
            Log.d(TAG, "onStatusUpdate");
        }
        synchronized (sCallBacksLock) {
            int index = mWiFiDBReceiverResponseListeners.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mWiFiDBReceiverResponseListeners.getBroadcastItem(i).onStatusUpdate(isSuccess,
                                                                                        error);
                    if (VERBOSE_DBG) {
                        Log.d(TAG, "onStatusUpdate: send update to listener");
                    }
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mWiFiDBReceiverResponseListeners.finishBroadcast();
        }
    }

    private void onServiceRequest() {
        if (VERBOSE_DBG) {
            Log.d(TAG, "onServiceRequest");
        }
        synchronized (sCallBacksLock) {
            int index = mWiFiDBReceiverResponseListeners.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mWiFiDBReceiverResponseListeners.getBroadcastItem(i).onServiceRequest();
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mWiFiDBReceiverResponseListeners.finishBroadcast();
        }
    }

    public IWiFiDBReceiver getWiFiDBReceiverBinder() {
        return mBinder;
    }

    protected void finalize() throws Throwable {
        native_wifi_db_receiver_deinit();
    }

    static {
        native_wifi_db_receiver_class_init();
    }

    private static native void native_wifi_db_receiver_class_init();
    private native void native_wifi_db_receiver_init();
    private native void native_wifi_db_receiver_deinit();
    private native int native_wifi_db_receiver_request_ap_list(int expireInDays);
    private native int native_wifi_db_receiver_push_ad(APLocationData[] locData,
                                                       APSpecialInfo[] splData,
                                                       int daysValid);
}

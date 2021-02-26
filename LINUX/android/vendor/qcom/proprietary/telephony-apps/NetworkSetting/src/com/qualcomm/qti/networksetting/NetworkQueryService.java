/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.networksetting;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.OperatorInfo;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;

import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

import java.util.ArrayList;

/**
 * Service code used to assist in querying the network for service
 * availability.
 */
public class NetworkQueryService extends Service {
    // debug data
    private static final String LOG_TAG = "NetworkQuery";
    private static final boolean DBG = true;

    private static int PHONE_COUNT;

    // static events
    private static final int EVENT_NETWORK_SCAN_COMPLETED = 100;

    // static states indicating the query status of the service
    private static final int QUERY_READY = -1;
    private static final int QUERY_IS_RUNNING = -2;

    // error statuses that will be retured in the callback.
    public static final int QUERY_OK = 0;
    private static final int QUERY_PARTIAL = 1;
    public static final int QUERY_EXCEPTION = -1;
    static final String ACTION_LOCAL_BINDER = "com.android.phone.intent.action.LOCAL_BINDER";

    /** state of the query service */

    private static final String ACTION_INCREMENTAL_NW_SCAN_IND
        = "qualcomm.intent.action.ACTION_INCREMENTAL_NW_SCAN_IND";

    //Querydetails for each phoneId
    private QueryDetails[] mQueryDetails;

    class QueryDetails {
        int state;
        String[] storeScanInfo;

        QueryDetails() {
            state = QUERY_READY;
            storeScanInfo = null;
        }

        void concatScanInfo(String[] scanInfo) {
            String[] concatScanInfo = new String[storeScanInfo.length + scanInfo.length];
            System.arraycopy(storeScanInfo, 0, concatScanInfo, 0, storeScanInfo.length);
            System.arraycopy(scanInfo, 0, concatScanInfo, storeScanInfo.length,
                    scanInfo.length);
            storeScanInfo= concatScanInfo;
        }

        void reset() {
            state = QUERY_READY;
            storeScanInfo = null;
        }
    }

    /**
     * Class for clients to access.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with
     * IPC.
     */
    public class LocalBinder extends Binder {
        INetworkQueryService getService() {
            return mBinder;
        }
    }
    private final IBinder mLocalBinder = new LocalBinder();

    /**
     * Local handler to receive the network query compete callback
     * from the RIL.
     */
    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                // if the scan is complete, broadcast the results.
                // to all registerd callbacks.
                case EVENT_NETWORK_SCAN_COMPLETED:
                    if (DBG) log("scan completed, broadcasting results");
                    broadcastQueryResults((AsyncResult) msg.obj);
                    break;
            }
        }
    };

    /**
     * List of callback objects, also used to synchronize access to
     * itself and to changes in state.
     */
    final RemoteCallbackList<INetworkQueryServiceCallback> mCallbacks =
        new RemoteCallbackList<INetworkQueryServiceCallback> ();

    /**
     * Implementation of the INetworkQueryService interface.
     */
    private final INetworkQueryService.Stub mBinder = new INetworkQueryService.Stub() {

        /**
         * Starts a query with a INetworkQueryServiceCallback object if
         * one has not been started yet.  Ignore the new query request
         * if the query has been started already.  Either way, place the
         * callback object in the queue to be notified upon request
         * completion.
         */
        public void startNetworkQuery(INetworkQueryServiceCallback cb, int subId) {
            if (cb != null) {
                // register the callback to the list of callbacks.
                synchronized (mCallbacks) {
                    int phoneId = SubscriptionManager.getPhoneId(subId);
                    log("phoneIndex" + phoneId);
                    if (!SubscriptionManager.isValidPhoneId(phoneId)) {
                        return;
                    }
                    mCallbacks.register(cb, new Integer(phoneId));
                    if (DBG) log("registering callback " + cb.getClass().toString());
                    QueryDetails queryDetails = mQueryDetails[phoneId];
                    switch (queryDetails.state) {
                        case QUERY_READY:
                            // TODO: we may want to install a timeout here in case we
                            // do not get a timely response from the RIL.
                            queryDetails.storeScanInfo = null;
                            boolean success = mQcRilHook.qcRilPerformIncrManualScan(phoneId);
                            log("start scan, result:" + success);
                            if (!success) {
                                Phone phone = PhoneFactory.getPhone(phoneId);
                                if (phone != null) {
                                    phone.getAvailableNetworks(
                                            mHandler.obtainMessage(EVENT_NETWORK_SCAN_COMPLETED,
                                                    new Integer(phoneId)));
                                    queryDetails.state = QUERY_IS_RUNNING;
                                    if (DBG) log("starting new query");
                                } else {
                                    if (DBG) {
                                        log("phone is null");
                                    }
                                }
                            } else {
                                 queryDetails.state = QUERY_IS_RUNNING;
                            }
                            break;

                        // do nothing if we're currently busy.
                        case QUERY_IS_RUNNING:
                            if (DBG) log("query already in progress");
                            break;
                        default:
                    }
                }
            }
        }

        /**
         * Stops a query with a INetworkQueryServiceCallback object as
         * a token.
         */
        public void stopNetworkQuery(INetworkQueryServiceCallback cb, int subId) {
            unregisterCallback(cb);
            int phoneId = SubscriptionManager.getPhoneId(subId);
            log("phoneIndex" + phoneId);
            if (!SubscriptionManager.isValidPhoneId(phoneId)) {
                if (DBG) log("invalid phone id");
                return;
            }
            synchronized (mCallbacks) {
                QueryDetails queryDetails = mQueryDetails[phoneId];
                switch (queryDetails.state) {
                    case QUERY_READY:
                        if (DBG) log("network scan not running.. no need to send cancel");
                        break;

                    case QUERY_IS_RUNNING:
                        boolean success = mQcRilHook.qcRilAbortNetworkScan(phoneId);
                        if (!success) {
                            if (DBG) log("failed to cancel ongoing network query");
                        } else {
                            if (DBG) log("successfully cancelled ongoing network query");
                            queryDetails.state = QUERY_READY;
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        /**
         * Unregisters the callback without impacting an underlying query.
         */
        public void unregisterCallback(INetworkQueryServiceCallback cb) {
            if (cb != null) {
                synchronized (mCallbacks) {
                    if (DBG) log("unregistering callback " + cb.getClass().toString());
                    mCallbacks.unregister(cb);
                }
            }
        }
    };

    private QcRilHook mQcRilHook;
    private QcRilHookCallback mQcRilHookCallback = new QcRilHookCallback() {
        @Override
        public void onQcRilHookReady() {
            //check if required
            log("onqcrilhookready");
        }
        @Override
        public void onQcRilHookDisconnected() {
            // TODO: Handle QcrilMsgTunnelService's disconnection.
            log("onqcrilhookdisconnected");
        }
    };

    @Override
    public void onCreate() {
        PHONE_COUNT = TelephonyManager.getDefault().getPhoneCount();
        mQueryDetails = new QueryDetails[PHONE_COUNT];
        for (int i = 0; i < PHONE_COUNT; i++) {
            mQueryDetails[i] = new QueryDetails();
        }
        mQcRilHook = new QcRilHook(this, mQcRilHookCallback);
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_INCREMENTAL_NW_SCAN_IND);
        intentFilter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        registerReceiver(mBroadcastReceiver, intentFilter);
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            log("onreceive " + intent);
            if (ACTION_INCREMENTAL_NW_SCAN_IND.equals(intent.getAction())) {
                broadcastIncrementalQueryResults(intent);
            } else if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(intent.getAction())) {
                for  (int i=0; i< PHONE_COUNT ; i++) {
                    int subId[] = SubscriptionManager.getSubId(i);
                    if (subId != null && subId.length > 0) {
                    int simState = TelephonyManager.getDefault().getSimState(
                            SubscriptionManager.getSlotIndex(subId[0]));
                        if (simState != TelephonyManager.SIM_STATE_READY) {
                            log("SIM state is not ready: sim_state = " + simState + "slot = "
                                    + SubscriptionManager.getSlotIndex(subId[0]));
                            mQueryDetails[i].reset();
                        }
                    }
                }
            }
        }
    };

    /**
     * Required for service implementation.
     */
    @Override
    public void onStart(Intent intent, int startId) {
    }

    @Override
    public void onDestroy() {
        unregisterReceiver(mBroadcastReceiver);
    }
    /**
     * Handle the bind request.
     */
    @Override
    public IBinder onBind(Intent intent) {
        if (DBG) log("binding service implementation");
        if (ACTION_LOCAL_BINDER.equals(intent.getAction())) {
            return mLocalBinder;
        }

        return mBinder;
    }

    /**
     * Broadcast the results from the query to all registered callback
     * objects.
     */
    private void broadcastQueryResults (AsyncResult ar) {
        // reset the state.
        if (ar == null) {
            if (DBG) log("AsyncResult is null.");
            return;
        }
        Integer phoneIndex = (Integer)ar.userObj;
        int phoneId = phoneIndex.intValue();
        if (!SubscriptionManager.isValidPhoneId(phoneId))
            return;
        QueryDetails queryDetails = mQueryDetails[phoneId];
        synchronized (mCallbacks) {
            if (queryDetails.state == QUERY_READY)
                return;
            queryDetails.state = QUERY_READY;

            // see if we need to do any work.
            // TODO: we may need greater accuracy here, but for now, just a
            // simple status integer will suffice.
            int exception = (ar.exception == null) ? QUERY_OK : QUERY_EXCEPTION;
            if (DBG) log("AsyncResult has exception " + exception);

            // Make the calls to all the registered callbacks.
            for (int i = (mCallbacks.beginBroadcast() - 1); i >= 0; i--) {
                INetworkQueryServiceCallback cb = mCallbacks.getBroadcastItem(i);
                if (DBG) log("broadcasting results to " + cb.getClass().toString());
                try {
                    if (phoneIndex.equals(mCallbacks.getBroadcastCookie(i)))
                        cb.onQueryComplete((ArrayList<OperatorInfo>) ar.result, exception);
                } catch (RemoteException e) {
                }
            }
            // finish up.
            mCallbacks.finishBroadcast();
        }
    }

    /**
     * Broadcast the results from the network scan broadcast to all registered callback
     * objects.
     */
    private void broadcastIncrementalQueryResults(Intent intent) {
        synchronized (mCallbacks) {
            int result = intent.getIntExtra("scan_result", QUERY_EXCEPTION);
            int phoneId = intent.getIntExtra("sub_id", SubscriptionManager.DEFAULT_PHONE_INDEX);
            log("phoneid " + phoneId);
            if (!SubscriptionManager.isValidPhoneId(phoneId))
                return;
            String[] scanInfo = intent.getStringArrayExtra("incr_nw_scan_data");
            QueryDetails queryDetails = mQueryDetails[phoneId];
            if (queryDetails.state == QUERY_READY)
                return;
            if (result != QUERY_PARTIAL) {
                queryDetails.state = QUERY_READY;
            }
            int[] subIds = SubscriptionManager.getSubId(phoneId);
            if (subIds != null && subIds.length > 0) {
                log("on scann result received, result:" + result + ", subscription:"
                            + subIds[0] + ",scanInfo:" + (scanInfo == null ? 0 : scanInfo.length));
                if (queryDetails.storeScanInfo != null && scanInfo != null) {
                   queryDetails.concatScanInfo(scanInfo);
                } else {
                   queryDetails.storeScanInfo = scanInfo;
                }
                Integer phoneIndex = new Integer(phoneId);
                for (int i = (mCallbacks.beginBroadcast() - 1); i >= 0; i--) {
                    INetworkQueryServiceCallback cb = mCallbacks.getBroadcastItem(i);
                    if (DBG) log("broadcasting results to " + cb.getClass().toString());
                    try {
                        if (phoneIndex.equals(mCallbacks.getBroadcastCookie(i))) {
                            cb.onIncrementalManualScanResult(queryDetails.storeScanInfo, result);
                        }
                    } catch (RemoteException e) {
                        log("Partial query remote exception " + e);
                    }
                }
                // finish up
                mCallbacks.finishBroadcast();
            } else {
                    log("no valid sub_id for nw scan result");
            }
        }
    }

    private static void log(String msg) {
        Log.d(LOG_TAG, msg);
    }
}

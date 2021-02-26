/******************************************************************************
 * @file    LteDirectDiscovery.java
 * @brief   This class contains the APIs for LTE Direct Discovery service.
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.ltedirect;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import com.qualcomm.qti.ltedirect.internal.ILteDirectDiscovery;
import com.qualcomm.qti.ltedirect.internal.ILteDirectDiscoveryService;
import com.qualcomm.qti.ltedirect.internal.ILteDirectDiscoveryStatusListener;
import com.qualcomm.qti.ltedirect.internal.LteDirectDiscoveryCapability;

/**
 * The LteDirectDiscovery class provides the APIs for participating the Lte Direct Discovery
 * service. This allows applications on one device to publish their identity (ie., Expression
 * or ProSe AppID) and discover the applications/peers on the other devices in proximity
 * ~500meters.
 *
 *
 * <p> In order to use the Lte Direct Discovery service, applications needs to call the
 * {@link #connectService} API to establish the binder connection to the service.
 * The applications shall call {@link #disconnectService} once they finished using the service.
 *
 *
 * <p> Each application need to have a unique identifier named OsAppId to participate in the
 * Lte Direct Discovery operations (Applications are allowed to use multiple osAppId's).
 * Applications need to create a client handle (for each OsAppId) by invoking the API
 * {@link #initializeClient} with the OsAppId and the {@link StatusListener} object, before
 * initiating any operation.  Once the application receives the successful
 * {@link initializeClient} response, (ie., no-null {@link ClientHandle} object), it can
 * start initiating the Lte Direct Discovery service operations. The {@link StatusListener}
 * object passed in the {@link initializeClient} is used to notify the response callbacks
 * of the {@link ClientHandle} APIs and the unsolicited indications.
 *
 *
 * <p> The applications can start publishing its identity (Expression) by calling the
 * {@link ClientHandle#publish} API. The response of the API is notified via the
 * {@link StatusListener#onPublishresponse}.  If the response is success, the device starts
 * broadcasting the expression for the time specified in the parameter duration in the publish
 * request. Once the time elapses, the service will notify through the
 * {@link StatusListener#onTransmissionStatus} with status as completed.
 *
 * <p> The applications can cancel the ongoing publish operation by calling the
 * {@link ClientHandle#cancelPublish} API.
 *
 *
 * <p> The applications can start discovering the nearby devices or application's
 * identity/expressions by calling the {@link ClientHandle#subscribe} API. The response of the API is
 * notified via the {@link StatusListener#onSubscribeResponse}.  If the response is success,
 * the device starts monotoring the broadcast indications from the nearby devices for the
 * time period mentioned in the duration in the subscribe request.  Once the time elapses,
 * the service will notify through the {@link StatusListener#onReceptionStatus} with status
 * as completed.  If the device finds a nearby devices which matches the requested expression
 * it will be notified through the {@link StatusListener#onMatchEvent}.
 *
 * <p> The applications can cancel the ongoing subscribe operation by calling the
 * {@link ClientHandle#cancelSubscribe} API.
 *
 * <p> In case of any failures for publish or subscribe request, the
 * {@link StatusListener#onAuthorizationResult} or {@link StatusListener#onExpressionStatus}
 * shall be invoked with appropriate error codes.
 *
 * <p> The applications can cancel all the ongoing operations including publish and subscribe
 * by calling the {@link ClientHandle#terminate} API.
 *
 * @version     1.0.0
 */
public class LteDirectDiscovery {
    private static final String LOG_TAG = "LteDirectDiscovery";

    private static final ComponentName DEFAULT_CONTAINER_COMPONENT =
        new ComponentName("com.qualcomm.qti.ltedirect",
                "com.qualcomm.qti.ltedirect.LteDirectDiscoveryService");

    /* Context */
    private Context mContext;

    /* used internally to check the status of serviceConnection */
    private Boolean mServiceConnected;

    private LteDirectDiscoveryServiceConnection mConnection;
    private ILteDirectDiscoveryService mService = null;

    //List of handlers to store with ids
    private Handler mServiceConnectionStatusHandler = null;
    private int mServiceConnectionStatusId;



    /**
     * Defines operation result codes.
     * This is the result code passed in the asynchronous response callback menthods
     * in {@link StatusListener}
     */
    public static class OperationResult {
        /**
         * Operation success.
         */
        public static final int SUCCESS          = 0;
        /**
         * Operation failed for a generic reason..
         */
        public static final int GENERIC_FAILURE  = 1;
        /**
         * Operation failed due to the device not supported.
         */
        public static final int NOT_SUPPORTED    = 2;
        /**
         * Operation failed due to the app authentication failure.
         */
        public static final int APP_AUTH_FAILURE = 3;

        /**
         * Convert the OperationResult to string.
         */
        public static String toString(int result) {
            switch (result) {
                case SUCCESS:
                    return "SUCCESS";
                case GENERIC_FAILURE:
                    return "GENERIC_FAILURE";
                case NOT_SUPPORTED:
                    return "NOT_SUPPORTED";
                case APP_AUTH_FAILURE:
                    return "APP_AUTH_FAILURE";
            }
            return "<Unknown: " + result + " >";
        }
    }

    /**
     * Defines Expression result codes.
     * This is the expression result code passed in the unsolicied response callback menthods
     * {@link StatusListener#onAuthorizationResult} and {@link StatusListener#onExpressionStatus}
     */
    public static class ResultCode {
        /**
         * Operation success.
         */
        public static final int SUCCESS                 = 0;
        /**
         * Operation failed due to a generic failure.
         */
        public static final int GENERIC_FAILURE         = 1;
        /**
         * Operation is temporarily successful.
         */
        public static final int IN_PROGRESS             = 2;
        /**
         * Operation failed as expression is invalid or defined out-of-scope.
         */
        public static final int INVALID_EXPRESSION_SCOPE = 3;
        /**
         * Operation failed due to unknown expression.
         */
        public static final int UNKNOWN_EXPRESSION      = 4;
        /**
         * Operation failed due to invalid discovery type.
         */
        public static final int INVALID_DISCOVERY_TYPE  = 5;
        /**
         * Operation failed as LTE Direct Discovery service is not available.
         */
        public static final int SERVICE_NOT_AVAILABLE   = 6;
        /**
         * Operation failed as app is not authorized for LTE-D
         */
        public static final int APP_AUTH_FAILURE        = 7;
        /**
         * Operation failed as LTE-D feature is not supported.
         */
        public static final int NOT_SUPPORTED           = 8;

        /**
         * Convert the OperationResult to string.
         */
        public static String toString(int result) {
            switch (result) {
                case SUCCESS:
                    return "SUCCESS";
                case GENERIC_FAILURE:
                    return "GENERIC_FAILURE";
                case IN_PROGRESS:
                    return "IN_PROGRESS";
                case INVALID_EXPRESSION_SCOPE:
                    return "INVALID_EXPRESSION_SCOPE";
                case UNKNOWN_EXPRESSION:
                    return "UNKNOWN_EXPRESSION";
                case INVALID_DISCOVERY_TYPE:
                    return "INVALID_DISCOVERY_TYPE";
                case SERVICE_NOT_AVAILABLE:
                    return "SERVICE_NOT_AVAILABLE";
                case APP_AUTH_FAILURE:
                    return "APP_AUTH_FAILURE";
                case NOT_SUPPORTED:
                    return "NOT_SUPPORTED";
            }
            return "<Unknown: " + result + " >";
        }
    }


    /**
     * Defines Discovery Type.
     * This is the discovery type passed in the {@link ClientHandle#publish} and
     * {@link ClientHandle#subscribe} methods.
     */
    public static class DiscoveryType {
        /**
         * Open Discovery.
         */
        public static final int OPEN       = 1;
        /**
         * Restricted Discovery.
         */
        public static final int RESTRICTED = 2;
    }


    private class LteDirectDiscoveryServiceConnection implements ServiceConnection {
        public void onServiceConnected(ComponentName name, IBinder boundService) {
            Log.e(LOG_TAG, "Onservice connected, get the binder object");
            mService = ILteDirectDiscoveryService.Stub.asInterface((IBinder) boundService);
            Log.e(LOG_TAG, "LteDirectDiscoveryService obtained from the binber!!!!");
            mServiceConnected = true;
            if(mServiceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            }else {
                Message msg = mServiceConnectionStatusHandler.
                    obtainMessage(mServiceConnectionStatusId, mServiceConnected);
                msg.sendToTarget();
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.i(LOG_TAG, "onServiceDisconnected, release the binder object");
            mService = null;
            mServiceConnected = false;
            if(mServiceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            }else {
                Message msg = mServiceConnectionStatusHandler.
                    obtainMessage(mServiceConnectionStatusId, mServiceConnected);
                msg.sendToTarget();
            }
        }
    }


    /**
     * Constructor.
     * Creates a new LteDirectDiscovery instance.
     */
    public LteDirectDiscovery(Context context) {
        mContext = context;
        mServiceConnected = false;
    }

    /**
     * Check if the service is connected or not.
     *
     * @return boolean true if service is connected, false oterwise
     */
    public boolean isServiceConnected() {
        return mServiceConnected;
    }

    /**
     * Initiate connection with the service.
     * Applications need to pass the callbackHandler and serviceConnectionStatusId for keeping
     * track of whether the application is connected to the LteDirectDiscovery service
     * (ie., binder IPC connection).
     * <ul compact>
     * <li> Application will receive the message "serviceConnectionStatusId" (ie., msg.what)
     *      to the "callbackHander" with the connection status (Boolean object in the msg.obj).
     * <br>    - connection status TRUE indicates connection to service is ready.
     * <br>    - connection status FALSE indicates connection to service is broken.
     * </li>
     * <li> Application need to keep track of the connection status flag, and initiate the
     *      {@link initializeClient} and the other operations only if the connection status is TRUE
     * <li> If the connection status received as FALSE, then application need to assume that
     *      all the on-going operations are aborted and wait for the next connection status TRUE
     *      message and re initialize the clients and the operations.
     * </ul>
     *
     * @param callbackHandler Handler to which the result message
     *                        needs to be sent. The message will
     *                        contain a boolean value.
     *                        <ul compact>
     *                        <li>false - service is not connected
     *                        <li>true  - service is connected
     *                        </ul>
     *
     * @param serviceConnectionStatusId This interger id will be
     *                                  put in the "what" variable
     *                                  of the message that is sent
     *                                  to the handler. Can be used
     *                                  as an identifier.
     *
     * @return boolean Immediate result of the operation. true if
     *                 successful.
     *                 NOTE: This does not garuntee a successful
     *                 connection. The client needs to use handler
     *                 to listen to the Result.
     */
    public boolean connectService(Handler callbackHandler, int serviceConnectionStatusId){
        mServiceConnectionStatusHandler = callbackHandler;
        mServiceConnectionStatusId = serviceConnectionStatusId;

        Log.d(LOG_TAG, "Creating LteDirectDiscovery Service, if not started start");
        Intent intent = new Intent();
        intent.setComponent(DEFAULT_CONTAINER_COMPONENT);
        mConnection = new LteDirectDiscoveryServiceConnection();
        boolean sucess = mContext.bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
        Log.d(LOG_TAG, "binService result: " + sucess);

        return sucess;
    }

    /**
     * Disconnect the connection with the Service.
     *
     * @return none.
     */
    public void disconnectService() {
        Log.d(LOG_TAG, "release LteDirectDiscovery Service");
        if (mConnection != null) {
            mContext.unbindService(mConnection);
            mConnection = null;
        }
    }

    private class InternalListener extends ILteDirectDiscoveryStatusListener.Stub {
        private static final int TYPE_DEVICE_CAPABILITY_CHANGED = 1;
        private static final int TYPE_SERVICE_STATUS = 2;
        private static final int TYPE_TERMINATE_RESPONSE = 3;
        private static final int TYPE_GET_SERVICE_STATUS_RESPONSE = 4;
        private static final int TYPE_PUBLISH_RESPONSE = 5;
        private static final int TYPE_CANCEL_PUBLISH_RESPONSE = 6;
        private static final int TYPE_SUBSCRIBE_RESPONSE = 7;
        private static final int TYPE_CANCEL_SUBSCRIBE_RESPONSE = 8;
        private static final int TYPE_AUTHORIZATION_RESULT = 9;
        private static final int TYPE_EXPRESSION_STATUS = 10;
        private static final int TYPE_TRANSMISSION_STATUS = 11;
        private static final int TYPE_RECEPTION_STATUS = 12;
        private static final int TYPE_MATCH_EVENT = 13;

        private StatusListener mListener;
        private final Handler mHandler;

        InternalListener(StatusListener listener, Looper looper) {
            mListener = listener;
            if (looper == null) {
                mHandler = new Handler() {
                    @Override
                    public void handleMessage(Message msg) {
                        handleListenerMessage(msg);
                    }
                };
            } else {
                mHandler = new Handler(looper) {
                    @Override
                    public void handleMessage(Message msg) {
                        handleListenerMessage(msg);
                    }
                };
            }
        }

        // Override all APIs
        public void onDeviceCapabilityChanged(LteDirectDiscoveryCapability capability) {
            Capability devCap = null;
            if (capability != null) {
                devCap = new Capability();
                devCap.subCount = capability.subCount;
                devCap.capability = new int[devCap.subCount];
                for (int i = 0; i < devCap.capability.length; i++) {
                    devCap.capability[i] = capability.capability[i];
                }
            }

            if (devCap != null) {
                Message msg = Message.obtain();
                msg.what = TYPE_DEVICE_CAPABILITY_CHANGED;
                msg.obj = devCap;
                mHandler.sendMessage(msg);
            }
        }

        public void onServiceStatus(int publishAllowed, int subscribeAllowed) {
            Message msg = Message.obtain();
            msg.what = TYPE_SERVICE_STATUS;
            Bundle b = new Bundle();
            b.putInt("publishAllowed", publishAllowed);
            b.putInt("subscribeAllowed", subscribeAllowed);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onTerminateResponse(int txnId, int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_TERMINATE_RESPONSE;
            Bundle b = new Bundle();
            b.putInt("txnId", txnId);
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onGetServiceStatusResponse(int txnId, int result,
                int publishAllowed, int subscribeAllowed) {
            Message msg = Message.obtain();
            msg.what = TYPE_GET_SERVICE_STATUS_RESPONSE;
            Bundle b = new Bundle();
            b.putInt("txnId", txnId);
            b.putInt("result", result);
            b.putInt("publishAllowed", publishAllowed);
            b.putInt("subscribeAllowed", subscribeAllowed);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onPublishResponse(int txnId, int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_PUBLISH_RESPONSE;
            Bundle b = new Bundle();
            b.putInt("txnId", txnId);
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onCancelPublishResponse(int txnId, int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_CANCEL_PUBLISH_RESPONSE;
            Bundle b = new Bundle();
            b.putInt("txnId", txnId);
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onSubscribeResponse(int txnId, int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_SUBSCRIBE_RESPONSE;
            Bundle b = new Bundle();
            b.putInt("txnId", txnId);
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onCancelSubscribeResponse(int txnId, int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_CANCEL_SUBSCRIBE_RESPONSE;
            Bundle b = new Bundle();
            b.putInt("txnId", txnId);
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onAuthorizationResult(int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_AUTHORIZATION_RESULT;
            Bundle b = new Bundle();
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onExpressionStatus(String expression, int result) {
            Message msg = Message.obtain();
            msg.what = TYPE_EXPRESSION_STATUS;
            Bundle b = new Bundle();
            b.putString("expression", expression);
            b.putInt("result", result);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onTransmissionStatus(String expression, int status) {
            Message msg = Message.obtain();
            msg.what = TYPE_TRANSMISSION_STATUS;
            Bundle b = new Bundle();
            b.putString("expression", expression);
            b.putInt("status", status);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onReceptionStatus(String expression, int status) {
            Message msg = Message.obtain();
            msg.what = TYPE_RECEPTION_STATUS;
            Bundle b = new Bundle();
            b.putString("expression", expression);
            b.putInt("status", status);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        public void onMatchEvent(String expression, String matchedExpression,
                int state, int metaDataIndex, String metaData) {
            Message msg = Message.obtain();
            msg.what = TYPE_MATCH_EVENT;
            Bundle b = new Bundle();
            b.putString("expression", expression);
            b.putString("matchedExpression", matchedExpression);
            b.putInt("state", state);
            b.putInt("metaDataIndex", metaDataIndex);
            b.putString("metaData", metaData);
            msg.obj = b;
            mHandler.sendMessage(msg);
        }

        private void handleListenerMessage(Message msg) {
            int txnId, result, status;
            String expression;
            int publishAllowed, subscribeAllowed;
            Bundle b;

            if (mListener == null) {
                return;
            }
            switch (msg.what) {
                case TYPE_DEVICE_CAPABILITY_CHANGED:
                    Capability devCap = (Capability) msg.obj;
                    mListener.onDeviceCapabilityChanged(devCap);
                    break;
                case TYPE_SERVICE_STATUS:
                    b = (Bundle) msg.obj;
                    publishAllowed = b.getInt("publishAllowed");
                    subscribeAllowed = b.getInt("subscribeAllowed");
                    mListener.onServiceStatus(publishAllowed, subscribeAllowed);
                    break;
                case TYPE_TERMINATE_RESPONSE:
                    b = (Bundle) msg.obj;
                    txnId = b.getInt("txnId");
                    result = b.getInt("result");
                    mListener.onTerminateResponse(txnId, result);
                    break;
                case TYPE_GET_SERVICE_STATUS_RESPONSE:
                    b = (Bundle) msg.obj;
                    txnId = b.getInt("txnId");
                    result = b.getInt("result");
                    publishAllowed = b.getInt("publishAllowed");
                    subscribeAllowed = b.getInt("subscribeAllowed");
                    mListener.onGetServiceStatusResponse(txnId, result,
                            publishAllowed, subscribeAllowed);
                    break;
                case TYPE_PUBLISH_RESPONSE:
                    b = (Bundle) msg.obj;
                    txnId = b.getInt("txnId");
                    result = b.getInt("result");
                    mListener.onPublishResponse(txnId, result);
                    break;
                case TYPE_CANCEL_PUBLISH_RESPONSE:
                    b = (Bundle) msg.obj;
                    txnId = b.getInt("txnId");
                    result = b.getInt("result");
                    mListener.onCancelPublishResponse(txnId, result);
                    break;
                case TYPE_SUBSCRIBE_RESPONSE:
                    b = (Bundle) msg.obj;
                    txnId = b.getInt("txnId");
                    result = b.getInt("result");
                    mListener.onSubscribeResponse(txnId, result);
                    break;
                case TYPE_CANCEL_SUBSCRIBE_RESPONSE:
                    b = (Bundle) msg.obj;
                    txnId = b.getInt("txnId");
                    result = b.getInt("result");
                    mListener.onCancelSubscribeResponse(txnId, result);
                    break;
                case TYPE_AUTHORIZATION_RESULT:
                    b = (Bundle) msg.obj;
                    result = b.getInt("result");
                    mListener.onAuthorizationResult(result);
                    break;
                case TYPE_EXPRESSION_STATUS:
                    b = (Bundle) msg.obj;
                    expression = b.getString("expression");
                    result = b.getInt("result");
                    mListener.onExpressionStatus(expression, result);
                    break;
                case TYPE_TRANSMISSION_STATUS:
                    b = (Bundle) msg.obj;
                    expression = b.getString("expression");
                    status = b.getInt("status");
                    mListener.onTransmissionStatus(expression, status);
                    break;
                case TYPE_RECEPTION_STATUS:
                    b = (Bundle) msg.obj;
                    expression = b.getString("expression");
                    status = b.getInt("status");
                    mListener.onReceptionStatus(expression, status);
                    break;
                case TYPE_MATCH_EVENT:
                    b = (Bundle) msg.obj;
                    expression = b.getString("expression");
                    String matchedExpression = b.getString("matchedExpression");
                    int state = b.getInt("state");
                    int metaDataIndex = b.getInt("metaDataIndex");
                    String metaData = b.getString("metaData");
                    mListener.onMatchEvent(expression, matchedExpression,
                            state, metaDataIndex, metaData);
                    break;

                default:
                    break;
            }
        }
    }

    /**
     * Initiate a client and registers a listener callback object.
     * Applications need to call this function and get the client
     * handle to participate in the Lte Direct Discovery operations.
     *
     * @param osAppId       Os App Id.
     * @param listener      Listener callback object.
     *                      All the response callback for the ClientHandle
     *                      requests and unsol indications are notified
     *                      through this listener object.
     * @param looper        A Looper object whose message queue will be used to
     *                      implement the callback mechanism, or null to make
     *                      callbacks on the calling thread
     *
     * @return ClientHandle Valid client handle on success
     *                      null on failure.
     *
     * @see                 StatusListener
     */
    public ClientHandle initializeClient(String osAppId, StatusListener listener, Looper looper) {
        InternalListener intListener = new InternalListener(listener, looper);
        ClientHandle handle = null;
        ILteDirectDiscovery client = null;

        Log.d(LOG_TAG, "initializeClient: osAppId = " + osAppId);

        if (mService != null) {
            try {
                client = mService.initializeClient(osAppId, intListener);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception " + e);
            }
        }
        if (client != null) {
            handle = new ClientHandle(client);
        } else {
            Log.e(LOG_TAG, "recevied NULL");
        }
        return handle;
    }

    /**
     * Initiate a client and registers a listener callback object.
     * Applications need to call this function and get the client
     * handle to participate in the Lte Direct Discovery operations.
     *
     * @param osAppId       Os App Id.
     * @param listener      Listener callback object.
     *                      All the response callback for the ClientHandle
     *                      requests and unsol indications are notified
     *                      through this listener object.
     *
     * @return ClientHandle Valid client handle on success
     *                      null on failure.
     *
     * @see                 StatusListener
     */
    public ClientHandle initializeClient(String osAppId, StatusListener listener) {
        return initializeClient(osAppId, listener, null);
    }

    /**
     * Release the client.
     * Applications need to call this function once they finished
     * with the operations on the specified osAppId.
     *
     * @param osAppId       Os App Id.
     *
     * @return boolean      true on success
     */
    public boolean releaseClient(String osAppId) {
        boolean ret = false;
        Log.d(LOG_TAG, "releaseClient: osAppId = " + osAppId);
        if (mService != null) {
            try {
                ret = mService.releaseClient(osAppId);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception " + e);
            }
        }
        return ret;
    }

    /**
     * Listener type for receiving the asynchronous responses and unsolicited indications.
     */
    public interface StatusListener {
        /**
         * Notifies the device LTE-D capability of all SUBs.
         *
         * @param capability LTE-D capability of SUB1 & SUB2
         * @return           none
         *
         * @see              ClientHandle#getDeviceCapability
         */
        public void onDeviceCapabilityChanged (Capability capability);

        /**
         * Notifies the LTE-D service status
         *
         * @param publishAllowed    Publish status
         *                          FALSE(0) = Currently UE is out of announcing coverage
         *                          TRUE(1)  = Currently UE is in announcing coverage
         * @param subscribeAllowed  Subscribe status
         *                          FALSE(0) = Currently UE is out of monitoring coverage
         *                          TRUE(1)  = Currently UE is in monitoring coverage
         * @return                  none
         *
         * @see                     ClientHandle#initializeClient
         */
        public void onServiceStatus (int publishAllowed, int subscribeAllowed);

        /**
         * Notifies the response status of the
         * {@link ClientHandle#terminate}
         *
         * @param txnId      Transaction id
         * @param result     Result status as defined in LteDirectDiscovery#OperationResult
         * @return           none
         *
         * @see              ClientHandle#terminate
         */
        public void onTerminateResponse (int txnId, int result);

        /**
         * Notifies the response status of
         * {@link ClientHandle#getServiceStatus}
         *
         * @param txnId      Transaction id
         * @param result     Result status as defined in LteDirectDiscovery#OperationResult
         * @param publishAllowed    Publish status
         *                          FALSE(0) = Currently UE is out of announcing coverage
         *                          TRUE(1)  = Currently UE is in announcing coverage
         * @param subscribeAllowed  Subscribe status
         *                          FALSE(0) = Currently UE is out of monitoring coverage
         *                          TRUE(1)  = Currently UE is in monitoring coverage
         * @return           none
         *
         * @see              ClientHandle#getServiceStatus
         */
        public void onGetServiceStatusResponse (int txnId, int result,
                int publishAllowed, int subscribeAllowed);

        /**
         * Notifies the response status of the
         * {@link ClientHandle#publish}
         * The result status of each expression is notified
         * through the
         * {@link StatusListener#onExpressionStatus}
         *
         * @param txnId      Transaction id
         * @param result     Result status as defined in LteDirectDiscovery#OperationResult
         * @return           none
         *
         * @see              ClientHandle#publish
         */
        public void onPublishResponse (int txnId, int result);

        /**
         * Notifies the response status of the
         * {@link ClientHandle#cancelPublish}
         *
         * @param txnId      Transaction id
         * @param result     Result status as defined in LteDirectDiscovery#OperationResult
         * @return           none
         *
         * @see              ClientHandle#cancelPublish
         */
        public void onCancelPublishResponse (int txnId, int result);

        /**
         * Notifies the response status of the
         * {@link ClientHandle#subscribe}
         * The result status of each expression is notified
         * through the
         * {@link StatusListener#onExpressionStatus}
         *
         * @param txnId      Transaction id
         * @param result     Result status as defined in LteDirectDiscovery#OperationResult
         * @return           none
         *
         * @see              ClientHandle#subscribe
         */
        public void onSubscribeResponse (int txnId, int result);

        /**
         * Notifies the response status of the
         * {@link ClientHandle#cancelSubscribe}
         *
         * @param txnId      Transaction id
         * @param result     Result status as defined in LteDirectDiscovery#OperationResult
         * @return           none
         *
         * @see              ClientHandle#cancelSubscribe
         */
        public void onCancelSubscribeResponse (int txnId, int result);

        /**
         * Notifies App authorization failure from LTE-D server.
         *
         * @param result     Authorization error code as defined in
         *                   LteDirectDiscovery#ResultCode
         * @return           none
         */
        public void onAuthorizationResult (int result);

        /**
         * Notifies the status of the Expressions
         *
         * @param expression Expression (PA ID)
         * @param result     Expression Result status as defined in
         *                   LteDirectDiscovery#ResultCode
         * @return           none
         *
         * @see              ClientHandle#publish
         * @see              ClientHandle#subscribe
         */
        public void onExpressionStatus (String expression, int result);

        /**
         * Transmission Status
         *
         * @param expression Expression (PA-ID)
         * @param status     Transmission status
         *                    0 = completed
         *                    1 = pendingTransmission
         * @return           none
         *
         * @see              ClientHandle#publish
         */
        public void onTransmissionStatus (String expression, int status);

        /**
         * Reception Status
         *
         * @param expression Expression (PA-ID)
         * @param status     Reception status
         *                    0 = completed
         *                    1 = pendingSubscription
         * @return           none
         *
         * @see              ClientHandle#subscribe
         */
        public void onReceptionStatus (String expression, int status);

        /**
         * Notifies the Match Event
         *
         * @param expression Subscribed Expression (PA-ID)
         * @param matchedExpression Matched Expression (PA-ID)
         * @param state      Match start/end
         *                    0 = Match Start
         *                    1 = Match End
         * @param metaDataIndex   Index value to look for changes in MetaData field
         * @param metaData   Meta Data associated with the Expression
         *                   (e.g. postal address, phone number, URL etc)
         * @return           none
         *
         * @see              ClientHandle#subscribe
         */
        public void onMatchEvent (String expression, String matchedExpression,
                int state, int metaDataIndex, String metaData);
    }

    /**
     * Lte Direct Discovery Capability of each phone subscriptions.
     */
    public static class Capability {
        public static final int SUB1 = 0;
        public static final int SUB2 = 1;
        public static final int SUB3 = 2;

        public static final int CAPABILITY_DISABLED = 0;
        public static final int CAPABILITY_ENABLED  = 1;

        public int subCount;
        public int capability[];
    }

    /**
     * Client handle.  Applications can invoke the APIs defined in this class to participate
     * in the Lte Direct Discovery operations.
     */
    public static class ClientHandle {
        private static final String LOG_TAG = "ClientHandle";

        private ILteDirectDiscovery mHandle;

        ClientHandle(ILteDirectDiscovery handle) {
            mHandle = handle;
        }

        /**
         * Get device LTE-D capability of each subscriptions
         *
         * @param        None
         * @return       LTE-D capability of SUB1 & SUB2
         *
         * @see          StatusListener#onDeviceCapabilityChanged
         */
        public Capability getDeviceCapability() {
            LteDirectDiscoveryCapability ret = null;
            Capability devCap = null;
            Log.d(LOG_TAG, "getDeviceCapability");
            if (mHandle != null) {
                try {
                    ret = mHandle.getDeviceCapability();
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            if (ret != null) {
                devCap = new Capability();
                devCap.subCount = ret.subCount;
                devCap.capability = new int[devCap.subCount];
                for (int i = 0; i < devCap.capability.length; i++) {
                    devCap.capability[i] = ret.capability[i];
                }
            }
            return devCap;
        }

        /**
         * Set Phone Subscription ID
         *
         * @param subId    Subscription ID
         * @return         Success/Failure.
         *
         * @see            StatusListener
         */
        public boolean setPhoneSubscriptionId(int subId) {
            boolean ret = false;
            Log.d(LOG_TAG, "setPhoneSubscriptionId: subId = " + subId);
            if (mHandle != null) {
                try {
                    ret = mHandle.setPhoneSubscriptionId(subId);
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }

        /**
         * Stop all ongoing operations
         *
         * @param txnId       Transaction id
         * @return            Success/Failure indicating if the request is placed successfully,
         *                    The actual response will be notified through the listener callback
         *                    {@link StatusListener#onTerminateResponse}
         *                    registered in initializeClient.
         *
         * @see               StatusListener#onTerminateResponse
         */
        public boolean terminate(int txnId) {
            boolean ret = false;
            Log.d(LOG_TAG, "terminate: txnId = " + txnId);
            if (mHandle != null) {
                try {
                    mHandle.terminate(txnId);
                    ret = true;
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }

        /**
         * Get publish and subscribe service status
         *
         * @param txnId       Transaction id
         * @return            Success/Failure indicating if the request is placed successfully,
         *                    The actual response will be notified through the listener callback
         *                    {@link StatusListener#onGetServiceStatusResponse}
         *                    registered in initializeClient.
         *
         * @see               StatusListener#onGetServiceStatusResponse
         */
        public boolean getServiceStatus(int txnId) {
            boolean ret = false;
            Log.d(LOG_TAG, "getServiceStatus: txnId = " + txnId);
            if (mHandle != null) {
                try {
                    mHandle.getServiceStatus(txnId);
                    ret = true;
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }

        /**
         * Publish an expression.
         *
         * @param txnId       Transaction id
         * @param expression  Expression or ProSe Application ID (PA ID) to be published
         *                    ProSe Application ID is composed of the following parts:
         *                    i. The ProSe Application ID Name is described in its
         *                       entirety by a data structure characterized by different
         *                       levels e.g., broad-level business category (Level 0) /
         *                       business sub-category (Level 1) / business name (Level 2) /
         *                       shop ID (Level 3). For the purpose of presentation, a
         *                       ProSe Application ID Name is usually displayed as a string
         *                       of labels in which the labels represent hierarchical levels.
         *                   ii. The PLMN ID that corresponds to the PLMN that assigned the
         *                       ProSe Application ID Name.
         *
         *                   Example: "mcc345.mnc012.ProSeApp.Food.Restaurants.Italian"
         *
         * @param expressionValidityTime Validity time of the expression
         *                    Units: minutes
         *                    Minimum value: 1
         *                    Maximum value: 525600 (1 year)
         * @param metaData    Meta data to be transmitted along with the expression
         * @param discoveryType Restricted/Open (Currently supports only Open)
         * @param duration    Duration for which to announce the code
         *                    Units: minutes
         *                    Minimum value: 1 min
         *                    Maximum value: 30 min
         * @return            Success/Failure indicating if the request is placed successfully,
         *                    The actual response will be notified through the listener callback
         *                    {@link StatusListener#onPublishResponse}
         *                    registered in initializeClient.
         *
         * @see               StatusListener#onPublishResponse
         * @see               StatusListener#onExpressionStatus
         * @see               StatusListener#onTransmissionStatus
         */
        public boolean publish(int txnId, String expression, int expressionValidityTime,
                String metaData, int discoveryType, int duration) {
            boolean ret = false;
            Log.d(LOG_TAG, "publish: txnId = " + txnId);
            if (mHandle != null) {
                try {
                    mHandle.publish(txnId, expression, expressionValidityTime, metaData, discoveryType, duration);
                    ret = true;
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }

        /**
         * Cancel the publication of an expression
         *
         * @param txnId       Transaction id
         * @param expression  Expression (PA-ID)
         * @return            Success/Failure indicating if the request is placed successfully,
         *                    The actual response will be notified through the listener callback
         *                    {@link StatusListener#onCancelPublishResponse}
         *                    registered in initializeClient.
         *
         * @see               StatusListener#onCancelPublishResponse
         */
        public boolean cancelPublish(int txnId, String expression) {
            boolean ret = false;
            Log.d(LOG_TAG, "cancelPublish: txnId = " + txnId);
            if (mHandle != null) {
                try {
                    mHandle.cancelPublish(txnId, expression);
                    ret = true;
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }

        /**
         * Subscribe an expression.
         *
         * @param txnId       Transaction id
         * @param expression  Expression or ProSe Application ID (PA ID) to be subscribed
         *                    ProSe Application ID is composed of the following parts:
         *                    i. The ProSe Application ID Name is described in its
         *                       entirety by a data structure characterized by different
         *                       levels e.g., broad-level business category (Level 0) /
         *                       business sub-category (Level 1) / business name (Level 2) /
         *                       shop ID (Level 3). For the purpose of presentation, a
         *                       ProSe Application ID Name is usually displayed as a string
         *                       of labels in which the labels represent hierarchical levels.
         *                   ii. The PLMN ID that corresponds to the PLMN that assigned the
         *                       ProSe Application ID Name.
         *
         *                   Example: "mcc345.mnc012.ProSeApp.Food.Restaurants.Italian"
         *
         * @param expressionValidityTime Validity time of the expression
         *                    Units: minutes
         *                    Minimum value: 1
         *                    Maximum value: 525600 (1 year)
         * @param discoveryType Restricted/Open (Currently supports only Open)
         * @param duration    Duration for which to subscribe the code
         *                    Units: minutes
         *                    Minimum value: 1 min
         *                    Maximum value: 30 min
         * @return            Success/Failure indicating if the request is placed successfully,
         *                    The actual response will be notified through the listener callback
         *                    {@link StatusListener#onSubscribeResponse}
         *                    registered in initializeClient.
         *
         * @see               StatusListener#onSubscribeResponse
         * @see               StatusListener#onExpressionStatus
         * @see               StatusListener#onReceptionStatus
         * @see               StatusListener#onMatchEvent
         */
        public boolean subscribe(int txnId, String expression, int expressionValidityTime,
                int discoveryType, int duration) {
            boolean ret = false;
            Log.d(LOG_TAG, "subscribe: txnId = " + txnId);
            if (mHandle != null) {
                try {
                    mHandle.subscribe(txnId, expression, expressionValidityTime, discoveryType, duration);
                    ret = true;
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }

        /**
         * Cancel the subscription.
         *
         * @param txnId       Transaction id
         * @param expression  Expression (PA-ID)
         * @return            Success/Failure indicating if the request is placed successfully,
         *                    The actual response will be notified through the listener callback
         *                    {@link StatusListener#onCancelSubscribeResponse}
         *                    registered in initializeClient.
         *
         * @see               StatusListener#onCancelSubscribeResponse
         */
        public boolean cancelSubscribe(int txnId, String expression) {
            boolean ret = false;
            Log.d(LOG_TAG, "cancelSubscribe: txnId = " + txnId);
            if (mHandle != null) {
                try {
                    mHandle.cancelSubscribe(txnId, expression);
                    ret = true;
                } catch (RemoteException e) {
                    Log.d(LOG_TAG, "Exception " + e);
                }
            }
            return ret;
        }
    }
}

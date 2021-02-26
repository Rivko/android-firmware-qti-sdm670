/******************************************************************************
  @file    RemoteSimlockManager.java
  @brief   Manager class which provides interface to communicate with remote
           simlock service.

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.remoteSimlock.manager;

import android.util.Log;
import android.os.Message;
import android.content.Context;
import android.os.IBinder;
import android.os.Handler;
import android.os.RemoteException;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;

import com.qualcomm.qti.remoteSimlock.IUimRemoteSimlockService;

/**
 * This class provides UimRemoteSimlockService Interface
 * for the clients.
 *
 * Client needs to instantiate this class to
 * use the IUimRemoteSimlockService.aidl functionality
 */

public class RemoteSimlockManager {
    private final String LOG_TAG = "RemoteSimlockManager";
    public static final String VERSION = "1.1";

    private Context context;


    /* used internally to check the status of serviceConnection */
    private Boolean serviceConnected;

    private SimlockManagerServiceConnection mConnection;
    private IUimRemoteSimlockService simlockService = null;

    //List of handlers to store with ids
    private Handler serviceConnectionStatusHandler = null;
    private int serviceConnectionStatusId;

    /**
     * Constructor
     * @param context context in which the bindService will be
     *                initiated.
     */
    public RemoteSimlockManager(Context context) {
        this.context = context;
        serviceConnected = false;
    }

    /**
     * To check if the service is connected or not
     * @return boolean true if service is connected, false oterwise
     */
    public boolean isServiceConnected() {
        return serviceConnected;
    }


    /**
     * Initiate connection with the service.
     *
     * @param callbackHandler Handler to which the result message
     *                        needs to be sent. The message will
     *                        contain a integer value.
     *                        0 - service is not connected
     *                        1 - service is connected
     *
     * @param serviceConnectionStatusId This integer id will be
     *                                  put in the "what" variable
     *                                  of the message that is sent
     *                                  to the handler. Can be used
     *                                  as an identifier.
     *                                  Boolean object will be put
     *                                  in "what.obj" with true as
     *                                  service connected and false
     *                                  as service disconnected.
     *
     * @return boolean Immediate result of the operation. true if
     *                 successful.
     *                 NOTE: This does not guarantee a successful
     *                 connection. The client needs to use handler
     *                 to listen to the Result.
     *
     */
    public boolean connectService(Handler callbackHandler, int serviceConnectionStatusId) {
        serviceConnectionStatusHandler = callbackHandler;
        this.serviceConnectionStatusId = serviceConnectionStatusId;

        Log.e(LOG_TAG, "Creating Simlock Service, if not started start");
        Intent intent = new Intent();
        intent.setComponent(new ComponentName("com.qualcomm.qti.uim",
                                              "com.qualcomm.qti.uim.RemoteSimLockService"));
        mConnection = new SimlockManagerServiceConnection();
        boolean success = context.bindService(intent, mConnection,
                                              Context.BIND_AUTO_CREATE);
        Log.e(LOG_TAG, "bind Service result: " + success);
        return success;
    }

    /**
     * Disconnect the connection with the Service.
     *
     */
    public void disconnectService() {
        Log.i(LOG_TAG, "release Simlock Service");
        context.unbindService(mConnection);
        mConnection = null;
    }

    /**
     * registerCallback will be used by simlock client
     * to register a callback to be notified asynchronously
     *
     * @param callback
     *    Defines the callback interface
     *
     * @return
     *    SUCCESS 0
     *       Successfully registered call back with simlock service.
     *    INTERNAL_FAILURE 8
     *       Failed to register callback due to internal error.
     */
    public int registerCallback(RemoteSimlockManagerCallback callback) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "registerCallback");

        try {
            ret = simlockService.registerCallback(callback);
        } catch(RemoteException e) {
            Log.e(LOG_TAG, "registerCallback, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }
        return ret;
    }

    /**
     * deregisterCallback will be used by
     * simlock client to deregister callback
     *
     * @param callback
     *    Defines the callback interface
     *
     * @return
     *    UIM_REMOTE_SIMLOCK_SUCCESS 0
     *       Successfully deregistered callback with SimLock service.

     *    UIM_REMOTE_SIMLOCK_ERROR 1
     *       Failed to deregister call back due to internal error.
     */
    public int deregisterCallback(RemoteSimlockManagerCallback callback) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "deregisterCallback");

        try {
            ret = simlockService.deregisterCallback(callback);
        } catch(RemoteException e){

            Log.e(LOG_TAG, "deregisterCallback, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }

        return ret;
    }

    /**
     * uimRemoteSimlockProcessSimlockData Send lock/unlock blob to SIMLock engine
     *
     * @param token
     *
     * @param simlockData
     *    The byte array representing simlock blob for lock and unlock.
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockProcessSimlockData(int token, byte[] simlockData) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "getCardState");

        try {
            ret = simlockService.uimRemoteSimlockProcessSimlockData(token, simlockData);

            if (ret == 0) {
                Log.i(LOG_TAG, "uimRemoteSimlockProcessSimlockData return " + ret);
            }

        } catch(RemoteException e){

            Log.e(LOG_TAG, "uimRemoteSimlockProcessSimlockData, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }

        return ret;
    }

    /**
     * uimRemoteSimlockGetSharedKey to get encrypted symmetric key from SIMLock engine
     *
     * @param token
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockGetSharedKey(int token) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "uimRemoteSimlockGetSharedKey");

        try {
            ret = simlockService.uimRemoteSimlockGetSharedKey(token);

            if (ret == 0)
            {
                Log.i(LOG_TAG, "uimRemoteSimlockGetSharedKey return " + ret);
            }
        } catch(RemoteException e){

            Log.e(LOG_TAG, "uimRemoteSimlockGetSharedKey, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }

        return ret;
    }

    /**
     * uimRemoteSimlockGenerateHMAC request to SIMLock engine to generate and return
     * HMAC on the input data
     *
     * @param token
     *
     * @parma data
     *    Input data for which HMAC needs to be generated
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockGenerateHMAC(int token, byte[] data) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "uimRemoteSimlockGenerateHMAC");

        try {
            ret = simlockService.uimRemoteSimlockGenerateHMAC(token, data);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "uimRemoteSimlockGenerateHMAC, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }

        return ret;
    }

    /**
     * uimRemoteSimlockGetVersion to get highest supported major/minor version of
     * incoming blob that is supported by SIMLock engine
     *
     * @param token
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockGetVersion(int token) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "uimRemoteSimlockGetVersion");

        try {
            ret = simlockService.uimRemoteSimlockGetVersion(token);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "uimRemoteSimlockGetVersion, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }
        return ret;
    }

    /**
     * uimRemoteSimlockGetSimlockStatus to get current SIMLock status
     *
     * @param token
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockGetSimlockStatus(int token)
    {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "uimRemoteSimlockGetSimlockStatus");

        try {
            ret = simlockService.uimRemoteSimlockGetSimlockStatus(token);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "uimRemoteSimlockGetSimlockStatus, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }
        return ret;
    }

    /**
     * uimRemoteSimlockGenerateRequest request to Simlock engine to generate
     * a request. The parameter requestType indicates the type of
     * request to generate. Only blob request is supported at the
     * present time.
     *
     * @param token
     *
     * @param requestType
     *    UIM_REMOTE_GENERATE_BLOB_REQUEST_TYPE   = 1;
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockGenerateRequest(int token, int requestType) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "uimRemoteSimlockGenerateRequest");

       try {
            ret = simlockService.uimRemoteSimlockGenerateRequest(token, requestType);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "uimRemoteSimlockGenerateRequest, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }
        return ret;
    }

    /**
     * uimRemoteSimlockUnlockTimer request to Simlock engine to start and stop
     * unlock device timer to unlock the device for the specific time to
     * download the blob. The Simlock engine unlocks the device only once per
     * UICC power cycle.
     *
     * @param token
     *
     * @param timerState
     *    Input data to start and stop the timer, 1 - start, 0 - stop
     *
     * @return immediate return error.
     *    UIM_REMOTE_SIMLOCK_SUCCESS       = 0;
     *    UIM_REMOTE_SIMLOCK_ERROR         = 1;
     */
    public int uimRemoteSimlockUnlockTimer(int token, int timerState) {
        int ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SimlockStatus.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "uimRemoteSimlockUnlockTimer");

        try {
            ret = simlockService.uimRemoteSimlockUnlockTimer(token, timerState);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "uimRemoteSimlockUnlockTimer, remote exception");
            e.printStackTrace();
            ret = SimlockStatus.UIM_REMOTE_SIMLOCK_ERROR;
        }
        return ret;
    }

    /*******************************************************************************
     *
     *                     INTERNAL HELPER FUNCTIONS/VARIABLES                           *
     ******************************************************************************/
    private class SimlockManagerServiceConnection implements ServiceConnection {

        public void onServiceConnected(ComponentName name, IBinder boundService) {

            Log.e(LOG_TAG, "On service connected, get the binder object");
            simlockService = IUimRemoteSimlockService.Stub.asInterface((IBinder) boundService);
            Log.e(LOG_TAG, "simlockService obtained from the binder!!!!");
            serviceConnected = true;
            if(serviceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            }else {
                Message msg = serviceConnectionStatusHandler.obtainMessage(
                                 serviceConnectionStatusId, serviceConnected);
                msg.sendToTarget();
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.i(LOG_TAG, "onServiceDisconnected, release the binder object");
            simlockService = null;
            serviceConnected = false;
            if(serviceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            } else {
                Message msg = serviceConnectionStatusHandler.obtainMessage(serviceConnectionStatusId, serviceConnected);
                msg.sendToTarget();
            }
        }
    }

    public class SimlockStatus {
        public static final int UIM_REMOTE_SIMLOCK_SUCCESS = 0;
        public static final int UIM_REMOTE_SIMLOCK_ERROR   = 1;
        public static final int SERVICE_NOT_CONNECTED      = 2;
    }
}

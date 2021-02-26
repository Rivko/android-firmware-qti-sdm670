/******************************************************************************
  @file    SAMManager.java
  @brief   Manager class which provides interface to communicate with SAM
           service.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.sam.manager;

import android.util.Log;
import android.os.Message;
import android.content.Context;
import android.os.IBinder;
import android.os.Handler;
import android.os.RemoteException;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;

import com.qualcomm.qti.sam.interfaces.CardState;
import com.qualcomm.qti.sam.interfaces.CardATR;
import com.qualcomm.qti.sam.interfaces.SlotMap;
import com.qualcomm.qti.sam.interfaces.ISAM;
import com.qualcomm.qti.sam.interfaces.ISAMResponseCallback;
import com.qualcomm.qti.sam.interfaces.ISAMUnsolicitedCallback;
import com.qualcomm.qti.sam.interfaces.SAMErrors;

/**
 * This class provides SAM Interface
 * for the clients.
 *
 * Client needs to instantiate this class to
 * use the ISAM.aidl functionality
 */

public class SAMManager {
    private final String LOG_TAG = "SAMManager";

    private Context context;


    /* used internally to check the status of serviceConnection */
    private Boolean serviceConnected;

    private SAMManagerServiceConnection mConnection;
    private ISAM samService = null;

    //List of handlers to store with ids
    private Handler serviceConnectionStatusHandler = null;
    private int serviceConnectionStatusId;

    /**
     * Constructor
     * @param context context in which the bindService will be
     *                initiated.
     */
    public SAMManager(Context context){
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
     *
     */
    public boolean connectService(Handler callbackHandler, int serviceConnectionStatusId){
        serviceConnectionStatusHandler = callbackHandler;
        this.serviceConnectionStatusId = serviceConnectionStatusId;

        Log.e(LOG_TAG, "Creating SAM Service, if not started start");
        Intent intent = new Intent();
        intent.setComponent(new ComponentName("com.qualcomm.qti.sam.service",
                                              "com.qualcomm.qti.sam.service.SAMService"));
        mConnection = new SAMManagerServiceConnection();
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
        Log.i(LOG_TAG, "release SAM Service");
        context.unbindService(mConnection);
        mConnection = null;
    }

    /**
     * registerUnsolicitedResponseCallback will be used by SAM client
     * to register a callback to be notified asynchronously
     *
     * @param callback
     *    Defines the callback interface
     *
     * @return
     *    SUCCESS 0
     *       Successfully registerd call back with SAM service.
     *    INTERNAL_FAILURE 8
     *       Failed to register call bac due to internal error.
     */
    public int registerUnsolicitedResponseCallback(SAMUnsolicitedCallback callback) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "registerCallback");

        try {
            ret = samService.registerUnsolicitedResponseCallback(callback);
        } catch(RemoteException e){

            Log.e(LOG_TAG, "registerCallback, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
     * deregisterUnsolicitedResponseCallback will be used by
     * SAM client to deregister callback
     *
     * @param callback
     *    Defines the callback interface
     *
     * @return
     *    SUCCESS 0
     *       Successfully deregisterd callback with SAM service.

     *    INTERNAL_FAILURE 8
     *       Failed to deregister call bac due to internal error.
     */
    public int deregisterUnsolicitedResponseCallback(SAMUnsolicitedCallback callback) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "deregisterCallback");

        try {
            ret = samService.deregisterUnsolicitedResponseCallback(callback);
        } catch(RemoteException e){

            Log.e(LOG_TAG, "deregisterCallback, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Get Card State.
    * @param - slotId - SAM card slot id
    *        - state - (output variabe) SAM card state
    * @return status of query on SAM card state
    */
    public int getCardState(int slotId, CardState cardState) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "getCardState");

        try {
            ret = samService.getCardState(slotId, cardState);

            if (ret == 0) {
                Log.i(LOG_TAG, "getCardState return " + ret + cardState.getCardState());
            }

        } catch(RemoteException e){

            Log.e(LOG_TAG, "getCardState, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Get Card ATR.
    * @param - slotId - SAM card slot id
    *        - cardATR - (output variabe) SAM card ATR
    * @return status of query on SAM card ATR
    */
    public int getCardATR(int slotId, CardATR cardATR) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "getCardATR");

        try {
            ret = samService.getCardATR(slotId, cardATR);

            if (ret == 0)
            {
                Log.i(LOG_TAG, "getCardATR return " + cardATR.getSlotId() + cardATR.getCardATR());
            }
        } catch(RemoteException e){

            Log.e(LOG_TAG, "getCardATR, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Send APDU command.
    * @param - slotId - SAM card slot id
    *       Following fields are used to derive the APDU ("command" and "length"
    *          values in TS 27.007 +CSIM and +CGLA commands).
    *        - class_type - class type
    *        - instruction - instruction
    *        - param1 - parameter 1
    *        - param2 - parameter 2
    *        - param3 - parameter 3
    *        - dataCmd - command
    *        - callback - callback to be called asynchronously.
    * @return intermediate status
    */
    public int streamApdu(int slotId, int class_type, int instruction, int param1, int param2, int param3, String dataCmd, SAMResponseCallback callback) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "streamApdu");

        try {
            ret = samService.streamApdu(slotId, class_type, instruction, param1, param2, param3, dataCmd, callback);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "streamApdu, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Switch Slots.
    * @param - slotMap - slot mapping information
    *        - callback - callback to be called asynchronously.
    * @return intermediate status
    */
    public int switchSlot(SlotMap slotMap, SAMResponseCallback callback) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "switchSlot");

        try {
            ret = samService.switchSlot(slotMap, callback);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "switchSlot, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Get slot mapping.
    * @param - slotMap - slot mapping information
    * @return status of the request
    */
    public int getSlotMapping(SlotMap slotMap)
    {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "getSlotMapping");

        try {
            ret = samService.getSlotMapping(slotMap);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "getSlotMapping, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Card power up request.
    * @param - slotId - SAM card slot id
    *        - callback - callback to be called asynchronously.
    *
    * @return intermediate status
    */
    public int cardPowerUp(int slotId, SAMResponseCallback callback) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "cardPowerUp");

        try {
            ret = samService.cardPowerUp(slotId, callback);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "cardPowerUp, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
    * Card power down request.
    * @param - slotId - SAM card slot id
    *        - callback - callback to be called asynchronously.
    *
    * @return intermediate status
    */
    public int cardPowerDown(int slotId, SAMResponseCallback callback) {
        int ret;

        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return SAMErrors.SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "cardPowerDown");

        try {
            ret = samService.cardPowerDown(slotId, callback);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "cardPowerDown, remote exception");
            e.printStackTrace();
            ret = SAMErrors.INTERNAL_FAILURE;
        }

        return ret;
    }

    /*******************************************************************************
     *
     *                     INTERNAL HELPER FUNCTIONS/VARIABLES                           *
     ******************************************************************************/
    private class SAMManagerServiceConnection implements ServiceConnection {

        public void onServiceConnected(ComponentName name, IBinder boundService) {

            Log.e(LOG_TAG, "Onservice connected, get the binder object");
            samService = ISAM.Stub.asInterface((IBinder) boundService);
            Log.e(LOG_TAG, "samService obtained from the binber!!!!");
            serviceConnected = true;
            if(serviceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            }else {
                Message msg = serviceConnectionStatusHandler.obtainMessage(serviceConnectionStatusId, serviceConnected);
                msg.sendToTarget();
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.i(LOG_TAG, "onServiceDisconnected, release the binder object");
            samService = null;
            serviceConnected = false;
            if(serviceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            } else {
                Message msg = serviceConnectionStatusHandler.obtainMessage(serviceConnectionStatusId, serviceConnected);
                msg.sendToTarget();
            }
        }
    }
}

/*
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.gsma.services.nfc;

import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.nfc.cardemulation.CardEmulation;
import android.nfc.cardemulation.AidGroup;
import com.gsma.services.utils.InsufficientResourcesException;
import com.gsma.services.nfc.SEController;
import android.os.Looper;

public final class NfcController
{
    private static final String TAG = "GsmaNfcController";
    final static boolean DBG = true;
    static HashMap<Context, NfcController> sNfcControllerMap = new HashMap<Context, NfcController>();
    private NfcController mNfcController;
    public Context mContext;
    private Callbacks mPendingCallbacks = null;
    MyConnection mConn = null;

    public interface Callbacks
    {
        // Card Emulation mode has been disabled
        public static final int CARD_EMULATION_DISABLED = 0;
        // Card Emulation mode has been enabled
        public static final int CARD_EMULATION_ENABLED = 1;
        // a problem occured during the enabling/disabling process
        public static final int CARD_EMULATION_ERROR = 256;

        // called when process for enabling the NFC Controller is finished
        public abstract void onEnableNfcController (boolean success);
        // called when process for enabling/disabling Card Emulation Mode is finished
        public abstract void onCardEmulationMode (int status);
        // called when process of getDefaultController is finished
        public abstract void onGetDefaultController (NfcController nfcController);
    }

    static final int MSG_RESULT_SUCCESS = 1;
    static final int MSG_RESULT_FAILED = 0;
    static final int MSG_RESULT_ENABLE_NFCC = 1;
    static final int MSG_RESULT_ENABLE_CE_MODE = 2;
    static final int MSG_RESULT_DISABLE_CE_MODE = 3;

    List<OffHostService> mOffHostServiceList;

    OffHostService mDefaultOffHostService;

    class msgHandler extends Handler
    {
        public msgHandler(Looper looper)
        {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case MSG_RESULT_ENABLE_NFCC:
                {
                    boolean isEnabled = (msg.arg1 == 1 ? true : false);
                    if (DBG) Log.d(TAG, "MSG_RESULT_ENABLE_NFCC isEnabled = " + isEnabled);
                    if (mPendingCallbacks != null) {
                        mPendingCallbacks.onEnableNfcController (isEnabled);
                        mPendingCallbacks = null;
                    }
                    break;
                }
                case MSG_RESULT_ENABLE_CE_MODE:
                {
                    boolean isSuccess = (msg.arg1 == 1 ? true : false);
                    if (DBG) Log.d(TAG, "MSG_RESULT_ENABLE_CE_MODE isSuccess = " + isSuccess);
                    if (mPendingCallbacks != null) {
                        if (isSuccess)
                            mPendingCallbacks.onCardEmulationMode (Callbacks.CARD_EMULATION_ENABLED);
                        else
                            mPendingCallbacks.onCardEmulationMode (Callbacks.CARD_EMULATION_ERROR);
                        mPendingCallbacks = null;
                    }
                    break;
                }
                case MSG_RESULT_DISABLE_CE_MODE:
                {
                    boolean isSuccess = (msg.arg1 == 1 ? true : false);
                    if (DBG) Log.d(TAG, "MSG_RESULT_DISABLE_CE_MODE isSuccess = " + isSuccess);
                    if (mPendingCallbacks != null) {
                        if (isSuccess)
                            mPendingCallbacks.onCardEmulationMode (Callbacks.CARD_EMULATION_DISABLED);
                        else
                            mPendingCallbacks.onCardEmulationMode (Callbacks.CARD_EMULATION_ERROR);
                        mPendingCallbacks = null;
                    }
                    break;
                }
                default:
                    super.handleMessage(msg);
            }
        }
    }

    final Messenger mMessenger = new Messenger(new msgHandler(Looper.getMainLooper()));

    private volatile IGsmaService mGsmaService;
    boolean mIsBound = false;
    private Object lock = new Object();

    public class MyConnection implements ServiceConnection
    {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service)
        {
            if (DBG) Log.d(TAG, "onServiceConnected");
            mGsmaService = IGsmaService.Stub.asInterface(service);
            try {
                //sync OffHostServices with NfcService
                mGsmaService.getOffHostServices(mContext.getPackageName(), mIGsmaServiceCallbacks);
                synchronized (lock)
                {
                    lock.wait(5000);
                }
            }
            catch (InterruptedException e) {
                Log.e(TAG, "In Interrupted exception onServiceConnected: " + e.getMessage());
            }
            catch (RemoteException e) {
                Log.e(TAG, "In Remote exception onServiceConnected: " + e.getMessage());
            }

            if (mPendingCallbacks != null)
                mPendingCallbacks.onGetDefaultController(NfcController.this);
        }

        @Override
        public void onServiceDisconnected(ComponentName className)
        {
            if (DBG) Log.d(TAG, "onServiceDiscnnected");
            mGsmaService = null;
        }
    };

    void bindService(Context context)
    {
        if (DBG) Log.d(TAG, "bindService()");
        mIsBound = ServiceUtils.bindService(context, mConn);
    }

    void unbindService(Context context)
    {
        if (DBG) Log.d(TAG, "unbindService()");
        if (mIsBound) {
            context.unbindService(mConn);
            mIsBound = false;
        }
    }

    protected NfcController (Context context, NfcController.Callbacks listeners)
    {
        mContext=context;
        mPendingCallbacks = listeners;
        mOffHostServiceList = new ArrayList<OffHostService>();
        mNfcController = this;
        if (mConn == null ) {
            mConn = new MyConnection();
            bindService(context);
        }
    }

    /**
     * Helper to return the instance of the NFC Controller in a callback function
     * context : the callling application's context
     * cb : callback to use for the default controller
     */
    public static synchronized void getDefaultController (Context context, NfcController.Callbacks cb)
    {
        if (DBG) Log.d(TAG, "getDefaultController()");
        NfcController nfcController = sNfcControllerMap.get(context);
        if (nfcController == null) {
            nfcController = new NfcController(context, cb);
            sNfcControllerMap.put(context, nfcController);
        }
        else {
            cb.onGetDefaultController(nfcController);
        }
    }

    // Return NFC Controller's state
    public boolean isEnabled()
    {
        if (DBG) Log.d(TAG, "isEnabled()");
        try {
            return mGsmaService.isNfccEnabled();
        }
        catch (RemoteException e) {
            Log.e(TAG, "mGsmaService.isNfccEnabled(): " + e.getMessage());
            return false;
        }
    }

    /**
     * asks the system to enable the NFC Controller and registers a callback function to run
     * when the activation is finished.
     * If the device doesn't support NFC, the callback will be called immediately.
     * User input is required to enable NFC. A question will be asked if user wants to enable
     * NFC or not. This question shall be generated within the OS context.
     */
    public void enableNfcController (NfcController.Callbacks cb)
    {
        if (DBG) Log.d(TAG, "enableNfcController()");
        try {
            if (mGsmaService.enableNfcc(mMessenger) == false) {
                cb.onEnableNfcController (false);
            }
            else {
                // store callback and call when NFC controller enabling is finished
                mPendingCallbacks = cb;
            }
        }
        catch (RemoteException e) {
            Log.e(TAG, "mGsmaService.enableNfcc(): " + e.getMessage());
        }
    }

    // Returns Card Emulation Mode's state
    public boolean isCardEmulationEnabled()
    {
        if (DBG) Log.d(TAG, "isCardEmulationEnabled()");
        boolean status = false;
            try {
                status = mGsmaService.isCardEmulationEnabled();
            }
            catch (RemoteException e) {
                Log.e(TAG, "isCardEmulationEnabled(): " + e.getMessage());
            }
            return status;
    }

    /**
     * asks the system to enable the "Card Emulation" mode and registers a callback function
     * to run when the process is finished. If the device does not support NFC, the callback
     * will be called immediately. If there is no user setting for card emulation on/off, the
     * change is not persistent and shall be overriden by the following events:
     *  - Turning off and on NFC
     *  - Full power cycle of the phone
     *  - Changing active SE
     * when UICC is the active SE, only applications signed with certificates from UICC are
     * granted to call this method. When eSE is the active SE, only applications signed with
     * system certificates are granted to call this method
     */
    public void enableCardEmulationMode (NfcController.Callbacks cb) throws IllegalStateException,SecurityException
    {
        if (DBG) Log.d(TAG, "enableCardEmulationMode()");
        try {
            if (mGsmaService.isNfccEnabled() == false) {
                // IllegalStateException - if NFC adaptor is not enabled
                throw new IllegalStateException("NFC adaptor is not enabled");
            }
            else {
                if (mGsmaService.isCardEmulationEnabled() == true) {
                    if (DBG) Log.d(TAG, "enableCardEmulationMode(): already enabled");
                    cb.onCardEmulationMode (Callbacks.CARD_EMULATION_ENABLED);
                    return;
                }
                if (mGsmaService.enableCardEmulationMode(mMessenger) == false) {
                    cb.onCardEmulationMode (Callbacks.CARD_EMULATION_ERROR);
                }
                else {
                    // store callback and call when NFC controller enabling is finished
                    mPendingCallbacks = cb;
                }
            }
        }
        catch (RemoteException e) {
            // SecurityException - if the application is not allowed to use this API
            throw new SecurityException("application is not allowed");
        }
    }

    /**
     * asks the system to disable the "Card Emulation" mode and registers a callback function
     * to run when the process is finished. If the device does not support NFC, the callback
     * will be called immediately.
     * when UICC is the active SE, only applications signed with certificates from UICC are
     * granted to call this method. When eSE is the active SE, only applications signed with
     * system certificates are granted to call this method
     */
    public void disableCardEmulationMode (NfcController.Callbacks cb) throws SecurityException
    {
        if (DBG) Log.d(TAG, "disableCardEmulationMode()");
        try {
            if (mGsmaService.isNfccEnabled() == false) {
                if (DBG) Log.d(TAG, "disableCardEmulationMode- Nfcc is disabled");
            }
            if (mGsmaService.isCardEmulationEnabled() == false) {
                if (DBG) Log.d(TAG, "enableCardEmulationMode(): already disabled");
                cb.onCardEmulationMode (Callbacks.CARD_EMULATION_DISABLED);
                return;
            }
            if (mGsmaService.disableCardEmulationMode(mMessenger) == false) {
                cb.onCardEmulationMode (Callbacks.CARD_EMULATION_ERROR);
            }
            else {
                // store callback and call when NFC controller enabling is finished
                mPendingCallbacks = cb;
            }
        }
        catch (RemoteException e) {
            // SecurityException - if the application is not allowed to use this API
            throw new SecurityException("application is not allowed");
        }
    }

    private IGsmaServiceCallbacks mIGsmaServiceCallbacks = new IGsmaServiceCallbacks.Stub()
    {
        public void onGetOffHostService (boolean isLast, boolean isDefault, String description,
                                         String seName, int bannerResId,
                                         List<String> dynamicAidGroupDescriptions,
                                         List<android.nfc.cardemulation.AidGroup> dynamicAidGroups)
        {
            if (DBG) Log.d(TAG, "onGetOffHostService() " + isLast + ", " + description + ", " + seName);
            if ((description != null) && (seName != null)) {
                OffHostService offHostService = new OffHostService(description, seName, mNfcController);
                //offHostService.setBanner (null);
                offHostService.setBanner (bannerResId);
                int index = 0;
                for (android.nfc.cardemulation.AidGroup dynamicAidGroup : dynamicAidGroups) {
                    com.gsma.services.nfc.AidGroup aidGroupGsma;
                    aidGroupGsma = offHostService.defineAidGroup (dynamicAidGroupDescriptions.get(index), dynamicAidGroup.getCategory());
                    for (String aid : dynamicAidGroup.getAids()) {
                        aidGroupGsma.addNewAid(aid);
                    }
                    index++;
                }
                mOffHostServiceList.add(offHostService);
                if(isDefault) {
                    if (DBG) Log.d(TAG, "isDefault: " + description);
                    mDefaultOffHostService = offHostService;
                }
            }
            if (isLast) {
                synchronized (lock)
                {
                    lock.notifyAll();
                    if (DBG) Log.d(TAG, "onGetOffHostService(): notified all" );
                }
            }
        }
    };

    protected boolean commitOffHostService (OffHostService offHostService)
    {
        if (DBG) Log.d(TAG, "commitOffHostService()");
        ArrayList<String> dynamicAidGroupDescriptions = new ArrayList<String>();
        ArrayList<android.nfc.cardemulation.AidGroup> dynamicAidGroups = new ArrayList<android.nfc.cardemulation.AidGroup>();
        boolean result = false;
        for (com.gsma.services.nfc.AidGroup aidGroupGsma : offHostService.getAidGroups()) {
            String[] aids = aidGroupGsma.getAids();
            ArrayList<String> aidList = new ArrayList<String>(aids.length);
            for (String aid : aids) {
                if (CardEmulation.isValidAid(aid)) {
                    aidList.add(aid.toUpperCase());
                }
                else {
                    Log.e(TAG, "commitOffHostService(): invalid AID:" + aid);
                }
            }
            // AidGroup must have at least one AID
            if (aidList.size() > 0) {
                try {
                    dynamicAidGroupDescriptions.add(aidGroupGsma.getDescription());
                    Log.d(TAG, "commitOffHostService(): Description : " + aidGroupGsma.getDescription());
                    android.nfc.cardemulation.AidGroup aidGroup = new
                            android.nfc.cardemulation.AidGroup(aidList, aidGroupGsma.getCategory());
                    dynamicAidGroups.add(aidGroup);
                }
                catch (Exception e) {
                    Log.e(TAG, "commitOffHostService(): " + e.getMessage());
                }
            }
        }
        Log.e(TAG, "dynamicAidGroups.size() " + dynamicAidGroups.size());
        try {
            result= mGsmaService.commitOffHostService(mContext.getPackageName(),
                                              offHostService.getLocation(),
                                              offHostService.getDescription(),
                                              offHostService.getBannerResId(),
                                              mContext.getApplicationInfo().uid,
                                              dynamicAidGroupDescriptions,
                                              dynamicAidGroups);
        }
        catch(RemoteException e) {
            Log.e(TAG, "commitOffHostService(): " + e.getMessage());
            result = false;
        }
    return result;
    }

    //Create a new "Off-Host" service
    public OffHostService defineOffHostService(String description, String SEName) throws IllegalArgumentException,UnsupportedOperationException
    {
        if (DBG) Log.d(TAG, "defineOffHostService():" + description + ", " + SEName);
        if ((SEName == null)||
            (SEName.startsWith("SIM")==false)) {
            throw new IllegalArgumentException("Invalid SE Name");
        }
        try {
            OffHostService newOffHostService = new OffHostService(description, SEName, this);
            mOffHostServiceList.add(newOffHostService);
            return newOffHostService;
        }
        catch (Exception e) {
            Log.e(TAG, "HCE is not supported");
            throw new UnsupportedOperationException();
        }
    }

    //Delete an existing dynamically created "Off-Host" service.
    public void deleteOffHostService(OffHostService service) throws IllegalArgumentException,UnsupportedOperationException
    {
        if (DBG) Log.d(TAG, "deleteOffHostService():" + mContext.getPackageName() + ", " + service.getLocation());
        if(service == null) {
            throw new IllegalArgumentException("Invalid OffHostService");
        }
        try {
            mGsmaService.deleteOffHostService(mContext.getPackageName(), service.getLocation());
        }
        catch(RemoteException e) {
            Log.e(TAG, "deleteOffHostService(): " + e.getMessage());
            throw new UnsupportedOperationException();
        }
        mOffHostServiceList.remove(service);
    }

    //Return the "Off-Host" service related to the current selected "Tap&Pay" menu entry.
    public OffHostService getDefaultOffHostService()
    {
        if (DBG) Log.d(TAG, "getDefaultOffHostService(): enter");
        try {
            String seName =  mGsmaService.getActiveSecureElement();
            for (OffHostService offHostService : mOffHostServiceList) {
                if (seName.startsWith(offHostService.getLocation())) {
                    if (offHostService.equals(mDefaultOffHostService)) {
                        return offHostService;
                    }
                }
            }
        }
        catch (RemoteException e) {
            Log.e(TAG, "mGsmaService.getActiveSecureElement() : " + e.getMessage());
            return null;
        }
        Log.e(TAG, "getDefaultOffHostService(): cannot find default");
        return null;
    }

    //Return the list of "Off-Host" services belonging to the calling application
    public OffHostService[] getOffHostServices()
    {
        if (DBG) Log.d(TAG, "getOffHostServices()");
        OffHostService[] offHostServiceArray = new OffHostService[mOffHostServiceList.size()];
        return mOffHostServiceList.toArray(offHostServiceArray);
    }
}

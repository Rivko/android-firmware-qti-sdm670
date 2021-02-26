/*
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.gsma.services.nfc;

import com.gsma.services.nfc.*;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.os.Message;
import android.os.Messenger;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.UserHandle;
import android.app.Service;
import android.nfc.NfcAdapter;
import android.util.Log;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.lang.InterruptedException;
import java.lang.UnsupportedOperationException;
import com.nxp.nfc.gsma.internal.NxpNfcController;
import com.nxp.nfc.gsma.internal.NxpNfcController.Callbacks;
import com.nxp.nfc.gsma.internal.NxpNfcController.NxpCallbacks;
import com.nxp.nfc.NxpNfcAdapter;
import java.io.IOException;

public class GsmaService extends Service
{
    private static final String TAG = "GsmaNfcService";
    final static boolean DBG = true;
    static NfcAdapter sNfcAdapter = null;
    private BroadcastReceiver mNfcAdapterEventReceiver;
    private Context mContext;
    private  NxpNfcController mNxpNfcController = null;
    private  NxpNfcAdapter sNxpNfcAdapter = null;

    static final String ENABLE_NFCC = "ENABLE_NFCC";
    static final String ENABLE_CE_MODE = "ENABLE_CE_MODE";
    static final String DISABLE_CE_MODE = "DISABLE_CE_MODE";

    static final int MSG_RESULT_SUCCESS = 1;
    static final int MSG_RESULT_FAILED = 0;
    static final int MSG_RESULT_ENABLE_NFCC = 1;
    static final int MSG_RESULT_ENABLE_CE_MODE = 2;
    static final int MSG_RESULT_DISABLE_CE_MODE = 3;

    final static String OMAPI_SERVICE = "org.simalliance.openmobileapi.service";
    final static String ACTION_CHECK_CERT = "org.simalliance.openmobileapi.service.ACTION_CHECK_CERT";
    final static String ACTION_CHECK_X509 = "org.simalliance.openmobileapi.service.ACTION_CHECK_X509";
    final static String ACTION_CHECK_AID = "org.simalliance.openmobileapi.service.ACTION_CHECK_AID";
    final static String EXTRA_PKG = "org.simalliance.openmobileapi.service.EXTRA_PKG";
    final static String EXTRA_SE_NAME = "org.simalliance.openmobileapi.service.EXTRA_SE_NAME";
    final static String EXTRA_AIDS = "org.simalliance.openmobileapi.service.EXTRA_AIDS";

    static HashMap<String, Messenger> sClientMessengerMap = new HashMap();
    String pack = null;
    private AdapterCallbacks mAdapterCallbacks;
    static HashMap<Context, IGsmaServiceCallbacks> sIGsmaServiceCallbacksMap = new HashMap();
    private NxpNfcControllerCallback mNxpCallback = null;
    private Messenger mClientMessenger = null;

    private void sendMsg(Messenger clientMessenger, int msgId, int result)
    {
        Message msg = Message.obtain(null, msgId, result, 0);
        try {
            clientMessenger.send(msg);
        }
        catch (RemoteException e) {
            Log.e(TAG, "clientMessenger.send() failed: " + e.getMessage());
        }
    }

    private void registerNfcAdapterEvent(String action, Messenger clientMessenger)
    {
        if (DBG) Log.d(TAG, "register NFC Adapter event for action:" + action);
        sClientMessengerMap.put(action, clientMessenger);
        if (mNfcAdapterEventReceiver != null) {
            return;
        }
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
        intentFilter.addAction("com.android.nfc_extras.action.ENABLE_NFC_ADAPTER_FAILED");
        mNfcAdapterEventReceiver = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                String action = intent.getAction();
                if (DBG) Log.d(TAG, "got intent " + action);
                if (action.equals(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED)) {
                    int isEnabled;
                    int adapterState = intent.getIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE, 1);
                    if (adapterState == NfcAdapter.STATE_TURNING_ON) {
                        if (DBG) Log.d(TAG, "NfcAdapter.STATE_TURNING_ON");
                        // wait for STATE_ON
                        return;
                    }
                    else if (adapterState == NfcAdapter.STATE_ON) {
                        if (DBG) Log.d(TAG, "NfcAdapter.STATE_ON");
                        isEnabled = 1;
                    }
                    else {
                        isEnabled = 0;
                    }
                    Messenger clientMessenger = sClientMessengerMap.remove(ENABLE_NFCC);
                    if (clientMessenger != null) {
                        sendMsg(clientMessenger, MSG_RESULT_ENABLE_NFCC, MSG_RESULT_SUCCESS);
                    }
                }
                else if (action.equals("com.android.nfc_extras.action.ENABLE_NFC_ADAPTER_FAILED")) {
                    if (DBG) Log.d(TAG, "Failed to enable NFC Controller");
                    Messenger clientMessenger = sClientMessengerMap.remove(ENABLE_NFCC);
                    if (clientMessenger != null) {
                        sendMsg(clientMessenger, MSG_RESULT_ENABLE_NFCC, MSG_RESULT_FAILED);
                    }
                }
                // if no client is waiting
                if (sClientMessengerMap.isEmpty()) {
                    unregisterNfcAdapterEvent();
                }
                else if (DBG) {
                    Log.d(TAG, "sClientMessengerMap.size() = " + sClientMessengerMap.size());
                }
            }
        };
        mContext.registerReceiver(mNfcAdapterEventReceiver, intentFilter);
    }

    private void unregisterNfcAdapterEvent()
    {
        if (DBG) Log.d(TAG, "unregister NFC Adapter event");
        mContext.unregisterReceiver(mNfcAdapterEventReceiver);
        mNfcAdapterEventReceiver = null;
    }

    private class AdapterCallbacks implements NxpNfcController.Callbacks
    {
        public void onGetOffHostService(boolean isLast, boolean isDefault, String description,
                                        String seName, int bannerResId,
                                        List<String> dynamicAidGroupDescriptions,
                                        List<android.nfc.cardemulation.AidGroup> dynamicAidGroups)
        {
            if (DBG) Log.d(TAG, "onGetOffHostService() " + isLast + ", " + description + ", "
                           + seName + ", " + String.valueOf(bannerResId));
            IGsmaServiceCallbacks callbacks = sIGsmaServiceCallbacksMap.get(mContext);
            if (callbacks != null) {
                try {
                    callbacks.onGetOffHostService(isLast, isDefault, description, seName, bannerResId,
                                                  dynamicAidGroupDescriptions, dynamicAidGroups);
                }
                catch (Exception e) {
                    Log.e(TAG, "onGetOffHostService() " + e.getMessage());
                }
            }
        }
    }

    public class NxpNfcControllerCallback implements NxpCallbacks
    {
        public void onNxpEnableNfcController(boolean success)
        {
            if(success == true) {
                //TODO: need to intimate to Gsma Framework
                sendMsg(mClientMessenger, MSG_RESULT_ENABLE_NFCC, MSG_RESULT_SUCCESS);
                Log.d(TAG, "NFC enabled");
            }
            else {
                sendMsg(mClientMessenger, MSG_RESULT_ENABLE_NFCC, MSG_RESULT_FAILED);
                Log.d(TAG, "NFC not enabled");
            }
        }
    }

    private IGsmaService.Stub mGsmaServiceBinder = new IGsmaService.Stub()
    {
        @Override
        public boolean isNfccEnabled()
        {
            if (DBG) Log.d(TAG, "isNfccEnabled()");
            if (sNfcAdapter != null) {
                return sNfcAdapter.isEnabled();
            }
            else {
                return false;
            }
        }

        @Override
        public boolean enableNfcc(Messenger clientMessenger)
        {
            if (DBG) Log.d(TAG, "enableNfcc()");
            mClientMessenger = clientMessenger;
            if (mNxpNfcController != null) {
                mNxpNfcController.enableNxpNfcController(mNxpCallback);
                return true;
            }
            else {
                return false;
            }
        }

        @Override
        @Deprecated
        public boolean isCardEmulationEnabled()
        {
            return false;
        }

        @Override
        @Deprecated
        public boolean enableCardEmulationMode(Messenger clientMessenger)
        {
            return false;
        }

        @Override
        @Deprecated
        public boolean disableCardEmulationMode(Messenger clientMessenger)
        {
            return false;
        }

        @Override
        @Deprecated
        public String getActiveSecureElement()
        {
            return "SIM";
        }

        @Override
        @Deprecated
        public void setActiveSecureElement(String SEName)
        {
        }

        @Override
        public void mgetPname(String packageN)
        {
            pack = packageN;
        }

        @Override
        public String[] getActiveSecureElementList(String packageName)
        {
            String[] secElemArray = null;
            if (sNfcAdapter == null) {
                sNfcAdapter = NfcAdapter.getDefaultAdapter(mContext);
            }
            Log.d(TAG, "In getActiveSecureElementList");
            if (sNxpNfcAdapter == null) {
                sNxpNfcAdapter = NxpNfcAdapter.getNxpNfcAdapter(sNfcAdapter);
            }
            try{
                secElemArray= sNxpNfcAdapter.getActiveSecureElementList(packageName);
                Log.d(TAG, "In  getActiveSecureElementList try block ");
                for(int i = 0; i <= secElemArray.length - 1; i++) {
                    if(secElemArray[i] == "com.nxp.smart_mx.ID") {
                    secElemArray[i] = SEController.SE_NAME_ESE1;
                    }
                    if(secElemArray[i] == "com.nxp.uicc.ID") {
                        secElemArray[i] = SEController.SE_NAME_SIM1;
                    }
                    if(secElemArray[i] == "com.nxp.uicc2.ID") {
                        secElemArray[i] = SEController.SE_NAME_SIM2;
                    }
                }
            }
            catch (IOException e) {
                Log.e(TAG,"getActiveSecureElementList : " + e.getMessage());
                secElemArray= null;
            }
            return secElemArray;
        }

        @Override
        public void enableMultiReception(String SEName)
        {
            if (DBG) Log.d(TAG, "enableMultiReception() " + SEName);
            try {
                if (mNxpNfcController == null) {
                    Log.e(TAG, "couldn't get NfcController");
                    return;
                }
                //nfcAdapter.GsmaPack(pack);
                mNxpNfcController.enableMultiReception(SEName, pack);
            }
            catch (Exception e) {
                Log.e(TAG, "enableMultiReception() : " + e.getMessage());
                // SecurityException - if the application is not allowed to use this API
                throw new SecurityException("application is not allowed");
            }
        }

        /*
         * removes aids from aidGroups that are not selectable for the given secure eleement.
         * @ param seName the secure element to query for selectability
         * @ param aidGroups the aids that we are checking and removing if not selectable
         */
        private synchronized void filterOutAidsNotInSecureElement (String seName,
                                  List<android.nfc.cardemulation.AidGroup> aidGroups)
        {
            if (DBG) Log.d(TAG, "filterOutAidsNotInSecureElement() " + seName);
            List<android.nfc.cardemulation.AidGroup> selectableAidGroups
                = new ArrayList<android.nfc.cardemulation.AidGroup>();
            final List<String> selectableAidList = new ArrayList<String>();
            final Object lock = new Object();
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(ACTION_CHECK_AID);

            // An AidGroup has a list of aids. Each AidGroup will be sent to the SmartCardService
            // for filtering by broadcast intent. The filtered lists will be returned and parsed
            // in below broadcast reciever.
            // These requests are serialized by the lock object.
            BroadcastReceiver receiver = new BroadcastReceiver()
            {
                @Override
                public void onReceive(Context context, Intent intent)
                {
                    String action = intent.getAction();
                    // Smartcard Service will return AIDs those are selectable
                    if (action.equals(ACTION_CHECK_AID)){
                        synchronized (lock)
                        {
                            String aidStringsWithComma = intent.getStringExtra(EXTRA_AIDS);
                            if (DBG) Log.d(TAG, "filterOutAidsNotInSecureElement(): selectable AIDs:"
                                           + aidStringsWithComma);
                            String[] aidStrings;
                            if (aidStringsWithComma != null) {
                                aidStrings = aidStringsWithComma.split(",");
                                for (int i = 0; (aidStrings != null)&&(i < aidStrings.length); i++) {
                                    selectableAidList.add(aidStrings[i]);
                                }
                            }
                            lock.notifyAll();
                            if (DBG) Log.d(TAG, "filterOutAidsNotInSecureElement(): Notifying All" );
                        }
                    }
                }
            };
            mContext.registerReceiver(receiver, intentFilter);
            for (android.nfc.cardemulation.AidGroup aidGroup: aidGroups) {
                selectableAidList.clear();
                StringBuilder aids = new StringBuilder();
                for (String aid: aidGroup.getAids()) {
                    if (aids.length() > 0)
                        aids.append(",");
                    aids.append(aid);
                }
                if (DBG) Log.d(TAG, "filterOutAidsNotInSecureElement(): checking " + seName +
                               ", " + aids.toString());
                Intent reqIntent = new Intent();
                reqIntent.setAction(ACTION_CHECK_AID);
                reqIntent.putExtra(EXTRA_SE_NAME, seName);
                reqIntent.putExtra(EXTRA_AIDS, aids.toString());
                reqIntent.setPackage(OMAPI_SERVICE);
                mContext.sendBroadcastAsUser(reqIntent,UserHandle.CURRENT);
                try {
                    synchronized (lock)
                    {
                        lock.wait(5000);
                        if (selectableAidList.size() > 0) {
                            // create new AID Group with only selectable AIDs
                            android.nfc.cardemulation.AidGroup selectableAidGroup
                                = new android.nfc.cardemulation.AidGroup(selectableAidList,
                                  aidGroup.getCategory());
                            selectableAidGroups.add(selectableAidGroup);
                        }
                    }
                }
                catch (Exception e) {
                    Log.e(TAG, "filterOutAidsNotInSecureElement(): " + e.getMessage());
                }
            }
            mContext.unregisterReceiver(receiver);
            // replace AID Group with only selectable AIDs
            aidGroups.clear();
            for (android.nfc.cardemulation.AidGroup aidGroup: selectableAidGroups) {
                aidGroups.add(aidGroup);
            }
        }

        @Override
        public boolean commitOffHostService(String packageName, String seName, String description,
                int bannerResId, int uid, List<String> aidGroupDescriptions,
                List<android.nfc.cardemulation.AidGroup> aidGroups)
        {
            if (DBG) Log.d(TAG, "commitOffHostService() " + packageName + ", " + seName);
            boolean result = false;
            if ((sNfcAdapter != null)&&(sNfcAdapter.isEnabled())&&(mNxpNfcController!=null)) {
                if(DBG) Log.d(TAG, "before filter: " +aidGroups.size());
                // filter out AIDs which are not selectable
                filterOutAidsNotInSecureElement (seName, aidGroups);
                if(DBG) Log.d(TAG, "after filter: " +aidGroups.size());
                for(String desc: aidGroupDescriptions) {
                    if(DBG) Log.d(TAG, "desc: " + desc);
                }
                try {
                    result = mNxpNfcController.commitOffHostService(packageName, seName, description,
                             bannerResId, uid, aidGroupDescriptions, aidGroups);
                }
                catch (Exception e) {
                    Log.e(TAG, "Exception in remote call (): " +e.getMessage());
                    e.printStackTrace();
                }
            }
            return result;
        }

        @Override
        public boolean deleteOffHostService(String packageName, String seName)
        {
            if (DBG) Log.d(TAG, "deleteOffHostService() " + packageName + ", " + seName);
            if ((sNfcAdapter!=null)&&(sNfcAdapter.isEnabled())&&(mNxpNfcController!=null)) {
                return (mNxpNfcController.deleteOffHostService(packageName, seName));
            }
            else {
                Log.e(TAG, "couldn't get NfcController");
                return false;
            }
        }

        @Override
        public boolean getOffHostServices(String packageName, IGsmaServiceCallbacks callbacks)
        {
            if (DBG) Log.d(TAG, "getOffHostServices() " + packageName);
            if ((sNfcAdapter!=null)&&(sNfcAdapter.isEnabled())&&(mNxpNfcController!=null)) {
                sIGsmaServiceCallbacksMap.put(mContext, callbacks);
                return (mNxpNfcController.getOffHostServices(packageName, mAdapterCallbacks));
            }
            Log.e(TAG, "getOffHostServices() couldn't get NfcAdapter/NfcController or NFC Service is not enabled");
            try {
                callbacks.onGetOffHostService(true, false, null, null, 0, null, null);
            }
            catch (Exception e) {
                Log.e(TAG, "getOffHostServices() " + e.getMessage());
            }
            return false;
        }
    };

    @Override
    public void onCreate()
    {
        Log.d(TAG,"service created");
        mContext = getApplicationContext();
        new Thread()
        {
            public void run()
            {
                for(int tries =0; tries<3; tries++) {
                    try {
                        mNxpNfcController = new NxpNfcController(mContext);
                        mNxpCallback = new NxpNfcControllerCallback();
                        mAdapterCallbacks = new AdapterCallbacks();
                        if (sNfcAdapter == null)
                            sNfcAdapter = NfcAdapter.getDefaultAdapter(mContext);
                        if((sNfcAdapter != null) && (mNxpNfcController != null))
                            Log.d(TAG,"NfcAdapter/NfcController acquired");
                        return;
                    }
                    catch (UnsupportedOperationException e) {
                        String errorMsg = "GSMA service gracefully failing to acquire NfcAdapter/NfcController at boot. try" + tries;
                        Log.e(TAG, errorMsg);
                        e.printStackTrace();
                    }
                    catch (Exception e) {
                        Log.e(TAG, "Exception in onCreate() " + e.getMessage());
                    }
                    try {
                        if(DBG) Log.d(TAG,"Waiting for NfcAdapter/NfcController");
                        synchronized(this)
                        {
                            wait(5000);
                        }
                    }
                    catch (InterruptedException e) {
                        Log.e(TAG,"Interupted while waiting for NfcAdapter. by" + e.getMessage());
                    }
                }
            }
        }.start();
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        if (IGsmaService.class.getName().equals(intent.getAction())) {
            return mGsmaServiceBinder;
        }
        return null;
    }
}

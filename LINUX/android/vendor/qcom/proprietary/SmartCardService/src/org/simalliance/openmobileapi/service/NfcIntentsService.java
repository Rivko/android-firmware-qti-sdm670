/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.simalliance.openmobileapi.service;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import android.Manifest;
import android.Manifest.permission;

import com.android.nfc_extras.NfcAdapterExtras;
import com.android.nfc_extras.NfcExecutionEnvironment;

import android.nfc.NfcAdapter;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.UserHandle;

import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.security.AccessControlEnforcer;

import com.nxp.nfc.NxpNfcAdapter;
import com.nxp.nfc.NxpConstants;
import com.nxp.nfc.INxpNfcAdapterExtras;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * The smartcard service is setup with privileges to access smart card hardware.
 * The service enforces the permission
 * 'org.simalliance.openmobileapi.service.permission.BIND'.
 */
public final class NfcIntentsService extends Service {

    public static final String _TAG = "SmartcardService - NfcIntentsService";

    final static String RF_FIELD_ON_DETECTED = NfcAdapterExtras.ACTION_RF_FIELD_ON_DETECTED;
    final static String RF_FIELD_OFF_DETECTED = NfcAdapterExtras.ACTION_RF_FIELD_OFF_DETECTED;
    final static String AID_SELECTED = NfcExecutionEnvironment.ACTION_AID_SELECTED;
    final static String NFC_EXTRAS_AID = NfcExecutionEnvironment.EXTRA_AID;
    final static String NFC_EXTRAS_DATA = "com.android.nfc_extras.extra.DATA";
    final static String NFC_EXTRAS_SENAME = "com.android.nfc_extras.extra.SE_NAME";

    final static String GSMA_TRANSACTION_EVENT = "com.gsma.services.nfc.action.TRANSACTION_EVENT";
    final static String GSMA_TRANSACTION_PERMISSION = "com.gsma.services.nfc.permission.TRANSACTION_EVENT";

    final static String NXP_TRANSACTION_DETECTED = NxpConstants.ACTION_TRANSACTION_DETECTED;
    final static String NXP_EXTRA_SOURCE = NxpConstants.EXTRA_SOURCE;
    final static String NXP_UICC_ID = NxpConstants.UICC_ID;
    final static String NXP_EXTRA_AID = NxpConstants.EXTRA_AID;
    final static String NXP_EXTRA_DATA = NxpConstants.EXTRA_DATA;

    final static String ACTION_CHECK_CERT = "org.simalliance.openmobileapi.service.ACTION_CHECK_CERT";
    final static String ACTION_CHECK_X509 = "org.simalliance.openmobileapi.service.ACTION_CHECK_X509";
    final static String ACTION_CHECK_AID = "org.simalliance.openmobileapi.service.ACTION_CHECK_AID";
    final static String EXTRA_PKG = "org.simalliance.openmobileapi.service.EXTRA_PKG";
    final static String EXTRA_SE_NAME = "org.simalliance.openmobileapi.service.EXTRA_SE_NAME";
    final static String EXTRA_AIDS = "org.simalliance.openmobileapi.service.EXTRA_AIDS";


    private BroadcastReceiver mNfcEventReceiver;
    private BroadcastReceiver mPackageUpdateReceiver;
    private BroadcastReceiver mNfcReceiver;
    private BroadcastReceiver mGsmaServiceEventReceiver;

    static NfcAdapter mNfcAdapter = null;
    private NxpNfcAdapter mNxpNfcAdapter = null;
    private INxpNfcAdapterExtras mNxpNfcAdapterExtras = null;
    private int numGsmaPackages;
    private int numNfcPackages;
    private String [] packageNames;
    private boolean [] nfcEventAccessFinal = null;
    /* Async task */
    InitialiseTask mInitialiseTask;
    // packages with Manifest.permission.NFC
    List<PackageInfo> mInstalledNfcPackages = new ArrayList<PackageInfo>();
    // packages with com.gsma.services.nfc.permission.TRANSACTION_EVENT
    List<PackageInfo> mInstalledGsmaPackages = new ArrayList<PackageInfo>();

    @Override
    public void onCreate() {

        final Context context = getApplicationContext();
        new Thread(){
            public void run() {
                for(int tries = 0; tries < 3; tries++) {
                    try {
                        if(mNfcAdapter == null)
                            mNfcAdapter = NfcAdapter.getDefaultAdapter(context);
                        if((mNxpNfcAdapter == null) && (mNfcAdapter != null))
                            mNxpNfcAdapter = NxpNfcAdapter.getNxpNfcAdapter(mNfcAdapter);
                        if(mNxpNfcAdapterExtras == null) {
                            mNxpNfcAdapterExtras = mNxpNfcAdapter.getNxpNfcAdapterExtrasInterface(mNfcAdapter.getNfcAdapterExtrasInterface());
                        }
                        if (mNxpNfcAdapterExtras == null) {
                            Log.i(_TAG, "Couldn't get NfcAdapter");
                            return;
                        } else {
                        Log.d(_TAG,"binding established");
                        return;
                        }

                    } catch (UnsupportedOperationException | SecurityException e) {
                        String errorMsg = "Smartcard service gracefully failing to acquire NfcAdapter at boot. try " + tries;
                        Log.e(_TAG, errorMsg);
                        new Throwable(_TAG + ": " + errorMsg, e);
                        e.printStackTrace();
                    }
                    try {
                        synchronized(this) {
                            wait(5000);
                        }
                    } catch (InterruptedException e) {
                        Log.d(_TAG, "Interupted while waiting for NfcAdapter by " + e);
                    }
                }
            }
        }.start();

        mInitialiseTask = new InitialiseTask();
        mInitialiseTask.execute();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private class InitialiseTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected Void doInBackground(Void... arg0) {
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            Log.i(_TAG, "OnPostExecute()");
            updatePackageCache();
            registerNfcEvent(getApplicationContext());
            registerAdapterStateChangedEvent(getApplicationContext());
            registerPackageUpdateEvent(getApplicationContext());
            registerGsmaServiceEvent(getApplicationContext());
            mInitialiseTask = null;
        }
    }

    void updatePackageCache() {
        PackageManager pm = getPackageManager();
        String[] permissions = new String[]{Manifest.permission.NFC, GSMA_TRANSACTION_PERMISSION};
        List<PackageInfo> packages = pm.getPackagesHoldingPermissions(permissions, PackageManager.GET_ACTIVITIES);

        // packages with GSMA permission, sorted by installed time
        Map<Long, PackageInfo> gsmaPackages = new TreeMap<Long, PackageInfo>();

        synchronized (this) {
            mInstalledNfcPackages.clear();
            mInstalledGsmaPackages.clear();
            boolean isGsmaService = false;
            for (PackageInfo packageInfo: packages) {
                if (packageInfo.applicationInfo != null) {
                    if (packageInfo.packageName.equals("com.qualcomm.qti.gsma.services.nfc") ||
                        packageInfo.packageName.equals("org.simalliance.openmobileapi.service")) {
                        // do not include these packages
                        continue;
                    }
                } else {
                    continue;
                }
                if (packageInfo.requestedPermissions != null) {
                    for (int xx = 0; xx < packageInfo.requestedPermissions.length; xx++) {
                        if (packageInfo.requestedPermissions[xx].equals(GSMA_TRANSACTION_PERMISSION)){
                            // add this into GSMA packages
                            isGsmaService = true;
                            break;
                        }
                    }
                }
                if (isGsmaService) {
                    // sorted by installed time for unicast mode
                    gsmaPackages.put(packageInfo.firstInstallTime, packageInfo);
                    isGsmaService = false;
                } else {
                    mInstalledNfcPackages.add(packageInfo);
                }
            }

            for (Map.Entry<Long, PackageInfo> entry: gsmaPackages.entrySet()){
                mInstalledGsmaPackages.add(entry.getValue());
            }
            numGsmaPackages = mInstalledGsmaPackages.size();
            numNfcPackages = mInstalledNfcPackages.size();
            packageNames = new String[numGsmaPackages + numNfcPackages];
            {
                int i = 0;
                for (PackageInfo pkg : mInstalledGsmaPackages) {
                    if (pkg != null && pkg.packageName != null) {
                        packageNames[i++] = new String(pkg.packageName);
                        Log.i(_TAG, "Package name :" + pkg.packageName + " Package install time :" + pkg.firstInstallTime);
                    }
                }
                for (PackageInfo pkg : mInstalledNfcPackages) {
                    if (pkg != null && pkg.packageName != null) {
                        packageNames[i++] = new String(pkg.packageName);
                    }
                }
            }
        }
    }

    private void registerPackageUpdateEvent(Context context) {
        Log.i(_TAG, "register Package Update event");

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Intent.ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
        intentFilter.addAction(Intent.ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
        intentFilter.addAction(Intent.ACTION_PACKAGE_ADDED);
        intentFilter.addAction(Intent.ACTION_PACKAGE_REMOVED);
        intentFilter.addDataScheme("package");

        mPackageUpdateReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (action.equals(Intent.ACTION_PACKAGE_REMOVED) ||
                    action.equals(Intent.ACTION_PACKAGE_ADDED) ||
                    action.equals(Intent.ACTION_EXTERNAL_APPLICATIONS_AVAILABLE) ||
                    action.equals(Intent.ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
                    updatePackageCache();
                }
            };
        };
        context.registerReceiver(mPackageUpdateReceiver, intentFilter);
    }

    private void unregisterPackageUpdateEvent(Context context) {
        if(mPackageUpdateReceiver!= null) {
            Log.i(_TAG, "unregister Package Update event");
            context.unregisterReceiver(mPackageUpdateReceiver);
            mPackageUpdateReceiver = null;
        }
    }

    private void registerAdapterStateChangedEvent(Context context) {
        Log.i(_TAG, "register ADAPTER_STATE_CHANGED event");

        IntentFilter intentFilter = new IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
        mNfcReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                final boolean nfcAdapterAction = intent.getAction().equals(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
                final boolean nfcAdapterOn = nfcAdapterAction && intent.getIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE, NfcAdapter.STATE_OFF) == NfcAdapter.STATE_ON; // is NFC Adapter turned on ?
                if( nfcAdapterOn){
                    Log.i(_TAG, "NFC Adapter is ON. Checking access rules for updates.");
                    SmartcardService.loadEseRules();
                }
            }
        };
        context.registerReceiver(mNfcReceiver, intentFilter);
    }

    private void unregisterAdapterStateChangedEvent(Context context) {
        if(mNfcReceiver!= null) {
            Log.i(_TAG, "unregister ADAPTER_STATE_CHANGED event");
            context.unregisterReceiver(mNfcReceiver);
            mNfcReceiver = null;
        }
     }

    private void registerNfcEvent(Context context) {
        Log.i(_TAG, "register NFC event");

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(RF_FIELD_ON_DETECTED);
        intentFilter.addAction(RF_FIELD_OFF_DETECTED);
        intentFilter.addAction(AID_SELECTED);
        intentFilter.addAction(NXP_TRANSACTION_DETECTED);
        intentFilter.addAction(ACTION_CHECK_CERT);
        intentFilter.addAction(ACTION_CHECK_X509);

        mNfcEventReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                boolean nfcAdapterExtraActionRfFieldOn = false;
                boolean nfcAdapterExtraActionRfFieldOff = false;
                boolean nfcAdapterExtraActionAidSelected = false;
                byte[] aid = null;
                byte[] data = null;
                String seName = null;
                String action = intent.getAction();

                if (action.equals(RF_FIELD_ON_DETECTED)){
                    nfcAdapterExtraActionRfFieldOn = true;
                    aid = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00 };
                    Log.i(_TAG, "got RF_FIELD_ON_DETECTED");
                }
                else if (action.equals(RF_FIELD_OFF_DETECTED)){
                    nfcAdapterExtraActionRfFieldOff = true;
                    aid = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00 };
                    Log.i(_TAG, "got RF_FIELD_OFF_DETECTED");
                }
                else if (action.equals(ACTION_CHECK_X509)){
                    Log.i(_TAG, "got ACTION_CHECK_X509");
                    String pkg = intent.getStringExtra(EXTRA_PKG);
                    seName = intent.getStringExtra(EXTRA_SE_NAME);
                    if (seName.equals(NXP_UICC_ID)) {
                        seName = "SIM1";
                    }
                    Log.i(_TAG, "got ACTION_CHECK_X509 pgk: " + pkg + " seName: " + seName);

                    if(mNfcAdapter == null)
                        mNfcAdapter = NfcAdapter.getDefaultAdapter(context);
                    if((mNxpNfcAdapter == null) && (mNfcAdapter != null))
                        mNxpNfcAdapter = NxpNfcAdapter.getNxpNfcAdapter(mNfcAdapter);
                    if(mNxpNfcAdapterExtras == null) {
                        mNxpNfcAdapterExtras = mNxpNfcAdapter.getNxpNfcAdapterExtrasInterface(mNfcAdapter.getNfcAdapterExtrasInterface());
                    }
                    if (mNxpNfcAdapterExtras == null) {
                        Log.i(_TAG, "Couldn't get NfcAdapter");
                        return;
                    }

                    SmartcardError error = new SmartcardError();
                    ITerminal terminal = SmartcardService.getTerminal(seName, error);
                    if (terminal == null) {
                        Log.i(_TAG, "Couldn't get terminal for " + seName);
                        return;
                    }

                    AccessControlEnforcer acEnforcer;
                    acEnforcer = terminal.getAccessControlEnforcer();
                    if( acEnforcer == null ) {
                        Log.i(_TAG, "Couldn't get AccessControlEnforcer for " + seName);
                        try {
                            mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, false);
                        } catch (RemoteException Re) {
                            Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                        }
                        return;
                    } else {
                        /*
                         * PackageManager needs to be initialized for the access control enforcer
                         */
                        acEnforcer.setPackageManager(getPackageManager());
                    }

                    try {
                       if (acEnforcer.hasCertificate(pkg) && acEnforcer.Checkx509Certif(pkg)) {
                            try {
                                Log.i(_TAG, "got ACTION_CHECK_X509 - returning ALLOWED");
                                mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, true);
                            } catch (RemoteException Re) {
                            Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                            }
                       } else {
                            try {
                                Log.i(_TAG, "got ACTION_CHECK_X509 - returning DENIED 1");
                                mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, false);
                            } catch (RemoteException Re) {
                            Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                            }
                       }
                    } catch (Exception e) {
                        try {
                            Log.i(_TAG, "got ACTION_CHECK_X509 - returning DENIED 2 exc : " + e);
                            mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, false);
                        } catch (RemoteException Re) {
                            Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                        }
                        catch (Exception e2) {
                            Log.e(_TAG, "notifyCheckCertResult - exception caught" + e2);
                        }
                    }

                    return;
                }
                else if (action.equals(ACTION_CHECK_CERT)){
                    Log.i(_TAG, "got ACTION_CHECK_CERT");
                    seName = intent.getStringExtra(EXTRA_SE_NAME);
                    if (seName.equals(NXP_UICC_ID)) {
                        seName = "SIM1";
                    }
                    String pkg = intent.getStringExtra(EXTRA_PKG);
                    Log.i(_TAG, "SE_NAME : " + seName + ", PKG : " + pkg);

                    NfcAdapter mNfcAdapter = NfcAdapter.getDefaultAdapter(context);
                    if (mNfcAdapter == null) {
                        Log.i(_TAG, "Couldn't get NfcAdapter");
                        return;
                    }

                    SmartcardError error = new SmartcardError();
                    ITerminal terminal = SmartcardService.getTerminal(seName, error);
                    if (terminal == null) {
                        Log.i(_TAG, "Couldn't get terminal for " + seName);
                        return;
                    }

                    AccessControlEnforcer acEnforcer;
                    acEnforcer = terminal.getAccessControlEnforcer();
                    if( acEnforcer == null ) {
                        Log.i(_TAG, "Couldn't get AccessControlEnforcer for " + seName);
                            try {
                                mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, false);
                            } catch (RemoteException Re) {
                                Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                            }
                        return;
                    }

                    Log.i(_TAG, "Checking access rules for " + seName);

                    acEnforcer.setPackageManager(getPackageManager());

                    try {
                        if (acEnforcer.hasCertificate(pkg)) {
                            try {
                                mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, true);
                            } catch (RemoteException Re) {
                                Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                            }
                        } else {
                            try {
                                mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, false);
                            } catch (RemoteException Re) {
                                Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                            }
                        }
                    } catch (Exception e) {
                        try {
                            mNxpNfcAdapterExtras.notifyCheckCertResult(pkg, false);
                        } catch (RemoteException Re) {
                            Log.e(_TAG, "notifyCheckCertResult - exception caught" + Re);
                        }
                    }
                    return;
                }
                else if (action.equals(AID_SELECTED) || action.equals(NXP_TRANSACTION_DETECTED)){
                    nfcAdapterExtraActionAidSelected = true;
                    if (action.equals(AID_SELECTED)) {
                        aid = intent.getByteArrayExtra(NFC_EXTRAS_AID);
                        data = intent.getByteArrayExtra(NFC_EXTRAS_DATA);
                        seName = intent.getStringExtra(NFC_EXTRAS_SENAME);
                        if(seName.equals(NXP_UICC_ID)) {
                            seName = "SIM1";
                            intent.putExtra(NFC_EXTRAS_SENAME, seName);
                        }
                    } else if (action.equals(NXP_TRANSACTION_DETECTED)) {
                        if (intent.getStringExtra(NXP_EXTRA_SOURCE).equals(NXP_UICC_ID)) {
                            seName = "SIM1";
                            intent.putExtra(NFC_EXTRAS_SENAME, seName);
                        }
                        aid = intent.getByteArrayExtra(NXP_EXTRA_AID);
                        data = intent.getByteArrayExtra(NXP_EXTRA_DATA);
                        // we need to replace the NXP extras
                        if (aid != null)
                            intent.putExtra(NFC_EXTRAS_AID, aid);
                        if (data != null)
                            intent.putExtra(NFC_EXTRAS_DATA, data);
                        if (seName != null)
                            intent.putExtra(NFC_EXTRAS_SENAME, seName);
                    }
                    if (aid == null) {
                        nfcAdapterExtraActionAidSelected = false;
                        Log.e(_TAG, "got " + action + " without AID");
                        return;
                    } if (data == null) {
                        Log.i(_TAG, "got " + action + " without DATA");
                        /* Temporary workaround to allow TRANSACTION_EVENT with no data */
                    } if (seName == null){
                        Log.d (_TAG, "got " + action + " without seName, we assume it comes from SIM1");
                        seName = "SIM1";
                        intent.putExtra(NFC_EXTRAS_SENAME, seName);
                    }
                    Log.i(_TAG, "got " + action + " for seName " + seName);
                }
                else {
                    Log.i(_TAG, "mNfcEventReceiver got unexpected intent:" + intent.getAction());
                    return;
                }

                try
                {
                    NfcAdapter mNfcAdapter = NfcAdapter.getDefaultAdapter(context);
                    if(mNfcAdapter == null)
                        mNfcAdapter = NfcAdapter.getDefaultAdapter(context);
                    if((mNxpNfcAdapter == null) && (mNfcAdapter != null))
                        mNxpNfcAdapter = NxpNfcAdapter.getNxpNfcAdapter(mNfcAdapter);
                    if(mNxpNfcAdapterExtras == null) {
                        mNxpNfcAdapterExtras = mNxpNfcAdapter.getNxpNfcAdapterExtrasInterface(mNfcAdapter.getNfcAdapterExtrasInterface());
                    }
                    if (mNxpNfcAdapterExtras == null) {
                        Log.i(_TAG, "Couldn't get NfcAdapter");
                        return;
                    }

                    if (nfcAdapterExtraActionRfFieldOn || nfcAdapterExtraActionRfFieldOff) {
                        SmartcardError error = new SmartcardError();
                        String readers[] = null;
                        try {
                            readers = SmartcardService.getSmartcardReaders();
                        } catch (Exception e) {
                            Log.e (_TAG, "Exception caught : " +e);
                        }
                        if (readers == null)
                            return;
                        ITerminal terminal;
                        ISmartcardServiceCallback callback = new ISmartcardServiceCallback.Stub(){};
                        for (int i = 0; i < readers.length; i++){
                            terminal = SmartcardService.getTerminal(readers[i], error);
                            if ((terminal == null)||(!terminal.isCardPresent())) {
                                Log.i(_TAG, "SE:" + readers[i] + " is not present");
                                continue;
                            }

                            Log.i(_TAG, "Checking access rules for RF Field On/Off for " +
                                        readers[i]);

                            // use cached rule without checking refresh tag
                            boolean [] nfcEventAccess = terminal.isNFCEventAllowed(
                                                            getPackageManager(), aid, packageNames,
                                                            false, callback);

                            // RF Field ON/OFF doesn't belong to any SE,
                            // so allow access to NFC Event if any SE allows
                            if (nfcEventAccessFinal == null) {
                                nfcEventAccessFinal = nfcEventAccess;
                            } else {
                                for (int j = 0; j < nfcEventAccess.length; j++) {
                                    if (nfcEventAccess[j] == true) {
                                        nfcEventAccessFinal[j] = true;
                                    }
                                }
                            }
                        }
                    } else if (nfcAdapterExtraActionAidSelected) {
                        SmartcardError error = new SmartcardError();
                        ISmartcardServiceCallback callback = new ISmartcardServiceCallback.Stub(){};
                        ITerminal terminal = SmartcardService.getTerminal(seName, error);
                        if (terminal == null) {
                            Log.i(_TAG, "Couldn't get terminal for " + seName);
                            return;
                        }

                        Log.i(_TAG, "Checking access rules for AID " +  ByteArrayConverter.byteArrayToHexString(aid) + " selected for " + seName);

                        nfcEventAccessFinal = terminal.isNFCEventAllowed(getPackageManager(),
                                                             aid, packageNames, true, callback);
                    }

                    if (nfcEventAccessFinal != null) {
                        synchronized(this) {
                            for (int i = 0; i < nfcEventAccessFinal.length; i++) {
                                if ( (nfcEventAccessFinal[i]) && (nfcAdapterExtraActionAidSelected) ) {

                                    for (PackageInfo pkg : mInstalledGsmaPackages) {
                                        if (pkg.packageName.equals(packageNames[i])) {
                                            intent.setAction(GSMA_TRANSACTION_EVENT);
                                            intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
                                            intent.setPackage(packageNames[i]);
                                            Log.i(_TAG, "sending transaction intent to : " + packageNames[i]);
                                            try {
                                                mNxpNfcAdapterExtras.deliverSeIntent(packageNames[i], intent);
                                            } catch (Exception ignore) {
                                                //ignore
                                            }
                                            break; // exit the loop, as the intent shall be sent only once.
                                        } else {
                                            Log.e(_TAG, pkg.packageName + " is not " + packageNames[i]);
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        Log.i(_TAG, "No NFC Rules detected for " + seName);
                    }
                } catch (Exception e) {
                    Log.e(_TAG, "NFC Event AC Exception: " + e.getMessage() );
                }
            }
        };
        context.registerReceiver(mNfcEventReceiver, intentFilter);
    }

    private void unregisterNfcEvent(Context context) {
        if(mNfcEventReceiver!= null) {
            Log.i(_TAG, "unregister NFC event");
            context.unregisterReceiver(mNfcEventReceiver);
            mNfcEventReceiver = null;
        }
    }

    private void registerGsmaServiceEvent(Context context) {
        Log.i(_TAG, "register GSMA Service event");

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_CHECK_AID);

        mGsmaServiceEventReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();

                // GSMA service asks to check if AIDs are selectable
                if (action.equals(ACTION_CHECK_AID)){
                    String seName = intent.getStringExtra(EXTRA_SE_NAME);

                    SmartcardError error = new SmartcardError();
                    Terminal terminal = (Terminal)SmartcardService.getTerminal(seName, error);
                    if (terminal == null) {
                        Log.i(_TAG, "ACTION_CHECK_AID: Couldn't get terminal for " + seName);
                        //notify GSMA service with empty
                        Intent returnIntent = new Intent();
                        returnIntent.setAction(ACTION_CHECK_AID);
                        returnIntent.putExtra(EXTRA_SE_NAME, seName);
                        returnIntent.putExtra(EXTRA_AIDS, "");
                        returnIntent.setPackage("com.qualcomm.qti.gsma.services.nfc");
                        context.sendBroadcastAsUser(returnIntent, UserHandle.CURRENT);
                        return;
                    }

                    String aidsStringWithComma = intent.getStringExtra(EXTRA_AIDS);
                    String[] aidsString;
                    StringBuilder selectableAids = new StringBuilder();
                    if (aidsStringWithComma != null) {
                        aidsString = aidsStringWithComma.split(",");
                        byte[] aid;
                        for (int i = 0; (aidsString != null)&&(i < aidsString.length); i++) {
                            Log.d(_TAG, "checking selectable at AID[" + i + "] = " +
                                         aidsString[i]);
                            // if prefix AID
                            if (aidsString[i].endsWith("*")) {
                                String prefixAid = aidsString[i].substring(0, aidsString[i].length() - 1);
                                aid = ByteArrayConverter.hexStringToByteArray(prefixAid);
                            } else {
                                aid = ByteArrayConverter.hexStringToByteArray(aidsString[i]);
                            }
                            if (aid.length >= 5) {
                                boolean selectable = terminal.isAidSelectable(aid);
                                if (selectable) {
                                    if (selectableAids.length() > 0)
                                        selectableAids.append(",");
                                    selectableAids.append(aidsString[i]);
                                }
                            }
                        }
                    }
                    //notify GSMA service with selectable AIDs
                    Log.i(_TAG, "selectable AIDs:" + selectableAids.toString());
                    Intent returnIntent = new Intent();
                    returnIntent.setAction(ACTION_CHECK_AID);
                    returnIntent.putExtra(EXTRA_SE_NAME, seName);
                    returnIntent.putExtra(EXTRA_AIDS, selectableAids.toString());
                    returnIntent.setPackage("com.qualcomm.qti.gsma.services.nfc");
                    context.sendBroadcastAsUser(returnIntent, UserHandle.CURRENT);
                }
            }
        };
        context.registerReceiver(mGsmaServiceEventReceiver, intentFilter);
    }

    private void unregisterGsmaServiceEvent(Context context) {
        if(mGsmaServiceEventReceiver!= null) {
            Log.i(_TAG, "unregister GSMA Service event");
            context.unregisterReceiver(mGsmaServiceEventReceiver);
            mGsmaServiceEventReceiver = null;
        }
    }

    public void onDestroy() {

        // Cancel the inialization background task if still running
        if(mInitialiseTask != null) mInitialiseTask.cancel(true);
        mInitialiseTask = null;

        // Unregister all the broadcast receivers

        unregisterNfcEvent(getApplicationContext());
        unregisterGsmaServiceEvent(getApplicationContext());
        unregisterPackageUpdateEvent(getApplicationContext());
        unregisterAdapterStateChangedEvent(getApplicationContext());
        Log.i(_TAG, Thread.currentThread().getName()
                + " ... NfcIntentsService onDestroy");

    }
}

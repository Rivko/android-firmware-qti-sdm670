/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
 */

/*
 * Copyright (C) 2011, The Android Open Source Project
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
/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.service;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyProperties;

import dalvik.system.DexClassLoader;
import dalvik.system.DexFile;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.PrintWriter;
import java.lang.reflect.Constructor;
import java.security.AccessControlException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.Set;
import java.util.TreeMap;
import java.util.Arrays;

import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.service.Channel;
import org.simalliance.openmobileapi.service.Channel.SmartcardServiceChannel;
import org.simalliance.openmobileapi.service.ISmartcardService;
import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.security.AccessControlEnforcer;
import org.simalliance.openmobileapi.service.security.ChannelAccess;
import org.simalliance.openmobileapi.service.Terminal.SmartcardServiceReader;

/**
 * The smartcard service is setup with privileges to access smart card hardware.
 * The service enforces the permission
 * 'org.simalliance.openmobileapi.service.permission.BIND'.
 */
public final class SmartcardService extends Service {

    public static final String _TAG = "SmartcardService";
    public static final String _UICC_TERMINAL = "SIM";
    public static final String _eSE_TERMINAL = "eSE";
    public static final String _SD_TERMINAL = "SD";

    public static String _UICC_TERMINAL_EXT[] = new String[] {"1", "2"};
    public static String _eSE_TERMINAL_EXT[] = new String[] {"1", "2"};
    public static String _SD_TERMINAL_EXT[] = new String[] {"1", "2"};

    public boolean seAcInit = false;
    public static boolean mIsMultiSimEnabled;
    public static String mIsisConfig;

    public static boolean hasStarted = false;
    public boolean[] simReady = new boolean[2];
    public boolean[] simLoaded = new boolean[2];
    public boolean[] simImsi = new boolean[2];
    public static final String simStateChangedAction =
        "org.simalliance.openmobileapi.action.simstate.changed";

    @SuppressWarnings({ "rawtypes" })
    static void setError(SmartcardError error, Class clazz, String message) {
        if (error != null) {
            error.setError(clazz, message);
        }
    }

    static void setError(SmartcardError error, Exception e) {
        if (error != null) {
            error.setError(e.getClass(), e.getMessage());
        }
    }

    /**
     * For now this list is setup in onCreate(), not changed later and therefore
     * not synchronized.
     */
    private static Map<String, ITerminal> mTerminals = new TreeMap<String, ITerminal>();

    /**
     * For now this list is setup in onCreate(), not changed later and therefore
     * not synchronized.
     */
    private static Map<String, ITerminal> mAddOnTerminals = new TreeMap<String, ITerminal>();

    /* Broadcast receivers */
    private BroadcastReceiver mSimReceiver;

    /* Async task */
    InitialiseTask mInitialiseTask;

    /**
     * ServiceHandler use to load rules from the terminal
     */
    private static ServiceHandler mServiceHandler;
    private TelephonyManager telManager = null;

    public static String readers[];

    public SmartcardService() {
        super();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(_TAG, Thread.currentThread().getName()
                        + " smartcard service onBind");
        if (ISmartcardService.class.getName().equals(intent.getAction())) {
            return mSmartcardBinder;
        }
        return null;
    }

    @Override
    public void onCreate() {
        Log.i(_TAG, Thread.currentThread().getName()
                + " smartcard service onCreate");

        final Context context = getApplicationContext();

        // Start up the thread running the service.  Note that we create a
        // separate thread because the service normally runs in the process's
        // main thread, which we don't want to block.  We also make it
        // background priority so CPU-intensive work will not disrupt our UI.
        HandlerThread thread = new HandlerThread("SmartCardServiceHandler");
        thread.start();
        hasStarted = true;
        // Get the HandlerThread's Looper and use it for our Handler
        mServiceHandler = new ServiceHandler(thread.getLooper());

        String multiSimConfig = SystemProperties.get(TelephonyProperties.PROPERTY_MULTI_SIM_CONFIG);
        Log.i(_TAG, "multiSimConfig = " + multiSimConfig);

        mIsMultiSimEnabled = (multiSimConfig.equals("dsds") ||
                              multiSimConfig.equals("dsda") ||
                              multiSimConfig.equals("tsts"));

        mIsisConfig = SystemProperties.get("persist.nfc.smartcard.isis");
        if(mIsisConfig == null || mIsisConfig.equals("")) {
            mIsisConfig = "none";
        }
        Log.i(_TAG, "mIsisConfig = " + mIsisConfig);
        createTerminals();
        mInitialiseTask = new InitialiseTask();
        mInitialiseTask.execute();
    }

    @Override
    public void dump(FileDescriptor fd, PrintWriter writer, String[] args) {
        writer.println("SMARTCARD SERVICE (dumpsys activity service org.simalliance.openmobileapi)");
        writer.println();

        String prefix = "  ";

        if(!Build.IS_DEBUGGABLE) {
            writer.println(prefix + "Your build is not debuggable!");
            writer.println(prefix + "Smartcard service dump is only available for userdebug and eng build");
        } else {
            writer.println(prefix + "List of terminals:");
            for (ITerminal terminal : mTerminals.values()) {
               writer.println(prefix + "  " + terminal.getName());
            }
            writer.println();

            writer.println(prefix + "List of add-on terminals:");
            for (ITerminal terminal : mAddOnTerminals.values()) {
               writer.println(prefix + "  " + terminal.getName());
            }
            writer.println();

            for (ITerminal terminal : mTerminals.values()) {
               terminal.dump(writer, prefix);
            }
            for (ITerminal terminal : mAddOnTerminals.values()) {
               terminal.dump(writer, prefix);
            }
        }
    }


    private class InitialiseTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected Void doInBackground(Void... arg0) {

            try {
                seAcInit = initializeAccessControl(null, null);
            } catch( Exception e ) {
                // do nothing since this is called were nobody can react.
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            Log.i(_TAG, "OnPostExecute()");
            registerSimStateEvents(getApplicationContext());
            mInitialiseTask = null;
        }
    }

    private void registerSimStateEvents(Context context) {
        Log.i(_TAG, "register SIM events");

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(simStateChangedAction);

        mSimReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String state = null;
                int slot = 0;
                String action = intent.getAction();
                if (action.equals(simStateChangedAction)){
                    state = intent.getStringExtra("SimState");
                    slot = intent.getIntExtra("SimSlot", 0);
                    Log.i(_TAG, "sim_state changed : " + state + " on sub " + slot);
                    if (IccCardConstants.INTENT_VALUE_ICC_ABSENT.equals(state)
                            || state.equals("NOT_READY")) {
                        simImsi[slot] = simLoaded[slot] = simReady [slot] = seAcInit = false;
                    }

                    if (IccCardConstants.INTENT_VALUE_ICC_IMSI.equals(state))
                        simImsi[slot] = true;
                    if (IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(state))
                        simLoaded[slot] = true;
                    if (IccCardConstants.INTENT_VALUE_ICC_READY.equals(state))
                        simReady[slot] = true;

                    if (simReady[slot] && simImsi[slot] && simLoaded[slot] && !seAcInit) {
                        Log.i(_TAG, "SIM LOADED, READY & IMSI OK . Checking access rules for updates.");
                        try {
                           mServiceHandler.sendMessage(MSG_LOAD_UICC_RULES, 0);
                       } catch (Exception e) {
                           Log.e(_TAG, "Got exception:" + e);
                       }
                    }
                }
            }
        };
        context.registerReceiver(mSimReceiver, intentFilter);
    }

    private void unregisterSimStateEvents(Context context) {
        if(mSimReceiver!= null) {
            Log.i(_TAG, "unregister SIM_STATE_CHANGED event");
            context.unregisterReceiver(mSimReceiver);
            mSimReceiver = null;
        }
    }

    public static void loadEseRules(){
        mServiceHandler.sendMessage(MSG_LOAD_ESE_RULES, 5);
    }

    /**
     * Initalizes Access Control.
     * At least the refresh tag is read and if it differs to the previous one (e.g. is null) the all
     * access rules are read.
     *
     * @param se
     */
    public boolean initializeAccessControl(String se, ISmartcardServiceCallback callback ) {
        return initializeAccessControl(false, se, callback);
    }

    public synchronized boolean initializeAccessControl(boolean reset, String se, ISmartcardServiceCallback callback ) {
        boolean result = true;
        boolean fullAccess = SystemProperties.get("persist.service.seek").equals("fullaccess");
        Log.i(_TAG, "Initializing Access Control");

        if( callback == null ) {
            callback = new ISmartcardServiceCallback.Stub(){};
        }

        Collection<ITerminal>col = mTerminals.values();
        Iterator<ITerminal> iter = col.iterator();
        while(iter.hasNext()){
            ITerminal terminal = iter.next();
            if( terminal == null ){
                continue;
            }

            if( se == null || terminal.getName().startsWith(se)) {
                boolean isCardPresent = false;
                try {
                    if (terminal.getName().equals("eSE1") && fullAccess)
                        isCardPresent = true;
                    else
                        isCardPresent = terminal.isCardPresent();
                } catch (CardException e) {
                    isCardPresent = false;
                }
                if ( ((terminal.getName().equals("SIM1")) && (telManager.getDefault().getSimState() == TelephonyManager.SIM_STATE_READY))
                    || ((terminal.getName().equals("SIM2")) && (telManager.getDefault().getSimState(2) == TelephonyManager.SIM_STATE_READY))) {
                    Log.i(_TAG, "Initializing Access Control for " + terminal.getName());
                    if(reset) terminal.resetAccessControl();
                    result &= terminal.initializeAccessControl(true, callback);
                } else if (isCardPresent) {
                    Log.i(_TAG, "Initializing Access Control for " + terminal.getName());
                    if(reset) terminal.resetAccessControl();
                    result &= terminal.initializeAccessControl(true, callback);
                } else {
                    Log.i(_TAG, "NOT initializing Access Control for " + terminal.getName() + " SE not present.");
                }
            }
        }
        col = this.mAddOnTerminals.values();
        iter = col.iterator();
        while(iter.hasNext()){
            ITerminal terminal = iter.next();
            if( terminal == null ){

                continue;
            }

            if( se == null || terminal.getName().startsWith(se)) {
                boolean isCardPresent = false;
                try {
                    isCardPresent = terminal.isCardPresent();
                } catch (CardException e) {
                    isCardPresent = false;

                }

                if(isCardPresent) {
                    Log.i(_TAG, "Initializing Access Control for " + terminal.getName());
                    if(reset) terminal.resetAccessControl();
                    result &= terminal.initializeAccessControl(true, callback);
                } else {
                    Log.i(_TAG, "NOT initializing Access Control for " + terminal.getName() + " SE not present.");
                }
            }
        }

        /**
         * In case the UICC is not yet ready (PIN_REQUIRED state), we still want the eSE to be initialized
         * the UICC will get initialized once it will be ready, thanks the sim_state event.
         */
        if (SystemProperties.get(TelephonyProperties.PROPERTY_SIM_STATE).equals("PIN_REQUIRED"))
            result = false;
        return result;
    }

    public void onDestroy() {
        Log.i(_TAG, " smartcard service onDestroy ...");
        for (ITerminal terminal : mTerminals.values())
            terminal.closeChannels();
        for (ITerminal terminal : mAddOnTerminals.values())
            terminal.closeChannels();

        // Cancel the inialization background task if still running
        if(mInitialiseTask != null) mInitialiseTask.cancel(true);
        mInitialiseTask = null;
        unregisterSimStateEvents(getApplicationContext());
        mServiceHandler = null;

        Log.i(_TAG, Thread.currentThread().getName()
                + " ... smartcard service onDestroy");

    }

    public static ITerminal getTerminal(String reader, SmartcardError error) {
        if (reader == null) {
            setError(error, NullPointerException.class, "reader must not be null");
            return null;
        }

        if (reader.equals("SIM"))
            reader = "SIM1";

        ITerminal terminal = mTerminals.get(reader);
        if (terminal == null) {
            if (!mIsisConfig.equals("none")) {
                if(reader.equals("SIM1")) {
                    terminal = mTerminals.get("SIM" + _UICC_TERMINAL_EXT[0]);
                }
            }
        }
        if (terminal == null) {
            terminal = mAddOnTerminals.get(reader);
            if (terminal == null) {
                setError(error, IllegalArgumentException.class, "unknown reader");
            }
        }
        return terminal;
    }

    private String[] createTerminals() {
        createBuildinTerminals();

        Set<String> names = mTerminals.keySet();
        ArrayList<String> list = new ArrayList<String>(names);
        Collections.sort(list);

        // set UICC on the top , SIM1(or SIM - UICC)/SIM2 and then eSE1/eSE2
        if(list.remove(_eSE_TERMINAL + _eSE_TERMINAL_EXT[1]))
            list.add(0, _eSE_TERMINAL + _eSE_TERMINAL_EXT[1]);
        if(list.remove(_eSE_TERMINAL + _eSE_TERMINAL_EXT[0]))
            list.add(0, _eSE_TERMINAL + _eSE_TERMINAL_EXT[0]);
        if(list.remove(_UICC_TERMINAL + _UICC_TERMINAL_EXT[1]))
            list.add(0, _UICC_TERMINAL + _UICC_TERMINAL_EXT[1]);
        if(list.remove(_UICC_TERMINAL + _UICC_TERMINAL_EXT[0]))
            list.add(0, _UICC_TERMINAL + _UICC_TERMINAL_EXT[0]);

        if (mIsisConfig.equals("none")) {
            createAddonTerminals();
            names = mAddOnTerminals.keySet();
            for (String name : names) {
                if (!list.contains(name)) {
                    list.add(name);
                }
            }
        }

        return list.toArray(new String[list.size()]);
    }

    public String[] updateTerminals() {
        Set<String> names = mTerminals.keySet();
        ArrayList<String> list = new ArrayList<String>(names);
        Collections.sort(list);

        // set UICC on the top , SIM1(or SIM - UICC)/SIM2 and then eSE1/eSE2
        if(list.remove(_eSE_TERMINAL + _eSE_TERMINAL_EXT[1]))
            list.add(0, _eSE_TERMINAL + _eSE_TERMINAL_EXT[1]);
        if(list.remove(_eSE_TERMINAL + _eSE_TERMINAL_EXT[0]))
            list.add(0, _eSE_TERMINAL + _eSE_TERMINAL_EXT[0]);
        if(list.remove(_UICC_TERMINAL + _UICC_TERMINAL_EXT[1]))
            list.add(0, _UICC_TERMINAL + _UICC_TERMINAL_EXT[1]);
        if(list.remove(_UICC_TERMINAL + _UICC_TERMINAL_EXT[0]))
            list.add(0, _UICC_TERMINAL + _UICC_TERMINAL_EXT[0]);

        if (mIsisConfig.equals("none")) {
            updateAddonTerminals();
            names = mAddOnTerminals.keySet();
            for (String name : names) {
                if (!list.contains(name)) {
                    list.add(name);
                }
            }
        }

        return list.toArray(new String[list.size()]);
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    private void createBuildinTerminals() {
        Class[] types = new Class[] {
            Context.class, int.class
        };
        Object[] args = new Object[] {
            this, 0
        };
        Object[] classes = getBuildinTerminalClasses();

        String smartcardConfig = SystemProperties.get("persist.nfc.smartcard.config");
        if(smartcardConfig == null || smartcardConfig.equals("")) {
        //Let's Add sim1, sim 2 and esE1 by default.
            smartcardConfig = "SIM1,SIM2";
        }
        Log.i(_TAG, "smartcardConfig = " + smartcardConfig);
        String[] terminals = smartcardConfig.split(",");
        int numUiccTerminal = 0;
        int numeSETerminal = 0;
        int numASSDTerminal = 0;
        for (int i = 0; i < terminals.length; i++) {
            if (terminals[i].startsWith("SIM")) {
                if (numUiccTerminal < 2) {
                    _UICC_TERMINAL_EXT[numUiccTerminal] = terminals[i].substring(3);
                    numUiccTerminal++;
                }
            }
            else if (terminals[i].startsWith("eSE")) {
                if (numeSETerminal < 2) {
                    _eSE_TERMINAL_EXT[numeSETerminal] = terminals[i].substring(3);
                    numeSETerminal++;
                }
            }
            else if (terminals[i].startsWith("SD")) {
                if (numASSDTerminal < 2) {
                    _SD_TERMINAL_EXT[numASSDTerminal] = terminals[i].substring(2);
                    numASSDTerminal++;
                }
            }
        }

        if ((!mIsMultiSimEnabled) && (numUiccTerminal > 1))
            numUiccTerminal = 1;

        for (Object clazzO : classes) {
            try {
                Class clazz = (Class) clazzO;
                Constructor constr = clazz.getDeclaredConstructor(types);

                int numSlots;
                if (constr.getName().contains("UiccTerminal")) {
                    numSlots = numUiccTerminal;
                } else if (constr.getName().contains("eSETerminal")) {
                    numSlots = numeSETerminal;
                } else if (constr.getName().contains("ASSDTerminal")) {
                    numSlots = numASSDTerminal;
                } else {
                    numSlots = 1;
                }
                for (int slot = 0; slot < numSlots; slot++) {
                    args[1] = slot;
                    ITerminal terminal = (ITerminal) constr.newInstance(args);
                    mTerminals.put(terminal.getName(), terminal);
                    Log.i(_TAG, Thread.currentThread().getName() + " adding "
                            + terminal.getName());
                }
            } catch (Throwable t) {
                Log.e(_TAG, Thread.currentThread().getName()
                        + " CreateReaders Error: "
                        + ((t.getMessage() != null) ? t.getMessage() : "unknown"));
            }
        }
    }

    private void createAddonTerminals() {
        String[] packageNames = AddonTerminal.getPackageNames(this);
        for (String packageName : packageNames) {
            try {
                String apkName = getPackageManager().getApplicationInfo(packageName, 0).sourceDir;
                DexFile dexFile = new DexFile(apkName);
                Enumeration<String> classFileNames = dexFile.entries();
                while (classFileNames.hasMoreElements()) {
                    String className = classFileNames.nextElement();
                    if (className.endsWith("Terminal")) {
                        ITerminal terminal = new AddonTerminal(this, packageName, className);
                        mAddOnTerminals.put(terminal.getName(), terminal);
                        Log.i(_TAG, Thread.currentThread().getName() + " adding "
                                + terminal.getName());
                    }
                }
            } catch (Throwable t) {
                Log.e(_TAG, Thread.currentThread().getName()
                        + " CreateReaders Error: "
                        + ((t.getMessage() != null) ? t.getMessage() : "unknown"));
            }
        }
    }

    private void updateAddonTerminals() {
        Set<String> names = mAddOnTerminals.keySet();
        ArrayList<String> namesToRemove = new ArrayList<String>();
        for (String name : names) {
            ITerminal terminal = mAddOnTerminals.get(name);
            if (!terminal.isConnected()) {
                namesToRemove.add(terminal.getName());
            }
        }
        for (String name : namesToRemove) {
            mAddOnTerminals.remove(name);
        }

        String[] packageNames = AddonTerminal.getPackageNames(this);
        for (String packageName : packageNames) {
            try {
                String apkName = getPackageManager().getApplicationInfo(packageName, 0).sourceDir;
                DexFile dexFile = new DexFile(apkName);
                Enumeration<String> classFileNames = dexFile.entries();
                while (classFileNames.hasMoreElements()) {
                    String className = classFileNames.nextElement();
                    if (className.endsWith("Terminal")) {
                        ITerminal terminal = new AddonTerminal(this, packageName, className);
                        if (!mAddOnTerminals.containsKey(terminal.getName())) {
                            mAddOnTerminals.put(terminal.getName(), terminal);
                            Log.i(_TAG, Thread.currentThread().getName()
                                    + " adding " + terminal.getName());
                        }
                    }
                }

            } catch (Throwable t) {
                Log.e(_TAG, Thread.currentThread().getName()
                        + " CreateReaders Error: "
                        + ((t.getMessage() != null) ? t.getMessage() : "unknown"));
            }
        }
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    private Object[] getBuildinTerminalClasses() {
        ArrayList classes = new ArrayList();
        try {
            String packageName = "org.simalliance.openmobileapi.service";
            String apkName = getPackageManager().getApplicationInfo(packageName, 0).sourceDir;
            DexClassLoader dexClassLoader = new DexClassLoader(apkName, getApplicationContext().getFilesDir().getAbsolutePath(), null, getClass()
                    .getClassLoader());

            Class terminalClass = Class.forName("org.simalliance.openmobileapi.service.Terminal", true, dexClassLoader);
            if (terminalClass == null) {
                return classes.toArray();
            }

            DexFile dexFile = new DexFile(apkName);
            Enumeration<String> classFileNames = dexFile.entries();
            while (classFileNames.hasMoreElements()) {
                String className = classFileNames.nextElement();
                Class clazz = Class.forName(className);
                Class superClass = clazz.getSuperclass();
                if (superClass != null && superClass.equals(terminalClass)
                        && !className.equals("org.simalliance.openmobileapi.service.AddonTerminal")) {
                    classes.add(clazz);
                }
            }
        } catch (Throwable exp) {
            // nothing to to
        }
        return classes.toArray();
    }

    /**
     * Get package name from the user id.
     *
     * This shall fix the problem the issue that process name != package name
     * due to anndroid:process attribute in manifest file.
     *
     * But this call is not really secure either since a uid can be shared between one
     * and more apks
     *
     * @param uid
     * @return The first package name associated with this uid.
     */
    public String getPackageNameFromCallingUid(int uid ){
       PackageManager packageManager = getPackageManager();
       if(packageManager != null){
               String packageName[] = packageManager.getPackagesForUid(uid);
               if( packageName != null && packageName.length > 0 ){
                       return packageName[0];
               }
       }
       throw new AccessControlException("Caller PackageName can not be determined");
    }

    public static String[] getSmartcardReaders () {
        return readers;
    }
    /**
     * The smartcard service interface implementation.
     */
    private final ISmartcardService.Stub mSmartcardBinder = new ISmartcardService.Stub() {

        @Override
        public String[] getReaders() throws RemoteException {
            Log.i(_TAG, "getReaders()");
            return updateTerminals();
        }

        @Override
        public ISmartcardServiceReader getReader(String reader,
                SmartcardError error) throws RemoteException {
            Terminal terminal = (Terminal)getTerminal(reader, error);
            if( terminal != null ){
                return terminal.new SmartcardServiceReader(SmartcardService.this);
            }
            Log.e(_TAG, "getReader(): setError IllegalArgumentException");
            setError(error, IllegalArgumentException.class, "invalid reader name");
            return null;
        }


        @Override
        public synchronized boolean[] isNFCEventAllowed(
                String reader,
                byte[] aid,
                String[] packageNames,
                ISmartcardServiceCallback callback,
                SmartcardError error)
                        throws RemoteException
        {
            try
            {
                if (callback == null) {
                    setError(error, NullPointerException.class, "callback must not be null");
                    return null;
                }
                ITerminal terminal = getTerminal(reader, error);
                if (terminal == null) {
                    return null;
                }
                if (aid == null || aid.length == 0) {
                    aid = new byte[] {
                            0x00, 0x00, 0x00, 0x00, 0x00
                    };
                }
                if (aid.length < 5 || aid.length > 16) {
                     setError(error, IllegalArgumentException.class, "AID out of range");
                     return null;
                }
                if (packageNames == null || packageNames.length == 0) {
                     setError(error, IllegalArgumentException.class, "process names not specified");
                     return null;
                }
                AccessControlEnforcer ac = null;
                if( terminal.getAccessControlEnforcer() == null ) {
                    ac = new AccessControlEnforcer( terminal );
                } else {
                    ac = terminal.getAccessControlEnforcer();
                }
                ac.setPackageManager(getPackageManager());
                ac.initialize(true, callback);
                return ac.isNFCEventAllowed(aid, packageNames, true, callback );
            } catch (Exception e) {
                setError(error, e);
                Log.i(_TAG, "isNFCEventAllowed Exception: " + e.getMessage() );
                return null;
            }
        }
    };

    /**
     * The smartcard service interface implementation.
     */
    final class SmartcardServiceSession extends ISmartcardServiceSession.Stub {

        private final SmartcardServiceReader mReader;
        /** List of open channels in use of by this client. */
        private final Set<Channel> mChannels = new HashSet<Channel>();

        private final Object mLock = new Object();

        private boolean mIsClosed;

        public int mHandle;

        private byte[] mAtr;

        public SmartcardServiceSession(SmartcardServiceReader reader){
            mReader = reader;
            mAtr = mReader.getAtr();
            mIsClosed = false;
            mHandle = 0;
        }

        public SmartcardServiceSession(SmartcardServiceReader reader, int shandle){
            mReader = reader;
            mAtr = mReader.getAtr();
            mIsClosed = false;
            mHandle = shandle;
            Log.i(_TAG, "SmartcardServiceSession Constructor - handle " + Integer.toString(mHandle) );
        }

        public ISmartcardServiceReader getReader() throws RemoteException {
            return mReader;
        }

        @Override
        public int getHandle() throws RemoteException {
            Log.i(_TAG, "SmartcardServiceSession  - getHandle : " + Integer.toString(mHandle) );
            return mHandle;
        }

        @Override
        public void setHandle(int handle) {
            mHandle = handle;
        }
        @Override
        public byte[] getAtr() throws RemoteException {
            return mAtr;
        }

        @Override
        public void close(SmartcardError error) throws RemoteException {
            if (mReader == null) {
                return;
            }
            try {
                mReader.closeSession(this);
            } catch (CardException | IOException e) {
                setError(error,e);
            }
        }

        @Override
        public void closeChannels(SmartcardError error) throws RemoteException {
            synchronized (mLock) {

                Iterator<Channel>iter = mChannels.iterator();
                try {
                    while(iter.hasNext()) {
                        Channel channel = iter.next();
                        if (channel != null && !channel.isClosed()) {
                            try {
                                channel.close();
                                // close changes indirectly mChannels, so we need a new iterator.
                                iter = mChannels.iterator();
                            } catch (Exception ignore) {
                                Log.e(_TAG, "ServiceSession channel - close Exception " + ignore.getMessage());
                            }
                            channel.setClosed();
                        }
                    }
                    mChannels.clear();
                } catch( Exception e ) {
                    Log.e(_TAG, "ServiceSession closeChannels Exception " + e.getMessage());
                }
            }
        }

        @Override
        public boolean isClosed() throws RemoteException {

            return mIsClosed;
        }

        @Override
        public ISmartcardServiceChannel openBasicChannel(byte[] aid, byte p2,
                ISmartcardServiceCallback callback, SmartcardError error)
                throws RemoteException {

            Channel channel = null;
            boolean noAid = false;

            if (mReader.getTerminal().getName().startsWith(_UICC_TERMINAL)) {
                Log.e(_TAG, "OpenBasicChannel(AID): not allowed for UICC");
                // OpenBasicChannel shall always return null w/o security exception.
                return null;
            } else if (isClosed()) {
                setError( error, IllegalStateException.class, "session is closed");
                return null;
            } else if (callback == null) {
                setError(error, NullPointerException.class, "callback must not be null");
                return null;
            } else if (mReader == null) {
                setError(error, NullPointerException.class, "reader must not be null");
                return null;
            } else if ((p2 != 0x00) && (p2 !=0x04) && (p2 != 0x08) && (p2 !=(byte)0x0C)) {
                setError(error, UnsupportedOperationException.class, "p2 not supported: " + String.format("%02x ", p2 & 0xFF));
                return null;
            } else if (aid == null || aid.length == 0) {
                aid = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00};
                noAid = true;
            } else if (aid.length < 5 || aid.length > 16) {
                setError(error, IllegalArgumentException.class, "AID out of range");
                return null;
            }

            try {

                String packageName = getPackageNameFromCallingUid( Binder.getCallingUid());
                Log.i(_TAG, "Enable access control on basic channel for " + packageName);
                ChannelAccess channelAccess = mReader.getTerminal().setUpChannelAccess(
                        getPackageManager(),
                        aid,
                        packageName,
                        true,
                        callback );
                Log.i(_TAG, "Access control successfully enabled.");

                channelAccess.setCallingPid(Binder.getCallingPid());

                if (noAid) {
                    channel = mReader.getTerminal().openBasicChannel(this, callback);
                } else {
                    channel = mReader.getTerminal().openBasicChannel(this, aid, callback);
                }

                if (channel == null) {
                    Log.i(_TAG, "OpenBasicChannel(AID) - returning null .");
                    return null;
                }

                if (!mIsisConfig.equals("none")) {
                    /* check if the same AID has been selected */
                    byte[] selectResponse = mReader.getTerminal().getSelectResponse();
                    byte[] selectedAid = mReader.getTerminal().getSelectedAid(selectResponse);

                    if (selectedAid == null) {
                        // assume the same AID is selected
                        Log.i(_TAG, "Cannot find selected AID");
                    } else if (!Arrays.equals(aid, selectedAid)) {
                        Log.i(_TAG, "Different AID is selected!!!");
                        Log.i(_TAG, "Enable access control on basic channel for " + packageName);
                        channelAccess = mReader.getTerminal().setUpChannelAccess(
                                null,
                                selectedAid,
                                packageName,
                                false,
                                callback );
                        Log.i(_TAG, "Access control successfully enabled.");
                        channelAccess.setCallingPid(Binder.getCallingPid());
                    }
                }

                channel.setChannelAccess(channelAccess);

                Log.i(_TAG, "Open basic channel success. Channel: " + channel.getChannelNumber() );

                SmartcardServiceChannel basicChannel = channel.new SmartcardServiceChannel(this);
                mChannels.add(channel);
                return basicChannel;

            } catch (Exception e) {
                if ((channel != null) && !(e instanceof MissingResourceException)) {
                    try {
                        mReader.getTerminal().closeChannel(channel);
                    } catch (Exception ignore) {
                    }
                }
                setError(error, e);
                Log.i(_TAG, "OpenBasicChannel Exception: " + e.getMessage());
                return null;
            }
        }

        @Override
        public ISmartcardServiceChannel openLogicalChannel(byte[] aid, byte p2,
                ISmartcardServiceCallback callback, SmartcardError error)
                throws RemoteException {

            Channel channel = null;
            boolean noAid = false;

            if (isClosed()) {
                setError( error, IllegalStateException.class, "session is closed");
                return null;
            } else if (callback == null) {
                setError(error, NullPointerException.class, "callback must not be null");
                return null;
            } else if (mReader == null) {
                setError(error, NullPointerException.class, "reader must not be null");
                return null;
            } else if ((p2 != 0x00) && (p2 !=0x04) && (p2 != 0x08) && (p2 !=(byte)0x0C)) {
                setError(error, UnsupportedOperationException.class, "p2 not supported" + String.format("%02x ", p2 & 0xFF));
                return null;
            } else if (aid == null || aid.length == 0) {
                aid = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00};
                noAid = true;
            } else if (aid.length < 5 || aid.length > 16) {
                setError(error, IllegalArgumentException.class, "AID out of range");
                return null;
            }

            try {

                String packageName = getPackageNameFromCallingUid( Binder.getCallingUid());
                Log.i(_TAG, "Enable access control on logical channel for " + packageName);
                ChannelAccess channelAccess = mReader.getTerminal().setUpChannelAccess(
                        getPackageManager(),
                        aid,
                        packageName,
                        true,
                        callback );
                Log.i(_TAG, "Access control successfully enabled.");
               channelAccess.setCallingPid(Binder.getCallingPid());


                Log.i(_TAG, "OpenLogicalChannel");
                if (noAid) {
                    channel = mReader.getTerminal().openLogicalChannel(this, callback);
                } else {
                    channel = mReader.getTerminal().openLogicalChannel(this, aid, p2, callback);
                }

                if (channel == null) {
                    Log.i(_TAG, "OpenLogicalChannel(AID) - returning null .");
                    return null;
                }

                if (!mIsisConfig.equals("none")) {
                    /* check if the same AID has been selected */
                    byte[] selectResponse = mReader.getTerminal().getSelectResponse();
                    byte[] selectedAid = mReader.getTerminal().getSelectedAid(selectResponse);

                    if (selectedAid == null) {
                        // assume the same AID is selected
                        Log.i(_TAG, "Cannot find selected AID");
                    } else if (!Arrays.equals(aid, selectedAid)) {
                        Log.i(_TAG, "Different AID is selected!!!");
                        Log.i(_TAG, "Enable access control on logical channel for " + packageName);
                        channelAccess = mReader.getTerminal().setUpChannelAccess(
                                null,
                                selectedAid,
                                packageName,
                                false,
                                callback );
                        Log.i(_TAG, "Access control successfully enabled.");
                        channelAccess.setCallingPid(Binder.getCallingPid());
                        channel.hasSelectedAid(true, selectedAid);
                    }
                }
                channel.setChannelAccess(channelAccess);

                Log.i(_TAG, "Open logical channel successfull. Channel: " + channel.getChannelNumber());
                SmartcardServiceChannel logicalChannel = channel.new SmartcardServiceChannel(this);
                mChannels.add(channel);
                return logicalChannel;
            } catch (Exception e) {
                if (channel != null) {
                    try {
                        mReader.getTerminal().closeChannel(channel);
                    } catch (Exception ignore) {}
                }
                setError(error, e);
                Log.i(_TAG, "OpenLogicalChannel Exception: " + e.getMessage());
                return null;
            }
        }

        void setClosed(){
            mIsClosed = true;

        }

        /**
         * Closes the specified channel. <br>
         * After calling this method the session can not be used for the
         * communication with the secure element any more.
         *
         * @param hChannel the channel handle obtained by an open channel command.
         */
        void removeChannel(Channel channel) {
            if (channel == null) {
                return;
            }
            mChannels.remove(channel);
        }
    }

    /*
     * Handler Thread used to load and initiate ChannelAccess condition
     */
    public final static int MSG_LOAD_UICC_RULES  = 1;
    public final static int MSG_LOAD_ESE_RULES   = 2;
    public final static int MSG_LOAD_SD_RULES    = 3;

    public final static int NUMBER_OF_TRIALS      = 3;
    public final static long WAIT_TIME            = 1000;

    private final class ServiceHandler extends Handler {

        @SuppressLint("HandlerLeak")
        public ServiceHandler(Looper looper) {
            super(looper);
        }

        public void sendMessage(int what, int nbTries) {
           mServiceHandler.removeMessages(what);
           Message newMsg = mServiceHandler.obtainMessage(what, nbTries, 0);
           mServiceHandler.sendMessage(newMsg);
        }

        @Override
        public void handleMessage(Message msg) {
           boolean result = true;

           Log.i(_TAG, "Handle msg: what=" + msg.what + " nbTries=" + msg.arg1);

           switch(msg.what) {
           case MSG_LOAD_UICC_RULES:
               try {
                   result = initializeAccessControl(true, _UICC_TERMINAL, null );
               } catch (Exception e) {
                   Log.e(_TAG, "Got exception:" + e);
               }
               break;

           case MSG_LOAD_ESE_RULES:
               try {
                   result = initializeAccessControl(true, _eSE_TERMINAL, null );
               } catch (Exception e) {
                   Log.e(_TAG, "Got exception:" + e);
               }
               break;

           case MSG_LOAD_SD_RULES:
               try {
                   result = initializeAccessControl(true, _SD_TERMINAL, null );
               } catch (Exception e) {
                   Log.e(_TAG, "Got exception:" + e);
               }
               break;
           }

           if(!result && msg.arg1 > 0) {
               // Try to re-post the message
               Log.e(_TAG, "Fail to load rules: Let's try another time (" + msg.arg1 + " remaining attempt");
               Message newMsg = mServiceHandler.obtainMessage(msg.what, msg.arg1 - 1, 0);
               mServiceHandler.sendMessageDelayed(newMsg, WAIT_TIME);
           }
        }
    }
}

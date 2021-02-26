/**
 * Copyright (c) 2014, Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.loadcarrier;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.SystemProperties;
import android.util.Log;

import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.android.internal.os.RegionalizationEnvironment;
import com.android.internal.os.IRegionalizationService;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;
import java.io.File;
import java.io.IOException;

public class TriggerService extends Service implements TriggerManager.OnTriggerCompleteListener {
    private static final String TAG = "TriggerService";

    // Save the matched carriers info.
    private HashMap<String, HashMap<String, String>> mMatchedCarriers
            = new HashMap<String, HashMap<String, String>>();

    private static int mTargetSlot = Utils.INVALID_SLOT_ID;
	private IRegionalizationService mRegionalizationService = null;

    // The action and extra used to start the trigger activity.
    private static final String ACTION_TRIGGER = "com.qualcomm.qti.carrierconfigure.trigger";
    private static final String EXTRA_PATH_LIST = "trigger_path_list";

    private static boolean isRunning = false;

    private ICarrierAccessCacheService mAccessCacheService = null;
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            if (Utils.DEBUG) Log.i(TAG, "Service Connected to " + name.getShortClassName());
            if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = ICarrierAccessCacheService.Stub.asInterface(service);
                if (isRunning) {
                    if (Utils.DEBUG) Log.i(TAG, "TriggerService is running.");
                } else {
                    if (Utils.DEBUG) Log.i(TAG, "Start the TriggerService to find the carriers.");
                    isRunning = true;
                    for (Trigger trigger : TriggerManager.getInstance(TriggerService.this,
                            mAccessCacheService).getActiveTriggerList()) {
                        trigger.startTrigger();
                    }
                }
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            if (Utils.DEBUG) Log.i(TAG, "Service disconnected to " + name.getShortClassName());
            if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = null;
            }
        }
    };
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        if (mServiceConnection != null) {
            if (mAccessCacheService == null) {
                // Bind the service to access cache dir.
                Intent intent = new Intent(ICarrierAccessCacheService.class.getName());
                intent.setPackage(ICarrierAccessCacheService.class.getPackage().getName());
                bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
        }

    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
        if (intent != null) {
            mTargetSlot = intent.getIntExtra(Utils.INTENT_CARRIER_TARGET_SLOT,
                    Utils.INVALID_SLOT_ID);
        }
        if (Utils.DEBUG) Log.i(TAG, "onStart mTargetSlot=" + mTargetSlot);

        if ((mAccessCacheService != null) && (!isRunning)) {
            if (Utils.DEBUG) Log.i(TAG, "Start the TriggerService to find the carriers.");
            isRunning = true;
            for (Trigger trigger : TriggerManager.getInstance(TriggerService.this,
                    mAccessCacheService).getActiveTriggerList()) {
                trigger.startTrigger();
            }
        }
    }

    private Intent buildStartTriggerIntent() {
        Intent startTriggerIntent = new Intent();
        startTriggerIntent.setAction(ACTION_TRIGGER);

        Bundle bundle = new Bundle();
        ArrayList<String> pathList = new ArrayList<String>();
        Iterator<Entry<String, HashMap<String, String>>> iterator = mMatchedCarriers
                .entrySet().iterator();
        while (iterator.hasNext()) {
            Entry<String, HashMap<String, String>> entry = iterator.next();
            Iterator<Entry<String, String>> iterator_carrier =
                    entry.getValue().entrySet().iterator();
            while (iterator_carrier.hasNext()) {
                Entry<String, String> entry_carrier = iterator_carrier.next();
                String path = entry_carrier.getKey();
                String contents = entry_carrier.getValue();
                if (Utils.DEBUG) Log.d(TAG, "Path:" + path + " Contents:" + contents);
                bundle.putString(path, contents);
                pathList.add(path);
            }
        }
        //String deviceType = SystemProperties.get("persist.device.type");
        String deviceType = "omt";
        ArrayList<String> contents = null;
        if (RegionalizationEnvironment.isSupported()) {
            mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
        }
        if(mRegionalizationService != null){
            try{
                //deviceType = mRegionalizationService.readFile(Utils.DEVICE_TYPE, "").get(0);
                contents = (ArrayList<String>)
                mRegionalizationService.readFile(Utils.DEVICE_TYPE, null);
                if (!contents.get(0).startsWith("devicetype=")) {
                    throw new IOException("Can't read devicetype from file devicetype!");
                }
                deviceType = contents.get(0).substring("devicetype=".length());
                Log.d(TAG,"!!!!!!!!!read over ........device type="+deviceType);
            }catch (Exception e) {
                Log.e(TAG, "Exception"+ e.getMessage());
            }
        }
        String currentCarriersName = Utils.getCurrentCarriersName(Utils.getCurrentCarriers());
        if (pathList.isEmpty() && deviceType != null && !currentCarriersName.equals("Default")) {
            if (deviceType.equals("omt")) {
                // non-match go to default if the device type is 'open market'
                String systemPos = RegionalizationEnvironment.getStoragePos();
                bundle.putString(systemPos+"/Default", " ");
                pathList.add(systemPos+"/Default");
            }
        }
        bundle.putStringArrayList(EXTRA_PATH_LIST, pathList);
        bundle.putInt(Utils.INTENT_CARRIER_TARGET_SLOT, mTargetSlot);

        startTriggerIntent.putExtras(bundle);
        return startTriggerIntent;
    }

    @Override
    public void onTriggerComplete() {
        if (Utils.DEBUG) Log.d(TAG,"onTriggerComplete() ");
        // Find all the matched carriers.
        for (Trigger trigger : TriggerManager.getInstance(this, mAccessCacheService)
                .getActiveTriggerList()) {
            if (trigger.isActive()) {
                if (Utils.DEBUG) Log.v(TAG, "hierarchy = " + trigger.getHierarchies());
                mMatchedCarriers.put(trigger.getHierarchies(),
                        trigger.getMatchedCarries());
            }
        }

        // Send the intent to activity to handle these carriers.
        sendBroadcast(buildStartTriggerIntent());
        Log.d(TAG, "ACTION_TRIGGER intent has been sent. Time="+System.currentTimeMillis());
        isRunning = false;
        // Stop this service.
        stopSelf();
    }

    @Override
    public void onDestroy() {
        if (mServiceConnection != null) {
            unbindService(mServiceConnection);
        }

        if (Utils.DEBUG) Log.i(TAG, "TriggerService onDestroy");

        super.onDestroy();
    }
}

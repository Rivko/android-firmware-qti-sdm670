/**
 * Copyright (c) 2015, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.os.IRegionalizationService;
import com.android.internal.os.RegionalizationEnvironment;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;

import java.util.List;
/**
 * SetTargetSlotService is used to set the system preferred slot and only start when first
 * boot up after switching is finished.
 */
public class SetTargetSlotService extends Service {
    private static final String TAG = "SetTargetSlotService";

    private int mTargetSubMask = Utils.INVALID_SUB_MASK;

    private boolean setDefaultDataSubIdDone = false;

    private ICarrierAccessCacheService mAccessCacheService = null;
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            if (Utils.DEBUG) Log.i(TAG, "Service Connected to " + name.getShortClassName());
            if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = ICarrierAccessCacheService.Stub.asInterface(service);
                getTargetSubMask();
                stopSelfIfAllDone();
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

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (Utils.DEBUG) Log.i(TAG, "Receive the action: " + action);
            if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(action)) {
                String stateExtra = intent.getStringExtra(
                        IccCardConstants.INTENT_KEY_ICC_STATE);
                if (Utils.DEBUG) Log.i(TAG, "ACTION_SIM_STATE_CHANGED: " + stateExtra);
                if (stateExtra.equals(IccCardConstants.INTENT_VALUE_ICC_LOADED)) {
                    unregisterReceiver(mReceiver);
                    if (mTargetSubMask != Utils.INVALID_SUB_MASK) {
                        setDefaultDataSubId();
                        stopSelfIfAllDone();
                    }
                }
            }
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();

        // Bind the service to access cache dir.
        Intent i = new Intent(ICarrierAccessCacheService.class.getName());
        i.setPackage(ICarrierAccessCacheService.class.getPackage().getName());
        bindService(i, mServiceConnection,
                Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);

        //Utils.saveCurrentSims(this);
    }

    @Override
    public void onDestroy() {
        if (mServiceConnection != null) {
            unbindService(mServiceConnection);
        }

        if (Utils.DEBUG) Log.i(TAG, "SetTargetSlotService onDestroy");
        Utils.setSharedPreference(SetTargetSlotService.this,
                Utils.SHARED_PREFERENCE_KEY_IS_FIRST_BOOT, "true");

        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void getTargetSubMask() {
        if (mAccessCacheService == null) {
            if (Utils.DEBUG) Log.e(TAG, "AccessCacheService have not been bound.");
            return;
        }
        String targetSubMaskString = null;
        if (Utils.isPresetMode()) {
            IRegionalizationService regionalizationService =
                    RegionalizationEnvironment.getRegionalizationService();
            List<String> contents = null;
            try {
                contents = regionalizationService.readFile(Utils.SUB_MASK_FILE_PATH, null);
            } catch (RemoteException e) {
                Log.e(TAG, "read the subMask file error. e=" + e);
            }
            if ((contents == null) || (contents.size() < 1)) {
                if (Utils.DEBUG) Log.d(TAG, "subMask file does not exist.");
            } else {
                targetSubMaskString = contents.get(0);
            }
        } else {
            try {
                targetSubMaskString = mAccessCacheService.readSubMaskFile();
            } catch (RemoteException e) {
                if (Utils.DEBUG) Log.e(TAG, "AccessCacheService error. e=" + e.getMessage());
                return;
            }
        }
        if (TextUtils.isEmpty(targetSubMaskString)) {
            if (Utils.DEBUG) Log.i(TAG, "targetSubMaskString is null.");
            return;
        }
        String subMaskRegularExpresstion = "^" + Utils.SUB_MASK_PRE + "[0-9]$";
        if (!targetSubMaskString.matches(subMaskRegularExpresstion)) {
            if (Utils.DEBUG) Log.d(TAG, "submask file match error. targetSubMaskString="
                    + targetSubMaskString);
            return;
        }
        if (Utils.DEBUG) Log.d(TAG, "targetSubMaskString=" + targetSubMaskString);
        int targetSubMask = Integer.parseInt(targetSubMaskString.substring(
                Utils.SUB_MASK_PRE.length()));
        if (Utils.DEBUG) Log.d(TAG, "targetSubMask=" + targetSubMask);
        if (targetSubMask != Utils.INVALID_SUB_MASK) {
            mTargetSubMask = targetSubMask;
            IntentFilter filter = new IntentFilter();
            filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
            filter.setPriority(1000);
            registerReceiver(mReceiver, filter);
        } else {
            setDefaultDataSubIdDone = true;
        }
    }

    private void setDefaultDataSubId() {
        if (Utils.DEBUG) Log.d(TAG, "Try to set preferred sub. mTargetSubMask="
                + mTargetSubMask);
        TelephonyManager telephonyManager = (TelephonyManager) this.getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = telephonyManager.getPhoneCount();
        int targetSlot = Utils.INVALID_SLOT_ID;
        for (int i=0; i<phoneCount; i++) {
            if ((mTargetSubMask & (0x0001 << i)) != 0) {
                targetSlot = i;
                break;
            }
        }
        if (Utils.DEBUG) Log.d(TAG, "Get target slot. targetSlot=" + targetSlot);
        if (targetSlot == Utils.INVALID_SLOT_ID) {
            return;
        }
        final SubscriptionManager subscriptionManager = SubscriptionManager.from(this);
        final SubscriptionInfo sir = SubscriptionManager.from(this)
                .getActiveSubscriptionInfoForSimSlotIndex(targetSlot);
        if (sir != null) {
            final int subId = sir.getSubscriptionId();
            subscriptionManager.setDefaultDataSubId(subId);
            Log.d(TAG, "DefaultDataSubId set to " + subId + ", targetSlot=" + targetSlot);
            Utils.setSharedPreference(this, Utils.SHARED_PREFERENCE_KEY_CURRENT_SUB_MASK,
                    String.valueOf(mTargetSubMask));
        } else {
            Log.d(TAG, "sir is null. targetSlot=" + targetSlot);
        }
        setDefaultDataSubIdDone = true;
    }

    private void stopSelfIfAllDone() {
        if (setDefaultDataSubIdDone) {
            SetTargetSlotService.this.stopSelf();
        }
    }
}

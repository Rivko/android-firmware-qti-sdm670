/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import com.android.internal.os.RegionalizationEnvironment;
import com.android.internal.os.IRegionalizationService;

import static com.android.internal.telephony.TelephonyIntents.SECRET_CODE_ACTION;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.Loader;
import android.content.Loader.OnLoadCompleteListener;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Carrier.CarriersStorage;
import com.qualcomm.qti.carrierconfigure.Actions.ActionCallback;
import com.qualcomm.qti.carrierconfigure.Actions.SwitchCarrierTask;
import com.qualcomm.qti.carrierconfigure.Actions.UpdateNVItemsTask;
import com.qualcomm.qti.carrierconfigure.Carrier.EmptyPathException;
import com.qualcomm.qti.carrierconfigure.Carrier.NullServiceException;
import com.qualcomm.qti.carrierconfigure.Carrier.SwitchData;
import com.qualcomm.qti.carrierconfigure.Utils;
import com.qualcomm.qti.carrierconfigure.Utils.CopyAndUnzipOtaFileThread;
import com.qualcomm.qti.loadcarrier.ILoadCarrierService;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class StartActivityReceiver extends BroadcastReceiver {
    private static final String TAG = "StartActivityReceiver";
    private static final String ACTION_PHONE_READY = "com.android.phone.ACTION_PHONE_READY";

    private static final String PROPERTY_SWITCH_WITHOUT_PROMPT =
            "persist.radio.trigger.silence";

    private static final String PROPERTY_SUPPORT_PROMPT_IN_SILENCE =
            "persist.radio.trigger.supportux";

    private static final String COM_MBN_FILE_PATH = "data/modem_config/mcfg_sw.mbn";

    public static final String CODE_SPEC_SWITCH   = "2277437234";
    public static final String CODE_SPEC_SWITCH_L = "73446625234";

    private int mTargetSlot = Utils.INVALID_SLOT_ID;
    private IRegionalizationService mRegionalizationService = null;

    @Override
    public void onReceive(Context context, Intent intent) {
        if (RegionalizationEnvironment.isSupported()) {
                mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
            }
        if (SECRET_CODE_ACTION.equals(intent.getAction())) {
            if (Utils.DEBUG) {
                Log.i(TAG, "Try to start QRD feature settings activity.");
            }

            String host = intent.getData() != null ? intent.getData().getHost() : null;
            if (CODE_SPEC_SWITCH.equals(host)) {
                Intent i = new Intent();
                i.setAction(Intent.ACTION_MAIN);
                i.setClass(context, ConfigurationActivity.class);
                i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(i);
            } else if (CODE_SPEC_SWITCH_L.equals(host)) {
                Intent i = new Intent();
                i.setAction(Intent.ACTION_MAIN);
                i.setClass(context, ConfigurationActivity.class);
                i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                i.addCategory(CODE_SPEC_SWITCH_L);
                context.startActivity(i);
            }
        } else if (Utils.ACTION_TRIGGER.equals(intent.getAction())) {
            Log.d(TAG, "ACTION_TRIGGER received. Time="+System.currentTimeMillis());
            // Get the carriers from the intent.
            Bundle bundle = intent.getExtras();
            ArrayList<String> pathList =
                    bundle == null ? null : bundle.getStringArrayList(Utils.EXTRA_PATH_LIST);
            if (pathList == null || pathList.size() < 1) return;
            mTargetSlot = bundle.getInt(Utils.INTENT_CARRIER_TARGET_SLOT);
            if (Utils.DEBUG) Log.i(TAG, "mTargetSlot = " + mTargetSlot);

            HashMap<String, String> carriers = new HashMap<String, String>();
            for (String path : pathList) {
                String contents = bundle.getString(path);
                carriers.put(path, contents);
            }
            CarriersStorage storage = new CarriersStorage();
            Carrier.parseCarriers(carriers, storage);

            ArrayList<Carrier> triggeredCarriers = null;
            if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
                triggeredCarriers = getTriggeredCarriers_fileBased(context, storage);
            } else {
                triggeredCarriers = getTriggeredCarriers(context, storage);
            }
            if (triggeredCarriers == null) return;
            Intent i = new Intent();
            if (mTargetSlot != Utils.INVALID_SLOT_ID) {
                i.setAction(Utils.ACTION_TRIGGER_START);
            } else {
                i.setAction(Utils.ACTION_TRIGGER_WELCOME);
            }
            i.setClass(context, ConfigurationActivity.class);
            i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            i.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST, triggeredCarriers);
            context.startActivity(i);
            context.stopService(getServiceIntent(context));
        } else if (Utils.ACTION_CARRIER_RESET.equals(intent.getAction())) {
            if (Utils.DEBUG) Log.i(TAG, "Show Carrier Reset activity.");

            Intent i = new Intent();
            i.setAction(Utils.ACTION_CARRIER_RESET);
            i.setClass(context, ConfigurationActivity.class);
            i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(i);
        }else if(Intent.ACTION_MASTER_CLEAR_NOTIFICATION.equals(intent.getAction())) {
            if (Utils.DEBUG) Log.i(TAG, "ACTION_MASTER_CLEAR_NOTIFICATION factory reset.");

            try {
                if(mRegionalizationService != null ){
                    mRegionalizationService.writeFile(Utils.NOT_TRIGGERED, "" + "\n", false);
                }
                if (Utils.DEBUG) Log.i(TAG, ".write over.");
            }catch (RemoteException re) {
                re.printStackTrace();
            }catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void saveCurrentSubId(Context context) {
        if (Utils.DEBUG) Log.d(TAG, "Save current sub ID");
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(
                Context.TELEPHONY_SERVICE);
        final int phoneCount = telephonyManager.getPhoneCount();
        final SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
        for (int i=0; i<phoneCount; i++) {
            final SubscriptionInfo sir = SubscriptionManager.from(context)
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            if (sir == null) {
                if (Utils.DEBUG) Log.d(TAG, "Slot:" + i + " is null");
            } else {
                final int subId = sir.getSubscriptionId();
                if (Utils.DEBUG) Log.d(TAG, "Slot:" + i + "'s subId:" + subId);
                Utils.setSharedPreference(context, Utils.SHARED_PREFERENCE_KEY_SUB_SLOT_PRE + i,
                        String.valueOf(subId));
            }
        }
    }

    // "*_fileBased" is used to distinguish the carrier switching through preset package
    // and file-based ota.zip with block-based ota.zip.
    private ArrayList<Carrier> getTriggeredCarriers_fileBased(Context context,
            CarriersStorage storage) {
        ArrayList<Carrier> triggeredCarriers = new ArrayList<Carrier>();
        ArrayList<String> notTriggeredCarriers = null;
        if (mTargetSlot == Utils.INVALID_SLOT_ID) {
            //notTriggeredCarriers = Carrier.getNotTriggeredCarriersName();
            notTriggeredCarriers = (ConfigurationApplication.getInstance()).getNotTriggeredCarriersName();
        }
        ArrayList<String> currentCarrierList = Carrier.getCurrentCarriers();
        Carrier specialROW = null;
        for (Carrier carrier : storage.mListCarrier) {
            if (carrier.mName.equals(Utils.SPECIAL_ROW_PACKAGE_NAME)) {
                specialROW = carrier;
                continue;
            }
            if(notTriggeredCarriers != null){
                for(int i=0;i<notTriggeredCarriers.size();i++)
                    Log.d(TAG, "notTriggeredCarriers add: " + notTriggeredCarriers.get(i));
            }
            if (currentCarrierList.contains(carrier.mName)
                    || notTriggeredCarriers == null
                    || notTriggeredCarriers.size() < 1
                    || !notTriggeredCarriers.contains(carrier.mName)) {
                triggeredCarriers.add(carrier.findLatestCarrier());
                if (Utils.DEBUG) Log.d(TAG, "triggeredCarriers add: " + carrier.mName);
            }
        }
        // If the triggered carrier list size is 0, do nothing.
        if (triggeredCarriers.size() == 0) {
            Log.i(TAG, "There isn't any carrier need triggered, do nothing.");
            if (specialROW != null) {
                Log.i(TAG, "ROW package exists.");
                if (!currentCarrierList.get(currentCarrierList.size()-1)
                        .equals(Utils.SPECIAL_ROW_PACKAGE_NAME)) {
                    int newSimSubMask = Utils.getNewSimsSubMask(context);
                    if (newSimSubMask != Utils.INVALID_SUB_MASK) {
                        specialROW.setSubMask(newSimSubMask);
                        triggeredCarriers.add(specialROW);
                        startSpecialInstallService(context, triggeredCarriers);
                        context.stopService(getServiceIntent(context));
                    }
                }
            } else {
                activateDefaultMBNIfNeeded(context);
            }
            return null;
        }
        if (triggeredCarriers.size() == 1) {
            // If triggered carriers is same with current top carrier do nothing.
            if (triggeredCarriers.get(0).mName.equals(
                    currentCarrierList.get(currentCarrierList.size()-1))) {
                Log.i(TAG, "Trigger carriers is same with current top carrier, do nothing.");
                if (specialROW != null) {
                    Log.i(TAG, "ROW package exists.");
                    if (!currentCarrierList.get(currentCarrierList.size()-1)
                            .equals(Utils.SPECIAL_ROW_PACKAGE_NAME)) {
                        int newSimSubMask = Utils.getNewSimsSubMask(context);
                        Log.i(TAG, "newSimSubMask=" + newSimSubMask);
                        int currentSimSubMask = triggeredCarriers.get(0).getSubMask();
                        Log.i(TAG, "currentSimSubMask=" + currentSimSubMask);
                        TelephonyManager telephonyManager = (TelephonyManager)
                                context.getSystemService(Context.TELEPHONY_SERVICE);
                        final int phoneCount = telephonyManager.getPhoneCount();
                        int phoneCountSubMask = Utils.INVALID_SUB_MASK;
                        for (int i=0; i<phoneCount; i++) {
                            phoneCountSubMask |= (0x0001 << i);
                        }
                        newSimSubMask = newSimSubMask & (~currentSimSubMask) & phoneCountSubMask;
                        Log.i(TAG, "Correct newSimSubMask=" + newSimSubMask);
                        if (newSimSubMask != Utils.INVALID_SUB_MASK) {
                            specialROW.setSubMask(newSimSubMask);
                            triggeredCarriers.clear();
                            triggeredCarriers.add(specialROW);
                            startSpecialInstallService(context, triggeredCarriers);
                            context.stopService(getServiceIntent(context));
                        }
                    }
                } else {
                    activateDefaultMBNIfNeeded(context, triggeredCarriers.get(0)
                            .getSubMask());
                }
                return null;
            }
            if (SystemProperties.getBoolean(PROPERTY_SWITCH_WITHOUT_PROMPT, false)) {
                int newSimSubMask = Utils.getNewSimsSubMask(context);
                Log.i(TAG, "newSimSubMask=" + newSimSubMask);
                int currentSimSubMask = triggeredCarriers.get(0).getSubMask();
                Log.i(TAG, "currentSimSubMask=" + currentSimSubMask);
                TelephonyManager telephonyManager = (TelephonyManager)
                        context.getSystemService(Context.TELEPHONY_SERVICE);
                final int phoneCount = telephonyManager.getPhoneCount();
                int phoneCountSubMask = Utils.INVALID_SUB_MASK;
                for (int i=0; i<phoneCount; i++) {
                    phoneCountSubMask |= (0x0001 << i);
                }
                Log.i(TAG, "phoneCountSubMask=" + phoneCountSubMask);

                //If switch form Default to regional, not to pop up dialog when silence
                String currentcarrier = null;
                ArrayList<String> carriers = Carrier.getCurrentCarriers();
                if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
                    currentcarrier = carriers.get(carriers.size() - 1);
                } else {
                    currentcarrier = Carrier.getCurrentCarriersName(carriers);
                }

                if(!SystemProperties.getBoolean(PROPERTY_SUPPORT_PROMPT_IN_SILENCE, true)
                        ||currentcarrier.equalsIgnoreCase("Default")){
                    startSpecialInstallService(context, triggeredCarriers);
                    context.stopService(getServiceIntent(context));
                    return null;
                }
                if ((newSimSubMask & currentSimSubMask & phoneCountSubMask)
                         != Utils.INVALID_SUB_MASK ){
                        Intent i = new Intent();
                        i.setAction(Utils.ACTION_TRIGGER_WELCOME);
                        i.setClass(context,
                            ConfigurationActivity.class);
                        i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        i.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST,
                            triggeredCarriers);
                        Carrier mSwitchCarrier = triggeredCarriers.get(0);
                        String localLanguage = SystemProperties.get("persist.sys.locale", null);
                        if(TextUtils.isEmpty(localLanguage)){
                            localLanguage = SystemProperties.get("ro.product.locale", null);
                        }
                        String switchCarrierLanguage = Carrier.getCarrierLanguage(mSwitchCarrier.mName);
                        Log.d(TAG, "Switch lanage localLanguage="+localLanguage+"---switchCarrierLanguage="+switchCarrierLanguage+"---mSwitchCarrier.mName="+mSwitchCarrier.mName);
                        if(localLanguage != null && switchCarrierLanguage != null && !localLanguage.equals(switchCarrierLanguage)){
                           context.startActivity(i);
                        } else {
                           startSpecialInstallService(context, triggeredCarriers);
                           context.stopService(getServiceIntent(context));
                        }
                }
                return null;
            }
        }

        if (mTargetSlot != Utils.INVALID_SLOT_ID) {
            boolean hasTargetSlot = false;
            for (Carrier carrier : triggeredCarriers) {
                Log.i(TAG, "carrier.getSubMask()=" + carrier.getSubMask() + " mContent=" +
                        carrier.mContent);
                if ((carrier.getSubMask() & (0x0001 << mTargetSlot)) != 0) {
                    Log.i(TAG, "carrier.getSubMask()=" + carrier.getSubMask() + " matched");
                    if (carrier.mName.equals(currentCarrierList.get(currentCarrierList.size()-1))) {
                        Log.i(TAG, "The target Slot is same with current carrier's, do nothing");
                        return null;
                    }
                    hasTargetSlot = true;
                }
            }
            if (!hasTargetSlot) {
                Log.i(TAG, "No trigger carrier matched the target slot.");
                return null;
            }
        }
        return triggeredCarriers;
    }

    private ArrayList<Carrier> getTriggeredCarriers(Context context, CarriersStorage storage) {
        String currentCarrierName = Carrier.getCurrentCarriersName(Carrier.getCurrentCarriers());
        Log.d(TAG, "currentCarrierName="+currentCarrierName);
        ArrayList<Carrier> triggeredCarriers = new ArrayList<Carrier>();
        ArrayList<String> notTriggeredCarriers = null;
        if (mTargetSlot == Utils.INVALID_SLOT_ID) {
            //notTriggeredCarriers = Carrier.getNotTriggeredCarriersName();
            notTriggeredCarriers = (ConfigurationApplication.getInstance()).getNotTriggeredCarriersName();
        }
        for (Carrier carrier : storage.mListCarrier) {
            if (currentCarrierName.equals(carrier.mName)
                    || notTriggeredCarriers == null
                    || notTriggeredCarriers.size() < 1
                    || !notTriggeredCarriers.contains(carrier.mName)) {
                triggeredCarriers.add(carrier.findLatestCarrier());
            }
        }

        // If the triggered carrier list size is 0, do nothing.
        if (triggeredCarriers.size() == 0) {
            Log.i(TAG, "There isn't any carrier need triggered, do nothing.");
            activateDefaultMBNIfNeeded(context);
            return null;
        }

        if (triggeredCarriers.size() == 1) {
            // If triggered carriers is same with current carriers do nothing.
            if (triggeredCarriers.get(0).mName.equals(currentCarrierName)) {
                Log.i(TAG, "Trigger carrier is same with current carrier do nothing.");
                activateDefaultMBNIfNeeded(context, triggeredCarriers.get(0)
                        .getSubMask());
                return null;
            }
            if (triggeredCarriers.get(0).mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
                Log.i(TAG, "Trigger carrier is only Default package.");
                return triggeredCarriers;
            }

            if (SystemProperties.getBoolean(PROPERTY_SWITCH_WITHOUT_PROMPT, false)) {
                startSpecialInstallService(context, triggeredCarriers);
                context.stopService(getServiceIntent(context));
                return null;
            }

            if (triggeredCarriers.get(0).mName.equals(Utils.SPECIAL_ROW_PACKAGE_NAME)) {
                startSpecialInstallService(context, triggeredCarriers);
                context.stopService(getServiceIntent(context));
                return null;
            }
        }

        if (triggeredCarriers.size() == 2) {
            if ((triggeredCarriers.get(0).mName.equals(currentCarrierName)
                    && triggeredCarriers.get(1).mName.equals(currentCarrierName
                            + Utils.CARRIER_TO_DEFAULT_NAME))
                    || (triggeredCarriers.get(0).mName.equals(currentCarrierName
                            + Utils.CARRIER_TO_DEFAULT_NAME)
                    && triggeredCarriers.get(1).mName.equals(currentCarrierName))) {
                Log.i(TAG, "Trigger carrier is same with current carrier, do nothing2");
                activateDefaultMBNIfNeeded(context, triggeredCarriers.get(0).mName
                        .equals(currentCarrierName) ? triggeredCarriers.get(0).getSubMask()
                        : triggeredCarriers.get(1).getSubMask());
                return null;
            }
            if ((triggeredCarriers.get(0).mName.equals(Utils.SPECIAL_ROW_PACKAGE_NAME)
                    && triggeredCarriers.get(1).mName.equals(currentCarrierName
                            + Utils.CARRIER_TO_DEFAULT_NAME))
                    || (triggeredCarriers.get(0).mName.equals(currentCarrierName
                            + Utils.CARRIER_TO_DEFAULT_NAME)
                    && triggeredCarriers.get(1).mName.equals(Utils.SPECIAL_ROW_PACKAGE_NAME))) {
                startSpecialInstallService(context, triggeredCarriers);
                context.stopService(getServiceIntent(context));
                return null;
            }
            if (SystemProperties.getBoolean(PROPERTY_SWITCH_WITHOUT_PROMPT, false)
                    && (triggeredCarriers.get(0).mName.equals(currentCarrierName
                    + Utils.CARRIER_TO_DEFAULT_NAME) || triggeredCarriers.get(1).mName.equals(
                    currentCarrierName + Utils.CARRIER_TO_DEFAULT_NAME))) {
                startSpecialInstallService(context, triggeredCarriers);
                context.stopService(getServiceIntent(context));
                return null;
            }
        }

        if (!currentCarrierName.equals("Default")) {
            boolean has2DefaultPackage = false;
            for (Carrier carrier : triggeredCarriers) {
                if (carrier.mName.equals(currentCarrierName + Utils.CARRIER_TO_DEFAULT_NAME)) {
                    has2DefaultPackage = true;
                    break;
                }
            }
            if (!has2DefaultPackage) {
                Log.i(TAG, "Need " + currentCarrierName + Utils.CARRIER_TO_DEFAULT_NAME);
                return null;
            }
        }

        if (mTargetSlot != Utils.INVALID_SLOT_ID) {
            boolean hasTargetSlot = false;
            for (Carrier carrier : triggeredCarriers) {
                Log.i(TAG, "carrier.getSubMask()=" + carrier.getSubMask() + " mContent=" +
                        carrier.mContent);
                if ((carrier.getSubMask() & (0x0001 << mTargetSlot)) != 0) {
                    Log.i(TAG, "carrier.getSubMask()=" + carrier.getSubMask() + " matched");
                    if (carrier.mName.equals(currentCarrierName)) {
                        Log.i(TAG, "The target Slot is same with current carrier's, do nothing");
                        return null;
                    }
                    hasTargetSlot = true;
                }
            }
            if (!hasTargetSlot) {
                Log.i(TAG, "No trigger carrier matched the target slot.");
                return null;
            }
        }
        return triggeredCarriers;
    }

    private void activateDefaultMBNIfNeeded(Context context) {
        this.activateDefaultMBNIfNeeded(context, Utils.INVALID_SUB_MASK);
    }

    private void activateDefaultMBNIfNeeded(Context context,
            int matchCurrentCarrierSubMask) {
        Carrier defaultCarrier = Carrier.getDefaultCarrierInstance();
        if (defaultCarrier != null) {
            File mbnFile = new File(defaultCarrier.getPath() + File.separator +
                    Utils.COM_MBN_FILE_PATH);
            if (mbnFile.exists()) {
                if (Utils.DEBUG) Log.d(TAG, "Default Carrier has a MBN file.");
                int newSimsSubMask = Utils.getNewSimsSubMask(context);
                if (newSimsSubMask != Utils.INVALID_SUB_MASK) {
                    // TODO: Use a new service instead.
                    if (Utils.DEBUG) Log.d(TAG, "New SIM cards subMask=" + newSimsSubMask);
                    newSimsSubMask &= ~matchCurrentCarrierSubMask;
                    if (newSimsSubMask != Utils.INVALID_SUB_MASK) {
                        if (Utils.DEBUG) Log.d(TAG, "Set new SIM to Default ROW. new subMask=" +
                                newSimsSubMask);
                        defaultCarrier.setSubMask(newSimsSubMask);
                        defaultCarrier.setMode(Carrier.ITEM_MODE_MP);
                        Log.d(TAG, "ITEM_MODE_MP=" + defaultCarrier.getMode());
                        ArrayList<Carrier> triggeredCarriers = new ArrayList<Carrier>();
                        triggeredCarriers.add(defaultCarrier);
                        startSpecialInstallService(context, triggeredCarriers);
                    } else {
                        if (Utils.DEBUG) Log.d(TAG, "New SIMs match the current package.");
                        Utils.saveCurrentSims(context);
                    }
                } else {
                    if (Utils.DEBUG) Log.d(TAG, "No new SIM card.");
                }
            }
        }
    }

    private void startSpecialInstallService(Context context,
            ArrayList<Carrier> triggeredCarriers) {
        Log.i(TAG, "Start TriggerSpecialInstallService.");
        Intent intent = new Intent();
        intent.setClass(context, TriggerSpecialInstallService.class);
        intent.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST, triggeredCarriers);
        context.startService(intent);
    }

    private Intent getServiceIntent(Context context) {
        Intent registTriggerIntent = new Intent();
        registTriggerIntent.setClass(context, RegistTriggerService.class);
        return registTriggerIntent;
    }

    //Add ConfigurationApplication to regist receiver, so this service is not needed.
    public static class RegistTriggerService extends Service {
        private final static String TAG = "RegistTriggerService";

        private StartActivityReceiver mReceiver = null;
        @Override
        public IBinder onBind(Intent intent) {
            return null;
        }

        @Override
        public void onCreate() {
            super.onCreate();
            IntentFilter filter = new IntentFilter();
            filter.addAction(Utils.ACTION_TRIGGER);
            filter.setPriority(1000);
            mReceiver = new StartActivityReceiver();
            registerReceiver(mReceiver, filter);
            Log.i(TAG,"register StartActivityReceiver");
        }

        @Override
        public void onDestroy() {
            if (mReceiver != null) {
                unregisterReceiver(mReceiver);
                mReceiver = null;
            }
            Log.i(TAG,"unregister StartActivityReceiver");
            super.onDestroy();
        }
    }

    public static class TriggerSpecialInstallService extends Service implements ActionCallback,
            OnClickListener {
        private static final String TAG = "TriggerSpecialInstallService";

        private static final String RE_MNC = "(\\d{2,3})?";
        private static final String SEP_COMMA = ",";

        private static final int RETRY_DELAY = 500;
        private static final int RETRY_COUNT = 10;

        private static final int MSG_UPDATE_NV_ITEMS = 0;
        private static final int MSG_SHOW_ALERT_DIALOG = MSG_UPDATE_NV_ITEMS + 1;
        private static final int MSG_SHOW_NOTICE_DIALOG = MSG_SHOW_ALERT_DIALOG + 1;
        private static final int MSG_START_SWITCH = MSG_SHOW_NOTICE_DIALOG + 1;
        private static final int MSG_COPY_AND_UNZIPFILE = MSG_START_SWITCH + 1;
        private static final int MSG_SWITCH_ERROR = MSG_COPY_AND_UNZIPFILE + 1;
        private static final int MSG_FINISH = MSG_SWITCH_ERROR + 1;

        private static final int DIALOG_ALERT = 1;
        private static final int DIALOG_NOTICE = 2;

        private AlertDialog mWaitDialog = null;

        private int mDialogId = 0;
        private int mSubMask = 0;
        private String mConfigId = null;

        private int mRetryTimes = 0;
        private Carrier mSwitchCarrier = null;
        private Carrier mSwitchToDefaultCarrier = null;
        private ArrayList<Carrier> mCarriers;

        private CarriersLoader mLoader = null;
        private CarriersStorage mCarriersStorage = null;
        private OnLoadCompleteListener<CarriersStorage> mLoaderListener =
                new OnLoadCompleteListener<CarriersStorage>() {
            @Override
            public void onLoadComplete(Loader<CarriersStorage> loader, CarriersStorage storage) {
                mCarriersStorage = storage;
                if (mSwitchCarrier != null) {
                    mHandler.sendEmptyMessage(MSG_UPDATE_NV_ITEMS);
                } else {
                    Log.d(TAG, "No target carrier. Do nothing");
                    mHandler.sendEmptyMessage(MSG_FINISH);
                }
            }
        };

        private ILoadCarrierService mService = null;
        private ICarrierAccessCacheService mAccessCacheService = null;
        private ServiceConnection mServiceConnection = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                if (Utils.DEBUG) Log.i(TAG, "Service Connected to " + name.getShortClassName());
                if (name.getShortClassName().equals(".LoadCarrierService")) {
                    mService = ILoadCarrierService.Stub.asInterface(service);
                } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                    mAccessCacheService = ICarrierAccessCacheService.Stub.asInterface(service);
                }
                if ((mService != null) && (mAccessCacheService != null)) {
                    mLoader = new CarriersLoader(TriggerSpecialInstallService.this, mService,
                            mAccessCacheService);
                    mLoader.registerListener(0, mLoaderListener);
                    mLoader.startLoading();
                }
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                if (name.getShortClassName().equals(".LoadCarrierService")) {
                    mService = null;
                } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                    mAccessCacheService = null;
                }
            }
        };

        private Handler mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MSG_UPDATE_NV_ITEMS:
                        if (Utils.DEBUG) Log.d(TAG, "Update NV Items start.");
                        // Before start the switch action, need update the NV Items first. And after
                        // the update action finished, will start the switch action.
                        findActiveSubMask(mSwitchCarrier);
                        updateNVItems();
                        break;
                    case MSG_SHOW_ALERT_DIALOG:
                        if (SystemProperties.getBoolean(PROPERTY_SWITCH_WITHOUT_PROMPT, false)
                                && !Utils.SPECIAL_ROW_PACKAGE_NAME.equals(mSwitchCarrier.mName)) {
                            // Try to switch the carrier.
                            mHandler.sendEmptyMessage(MSG_START_SWITCH);
                        } else {
                            showAlertDialog();
                        }
                        break;
                    case MSG_SHOW_NOTICE_DIALOG:
                        if (SystemProperties.getBoolean(PROPERTY_SWITCH_WITHOUT_PROMPT, false)
                                && !Utils.SPECIAL_ROW_PACKAGE_NAME.equals(mSwitchCarrier.mName)) {
                            // no need to prompt the user to restart phone
                            mHandler.sendEmptyMessage(MSG_FINISH);
                        } else {
                            showNoticeDialog();
                        }
                        break;
                    case MSG_START_SWITCH:
                        if (Utils.DEBUG) Log.d(TAG, "Switch action start.");
                        mWaitDialog = getWaitDialog();
                        mWaitDialog.show();
                        startSwitchAction();
                        break;
                    case MSG_COPY_AND_UNZIPFILE:
                        startSwitchAction_filebased(msg);
                        break;
                    case MSG_SWITCH_ERROR:
                        Toast.makeText(TriggerSpecialInstallService.this,
                                R.string.alert_switch_error, Toast.LENGTH_LONG).show();
                        break;
                    case MSG_FINISH:
                        if (Utils.DEBUG) Log.d(TAG, "TriggerSpecialInstallService finished.");
                        if (mWaitDialog != null) {
                            mWaitDialog.dismiss();
                            mWaitDialog = null;
                        }
                        if (mServiceConnection != null) {
                            unbindService(mServiceConnection);
                        }
                        stopSelf();
                        break;
                    default:
                        break;
                }
            }
        };

        @Override
        public void onCreate() {
            super.onCreate();
            // Try to bind the service.
            if (mServiceConnection != null) {
                if (mService == null) {
                    // Bind the service to get the carriers stored in SD card.
                    Intent intent = new Intent(ILoadCarrierService.class.getName());
                    intent.setPackage(ILoadCarrierService.class.getPackage().getName());
                    bindService(intent, mServiceConnection,
                            Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
                }
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
            Log.d(TAG, "TriggerSpecialInstallService start.");
            mCarriers = intent.getParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST);
            if (mCarriers == null || mCarriers.size() < 1) {
                Log.d(TAG, "No carrier. Do nothing.");
                mHandler.sendEmptyMessage(MSG_FINISH);
                return;
            } else if ((mCarriers.size() == 1) && (!mCarriers.get(0).mName.endsWith(
                    Utils.CARRIER_TO_DEFAULT_NAME))) {
                mSwitchCarrier = mCarriers.get(0);
                Log.d(TAG, "mSwitchCarrier = " + mSwitchCarrier);
            } else if (mCarriers.size() == 2) {
                for (Carrier carrier : mCarriers) {
                    Log.d(TAG, "carrier.mName=" + carrier.mName);
                    if (carrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
                        mSwitchToDefaultCarrier = carrier;
                    } else {
                        mSwitchCarrier = carrier;
                    }
                }
                if ((mSwitchCarrier == null) || (mSwitchToDefaultCarrier == null)) {
                    Log.d(TAG, "mSwitchCarrier = " + mSwitchCarrier
                            + "mSwitchToDefaultCarrier = " + mSwitchToDefaultCarrier);
                    mHandler.sendEmptyMessage(MSG_FINISH);
                    return;
                }
            } else if (mCarriers.size() > 2) {
                Log.d(TAG, "Too many carriers. Do nothing now.");
                mHandler.sendEmptyMessage(MSG_FINISH);
                return;
            } else {
                Log.d(TAG, "Wrong Carriers.");
                mHandler.sendEmptyMessage(MSG_FINISH);
                return;
            }
        }

        @Override
        public IBinder onBind(Intent intent) {
            return null;
        }

        @Override
        public void onDestroy() {
            super.onDestroy();
        }

        @Override
        public void onActionFinished(int requestCode) {
            switch (requestCode) {
                case Actions.REQUEST_UPDATE_NV_ITEMS:
                    if (Utils.DEBUG) Log.d(TAG, "Update NV items finished.");
                    Utils.saveCurrentSims(this, mSwitchCarrier.getSubMask());
                    String mode = mSwitchCarrier.getMode();
                    if (TextUtils.isEmpty(mode)) {
                        mHandler.sendEmptyMessage(MSG_SHOW_ALERT_DIALOG);
                    } else {
                        //Carrier.saveAsNotTriggeredCarrier(mCarriers);
                        ((ConfigurationApplication)(this.getApplication())).saveAsNotTriggeredCarrier(mCarriers);
                        if (mode.equals(Carrier.ITEM_MODE_AP_MP) && (!mSwitchCarrier.mType.equals(
                                Carrier.TYPE_DIRECTORY) || Utils.isFileBasedSwitchEnabled()
                                || Utils.isPresetMode())) {
                            mHandler.sendEmptyMessage(MSG_SHOW_ALERT_DIALOG);
                        } else {
                            mHandler.sendEmptyMessage(MSG_SHOW_NOTICE_DIALOG);
                        }
                    }
                    break;
                case Actions.REQUEST_UPDATE_ROW_NV_ITEMS:
                    // Do nothing now.
                    break;
                case Actions.REQUEST_SWITCH_CARRIER:
                    if (Utils.DEBUG) Log.d(TAG, "Switch action finished.");
                    mHandler.sendEmptyMessage(MSG_FINISH);
                    break;
            }
        }

        @Override
        public void onActionError(int requestCode, int resultCode, Exception ex) {
            switch (requestCode) {
                case Actions.REQUEST_UPDATE_NV_ITEMS:
                    if (resultCode == Actions.RESULT_NO_MBN) {
                        Log.d(TAG, "No mbn files.");
                        //Carrier.saveAsNotTriggeredCarrier(mCarriers);
                        ((ConfigurationApplication)(this.getApplication())).saveAsNotTriggeredCarrier(mCarriers);
                        String mode = mSwitchCarrier.getMode();
                        if (TextUtils.isEmpty(mode)) {
                            mHandler.sendEmptyMessage(MSG_SHOW_ALERT_DIALOG);
                        } else {
                            if (mode.equals(Carrier.ITEM_MODE_AP) && (!mSwitchCarrier.mType.equals(
                                    Carrier.TYPE_DIRECTORY) || Utils.isFileBasedSwitchEnabled()
                                    || Utils.isPresetMode())) {
                                mHandler.sendEmptyMessage(MSG_SHOW_ALERT_DIALOG);
                                break;
                            }
                        }
                    }
                    Log.e(TAG, "Error when updating NV items");
                    mHandler.sendEmptyMessage(MSG_FINISH);
                    break;
                case Actions.REQUEST_UPDATE_ROW_NV_ITEMS:
                    // Do nothing now.
                    break;
                default:
                    Log.e(TAG, "Get the request[ " + requestCode + "] error, ex is "
                            + ex.getMessage());
                    mHandler.sendEmptyMessage(MSG_SWITCH_ERROR);
                    break;
            }
        }

        @Override
        public void onClick(DialogInterface dialog, int which) {
            switch (mDialogId) {
                case DIALOG_ALERT:
                    switch (which) {
                        case DialogInterface.BUTTON_POSITIVE:
                            // For user press OK, then try to switch the carrier.
                            mHandler.sendEmptyMessage(MSG_START_SWITCH);
                            break;
                        case DialogInterface.BUTTON_NEGATIVE:
                            mHandler.sendEmptyMessage(MSG_FINISH);
                            break;
                    }
                    break;
                case DIALOG_NOTICE:
                    mHandler.sendEmptyMessage(MSG_FINISH);
                    break;
            }
        }

        private void findActiveSubMask(Carrier carrier) {
            TelephonyManager telephonyManager = (TelephonyManager) getSystemService(
                    Context.TELEPHONY_SERVICE);
            int phoneCount = telephonyManager.getPhoneCount();
            mSubMask = 0;
            for (int i = 0; i < phoneCount; i++) {
                String mccMnc = telephonyManager.getSimOperator(SubscriptionManager.getSubId(i)[0]);
                Log.d(TAG, "mccmnc=" + mccMnc);
                if (!TextUtils.isEmpty(mccMnc)) {
                    mSubMask = mSubMask | ( 0x0001 << i);
                }
            }
            mConfigId = carrier.mName;
        }

        private void showNoticeDialog() {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(R.string.notice_row_title)
                    .setMessage(R.string.notice_row_text)
                    .setPositiveButton(android.R.string.ok, this);

            AlertDialog dialog = builder.create();
            dialog.setCanceledOnTouchOutside(false);
            dialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
            dialog.show();

            mDialogId = DIALOG_NOTICE;
        }

        private void showAlertDialog() {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(R.string.notice_special_install_title)
                    .setMessage(R.string.notice_special_install_text)
                    .setPositiveButton(android.R.string.ok, this)
                    .setNegativeButton(android.R.string.cancel, this);

            AlertDialog dialog = builder.create();
            dialog.setCanceledOnTouchOutside(false);
            dialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
            dialog.show();

            mDialogId = DIALOG_ALERT;
        }

        private void findDependencyCarriers(Carrier carrier, CarriersStorage storage,
                ArrayList<Carrier> list) {
            String dependcarrier = carrier.getDependency();
            if (TextUtils.isEmpty(dependcarrier)) return;
            for (Carrier carrierItem : storage.mListCarrier) {
                if (carrierItem.mName.equals(dependcarrier)) {
                    list.add(carrierItem);
                    findDependencyCarriers(carrierItem, storage, list);
                    break;
                }
            }
        }

        private void startSwitchAction() {
            try {
                if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
                    if (mCarriersStorage != null) {
						/*String type = mSwitchCarrier.getDeviceType();
                        if (RegionalizationEnvironment.isSupported()) {
                            mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
                        }
                        if(mRegionalizationService != null){
                            try{
                                mRegionalizationService.writeFile(Utils.DEVICE_TYPE, type + "\n", false);
                                Log.d(TAG,".........write over ........"+type);
                            }catch (Exception e) {
                                Log.e(TAG, "IOException"+ e.getMessage());
                            }
                        }*/
                        ArrayList<Carrier> carrierlist = new ArrayList<Carrier>();
                        carrierlist.add(mSwitchCarrier);
                        findDependencyCarriers(mSwitchCarrier, mCarriersStorage, carrierlist);
                        new CopyAndUnzipOtaFileThread(this, mHandler,
                                MSG_COPY_AND_UNZIPFILE, mService, mAccessCacheService,
                                carrierlist).start();
                    } else {
                        Log.e(TAG, "LoadCarriers have not finished.");
                    }
                } else {
                    SwitchData data = mSwitchCarrier.getSwitchData(mService, mAccessCacheService);
                    SwitchCarrierTask switchTask = new SwitchCarrierTask(this,
                            Actions.REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
                    if (mSwitchToDefaultCarrier != null && !mSwitchCarrier.getBaseCarrierName()
                            .equals(Carrier.getCurrentCarriersName(Carrier.getCurrentCarriers()))
                            && !mSwitchCarrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
                        switchTask.execute(mSwitchToDefaultCarrier.getSwitchData(mService,
                                mAccessCacheService), data);
                    } else switchTask.execute(data);
                }
            } catch (NullServiceException ex) {
                Log.e(TAG, "Catch the NullServiceException: " + ex.getMessage());
                if (mRetryTimes <= RETRY_COUNT) {
                    mRetryTimes = mRetryTimes + 1;
                    mHandler.sendEmptyMessageDelayed(MSG_START_SWITCH, RETRY_DELAY);
                } else {
                    Log.e(TAG, "Already couldn't get the service, please check the status.");
                    mHandler.sendEmptyMessage(MSG_SWITCH_ERROR);
                }
            } catch (EmptyPathException ex) {
                Log.e(TAG, "Catch the EmptyPathException: " + ex.getMessage());
                // There is some error when we get the switch intent.
                // Send the switch error message, .
                mHandler.sendEmptyMessage(MSG_SWITCH_ERROR);
            }
        }

        private void startSwitchAction_filebased(Message msg) {
            try {
                SwitchCarrierTask switchTask = new SwitchCarrierTask(this,
                        Actions.REQUEST_SWITCH_CARRIER, this,
                        (ArrayList<SwitchData>)msg.obj, mAccessCacheService);
                switchTask.execute();
            } catch (IllegalArgumentException ex) {
                Log.e(TAG, "Catch the IllegalArgumentException: " + ex.getMessage());
            }
        }

        private void updateNVItems() {
            // Get the intent to start the action service to handle the switch action.
            UpdateNVItemsTask updateTask = null;
            if (Utils.DEFAULT_SPEC_NAME.equals(mSwitchCarrier.mName)) {
                ArrayList<Carrier> otherCarriers = new ArrayList<Carrier>();
                otherCarriers.add(mSwitchCarrier);
                updateTask = new UpdateNVItemsTask(this, Actions.REQUEST_UPDATE_NV_ITEMS,
                        this, null, otherCarriers, true, mAccessCacheService);
            } else if (Utils.SPECIAL_ROW_PACKAGE_NAME.equals(mSwitchCarrier.mName)) {
                Log.i(TAG, "mSwitchCarrier.getSubMask()=" + mSwitchCarrier.getSubMask());
                updateTask = new UpdateNVItemsTask(this, Actions.REQUEST_UPDATE_NV_ITEMS,
                        this, mSwitchCarrier, null, true, mAccessCacheService);
            } else {
                updateTask = new UpdateNVItemsTask(this, Actions.REQUEST_UPDATE_NV_ITEMS,
                        this, mSwitchCarrier, null, true, mAccessCacheService);
            }
            updateTask.execute();
        }

        private AlertDialog getWaitDialog() {
            ProgressDialog dialog = new ProgressDialog(this);
            String switchedCarrierLanguage = getSwitchCarrierLanguage();
            if(switchedCarrierLanguage == null) {
                dialog.setMessage(getString(R.string.progress_wait));
            } else {
            dialog.setMessage(getString(R.string.progress_wait) +
                    switchedCarrierLanguage);
            }
            dialog.setCanceledOnTouchOutside(false);
            dialog.setCancelable(false);
            dialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
            return dialog;
        }

        private String getSwitchCarrierLanguage() {
            if(SystemProperties.getBoolean(PROPERTY_SWITCH_WITHOUT_PROMPT, false)) {
                String switchCarrierLanguage = Carrier.getCarrierLanguage(
                    mSwitchCarrier.mName);
                String localLanguage = getResources().getConfiguration().locale
                    .toLanguageTag();
                if(switchCarrierLanguage != null
                    && !switchCarrierLanguage.equals(localLanguage)) {
                        return getString(R.string.carrier_language_change, switchCarrierLanguage);
                }
                Log.d(TAG, "switchCarrierLanguage " + switchCarrierLanguage
                        +"localLanguage "+localLanguage);
            }
            return null;
        }
    }
}

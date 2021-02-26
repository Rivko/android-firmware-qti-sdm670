/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.Service;
import android.content.pm.IPackageDataObserver;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PowerManager;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.os.Build;
import android.net.Uri;
import android.content.ContentResolver;
import android.widget.Toast;

import com.android.internal.os.IRegionalizationService;
import com.android.internal.os.RegionalizationEnvironment;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;
import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Carrier.SwitchData;
import com.android.internal.app.LocalePicker;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;
import java.io.UnsupportedEncodingException;

public class Actions {
    public interface ActionCallback {
        public void onActionFinished(int requestCode);
        public void onActionError(int requestCode, int resultCode, Exception ex);
    }

    public static final int REQUEST_SWITCH_CARRIER = 0;
    public static final int REQUEST_UPDATE_NV_ITEMS = 1;
    public static final int REQUEST_UPDATE_ROW_NV_ITEMS = 2;

    public static final int RESULT_OK = 0;
    public static final int RESULT_FAILURE = -1;
    public static final int RESULT_NO_MBN = -2;

    /**
     * This service will be running in the phone process as it used to edit the sim mode prop.
     */
    public static class UpdateSimMode extends Service {
        public static final String ACTION_SIM_MODE_UPDATE =
                "com.qualcomm.qti.carrierconfigure.simmode.update";
        public static final String EXTRA_NEW_MODE = "new_mode";

        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            if (ACTION_SIM_MODE_UPDATE.equals(intent.getAction())) {
                String newValue = intent.getStringExtra(EXTRA_NEW_MODE);
                Utils.setSimMode(newValue);
                Log.i("UpdateSimModeService", "Update the sim mode to " + newValue);

                PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
                pm.reboot("Reboot for change sim mode to: " + newValue);
            }
            return super.onStartCommand(intent, flags, startId);
        }

        @Override
        public IBinder onBind(Intent intent) {
            return null;
        }
    }

    /**
     * This AsyncTask used to set the MBN file to update the NV items.
     */
    public static class UpdateNVItemsTask extends AsyncTask<Void, Void, Void> {
        private static final String TAG = "UpdateNVItemsTask";

        // These values need same as the definition in the QcrilMsgTunnelSocket.
        // As them will be send by the QcrilMsgTunnelSocket.
        private static final String EXTRA_PDC_ACTIVE = "active";
        private static final String ACTION_PDC_DATA_RECEIVED =
                "qualcomm.intent.action.ACTION_PDC_DATA_RECEIVED";
        private static final String ACTION_PDC_CONFIGS_CLEARED =
                "qualcomm.intent.action.ACTION_PDC_CONFIGS_CLEARED";
        private static final String ACTION_PDC_CONFIG_LIST_RECEIVED =
            "qualcomm.intent.action.ACTION_PDC_CONFIG_LIST_RECEIVED";
        private static final String MBN_CONFIG_ID_PREFIX = "Regional_";
        private static final String ROW_MBN_CONFIG_ID_PREFIX = MBN_CONFIG_ID_PREFIX + "ROW_";
        private final String ENCODING = "ISO-8859-1";
        // The result for these two actions.
        private static final int RESULT_SUCCESS = 0;
        private static final int RESULT_FAILURE = -1;

        private static final int RESULT_NEED_CLEAN = -2;
        private static final int RESULT_ALREADY_CLEAN = -3;

        // The messages will be handled.
        private static final int MSG_READY_CONFIG = 0;
        private static final int MSG_GET_CONFIG = MSG_READY_CONFIG + 1;
        private static final int MSG_DEACTIVATE_CONFIG = MSG_GET_CONFIG + 1;
        private static final int MSG_SET_CONFIG = MSG_DEACTIVATE_CONFIG + 1;
        private static final int MSG_SELECT_CONFIG = MSG_SET_CONFIG + 1;
        private static final int MSG_ACTIVATE_CONFIG = MSG_SELECT_CONFIG + 1;
        private static final int MSG_ACTION_TIMEOUT = MSG_ACTIVATE_CONFIG + 1;
        private static final int MSG_GET_MBN_VERSION = MSG_ACTION_TIMEOUT + 1;
        private static final int MSG_ACTION_FINISHED = MSG_GET_MBN_VERSION + 1;
        private static final int MSG_EVENT_PDC_CONFIG_LIST_DONE = MSG_ACTION_FINISHED + 1;

        // Set this update action's timeout is 10 seconds.
        private static final int TIMEOUT_MILLIS = 10 * 1000;

        // The max retry time if the action is failure.
        private static final int MAX_RETRY_TIMES = 5;

        private static final int MODE_NONE = 0;
        private static final int MODE_READY = MODE_NONE + 1;
        private static final int MODE_SET = MODE_READY + 1;

        private int mRetryTimes = 0;
        private boolean mReady = false;
        private int mCurActionMode = MODE_NONE;

        private int mRequestCode = 0;
        private int mResultCode = RESULT_OK;
        private Context mContext = null;
        private Carrier mTargetCarrier = null;
        private ArrayList<Carrier> mOtherCarriers = null;
        private int mSubMask = Utils.INVALID_SUB_MASK;
        private Message mCurrentMsg = null;
        private QcRilHook mQcRilHook = null;
        private ActionCallback mCallback = null;
        private ICarrierAccessCacheService mAccessCacheService = null;
        private boolean mSaveOriginalConfig = false;

        private int mPhoneCount = 0;
        private boolean[] mSlotHasBeenSet = null;
        private String[] mSlotCurrentConfigId = null;
        private boolean mTargetCarrierScanned = false;
        private ModemSetConfigData[] mModemSetConfigData = null;
        private IRegionalizationService mRegionalizationService = null;

        private Handler mHandler = new Handler() {
            private static final int RETRY_DELAY_MILLIS = 100;
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MSG_READY_CONFIG:
                        if (mReady && mQcRilHook != null) {
                            handleReadyMessage();
                        } else {
                            Message newMsg = Message.obtain(msg);
                            sendMessageDelayed(newMsg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_GET_CONFIG:
                        if (mReady && mQcRilHook != null) {
                            handleGetMessage();
                        } else {
                            Message newMsg = Message.obtain(msg);
                            sendMessageDelayed(newMsg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_DEACTIVATE_CONFIG:
                        if (mReady && mQcRilHook != null) {
                            handleDeactivateMessage();
                        } else {
                            sendMessageDelayed(msg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_SET_CONFIG:
                        if (mReady && mQcRilHook != null) {
                            ModemSetConfigData mscd = (ModemSetConfigData) msg.obj;
                            handleSetMessage(mscd);
                        } else {
                            sendMessageDelayed(msg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_SELECT_CONFIG:
                        if (mReady && mQcRilHook != null) {
                            ModemSetConfigData mscd = (ModemSetConfigData) msg.obj;
                            handleSelectMessage(mscd);
                        } else {
                            sendMessageDelayed(msg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_ACTIVATE_CONFIG:
                        if (mReady && mQcRilHook != null) {
                            handleActivateMessage();
                        } else {
                            sendMessageDelayed(msg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_GET_MBN_VERSION:
                        if (mReady && mQcRilHook != null) {
                            handleGetMbnVersion();
                        } else {
                            sendMessageDelayed(msg, RETRY_DELAY_MILLIS);
                        }
                        break;
                    case MSG_ACTION_TIMEOUT:
                        Log.w(TAG, "This action is timeout.");
                    case MSG_ACTION_FINISHED:
                        Log.d(TAG, "Update action finished.");
                        if (mCurActionMode != MODE_NONE) {
                            // Unregister the receiver as the action finished.
                            mContext.unregisterReceiver(mReceiver);
                            // Reset the value.
                            mCurActionMode = MODE_NONE;
                        }

                        // Remove all message.
                        removeMessages(MSG_SET_CONFIG);
                        removeMessages(MSG_ACTION_TIMEOUT);
                        removeMessages(MSG_ACTION_FINISHED);

                        if (mTargetCarrier != null) {
                            writeSubMask(mTargetCarrier.getSubMask());
                        }

                        // Reset the value.
                        mRetryTimes = 0;

                        if (mCallback != null) {
                            if (mResultCode == RESULT_OK) {
                                mCallback.onActionFinished(mRequestCode);
                            } else {
                                mCallback.onActionError(mRequestCode, mResultCode, null);
                            }
                        }

                        if (mQcRilHook != null) {
                            mQcRilHook.dispose();
                            mQcRilHook = null;
                        }
                        break;
                    case MSG_EVENT_PDC_CONFIG_LIST_DONE:
                        Log.d(TAG,"MSG_EVENT_PDC_CONFIG_LIST_DONE");
                        if (msg.obj != null) {
                            byte[] result = (byte[])msg.obj;
                            parseConfigList(result);
                        } else {
                            Log.d(TAG,"pdc config list is empty");
                            trySetconfig();
                        }
                        break;
                }
            }
        };

        private BroadcastReceiver mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                Log.i(TAG, "Receive the action: " + action);

                int actionMode = MODE_NONE;
                if (ACTION_PDC_CONFIGS_CLEARED.equals(action)) {
                    actionMode = MODE_NONE;
                } else if (ACTION_PDC_DATA_RECEIVED.equals(action)) {
                    actionMode = MODE_SET;
                }else if (ACTION_PDC_CONFIG_LIST_RECEIVED.equals(action)) {
                    byte[] result = intent.getByteArrayExtra("list_data");
                    Message msg = mHandler.obtainMessage(MSG_EVENT_PDC_CONFIG_LIST_DONE);
                    msg.obj = result;
                    msg.sendToTarget();
                }

                if (actionMode != mCurActionMode) {
                    // We need wait for the current action finish.
                    return;
                }

                byte[] activeData = intent.getByteArrayExtra(EXTRA_PDC_ACTIVE);
                if (activeData == null) {
                    Log.e(TAG, "The action didn't contains the active data!");
                    mHandler.sendEmptyMessage(MSG_ACTION_FINISHED);
                    return;
                }

                ByteBuffer payload = ByteBuffer.wrap(activeData);
                payload.order(ByteOrder.nativeOrder());
                int res = payload.get();
                Log.i(TAG, "Get the payload's result = " + res);

                switch (res) {
                    case RESULT_SUCCESS:
                    case RESULT_ALREADY_CLEAN:
                        // It means mbn set successfully. so send finished message.
                        Log.i(TAG, "Set the MBN file successfully");
                        updateSlotStatus();
                        sendSetConfigMessage();
                        break;
                    case RESULT_FAILURE:
                    case RESULT_NEED_CLEAN:
                        if (mCurrentMsg != null && mRetryTimes <= MAX_RETRY_TIMES) {
                            Message msg = new Message();
                            msg.what = mCurrentMsg.what;
                            msg.obj= mCurrentMsg.obj;
                            mHandler.sendMessage(msg);
                            mRetryTimes = mRetryTimes + 1;
                            return;
                        } else {
                            // TODO: We don't handle the error message now. If we need
                            // handle the error message, we could call the callback.
                        }
                        sendSetConfigMessage();
                        break;
                    default:
                        Log.w(TAG, "We do not handle this result.");
                        break;
                }
            }
        };

        public UpdateNVItemsTask(Context context, int requestCode, ActionCallback callback,
                Carrier targetCarrier, ArrayList<Carrier> otherCarriers,
                ICarrierAccessCacheService accessCacheService) {
            this(context, requestCode, callback, targetCarrier, otherCarriers, false,
                    accessCacheService);
        }

        public UpdateNVItemsTask(Context context, int requestCode, ActionCallback callback,
                Carrier targetCarrier, ArrayList<Carrier> otherCarriers,
                boolean saveOriginalConfig, ICarrierAccessCacheService accessCacheService) {
            mContext = context;
            mRequestCode = requestCode;
            mCallback = callback;
            mTargetCarrier = targetCarrier;
            mOtherCarriers = otherCarriers;
            mSaveOriginalConfig = saveOriginalConfig;
            mAccessCacheService = accessCacheService;
            if (mQcRilHook == null) {
                mQcRilHook = new QcRilHook(mContext, new QcRilHookCallback() {
                    @Override
                    public void onQcRilHookReady() {
                        if (Utils.DEBUG) Log.i(TAG, "The qcrilhook has been ready.");
                        mReady = true;
                    }
                    @Override
                    public void onQcRilHookDisconnected() {
                        if (Utils.DEBUG) Log.i(TAG, "The qcrilhook has been disconnected.");
                        mReady = false;
                    }
                });
            }
        }

        @Override
        protected void finalize() throws Throwable {
            super.finalize();
            if (mQcRilHook != null) {
                mQcRilHook.dispose();
                mQcRilHook = null;
            }
        }

        @Override
        protected Void doInBackground(Void... v) {
            TelephonyManager telephonyManager = (TelephonyManager) mContext.
                    getSystemService(Context.TELEPHONY_SERVICE);
            mPhoneCount = telephonyManager.getPhoneCount();
            mSlotHasBeenSet = new boolean[mPhoneCount];
            for (int i=0; i<mPhoneCount; i++) {
                mSlotHasBeenSet[i] = false;
            }
            mSlotCurrentConfigId = new String[mPhoneCount];
            mModemSetConfigData = new ModemSetConfigData[mPhoneCount];

            mCurrentMsg = new Message();
            mCurrentMsg.what = MSG_READY_CONFIG;
            mHandler.sendMessage(mCurrentMsg);

            // Send the timeout message as delayed.
            mHandler.sendEmptyMessageDelayed(MSG_ACTION_TIMEOUT, TIMEOUT_MILLIS);
            return null;
        }

        private void updateSlotStatus() {
            for (int i = 0; i < mPhoneCount; i++) {
                if ((mSubMask & (0x0001 << i)) != 0) {
                    mSlotHasBeenSet[i] = true;
                }
            }
        }

        private void handleReadyMessage() {
            // Register the receiver to handle the result.
            IntentFilter filter = new IntentFilter();
            filter.addAction(ACTION_PDC_DATA_RECEIVED);
            filter.addAction(ACTION_PDC_CONFIGS_CLEARED);
            filter.addAction(ACTION_PDC_CONFIG_LIST_RECEIVED);
            mContext.registerReceiver(mReceiver, filter);

            boolean mbnFlag =  SystemProperties.getBoolean("persist.activate_mbn.enabled",false);
            Log.d(TAG, "mbnFlag : " + mbnFlag);
            if(!mbnFlag) {
                return;
            }
            Message newMsg = new Message();
            newMsg.what = MSG_GET_CONFIG;
            mHandler.sendMessage(newMsg);
        }

        private void handleGetMessage() {
            mCurActionMode = MODE_READY;
            for (int i = 0; i < mPhoneCount; i++) {
                mSlotCurrentConfigId[i] = mQcRilHook.qcRilGetConfig(SubscriptionManager.getPhoneId(
                        SubscriptionManager.getSubId(i)[0]));
                if (Utils.DEBUG) Log.d(TAG, "Slot " + i + "'s current config ID is "
                        + mSlotCurrentConfigId[i]);
            }

            Message newMsg = new Message();
            newMsg.what = MSG_DEACTIVATE_CONFIG;
            mHandler.sendMessage(newMsg);
        }

        private void handleDeactivateMessage() {
            boolean result = mQcRilHook.qcRilDeactivateConfigs();
            if (Utils.DEBUG) Log.d(TAG, "Deactivate result: result = " + result);
            sendSetConfigMessage();
        }

        private void handleSetMessage(ModemSetConfigData mscd) {
            mCurActionMode = MODE_SET;
            mSubMask = mscd.getSubMask();
            if (Utils.DEBUG) Log.d(TAG, "Try to set the configuration. " + mscd.toString());
            boolean availableConfigs = mQcRilHook.qcRilGetAllConfigs();
            Log.d(TAG, "availableConfigs = " + availableConfigs);
            if(!availableConfigs) {
                boolean result = mQcRilHook.qcRilSetConfig(mscd.getPath(), mscd.getConfigId(),
                        mscd.getSubMask());
                Log.d(TAG, "unavailableConfigs, Set result: result = " + result);
            }
        }

        private boolean parseConfigList(byte[] data) {
            try {
                ByteBuffer payload = ByteBuffer.wrap(data);
                payload.order(ByteOrder.nativeOrder());

                byte result = payload.get();
                if (result == -1) {
                    Log.e(TAG, "payload result empty");
                    trySetconfig();
                    return false;
                }
                byte moreAvail = payload.get();
                int numConfigs = payload.get();
                if (numConfigs <= 0) {
                    Log.e(TAG, "QCRIL_EVT_HOOK_GET_AVAILABLE_CONFIGS failed w/"
                            + "invalid payload, numStrings = 0");
                    trySetconfig();
                    return false;
                }
                ModemSetConfigData mscd = null;
                boolean hasLoad = false;
                Log.d(TAG, "numConfigs: " + numConfigs + ", is more available: " + moreAvail);
                for (int i = 0; i < numConfigs; i++) {
                    String configIdItem;
                    byte idLen = payload.get();
                    byte configId[] = new byte[idLen];
                    payload.get(configId);
                    try {
                        configIdItem = new String(configId, ENCODING);
                        Log.d(TAG, "numConfigs["+ i + "] : " + "configIdItem = " + configIdItem);
                    } catch (UnsupportedEncodingException e) {
                        Log.d(TAG, "unsupport " + ENCODING);
                        break;
                    }
                    for (int j = 0; j < mPhoneCount; j++) {
                        if(mModemSetConfigData[j] != null) {
                            if (configIdItem.equals(mModemSetConfigData[j].mConfigId)) {
                                hasLoad = true;
                                mscd= mModemSetConfigData[j];
                                break;
                            }
                            mscd= mModemSetConfigData[j];
                        }
                    }
                    if(hasLoad) {
                        break;
                    }
                }

                if (hasLoad) {
                    Log.d(TAG, mscd.getConfigId() + " has been loaded before.");
                    boolean selectResult = mQcRilHook.qcRilSelectConfig(mscd.getConfigId(),
                            mscd.getSubMask());
                    Log.d(TAG, "Select result: result = " + selectResult);
                } else {
                    boolean setResult = mQcRilHook.qcRilSetConfig(mscd.getPath(), mscd.getConfigId(),
                            mscd.getSubMask());
                    Log.d(TAG, "Set result: result = " + setResult);
                }
                return (moreAvail == 1);
            } catch(Exception e) {
                return false;
            }
        }

        private void trySetconfig() {
            for (int i = 0; i < mPhoneCount; i++) {
                if(mModemSetConfigData[i] != null) {
                    boolean setResult = mQcRilHook.qcRilSetConfig(mModemSetConfigData[i].getPath(), mModemSetConfigData[i].getConfigId(),
                            mModemSetConfigData[i].getSubMask());
                    Log.d(TAG, "try to Set result: result = " + setResult);
                }
            }
        }

        private void handleSelectMessage(ModemSetConfigData mscd) {
            mCurActionMode = MODE_SET;
            mSubMask = mscd.getSubMask();
            if (Utils.DEBUG) Log.d(TAG, "Try to select the configId: " + mscd.getConfigId());
            boolean result = mQcRilHook.qcRilSelectConfig(mscd.getConfigId(), mscd.getSubMask());
            Log.d(TAG, "Select result: result = " + result);
        }

        private void handleGetMbnVersion() {
            if (RegionalizationEnvironment.isSupported()) {
                mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
            }
            for (int i = 0; i < mPhoneCount; i++) {
                int phoneId = SubscriptionManager.getPhoneId(SubscriptionManager.getSubId(i)[0]);
                if (Utils.DEBUG) Log.d(TAG, "Try to get mbn version slot: " + i + " PhoneId: " + phoneId);
                if(mModemSetConfigData[i] != null) {
                    String metaInfo = null;
                    byte[] qcVersion = null;
                    String qcVersionString = null;
                    String mbnVersion = null;
                    if(mModemSetConfigData[i].getConfigId() != null) {
                        metaInfo = mQcRilHook.qcRilGetMetaInfoForConfig(mModemSetConfigData[i].getConfigId());
                    }
                    if(mModemSetConfigData[i].getPath() != null) {
                        qcVersion = mQcRilHook.qcRilGetQcVersionOfFile(mModemSetConfigData[i].getPath());
                    }
                    if(metaInfo != null) {
                        mbnVersion = metaInfo;
                    }
                    if(qcVersion != null && qcVersion.length >= 0) {
                        qcVersionString = bytesToHexString(qcVersion,qcVersion.length);
                        if(metaInfo != null) {
                           mbnVersion += "_" + qcVersionString;
                        } else {
                           mbnVersion = qcVersionString;
                        }
                    }
                    Log.d(TAG, "mbnVersion = " + mbnVersion + " metaInfo= " + metaInfo + " qcVersion = " + qcVersionString);
                    if(mbnVersion != null) {
                        //SystemProperties.set("persist.mbn.version", mbnVersion);
                        if(mRegionalizationService != null){
                            try{
                                mRegionalizationService.writeFile(Utils.MBN_VERSION, mbnVersion + "\n", false);
                                Log.d(TAG,".....handleGetMbnVersion....write over ........");
                            }catch (Exception e) {
                                Log.e(TAG, "Exception"+ e.getMessage());
                            }
                        }
                    }else {
                        //if(SystemProperties.get("persist.mbn.version") == null) {
                        if(mRegionalizationService != null){
                        //SystemProperties.set("persist.mbn.version", Build.DISPLAY);
                            try{
                                mRegionalizationService.writeFile(Utils.MBN_VERSION, Build.DISPLAY + "\n", false);
                                Log.d(TAG,".........write default mbn over ........");
                            }catch (Exception e){
                                Log.e(TAG, "Exception"+ e.getMessage());
                            }
                        }
					}
                }
            }
            Message newMsg = new Message();
            newMsg.what = MSG_ACTION_FINISHED;
            mHandler.sendMessage(newMsg);
        }

        private static String bytesToHexString(byte[] bytes, int length) {
            if (bytes == null || length == 0) return null;

            StringBuilder ret = new StringBuilder(2*length);

            for (int i = 0 ; i < length ; i++) {
                int b;
                b = 0x0f & (bytes[i] >> 4);
                ret.append("0123456789abcdef".charAt(b));

                b = 0x0f & bytes[i];
                ret.append("0123456789abcdef".charAt(b));
            }
            return ret.toString();
        }

        private void handleActivateMessage() {
            for (int i = 0; i < mPhoneCount; i++) {
                int phoneId = SubscriptionManager.getPhoneId(SubscriptionManager.getSubId(i)[0]);
                if (Utils.DEBUG) Log.d(TAG, "Try to activate slot: " + i + " PhoneId: " + phoneId);
                boolean result = mQcRilHook.qcRilActivateConfig(phoneId);
                if (Utils.DEBUG) Log.d(TAG, "Activate result = " + result);
            }
            Message newMsg = new Message();
            newMsg.what = MSG_GET_MBN_VERSION;
            mHandler.sendMessage(newMsg);
        }

        private void sendSetConfigMessage() {
            for (int i = 0; i < mPhoneCount; i++) {
                if (mSlotHasBeenSet[i]) {
                    continue;
                }
                if (Utils.DEBUG) Log.d(TAG, "Try to set mbn files for slot: " + i);
                if ((mTargetCarrier != null) && (!mTargetCarrierScanned)
                        && ((mTargetCarrier.getSubMask() & (0x0001 << i)) != 0 )) {
                    if (Utils.DEBUG) Log.d(TAG, "Try to set target carrier's commercial mbn.");
                    mTargetCarrierScanned = true;
                    String mbnFileName = mTargetCarrier.getMBNFileName(mAccessCacheService);
                    if (!TextUtils.isEmpty(mbnFileName)) {
                        String configId = MBN_CONFIG_ID_PREFIX + mTargetCarrier.mName;
                        ModemSetConfigData mscd = new ModemSetConfigData(mbnFileName, 0x0001 << i,
                                configId);
                        mModemSetConfigData[i] = mscd;
                        mCurrentMsg = new Message();
                        mCurrentMsg.what = MSG_SET_CONFIG;
                        mCurrentMsg.obj= mscd;
                        mHandler.sendMessage(mCurrentMsg);
                        return;
                    }
                    if (Utils.DEBUG) Log.d(TAG, "There is no commercial mbn in target carrier.");
                }

                if ((mOtherCarriers != null) && (mOtherCarriers.size() > 0)) {
                    for (int j = 0; j < mOtherCarriers.size(); j++) {
                        Carrier carrier = mOtherCarriers.get(j);
                        if ((carrier.getSubMask() & (0x0001 << i)) != 0) {
                            if (Utils.DEBUG) Log.d(TAG, "Try to set ROW mbn in carrier:" +
                                    carrier.mName);
                            String mbnFileName = carrier.getROWMBNFileName(
                                    mAccessCacheService);
                            if (!TextUtils.isEmpty(mbnFileName)) {
                                int subMask = carrier.getSubMask();
                                String configId = ROW_MBN_CONFIG_ID_PREFIX + carrier.mName;
                                ModemSetConfigData mscd = new ModemSetConfigData(mbnFileName,
                                        0x0001 << i, configId);
                                mModemSetConfigData[i] = mscd;
                                mCurrentMsg = new Message();
                                mCurrentMsg.what = MSG_SET_CONFIG;
                                mCurrentMsg.obj = mscd;
                                mHandler.sendMessage(mCurrentMsg);
                                return;
                            }
                            if (Utils.DEBUG) Log.d(TAG, "There is no ROW mbn in carrier:" +
                                    carrier.mName);
                        }
                    }
                }

                if (mSaveOriginalConfig && !TextUtils.isEmpty(mSlotCurrentConfigId[i])) {
                    if (Utils.DEBUG) Log.d(TAG, "Try to re-select for slot " + i +": " +
                            mSlotCurrentConfigId[i]);
                    ModemSetConfigData mscd = new ModemSetConfigData(null, 0x0001 << i,
                            mSlotCurrentConfigId[i]);
                    mCurrentMsg = new Message();
                    mCurrentMsg.what = MSG_SELECT_CONFIG;
                    mCurrentMsg.obj= mscd;
                    mHandler.sendMessage(mCurrentMsg);
                    return;
                }
            }

            if (Utils.DEBUG) Log.d(TAG, "All carriers have been scanned.");
            Message newMsg = new Message();
            newMsg.what = MSG_ACTIVATE_CONFIG;
            mHandler.sendMessage(newMsg);
        }

        private void writeSubMask(int subMask) {
            if (Utils.DEBUG) Log.d(TAG, "Try to write subMask file.");
            if (subMask == Utils.INVALID_SUB_MASK) {
                Log.d(TAG, "Invalid subMask");
                return;
            }
            if (Utils.isPresetMode()) {
                IRegionalizationService regionalizationService =
                        RegionalizationEnvironment.getRegionalizationService();
                boolean result = false;
                try {
                    result = regionalizationService.writeFile(Utils.SUB_MASK_FILE_PATH,
                            Utils.SUB_MASK_PRE + subMask, false);
                } catch (RemoteException e) {
                    Log.e(TAG, "write the subMask file error. e=" + e);
                    result = false;
                }
                if (result) {
                    if (Utils.DEBUG) Log.i(TAG, "Write subMask=" + subMask +" success.");
                } else {
                    if (Utils.DEBUG) Log.i(TAG, "Write subMask=" + subMask +" failed.");
                }
            } else {
                if (mAccessCacheService == null) {
                    Log.d(TAG, "mAccessCacheService is null");
                    return;
                }
                try {
                    if (mAccessCacheService.writeSubMaskFile(Utils.SUB_MASK_PRE + subMask)) {
                        if (Utils.DEBUG) Log.i(TAG, "Write subMask=" + subMask +" success.");
                    }
                } catch (RemoteException e) {
                    if (Utils.DEBUG) Log.e(TAG, "Failed to write subMask file. e=" + e);
                }
            }
        }

        private class ModemSetConfigData {
            private final String mPath;
            private final int mSubMask;
            private final String mConfigId;

            public ModemSetConfigData(String path, int subMask, String configId) {
                mPath = path;
                mSubMask = subMask;
                mConfigId = configId;
            }

            public String getPath() {
                return mPath;
            }

            public int getSubMask() {
                return mSubMask;
            }

            public String getConfigId() {
                return mConfigId;
            }

            @Override
            public String toString() {
                return "path=" + mPath + " subMask=" + mSubMask + " configId=" + mConfigId;
            }
        }
    }

    /**
     * This task used to start the switch action.
     */
    public static class SwitchCarrierTask extends AsyncTask<SwitchData, Void, Void> {
        private static final String TAG = "SwitchCarrierTask";

        // The string used by switch action.
        private static final String RMFLAG_FILE_NAME               = "rmflag";
        private static final String ACTION_FILE_NAME               = "action";
        private static final String SWITCH_FILE_NAME               = "ota.zip";
        private static final String NEW_CARRIER_STR_PRE            = "newCarrier";
        private static final String NEW_PACK_COUNT_STR_PRE         = "newPackCount=";
        private static final String NEW_SPEC_STR_PRE               = "strNewSpec";
        private static final String PACK_COUNT_STR_PRE             = "packCount=";
        private static final String SPEC_STR_PRE                   = "strSpec";
        private static final String PACK_STORAGE_POS               = "packStorage=";
        private final static String OEM_PARTITION                  = "/oem";
        private final static String SPEC_FILE_PATH                 = "/persist/speccfg/spec";
        private final static String RO_VENOR_PROP_FILE_PATH        = "/persist/speccfg/vendor_ro.prop";
        private final static String PERSIST_VENOR_PROP_FILE_PATH   = "/persist/speccfg/vendor_persist.prop";

        private static final String RESTORE_CARRIERS_URI =
            "content://telephony/carriers/restore";
        private static final Uri DEFAULTAPN_URI = Uri.parse(RESTORE_CARRIERS_URI);

        private static Context mContext = null;
        private int mRequestCode = 0;
        private ActionCallback mCallback = null;
        private static ICarrierAccessCacheService mAccessCacheService;
        private IRegionalizationService mRegionalizationService = null;
        private ArrayList<SwitchData> mDataList = null;

        public SwitchCarrierTask(Context context, int requestCode, ActionCallback callback,
                ICarrierAccessCacheService service) {
            mContext = context;
            mCallback = callback;
            mRequestCode = requestCode;
            mAccessCacheService = service;
            if (RegionalizationEnvironment.isSupported()) {
                mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
            }
        }

        public SwitchCarrierTask(Context context, int requestCode,
                ActionCallback callback, ArrayList<SwitchData> dataList,
                ICarrierAccessCacheService service) {
            mContext = context;
            mCallback = callback;
            mRequestCode = requestCode;
            mDataList = dataList;
            mAccessCacheService = service;
            if (RegionalizationEnvironment.isSupported()) {
                mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
            }
        }

        @Override
        protected Void doInBackground(SwitchData... params) {
            try {
                if (Utils.isFileBasedSwitchEnabled()) {
                    if (mDataList != null)
                        switchToNewCarriers_fileBased(mContext, mDataList);
                    else switchToNewCarrier_fileBased(mContext, params[0]);
                    if (mCallback != null) mCallback.onActionFinished(mRequestCode);
                } else if (Utils.isPresetMode()) {
                    if (mDataList != null)
                        switchToNewCarriers_regionalEnv(mContext, mDataList);
                    else switchToNewCarrier_regionalEnv(mContext, params[0]);
                } else {
                    if (params.length > 1)
                        switchToNewCarrier(mContext, params[0], params[1]);
                    else switchToNewCarrier(mContext, params[0], null);
                    if (mCallback != null) mCallback.onActionFinished(mRequestCode);
                }
            } catch (IOException e) {
                Log.e(TAG, "Something error when switch the carrier, ex = " + e.getMessage());
                if (mCallback != null) mCallback.onActionError(mRequestCode, RESULT_FAILURE, e);
            }
            return null;
        }

        // *_regionalEnv is used for switching carrier without changing system partition
        // and entering into recovery mode.
        private void switchToNewCarrier_regionalEnv(Context context, SwitchData data)
                throws IOException {
            if (data == null) throw new IOException("Switch data is null.");
            Log.i(TAG, "Try to switch to new carrier: " + data._name);
            Log.i(TAG, "Or try to switch with this file: " + data._path);
            String otaPath = "";
            String storagePos = "";

            if (!TextUtils.isEmpty(data._path)) {
                if (data._path.endsWith(SWITCH_FILE_NAME)) {
                    otaPath = data._path;
                    storagePos = OEM_PARTITION;
                } else {
                    storagePos = data._path.substring(0, data._path.lastIndexOf(File.separator));
                }
            } else throw new IOException("Carrier pack's path is null.");

            // Try to use the given new carrier name to switch the carrier.
            if (!TextUtils.isEmpty(data._name)) {
                boolean result = false;
                try {
                    String contents = PACK_STORAGE_POS + storagePos + "\n" +
                            PACK_COUNT_STR_PRE + "1\n" +
                            SPEC_STR_PRE + "1=" + data._name + "\n";
                    result = mRegionalizationService.writeFile(SPEC_FILE_PATH, contents, false);
                    Carrier.setQGPVersionAndDeviceType(data,storagePos);
                    if(data._name.equals("Default")){
                        result &= mRegionalizationService.writeFile(Utils.MBN_VERSION,""+"\n",false);
                    }
                    if (!TextUtils.isEmpty(otaPath)) {
                        // Write spec file to /cache/action for filebased.ota.zip upgrading
                        result &= mAccessCacheService.writeActionFile(contents);
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "write the spec file error.");
                    result = false;
                }
                if (!result) throw new IOException("write spec file failed.");

                // Write prop pointer file VENOR_PROP_FILE first,
                // because properties only can be loaded on the early boot.
                boolean success = false;
                try {
                    success = mRegionalizationService.writeFile(RO_VENOR_PROP_FILE_PATH, "\n", false);
                    success &= mRegionalizationService.writeFile(RO_VENOR_PROP_FILE_PATH,
                            "import " + storagePos + "/" + data._name
                            + "/system/vendor/vendor.prop\n", true);
                    success &= mRegionalizationService.writeFile(PERSIST_VENOR_PROP_FILE_PATH, "\n", false);
                    success &= mRegionalizationService.writeFile(PERSIST_VENOR_PROP_FILE_PATH,
                            "import " + storagePos + "/" + data._name
                            + "/system/vendor/vendor.prop\n", true);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                if (!success) {
                    throw new IOException("write prop file failed.");
                }

                // Wipe data & reboot
                wipeData_regionalEnv(otaPath);
            }
        }

        private void switchToNewCarriers_regionalEnv(Context context, ArrayList<SwitchData> data)
                throws IOException {
            if (data == null || data.size() < 1) throw new IOException("Switch data is null.");
            Log.i(TAG, "Try to switch to new carriers num = " + data.size());
            int packCount = 0;
            String strCarrierName = "";
            String otaPath = "";
            String storagePos = "";
            for (int i = 0; i < data.size(); i++) {
                if (!TextUtils.isEmpty(data.get(i)._path)) {
                    if (data.get(i)._path.endsWith(SWITCH_FILE_NAME)) {
                        otaPath = data.get(i)._path;
                        break;
                    }
                } else throw new IOException("Carrier pack's path is null.");
            }

            if (TextUtils.isEmpty(otaPath)) {
                storagePos = data.get(0)._path.substring(
                        0, data.get(0)._path.lastIndexOf(File.separator));
            } else {
                storagePos = OEM_PARTITION;
            }

            // Write prop pointer file VENOR_PROP_FILE first,
            // because properties only can be loaded on the early boot.
            boolean success = false;
            try {
                success = mRegionalizationService.writeFile(RO_VENOR_PROP_FILE_PATH, "\n", false);
                success &= mRegionalizationService.writeFile(PERSIST_VENOR_PROP_FILE_PATH, "\n", false);
            } catch (RemoteException e) {
                e.printStackTrace();
            }

            // When there are multi packages, the importing order for ro.* and persist.*
            // should be reversed.
            for (int i = 0; i < data.size(); i++) {
                try {
                    if (!TextUtils.isEmpty(data.get(i)._name)) {
                        success &= mRegionalizationService.writeFile(RO_VENOR_PROP_FILE_PATH,
                                "import " + storagePos + "/" + data.get(i)._name
                                + "/system/vendor/vendor.prop\n", true);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            // Reverse the carrier list for installing
            for (int i = data.size() - 1; i >= 0; i--) {
                Log.i(TAG, "Try to switch to carrier " + i + ":" + data.get(i)._name);
                Log.i(TAG, "Or try to switch with this file " + i + ":" + data.get(i)._path);
                if (!TextUtils.isEmpty(data.get(i)._name)) {
                    packCount++;
                    strCarrierName += SPEC_STR_PRE
                            + packCount + "=" + data.get(i)._name + "\n";
                }

                try {
                    if (!TextUtils.isEmpty(data.get(i)._name)) {
                        success &= mRegionalizationService.writeFile(PERSIST_VENOR_PROP_FILE_PATH,
                                "import " + storagePos + "/" + data.get(i)._name
                                + "/system/vendor/vendor.prop\n", true);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            if (!success) {
                throw new IOException("write prop file failed.");
            }

            if (!TextUtils.isEmpty(strCarrierName)) {
                boolean result = false;
                try {
                    String contents = PACK_STORAGE_POS + storagePos + "\n"
                            + PACK_COUNT_STR_PRE + packCount + "\n"
                            + strCarrierName;
                    result = mRegionalizationService.writeFile(SPEC_FILE_PATH, contents, false);
                    Carrier.setQGPVersionAndDeviceType(data.get(0),storagePos);
                    if(data.get(0)._name.equals("Default")){
                        result &= mRegionalizationService.writeFile(Utils.MBN_VERSION,""+"\n",false);
                    }
                    if (!TextUtils.isEmpty(otaPath)) {
                        // Write spec file to /cache/action for filebased.ota.zip upgrading
                        result &= mAccessCacheService.writeActionFile(contents);
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "write the spec file error.");
                    result = false;
                }
                if (!result) throw new IOException("write spec file failed.");

                // Wipe data & reboot
                wipeData_regionalEnv(otaPath);
            }
        }

        // Wipe old carrier's data for new carrier
        private void wipeData_regionalEnv(String otaPath) {
            String wipeLevel = SystemProperties.get("persist.regional.wipedata.level", "");
            File dataDir = Environment.getDataDirectory();
            Utils.setSharedPreference(mContext, Utils.SHARED_PREFERENCE_KEY_RESTORE_3RD_APKS, "");

            try {
                if (wipeLevel.equals("all")) {
                    Log.d(TAG, "Switch carrier under wiping data level = all!");
                    String[] subDir = dataDir.list();
                    if (subDir != null) {
                        for (int i = 0; i < subDir.length; i++) {
                            if (!TextUtils.isEmpty(subDir[i]) &&
                                    !subDir[i].equals("app-regional") &&
                                    !subDir[i].equals("data") &&
                                    !subDir[i].equals("user")) {
                                mRegionalizationService.deleteFilesUnderDir(
                                        new File(dataDir, subDir[i]).getAbsolutePath(),
                                        "", false);
                            }
                        }
                    }
                    mRegionalizationService.deleteFilesUnderDir(
                            new File(dataDir, "switch_spec").getAbsolutePath(), "", true);
                    mRegionalizationService.deleteFilesUnderDir(
                            new File(dataDir, "resource-cache").getAbsolutePath(), "", false);
                    wipeAppData(otaPath);
                } else if (wipeLevel.equals("none")) {
                    Log.d(TAG, "Switch carrier under wiping data level = none!");
                    // Reboot the DUT
                    PowerManager pm =
                            (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
                    pm.reboot("");
                } else {
                    Log.d(TAG, "Switch carrier under wiping data level = default!");
                    mRegionalizationService.deleteFilesUnderDir(
                            new File(dataDir, "misc/wifi").getAbsolutePath(), "", false);
                    mRegionalizationService.deleteFilesUnderDir(
                            new File(dataDir, "resource-cache").getAbsolutePath(), "", false);
                    mRegionalizationService.deleteFilesUnderDir(
                            new File(dataDir, "switch_spec").getAbsolutePath(), "", true);

                    // Wipe media data and reboot system
                    ArrayList<PackageInfo> packages = Utils.getLevelDefaultPackageList();
                    ClearUserDataObserver clearDataObserver = new ClearUserDataObserver(packages);
                    clearDataObserver.mOtaPath = otaPath;
                    PackageManager pm = mContext.getPackageManager();
                    for (int i = 0; i < packages.size(); i++) {
                        PackageInfo packageInfo = packages.get(i);
                        pm.clearApplicationUserData(packageInfo.packageName, clearDataObserver);
                        if (Utils.DEBUG) Log.d(TAG, "packagename = " + packageInfo.packageName);
                    }
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        // Wipe all apps' data
        private void wipeAppData(String otaPath) {
            List<PackageInfo> packages = mContext.getPackageManager().getInstalledPackages(0);
            if(packages == null || packages.size() == 0){
                Toast.makeText(mContext,R.string.alert_packlist_error,Toast.LENGTH_LONG).show();
                return;
            }
            Iterator<PackageInfo> iterator = packages.iterator();
            // Add the exception apps without clearing data
            while (iterator.hasNext()) {
                PackageInfo packageInfo = iterator.next();
                if (Utils.isInExcludedPackageList(packageInfo.packageName)) {
                    if (Utils.DEBUG) Log.d(TAG, "Remove from packages: " + packageInfo.packageName);
                    iterator.remove();
                }
            }
            Utils.addLastPackages(packages);

            boolean hasTelephonyProvider = false;
            for (int i = 0; i < packages.size(); i++) {
                PackageInfo packageInfo = packages.get(i);
                if(packageInfo.packageName.equals("com.android.providers.telephony")){
                    hasTelephonyProvider = true;
                    break;
                }
            }

            if(!hasTelephonyProvider){
                ContentResolver resolver = mContext.getContentResolver();
                resolver.delete(DEFAULTAPN_URI, null, null);
            }

            ClearUserDataObserver clearDataObserver = new ClearUserDataObserver(packages);
            clearDataObserver.mOtaPath = otaPath;
            PackageManager pm = mContext.getPackageManager();

            // Set timer for first package
            MyTimerTask.sNextPack = packages.get(0).packageName;
            Timer timer = new Timer();
            MyTimerTask myTimerTask = new MyTimerTask();
            myTimerTask.mNextPack = packages.get(0).packageName;
            timer.schedule(myTimerTask, MyTimerTask.TIMER_INTERVAL);

            for (int i = 0; i < packages.size(); i++) {
                PackageInfo packageInfo = packages.get(i);
                pm.clearApplicationUserData(packageInfo.packageName, clearDataObserver);
                if (Utils.DEBUG) Log.d(TAG, "packagename = " + packageInfo.packageName);
            }
        }

        // Timer to wipe app's data
        static class MyTimerTask extends TimerTask {
            public static String sNextPack = "";
            public String mNextPack = "";
            public final static int TIMER_INTERVAL = 10 * 1000;

            public void run() {
                synchronized(sNextPack) {
                    if (!TextUtils.isEmpty(sNextPack) && !TextUtils.isEmpty(mNextPack)
                            && mNextPack.equals(sNextPack)) {
                        Log.d(TAG, "Clearing data of " + mNextPack + " is time out!");
                        // Wipe app data faild and go to recovery to wipe
                        boolean result = false;
                        try {
                            result = mAccessCacheService.writeCommandFile(null, null, true);
                        } catch (RemoteException e) {
                            Log.e(TAG, "write the command file error.");
                        }
                        if (result) {
                            PowerManager pm = (PowerManager) mContext.getSystemService(
                                    Context.POWER_SERVICE);
                            pm.reboot(PowerManager.REBOOT_RECOVERY);
                        }
                    }
                }
            }
        }

        private class ClearUserDataObserver extends IPackageDataObserver.Stub {
            private List<PackageInfo> mPackageList = null;
            private int mIndex = 0;
            public String mOtaPath = "";

            public ClearUserDataObserver(List<PackageInfo> packageList) {
                mPackageList = packageList;
                mIndex = 0;
            }

            public void onRemoveCompleted(final String packageName, final boolean succeeded) {
                Log.d(TAG, "onRemoveCompleted() - packageName = "
                        + packageName + " succeeded = " + succeeded);

                mIndex += 1;
                if (mIndex < mPackageList.size()) {
                    synchronized(MyTimerTask.sNextPack) {
                        MyTimerTask.sNextPack = mPackageList.get(mIndex).packageName;
                        Timer timer = new Timer();
                        MyTimerTask myTimerTask = new MyTimerTask();
                        myTimerTask.mNextPack = mPackageList.get(mIndex).packageName;
                        timer.schedule(myTimerTask, MyTimerTask.TIMER_INTERVAL);
                    }
                }

                if (mPackageList.get(mPackageList.size()-1).packageName.equals(packageName)) {
                    Log.d(TAG, "onRemoveCompleted() - last packageName = " + packageName);
                    MyTimerTask.sNextPack = "";
                    if (TextUtils.isEmpty(mOtaPath)) {
                        // Reboot the DUT
                        PowerManager pm =
                                (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
                        pm.reboot("");
                    } else {
                        try {
                            installPackage(mContext, mOtaPath);
                        } catch (IOException e) {
                            Log.e(TAG, "Something error when switch the carrier, ex = "
                                    + e.getMessage());
                            if (mCallback != null)
                                mCallback.onActionError(mRequestCode, RESULT_FAILURE, e);
                        }
                    }
                }
            }
        }

        // "*_fileBased" is used to distinguish the carrier switching through preset package
        // and file-based ota.zip with block-based ota.zip.
        private void switchToNewCarrier_fileBased(Context context, SwitchData data)
                throws IOException {
            if (data == null) throw new IOException("Switch data is null.");
            Log.i(TAG, "Try to switch to new carrier: " + data._name);
            Log.i(TAG, "Or try to switch with this file: " + data._path);

            // Try to use the given new carrier name to switch the carrier.
            if (!TextUtils.isEmpty(data._name)) {
                File actionFile = new File(context.getCacheDir() + "/" + ACTION_FILE_NAME);
                if (actionFile != null && actionFile.exists()) {
                    actionFile.delete();
                }
                actionFile.createNewFile();
                Utils.writeFile(actionFile, NEW_PACK_COUNT_STR_PRE + "1\n", false);
                Utils.writeFile(actionFile, NEW_SPEC_STR_PRE + "1=" + data._name + "\n", true);
                if (TextUtils.isEmpty(data._path)) {
                    try {
                        // Copy the empty ota.zip from system/vendor/speccfg/ to /cache/.
                        String otaPath = mAccessCacheService.copyFile(
                                Carrier.SPEC_CONFIG_PATH + SWITCH_FILE_NAME);
                        if (!TextUtils.isEmpty(otaPath)) {
                            installPackage(context, otaPath);
                        }
                    } catch (RemoteException e) {
                         Log.e(TAG, "copy the ota.zip file to /cache error.");
                    }
                }
            }

            // Try to use the given new carrier path to switch the carrier.
            if (!TextUtils.isEmpty(data._path)) {
                installPackage(context, data._path);
            }
        }

        private void switchToNewCarriers_fileBased(Context context, ArrayList<SwitchData> data)
                throws IOException {
            if (data == null || data.size() < 1) throw new IOException("Switch data is null.");
            Log.i(TAG, "Try to switch to new carriers num = " + data.size());
            int packCount = 0;
            String strCarrierName = "";
            String path = "";
            // Reverse the carrier list for installing
            for (int i = data.size() - 1; i >= 0; i--) {
                Log.i(TAG, "Try to switch to carrier " + i + ":" + data.get(i)._name);
                Log.i(TAG, "Or try to switch with this file " + i + ":" + data.get(i)._path);
                if (!TextUtils.isEmpty(data.get(i)._name)) {
                    packCount++;
                    strCarrierName += NEW_SPEC_STR_PRE
                            + packCount + "=" + data.get(i)._name + "\n";
                }
                if (!TextUtils.isEmpty(data.get(i)._path) &&
                        data.get(i)._path.endsWith(SWITCH_FILE_NAME)) {
                    path = data.get(i)._path;
                    File rmflagFile = new File(context.getCacheDir() + "/" + RMFLAG_FILE_NAME);
                    rmflagFile.createNewFile();
                }

            }
            if (!TextUtils.isEmpty(strCarrierName)) {
                File actionFile = new File(context.getCacheDir() + "/" + ACTION_FILE_NAME);
                if (actionFile != null && actionFile.exists()) {
                    actionFile.delete();
                }
                actionFile.createNewFile();
                Utils.writeFile(actionFile, NEW_PACK_COUNT_STR_PRE + packCount + "\n", false);
                Utils.writeFile(actionFile, strCarrierName, true);
            }

            // Try to use the given new carrier path to switch the carrier.
            if (!TextUtils.isEmpty(path)) {
                installPackage(context, path);
            } else {
                try {
                    // Copy the empty ota.zip from system/vendor/speccfg/ to /cache/.
                    String otaPath = mAccessCacheService.copyFile(
                            Carrier.SPEC_CONFIG_PATH + SWITCH_FILE_NAME);
                    if (!TextUtils.isEmpty(otaPath)) {
                        installPackage(context, otaPath);
                    }
                } catch (RemoteException e) {
                     Log.e(TAG, "copy the ota.zip file to /cache error.");
                }
            }
        }

        private void switchToNewCarrier(Context context, SwitchData data, SwitchData data2) throws IOException {
            if (data == null) throw new IOException("Switch data is null.");
            Log.i(TAG, "Try to switch to new carrier: " + data._name);
            Log.i(TAG, "Or try to switch with this file: " + data._path);

            // Try to use the given new carrier path to switch the carrier.
            if (data2 != null && !TextUtils.isEmpty(data2._path)) {
                Log.d(TAG,"write in action file path=" + data2._path);
                boolean result = false;
                try {
                    String contents = NEW_CARRIER_STR_PRE + "=" + data2._path + "\n";
                    result = mAccessCacheService.writeActionFile(contents);
                } catch (RemoteException e) {
                    Log.e(TAG, "write the action file error.");
                    result = false;
                }
                if (!result) throw new IOException("write action file failed.");
            }

            // Try to use the given new carrier path to switch the carrier.
            if (!TextUtils.isEmpty(data._path)) {
                Log.i(TAG,"installPackage filepath = " + data._path);
                installPackage(context, data._path);
            }
        }

        private void installPackage(Context context, String filePath) throws IOException {

            final String filepathStr = "--update_package=" + filePath;
            final String localeStr = "--locale=" + Locale.getDefault().toString();
            boolean result = false;
            try {
                result = mAccessCacheService.writeCommandFile(filepathStr, localeStr, false);
            } catch (RemoteException e) {
                Log.e(TAG, "write the command file error.");
                result = false;
            }
            if (!result) throw new IOException("write command file failed.");

            Utils.setSharedPreference(context, Utils.SHARED_PREFERENCE_KEY_IS_FIRST_BOOT, "");

            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            pm.reboot(PowerManager.REBOOT_RECOVERY);

            throw new IOException("Reboot failed (no permissions?)");
        }
    }
}

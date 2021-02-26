/**
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.phonefeature;

import android.app.AlertDialog;
import android.app.Service;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.os.SystemVibrator;
import android.os.Vibrator;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.telephony.DisconnectCause;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Toast;

import com.android.internal.telephony.BlockChecker;
import com.android.internal.telephony.Call;
import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.Connection;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;

public class FeatureService extends Service {

    private RoamingSetting mRoamingSetting;
    private SimDetector mSimDetector;
    private Call.State mLastFgCallState = Call.State.IDLE;

    public static final String ACTION_ENABLE_TDD_LTE =
            "com.qualcomm.qti.phonefeature.ENABLE_TDD_LTE";
    public static final String ACTION_SET_PREF_NETWORK =
            "com.qualcomm.qti.phonefeature.SET_PREF_NETWORK";
    public static final String ACTION_SET_SMSC =
            "com.qualcomm.qti.phonefeature.SET_SMSC";
    public static final String ACTION_GET_SMSC =
            "com.qualcomm.qti.phonefeature.GET_SMSC";

    private static final String SP_PROVISION = "provision";

    private static final long TDD_REGISTER_TIMEOUT = 3 * 60 * 1000;

    private static final int EVENT_ROAMING_CHANGED = 1;
    private static final int EVENT_SERVICE_CHANGED = 2;
    private static final int EVENT_CALL_STATE_CHANGED = 3;
    private static final int EVENT_CALL_DISCONNECTED = 4;
    private static final int EVENT_TIMEOUT_SET_TDD_LTE = 5;
    private static final int EVENT_TIMEOUT_SET_TDD_LTE_AGAIN = 6;
    private static final int EVENT_SET_TDD_LTE_DONE = 7;

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_ROAMING_CHANGED:
                    Log.d(AppGlobals.TAG, "on EVENT_ROAMING_CHANGED");
                    break;
                case EVENT_SERVICE_CHANGED:
                    int slotId = (Integer) ((AsyncResult) msg.obj).result;
                    int dds = SubscriptionManager
                            .getSlotId(SubscriptionManager.getDefaultDataSubscriptionId());
                    Log.d(AppGlobals.TAG, "EVENT_SERVICE_CHANGED on slot=" + slotId + ", dds="
                            + dds);
                    if (slotId == dds) {
                        ApnController.getInstance(FeatureService.this).updateApn();
                    }
                    break;
                case EVENT_CALL_STATE_CHANGED:
                    onPhoneStateChanged();
                    break;
                case EVENT_CALL_DISCONNECTED:
                    onCallDisconnected((AsyncResult) msg.obj);
                    break;
                case EVENT_TIMEOUT_SET_TDD_LTE:
                case EVENT_TIMEOUT_SET_TDD_LTE_AGAIN:
                    int serviceState = AppGlobals.getInstance().mPhones[(Integer) msg.obj]
                            .getServiceState().getDataRegState();
                    Log.d(AppGlobals.TAG, "on EVENT_TIMEOUT_SET_TDD_LTE, band="
                            + getBand((Integer) msg.obj) + ", data_service_state=" + serviceState);
                    if (getBand((Integer) msg.obj) == Constants.NW_BAND_LTE_TDD
                            && serviceState != ServiceState.STATE_IN_SERVICE) {
                        alertNotRegisteredInTDD((Integer) msg.obj,
                                msg.what == EVENT_TIMEOUT_SET_TDD_LTE);
                    }
                    break;

                case EVENT_SET_TDD_LTE_DONE:
                    Log.d(AppGlobals.TAG, "on EVENT_SET_TDD_LTE_DONE, band=" + getBand(msg.arg1));
                    if (getBand(msg.arg1) == Constants.NW_BAND_LTE_TDD) {
                        sendMessageDelayed(obtainMessage(EVENT_TIMEOUT_SET_TDD_LTE, msg.arg1),
                                TDD_REGISTER_TIMEOUT);
                    }
                    Message callback = null;
                    if (msg.obj != null) {
                        callback = (Message) ((Bundle) msg.obj)
                                .getParcelable(FeatureProvider.EXTRA_USEROBJ);
                    }
                    Constants.response(null, callback);
                    break;
            }
        }

    };

    @Override
    public void onCreate() {
        super.onCreate();
        mSimDetector = new SimDetector(this);
        if (getResources().getBoolean(R.bool.feature_roaming_bind_data)) {
            mRoamingSetting = new RoamingSetting(this);
        }

        AppGlobals.getInstance().mServiceMonitor.registerRoamingStateChanged(mHandler,
                EVENT_ROAMING_CHANGED, null);
        AppGlobals.getInstance().mServiceMonitor.registerServiceStateChanged(mHandler,
                EVENT_SERVICE_CHANGED, null);
        CallManager.getInstance().registerForPreciseCallStateChanged(mHandler,
                EVENT_CALL_STATE_CHANGED, null);
        CallManager.getInstance().registerForDisconnect(mHandler, EVENT_CALL_DISCONNECTED, null);
    }

    protected void onCallDisconnected(AsyncResult r) {
        Connection c = (Connection) r.result;
        final String number = c.getAddress();
        if (!BlockChecker.isBlocked(getApplicationContext(), number)) {
            showCallDurationIfNeed(c);
        }
    }

    protected void showCallDurationIfNeed(Connection c) {
        if (c == null) {
            Log.d(AppGlobals.TAG, "not need show duration, connection is null");
            return;
        }

        int cause = c.getDisconnectCause();
        if (cause == DisconnectCause.INCOMING_MISSED
                || cause == DisconnectCause.CALL_BARRED
                || cause == DisconnectCause.FDN_BLOCKED
                || cause == DisconnectCause.CS_RESTRICTED
                || cause == DisconnectCause.CS_RESTRICTED_EMERGENCY
                || cause == DisconnectCause.CS_RESTRICTED_NORMAL
                || cause == DisconnectCause.NOT_VALID
                || cause == DisconnectCause.INCOMING_REJECTED
                || cause == DisconnectCause.IMS_MERGED_SUCCESSFULLY){
            Log.d(AppGlobals.TAG, "not need show duration, caused by " + cause);
            return;
        } else if (Settings.System.getInt(getContentResolver(), "show_call_duration", 1) != 1) {
            Log.d(AppGlobals.TAG, "not need show duration, setting is disabled ");
            return;
        }

        long duration = 0;
        if (c.getConnectTime() != 0) {
            duration = (System.currentTimeMillis() - c.getConnectTime()) / 1000;
        }
        long minutes = 0;
        long seconds = 0;

        if (duration >= 60) {
            minutes = duration / 60;
        }
        seconds = duration % 60;
        Toast.makeText(this, getResources().getString(R.string.duration) + minutes +
                getResources().getString(R.string.mins) + seconds +
                getResources().getString(R.string.secs),
                Toast.LENGTH_SHORT).show();

    }

    protected void onPhoneStateChanged() {
        Call.State state = CallManager.getInstance().getActiveFgCallState();
        if (mLastFgCallState.isDialing() && state == Call.State.ACTIVE) {
            vibrateAfterCallConnected();
        }
        mLastFgCallState = state;
    }

    private void vibrateAfterCallConnected() {
        int defaultVibrateEnabled = getResources()
                    .getInteger(R.integer.config_default_vibrate_after_connected);
        if (Settings.System.getInt(getContentResolver(), "vibrate_on_accepted",
                defaultVibrateEnabled) == 1) {
            Vibrator mSystemVibrator = new SystemVibrator();
            int nVibratorLength = 100;
            mSystemVibrator.vibrate(nVibratorLength);
            SystemClock.sleep(nVibratorLength);
            mSystemVibrator.cancel();
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null){
            if (ACTION_SET_PREF_NETWORK.equals(intent.getAction())) {
                setPrefNetwork(intent.getExtras());
            } else if (ACTION_SET_SMSC.equals(intent.getAction())) {
                int slot = intent.getExtras().getInt(PhoneConstants.SLOT_KEY, 0);
                String smsc = intent.getExtras().getString(FeatureProvider.EXTRA_SMSC);
                Message message = (Message) intent.getExtras()
                        .getParcelable(FeatureProvider.EXTRA_CALLBACK);
                new Smsc(slot, message).set(smsc);
            } else if (ACTION_GET_SMSC.equals(intent.getAction())) {
                int slot = intent.getExtras().getInt(PhoneConstants.SLOT_KEY, 0);
                Message message = (Message) intent.getExtras()
                        .getParcelable(FeatureProvider.EXTRA_CALLBACK);
                new Smsc(slot, message).get();
            } else if(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED.equals(intent.getAction())) {
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY, 0);
                AppGlobals.getInstance().mCardMonitor.handleRadioTechnologyChanged(phoneId);
            } else if(Intent.ACTION_SHUTDOWN.equals(intent.getAction()) &&
                    !intent.getBooleanExtra(Intent.EXTRA_SHUTDOWN_USERSPACE_ONLY, false)) {
                AppGlobals.getInstance().mCardMonitor.handleShutdown();
            } else if(TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(intent.getAction())) {
                String simStatus = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                            SubscriptionManager.INVALID_SIM_SLOT_INDEX);
                AppGlobals.getInstance().mCardMonitor.handleSimStateChanged(phoneId, simStatus);
            }
        }
        return super.onStartCommand(intent, flags, startId);
    }

    private int getBand(int slot) {
        int band = Constants.NW_BAND_LTE_NV;
        try {
            band = TelephonyManager.getIntAtIndex(getContentResolver(),
                    Constants.SETTING_NETWORK_BAND, slot);
        } catch (SettingNotFoundException e) {
        }
        return band;
    }

    private void alertNotRegisteredInTDD(final int slot, final boolean notifyAgin) {
        DialogInterface.OnClickListener listener = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (which == DialogInterface.BUTTON_NEGATIVE) {
                    int prefNetwork = Phone.NT_MODE_GSM_ONLY;
                    try {
                        prefNetwork = TelephonyManager.getIntAtIndex(getContentResolver(),
                                Constants.SETTING_DEFAULT_PREF_NETWORK_MODE, slot);
                    } catch (SettingNotFoundException e) {
                        Log.e(AppGlobals.TAG, "onClick: can not get previous network", e);
                        return;
                    }
                    setPrefNetwork(slot, prefNetwork, Constants.NW_BAND_LTE_FDD, null, null);
                } else if (notifyAgin) {
                    mHandler.sendMessageDelayed(
                            mHandler.obtainMessage(EVENT_TIMEOUT_SET_TDD_LTE_AGAIN, slot),
                            TDD_REGISTER_TIMEOUT);
                }
            }
        };
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        AlertDialog dialog = builder.setTitle(R.string.alert_title)
                .setMessage(R.string.message_not_registered_in_tdd)
                .setNegativeButton(R.string.choose_yes, listener)
                .setNeutralButton(R.string.choose_no, listener).create();
        dialog.getWindow().setType(
                WindowManager.LayoutParams.TYPE_SYSTEM_DIALOG);
        dialog.show();
    }

    private void updatePrefNetwork(int slot) {
        int prefNetwork = Phone.NT_MODE_GSM_ONLY;
        try {
            prefNetwork = TelephonyManager.getIntAtIndex(getContentResolver(),
                    Settings.Global.PREFERRED_NETWORK_MODE, slot);
        } catch (SettingNotFoundException e) {
            Log.e(AppGlobals.TAG, "updatePrefNetwork: failed to get network on slot" + slot, e);
            return;
        }
        int band = getBand(slot);
        if (band == Constants.NW_BAND_LTE_NV || band == Constants.NW_BAND_LTE_DEFAULT) {
            setPrefNetwork(slot, prefNetwork, null, null, null);
        } else {
            setPrefNetwork(slot, prefNetwork, band, null, null);
        }
    }

    private void setPrefNetwork(Bundle bundle) {
        Bundle response = new Bundle();
        int slot = bundle.getInt(PhoneConstants.SLOT_KEY, 0);
        int band = bundle.getInt(FeatureProvider.EXTRA_BAND, -1);
        int network = bundle.getInt(FeatureProvider.EXTRA_NETWORK, -1);
        int acq = bundle.getInt(FeatureProvider.EXTRA_ACQ_ORDER, -1);
        Message callback = (Message) bundle.getParcelable(FeatureProvider.EXTRA_CALLBACK);
        String error = null;
        if (band == -1 && network == -1) {
            error = "network is empty";
        } else if (band != -1
                && (band < Constants.NW_BAND_LTE_FULL || band > Constants.NW_BAND_LTE_FDD)) {
            error = String.format("band is not in [%d, %d]", Constants.NW_BAND_LTE_FULL,
                    Constants.NW_BAND_LTE_FDD);
        } else if (acq != -1 && acq != Constants.NW_ACQ_ORDER_LTE_TDS_GSM
                && acq != Constants.NW_ACQ_ORDER_LTE_UMTS_GSM) {
            error = String.format("acq is not %d & %d]", Constants.NW_ACQ_ORDER_LTE_TDS_GSM,
                    Constants.NW_ACQ_ORDER_LTE_UMTS_GSM);
        }
        if (error != null) {
            response.putSerializable(FeatureProvider.EXTRA_EXCEPTION, new IllegalArgumentException(
                    error));
            Constants.response(response, callback);
            return;
        }
        if (network == -1) {
            try {
                network = band == Constants.NW_BAND_LTE_TDD ? Phone.NT_MODE_LTE_ONLY
                        : TelephonyManager.getIntAtIndex(getContentResolver(),
                                Constants.SETTING_DEFAULT_PREF_NETWORK_MODE, slot);
            } catch (SettingNotFoundException e) {
                response.putSerializable(FeatureProvider.EXTRA_EXCEPTION, e);
                Constants.response(response, callback);
                return;
            }
        }
        setPrefNetwork(slot, network, band == -1 ? null : band, acq == -1 ? null : band, callback);
    }

    private void setPrefNetwork(int slot, int network, Integer band, Integer acq,
            Message callback) {
        mHandler.removeMessages(EVENT_TIMEOUT_SET_TDD_LTE);
        mHandler.removeMessages(EVENT_TIMEOUT_SET_TDD_LTE_AGAIN);
        Message msg = callback;
        int prefNetwork = network;
        if (band != null && band == Constants.NW_BAND_LTE_TDD) {
            msg = mHandler.obtainMessage(EVENT_SET_TDD_LTE_DONE, callback);
            msg.arg1 = slot;
            // Avoid handler becomes null
            msg.replyTo = new Messenger(mHandler);
        }
        PrefNetworkRequest request = new PrefNetworkRequest(this, slot, prefNetwork, msg);
        // ACQ is available for all networks, so keep it not be changed.
        if (acq != null) {
            request.setAcqOrder(acq);
        } else {
            try {
                request.setAcqOrder(TelephonyManager.getIntAtIndex(getContentResolver(),
                        Constants.SETTING_ACQ, slot));
            } catch (SettingNotFoundException e) {
                // do nothing since this is called were nobody can react.
            }
        }
        if (band != null) {
            request.setBand(band);
        }
        request.loop();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mSimDetector != null) {
            mSimDetector.dispose();
        }
        if (mRoamingSetting != null) {
            mRoamingSetting.dispose();
        }
        CallManager.getInstance().unregisterForDisconnect(mHandler);
        CallManager.getInstance().unregisterForPreciseCallStateChanged(mHandler);
        AppGlobals.getInstance().mServiceMonitor.unregisterRoamingStateChanged(mHandler);
        AppGlobals.getInstance().mServiceMonitor.unregisterServiceStateChanged(mHandler);
    }
}

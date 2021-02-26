/*=========================================================================
  WbcService.java
  DESCRIPTION
  Wipower Battery Control Service

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  =========================================================================*/

package com.quicinc.wbcservice;

import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.util.Log;
import android.util.Slog;
import android.os.RemoteException;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Message;
import android.provider.Settings;
import android.os.SystemProperties;
import android.os.UserHandle;

import com.quicinc.wbc.IWbcEventListener;
import com.quicinc.wbc.IWbcService;
import com.quicinc.wbc.WbcTypes;

import java.util.HashMap;
import java.util.Map;

public class WbcService extends IWbcService.Stub implements WipowerBatteryControl.WbcEventListener {
    private static boolean sDbg = false;
    private static final boolean DEBUG = true;
    private static final String TAG = "Wbc-Svc";

    private static final int MSG_INTERNAL_BASE = 1000;
    private static final int MSG_INTERNAL_START = 1 + MSG_INTERNAL_BASE;

    private static final int MSG_SYSTEM_BASE = 100;
    private static final int MSG_BATTERY_CHANGED         = 1 + MSG_SYSTEM_BASE;
    private static final int MSG_BLUETOOTH_STATE_CHANGED = 2 + MSG_SYSTEM_BASE;
    private static final int MSG_POWER_CONNECTED         = 3 + MSG_SYSTEM_BASE;
    private static final int MSG_POWER_DISCONNECTED      = 4 + MSG_SYSTEM_BASE;
    private static final int MSG_BOOT_COMPLETED          = 5 + MSG_SYSTEM_BASE;

    private static final int MSG_WBC_HAL_EVENT_BASE = 0;
    private static final int MSG_WBC_HAL_EVENT_WIPOWER_CAPABILITY      = 1 + MSG_WBC_HAL_EVENT_BASE;
    private static final int MSG_WBC_HAL_EVENT_PTU_PRESENCE            = 2 + MSG_WBC_HAL_EVENT_BASE;
    private static final int MSG_WBC_HAL_EVENT_WIPOWER_CHARGING_STATUS = 3 + MSG_WBC_HAL_EVENT_BASE;
    private static final int MSG_WBC_HAL_EVENT_BATTERY_CHARGE_COMPLETE_STATUS = 4 + MSG_WBC_HAL_EVENT_BASE;
    private static final int MSG_WBC_HAL_EVENT_USB_CHARGING_PRESENT    = 5 + MSG_WBC_HAL_EVENT_BASE;
    private static final int MSG_WBC_HAL_EVENT_BATTERY_OVERHEAT        = 6 + MSG_WBC_HAL_EVENT_BASE;

    private static final long BOOT_COMPLETED_SEND_DELAY_MS = 10000; // 10 seconds

    private final Context mContext;
    private HandlerThread mHandlerThread;
    private ServiceHandler mServiceHandler;
    private BluetoothAdapter mBluetoothAdapter = null;
    public WipowerBatteryControl mWbcInterface;

    private final Map<IBinder, ListenerMonitor> mClientListeners = new HashMap<IBinder, ListenerMonitor>();

    private StatusChangeReceiver mReceiver = new StatusChangeReceiver();

    public WbcService(Context context) {
        mContext = context;
        mWbcInterface = new WipowerBatteryControl(this);

        String debug = SystemProperties.get("persist.wbc.log_level", "0");
        sDbg = debug.equals("0") ? false : true;

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothAdapter.ACTION_BLE_STATE_CHANGED);
        intentFilter.addAction(Intent.ACTION_BOOT_COMPLETED);
        if (DEBUG) {
            intentFilter.addAction("com.quicinc.wbcservice.action.WIPOWER_CAPABLE_STATUS");
            intentFilter.addAction("com.quicinc.wbcservice.action.PTU_PRESENCE_STATUS");
            intentFilter.addAction("com.quicinc.wbcservice.action.WIPOWER_CHARGING_ACTIVE_STATUS");
            intentFilter.addAction("com.quicinc.wbcservice.action.CHARGE_COMPLETE_STATUS");
            intentFilter.addAction("com.quicinc.wbcservice.action.USB_CHARGING_PRESENT");
            intentFilter.addAction("com.quicinc.wbcservice.action.BATTERY_OVERHEAT");
        }
        context.registerReceiver(mReceiver, intentFilter);

        mHandlerThread= new HandlerThread("WbcSvcThread", Process.THREAD_PRIORITY_BACKGROUND);
        mHandlerThread.start();
        mServiceHandler = new ServiceHandler(mHandlerThread.getLooper());
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        Message msg = mServiceHandler.obtainMessage();
        msg.what = MSG_INTERNAL_START;
        msg.arg1 = 0;
        msg.arg2 = 0;
        mServiceHandler.sendMessageDelayed(msg, 2000);
    }

    public void echo(int val) {
        mWbcInterface.echo(val);
    }

    public int getWipowerCapable() {
        return mWbcInterface.getWipowerCapable();
    }

    public int getPtuPresence() {
        return mWbcInterface.getPtuPresence();
    }

    public int getWipowerCharging() {
        return mWbcInterface.getWipowerCharging();
    }

    public int getChargeComplete() {
        return mWbcInterface.getChargeComplete();
    }

    public int getUSBChargingPresent() {
        return mWbcInterface.getUSBChargingPresent();
    }

    public int getBatteryOverheat() {
        return mWbcInterface.getBatteryOverheat();
    }

    private final class ListenerMonitor implements IBinder.DeathRecipient {
        private final IWbcEventListener mListener;

        public ListenerMonitor(IWbcEventListener listener) {
            mListener = listener;
        }

        public IWbcEventListener getListener() {
            return mListener;
        }

        @Override
        public void binderDied() {
            WbcService.this.unregister(this.mListener);
        }
    }

    public void register(IWbcEventListener listener) throws RemoteException {
        if (listener != null) {
            IBinder binder = listener.asBinder();
            synchronized (mClientListeners) {
                if (!mClientListeners.containsKey(binder)) {
                    ListenerMonitor listenerMonitor = new ListenerMonitor(listener);
                    binder.linkToDeath(listenerMonitor, 0);
                    mClientListeners.put(binder, listenerMonitor);
                }
            }
        }
    }

    public void unregister(IWbcEventListener listener) {
        if (listener != null) {
            IBinder binder = listener.asBinder();
            synchronized (mClientListeners) {
                ListenerMonitor listenerMonitor = mClientListeners.remove(binder);
                if (listenerMonitor != null) {
                    binder.unlinkToDeath(listenerMonitor, 0);
                }
            }
        }
    }

    @Override
    public void onWbcEventUpdate(int what, int arg1, int arg2) {
        while (mServiceHandler == null) {
            synchronized (this) {
                try {
                    wait(100);
                } catch (InterruptedException e) {
                }
            }
        }

        if (sDbg) Log.v(TAG, "onWbcEventUpdate rcvd: " + what + ", " + arg1 + ", " + arg2);

        Message msg = mServiceHandler.obtainMessage();

        switch (what) {
        case WipowerBatteryControl.WBC_EVENT_TYPE_WIPOWER_CAPABLE_STATUS:
            msg.what = MSG_WBC_HAL_EVENT_WIPOWER_CAPABILITY;
            break;
        case WipowerBatteryControl.WBC_EVENT_TYPE_PTU_PRESENCE_STATUS:
            msg.what = MSG_WBC_HAL_EVENT_PTU_PRESENCE;
            break;
        case WipowerBatteryControl.WBC_EVENT_TYPE_WIPOWER_CHARGING_ACTIVE_STATUS:
            msg.what = MSG_WBC_HAL_EVENT_WIPOWER_CHARGING_STATUS;
            break;
        case WipowerBatteryControl.WBC_EVENT_TYPE_CHARGE_COMPLETE:
            msg.what = MSG_WBC_HAL_EVENT_BATTERY_CHARGE_COMPLETE_STATUS;
            break;
        case WipowerBatteryControl.WBC_EVENT_TYPE_USB_CHARGING_PRESENT:
            msg.what = MSG_WBC_HAL_EVENT_USB_CHARGING_PRESENT;
            break;
        case WipowerBatteryControl.WBC_EVENT_TYPE_BATTERY_OVERHEAT:
            msg.what = MSG_WBC_HAL_EVENT_BATTERY_OVERHEAT;
            break;
        default:
            if (sDbg) Log.w(TAG, "Rcvd unknown WBC event: " + what);
            msg.what = what;
            break;
        }

        msg.arg1 = arg1;
        msg.arg2 = arg2;
        mServiceHandler.sendMessage(msg);
    }

    private final class StatusChangeReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Message msg = mServiceHandler.obtainMessage();
            long sendDelayMs = 0;

            if (intent.getAction().equals(BluetoothAdapter.ACTION_BLE_STATE_CHANGED)) {
                if (sDbg) Log.v(TAG, "Bluetooth changed, extra_state: "
                        + intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, -1)
                        + ", extra_prev_state: " + intent.getIntExtra(BluetoothAdapter.EXTRA_PREVIOUS_STATE, -1));
                msg.what = MSG_BLUETOOTH_STATE_CHANGED;
            } else if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
                if (sDbg) Log.v(TAG, "Rcvd: ACTION_BOOT_COMPLETED");
                msg.what = MSG_BOOT_COMPLETED;

                String delay = SystemProperties.get("persist.wbc.bcd", "0");
                sendDelayMs = Long.parseLong(delay);
                if (sendDelayMs <= 0) {
                    sendDelayMs = BOOT_COMPLETED_SEND_DELAY_MS;
                }
            }

            msg.arg1 = 0;
            msg.arg2 = 0;
            msg.obj = intent;

            if (DEBUG) {
                if (intent.getAction().equals("com.quicinc.wbcservice.action.PTU_PRESENCE_STATUS")) {
                    msg.what = MSG_WBC_HAL_EVENT_PTU_PRESENCE;
                    msg.arg1 = intent.getIntExtra("arg1", 0);
                    Log.v(TAG, "DBG Intent: PTU_PRESENCE_STATUS");
                } else if (intent.getAction().equals("com.quicinc.wbcservice.action.WIPOWER_CHARGING_ACTIVE_STATUS")) {
                    msg.what = MSG_WBC_HAL_EVENT_WIPOWER_CHARGING_STATUS;
                    msg.arg1 = intent.getIntExtra("arg1", 0);
                    Log.v(TAG, "DBG Intent: WIPOWER_CHARGING_ACTIVE_STATUS");
                }  else if (intent.getAction().equals("com.quicinc.wbcservice.action.CHARGE_COMPLETE_STATUS")) {
                    msg.what = MSG_WBC_HAL_EVENT_BATTERY_CHARGE_COMPLETE_STATUS;
                    msg.arg1 = intent.getIntExtra("arg1", 0);
                    Log.v(TAG, "DBG Intent: CHARGE_COMPLETE_STATUS");
                }  else if (intent.getAction().equals("com.quicinc.wbcservice.action.USB_CHARGING_PRESENT")) {
                    msg.what = MSG_WBC_HAL_EVENT_USB_CHARGING_PRESENT;
                    msg.arg1 = intent.getIntExtra("arg1", 0);
                    Log.v(TAG, "DBG Intent: USB_CHARGING_PRESENT");
                }  else if (intent.getAction().equals("com.quicinc.wbcservice.action.BATTERY_OVERHEAT")) {
                    msg.what = MSG_WBC_HAL_EVENT_BATTERY_OVERHEAT;
                    msg.arg1 = intent.getIntExtra("arg1", 0);
                    Log.v(TAG, "DBG Intent: BATTERY OVERHEAT");
                }
            }

            if (sendDelayMs > 0) {
                if (sDbg) Log.v(TAG, "sendMessageDelayed: " + sendDelayMs + " millis");
                mServiceHandler.sendMessageDelayed(msg, sendDelayMs);
            } else {
                mServiceHandler.sendMessage(msg);
            }
        }
    }

    private final class ServiceHandler extends Handler {

        abstract class State {
            void onEntry() { }
            void onExit() { }
            void onBluetoothOn() { }
            void onBluetoothOff() { }
            void onWiPowerPtuPresent() { }
            void onWiPowerPtuRemoved() { }
            void onWiPowerChargingActive() { }
            void onWiPowerChargeComplete() { }
            void onWiPowerNotCharging() { }
            void onWiPowerUSBChargingPresent() { }
            void onWiPowerBatteryOverheat() { }
            void onBootCompleted() { }
        }

        class WiPowerRemoved extends State {
            void onWiPowerPtuPresent() {
                if (isBluetoothOn()) {
                    setState(mWiPowerPresentNotCharging);
                } else {
                    setState(mWiPowerPresentBluetoothOff);
                }
            }
        }

        class WiPowerPresentNotCharging extends State {
            void onEntry() {
                // check if charging is in progress and move to charging state
                if (mWbcInterface.getWipowerCharging() ==
                        WipowerBatteryControl.WBC_WIPOWER_STATUS_CHARGING_ACTIVE) {
                    setState(mWiPowerPresentCharging);
                }
            }

            void onBluetoothOff() {
                setState(mWiPowerPresentBluetoothOff);
            }

            void onWiPowerPtuRemoved() {
                setState(mWiPowerRemoved);
            }

            void onWiPowerChargingActive() {
                setState(mWiPowerPresentCharging);
            }

            void onWiPowerChargeComplete() {
                setState(mWiPowerChargeComplete);
            }

            void onWiPowerUSBChargingPresent() {
                setState(mWiPowerUSBChargingPresent);
            }

            void onWiPowerBatteryOverheat() {
                setState(mWiPowerBatteryOverheat);
            }
        }

        class WiPowerPresentCharging extends State {
            void onEntry() {
                mContext.sendBroadcastAsUser(new Intent(WbcTypes.ACTION_WIPOWER_ICON_ENABLE), UserHandle.ALL);
            }

            void onExit() {
                mContext.sendBroadcastAsUser(new Intent(WbcTypes.ACTION_WIPOWER_ICON_DISABLE), UserHandle.ALL);
            }

            void onBluetoothOff() {
                setState(mWiPowerPresentBluetoothOff);
            }

            void onWiPowerPtuRemoved() {
                setState(mWiPowerRemoved);
            }

            void onWiPowerNotCharging() {
                setState(mWiPowerPresentNotCharging);
            }

            void onWiPowerChargeComplete() {
                setState(mWiPowerChargeComplete);
            }

            void onWiPowerUSBChargingPresent() {
                setState(mWiPowerUSBChargingPresent);
            }

            void onWiPowerBatteryOverheat() {
                setState(mWiPowerBatteryOverheat);
            }
        }

        class WiPowerPresentBluetoothOff extends State {
            void onBluetoothOn() {
                setState(mWiPowerPresentNotCharging);
            }

            void onWiPowerPtuRemoved() {
                setState(mWiPowerRemoved);
            }
        }

        State mWiPowerRemoved;
        State mWiPowerPresentNotCharging;
        State mWiPowerPresentCharging;
        State mWiPowerChargeComplete;
        State mWiPowerUSBChargingPresent;
        State mWiPowerBatteryOverheat;
        State mWiPowerPresentBluetoothOff;

        // current state
        State mState;

        public ServiceHandler(Looper looper) {
            super(looper);

            mWiPowerRemoved = new WiPowerRemoved();
            mWiPowerPresentNotCharging = new WiPowerPresentNotCharging();
            mWiPowerPresentCharging = new WiPowerPresentCharging();
            mWiPowerPresentBluetoothOff = new WiPowerPresentBluetoothOff();

            mState = mWiPowerRemoved;
        }

        void setState(State state) {
            mState.onExit();

            if (sDbg) Log.d(TAG, "State change: " + mState.getClass().getSimpleName() + " --> " + state.getClass().getSimpleName());
            mState = state;

            mState.onEntry();
        }

        boolean isBluetoothOn() {
            mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if (mBluetoothAdapter != null)
               return mBluetoothAdapter.isLeEnabled();
            return false;
        }

        void handleHalPtuEvent(Message msg) {
            switch (msg.arg1) {
            case WipowerBatteryControl.WBC_PTU_STATUS_NOT_PRESENT:
                mState.onWiPowerPtuRemoved();
                break;
            case WipowerBatteryControl.WBC_PTU_STATUS_PRESENT:
                mState.onWiPowerPtuPresent();
                if (!SystemProperties.get("bluetooth.wipower").equals("true") ||
                    !SystemProperties.get("bluetooth.a4wp").equals("true")) {
                    Intent intent = new Intent(WbcTypes.ACTION_PTU_PRESENT);
                    intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
                    mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
                } else {
                    if (sDbg) Log.d(TAG, "No broadcast PTU_PRESENT since A4wp/WipowerSerivce running");
                }
                break;
            default:
                if (sDbg) Log.w(TAG, "Rcvd unknown HAL PTU event: " + msg.arg1);
                break;
            }
        }

        void handleHalWiPowerEvent(Message msg) {
            switch (msg.arg1) {
            case WipowerBatteryControl.WBC_WIPOWER_STATUS_NOT_CHARGING:
                mState.onWiPowerNotCharging();
                break;
            case WipowerBatteryControl.WBC_WIPOWER_STATUS_CHARGING_ACTIVE:
                mState.onWiPowerChargingActive();
                break;
            default:
                if (sDbg) Log.w(TAG, "Rcvd unknown HAL WiPower event: " + msg.arg1);
                break;
            }
        }

        void handleBluetoothEvent(Message msg) {
            if (msg == null || msg.obj == null) {
                Log.e(TAG, "BT event invalid!!");
                return;
            }

            Intent intent = (Intent) msg.obj;
            int btState = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, -1);

            switch (btState) {
                case BluetoothAdapter.STATE_ON:
                case BluetoothAdapter.STATE_BLE_ON:
                    mState.onBluetoothOn();
                    break;
                case BluetoothAdapter.STATE_OFF:
                    mState.onBluetoothOff();
                    break;
                case BluetoothAdapter.STATE_TURNING_ON:
                    break;
                case BluetoothAdapter.STATE_TURNING_OFF:
                    break;
                default:
                    break;
            }
        }

        @Override
        public void handleMessage(Message msg) {
            if (sDbg) Log.d(TAG, "Handler rcvd msg: " + msg.what);

            switch (msg.what) {
            case MSG_INTERNAL_START:
                if (sDbg) Log.d(TAG, "Msg Start");

                int ptuStatus = mWbcInterface.getPtuPresence();
                if (ptuStatus == WipowerBatteryControl.WBC_PTU_STATUS_PRESENT) {
                    if (isBluetoothOn()) {
                        setState(mWiPowerPresentNotCharging);
                    } else {
                        setState(mWiPowerPresentBluetoothOff);
                    }
                } else {
                    ; // do nothing as we default state is WiPowerRemoved
                }
                break;
            case MSG_BLUETOOTH_STATE_CHANGED:
                handleBluetoothEvent(msg);
                break;
            case MSG_WBC_HAL_EVENT_WIPOWER_CAPABILITY:
                notifyClients(msg);
                break;
            case MSG_WBC_HAL_EVENT_PTU_PRESENCE:
                handleHalPtuEvent(msg);
                break;
            case MSG_WBC_HAL_EVENT_WIPOWER_CHARGING_STATUS:
                handleHalWiPowerEvent(msg);
                notifyClients(msg);
                break;
            case MSG_WBC_HAL_EVENT_USB_CHARGING_PRESENT:
                notifyClients(msg);
                break;
            case MSG_WBC_HAL_EVENT_BATTERY_CHARGE_COMPLETE_STATUS:
                notifyClients(msg);
                break;
            case MSG_WBC_HAL_EVENT_BATTERY_OVERHEAT:
                notifyClients(msg);
                break;
            case MSG_BOOT_COMPLETED:
                mState.onBootCompleted();
                break;
            default:
                if (sDbg) Log.w(TAG, "Rcvd Unknown msg: " + msg.what);
                notifyClients(msg);
                break;
            }
        }

        void notifyClients(Message msg) {
            if (mClientListeners != null && !mClientListeners.isEmpty()) {
                if (sDbg) Log.d(TAG, "mClientListeners.entrySet().size() = " + mClientListeners.entrySet().size());
                synchronized (mClientListeners) {
                    for (Map.Entry<IBinder, ListenerMonitor> entry : mClientListeners.entrySet()) {
                        IWbcEventListener listener = entry.getValue().getListener();
                        try {
                            listener.onWbcEventUpdate(msg.what, msg.arg1, msg.arg2);
                        } catch (RemoteException e) {
                            Log.w(TAG, "Error in notifyClients. RemoteException: " + e.getLocalizedMessage());
                            WbcService.this.unregister(listener);
                        }
                    }
                }
            }
        }
    }
}

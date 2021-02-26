/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.Activity;
import android.app.Fragment;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.Loader;
import android.content.Loader.OnLoadCompleteListener;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.os.RegionalizationEnvironment;
import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Actions.ActionCallback;
import com.qualcomm.qti.carrierconfigure.Actions.SwitchCarrierTask;
import com.qualcomm.qti.carrierconfigure.Actions.UpdateNVItemsTask;
import com.qualcomm.qti.carrierconfigure.Carrier.CarriersStorage;
import com.qualcomm.qti.carrierconfigure.Carrier.EmptyPathException;
import com.qualcomm.qti.carrierconfigure.Carrier.NullServiceException;
import com.qualcomm.qti.carrierconfigure.Carrier.SwitchData;
import com.qualcomm.qti.carrierconfigure.Utils.CopyAndUnzipOtaFileThread;
import com.qualcomm.qti.carrierconfigure.Utils.MyAlertDialog;
import com.qualcomm.qti.carrierconfigure.Utils.MyNoticeDialog;
import com.qualcomm.qti.carrierconfigure.Utils.WaitDialog;
import com.qualcomm.qti.loadcarrier.ILoadCarrierService;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class CarrierResetFragment extends Fragment implements OnClickListener, ActionCallback {
    private static final String TAG = "CarrierResetFragment";

    private static final String RE_MNC = "(\\d{2,3})?";
    private static final String SEP_COMMA = ",";

    private static final int RETRY_DELAY = 500;
    private static final int RETRY_COUNT = 10;

    private static final int MSG_START_SWITCH = 0;
    private static final int MSG_SWITCH_ERROR = 1;

    private int mSubMask = 0;
    private String mConfigId = null;

    private int mRetryTimes = 0;
    private Carrier mSwitchToDefaultCarrier = null;
    private CarriersLoader mLoader = null;
    private CarriersStorage mCarriersStorage = null;
    private boolean mIsParsingFinished = false;
    private OnLoadCompleteListener<CarriersStorage> mLoaderListener =
            new OnLoadCompleteListener<CarriersStorage>() {
        @Override
        public void onLoadComplete(Loader<CarriersStorage> loader, CarriersStorage storage) {
            mIsParsingFinished = ((CarriersLoader) loader).isParsingFinished();
            if (mIsParsingFinished) {
                findSwitchToDefaultCarrier(storage);
            }
        }
    };

    private ILoadCarrierService mLoadCarrierService = null;
    private ICarrierAccessCacheService mAccessCacheService = null;
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            if (Utils.DEBUG) Log.i(TAG, "Service Connected to " + name.getShortClassName());
            if (name.getShortClassName().equals(".LoadCarrierService")) {
                mLoadCarrierService = ILoadCarrierService.Stub.asInterface(service);
            } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = ICarrierAccessCacheService.Stub.asInterface(service);
            }
            if (mLoader != null) {
                mLoader.notifyCarrierLoadServiceChanged(mLoadCarrierService, mAccessCacheService);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            if (name.getShortClassName().equals(".LoadCarrierService")) {
                mLoadCarrierService = null;
            } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = null;
            }
            if (mLoader != null) {
                mLoader.notifyCarrierLoadServiceChanged(mLoadCarrierService, mAccessCacheService);
            }
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == MSG_START_SWITCH) {
                if (Utils.DEBUG) Log.d(TAG, "Start to switch to Default.");
                // Show the "Please wait ..." dialog.
                WaitDialog wait = WaitDialog.newInstance();
                wait.show(getFragmentManager(), WaitDialog.TAG_LABEL);

                Utils.resetSimsInfo(getActivity());

                // Before start the switch action, need update the NV Items first. And after
                // the update action finished, will start the switch action.
                updateROWNVItems();
            } else if (msg.what == MSG_SWITCH_ERROR) {
                // There is some error, prompt one toast to alert the user.
                WaitDialog wait = (WaitDialog) getFragmentManager()
                        .findFragmentByTag(WaitDialog.TAG_LABEL);
                wait.dismiss();
                Toast.makeText(getActivity(), R.string.alert_switch_error, Toast.LENGTH_LONG)
                        .show();
            }
        }
    };

    public CarrierResetFragment() {
    }

    public static CarrierResetFragment newInstance() {
        final CarrierResetFragment fragment = new CarrierResetFragment();
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Try to bind the service.
        if (mServiceConnection != null) {
            if (mLoadCarrierService == null) {
                // Bind the service to get the carriers stored in SD card.
                Intent intent = new Intent(ILoadCarrierService.class.getName());
                intent.setPackage(ILoadCarrierService.class.getPackage().getName());
                getActivity().bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
            if (mAccessCacheService == null) {
                // Bind the service to access cache dir.
                Intent intent = new Intent(ICarrierAccessCacheService.class.getName());
                intent.setPackage(ICarrierAccessCacheService.class.getPackage().getName());
                getActivity().bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
        }

        mLoader = new CarriersLoader(getActivity(), mLoadCarrierService, mAccessCacheService);
        mLoader.registerListener(0, mLoaderListener);
        mLoader.startLoading();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        View rootView = inflater.inflate(R.layout.carrier_reset_confirm, container, false);
        Button resetButton = (Button) rootView.findViewById(R.id.execute_carrier_clear);
        resetButton.setOnClickListener(this);
        return rootView;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mLoader != null) {
            mLoader.unregisterListener(mLoaderListener);
            mLoader.stopLoading();
            mLoader.reset();
        }

        if (mServiceConnection != null) {
            getActivity().unbindService(mServiceConnection);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.execute_carrier_clear:
                if (mSwitchToDefaultCarrier != null) {
                    mHandler.sendEmptyMessage(MSG_START_SWITCH);
                } else {
                    if (mIsParsingFinished) {
                        Toast.makeText(getActivity(), R.string.carrier_clear_no_default_pack,
                                Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(getActivity(), R.string.carrier_clear_parsing_not_finish,
                                Toast.LENGTH_LONG).show();
                    }
                }
                break;
            default:
                break;
        }
    }

    private void findSwitchToDefaultCarrier(CarriersStorage storage) {
        if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
            for (Carrier carrier : storage.mListCarrier) {
                if (carrier.mName.equals("Default")) {
                    mSwitchToDefaultCarrier = carrier;
                    break;
                }
            }
        } else {
            String switchToDefaultCarrierName = Carrier.getCurrentCarriersName(
                    Carrier.getCurrentCarriers()) + Utils.CARRIER_TO_DEFAULT_NAME;
            for (Carrier carrier : storage.mListCarrier) {
                if (carrier.mName.equals(switchToDefaultCarrierName)) {
                    mSwitchToDefaultCarrier = carrier;
                    break;
                }
            }
        }
        return;
    }

    private void findMatchedSubscriptionId(Carrier carrier) {
        mConfigId = carrier.mName;
        mSubMask = 0;
        if (matchedByICC(carrier) || matchedByMCCMNCGID(carrier) || matchedByMCCMNC(carrier)
                || matchedBySPN(carrier) || matchedByMCC(carrier)) {
            Log.d(TAG, "Match success.");
        }
        carrier.setSubMask(mSubMask);
    }

    private boolean matchedByICC(Carrier carrier) {
        String carrierICC = carrier.getICC();
        if (TextUtils.isEmpty(carrierICC)) {
            return false;
        }

        boolean matched = false;
        TelephonyManager telephonyManager = (TelephonyManager) getActivity().getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = telephonyManager.getPhoneCount();
        for (int i = 0; i < phoneCount; i++) {
            String icc = telephonyManager.getSimSerialNumber(SubscriptionManager.getSubId(i)[0]);
            if (!TextUtils.isEmpty(icc) && (carrierICC.equals(icc))) {
                Log.d(TAG, "slot " + i + " matched by ICC");
                mSubMask = mSubMask | ( 0x0001 << i);
                matched = true;
            }
        }
        return matched;
    }

    private boolean matchedByMCCMNCGID(Carrier carrier) {
        String carrierGID = carrier.getGID();
        String carrierMCCMNCString = carrier.getMCCMNC();
        if (TextUtils.isEmpty(carrierGID) || TextUtils.isEmpty(carrierMCCMNCString)) {
            return false;
        }

        boolean matched = false;
        String[] carrierMCCMNCs = carrierMCCMNCString.split(SEP_COMMA);
        TelephonyManager telephonyManager = (TelephonyManager) getActivity().getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = telephonyManager.getPhoneCount();
        for (int i = 0; i < phoneCount; i++) {
            String gid = telephonyManager.getGroupIdLevel1(SubscriptionManager.getSubId(i)[0]);
            if (!TextUtils.isEmpty(gid) && (carrierGID.equals(gid))) {
                String mccmnc = telephonyManager.getSimOperator(SubscriptionManager.getSubId(i)[0]);
                if (!TextUtils.isEmpty(mccmnc)) {
                    for (String carrierMCCMNC : carrierMCCMNCs) {
                        if (mccmnc.equals(carrierMCCMNC)) {
                            Log.d(TAG, "slot " + i + " matched by GIDMCCMNC");
                            mSubMask = mSubMask | ( 0x0001 << i);
                            matched = true;
                            break;
                        }
                    }
                }
            }
        }
        return matched;
    }

    private boolean matchedByMCCMNC(Carrier carrier) {
        String carrierMCCMNCString = carrier.getMCCMNC();
        if (TextUtils.isEmpty(carrierMCCMNCString)) {
            return false;
        }

        boolean matched = false;
        String[] carrierMCCMNCs = carrierMCCMNCString.split(SEP_COMMA);
        TelephonyManager telephonyManager = (TelephonyManager) getActivity().getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = telephonyManager.getPhoneCount();
        for (int i = 0; i < phoneCount; i++) {
            String mccmnc = telephonyManager.getSimOperator(SubscriptionManager.getSubId(i)[0]);
            if (!TextUtils.isEmpty(mccmnc)) {
                for (String carrierMCCMNC : carrierMCCMNCs) {
                    if (mccmnc.equals(carrierMCCMNC)) {
                        Log.d(TAG, "slot " + i + " matched by MCCMNC");
                        mSubMask = mSubMask | ( 0x0001 << i);
                        matched = true;
                        break;
                    }
                }
            }
        }
        return matched;
    }

    private boolean matchedBySPN(Carrier carrier) {
        String carrierSPN = carrier.getSPN();
        if (TextUtils.isEmpty(carrierSPN)) {
            return false;
        }

        boolean matched = false;
        TelephonyManager telephonyManager = (TelephonyManager) getActivity().getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = telephonyManager.getPhoneCount();
        for (int i = 0; i < phoneCount; i++) {
            /*String spn = NewUtils.getSimOperatorNameForSubscription(
                    SubscriptionManager.getSubId(i)[0]);*/
            String spn = telephonyManager.getSimOperatorNameForPhone(
                   SubscriptionManager.getPhoneId(SubscriptionManager.getSubId(i)[0]));
            if (!TextUtils.isEmpty(spn) && (carrierSPN.equals(spn))) {
                Log.d(TAG, "slot " + i + " matched by SPN");
                mSubMask = mSubMask | ( 0x0001 << i);
                matched = true;
            }
        }
        return matched;
    }

    private boolean matchedByMCC(Carrier carrier) {
        String carrierMCCMNCString = carrier.getMCCMNC();
        if (TextUtils.isEmpty(carrierMCCMNCString)) {
            return false;
        }

        boolean matched = false;
        TelephonyManager telephonyManager = (TelephonyManager) getActivity().getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = telephonyManager.getPhoneCount();
        String[] carrierMCCMNCs = carrierMCCMNCString.split(SEP_COMMA);
        for (int i = 0; i < phoneCount; i++) {
            String mccmnc = telephonyManager.getSimOperator(SubscriptionManager.getSubId(i)[0]);
            if (!TextUtils.isEmpty(mccmnc)) {
                String mcc = mccmnc.substring(0, 3);
                for (String carrierMCCMNC : carrierMCCMNCs) {
                    if (mcc.equals(carrierMCCMNC)) {
                        Log.d(TAG, "slot " + i + " matched by MCC");
                        mSubMask = mSubMask | ( 0x0001 << i);
                        matched = true;
                        break;
                    }
                }
            }
        }
        return matched;
    }

    @Override
    public void onActionFinished(int requestCode) {
        switch (requestCode) {
            case Actions.REQUEST_UPDATE_ROW_NV_ITEMS:
                startSwitchAction();
                break;
            case Actions.REQUEST_SWITCH_CARRIER:
                getActivity().setResult(Activity.RESULT_OK);
                getActivity().finish();
                break;
        }
    }

    @Override
    public void onActionError(int requestCode, int resultCode, Exception ex) {
        switch (requestCode) {
            case Actions.REQUEST_UPDATE_ROW_NV_ITEMS:
                if (resultCode == Actions.RESULT_NO_MBN) {
                    startSwitchAction();
                }
                break;
            default:
                Log.e(TAG, "Get the request[ " + requestCode + "] error, ex is " + ex.getMessage());
                mHandler.sendEmptyMessage(MSG_SWITCH_ERROR);
                break;
        }
    }

    private void startSwitchAction() {
        try {
            SwitchData data = mSwitchToDefaultCarrier.getSwitchData(mLoadCarrierService,
                    mAccessCacheService);
            SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                    Actions.REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
            switchTask.execute(data);
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

    private void updateROWNVItems() {
        mSwitchToDefaultCarrier.setSubMask(Integer.parseInt(Utils.getSharedPreference(
                getActivity(), Utils.SHARED_PREFERENCE_KEY_CURRENT_SUB_MASK)));
        if (mSwitchToDefaultCarrier.getSubMask() == Utils.INVALID_SUB_MASK) {
            startSwitchAction();
        } else {
            if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
                ArrayList<Carrier> otherCarriers = new ArrayList<Carrier>();
                otherCarriers.add(mSwitchToDefaultCarrier);
                UpdateNVItemsTask updateTask = new UpdateNVItemsTask(getActivity(),
                        Actions.REQUEST_UPDATE_ROW_NV_ITEMS, this, null, otherCarriers, true,
                        mAccessCacheService);
                updateTask.execute();

            } else {
                UpdateNVItemsTask updateTask = new UpdateNVItemsTask(getActivity(),
                        Actions.REQUEST_UPDATE_ROW_NV_ITEMS, this, mSwitchToDefaultCarrier, null,
                        true, mAccessCacheService);
                updateTask.execute();
            }
        }
    }
}

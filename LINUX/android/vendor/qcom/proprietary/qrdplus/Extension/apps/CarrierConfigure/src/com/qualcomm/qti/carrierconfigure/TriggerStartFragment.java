/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
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
import android.os.SystemProperties;
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
import com.android.internal.os.IRegionalizationService;
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
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

public class TriggerStartFragment extends Fragment implements OnClickListener,
        MyAlertDialog.OnAlertDialogButtonClick, MyNoticeDialog.OnNoticeDialogButtonClick,
        ActionCallback {
    private static final String TAG = "TriggerStartFragment";

    private static final String ARG_CARRIER_LIST = "carrier_list";
    private static final int REQUEST_START_TRIGGER = 1;

    private static final String RE_MNC = "(\\d{2,3})?";
    private static final String SEP_COMMA = ",";

    private static final int RETRY_DELAY = 500;
    private static final int RETRY_COUNT = 10;

    private static final int MSG_START_SWITCH = 0;
    private static final int MSG_SWITCH_ERROR = 1;
    private static final int MSG_COPY_AND_UNZIPFILE = 2;

    private static final int UPDATE_ROW_NV_ITEMS_FINISH = 1;

    private static final int SHOW_ASK_BUTTON_TIMES_INIT = 0;
    private static final int SHOW_ASK_BUTTON_TIMES_MAX = SHOW_ASK_BUTTON_TIMES_INIT + 3;

    private int mMagicValue;
    private ArrayList<Carrier> mCarriers;

    private int mSubMask = 0;
    private String mConfigId = null;
    private TextView message = null;
    private Button  btn_no = null;
    private LinearLayout mCarriersContainer;

    private int mRetryTimes = 0;
    private Carrier mSelectedCarrier;
    private Carrier mSwitchToDefaultCarrier = null;
    private CarriersLoader mLoader = null;
    private CarriersStorage mCarriersStorage = null;
    private IRegionalizationService mRegionalizationService = null;
    private OnLoadCompleteListener<CarriersStorage> mLoaderListener =
            new OnLoadCompleteListener<CarriersStorage>() {
        @Override
        public void onLoadComplete(Loader<CarriersStorage> loader, CarriersStorage storage) {
            mCarriersStorage = storage;
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
                if (Utils.DEBUG) Log.d(TAG, "Start switch! For user press yes.");
                // Show the "Please wait ..." dialog.
                WaitDialog wait = WaitDialog.newInstance();
                wait.show(getFragmentManager(), WaitDialog.TAG_LABEL);

                Utils.resetSimsInfo(getActivity());

                // Before start the switch action, need update the NV Items first. And after
                // the update action finished, will start the switch action.
                updateNVItems();
            } else if (msg.what == MSG_SWITCH_ERROR) {
                // There is some error, prompt one toast to alert the user.
                WaitDialog wait = (WaitDialog) getFragmentManager()
                        .findFragmentByTag(WaitDialog.TAG_LABEL);
                wait.dismiss();
                Toast.makeText(getActivity(), R.string.alert_switch_error, Toast.LENGTH_LONG)
                        .show();
            } else if (msg.what == MSG_COPY_AND_UNZIPFILE) {
                startSwitchAction_filebased(msg);
            }
        }
    };

    public TriggerStartFragment() {
    }

    public static TriggerStartFragment newInstance(ArrayList<Carrier> carriers) {
        final TriggerStartFragment fragment = new TriggerStartFragment();

        final Bundle args = new Bundle(1);
        args.putParcelableArrayList(ARG_CARRIER_LIST, carriers);
        fragment.setArguments(args);

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

        mCarriers = getArguments().getParcelableArrayList(ARG_CARRIER_LIST);
        mMagicValue = mCarriers.hashCode();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        View rootView = inflater.inflate(R.layout.trigger_start_fragment, container, false);

        mCarriersContainer = (LinearLayout) rootView.findViewById(R.id.carriers_container_view);
        btn_no =(Button) rootView.findViewById(R.id.btn_no);
        btn_no.setOnClickListener(this);
        message = (TextView) rootView.findViewById(R.id.trigger_message);
        message.setText(R.string.trigger_message);

        if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
            buildCarriersView_fileBased(mCarriersContainer);
        } else {
            buildCarriersView(mCarriersContainer);
        }

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
            case R.id.btn_no:
                 btn_no.setEnabled(false);
                 Toast.makeText(getActivity(), R.string.welcome_btn_no_wait, Toast.LENGTH_LONG)
                        .show();
                 updateROWNVItems();
                 /* Intent intent = new Intent();
                   intent.setAction(Utils.ACTION_TRIGGER_CONFIGUAGE_LATER);
                   intent.setClass(getActivity(), ConfigurationActivity.class);
                   intent.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST,
                        mCarriers);
                   startActivityForResult(intent, REQUEST_START_TRIGGER);*/
                break;
            default:
                if (v.getId() >= mMagicValue
                        && v.getId() < mMagicValue + mCarriers.size()) {
                    mSelectedCarrier = (Carrier) v.getTag();
                    if (Utils.DEBUG) Log.d(TAG, "Click the carrier " + mSelectedCarrier);
                    mConfigId = mSelectedCarrier.mName;
                    mSubMask = mSelectedCarrier.getSubMask();
                    Log.d(TAG, "mSubMask=" + mSubMask + " mConfigId=" + mConfigId);
                    // Show the dialog to alert the user.
                   Intent i = new Intent();
                   i.setAction(Utils.ACTION_TRIGGER_REBOOT);
                   i.setClass(getActivity(), ConfigurationActivity.class);
                   i.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST,
                        mCarriers);
                   startActivityForResult(i, REQUEST_START_TRIGGER);
                }
                break;
        }
    }

    private void clickButtonNo() {
        Carrier currentCarrier = Carrier.getCurrentCarrierInstance();
        if (currentCarrier.mName.equals(Utils.DEFAULT_SPEC_NAME)
                || currentCarrier.mName.equals(Utils.SPECIAL_ROW_PACKAGE_NAME)) {
            saveSimsInfo();
            //Carrier.saveAsNotTriggeredCarrier(mCarriers);
            ((ConfigurationApplication)(getActivity().getApplication())).saveAsNotTriggeredCarrier(mCarriers);
            getActivity().setResult(Activity.RESULT_OK);
            getActivity().finish();
            return;
        }

        if (Utils.DEBUG) Log.d(TAG, "Check whether need to switch back to Default");
        if (Utils.DEBUG) Log.d(TAG, "currentCarrier name: " + currentCarrier.mName
                + " currentTopCarrier: " + currentCarrier.getTopCarrierTitle());
        boolean hasCurrentCarrier = false;
        if (currentCarrier != null) {
            for (Carrier carrier : mCarriers) {
                if (Utils.DEBUG) Log.d(TAG, "carrier name: " + carrier.mName
                        + " currentTopCarrier: " + currentCarrier.getTopCarrierTitle());
                if (carrier.getTopCarrierTitle().equals(currentCarrier.getTopCarrierTitle())) {
                    hasCurrentCarrier = true;
                    break;
                }
            }
        }

        if (hasCurrentCarrier) {
            saveSimsInfo();
            //Carrier.saveAsNotTriggeredCarrier(mCarriers);
            ((ConfigurationApplication)(getActivity().getApplication())).saveAsNotTriggeredCarrier(mCarriers);
            getActivity().setResult(Activity.RESULT_OK);
            getActivity().finish();
        } else {
            if ((!Utils.isFileBasedSwitchEnabled()) && (!Utils.isPresetMode())
                    && (mSwitchToDefaultCarrier != null)) {
                if (Utils.DEBUG) Log.i(TAG, "Try to switch to Default");
                Utils.resetSimsInfo(getActivity());
                SwitchData data = mSwitchToDefaultCarrier.getSwitchData(mLoadCarrierService,
                        mAccessCacheService);
                SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                        Actions.REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
                switchTask.execute(data);
            } else if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
                if (Utils.DEBUG) Log.i(TAG, "Try to switch to Default");
                Utils.resetSimsInfo(getActivity());
                SwitchData data = Carrier.getDefaultCarrierInstance().getSwitchData(
                        mLoadCarrierService, mAccessCacheService);
                SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                        Actions.REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
                switchTask.execute(data);
            } else {
                saveSimsInfo();
                //Carrier.saveAsNotTriggeredCarrier(mCarriers);
                ((ConfigurationApplication)(getActivity().getApplication())).saveAsNotTriggeredCarrier(mCarriers);
                getActivity().setResult(Activity.RESULT_OK);
                getActivity().finish();
            }
        }
    }

    private void saveSimsInfo() {
        int subMask = Utils.INVALID_SUB_MASK;
        for (Carrier carrier : mCarriers) {
            if (carrier.getSubMask() != Utils.INVALID_SUB_MASK) {
                subMask |= carrier.getSubMask();
            }
        }
        Utils.saveCurrentSims(getActivity(), subMask);
    }

    private void findMatchedSubscriptionId(Carrier carrier) {
        mSubMask = 0;
        if (matchedByICC(carrier) || matchedByMCCMNCGID(carrier) || matchedByMCCMNC(carrier)
                || matchedBySPN(carrier) || matchedByMCC(carrier)) {
            Log.d(TAG, "Match success.");
        }

        // means matched by Dependency
        if (mSubMask == 0) {
            TelephonyManager telephonyManager = (TelephonyManager) getActivity().getSystemService(
                    Context.TELEPHONY_SERVICE);
            int phoneCount = telephonyManager.getPhoneCount();
            for (int i = 0; i < phoneCount; i++) {
                String mccMnc = telephonyManager.getSimOperator(SubscriptionManager.getSubId(i)[0]);
                if (!TextUtils.isEmpty(mccMnc)) {
                    mSubMask = mSubMask | ( 0x0001 << i);
                }
            }
        }
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
    public void onAlertDialogButtonClick(int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                // For user press OK, then try to switch the carrier.
                mHandler.sendEmptyMessage(MSG_START_SWITCH);
                break;
        }
    }

    @Override
    public void onNoticeDialogButtonClick(int dialogId) {
        switch (dialogId) {
            case UPDATE_ROW_NV_ITEMS_FINISH:
                getActivity().setResult(Activity.RESULT_OK);
                getActivity().finish();
                break;
            default:
                break;
        }
    }

    @Override
    public void onActionFinished(int requestCode) {
        switch (requestCode) {
            case Actions.REQUEST_UPDATE_NV_ITEMS:
                // After the update NV items action finished, start the switch action.
                startSwitchAction();
                break;
            case Actions.REQUEST_UPDATE_ROW_NV_ITEMS:
                // Save the carriers as not triggered.
                //Carrier.saveAsNotTriggeredCarrier(mCarriers);
                ((ConfigurationApplication)(getActivity().getApplication())).saveAsNotTriggeredCarrier(mCarriers);
                // TODO: Several notice or alert dialog should be added in the future
                // for more friendly UI.
                /*
                // Pop up a notification dialog to let user reboot manually.
                MyNoticeDialog dialog = MyNoticeDialog.newInstance(this,
                        UPDATE_ROW_NV_ITEMS_FINISH, R.string.notice_row_title,
                        R.string.notice_row_text);
                dialog.show(getFragmentManager(), MyNoticeDialog.TAG_LABEL);
                */
                clickButtonNo();
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
            case Actions.REQUEST_UPDATE_NV_ITEMS:
                // If the update action failed, just go on to start switch action for now.
                startSwitchAction();
                break;
            case Actions.REQUEST_UPDATE_ROW_NV_ITEMS:
                if (resultCode == Actions.RESULT_NO_MBN) {
                    //Carrier.saveAsNotTriggeredCarrier(mCarriers);
                    ((ConfigurationApplication)(getActivity().getApplication())).saveAsNotTriggeredCarrier(mCarriers);
                    getActivity().setResult(Activity.RESULT_OK);
                    getActivity().finish();
                }
                break;
            default:
                Log.e(TAG, "Get the request[ " + requestCode + "] error, ex is " + ex.getMessage());
                mHandler.sendEmptyMessage(MSG_SWITCH_ERROR);
                break;
        }
    }

    // "*_fileBased" is used to distinguish the carrier switching through preset package
    // and file-based ota.zip with block-based ota.zip.
    private void buildCarriersView_fileBased(LinearLayout parentView) {
        ArrayList<String> carriers = Carrier.getCurrentCarriers();
        String currentCarrierName = carriers.get(carriers.size() -1);
        final int simNumber = mCarriers.size();
        for (int i = 0; i < mCarriers.size(); i++) {
            Carrier carrier = mCarriers.get(i);

            // Init the button view.
            Button btn = new Button(getActivity());
            String info = getActivity().getResources()
                .getText(R.string.trigger_title_info).toString();
            if(simNumber > 1){
               message.setText(getActivity().getResources()
                .getText(R.string.trigger_title_info_multiSim).toString());
               btn.setText(getCarrierDisplay(carrier)+"");
            }else {
               message.setText(info +"  "+ getCarrierDisplay(carrier) + "?");
               btn.setText(R.string.configure_now);
            }

            if(i > 0){
              Button btn1 = new Button(getActivity());
              btn1.setBackground(null);
              parentView.addView(btn1,
                    new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
            }
            btn.setBackgroundResource(R.color.primary_color);
            btn.setAllCaps(false);
            btn.setTextAppearance(getActivity(), android.R.attr.textAppearanceMedium);
            btn.setTag(carrier);
            btn.setId(mMagicValue + i);
            btn.setEnabled(!carrier.mName.equalsIgnoreCase(currentCarrierName));
            btn.setOnClickListener(new OnClickListener(){
               @Override
               public void onClick(View v) {
                   Intent i = new Intent();
                   i.setAction(Utils.ACTION_TRIGGER_REBOOT);
                   i.putExtra("onclick",((Carrier)((Button)v).getTag()).mName.toString());
                   i.setClass(getActivity(), ConfigurationActivity.class);
                   i.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST,
                        mCarriers);
                   startActivityForResult(i, REQUEST_START_TRIGGER);
               }
            });
            // Add this button to parent view.
            parentView.addView(btn,
                    new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        }
    }

    private void buildCarriersView(LinearLayout parentView) {
        ArrayList<String> carriers = Carrier.getCurrentCarriers();
        String currentCarrierName = Carrier.getCurrentCarriersName(carriers);

        travelCarriers(parentView, travelCarriers(parentView, currentCarrierName));
    }

    private boolean isUninstallMode() {
        Carrier currentCarrier = Carrier.getCurrentCarrierInstance();
        if (currentCarrier != null) {
            String currentCarrierDependency = currentCarrier.getDependency();
            if (!TextUtils.isEmpty(currentCarrierDependency)) {
                for (Carrier carrier : mCarriers) {
                    if (carrier.mName.equals(currentCarrierDependency)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private String travelCarriers(LinearLayout parentView, String fullName) {
        if (fullName == null) return null;
        boolean needScanAgain = !Carrier.getBaseCarrierName(fullName).equals("Default");
        for (int i = 0; i < mCarriers.size(); i++) {
            Carrier carrier = mCarriers.get(i);
            if (!carrier.getBaseCarrierName().equals(Carrier.getSwitchCarrierName(fullName)))
                    continue;
            if (!needScanAgain && mSwitchToDefaultCarrier != null && carrier.mName.equals("Default"))
                    continue;

            // Init the button view.
            Button btn = new Button(getActivity());
            btn.setText(getCarrierDisplay(carrier));
            btn.setTextAppearance(getActivity(), android.R.attr.textAppearanceMedium);
            btn.setTag(carrier);
            btn.setId(mMagicValue + i);
            btn.setOnClickListener(this);
            if (carrier.mName.endsWith("2Default")) {
                if (mCarriers.size() > 1) {
                    btn.setVisibility(View.GONE);
                }
            } else {
                btn.setEnabled(!(carrier.mName.equalsIgnoreCase(Carrier.getCurrentCarriersName(
                        Carrier.getCurrentCarriers()))));
            }

            // Add this button to parent view.
            parentView.addView(btn,
                    new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

            if (!needScanAgain && carrier.getSwitchCarrierName().equals("Default")
                     && !"Default".equals(fullName)) {
                needScanAgain = true;
                mSwitchToDefaultCarrier = carrier;
            }
        }
        if (needScanAgain && mSwitchToDefaultCarrier != null)
            return mSwitchToDefaultCarrier.getSwitchCarrierName();
        else return null;

    }

    private String getCarrierDisplay(Carrier carrier) {
        if (carrier == null) return null;

        if (carrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
            return carrier.getSwitchCarrierName();
        }

        String target = carrier.getTarget();
        String brand =  carrier.getBrand();

        String display = TextUtils.isEmpty(brand) ? carrier.getTopCarrierTitle() : brand;
        if (!TextUtils.isEmpty(carrier.getShowSimSpn())
                && (carrier.getShowSimSpn().equalsIgnoreCase("true"))) {
            if (Utils.DEBUG) Log.d(TAG, "Try to display SPN instead of name/brand: " + display);
            int subMask = carrier.getSubMask();
            if (subMask != Utils.INVALID_SUB_MASK) {
                TelephonyManager telephonyManager = (TelephonyManager) getActivity()
                        .getSystemService(Context.TELEPHONY_SERVICE);
                int phoneCount = telephonyManager.getPhoneCount();
                int slotId = Utils.INVALID_SLOT_ID;
                for (int i=0; i<phoneCount; i++) {
                    if ((subMask & (0x0001 << i)) != 0) {
                        slotId = i;
                        break;
                    }
                }
                if (slotId != Utils.INVALID_SLOT_ID) {
                    /*String spn = NewUtils.getSimOperatorNameForSubscription(
                            SubscriptionManager.getSubId(slotId)[0]);*/
                    String spn = telephonyManager.getSimOperatorNameForPhone(
                            SubscriptionManager.getPhoneId(SubscriptionManager.getSubId(slotId)[0]));
                    if (!TextUtils.isEmpty(spn)) {
                        display = spn;
                        if (Utils.DEBUG) Log.d(TAG, "Display SPN: " + display);
                    }
                }
            }
        }
        if (!TextUtils.isEmpty(target)) {
            display = display + " - " + target;
        }
        return display;
    }

    private void findDependencyCarriers(Carrier carrier,
            CarriersStorage storage,
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
                /*String type = mSelectedCarrier.getDeviceType();
                //SystemProperties.set("persist.radio.device_type", type);
                if (RegionalizationEnvironment.isSupported()) {
                    mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
                }
                if(mRegionalizationService != null){
                    try{
                        mRegionalizationService.writeFile(Utils.DEVICE_TYPE, type + "\n", false);
                        Log.d(TAG,".........write over ........");
                    }catch (Exception e) {
                        Log.e(TAG, "IOException"+ e.getMessage());
                    }
                }*/
                String dependcarrier = mSelectedCarrier.getDependency();
                if (!TextUtils.isEmpty(dependcarrier)) {
                    if (mCarriersStorage != null) {
                        ArrayList<Carrier> carrierlist = new ArrayList<Carrier>();
                        carrierlist.add(mSelectedCarrier);
                        findDependencyCarriers(mSelectedCarrier, mCarriersStorage, carrierlist);
                        new CopyAndUnzipOtaFileThread(getActivity(), mHandler,
                                MSG_COPY_AND_UNZIPFILE, mLoadCarrierService, mAccessCacheService,
                                carrierlist).start();
                    }
                } else {
                    SwitchData data = null;
                    if (Carrier.TYPE_OTA_ZIP_FILE.equals(mSelectedCarrier.mType)) {
                        File srcFile = new File(mSelectedCarrier.getPath());
                        if (mSelectedCarrier.getPath().startsWith("/cache")) {
                            data = new SwitchData(mSelectedCarrier.mName,
                                    mSelectedCarrier.getPath());
                        } else {
                            data = new SwitchData(mSelectedCarrier.mName,
                                    "/sdcard/" + srcFile.getName());
                        }
                    } else {
                        data = mSelectedCarrier.getSwitchData(mLoadCarrierService,
                                mAccessCacheService);
                    }

                    SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                            Actions.REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
                    switchTask.execute(data);
                }
            } else {
                SwitchData data = mSelectedCarrier.getSwitchData(mLoadCarrierService,
                        mAccessCacheService);
                SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                        Actions.REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
                if (mSwitchToDefaultCarrier != null && !mSelectedCarrier.getBaseCarrierName()
                        .equals(Carrier.getCurrentCarriersName(Carrier.getCurrentCarriers()))
                        && !mSelectedCarrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
                    switchTask.execute(mSwitchToDefaultCarrier.getSwitchData(mLoadCarrierService,
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
            SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                    Actions.REQUEST_SWITCH_CARRIER, this,
                    (ArrayList<SwitchData>)msg.obj, mAccessCacheService);
            switchTask.execute();
        } catch (IllegalArgumentException ex) {
            Log.e(TAG, "Catch the IllegalArgumentException: " + ex.getMessage());
        }
    }

    private void updateNVItems() {
        ArrayList<Carrier> otherCarriers = new ArrayList<Carrier>();
        if (mSelectedCarrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
            otherCarriers.add(mSelectedCarrier);
            UpdateNVItemsTask updateTask = new UpdateNVItemsTask(getActivity(),
                    Actions.REQUEST_UPDATE_NV_ITEMS, this, null, otherCarriers,
                    mAccessCacheService);
            updateTask.execute();
            return;
        }
        for (Carrier carrier : mCarriers) {
            if (!carrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)
                    && !carrier.equals(mSelectedCarrier)
                    && carrier.getSubMask() != Utils.INVALID_SUB_MASK) {
                otherCarriers.add(carrier);
                if (Utils.DEBUG) Log.d(TAG, "Add " + carrier.mName + " into otherCarriers");
            }
        }
        addDefaultCarrierIfExist(otherCarriers);
        UpdateNVItemsTask updateTask = new UpdateNVItemsTask(getActivity(),
                Actions.REQUEST_UPDATE_NV_ITEMS, this, mSelectedCarrier, otherCarriers,
                mAccessCacheService);
        updateTask.execute();
    }

    private void updateROWNVItems() {
        Carrier currentCarrier = Carrier.getCurrentCarrierInstance();
        Carrier targetCarrier = null;
        ArrayList<Carrier> otherCarriers = new ArrayList<Carrier>();
        for (Carrier carrier : mCarriers) {
                if (!carrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)
                    && carrier.getSubMask() != Utils.INVALID_SUB_MASK) {
                if (carrier.mName.equals(currentCarrier.mName)) {
                    targetCarrier = carrier;
                    if (Utils.DEBUG) Log.d(TAG, "Set " + carrier.mName + " as targetCarrier");
                } else {
                    otherCarriers.add(carrier);
                    if (Utils.DEBUG) Log.d(TAG, "Add " + carrier.mName + " into otherCarriers");
                }
            }
        }
        addDefaultCarrierIfExist(otherCarriers);
        UpdateNVItemsTask updateTask = new UpdateNVItemsTask(getActivity(),
                Actions.REQUEST_UPDATE_ROW_NV_ITEMS, this, targetCarrier, otherCarriers, true,
                mAccessCacheService);
        updateTask.execute();
    }

    private void addDefaultCarrierIfExist(ArrayList<Carrier> otherCarriers) {
        Carrier defaultCarrier = Carrier.getDefaultCarrierInstance();
        if (defaultCarrier != null) {
            File mbnFile = new File(defaultCarrier.getPath() + File.separator +
                    Utils.COM_MBN_FILE_PATH);
            if (mbnFile.exists()) {
                if (Utils.DEBUG) Log.d(TAG, "Default Carrier has a MBN file.");
                int newSimsSubMask = Utils.getNewSimsSubMask(getActivity());
                if (Utils.DEBUG) Log.d(TAG, "newSimsSubMask=" + newSimsSubMask);
                if (newSimsSubMask != Utils.INVALID_SUB_MASK) {
                    if (Utils.DEBUG) Log.d(TAG, "Add default carrier into otherCarriers");
                    defaultCarrier.setSubMask(newSimsSubMask);
                    otherCarriers.add(defaultCarrier);
                }
            }
        }
    }
}

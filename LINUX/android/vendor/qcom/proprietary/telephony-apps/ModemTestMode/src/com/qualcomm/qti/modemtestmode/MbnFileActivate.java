/**
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 **/

package com.qualcomm.qti.modemtestmode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.BufferUnderflowException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.provider.Settings;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.TelephonyProperties;
import com.qualcomm.qti.modemtestmode.IMbnTestService;
import android.telephony.TelephonyManager;

public class MbnFileActivate extends Activity {
    private final String TAG = "MbnFileActivate";
    private final String ENCODING = "ISO-8859-1";
    private final int EVENT_QCRIL_HOOK_READY = 1;
    private final int EVENT_DISMISS_REBOOT_DIALOG = 2;
    private final int EVENT_PDC_CONFIG_LIST_DONE = 3;
    private final int NEED_REBOOT = 1;

    private Context mContext = null;
    private MbnMetaInfo mCurMbn = null;
    private MbnMetaInfo mCurHwMbn = null;
    private ArrayList<MbnMetaInfo> mMbnMetaInfoList = null;

    private HashMap<String, String> mCarrierNetworkModeMap = null;

    private ListView mMetaListView = null;
    private MetaInfoListAdapter mMetaAdapter = null;
    private int mMetaInfoChoice = 0;
    private IMbnTestService mMbnTestService = null;
    private boolean mIsCommercial = false;
    private boolean mIsBound = false;
    private boolean mIsListPending = false;
    private ProgressDialog mProgressDialog;

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            log("Received: " + action);
            if (MbnTestUtils.PDC_DATA_ACTION.equals(action)) {
                byte[] result = intent.getByteArrayExtra(MbnTestUtils.PDC_ACTIVATED);
                int sub = intent.getIntExtra(MbnTestUtils.SUB_ID, MbnTestUtils.DEFAULT_SUB);
                ByteBuffer payload = ByteBuffer.wrap(result);
                payload.order(ByteOrder.nativeOrder());
                int error = payload.get();
                log("Sub:" + sub + " activated:" + new String(result) + " error code:" + error);
                int what = EVENT_DISMISS_REBOOT_DIALOG;
                int needReboot = 0;
                if (error == MbnTestUtils.LOAD_MBN_SUCCESS) {
                    needReboot = NEED_REBOOT;
                }
                Message msg = mHandler.obtainMessage(what);
                msg.arg1 = needReboot;
                msg.sendToTarget();
            } else if (MbnTestUtils.PDC_CONFIG_LIST_RECEIVED.equals(action)) {
                byte[] result = intent.getByteArrayExtra("list_data");
                Message msg = mHandler.obtainMessage(EVENT_PDC_CONFIG_LIST_DONE);
                msg.obj = result;
                msg.sendToTarget();
            }
        }

    };

    private Handler mHandler = new Handler() {
        @Override
        public void dispatchMessage(Message msg) {
            switch (msg.what) {
            case EVENT_QCRIL_HOOK_READY:
                log("EVENT_QCRIL_HOOK_READY");
                MbnAppGlobals.getInstance().unregisterQcRilHookReady(mHandler);
                if (!getMbnInfo())
                    showConfigPrompt();
                break;
            case EVENT_PDC_CONFIG_LIST_DONE:
                log("EVENT_PDC_CONFIG_LIST_DONE");
                if (!mIsListPending)
                    log("unexpected EVENT_PDC_CONFIG_LIST_DONE!");
                if (msg.obj != null) {
                    byte[] result = (byte[])msg.obj;
                    if (!parseConfigList(result)) {
                        setActivityView();
                        mIsListPending = false;
                    }
                } else {
                    setActivityView();
                    mIsListPending = false;
                }
                break;
            case EVENT_DISMISS_REBOOT_DIALOG:
                log("EVENT_DISMISS_REBOOT_DIALOG");
                //Only set config when activating successfully.
                if (msg.arg1 == NEED_REBOOT) {
                    String carrier = mMbnMetaInfoList.get(mMetaInfoChoice).getCarrier().toLowerCase();
                    new Thread(new PostSettingThread(carrier)).start();
                } else {
                    mProgressDialog.dismiss();
                    MbnTestUtils.showToast(mContext, mContext.getString(R.string.fail_to_activate_mbn));
                }
                break;
            default:
                break;
            }
        }
    };

    private class PostSettingThread implements Runnable {
        private String mCarrier;
        public PostSettingThread(String carrier) {
            mCarrier = carrier;
        }

        public void run() {
            setConfig();
            String mbnType;
            if (mIsCommercial) {
                mbnType = MbnTestUtils.COMMERCIAL_MBN;
                //new ApnHandlerHelper(mContext, mCarrier).restoreDefaultAPN();
            } else {
                mbnType = MbnTestUtils.TEST_MBN;
                //new ApnHandlerHelper(mContext, mCarrier).restoreLabTestAPN();
            }
            try {
                Thread.sleep(15000);
            } catch (Exception e) {
            }
            MbnTestUtils.rebootSystem(mContext);
        }
    }

    private ServiceConnection mMbnServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mMbnTestService = (IMbnTestService) IMbnTestService.Stub.asInterface(service);
            mIsBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mMbnTestService = null;
            mIsBound = false;
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mContext = this;
        setContentView(R.layout.mbn_activate);
        bindService(new Intent(this, MbnTestService.class),
                mMbnServiceConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        //set the mode to SW MBN Mode on launching MBN Test App.
        MbnTestUtils.setSwMbnMode(true);
        MbnAppGlobals.getInstance().registerQcRilHookReady(mHandler, EVENT_QCRIL_HOOK_READY, null);
        // Other Activity is also using the broadcast.
        IntentFilter filter = new IntentFilter();
        filter.addAction(MbnTestUtils.PDC_DATA_ACTION);
        filter.addAction(MbnTestUtils.PDC_CONFIG_LIST_RECEIVED);
        registerReceiver(mBroadcastReceiver, filter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mBroadcastReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mMbnServiceConnection);
    }

    // True, if there are more
    // False, otherwise
    private boolean parseConfigList(byte[] data) {
        boolean ret = false;
        ByteBuffer payload = ByteBuffer.wrap(data);
        payload.order(ByteOrder.nativeOrder());

        byte result = payload.get();
        if (result == -1)
            return false;
        byte moreAvail = payload.get();
        int numConfigs = payload.get();
        if (numConfigs <= 0) {
            Log.e(TAG, "QCRIL_EVT_HOOK_GET_AVAILABLE_CONFIGS failed w/"
                    + "invalid payload, numStrings = 0");
            return false;
        }

        Log.d(TAG, "numConfigs: " + numConfigs + ", is more available: " + moreAvail);
        for (int i = 0; i < numConfigs; i++) {
            String configIdItem;
            byte idLen = payload.get();
            byte configId[] = new byte[idLen];
            payload.get(configId);
            try {
                configIdItem = new String(configId, ENCODING);
            } catch (UnsupportedEncodingException e) {
                Log.d(TAG, "unsupport " + ENCODING);
                break;
            }
            // skip Goden MBN
            if (configIdItem == null || (configIdItem != null && configIdItem.startsWith(
                    MbnTestUtils.GOLDEN_MBN_ID_PREFIX)))
                continue;
            String metaInfo = MbnAppGlobals.getInstance().getMetaInfoForConfig(configIdItem);
            // skip MBN that has no meta info
            if (metaInfo == null)
                continue;
            byte[] qcVersion = MbnAppGlobals.getInstance().getQcVersionOfID(configIdItem);
            byte[] oemVersion = MbnAppGlobals.getInstance().getOemVersionOfID(configIdItem);
            MbnMetaInfo metaItem = new MbnMetaInfo(mContext, configIdItem, metaInfo);
            metaItem.setQcVersion(qcVersion);
            metaItem.setOemVersion(oemVersion);
            mMbnMetaInfoList.add(metaItem);
        }

        return (moreAvail == 1);
    }

    private void setActivityView() {
        if (mMbnMetaInfoList.size() == 0) {
            log("No MBN meta list");
            return;
        }
        mCarrierNetworkModeMap = new HashMap<String, String>();
        String[] carriers = getResources().getStringArray(R.array.carriers);
        String[] networkMode = getResources().getStringArray(R.array.carriers_network_mode);
        for (int i = 0; i < Math.min(carriers.length, networkMode.length); i++) {
            mCarrierNetworkModeMap.put(carriers[i].toLowerCase(), networkMode[i]);
        }
        TextView mbnIdTextView = (TextView) findViewById(R.id.mbn_id);
        if (mCurMbn.getMetaInfo() == null) {
            mbnIdTextView.setText(getString(R.string.mbn_name) + mCurMbn.getMetaInfo());
        } else {
            String mbnVer = getString(R.string.oem_mbn);
            if (mCurMbn.isQcMbn()) {
                mbnVer = getString(R.string.qc_mbn);
            }
            mbnIdTextView.setText(getString(R.string.mbn_name) + mCurMbn.getMetaInfo() +
                    "\n" + getString(R.string.carrier_name) + mCurMbn.getCarrier() +
                    "\n" + getString(R.string.device_type) + mCurMbn.getDeviceType() +
                    "\n" + getString(R.string.source) + mCurMbn.getSourceString() +
                    "/" + mbnVer);
        }
        Button mbnValidate = (Button) findViewById(R.id.mbn_validate);
        mbnValidate.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(mContext, MbnTestValidate.class);
                startActivity(intent);
            }
        });

        if (mCurMbn.getMbnId() != null) {
            mbnValidate.setVisibility(View.VISIBLE);
        }

        mMetaListView = (ListView) findViewById(R.id.meta_info_list);
        mMetaAdapter = new MetaInfoListAdapter(mContext, R.layout.meta_info_list, mMbnMetaInfoList);
        mMetaListView.setAdapter(mMetaAdapter);
        mMetaListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);

        if (mCurMbn.getMetaInfo() != null) {
            for (int i = 0; i < mMbnMetaInfoList.size(); i++) {
                if (mMbnMetaInfoList.get(i).getMbnId().equals(mCurMbn.getMbnId())) {
                    mMetaInfoChoice = i;
                    break;
                }
            }
            //log("choice:---- :" + mMetaInfoChoice);
        }
        mMetaListView.setItemChecked(mMetaInfoChoice, true);
        mMetaListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                mMetaInfoChoice = position;
                //if (((ListView)parent).getTag() != null) {
                //    ((View)((ListView)parent).getTag()).setBackgroundDrawable(null);
                //}
                //((ListView)parent).setTag(view);
                //view.setBackgroundColor(MbnTestUtils.DEFAULT_COLOR);
            }
        });

        Button exitButton = (Button) findViewById(R.id.exit);
        exitButton.setVisibility(View.VISIBLE);
        exitButton.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        Button activateButton = (Button) findViewById(R.id.activate);
        activateButton.setVisibility(View.VISIBLE);
        activateButton.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (isSameWithActivated()) {
                    MbnTestUtils.showToast(mContext, String.format(mContext.getString(
                            R.string.same_mbn_with_activated), mCurMbn.getMetaInfo()));
                } else {
                    showActivatingDialog();
                }
            }
        });
    }

    private void showActivatingDialog() {
        Dialog dialog = new ActivateDialog(mContext);
        dialog.setTitle(R.string.activate_hint);
        dialog.setCanceledOnTouchOutside(false);
        dialog.show();
    }

    private class ActivateDialog extends Dialog implements View.OnClickListener {
        private Button mCancelBtn, mRebootBtn;

        public ActivateDialog(Context context) {
            super(context);
        }

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activate_dialog);

            MbnMetaInfo tmp = mMbnMetaInfoList.get(mMetaInfoChoice);
            TextView mbnIdTV = (TextView) findViewById(R.id.mbn_id);
            mbnIdTV.setText(getString(R.string.mbn_name) + tmp.getMetaInfo());

            TextView sourceTV = (TextView) findViewById(R.id.mbn_source);
            sourceTV.setText(getString(R.string.source) + tmp.getSourceString());

            TextView mobileData = (TextView) findViewById(R.id.mobile_data);
            TextView roamingData = (TextView) findViewById(R.id.data_roaming);
            if (tmp.getMetaInfo().toLowerCase().contains(MbnTestUtils.COMMERCIAL_MBN)) {
                mIsCommercial = true;
                mobileData.setVisibility(View.GONE);
                roamingData.setVisibility(View.GONE);
            }

            mCancelBtn = (Button) findViewById(R.id.cancel);
            mCancelBtn.setOnClickListener(this);

            mRebootBtn = (Button) findViewById(R.id.confirm);
            mRebootBtn.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            switch (v.getId()) {
            case R.id.cancel:
                dismiss();
                break;
            case R.id.confirm:
                showProgressDialog();
                int subMask = 1;
                String mbnId = mCurHwMbn.getMbnId();
                if (!TextUtils.isEmpty(mbnId) && (mbnId.toLowerCase().contains("dsds") ||
                        mCurHwMbn.getMbnId().toLowerCase().contains("dsda"))) {
                    subMask = 1 + 2;
                }
                // only deactivate in activated configs
                if (mCurMbn.getMetaInfo() != null) {
                    MbnAppGlobals.getInstance().deactivateConfigs();
                }
                MbnAppGlobals.getInstance().selectConfig(mMbnMetaInfoList.get(mMetaInfoChoice).
                        getMbnId(), subMask);
                break;
            }
        }
    }

    private void showProgressDialog() {
        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage(getResources().getString(R.string.Activating_hint));
        mProgressDialog.setCancelable(false);
        mProgressDialog.show();
    }

    private boolean isSameWithActivated() {
        if (mMbnMetaInfoList.get(mMetaInfoChoice).getMbnId().equals(mCurMbn.getMbnId())) {
            return true;
        }
        return false;
    }

    private void setConfig() {
        String gpsTarget = "0";
        if (mMbnMetaInfoList.get(mMetaInfoChoice).getDeviceType().toLowerCase().equals("sglte")) {
            gpsTarget = "1";
        }
        int multiModeNumber = 1;
        String mbnId = mCurHwMbn.getMbnId();
        if (!TextUtils.isEmpty(mbnId) && (mbnId.toLowerCase().contains("dsds") ||
                mCurHwMbn.getMbnId().toLowerCase().contains("dsda"))) {
            multiModeNumber = 2;
        }

        MbnTestUtils.setupData(mContext, mIsCommercial);
    }

    private boolean getMbnInfo() {

        String mbn = MbnAppGlobals.getInstance().getMbnConfig(0);
        String meta = MbnAppGlobals.getInstance().getMetaInfoForConfig(mbn);
        String hwMbn = MbnAppGlobals.getInstance().getMbnConfig(0, MbnAppGlobals.MBN_TYPE_HW);
        String hwMeta = MbnAppGlobals.getInstance().getMetaInfoForConfig(hwMbn);

        byte[] qcVer = MbnAppGlobals.getInstance().getQcVersionOfID(mbn);
        byte[] oemVer = MbnAppGlobals.getInstance().getOemVersionOfID(mbn);
        mCurMbn = new MbnMetaInfo(mContext, mbn, meta);
        mCurHwMbn = new MbnMetaInfo(mContext, hwMbn, hwMeta);
        mCurMbn.setQcVersion(qcVer);
        mCurMbn.setOemVersion(oemVer);
        log("Current Sw Mbn:" + mCurMbn);
        log("Current Hw Mbn:" + mCurHwMbn);

        if (hwMbn == null) {
            log("Failed to get Hw MBN Config details");
            return false;
        }

        if ((mMbnMetaInfoList = new ArrayList<MbnMetaInfo>()) == null)
            return false;

        if (!MbnAppGlobals.getInstance().getAllConfigs()) {
            log("Failed to get availableConfigs");
            return false;
        }
        mIsListPending = true;
        return true;
    }

    private void showConfigPrompt() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage(R.string.check_config_hint)
        .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                finish();
            }
        });
        builder.show();
    }

    private void log(String msg) {
        Log.d(TAG, "MbnTest_" + msg);
    }

}

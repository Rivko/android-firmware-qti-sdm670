/*
 *Copyright (c) 2015 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.phonefeature.uplmn;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.provider.Settings;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import android.view.Menu;
import android.view.MenuItem;
import android.util.Log;

import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.IccCard;
import com.android.internal.telephony.uicc.IccCardApplicationStatus;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.IccCardStatus.CardState;
import com.android.internal.telephony.uicc.IccConstants;
import com.android.internal.telephony.uicc.IccFileHandler;
import com.android.internal.telephony.uicc.IccUtils;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccCardApplication;
import com.android.internal.telephony.uicc.UiccController;
import com.qualcomm.qti.phonefeature.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class UserPLMNListPreference extends PreferenceActivity
        implements DialogInterface.OnCancelListener {
    private static final String LOG_TAG = "UserPLMNListPreference";
    private static final boolean DBG = true;

    private static final int BUSY_READING_DIALOG = 99;
    private static final int BUSY_SAVING_DIALOG = 100;
    private static final int UPLMNLIST_ADD = 101;
    private static final int UPLMNLIST_EDIT = 102;
    private static final int MENU_ADD_OPTIION = Menu.FIRST;

   /* 5n bytes:
    1 to 3  nth PLMN (highest priority)
    4 to 5  nth PLMN Access Technology Identifier */
    private static final int UPLMN_SEL_DATA_LEN = 5;
    private static final int GSM_MASK = 1;
    /**
     * GSM compact access technology.
     */
    private static final int GSM_COMPACT_MASK = 2;
    /**
     * UMTS radio access technology.
     */
    private static final int UMTS_MASK = 4;
    /**
     * LTE radio access technology.
     */
    private static final int LTE_MASK = 8;

    private IccFileHandler mIccFileHandler = null;
    private List<UPLMNInfoWithEf> mUPLMNList;
    private PreferenceScreen mUPLMNListContainer;
    private static final String BUTTON_UPLMN_LIST_KEY = "button_uplmn_list_key";
    private Map<Preference, UPLMNInfoWithEf> mPreferenceMap
            = new LinkedHashMap<Preference, UPLMNInfoWithEf>();
    private UPLMNInfoWithEf mOldInfo;
    private int mNumRec = 0;
    private boolean mAirplaneModeOn = false;
    private IntentFilter mIntentFilter;
    private int mPhoneId = 0;
    protected boolean mIsForeground = false;

    private MyHandler mHandler = new MyHandler();

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (Intent.ACTION_AIRPLANE_MODE_CHANGED.equals(action)) {
                mAirplaneModeOn = intent.getBooleanExtra("state", false);
                setScreenEnabled();
            }
        }
    };

    @Override
    protected Dialog onCreateDialog(int id) {
        ProgressDialog dialog = new ProgressDialog(this);
        dialog.setTitle(getText(R.string.uplmn_list_setting_title));
        dialog.setIndeterminate(true);
        switch(id) {
            case BUSY_READING_DIALOG:
                dialog.setCancelable(true);
                dialog.setOnCancelListener(this);
                dialog.setMessage(getText(R.string.reading_settings));
                return dialog;
            case BUSY_SAVING_DIALOG:
                dialog.setCancelable(false);
                dialog.setMessage(getText(R.string.updating_settings));
                return dialog;
        }
        return null;
    }

    @Override
    public void onCancel(DialogInterface dialog) {
        finish();
    }

    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        addPreferencesFromResource(R.xml.uplmn_list);
        mUPLMNListContainer = (PreferenceScreen) findPreference(BUTTON_UPLMN_LIST_KEY);
        mPhoneId = getIntent().getIntExtra(PhoneConstants.PHONE_KEY,
                SubscriptionManager.getPhoneId(SubscriptionManager.getDefaultSubscriptionId()));
        loadIccFileHandler();

        mIntentFilter = new IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        registerReceiver(mReceiver, mIntentFilter);
    }

    private void loadIccFileHandler() {
        UiccCard newCard = null;
        UiccController uiccController = UiccController.getInstance();
        if (uiccController != null) {
            newCard = uiccController.getUiccCard(mPhoneId);
        }

        log("newCard = " + newCard);
        if (newCard != null) {
            // Always get IccApplication 0.
            UiccCardApplication newUiccApplication = newCard
                    .getApplication(UiccController.APP_FAM_3GPP);
            log("newUiccApplication = " + newUiccApplication);
            if (newUiccApplication != null) {
                mIccFileHandler = newUiccApplication.getIccFileHandler();
                log("fh = " + mIccFileHandler);
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    public void onResume() {
        super.onResume();
        mIsForeground = true;
        getUPLMNInfoFromEf();
        showReadingDialog();
        mAirplaneModeOn = Settings.System.getInt(getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) == 1;
    }

    @Override
    public void onPause() {
        super.onPause();
        mIsForeground = false;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        menu.add(0, MENU_ADD_OPTIION, 0, R.string.uplmn_list_setting_add_plmn)
                .setShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        return true;
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        if (menu != null) {
            menu.setGroupEnabled(0, !mAirplaneModeOn);
        }
        return super.onPrepareOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case MENU_ADD_OPTIION:
            if (mIccFileHandler == null) return true;
            Intent intent = new Intent(this, UPLMNEditor.class);
            intent.putExtra(UPLMNEditor.UPLMN_CODE, "");
            intent.putExtra(UPLMNEditor.UPLMN_PRIORITY, 0);
            intent.putExtra(UPLMNEditor.UPLMN_SERVICE, 0);
            intent.putExtra(UPLMNEditor.UPLMN_ADD, true);
            intent.putExtra(UPLMNEditor.UPLMN_SIZE, mUPLMNList.size());
            startActivityForResult(intent, UPLMNLIST_ADD);
            break;
        case android.R.id.home:
            finish();
            return true;
        default:
            break;
        }
        return super.onOptionsItemSelected(item);
    }

    private void showReadingDialog() {
        if (mIsForeground && mIccFileHandler != null) {
            showDialog(BUSY_READING_DIALOG);
        }
    }

    private void showSavingDialog() {
        if (mIsForeground) {
            showDialog(BUSY_SAVING_DIALOG);
        }
    }

    private void dismissDialogSafely(int id) {
        try {
            dismissDialog(id);
        } catch (IllegalArgumentException e) {
            // This is expected in the case where we were in the background
            // at the time we would normally have shown the dialog, so we didn't
            // show it.
        }
    }

    public void onFinished(Preference preference, boolean reading) {
        log("onFinished reading: " + reading);
        if (reading) {
            dismissDialogSafely(BUSY_READING_DIALOG);
        } else {
            dismissDialogSafely(BUSY_SAVING_DIALOG);
        }
        preference.setEnabled(true);
        setScreenEnabled();
    }

    private void getUPLMNInfoFromEf() {
        log("UPLMNInfoFromEf Start read...");
        if (mIccFileHandler != null) {
            readEfFromIcc(mIccFileHandler, IccConstants.EF_PLMNWACT);
        } else {
            Log.w(LOG_TAG, "mIccFileHandler is null");
        }
    }

    private void readEfFromIcc(IccFileHandler mfh, int efid) {
        mfh.loadEFTransparent(efid,
                mHandler.obtainMessage(MyHandler.MESSAGE_GET_EF_DONE));
    }

    private void writeEfToIcc(IccFileHandler mfh, byte[] efdata, int efid) {
        mfh.updateEFTransparent(efid, efdata,
                mHandler.obtainMessage(MyHandler.MESSAGE_SET_EF_DONE));
    }

    private void refreshUPLMNListPreference(ArrayList<UPLMNInfoWithEf> list) {
        if (mUPLMNListContainer.getPreferenceCount() != 0) {
            mUPLMNListContainer.removeAll();
        }

        if (this.mPreferenceMap != null) {
            mPreferenceMap.clear();
        }

        if (mUPLMNList != null) {
            mUPLMNList.clear();
        }
        mUPLMNList = list;
        if (list == null) {
            log("refreshUPLMNListPreference : NULL UPLMN list!");
        } else {
            log("refreshUPLMNListPreference : list.size()" + list.size());
        }

        if (list == null || list.size() == 0) {
            if (DBG) {
                log("refreshUPLMNListPreference : NULL UPLMN list!");
            }
            if (list == null) {
                mUPLMNList = new ArrayList<UPLMNInfoWithEf>();
            }
            return;
        }

        for (UPLMNInfoWithEf network : list) {
            addUPLMNPreference(network);
            if (DBG) {
                log(network.toString());
            }
        }
    }

    class UPLMNInfoWithEf {

        private String mOperatorNumeric;

        private int mNetworkMode;
        private int mPriority; // priority is the index of the plmn in the list.

        public String getOperatorNumeric() {
            return mOperatorNumeric;
        }

        public int getNetworMode() {
            return mNetworkMode;
        }

        public int getPriority() {
            return mPriority;
        }

        public void setOperatorNumeric(String operatorNumeric) {
            this.mOperatorNumeric = operatorNumeric;
        }

        public void setPriority(int index) {
            this.mPriority = index;
        }

        public UPLMNInfoWithEf(String operatorNumeric, int mNetworkMode,
                int mPriority) {
            this.mOperatorNumeric = operatorNumeric;
            this.mNetworkMode = mNetworkMode;
            this.mPriority = mPriority;
        }

        public String toString() {
            return "UPLMNInfoWithEf " + mOperatorNumeric + "/" + mNetworkMode
                    + "/" + mPriority;
        }
    }

    class PriorityCompare implements Comparator<UPLMNInfoWithEf> {

        public int compare(UPLMNInfoWithEf object1, UPLMNInfoWithEf object2) {
            return (object1.getPriority() - object2.getPriority());
        }
    }

    private void addUPLMNPreference(UPLMNInfoWithEf network) {
        Preference pref = new Preference(this);
        String plmnName = network.getOperatorNumeric();
        String extendName = getNetworkModeString(network.getNetworMode(), plmnName);
        pref.setTitle(plmnName + "(" + extendName + ")");
        mUPLMNListContainer.addPreference(pref);
        mPreferenceMap.put(pref, network);
    }

    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
            Preference preference) {
        Intent intent = new Intent(this, UPLMNEditor.class);
        UPLMNInfoWithEf info = this.mPreferenceMap.get(preference);
        mOldInfo = info;

        intent.putExtra(UPLMNEditor.UPLMN_CODE, info.getOperatorNumeric());
        intent.putExtra(UPLMNEditor.UPLMN_PRIORITY, info.getPriority());
        intent.putExtra(UPLMNEditor.UPLMN_SERVICE, info.getNetworMode());
        intent.putExtra(UPLMNEditor.UPLMN_ADD, false);
        intent.putExtra(UPLMNEditor.UPLMN_SIZE, mUPLMNList.size());
        startActivityForResult(intent, UPLMNLIST_EDIT);
        return true;
    }

    protected void onActivityResult(final int requestCode,
            final int resultCode, final Intent intent) {
        log("resultCode = " + resultCode + ", requestCode = " + requestCode);

        if (intent != null) {
            UPLMNInfoWithEf newInfo = createNetworkInfofromIntent(intent);
            if (resultCode == UPLMNEditor.RESULT_CODE_DELETE) {
                handleSetUPLMN(handleDeleteList(mOldInfo));
            } else if (resultCode == UPLMNEditor.RESULT_CODE_EDIT) {
                if (requestCode == UPLMNLIST_ADD) {
                    handleAddList(newInfo);
                } else if (requestCode == UPLMNLIST_EDIT) {
                    handleSetUPLMN(handleModifiedList(newInfo, mOldInfo));
                }
            }
        }
    }

    private UPLMNInfoWithEf createNetworkInfofromIntent(Intent intent) {
        String numberName = intent.getStringExtra(UPLMNEditor.UPLMN_CODE);
        int priority = intent.getIntExtra(UPLMNEditor.UPLMN_PRIORITY, 0);
        int act = intent.getIntExtra(UPLMNEditor.UPLMN_SERVICE, 0);
        return new UPLMNInfoWithEf(numberName, act, priority);
    }

    private void handleSetUPLMN(ArrayList<UPLMNInfoWithEf> list) {
        showSavingDialog();
        byte[] data = new byte[mNumRec * UPLMN_SEL_DATA_LEN];
        byte[] mccmnc = new byte[6];
        for (int i = 0; i < mNumRec; i++) {
            data[i * UPLMN_SEL_DATA_LEN] = (byte) 0xFF;
            data[i * UPLMN_SEL_DATA_LEN + 1] = (byte) 0xFF;
            data[i * UPLMN_SEL_DATA_LEN + 2] = (byte) 0xFF;

            data[i * UPLMN_SEL_DATA_LEN + 3] = 0;
            data[i * UPLMN_SEL_DATA_LEN + 4] = 0;
        }
        for (int i = 0; ((i < list.size()) && (i < mNumRec)); i++) {
            UPLMNInfoWithEf ni = list.get(i);
            String strOperNumeric = ni.getOperatorNumeric();
            if (strOperNumeric == null) {
                break;
            }
            log("strOperNumeric = " + strOperNumeric);
            if (strOperNumeric.length() == 5) {
                strOperNumeric = strOperNumeric + "F";
            }
            mccmnc = hexStringToBytes(strOperNumeric);

            data[i * UPLMN_SEL_DATA_LEN] = (byte) ((mccmnc[1] << 4) + mccmnc[0]);
            log("mccmnc[0] = " + mccmnc[0]);
            log("mccmnc[1] = " + mccmnc[1]);
            log("data[i*UPLMN_SEL_DATA_LEN] = "
                    + data[i * UPLMN_SEL_DATA_LEN]);
            data[i * UPLMN_SEL_DATA_LEN + 1] = (byte) ((mccmnc[5] << 4) + mccmnc[2]);
            log("data[1] = " + data[1]);
            data[i * UPLMN_SEL_DATA_LEN + 2] = (byte) ((mccmnc[4] << 4) + mccmnc[3]);
            log("data[2] = " + data[2]);
            if ((ni.getNetworMode() & UMTS_MASK) != 0) {
                data[i * UPLMN_SEL_DATA_LEN + 3] = (byte) 0x80;
            } else {
                data[i * UPLMN_SEL_DATA_LEN + 3] = 0;
            }
            if ((ni.getNetworMode() & LTE_MASK) != 0) {
                data[i * UPLMN_SEL_DATA_LEN + 3] = (byte) (data[i
                        * UPLMN_SEL_DATA_LEN + 3] | 0x40);
            }
            if ((ni.getNetworMode() & GSM_MASK) != 0) {
                data[i * UPLMN_SEL_DATA_LEN + 4] = (byte) 0x80;
            } else {
                data[i * UPLMN_SEL_DATA_LEN + 4] = 0;
            }

            if ((ni.getNetworMode() & GSM_COMPACT_MASK) != 0) {
                data[i * UPLMN_SEL_DATA_LEN + 4] = (byte) (data[i
                        * UPLMN_SEL_DATA_LEN + 4] | 0x40);
            }
        }
        log("update EFuplmn Start.");
        if (mIccFileHandler != null) {
            writeEfToIcc(mIccFileHandler, data, IccConstants.EF_PLMNWACT);
        }

    }

    private void handleAddList(UPLMNInfoWithEf newInfo) {
        log("handleAddList: add new network: " + newInfo);
        dumpUPLMNInfo(mUPLMNList);
        ArrayList<UPLMNInfoWithEf> list = new ArrayList<UPLMNInfoWithEf>();
        for (int i = 0; i < mUPLMNList.size(); i++) {
            list.add(mUPLMNList.get(i));
        }
        PriorityCompare pc = new PriorityCompare();
        int position = Collections.binarySearch(mUPLMNList, newInfo, pc);
        if (position >= 0)
            list.add(position, newInfo);
        else
            list.add(newInfo);
        updateListPriority(list);
        dumpUPLMNInfo(list);
        handleSetUPLMN(list);
    }

    private void dumpUPLMNInfo(List<UPLMNInfoWithEf> list) {
        if (!DBG) {
            return;
        }
        for (int i = 0; i < list.size(); i++) {
            log("dumpUPLMNInfo : " + list.get(i).toString());
        }
    }

    private ArrayList<UPLMNInfoWithEf> handleModifiedList(
            UPLMNInfoWithEf newInfo, UPLMNInfoWithEf oldInfo) {
        log("handleModifiedList: change old info: " + oldInfo.toString()
            + " new info: " + newInfo.toString());
        dumpUPLMNInfo(mUPLMNList);

        PriorityCompare pc = new PriorityCompare();
        int oldposition = Collections.binarySearch(mUPLMNList, oldInfo, pc);
        int newposition = Collections.binarySearch(mUPLMNList, newInfo, pc);

        ArrayList<UPLMNInfoWithEf> list = new ArrayList<UPLMNInfoWithEf>();
        for (int i = 0; i < mUPLMNList.size(); i++) {
            list.add(mUPLMNList.get(i));
        }

        if (oldposition > newposition) {
            list.remove(oldposition);
            list.add(newposition, newInfo);
        } else if (oldposition < newposition) {
            list.add(newposition + 1, newInfo);
            list.remove(oldposition);
        } else {
            list.remove(oldposition);
            list.add(oldposition, newInfo);
        }

        updateListPriority(list);
        dumpUPLMNInfo(list);
        return list;
    }

    private void updateListPriority(ArrayList<UPLMNInfoWithEf> list) {
        int priority = 0;
        for (UPLMNInfoWithEf info : list) {
            info.setPriority(priority++);
        }
    }

    private ArrayList<UPLMNInfoWithEf> handleDeleteList(UPLMNInfoWithEf network) {
        log("handleDeleteList : " + network.toString());
        dumpUPLMNInfo(mUPLMNList);

        ArrayList<UPLMNInfoWithEf> list = new ArrayList<UPLMNInfoWithEf>();
        PriorityCompare pc = new PriorityCompare();
        int position = Collections.binarySearch(mUPLMNList, network, pc);

        for (int i = 0; i < mUPLMNList.size(); i++) {
            list.add(mUPLMNList.get(i));
        }

        list.remove(position);
        network.setOperatorNumeric(null);
        list.add(network);

        updateListPriority(list);
        dumpUPLMNInfo(list);

        return list;
    }

    private class MyHandler extends Handler {
        private static final int MESSAGE_GET_UPLMN_LIST = 0;
        private static final int MESSAGE_GET_EF_DONE = 1;
        private static final int MESSAGE_SET_EF_DONE = 2;

        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_GET_UPLMN_LIST:
                handleGetUPLMNList(msg);
                break;

            case MESSAGE_SET_EF_DONE:
                handleSetEFDone(msg);
                break;

            case MESSAGE_GET_EF_DONE:
                handleGetEFDone(msg);
                break;
            default:
                break;
            }
        }

        public void handleGetUPLMNList(Message msg) {
            if (DBG) {
                log("handleGetUPLMNList: done");
            }

            if (msg.arg2 == MyHandler.MESSAGE_GET_UPLMN_LIST) {
                onFinished(mUPLMNListContainer, true);
            } else {
                onFinished(mUPLMNListContainer, false);
            }

            AsyncResult ar = (AsyncResult) msg.obj;
            boolean isUserException = false;
            if (ar.exception != null) {
                log("handleGetUPLMNList with exception = "
                        + ar.exception);
                if (mUPLMNList == null) {
                    mUPLMNList = new ArrayList<UPLMNInfoWithEf>();
                }
            } else {
                refreshUPLMNListPreference((ArrayList<UPLMNInfoWithEf>) ar.result);
            }
        }

        public void handleSetEFDone(Message msg) {
            if (DBG) {
                log("handleSetEFDone: done");
            }
            AsyncResult ar = (AsyncResult) msg.obj;
            boolean isUserException = false;
            if (ar.exception != null) {
                log("handleSetEFDone with exception = "
                        + ar.exception);
            } else {
                if (DBG) {
                    log("handleSetEFDone: with OK result!");
                }
            }
            getUPLMNInfoFromEf();
        }

        public void handleGetEFDone(Message msg) {
            if (DBG) {
                log("handleGetEFDone: done");
            }

            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar.exception != null) {
                log("handleGetEFDone with exception = "
                        + ar.exception);
                Message message = mHandler.obtainMessage();
                message.what = MESSAGE_GET_UPLMN_LIST;
                message.obj = msg.obj;
                // trigger tone stop after timeout duration
                mHandler.sendMessage(message);
            } else {
                byte[] data = (byte[]) ar.result;
                log("result=" + IccUtils.bytesToHexString(data));
                mNumRec = data.length / UPLMN_SEL_DATA_LEN;
                log("mNumRec=" + mNumRec);
                AsyncResult mret;
                ArrayList<UPLMNInfoWithEf> ret;
                ret = new ArrayList<UPLMNInfoWithEf>(mNumRec);
                byte[] mcc = new byte[3];
                byte[] mnc = new byte[3];
                int num_mnc_digits = 0;
                int access_tech = 0;
                String strOperName = null;
                // Covert the plmn data to the EF_OPLMNwACT,
                // mcc+mnc+access technology is one record for the EF
                for (int i = 0; i < mNumRec; i++) {
                    access_tech = 0;
                    mcc[0] = (byte) (data[i * UPLMN_SEL_DATA_LEN] & 0x0F);
                    mcc[1] = (byte) ((data[i * UPLMN_SEL_DATA_LEN] & 0xF0) >> 4);
                    mcc[2] = (byte) (data[i * UPLMN_SEL_DATA_LEN + 1] & 0x0F);

                    mnc[0] = (byte) (data[i * UPLMN_SEL_DATA_LEN + 2] & 0x0F);
                    mnc[1] = (byte) ((data[i * UPLMN_SEL_DATA_LEN + 2] & 0xF0) >> 4);
                    if ((byte) (data[i * UPLMN_SEL_DATA_LEN + 1] & 0xF0) == (byte) 0xF0) {
                        num_mnc_digits = 2;
                        mnc[2] = (byte) ((data[i * UPLMN_SEL_DATA_LEN + 1] & 0xF0) >> 4);
                    } else {
                        num_mnc_digits = 3;
                        mnc[2] = (byte) ((data[i * UPLMN_SEL_DATA_LEN + 1] & 0xF0) >> 4);
                    }

                    if ((data[i * UPLMN_SEL_DATA_LEN + 3] & 0x40) != 0) {
                        access_tech = access_tech | LTE_MASK;
                    }
                    if ((data[i * UPLMN_SEL_DATA_LEN + 3] & 0x80) != 0) {
                        access_tech = access_tech | UMTS_MASK;
                    }
                    if ((data[i * UPLMN_SEL_DATA_LEN + 4] & 0x80) != 0) {
                        access_tech = access_tech | GSM_MASK;
                    }

                    if ((data[i * UPLMN_SEL_DATA_LEN + 4] & 0x40) != 0) {
                        access_tech = access_tech | GSM_COMPACT_MASK;
                    }

                    if ((data[i * UPLMN_SEL_DATA_LEN] != (byte) 0xFF)
                            && (data[i * UPLMN_SEL_DATA_LEN + 1] != (byte) 0xFF)
                            && (data[i * UPLMN_SEL_DATA_LEN + 2] != (byte) 0xFF)) {
                        if (num_mnc_digits == 2) {
                            strOperName = bytesToHexString(mcc)
                                    + bytesToHexString(mnc).substring(0, 2);
                        } else if (num_mnc_digits == 3) {
                            strOperName = bytesToHexString(mcc)
                                    + bytesToHexString(mnc);
                        }
                        ret.add(new UPLMNInfoWithEf(strOperName, access_tech, i));

                    }
                }
                Message message = mHandler.obtainMessage();
                message.what = MESSAGE_GET_UPLMN_LIST;
                if (ret == null) {
                    log("handleGetEFDone : NULL ret list!");
                } else {
                    log("handleGetEFDone : ret.size()" + ret.size());
                }
                mret = new AsyncResult(message.obj, (Object) ret, null);

                message.obj = mret;
                mHandler.sendMessage(message);
            }
        }
    }

    private String getNetworkModeString(int EFNWMode, String plmn) {
        Log.d(LOG_TAG, "plmn = " + plmn);
        int index = UPLMNEditor.convertEFMode2Ap(EFNWMode);
        String[] CuPlmnArray = getResources().getStringArray(R.array.uplmn_cu_mcc_mnc_values);
        for (String CuPlmn : CuPlmnArray) {
            if (plmn.equals(CuPlmn)) {
                return getResources().getStringArray
                        (R.array.uplmn_prefer_network_mode_w_choices)[index];
            }
        }
        return getResources().getStringArray(R.array.uplmn_prefer_network_mode_td_choices)[index];
    }

    private void setScreenEnabled() {
        getPreferenceScreen().setEnabled(!mAirplaneModeOn);
        invalidateOptionsMenu();
    }

    public String bytesToHexString(byte[] bytes) {
        if (bytes == null)
            return null;

        StringBuilder ret = new StringBuilder(bytes.length);

        for (int i = 0; i < bytes.length; i++) {
            int b;
            b = 0x0f & bytes[i];
            ret.append("0123456789abcdef".charAt(b));
        }

        return ret.toString();
    }

    public static byte[] hexStringToBytes(String s) {
        byte[] ret;

        if (s == null)
            return null;

        int sz = s.length();

        ret = new byte[sz];

        for (int i = 0; i < sz; i++) {
            ret[i] = (byte) (hexCharToInt(s.charAt(i)));
            log("hexStringToBytes = " + ret[i]);
        }

        return ret;
    }

    static int hexCharToInt(char c) {
        if (c >= '0' && c <= '9')
            return (c - '0');
        if (c >= 'A' && c <= 'F')
            return (c - 'A' + 10);
        if (c >= 'a' && c <= 'f')
            return (c - 'a' + 10);

        throw new RuntimeException("invalid hex char '" + c + "'");
    }

    private static void log(String msg) {
        Log.d(LOG_TAG, msg);
    }
}

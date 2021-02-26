/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Telephone;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Iterator;

public class TelephoneActivity extends BaseActivity {

    private EditText mNumberEdit;
    private Button mCallBtn;

    private static final String DEFAULT_PHONE_NUMBER = "10086";
    private static final String ACTION_PHONE_STATE = "android.intent.action.PHONE_STATE";
    private int mSlot;
    private boolean isSimEnable = true;
    private boolean mCalled = false;
    private int mResult = TestCase.STATE_FAIL;

    private BroadcastReceiver mPhoneStatReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            LogUtils.logi( "onReceive: action:: " + action);

            if ( Intent.ACTION_NEW_OUTGOING_CALL.equals(action)){
                LogUtils.logd("Make A Call!");
                mCalled = true;
            } else if ( ACTION_PHONE_STATE.equals(action)){
                String state = intent.getExtras().getString(TelephonyManager.EXTRA_STATE);
                LogUtils.logi( "state " + state);
                if ( state.equals(TelephonyManager.EXTRA_STATE_OFFHOOK)){
                    handleResult();
                }
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mSlot = Integer.valueOf(mTestCase.getParameter().get("slot"));
        LogUtils.logd("slot: " + mSlot);
        initView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        initData();
        if ( !mCalled ) {
            registerListener();
        } else {
            updateResult(mResult);
        }
    }

    @Override
    protected void onDestroy() {
        unRegisterListener();
        super.onDestroy();
    }

    private void initView(){
        mNumberEdit = (EditText)findViewById(R.id.number_edit);
        mCallBtn = (Button)findViewById(R.id.call);
    }

    private void initData(){
        checkSimCardEnable();
        if (mNumberEdit.getText() == null || mNumberEdit.getText().toString().isEmpty()){
            mNumberEdit.setText(DEFAULT_PHONE_NUMBER);
            mNumberEdit.setSelection(DEFAULT_PHONE_NUMBER.length());
        }

        mNumberEdit.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                if (isSimEnable){
                    String number = mNumberEdit.getText().toString();
                    if ( number == null || number.equals("")){
                        mCallBtn.setEnabled(false);
                    } else {
                        mCallBtn.setEnabled(true);
                    }
                }
            }
        });

        mCallBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                makeCall();
            }
        });

    }

    private PhoneAccountHandle getPhoneAccount(){
        PhoneAccountHandle result = null;
        TelecomManager telcomMgr = (TelecomManager)this.getSystemService(Service.TELECOM_SERVICE);
        TelephonyManager telMgr = (TelephonyManager)this.getSystemService(Service.TELEPHONY_SERVICE);

        Iterator<PhoneAccountHandle> phoneAccounts = telcomMgr.
                getAllPhoneAccountHandles().listIterator();

        while ( phoneAccounts.hasNext()){
            PhoneAccountHandle handle = phoneAccounts.next();
            PhoneAccount account = telcomMgr.getPhoneAccount(handle);
            LogUtils.logd("account enable:" + account.isEnabled());
            int subId = telMgr.getSubIdForPhoneAccount(account);
            LogUtils.logd("subId:" + subId);
            //convert to slotId
            int slotId = getSlotId(subId);
            LogUtils.logd("slotId:" + slotId);
            //compare with the specific slot id.
            if (mSlot == slotId) {
                result =  handle;
            }
        }
        return result;
    }

    private int getSlotId(int subId){
        int slotId = -100;
        boolean getResult = false;

        Class clazz = SubscriptionManager.class;

        try {
            Method slotIndex = clazz.getMethod("getSlotIndex", int.class);
            slotId = (Integer)slotIndex.invoke(clazz, subId);
            LogUtils.logd("call getSlotIndex success, slotId = " + slotId);
            getResult = true;
        } catch (Exception e) {
            LogUtils.logd("call getSlotIndex failed!!");
            getResult = false;
            e.printStackTrace();
        }

        if (getResult) return slotId;

        try {
            Method slotIndex = clazz.getMethod("getSlotId", int.class);
            slotId = (Integer)slotIndex.invoke(clazz, subId);
            LogUtils.logd("call getSlotId success, slotId = " + slotId);
            getResult = true;
        } catch (Exception e) {
            LogUtils.logd("call getSlotId failed!!");
            getResult = false;
            e.printStackTrace();
        }

        return slotId;
    }

    private void checkSimCardEnable(){
        if ( getPhoneAccount() == null){
            isSimEnable = false;
            mCallBtn.setEnabled(false);
            mCallBtn.setText(R.string.TELEPHONE_sim_disable);
            updateResult(TestCase.STATE_FAIL);
        } else {
            isSimEnable = true;
            mCallBtn.setEnabled(true);
            mCallBtn.setText(R.string.TELEPHONE_call);
        }
    }

    private void makeCall(){
        String number = mNumberEdit.getText().toString();

        Intent intent = new Intent(Intent.ACTION_CALL);
        Uri data = Uri.parse("tel:" + number);
        intent.setData(data);
        PhoneAccountHandle handle = getPhoneAccount();
        if ( handle != null) intent.putExtra(TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE, handle);
        startActivity(intent);
    }

    private void handleResult(){
        TelephonyManager telMgr = (TelephonyManager)this.getSystemService(Service.TELEPHONY_SERVICE);
        boolean isRadioOn = telMgr.isRadioOn();
        LogUtils.logd("isRadioOn " + isRadioOn);
        mResult = isRadioOn ? TestCase.STATE_PASS: TestCase.STATE_FAIL;
        try {
            Thread.sleep(1500);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        telMgr.endCall();
    }

    @Override
    protected int getLayoutId() {
        return R.layout.telephone_act;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    }

    private void registerListener() {
        IntentFilter filter = new IntentFilter(Intent.ACTION_NEW_OUTGOING_CALL);
        filter.addAction(ACTION_PHONE_STATE);
        registerReceiver(mPhoneStatReceiver, filter);
    }

    private void unRegisterListener() {
        if (null != mPhoneStatReceiver) {
            unregisterReceiver(mPhoneStatReceiver);
        }
    }
}

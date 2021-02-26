/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.service.device;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.telephony.SmsManager;
import android.widget.Toast;

import com.qualcomm.qti.smartassistant.R;
import com.qualcomm.qti.smartassistant.utils.LogUtils;

public class SmsSender {

    private static final String TAG = "SmsSender";
    private String SMS_SEND_ACTION = "SMS_SEND";
    private String SMS_DELIVERED_ACTION = "SMS_DELIVERED";

    private SmsStatusReceiver mSmsStatusReceiver;
    private SmsDeliveryStatusReceiver mSmsDeliveryStatusReceiver;
    private Context mContext;

    public SmsSender(Context context) {
        mContext = context;

        mSmsStatusReceiver = new SmsStatusReceiver();
        context.registerReceiver(mSmsStatusReceiver, new IntentFilter(SMS_SEND_ACTION));

        mSmsDeliveryStatusReceiver = new SmsDeliveryStatusReceiver();
        context.registerReceiver(mSmsDeliveryStatusReceiver, new IntentFilter(SMS_DELIVERED_ACTION));
    }

    public void sendMessage(String phoneNumber, String message) {
        SmsManager smsManager = SmsManager.getDefault();
        PendingIntent sentIntent = PendingIntent.getBroadcast(mContext, 0,
                new Intent(SMS_SEND_ACTION), 0);
        PendingIntent deliveryIntent = PendingIntent.getBroadcast(mContext, 0,
                new Intent(SMS_DELIVERED_ACTION), 0);
        smsManager.sendTextMessage(phoneNumber, null,
                message, sentIntent, deliveryIntent);
        LogUtils.d(TAG, "sent message = " + message);
    }

    private void unRegisterReceiver(){
        try {
            mContext.unregisterReceiver(mSmsStatusReceiver);
            mContext.unregisterReceiver(mSmsDeliveryStatusReceiver);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    private void showSendResultMessage(String message){
        Toast.makeText(mContext,message,Toast.LENGTH_LONG).show();
    }

    private class SmsStatusReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            LogUtils.d(TAG, "SmsStatusReceiver onReceive.");
            switch (getResultCode()) {
                case Activity.RESULT_OK:
                    showSendResultMessage(mContext.getResources().getString(R.string.sms_send_success));
                    LogUtils.d(TAG, "Activity.RESULT_OK");
                    break;
                case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
                    LogUtils.d(TAG, "RESULT_ERROR_GENERIC_FAILURE");
                    showSendResultMessage(mContext.getResources().getString(R.string.sms_send_failure));
                    break;
                case SmsManager.RESULT_ERROR_NO_SERVICE:
                    LogUtils.d(TAG, "RESULT_ERROR_NO_SERVICE");
                    showSendResultMessage(mContext.getResources().getString(R.string.sms_send_failure));
                    break;
                case SmsManager.RESULT_ERROR_NULL_PDU:
                    LogUtils.d(TAG, "RESULT_ERROR_NULL_PDU");
                    showSendResultMessage(mContext.getResources().getString(R.string.sms_send_failure));
                    break;
                case SmsManager.RESULT_ERROR_RADIO_OFF:
                    LogUtils.d(TAG, "RESULT_ERROR_RADIO_OFF");
                    showSendResultMessage(mContext.getResources().getString(R.string.sms_send_failure));
                    break;
            }
            unRegisterReceiver();
        }
    }

    private class SmsDeliveryStatusReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            LogUtils.d(TAG, "SmsDeliveryStatusReceiver onReceive.");
            switch (getResultCode()) {
                case Activity.RESULT_OK:
                    LogUtils.d(TAG, "RESULT_OK");
                    break;
                case Activity.RESULT_CANCELED:
                    LogUtils.d(TAG, "RESULT_CANCELED");
                    break;
            }
            unRegisterReceiver();
        }
    }
}

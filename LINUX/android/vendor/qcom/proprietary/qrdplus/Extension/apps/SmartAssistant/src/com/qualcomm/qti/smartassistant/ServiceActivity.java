/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant;

import android.Manifest;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.smartassistant.asr.CandidateInfo;
import com.qualcomm.qti.smartassistant.service.Messages;
import com.qualcomm.qti.smartassistant.service.SmartAssistantService;
import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.utils.PermissionUtils;

import java.util.List;
import java.util.Map;

public class ServiceActivity extends Activity {

    private static final String TAG = "ServiceActivity";
    private static final int REQUEST_CODE = 100;
    private Messenger mSendToServiceMessager = null;
    private boolean mServiceBind;
    private final static String[] REQUIRE_PERMISSIONS = {
            Manifest.permission.CALL_PHONE,
            Manifest.permission.SEND_SMS,
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.ACCESS_WIFI_STATE,
            Manifest.permission.ACCESS_NETWORK_STATE,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.MOUNT_UNMOUNT_FILESYSTEMS,
            Manifest.permission.INTERNET,
            Manifest.permission.READ_PHONE_STATE,
            Manifest.permission.READ_CONTACTS,
            Manifest.permission.MODIFY_AUDIO_SETTINGS,
            Manifest.permission.WRITE_SETTINGS,
            Manifest.permission.CHANGE_NETWORK_STATE,
            Manifest.permission.BLUETOOTH,
            Manifest.permission.BLUETOOTH_ADMIN,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.SET_ALARM,
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch(msg.what) {
                case Messages.START_VOICE_INPUT_RESPONSE:
                    handleStartVoiceInput();
                    break;
                case Messages.STOP_VOICE_INPUT_RESPONSE:
                    handleStopVoiceInput();
                    break;
                case Messages.LOAD_HTML_URL_RESPONSE:
                    handleLoadHtml((String) msg.obj);
                    break;
                case Messages.RENDER_VOICE_INPUT_TEXT_RESPONSE:
                    handleRenderVoiceInputText((String) msg.obj);
                    break;
                case Messages.CALL_BY_CONTACTS_RESPONSE:
                    handleCallByContacts((List<CandidateInfo>)msg.obj);
                    break;
                case Messages.SEND_SMS_BY_CONTACTS_RESPONSE:
                    Map<String, Object> smsParams = (Map<String, Object>)msg.obj;
                    List<CandidateInfo> candidateInfos =(List<CandidateInfo>)smsParams.get("candidate");
                    String message = (String)smsParams.get("message");
                    handleSendSmsByContacts(candidateInfos,message);
                    break;
                case Messages.START_RECOGNITION_RESPONSE:
                    handleStartRecognition();
                    break;
                case Messages.STOP_RECOGNITION_RESPONSE:
                    handleStopRecognition();
                    break;
                default:
                    break;
            }
        }
    };

    private Messenger mMessenger = new Messenger(mHandler);

    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mSendToServiceMessager = new Messenger(iBinder);
            mServiceBind = true;
            sendRequest(Messages.REGISTER_CLIENT_REQUEST, ServiceActivity.this);
            executeAfterBindService();

        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            executeAfterUnBindService();
            sendRequest(Messages.UNREGISTER_CLIENT_REQUEST, ServiceActivity.this);
            mSendToServiceMessager = null;
            mServiceBind = false;
        }
    };

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkPermissions();
    }

    private void checkPermissions(){
        String[] neededPermissions = PermissionUtils.checkRequestedPermission(
                this, REQUIRE_PERMISSIONS);
        if (neededPermissions.length > 0) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                ActivityCompat.requestPermissions(this, neededPermissions, REQUEST_CODE);
                Log.e(TAG, "checkPermissions RequestedPermission size = " +
                        neededPermissions.length);
            }
        }else {
            startService();
            bindService();
        }
    }

    private void bindService(){
        Intent intent = new Intent(this, SmartAssistantService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    private void startService(){
        Intent serviceIntent = new Intent(this, SmartAssistantService.class);
        startService(serviceIntent);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                           int[] grantResults) {
        if (requestCode == REQUEST_CODE){
            boolean isAllGranted = true;

            for (int grant : grantResults) {
                if (grant != PackageManager.PERMISSION_GRANTED) {
                    isAllGranted = false;
                    break;
                }
            }

            if (isAllGranted){
                startService();
                bindService();
                LogUtils.d(TAG," permissions granted success");
            }else {
                // show dialog tips to user
                String tips = getResources().getString(R.string.permission_granted_failed);
                Toast.makeText(ServiceActivity.this, tips, Toast.LENGTH_LONG).show();
                LogUtils.e(TAG," permissions granted failed");
                finish();
            }
        }
    }

    protected void sendRequest(int what, Object obj) {
        if (null == mSendToServiceMessager) {
            return;
        }
        Message msg = Message.obtain(null, what, obj);
        msg.replyTo = mMessenger;

        try {
            mSendToServiceMessager.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        try {
            if (mServiceBind) {
                sendRequest(Messages.UNREGISTER_CLIENT_REQUEST, ServiceActivity.this);
                unbindService(mConnection);
                mServiceBind = false;
            }
        }catch (Exception e){
            e.printStackTrace();
        }
        mSendToServiceMessager = null;
        super.onDestroy();
    }

    protected void handleStartVoiceInput() {}
    protected void handleStopVoiceInput() {}
    protected void handleLoadHtml(String url) {}
    protected void handleRenderVoiceInputText(String text) {}
    protected void handleCallByContacts(List<CandidateInfo> candidateInfos){}
    protected void handleSendSmsByContacts(List<CandidateInfo> candidateInfos, String message){}
    protected void handleStartRecognition() {}
    protected void handleStopRecognition() {}

    protected void executeAfterBindService() {}
    protected void executeAfterUnBindService() {}
}

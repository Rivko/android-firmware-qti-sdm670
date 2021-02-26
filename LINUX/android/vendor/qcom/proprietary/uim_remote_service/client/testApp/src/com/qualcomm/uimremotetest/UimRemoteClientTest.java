/******************************************************************************
  @file    UimRemoteClientTest.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.uimremoteclienttest;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;
import com.qualcomm.uimremoteclient.IUimRemoteClientService;
import com.qualcomm.uimremoteclient.IUimRemoteClientServiceCallback;
import com.qualcomm.uimremoteclienttest.R;

public class UimRemoteClientTest extends Activity {
    private static final String LOG_TAG = "UimRemoteClientTest";
    private IUimRemoteClientService mService = null;
    private UimRemoteClientServiceConnection mConnection = new UimRemoteClientServiceConnection();

    private final IUimRemoteClientServiceCallback mCb = new IUimRemoteClientServiceCallback.Stub() {
        private void updateUI(final String toastStr) {
            Log.d(LOG_TAG, "updateUI() - " + toastStr);
            runOnUiThread(new Runnable(){
                @Override
                public void run(){
                    Toast.makeText(UimRemoteClientTest.this, toastStr, Toast.LENGTH_LONG).show();
                }
            });
        }

        public void uimRemoteEventResponse(int slot, int responseCode) {
            final String toastStr = new String("EventResponse - slot: ") + slot + ", respCode: " + responseCode;
            updateUI(toastStr);
        }

        public void uimRemoteApduResponse(int slot, int responseCode) {
            final String toastStr = new String("ApduResponse - slot: ") + slot + ", respCode: " + responseCode;
            updateUI(toastStr);
        }

        public void uimRemoteApduIndication(int slot, byte[] apduCmd) {
            final String toastStr = new String("ApduIndication - slot: ") + slot + ", apduCmd length" + apduCmd.length;
            updateUI(toastStr);
        }

        public void uimRemoteConnectIndication(int slot) {
            final String toastStr = new String("ConnectIndication - slot: ") + slot;
            updateUI(toastStr);
        }

        public void uimRemoteDisconnectIndication(int slot) {
            final String toastStr = new String("DisconnectIndication - slot: ") + slot;
            updateUI(toastStr);
        }

        public void uimRemotePowerUpIndication(int slot) {
            final String toastStr = new String("PowerUpIndication - slot: ") + slot;
            updateUI(toastStr);
        }

        public void uimRemotePowerDownIndication(int slot) {
            final String toastStr = new String("PowerDownIndication - slot: ") + slot;
            updateUI(toastStr);
        }

        public void uimRemoteResetIndication(int slot) {
            final String toastStr = new String("ResetIndication - slot: ") + slot;
            updateUI(toastStr);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d(LOG_TAG, "onCreate()");

        initService();
    }

    @Override
    protected void onDestroy() {
        Log.d(LOG_TAG, "onDestroy()");
        releaseService();
        super.onDestroy();
    }

    private void initService() {
        Log.i(LOG_TAG, "initService() - Binding to the mService " + IUimRemoteClientService.class.getName());
        Intent i = new Intent();
        i.setComponent(new ComponentName("com.qualcomm.uimremoteclient", "com.qualcomm.uimremoteclient.UimRemoteClientService"));
        boolean ret = bindService(i, mConnection, Context.BIND_AUTO_CREATE);
        Log.d(LOG_TAG, "initService() - bound uimRemote with " + ret);
    }

    private void releaseService() {
        Log.d(LOG_TAG, "releaseService()");
        unbindService(mConnection);
        mConnection = null;
    }

    class UimRemoteClientServiceConnection implements ServiceConnection {
        public void onServiceConnected(ComponentName name, IBinder boundService) {
            mService = IUimRemoteClientService.Stub.asInterface((IBinder) boundService);
            Log.d(LOG_TAG, "onServiceConnected()");
            Toast.makeText(UimRemoteClientTest.this, "Service connected", Toast.LENGTH_LONG).show();
        }

        public void onServiceDisconnected(ComponentName name) {
            mService = null;
            Log.d(LOG_TAG, "onServiceDisconnected()");
            Toast.makeText(UimRemoteClientTest.this, "Service disconnected", Toast.LENGTH_LONG).show();
        }
    }

    public void RegCb(View view) {
        Log.i(LOG_TAG, "RegCb()");
        try {
            int val = mService.registerCallback(mCb);
            Log.d(LOG_TAG, "RegCb() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void UnregCb(View view) {
        Log.i(LOG_TAG, "UnregCb()");
        try {
            int val = mService.deregisterCallback(mCb);
            Log.d(LOG_TAG, "UnregCb() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteEvent(View view) {
        Log.i(LOG_TAG, "uimRemoteEvent()");

        EditText editText;
        String textStr;
        int slot = 0;
        int event = 0;
        byte[] atr = null;
        int errCode = 0;

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientEvent_slot);
        textStr = editText.getText().toString();
        try {
            slot = Integer.valueOf(textStr);
        } catch (Exception e) {
        }

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientEvent_event);
        textStr = editText.getText().toString();
        try {
            event = Integer.valueOf(textStr);
        } catch (Exception e) {
        }

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientEvent_atr);
        textStr = editText.getText().toString();
        if (null != textStr) {
            atr = textStr.getBytes();
        }

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientEvent_errCode);
        textStr = editText.getText().toString();
        try {
            errCode = Integer.valueOf(textStr);
        } catch (Exception e) {
        }

        Log.d(LOG_TAG, "uimRemoteEvent() - slot : " + slot + ", event: " + event + ", errCode: " + errCode);
        if (atr != null) {
            Log.d(LOG_TAG, "atr length: " + atr.length);
        } else {
            Log.d(LOG_TAG, "atr is null");
        }

        try {
            int val = mService.uimRemoteEvent(slot, event, atr, errCode);
            Log.d(LOG_TAG, "uimRemoteEvent() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteApdu(View view) {
        Log.i(LOG_TAG, "uimRemoteApdu()");
        EditText editText;
        Editable text;
        String textStr;
        int slot = 0;
        int apduStatus = 0;
        byte[] apduResp = null;

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientApdu_slot);
        textStr = editText.getText().toString();
        try {
            slot = Integer.valueOf(textStr);
        } catch (Exception e) {
        }

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientApdu_apduStatus);
        textStr = editText.getText().toString();
        try {
            apduStatus = Integer.valueOf(textStr);
        } catch (Exception e) {
        }

        editText = (EditText) findViewById(R.id.editText_UimRemoteClientApdu_apduResp);
        textStr = editText.getText().toString();
        if (null != textStr) {
            apduResp = textStr.getBytes();
        }

        Log.d(LOG_TAG, "uimRemoteApdu() - slot : " + slot + ", apduStatus: " + apduStatus);
        if (apduResp != null) {
            Log.d(LOG_TAG, "apduResp length: " + apduResp.length);
        } else {
            Log.d(LOG_TAG, "apduResp is null");
        }

        try {
            int val = mService.uimRemoteApdu(slot, apduStatus, apduResp);
            Log.d(LOG_TAG, "uimRemoteApdu() returns " + val);
        }
        catch (RemoteException e) {
        }
    }
}

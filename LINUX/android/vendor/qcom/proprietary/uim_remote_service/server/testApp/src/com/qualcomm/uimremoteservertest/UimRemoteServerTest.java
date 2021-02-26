/******************************************************************************
  @file    UimRemoteServerTest.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.uimremoteservertest;

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
import com.qualcomm.uimremoteserver.IUimRemoteServerService;
import com.qualcomm.uimremoteserver.IUimRemoteServerServiceCallback;
import com.qualcomm.uimremoteservertest.R;

public class UimRemoteServerTest extends Activity {
    private static final String LOG_TAG = "UimRemoteServerTest";
    private IUimRemoteServerService mService = null;
    private UimRemoteServerServiceConnection mConnection = new UimRemoteServerServiceConnection();

    private final IUimRemoteServerServiceCallback mCb = new IUimRemoteServerServiceCallback.Stub() {
        private void updateUI(final String toastStr) {
            Log.d(LOG_TAG, "updateUI() - " + toastStr);
            runOnUiThread(new Runnable(){
                @Override
                public void run(){
                Toast.makeText(UimRemoteServerTest.this, toastStr, Toast.LENGTH_LONG).show();
                }
            });
        }

        public void uimRemoteServerConnectResp(int slot, int connStatus, int maxMessageSize) {
            final String toastStr = new String("uimRemoteServerConnectResp() - connStatus: ") + connStatus +
                    ", maxMessageSize: " + maxMessageSize;
            updateUI(toastStr);
        }

        public void uimRemoteServerDisconnectResp(int slot, int disconnStatus) {
            final String toastStr = new String("uimRemoteServerDisconnectResp() - disconnStatus: ") + disconnStatus;
            updateUI(toastStr);
        }

        public void uimRemoteServerApduResp(int slot, int respStatus, byte[] respData) {
            final String toastStr = new String("uimRemoteServerApduResp() - respStatus: ") + respStatus + ", respData length: " + respData.length;
            updateUI(toastStr);
        }

        public void uimRemoteServerTransferAtrResp(int slot, int transferStatus, byte[] atr) {
            final String toastStr = new String("uimRemoteServerTransferAtrResp() - transferStatus: ") + transferStatus + ", atr length: " + atr.length;
            updateUI(toastStr);
        }

        public void uimRemoteServerPowerResp(int slot, int powerStatus) {
            final String toastStr = new String("uimRemoteServerPowerResp() - powerStatus: ") + powerStatus;
            updateUI(toastStr);
        }

        public void uimRemoteServerResetSimResp(int slot, int resetStatus) {
            final String toastStr = new String("uimRemoteServerResetSimResp() - resetStatus: ") + resetStatus;
            updateUI(toastStr);
        }

        public void uimRemoteServerDisconnectInd(int slot, int disconnectType) {
            final String toastStr = new String("uimRemoteServerDisconnectInd() - disconnectType: ") + disconnectType;
            updateUI(toastStr);
        }

        public void uimRemoteServerStatusInd(int slot, int status) {
            final String toastStr = new String("uimRemoteServerStatusInd() - status: ") + status;
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
        Log.i(LOG_TAG, "initService() - Binding to the service " + IUimRemoteServerService.class.getName());
        Intent i = new Intent();
        i.setComponent(new ComponentName("com.qualcomm.uimremoteserver", "com.qualcomm.uimremoteserver.UimRemoteServerService"));
        boolean ret = bindService(i, mConnection, Context.BIND_AUTO_CREATE);
        Log.d(LOG_TAG, "initService() bound uimRemote with " + ret);
    }

    private void releaseService() {
        unbindService(mConnection);
        mConnection = null;
        Log.d(LOG_TAG, "releaseService()");
      }

    class UimRemoteServerServiceConnection implements ServiceConnection {

        public void onServiceConnected(ComponentName name, IBinder boundService) {
          mService = IUimRemoteServerService.Stub.asInterface((IBinder) boundService);
          Log.d(LOG_TAG, "onServiceConnected()");
          Toast.makeText(UimRemoteServerTest.this, "Service connected", Toast.LENGTH_LONG).show();
        }

        public void onServiceDisconnected(ComponentName name) {
          mService = null;
          Log.d(LOG_TAG, "onServiceDisconnected()");
          Toast.makeText(UimRemoteServerTest.this, "Service disconnected", Toast.LENGTH_LONG).show();
        }
    }

    public void regCb(View view) {
        try {
            int val = mService.registerCallback(mCb);
            Log.d(LOG_TAG, "regCb() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void unregCb(View view) {
        try {
            int val = mService.deregisterCallback(mCb);
            Log.d(LOG_TAG, "unregCb() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteServerConnectReq(View view) {
        Log.i(LOG_TAG, "uimRemoteServerConnectReq()");

        EditText editText;
        String textStr;
        int maxMessageSize = 0;
        int slot = 0;

        editText = (EditText) findViewById(R.id.editText_UimRemoteServerConnectReq_maxMessageSize);
        textStr = editText.getText().toString();
        try {
            maxMessageSize = Integer.valueOf(textStr);
        } catch (Exception e) {
        }

        Log.d(LOG_TAG, "uimRemoteServerConnectReq() - maxMessageSize : " + maxMessageSize);

        try {
            int val = mService.uimRemoteServerConnectReq(slot, maxMessageSize);
            Log.d(LOG_TAG, "uimRemoteServerConnectReq() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteServerDisconnectReq(View view) {
        Log.i(LOG_TAG, "uimRemoteServerDisconnectReq()");
        int slot = 0;

        try {
            int val = mService.uimRemoteServerDisconnectReq(slot);
            Log.d(LOG_TAG, "uimRemoteServerDisconnectReq() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteServerApduReq(View view) {
        Log.i(LOG_TAG, "uimRemoteServerApduReq()");

        EditText editText;
        String textStr;
        byte[] cmd = null;
        int slot = 0;

        editText = (EditText) findViewById(R.id.editText_UimRemoteServerApduReq_cmd);
        textStr = editText.getText().toString();
        try {
            cmd = textStr.getBytes();
        } catch (Exception e) {
        }

        Log.d(LOG_TAG, "uimRemoteServerApduReq() - cmd length: " + cmd.length);

        try {
            int val = mService.uimRemoteServerApduReq(slot, cmd);
            Log.d(LOG_TAG, "uimRemoteServerApduReq() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteServerTransferAtrReq(View view) {
        Log.i(LOG_TAG, "uimRemoteServerTransferAtrReq()");
        int slot = 0;

        try {
            int val = mService.uimRemoteServerTransferAtrReq(slot);
            Log.d(LOG_TAG, "uimRemoteServerTransferAtrReq() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteServerPowerReq(View view) {
        Log.i(LOG_TAG, "uimRemoteServerPowerReq()");

        EditText editText;
        String textStr;
        boolean powerState = false;
        int slot = 0;

        editText = (EditText) findViewById(R.id.editText_UimRemoteServerPowerReq_powerState);
        textStr = editText.getText().toString();
        try {
            powerState = Boolean.valueOf(textStr);
        } catch (Exception e) {
        }

        Log.d(LOG_TAG, "uimRemoteServerPowerReq() - powerState : " + powerState);

        try {
            int val = mService.uimRemoteServerPowerReq(slot, powerState);
            Log.d(LOG_TAG, "uimRemoteServerPowerReq() returns " + val);
        }
        catch (RemoteException e) {
        }
    }

    public void uimRemoteServerResetSimReq(View view) {
        Log.i(LOG_TAG, "uimRemoteServerResetSimReq()");
        int slot = 0;

        try {
            int val = mService.uimRemoteServerResetSimReq(slot);
            Log.d(LOG_TAG, "uimRemoteServerResetSimReq() returns " + val);
        }
        catch (RemoteException e) {
        }
    }
}

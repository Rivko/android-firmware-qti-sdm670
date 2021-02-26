/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.sva;

import android.app.Activity;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;

public abstract class ServiceActivity extends Activity {
    private final static String TAG = "ListenLog.ServiceActivity";
    private Messenger sendToServiceMessenger;

    protected abstract Messenger getMMessenger();

    protected abstract void activityOnServiceConnected();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    // Creates a callback method for once the Service is connected
    protected ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.v(TAG, "onServiceConnected");
            sendToServiceMessenger = new Messenger(service);

            // A hook for the subclass activity to do whatever it needs to do once the service is
            // connected.
            activityOnServiceConnected();

            registerService();

            Log.v(TAG, "onServiceConnected: Global.getEnableListen()=  " + Global.getInstance().getSettingEnableListen()
                    + ", Global.getEnableVoiceWakeup= " + Global.getInstance().getSettingEnableVoiceWakeup());
            if (Global.getInstance().getSettingEnableListen()){
                sendDataReply(MessageType.MSG_LISTEN_SET_PARAM, MessageType.MSG_ENABLE, null);
            }
            if (Global.getInstance().getSettingEnableVoiceWakeup()){
                sendDataReply(MessageType.MSG_VOICEWAKEUP_SET_PARAM, MessageType.MSG_ENABLE, null);
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.v(TAG, "onServiceDisconnected");
            unregisterService();
        }
    };

    private void registerService() {
        Log.v(TAG, "registerService");
        Message msg = Message.obtain(null, MessageType.MSG_REGISTER_CLIENT);
        msg.replyTo = getMMessenger();
        sendToService(msg);
    }

    protected void unregisterService() {
        Log.v(TAG, "unregisterService");
        Message msg = Message.obtain(null, MessageType.MSG_UNREGISTER_CLIENT);
        msg.replyTo = getMMessenger();
        sendToService(msg);
        sendToServiceMessenger = null;
    }

    // Sends messages to the Service
    protected void sendReply(int what, Object obj) {
        Log.v(TAG, "sendReply");
        if (null == sendToServiceMessenger) {
            Log.v(TAG, "sendReply: sendToServiceMessenger is null right now- returning");
            return;
        }

        Message msg = Message.obtain(null, what, obj);
        msg.replyTo = getMMessenger();
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    // Sends message to the Service with one or two ints as data
    protected void sendDataReply(int what, int msgArg1, Integer msgArg2) {
        Log.v(TAG, "sendMessageDataAll");
        if (null == sendToServiceMessenger) {
            return;
        }

        Message msg = Message.obtain(null, what);
        msg.arg1 = msgArg1;
        if (null != msgArg2) {
            msg.arg2 = msgArg2.intValue();
        }
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    // Sends message to the Service with one or two ints as data
    public void sendDataReply(int what, Object obj, int msgArg1, Integer msgArg2) {
        Log.v(TAG, "sendMessageDataAll");
        if (null == sendToServiceMessenger) {
            return;
        }

        Message msg = Message.obtain(null, what, obj);
        msg.arg1 = msgArg1;
        if (null != msgArg2) {
            msg.arg2 = msgArg2.intValue();
        }
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    // Sends a message to the service without any associated data
    protected void sendToService(Message msg) {
        Log.v(TAG, "sendToService");
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

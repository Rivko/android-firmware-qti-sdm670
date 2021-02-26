/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.concurrent.atomic.AtomicInteger;

import android.content.Context;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.INetworkManagementService;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

public class AppNetAccessSocket {
    private static final String TAG = "NetControlSocket";

    private OutputStream  mOutStream;
    private InputStream mInStream;
    private LocalSocket mSocket;
    private AtomicInteger mSequenceNumber;
    private static final int RESTRICT_TYPE_DATA = 0;
    private static final int RESTRICT_TYPE_WLAN = 1;
    private boolean mConnected = false;

    public AppNetAccessSocket() {
        mSequenceNumber = new AtomicInteger(0);
    }

    private String makeCommand(String cmd, Object... args) {
        StringBuilder cmdBuilder = new StringBuilder();
        int sequenceNumber = mSequenceNumber.incrementAndGet();
        cmdBuilder.append(sequenceNumber).append(' ').append(cmd);
        for (Object arg : args) {
            final String argString = String.valueOf(arg);
            cmdBuilder.append(' ');
            cmdBuilder.append(argString);
        }
        cmdBuilder.append('\0');
        String cmdString = cmdBuilder.toString();
        return cmdString;
    }

    private boolean deamonEnabled() {
        boolean enabled = false;
        INetworkManagementService networkService = INetworkManagementService.Stub.asInterface(
                ServiceManager.getService(Context.NETWORKMANAGEMENT_SERVICE));
        try {
            if (networkService.isBandwidthControlEnabled()) {
                enabled = true;
            }
        } catch (RemoteException e) {
            Log.w(TAG, "No bandwidth control; leaving");
            enabled = false;
        }
       return enabled;
    }
    public boolean connectToSocket() {
        if (mConnected) return true;
        boolean connected = false;

        try {
            mSocket = new LocalSocket();
            LocalSocketAddress address = new LocalSocketAddress("netd",
                    LocalSocketAddress.Namespace.RESERVED);
            mSocket.connect(address);
            mInStream = mSocket.getInputStream();
            mOutStream = mSocket.getOutputStream();
            if (mOutStream != null) {
                mConnected =  true;
                connected = true;
                if (!deamonEnabled()) {
                    String enable = makeCommand("bandwidth", "enable");
                    mOutStream.write(enable.getBytes());
                    mOutStream.flush();
                } else {
                    Log.d(TAG, "Native deamon enabled");
                }
            }
        } catch (IOException ex) {
            Log.d(TAG, "exception = " + ex.toString());
        }
        return connected;
    }

    public void closeSocket() {
        if (mSocket == null) {
            return;
        }
        try {
            mSocket.close();
        } catch (IOException ex) {
            Log.e(TAG, "exception when close socket");
        }
        mConnected = false;
    }

    private boolean setAppRestrict(boolean restrict, int uid, int type) {
        boolean result = true;
        String arg = null;
        if (type == RESTRICT_TYPE_DATA) {
            arg = restrict ? "addrestrictappsondata" : "removerestrictappsondata";
        } else {
            arg = restrict ? "addrestrictappsonwlan" : "removerestrictappsonwlan";
        }
        String fullCmd = makeCommand("bandwidth", arg, uid);
        Log.d(TAG, "fullCmd = " + fullCmd);
        try {
            mOutStream.write(fullCmd.getBytes());
            mOutStream.flush();
        } catch (IOException ex) {
            Log.e(TAG, "exception when write to socket");
            result = false;
        }
        return result;

    }
    public boolean setAppRestrictMobile(boolean restrictMobile, int uid) {
        return setAppRestrict(restrictMobile, uid, RESTRICT_TYPE_DATA);
    }

    public boolean setAppRestrictWlan(boolean restrictWlan, int uid) {
        return setAppRestrict(restrictWlan, uid, RESTRICT_TYPE_WLAN);
    }
}

/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.util.Log;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;

public class TcpCommandClient implements Runnable {
    static final String TAG = "TcpCommandClient";

    private final InetAddress mAddr;
    private final int mPort;
    private TcpCommand mCmd;

    public TcpCommandClient(InetAddress addr, int port) {
        mAddr = addr;
        mPort = port;
    }

    // not thread-safe, can called only once on an instance
    public void sendACommand(TcpCommand cmd) {
        Log.v(TAG, "send a command " + cmd);
        mCmd = cmd;
        Thread thread = new Thread(this, "TcpCommandClient");
        thread.start();
    }

    @Override
    public void run() {
        try {
            connectAndSendACommand();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void connectAndSendACommand() throws IOException {
        Log.v(TAG, "connectAndSendACommand");
        Socket socket = new Socket(mAddr, mPort);
        try {
            socket.getOutputStream().write(mCmd.toBuffer());
        } finally {
             socket.close();
        }
    }
}

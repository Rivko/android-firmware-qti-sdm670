/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.util.Log;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;

class TcpCommandServer implements Runnable {
    static final String TAG = "TcpCommandServer";

    interface CommandListener {
        void onConnectionRequestCommand(InetAddress addr, int port);
    }

    private ServerSocket mServerSocket;
    private Thread mServerThread;
    private CommandListener mListener;

    public boolean start() {
        Log.v(TAG, "start");
        if (mServerSocket != null) {
            throw new IllegalStateException("Server is already started");
        }
        try {
            mServerSocket = new ServerSocket(0);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        mServerThread = new Thread(this, "TcpCommandServer");
        mServerThread.start();
        return true;
    }

    public void stop() {
        Log.v(TAG, "stop");
        if (mServerSocket == null) {
            throw new IllegalStateException("Server is not started");
        }
        try {
            mServerSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        mServerSocket = null;
    }

    public boolean isStarted() {
        return mServerSocket != null;
    }

    public void setListener(CommandListener listener) {
        mListener = listener;
    }

    int getServerPort() {
        return mServerSocket.getLocalPort();
    }

    @Override
    public void run() {
        while (true) {
            try {
                Log.v(TAG, "waiting for connection");
                Socket socket = mServerSocket.accept();
                handleASocket(socket);
            } catch (IOException e) {
                e.printStackTrace();
                break;
            }
        }
        Log.v(TAG, "quit server thread");
    }

    private void handleASocket(Socket socket) throws IOException {
        try {
            handleACommand((InetSocketAddress) socket.getRemoteSocketAddress(),
                    new TcpCommand(socket.getInputStream()));
        } finally {
            socket.close();
        }
    }

    private void handleACommand(InetSocketAddress addr, TcpCommand cmd) {
        switch (cmd.getType()) {
            case TcpCommand.TYPE_REQUEST_CONNECTION:
                int port = ByteBuffer.wrap(cmd.getValue()).getShort() & 0xffff;
                Log.v(TAG, "request connection command " + addr.getAddress().getHostAddress() + ":" + port);
                if (mListener != null) {
                    mListener.onConnectionRequestCommand(addr.getAddress(), port);
                }
                break;
            default:
                Log.e(TAG, "Unsupported cmd " + cmd.getType());
                break;
        }
    }

}

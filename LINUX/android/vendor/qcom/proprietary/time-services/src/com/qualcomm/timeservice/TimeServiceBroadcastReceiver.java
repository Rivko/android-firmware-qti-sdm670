/*******************************************************************************
@file    TimeServiceBroadcastReceiver.java
@brief   Updates time-services user time offset when user changes time of
         the day and Android sends a TIME_CHANGED or DATE_CHANGED intents.
         time-services restores the time of the day after reboot using
         this offset.
---------------------------------------------------------------------------
Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.timeservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.os.SystemClock;
import android.os.PowerManager;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import java.io.IOException;

import java.io.InputStream;
import java.io.OutputStream;

public class TimeServiceBroadcastReceiver extends BroadcastReceiver {

    private static final String TAG = "TimeService";

    private static final String SOCKET_NAME = "time_genoff";

    private static final int INT_SIZE = 4;
    private static final int LONG_SIZE = 8;
    private static final int BIT_IN_BYTE = 8;

    private static final int ATS_USER_BASE = 2;
    private static final int TIME_MSEC_UNIT = 1;
    private static final int T_SET_OPERATION = 0;
    private static final int INIT_SEND_RESULT = -1;

    private static final int BYTES_TO_SEND = 24;
    private static final int BYTES_TO_RECEIVE = 28;

    private LocalSocket mSocket;

    private InputStream mIn;
    private OutputStream mOut;

    private SendData data = null;
    @Override
    public void onReceive(Context context, Intent intent) {

        if ((Intent.ACTION_TIME_CHANGED.equals(intent.getAction())) ||
            (Intent.ACTION_DATE_CHANGED.equals(intent.getAction()))) {

            Log.d(TAG, "Received" + intent.getAction() + " intent. " +
                       "Current Time is " + System.currentTimeMillis());
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            PowerManager.WakeLock wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
            wakeLock.acquire();

            try {
                mSocket = new LocalSocket();
                LocalSocketAddress address = new LocalSocketAddress(SOCKET_NAME,
                        LocalSocketAddress.Namespace.ABSTRACT);

                mSocket.connect(address);
                mIn = mSocket.getInputStream();
                mOut = mSocket.getOutputStream();

                data = new SendData(ATS_USER_BASE, TIME_MSEC_UNIT, T_SET_OPERATION,
                        INIT_SEND_RESULT, System.currentTimeMillis());

                mOut.write(data.getBuf());

                byte[] receive = new byte[BYTES_TO_RECEIVE];
                mIn.read(receive);
                int ret = data.getSendResult(receive);
                if (ret < 0) {
                    Log.d(TAG, "Failed to set time " + ret);
                } else {
                    Log.d(TAG, "Set time successfully!");
                }

            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                try {
                    if (mOut != null) {
                        mOut.close();
                        mOut = null;
                    }

                    if (mIn != null) {
                        mIn.close();
                        mIn = null;
                    }

                    if (mSocket != null) {
                        mSocket.close();
                        mSocket = null;
                    }
                } catch (IOException e) {
                    Log.e(TAG, "Failed closing socket: " + e);
                }
            }

            wakeLock.release();
        } else {
            Log.w(TAG, "Received Unexpected Intent " + intent.toString());
        }
    }


    class SendData {

        private byte[] buf = new byte[BYTES_TO_SEND];
        public int base;
        public int unit;
        public int operation;
        public int result;
        public long value;

        public SendData(int base, int unit, int operation, int result, long value) {
            this.base = base;
            this.unit = unit;
            this.operation = operation;
            this.result = result;
            this.value = value;

            byte[] temp = intToByte(base);
            System.arraycopy(temp, 0, buf, 0, temp.length);

            temp = intToByte(unit);
            System.arraycopy(temp, 0, buf, INT_SIZE, temp.length);

            temp = intToByte(operation);
            System.arraycopy(temp, 0, buf, INT_SIZE * 2, temp.length);

            temp = intToByte(result);
            System.arraycopy(temp, 0, buf, INT_SIZE * 3, temp.length);

            temp = longToByte(value);
            System.arraycopy(temp, 0, buf, INT_SIZE * 4, temp.length);
        }

        public int getSendResult(byte[] bArr) {
            byte[] temp = new byte[INT_SIZE];

            System.arraycopy(bArr, BYTES_TO_SEND, temp, 0, INT_SIZE);
            int result = byteToInt(temp);

            return result;
        }

        private byte[] intToByte(int value) {
            byte[] src = new byte[INT_SIZE];
            for (int i = 0; i < src.length; i++) {
                src[i] = (byte) ((value >> (BIT_IN_BYTE * i)) & 0XFF);
            }
            return src;
        }

        private int byteToInt(byte[] b) {
            int value = 0;
            for (int i = 0; i < b.length; i++) {
                value |= (b[i] & 0XFF) << (BIT_IN_BYTE * i);
            }

            return value;
        }

        private byte[] longToByte(long value) {

            byte[] src = new byte[BIT_IN_BYTE];
            for (int i = 0; i < src.length; i++) {
                src[i] = (byte) ((value >> (BIT_IN_BYTE * i)) & 0XFF);
            }
            return src;
        }

        public byte[] getBuf() {
	    return buf;
        }
    }
}

/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.android.qti.qpay;

import android.content.Context;
import android.os.RemoteException;
import android.util.Log;
import java.lang.ArithmeticException;
import java.lang.Byte;
import java.util.Arrays;

public final class QPayAdapter {

    final boolean DBG = true;
    boolean isPending;
    boolean isIrqPending;
    boolean hasAnswered;
    boolean isWtx;
    boolean power_cmdid;
    private final int TEE_SEReaderOpenSession = 53;
    private final int TEE_SEReaderCloseSessions = 54;
    private final int TEE_SESessionGetATR = 61;
    private final int TEE_SESessionClose = 63;
    private final int TEE_SESessionCloseChannels = 64;
    private final int TEE_SESessionOpenBasicChannel = 65;
    private final int TEE_SESessionOpenLogicalChannel = 66;
    private final int TEE_SEChannelClose = 71;

    private final byte[] OK = {0x00};
    private final byte[] WTX_REQ = {0x06, (byte)0xc3};

    byte irqPending = 0x06;
    byte channelPending = 0x07;
    byte[] mSW = null;
    byte[] rApdu = null;
    byte[] taFailed = new byte[] {(byte)0xFF};
    byte[] cmdFailed = new byte[] {(byte)0xFF, (byte)0xFF};
    int channelNumber;
    int sessionCount = 0;
    int channelCount = 0;
    Context mContext;

    String TAG = "QPayAdapter";

    static {
        System.loadLibrary("QPayJNI");
    }

    public QPayAdapter(Context context) throws NullPointerException {
        if (context != null) {
            if (DBG) Log.d(TAG, "QpayAdapter Constructor ");
            mContext = context;
        } else {
            throw new NullPointerException(TAG + " - " + "Context is null");
        }
    }

    //native methods
    public static native byte[] nativeOpen();
    public static native byte[] nativeClose();
    public static native byte[] nativeTransceive(int cmd_id, byte[] apdu, int session_handle, int channel_handle);
    public static native int nativeInit();
    public static native void nativeUninit();
    public static native int nativeGetSessionHandle();
    public static native int nativeGetChannelHandle();
    public static native int nativeGetApduMaxSize();
    public static native int nativeeSEPowerOn();
    public static native int nativeeSEPowerOff();
    public static native int nativeeSEGetState();

   /*
    * method used to exchange data with the TA app :
    * as soon as we'll send an apdu to the TA app, if the TA APP has to communicate with the eSE,
    * it will answer "FF FF". in this case, we'll wait 5ms, before sending again the same apdu, with the FF status byte.
    * if the eSE has answered or the TA does not need to talk to the eSE, the TA APP will forward the rApdu (if it exists)
    * plus the status byte.
    * if the status byte is 00 or FF, followed by the rAPDU, the communication with the eSE is done & successfull,
    * and we can forward the rAPDU to the upper layer.
    * if the rAPDU is 00 (only 1 byte length), it means the TA did not have to talk with the eSE (getReaders or OpenSession for ex.)
    * and the communication with the TA has been successful.
    * if the rAPDU is FF, an issue happened.
    * if the rAPDU is a WTX request, we'll wait BWT before coming back to the TA to get the eSE answer.
    *
    */
    public byte[] genericTransceive(int cmd_id, byte[] buff, int sessionHandle, int channelHandle){

        try {
            do {
                rApdu = nativeTransceive(cmd_id, buff, sessionHandle, channelHandle);
                if (rApdu == null) {
                    isPending = false;
                    throw new RemoteException("Unable to communicate with the TA.");
                }
                buff[0] = rApdu[0];
                try {
                    synchronized(this){
                        wait(5);
                    }
                } catch (Exception e) {
                    isPending = false;
                    Log.d(TAG, "Interupted while waiting for eSE answer: " + e);
                }
                isIrqPending = ((rApdu[0] == irqPending) || (rApdu[0] == channelPending));
                hasAnswered =  ((rApdu[0] == 0x00) || ((rApdu[0] & 0xFF) == 0xFF));
                isWtx = ((rApdu.length == 3) && ((rApdu[1] & 0xFF) == 0xC3) );

                if (hasAnswered)
                    Log.d(TAG, " HAS ANSWERED : " + byteArrayToHex(rApdu) + " for cmd_id : " + Integer.toString(cmd_id));
                if (isWtx) {
                    int timer = ((rApdu[2] & 0xFF) * 1000) - 5;
                    if (DBG) Log.d(TAG, "WTX Request - Let's wait for : " + Integer.toString(timer) + " ms");
                    try {
                        synchronized(this){
                            wait(timer);
                        }
                    } catch (Exception e) {
                        Log.d(TAG, "Interupted while waiting for BWT : " + e);
                        isPending = false;
                        return cmdFailed;
                    }
                }
            } while (!(hasAnswered));
        } catch (Exception e) {
            Log.d(TAG, "Interupted while trying to communicate with the eSE - let's close everything" + e);
            close();
            return cmdFailed;
        }

        if(isPending){
            isPending = false;
        }

        if (Arrays.equals(rApdu,taFailed)) {
            Log.d(TAG, "rApdu received FAILED for cmd_id : " + Integer.toString(cmd_id) + " returning :" + byteArrayToHex(rApdu));
            return cmdFailed;

        } else if (rApdu[0] == 0x00) {
            if (rApdu.length > 1) {
                byte[] ret = Arrays.copyOfRange(rApdu, 1, rApdu.length);
                Log.d(TAG, "SUCCESS - returning :" + byteArrayToHex(ret));
                return ret;
            } else {
                Log.d(TAG, "SUCCESS - returning :" + byteArrayToHex(rApdu));
                return rApdu;
            }
        } else if ((((rApdu[1] & 0xFF) == 0x6a) || ((rApdu[1] & 0xFF) == 0x68)) && ((rApdu[2] & 0xFF) == 0x81)){
            //specific management of the manage channel command issue (no more channel available)
            isPending = false;
            Log.d(TAG, "manage channel failed, returning :" + byteArrayToHex(rApdu));
            return rApdu;
        } else {
            Log.d(TAG, "rApdu received FAILED, returning :" + byteArrayToHex(rApdu));
            return cmdFailed;
        }
    }

    public byte[] transceive(int cmd_id, byte[] cmd, int sessionHandle, int channelHandle) {
        int cmdLen = 0;
        byte[] ret = null;
        byte[] buff = null;
        if (cmd_id == TEE_SESessionGetATR) {
            /* NQ220 / NQ330 ATR not available over SPI */
            return null;
        }
        if (!eSEPower(true)) {
            Log.e(TAG, " eSE Not powered on");
            return null;
        }
        do {
            try {
                synchronized(this){
                    wait(5);
                }
            } catch (Exception e) {
                Log.d(TAG, "Interupted while waiting Channel/IRQ pending to be released " + e);
            }
        } while (isPending);

        isPending = true;
        if (cmd == null) {
            buff = new byte[1];
        } else {
            if (DBG) Log.d(TAG, " Command received from SCS : " + byteArrayToHex(cmd));
            cmdLen = cmd.length;
            if (cmdLen > getApduMaxSize()) {
                if (DBG) Log.e(TAG, "The Command received from SCS is too long: " + Integer.toString(cmdLen) + "bytes");
                return null;
            }
            buff = new byte[cmdLen+1];
        }
        buff[0] = 0x00;
        for (int i = 1; i < buff.length; i++)
            {
                buff[i] = cmd[i-1];
            }
        ret = genericTransceive(cmd_id, buff, sessionHandle, channelHandle);
        if (!(Arrays.equals(ret,cmdFailed) || (ret[0] & 0xFF) == 0XFF)) {
            if (cmd_id == TEE_SEReaderOpenSession) {
                ++sessionCount;
                power_cmdid = true;
            } else if (cmd_id == TEE_SEReaderCloseSessions) {
                sessionCount = channelCount = 0;
                power_cmdid = true;
            } else if (cmd_id == TEE_SESessionClose) {
                --sessionCount;
                power_cmdid = true;
            } else if (cmd_id == TEE_SESessionCloseChannels) {
                if (sessionCount == 1) {
                    channelCount = 0;
                }
                power_cmdid = true;
            } else if ((cmd_id == TEE_SESessionOpenBasicChannel) ||
                 (cmd_id == TEE_SESessionOpenLogicalChannel)) {
                ++channelCount;
                power_cmdid = true;
            } else if (cmd_id == TEE_SEChannelClose) {
                --channelCount;
                power_cmdid = true;
            }
            if (((channelCount == 0) && (sessionCount == 0)) && power_cmdid) {
                if (eSEPower(false)) {
                    Log.d(TAG, "eSE powered off");
                } else {
                    Log.e(TAG, "Failed to power off the eSE");
                }
                power_cmdid = false;
            } else {
                Log.d(TAG, channelCount + " channel(s) & " + sessionCount + " session(s)");
            }
        }
        return ret;
    }

    public int getChannelHandle(){
        return nativeGetChannelHandle();
    }

    public int getSessionHandle(){
        return nativeGetSessionHandle();
    }

    public int getApduMaxSize() {
        return nativeGetApduMaxSize();
    }

    public boolean iseSEPoweredON() {
        if (nativeeSEGetState() == 1)
            return true;
        return false;
    }
    public boolean getNfcState(){
        /*
         *since we implemented the eSE power management, we are
         * not NFC dependant anymore. we return true to keep the
         * compatibility with the actual version of the SmartcardService
        */
        return true;
    }

    public boolean open() {
        if (nativeOpen()!=null)
            return true;
        return false;
    }

    public boolean close() {
        if (eSEPower(false))
            nativeUninit();
        return true;
    }

    private boolean eSEPower(boolean state) {
        if (state) {
            return (nativeeSEPowerOn() == 1);
        } else {
            if (nativeeSEPowerOff() == 0) {
                sessionCount = channelCount = 0;
                isPending = false;
                return true;
            }
        }
        return false;
    }

    public static String byteArrayToHex(byte[] a) {
        if ((a.length == 0) || (a == null))
            return ("length null");
        else if (a.length < 2)
            return Byte.toString(a[0]);
        else {
            try {
                StringBuilder sb = new StringBuilder(a.length * 2);
                for(byte b: a)
                sb.append(String.format("%02x", b & 0xff));
                return sb.toString();
            } catch (ArithmeticException e) {
                return ("ArithmeticException caught " + e);
            }
       }
    }
}

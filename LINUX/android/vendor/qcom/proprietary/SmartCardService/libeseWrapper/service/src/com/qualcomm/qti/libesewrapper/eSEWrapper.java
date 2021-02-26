/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.libesewrapper;

import android.content.Context;
import android.util.Log;

public final class eSEWrapper {

    private final String TAG= "eSEWrapper";
    private final boolean DBG = false;

    static {
        System.loadLibrary("eSEWrapperJNI");
    }

    public eSEWrapper(Context context) throws NullPointerException {
        if (context != null) {
            if (DBG) Log.d(TAG, "eSEWrapper Constructor ");
        } else {
            throw new NullPointerException(TAG + " - " + "Context is null");
        }
    }

    //native methods
    public static native int nativeOpen();
    public static native int nativeClose();
    public static native byte[] nativeTransceive(byte[] txapdu);

    public boolean openConnection() {
        return (nativeOpen() == 0);
    }

    public boolean closeConnection() {
        return (nativeClose() == 0);
    }

    public byte[] transceive(byte[] buff){
        if (DBG) Log.d(TAG, "eSEWrapper tApdu : " + byteArrayToHex(buff));
        byte[] rApdu = nativeTransceive(buff);
        if (DBG) Log.d(TAG, "eSEWrapper rApdu : " + byteArrayToHex(rApdu));
        return rApdu;
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
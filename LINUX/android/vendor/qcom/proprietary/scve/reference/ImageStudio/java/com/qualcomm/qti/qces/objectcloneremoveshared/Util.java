/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.objectcloneremoveshared;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Environment;
import android.widget.Toast;

import com.qualcomm.qti.qces.imageedit.softcut.R;

/**
 * Generic utility functions that don't belong anywhere else.
 * @author slaver
 */
public class Util {
    /**
     * Not constructable, static helper utils only
     */
    private Util() {
    }

    /**
     * Test if an OpenCL library is available on this device
     * @return true if libopencl.so is present, false otherwise
     */
    public static boolean isOpenCLAvailable() {
        try {
            System.loadLibrary("OpenCL");
            return true;
        } catch (UnsatisfiedLinkError e) {
            return false;
        }
    }

    /**
     * Display a dialog indicating that the device is not compatible with this app. After the dialog
     * is dismissed (for any reason, including cancelled), finish the app.
     * @param context Context in which this dialog will be displayed
     * @return an AlertDialog with an error message indicating the device is incompatible, and a
     *         single button allowing the user to acknowledge.
     */
    public static AlertDialog createDeviceIncompatibleDialog(Context context) {
        AlertDialog.Builder adb = new AlertDialog.Builder(context);
        AlertDialog ad = adb.setMessage(R.string.device_incompatible_error)
                .setPositiveButton(android.R.string.ok, null).create();
        return ad;
    }

    /**
     * Tests if external storage is mounted. If not, a toast will be displayed to the user asking
     * them to mount the external storage to continue.
     * @param context Context in which an error toast will be displayed if external storage is not
     *        mounted
     * @return true if external storage is mounted, false otherwise
     */
    public static boolean verifyExternalStorageMounted(Context context) {
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            return true;
        }

        Toast.makeText(context, R.string.media_not_mounted, Toast.LENGTH_SHORT).show();

        return false;
    }
}

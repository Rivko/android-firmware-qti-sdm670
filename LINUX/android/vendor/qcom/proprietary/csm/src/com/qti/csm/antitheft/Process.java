/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.antitheft;

import android.app.admin.DevicePolicyManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.res.Resources;
import android.telephony.SmsManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qti.csm.R;

import java.util.Random;

public class Process {
    private final static Object lock = new Object();

    //Command define
    private final static int COMMAND_INVALID = 0;
    private final static int COMMAND_FOUND = 1;
    private final static int COMMAND_LOCK = 2;
    private final static int COMMAND_WIPE = 3;
    private final static String MSG_LOCK = "#Lock";
    private final static String MSG_FOUND = "#Found";
    private final static String MSG_WIPE = "#Wipe";


    private static void newRequest(Context context, String senderNumber, int command) {

        synchronized (lock) {
            String message;
            String pin;
            Resources res = context.getResources();
            if (Common.LOG)
                Log.d(Common.TAG, "Type: " + command);
            if (context == null) {
                Log.e(Common.TAG, "Unknown input");
                return;
            }
            DevicePolicyManager devicePolicyManager = (DevicePolicyManager) context
                    .getSystemService(Context.DEVICE_POLICY_SERVICE);
            ComponentName deviceAdmin =
                    new ComponentName(context, SystemManager.class);

            if (!isDeviceAdminActive(devicePolicyManager, deviceAdmin)) {
                message = res.getString(R.string.message_dev_admin_not_enabled);
                sendMessage(senderNumber, message);
                return;
            }

            switch (command) {
                case COMMAND_FOUND:
                    Log.d(Common.TAG, "Found");
                    break;
                case COMMAND_LOCK:
                    int min = 99999;
                    int max = 99999999;
                    Random r = new Random();
                    int iPin = r.nextInt(max - min + 1) + min;
                    pin = "" + iPin;
                    devicePolicyManager.lockNow();
                    boolean re = devicePolicyManager.resetPassword(pin, 0);
                    if (re) {
                        message = String.format(res.getString(R.string.message_new_pin), pin);
                    } else {
                        message = res.getString(R.string.message_lock);
                    }
                    sendMessage(senderNumber, message);
                    break;
                case COMMAND_WIPE:
                    message = res.getString(R.string.message_wipe);
                    sendMessage(senderNumber, message);
                    devicePolicyManager.wipeData(0);
                    break;
                default:
                    break;
            }
        }
    }

    private static boolean isDeviceAdminActive(DevicePolicyManager devicePolicyManager,
                                               ComponentName deviceAdmin) {
        if (devicePolicyManager != null && devicePolicyManager.isAdminActive(deviceAdmin)) {
            return true;
        }
        return false;
    }


    public static boolean telInfoUpdate(Context context, final String senderNumber,
                                        final String message) {
        if (context == null) {
            Log.e(Common.TAG, "Unknown context");
            return false;
        }
        if (senderNumber == null) {
            Log.e(Common.TAG, "No number");
            return false;
        }

        String friendNumber = Configuration.getFriendNumber(context);
        String confirmationCode = Configuration.getConfirmationCode(context);
        if (friendNumber == null) {
            Log.e(Common.TAG, "Friend Number not set. Returning!");
            return false;
        }
        if (confirmationCode == null) {
            Log.e(Common.TAG, "Confirmation code not set. Returning!");
            return false;
        }
        if (senderNumber.contains(friendNumber) == false) {
            if (Common.LOG)
                Log.d(Common.TAG, "Ignore info");
            return false;
        }

        if (Common.LOG)
            Log.d(Common.TAG, "\tFrom: " + senderNumber + "\tmsg: " + message);

        int command = COMMAND_INVALID;
        if (MSG_FOUND.equalsIgnoreCase(message))
            command = COMMAND_FOUND;
        else if (MSG_LOCK.equalsIgnoreCase(message))
            command = COMMAND_LOCK;
        else if (MSG_WIPE.equalsIgnoreCase(message))
            command = COMMAND_WIPE;

        if (command != COMMAND_INVALID) {
            newRequest(context, senderNumber, command);
            return true;
        }

        return false;
    }

    private static void sendMessage(String senderNumber, String message) {
        SmsManager.getDefault().sendTextMessage(senderNumber, null, message,
                null, null);
    }

    public static void updateTel(Context context) {
        if (context == null) {
            Log.e(Common.TAG, "Context is null. Returning!");
            return;
        }
        synchronized (lock) {
            TelephonyManager telephonyMgr = (TelephonyManager) context
                    .getSystemService(Context.TELEPHONY_SERVICE);
            String newSubscriberId = telephonyMgr.getSubscriberId();

            if (newSubscriberId == null)
                return;

            String oldSubscriberId = Configuration.getSubscriberId(context);
            if (newSubscriberId.equals(oldSubscriberId)) {
                if (Common.LOG)
                    Log.d(Common.TAG, "Done tel");
                return;
            }

            if (Common.LOG)
                Log.d(Common.TAG, "New tel");

            String newNumber = telephonyMgr.getLine1Number();
            if (newNumber == null) {
                Log.e(Common.TAG, "try wait");
                try {
                    Thread.sleep(10000);
                    Log.e(Common.TAG, "Done waiting.");
                } catch (InterruptedException e) {
                    Log.e(Common.TAG, "Sleep wrong");
                }
            }
            Resources res = context.getResources();
            String message = res.getString(R.string.message_new_number);
            String friendNumber = Configuration.getFriendNumber(context);
            if (friendNumber != null)
                sendMessage(friendNumber, message);
            Configuration.setSubscriberId(context, newSubscriberId);
        }
    }
}

// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
package com.qualcomm.qti.seccamapi;

import android.os.Bundle;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;

public class SecCamServiceVendorClient {

    private final static String LOG_TAG = "SECCAM-SERVICE-VENDOR-CLIENT";

    // Vendor specific command IDs
    public static final int MSG_VENDOR_EXCHANGE_TIMESTAMP     = 2000;
    public static final int MSG_VENDOR_INIT_BIOMETRICS_BUFFER = 2001;
    public static final int MSG_VENDOR_RUN_BIOMETRICS_TEST    = 2002;
    public static final int MSG_VENDOR_START_MLVMAPP          = 2003;
    public static final int MSG_VENDOR_STOP_MLVMAPP           = 2004;

    public static boolean handleVendorMessage(Message msg) {
        Log.d(LOG_TAG, "handleVendorMessage");
        boolean ret = false;

        switch (msg.what) {
            case MSG_VENDOR_EXCHANGE_TIMESTAMP: {
                Bundle in_bundle = msg.getData();
                long result = in_bundle.getLong("tzTimestamp");
                Log.d(LOG_TAG, "handleVendorMessage(EXCHANGE TIMESTAMP) - TA timestamp recieved: " + result);
                ret = true;
                break;
            }
            case MSG_VENDOR_INIT_BIOMETRICS_BUFFER: {
                Bundle in_bundle = msg.getData();
                long result = in_bundle.getLong("result");
                Log.d(LOG_TAG, "handleVendorMessage(INIT BIOMETRICS BUFFER) - result: " + result);
                ret = true;
                break;
            }
            case MSG_VENDOR_RUN_BIOMETRICS_TEST: {
                Bundle in_bundle = msg.getData();
                long result = in_bundle.getLong("result");
                Log.d(LOG_TAG, "handleVendorMessage(RUN BIOMETRICS TEST) - result: " + result);
                ret = true;
                break;
            }
            case MSG_VENDOR_START_MLVMAPP: {
                Bundle in_bundle = msg.getData();
                long result = in_bundle.getLong("result");
                Log.d(LOG_TAG, "handleVendorMessage(MSG_VENDOR_START_MLVMAPP) - result: " + result);
                ret = true;
                break;
            }
            case MSG_VENDOR_STOP_MLVMAPP: {
                Bundle in_bundle = msg.getData();
                long result = in_bundle.getLong("result");
                Log.d(LOG_TAG, "handleVendorMessage(MSG_VENDOR_STOP_MLVMAPP) - result: " + result);
                ret = true;
                break;
            }

            default:
                Log.d(LOG_TAG, "recieved unhandled command ID");
        }

        return ret;
    }
}

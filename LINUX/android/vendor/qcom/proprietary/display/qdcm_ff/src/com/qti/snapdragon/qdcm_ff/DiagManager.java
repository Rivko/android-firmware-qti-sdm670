/* ====================================================================
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
* =====================================================================
* @file DiagManager.java
*
*/

package com.qti.snapdragon.qdcm_ff;

import android.util.Log;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

public class DiagManager {

    private static final String LOG_TAG = "QDCM_FF_Mobile";

    public DiagManager() {
    }

    public boolean enableDiagOverSockets(String ipAddress, String port) {
        Log.d(LOG_TAG, "ip: " + ipAddress + " port: " + port);

        String command = "diag_socket_log -a " + ipAddress + " -p " + port + " &";
        Process p;
        try {
            p = Runtime.getRuntime().exec(command);
        }
        catch (Exception e) {
            Log.d(LOG_TAG, "enableDiagOverSockets(): " + e.getMessage());
            return false;
        }

        return true;
    }
}



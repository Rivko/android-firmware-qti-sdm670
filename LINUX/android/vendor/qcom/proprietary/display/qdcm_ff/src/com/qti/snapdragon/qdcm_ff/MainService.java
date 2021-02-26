/* ====================================================================
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file MainService.java
 *
 */

package com.qti.snapdragon.qdcm_ff;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;
import android.os.RemoteException;

import vendor.display.color.V1_1.IDisplayColor;

public class MainService extends IntentService {

    private static final String LOG_TAG = "QDCM_FF";
    private static final String INTENT_START_SERVICE = "com.qti.snapdragon.qdcm_ff.START_SERVICE";
    private static final String INTENT_STOP_SERVICE = "com.qti.snapdragon.qdcm_ff.STOP_SERVICE";

  public MainService() {
        super("MainService");
  }

  @Override
  protected void onHandleIntent(Intent intent) {
        Log.d(LOG_TAG, "Service started");

        final String action = intent.getAction();
        Log.d(LOG_TAG, "Receiver Intent: " + action);

        if (action.equals(INTENT_START_SERVICE)) {
           startQdcmSocketService();
        }
        else if (action.equals(INTENT_STOP_SERVICE)) {
           stopQdcmSocketService();
        }
  }

    /**
     * Launch the qdcm socket service by calling HIDL API in color service
     */
    private void startQdcmSocketService() {
        try {
            IDisplayColor mDisplayColor = IDisplayColor.getService();
            if (mDisplayColor == null) {
                Log.d(LOG_TAG, "mDisplayColor is null");
            }
            else {
                Log.d(LOG_TAG, "Starting Qdcm Socket Service...");
                mDisplayColor.toggleSocketService(true);
            }
        } catch (RemoteException e) {
            Log.d(LOG_TAG, "RemoteException:" + e);
        }
    }

    /**
     * Close the qdcm socket service by calling HIDL API in color service
     */
    private void stopQdcmSocketService() {
        try {
            IDisplayColor mDisplayColor = IDisplayColor.getService();
            if (mDisplayColor == null) {
                Log.d(LOG_TAG, "mDisplayColor is null");
            }
            else {
                Log.d(LOG_TAG, "Stopping Qdcm Socket Service...");
                mDisplayColor.toggleSocketService(false);
            }
        } catch (RemoteException e) {
            Log.d(LOG_TAG, "RemoteException:" + e);
        }
    }

}

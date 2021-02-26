/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import java.util.Arrays;

import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.auth.healthauthservice.IHealthAuthListener;

public class HealthAuthListenerImpl extends IHealthAuthListener.Stub {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public HealthAuthListenerImpl(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onDeviceHealthReport(final byte[] nonce, final int result,
      final String authenticatorName, final byte[] isRooted)
      throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onDeviceHealthReport() : is Rooted :" + result);
    new Thread() {
      @Override
      public void run() {
        fidoCryptoClientActivity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            if (isRooted != null) {
              Log.d(FidoCryptoClientActivity.LOG_TAG,
                "Device Health extension returned=" + Arrays.toString(isRooted));
            }
            fidoCryptoClientActivity.showExtension(isRooted);
            fidoCryptoClientActivity.authenticatorName = authenticatorName;
            fidoCryptoClientActivity.extensionReturned = isRooted;
            if (result == 0) {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                  "Failed to run RTIC ", Toast.LENGTH_SHORT).show();
            } else {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                  "RTIC run successfully ", Toast.LENGTH_SHORT).show();
            }
          }
        });
      }
    }.start();
  }

}

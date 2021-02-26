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

import com.qualcomm.qti.auth.securesampleauthservice.ISecureSampleAuth2Listener;

public class SecureSampleAuth2ListenerImpl extends ISecureSampleAuth2Listener.Stub {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SecureSampleAuth2ListenerImpl(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onUserVerificationResult(final int result,
      final String authenticatorName, final long userId,
      final long userEntityId)
      throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onUserVerificationResult()");
    new Thread() {
      @Override
      public void run() {
        fidoCryptoClientActivity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            fidoCryptoClientActivity.authenticatorName = authenticatorName;
            if (result == 0) {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                  "user verification success ", Toast.LENGTH_SHORT).show();
            } else {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                  "user verification error ", Toast.LENGTH_SHORT).show();
            }
          }
        });
      }
    }.start();
  }

  @Override
  public void onCancel(final byte[] nonce) throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onCancel()");
  }

  @Override
  public void onEnrollmentStatus(final long userId, final boolean status) throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onEnrollmentStatus()");
    new Thread() {
      @Override
      public void run() {
        fidoCryptoClientActivity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            if (status == true) {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                "User is enrolled", Toast.LENGTH_SHORT).show();
            } else {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                "User is not enrolled", Toast.LENGTH_SHORT).show();
            }
          }
        });
      }
    }.start();
  }


}

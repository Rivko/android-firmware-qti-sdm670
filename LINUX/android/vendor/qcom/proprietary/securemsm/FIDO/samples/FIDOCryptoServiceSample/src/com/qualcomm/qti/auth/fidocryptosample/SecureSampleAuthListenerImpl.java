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

import com.qualcomm.qti.auth.securesampleauthservice.ISecureSampleAuthListener;

public class SecureSampleAuthListenerImpl extends ISecureSampleAuthListener.Stub {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SecureSampleAuthListenerImpl(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onUserVerificationResult(final byte[] nonce, final int result,
      final String authenticatorName, final long userId,
      final long userEntityId, final byte[] encapsulatedResult)
      throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onUserVerificationResult()");
    new Thread() {
      @Override
      public void run() {
        fidoCryptoClientActivity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            if(encapsulatedResult != null){
              Log.d(FidoCryptoClientActivity.LOG_TAG,
                "SSA encapsulatedResult=" + Arrays.toString(encapsulatedResult));
              fidoCryptoClientActivity.showResponse(encapsulatedResult);
            }
            fidoCryptoClientActivity.authenticatorName = authenticatorName;
            fidoCryptoClientActivity.encapsulatedResult = encapsulatedResult;
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
    if(fidoCryptoClientActivity.isFingerprint) {
      fidoCryptoClientActivity.cancelSuiButton.callOnClick();
    }
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

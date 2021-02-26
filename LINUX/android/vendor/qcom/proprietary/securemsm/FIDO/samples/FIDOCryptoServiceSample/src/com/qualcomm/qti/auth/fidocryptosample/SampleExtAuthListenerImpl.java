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

import com.qualcomm.qti.auth.sampleextauthservice.ISampleExtAuthListener;

public class SampleExtAuthListenerImpl extends ISampleExtAuthListener.Stub {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SampleExtAuthListenerImpl(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onGetTokenResult(final byte[] nonce, final int result,
      final String authenticatorName, final byte[] encapsulatedResult)
      throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onUserVerificationResult()");
    new Thread() {
      @Override
      public void run() {
        fidoCryptoClientActivity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            if (encapsulatedResult != null) {
              Log.d(FidoCryptoClientActivity.LOG_TAG,
                "SEA extension Returned=" + Arrays.toString(encapsulatedResult));
            }
            fidoCryptoClientActivity.showExtension(encapsulatedResult);
            fidoCryptoClientActivity.authenticatorName = authenticatorName;
            fidoCryptoClientActivity.extensionReturned = encapsulatedResult;
            if (result == 0) {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                  "token obtained ", Toast.LENGTH_SHORT).show();
            } else {
              Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
                  "error in getting token ", Toast.LENGTH_SHORT).show();
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

}

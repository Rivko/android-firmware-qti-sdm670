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

import com.qualcomm.qti.auth.secureextauthservice.ISecureExtAuthListener;

public class SecureExtAuthListenerImpl extends ISecureExtAuthListener.Stub {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SecureExtAuthListenerImpl(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onGetTokenResult(final byte[] nonce, final int result,
      final String authenticatorName, final byte[] encapResult)
      throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG, "onGetTokenResult()");
    new Thread() {
      @Override
      public void run() {
        fidoCryptoClientActivity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            if (encapResult != null) {
              Log.d(FidoCryptoClientActivity.LOG_TAG,
                "SecExt extension Returned=" + Arrays.toString(encapResult));
            }
            fidoCryptoClientActivity.showExtension(encapResult);
            fidoCryptoClientActivity.authenticatorName = authenticatorName;
            fidoCryptoClientActivity.extensionReturned = encapResult;
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

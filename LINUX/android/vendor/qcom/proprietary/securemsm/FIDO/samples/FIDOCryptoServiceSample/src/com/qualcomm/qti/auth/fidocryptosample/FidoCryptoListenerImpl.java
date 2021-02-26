/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import java.io.FileOutputStream;

import android.content.Context;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.format.Time;
import android.util.Base64;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.auth.IFidoCryptoListener;

public class FidoCryptoListenerImpl extends IFidoCryptoListener.Stub {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public FidoCryptoListenerImpl(final FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onResult(final int resultCode, final byte[] uafResponse)
      throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        "onResult(" + FidoCryptoResultCode.values()[resultCode].name() + ")");
    if (resultCode != FidoCryptoResultCode.SUCCESS.ordinal()) {
      fidoCryptoClientActivity.runOnUiThread(new Runnable() {
        @Override
        public void run() {
          Toast.makeText(
              fidoCryptoClientActivity.getBaseContext(),
              "operation failed with error "
                  + FidoCryptoResultCode.values()[resultCode].name(),
              Toast.LENGTH_SHORT).show();
        }
      });
      return;
    }

    if (uafResponse != null) {
      try {
        Time now = new Time(Time.getCurrentTimezone());
        now.setToNow();
        String timeString = now.format("%T: response: ");
        fidoCryptoClientActivity.outputStream.write(timeString.getBytes());
        fidoCryptoClientActivity.outputStream.write(Base64.encodeToString(uafResponse,
            Base64.DEFAULT).getBytes());
      } catch (Exception e) {
        e.printStackTrace();
      }
      fidoCryptoClientActivity.runOnUiThread(new ProcessOnResult(fidoCryptoClientActivity,
        uafResponse));
    }
  }
}

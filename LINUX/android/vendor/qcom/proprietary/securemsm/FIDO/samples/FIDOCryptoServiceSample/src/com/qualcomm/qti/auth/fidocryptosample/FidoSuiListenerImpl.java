/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import com.qualcomm.qti.auth.fidosuiservice.IFidoSuiListener;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class FidoSuiListenerImpl extends IFidoSuiListener.Stub {

  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
      + FidoSuiListenerImpl.class.getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public FidoSuiListenerImpl(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public IBinder asBinder() {
    return this;
  }

  @Override
  public void onResult(final int code)
      throws RemoteException {
    Log.d(
        LOG_TAG,
        "onResult+ code=" + code);
    fidoCryptoClientActivity.showSuiResponse(code);
    Log.d(LOG_TAG, "onResult- response=" + code);
  }
}

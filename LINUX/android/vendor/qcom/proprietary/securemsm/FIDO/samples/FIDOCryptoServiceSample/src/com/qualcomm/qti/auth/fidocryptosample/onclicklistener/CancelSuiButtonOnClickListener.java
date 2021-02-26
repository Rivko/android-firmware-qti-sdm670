/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.onclicklistener;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class CancelSuiButtonOnClickListener implements OnClickListener {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public CancelSuiButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    if (fidoCryptoClientActivity.fidoSuiConnection == null) {
      Toast.makeText(fidoCryptoClientActivity.getApplicationContext(),
          "FIDOSuiService not bound", Toast.LENGTH_LONG).show();
      return;
    }
    if (fidoCryptoClientActivity.fidoSuiController == null) {
      Toast.makeText(fidoCryptoClientActivity.getApplicationContext(),
          "SUI not active", Toast.LENGTH_LONG).show();
      return;
    }
    try {
      fidoCryptoClientActivity.fidoSuiController.abortSui();
    } catch (RemoteException e) {
      Log.e(FidoCryptoClientActivity.LOG_TAG, "fido sui service is unreachable", e);
    }

  }
}

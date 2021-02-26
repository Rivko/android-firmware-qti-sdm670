/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.onclicklistener;

import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class UnBindFidoSuiButtonOnClickListener implements OnClickListener {
  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
      + UnBindFidoSuiButtonOnClickListener.class.getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public UnBindFidoSuiButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(LOG_TAG, "onClick+");
    if (fidoCryptoClientActivity.fidoSuiConnection.isBound()) {
      fidoCryptoClientActivity.unbindFidoSuiService();
      Toast.makeText(v.getContext(), "unbinding fidosui service",
          Toast.LENGTH_SHORT).show();
    } else {
      Toast.makeText(v.getContext(), "fidosui service not bound",
          Toast.LENGTH_SHORT).show();
    }
    Log.d(LOG_TAG, "onClick-");
  }

}

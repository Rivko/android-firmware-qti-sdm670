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
import com.qualcomm.qti.auth.fidocryptosample.ServiceAlreadyBoundException;

public class BindFidoSuiButtonOnClickListener implements OnClickListener {
  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
      + BindFidoSuiButtonOnClickListener.class.getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public BindFidoSuiButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(LOG_TAG, "onClick+");
    try {
      fidoCryptoClientActivity.bindFidoSuiService();
      Toast.makeText(v.getContext(), "binding FIDOSui service",
          Toast.LENGTH_SHORT).show();
    } catch (ServiceAlreadyBoundException e) {
      Toast.makeText(v.getContext(), "service already bound",
          Toast.LENGTH_SHORT).show();
    }
    Log.d(LOG_TAG, "onClick-");
  }

}

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

public class BindFidoCryptoButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public BindFidoCryptoButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        BindFidoCryptoButtonOnClickListener.class.getSimpleName()
            + ".onClick()");
    try {
      fidoCryptoClientActivity.bindFidoCryptoService();
      Toast.makeText(v.getContext(), "binding service ...", Toast.LENGTH_SHORT)
          .show();
    } catch (ServiceAlreadyBoundException e) {
      Toast.makeText(v.getContext(), "service already bound",
          Toast.LENGTH_SHORT).show();
    }
  }

}

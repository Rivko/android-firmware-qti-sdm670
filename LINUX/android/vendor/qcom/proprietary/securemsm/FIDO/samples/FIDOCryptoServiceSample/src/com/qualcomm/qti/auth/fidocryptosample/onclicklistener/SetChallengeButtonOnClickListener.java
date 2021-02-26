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
import com.qualcomm.qti.auth.fidocryptosample.samples.SetChallengeCase;

public class SetChallengeButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SetChallengeButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        SetChallengeButtonOnClickListener.class.getSimpleName()
            + ".onClick()");
    try {
      if (!fidoCryptoClientActivity.fidoCryptoConnection.isBound()) {
        Toast.makeText(v.getContext(), "fidocrypto service not connected",
            Toast.LENGTH_SHORT).show();
        Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
        return;
      }
      fidoCryptoClientActivity.newChallenge();
      new SetChallengeCase(fidoCryptoClientActivity).test(v, (byte)0);
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

}

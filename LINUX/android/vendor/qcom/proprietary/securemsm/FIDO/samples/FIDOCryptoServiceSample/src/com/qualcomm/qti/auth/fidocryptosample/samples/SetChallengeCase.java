/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.samples;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoResultCode;

public class SetChallengeCase extends AbstractSampleCase {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SetChallengeCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v, byte authIndex) throws RemoteException {
    int res = fidoCryptoClientActivity.setChallenge();
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }
}

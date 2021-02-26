/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.samples;

import java.util.Arrays;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoResultCode;

public class GetInfoSampleCase extends AbstractSampleCase {
  private static final byte[] GETINFO_COMMAND = new byte[] {
  /* 0x00 */getByte(TAG_UAFV1_GETINFO_CMD, 0),
  /* 0x01 */getByte(TAG_UAFV1_GETINFO_CMD, 1),
  /* 0x02 */0,
  /* 0x03 */0 };

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public GetInfoSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v) throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        "GETINFO_COMMAND=" + Arrays.toString(GetInfoSampleCase.GETINFO_COMMAND));
    int res = fidoCryptoClientActivity.processEx(
        GetInfoSampleCase.GETINFO_COMMAND, good_cookie, useTLocExParam);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }
}

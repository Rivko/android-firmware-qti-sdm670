/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.samples;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoResultCode;

public class OpenSettingsSampleCase extends AbstractSampleCase {
  private static final byte[] OPEN_SETTINGS = new byte[] {
  /* 0x00 */getByte(TAG_UAFV1_OPEN_SETTINGS_CMD, 0),
  /* 0x01 */getByte(TAG_UAFV1_OPEN_SETTINGS_CMD, 1),
  /* 0x02 */0x05,
  /* 0x03 */0x00,
  /* 0x04 */getByte(TAG_AUTHENTICATOR_INDEX, 0),
  /* 0x05 */getByte(TAG_AUTHENTICATOR_INDEX, 1),
  /* 0x06 */0x01,
  /* 0x07 */0x00,
  /* 0x08 */0x08 };

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public OpenSettingsSampleCase(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v) throws RemoteException {
    int res = fidoCryptoClientActivity.processEx(OPEN_SETTINGS,
                good_cookie, useTLocExParam);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }
}

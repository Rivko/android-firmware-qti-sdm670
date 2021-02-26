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

public class DeregisterSampleCase extends AbstractSampleCase {

  private static final byte[] DEREGISTER_COMMAND_NOAPPID = new byte[] {
  /* 0x00 */getByte(TAG_UAFV1_DEREGISTER_CMD, 0),
  /* 0x01 */getByte(TAG_UAFV1_DEREGISTER_CMD, 1),
  /* 0x02 */0x12,
  /* 0x03 */0x00,
  /* 0x04 */getByte(TAG_AUTHENTICATOR_INDEX, 0),
  /* 0x05 */getByte(TAG_AUTHENTICATOR_INDEX, 1),
  /* 0x06 */0x01,
  /* 0x07 */0x00,
  /* 0x08 */0x08,
  /* 0x09 */getByte(TAG_KEYID, 0),
  /* 0x0A */getByte(TAG_KEYID, 1),
  /* 0x0B */0x02,
  /* 0x0C */0x00,
  /* 0x0D */0x15,
  /* 0x0E */0x16,
  /* 0x0F */getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 0),
  /* 0x10 */getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 1),
  /* 0x11 */0x03,
  /* 0x12 */0x00,
  /* 0x13 */0x1B,
  /* 0x14 */0x1C,
  /* 0x15 */0x1D };

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public DeregisterSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v) throws RemoteException {
    int res = fidoCryptoClientActivity.processEx(DEREGISTER_COMMAND_NOAPPID,
        good_cookie, useTLocExParam);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }
}

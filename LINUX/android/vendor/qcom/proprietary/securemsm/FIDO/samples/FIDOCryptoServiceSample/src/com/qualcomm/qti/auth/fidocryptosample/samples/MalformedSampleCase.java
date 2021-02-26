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

public class MalformedSampleCase extends AbstractSampleCase {
  private final static byte[] MALFORMED_REQUEST = new byte[] {
  /* 0x00 */getByte(TAG_UAFV1_GETINFO_CMD, 0),
  /* 0x01 */getByte(TAG_UAFV1_DEREGISTER_CMD_RESPONSE, 1),
  /* 0x02 */0 };

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public MalformedSampleCase(FidoCryptoClientActivity iFidoCryptoClient) {
    this.fidoCryptoClientActivity = iFidoCryptoClient;
  }

  public void test(View v) throws RemoteException {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        "MALFORMED_REQUEST=" + Arrays.toString(MALFORMED_REQUEST));
    int res = fidoCryptoClientActivity.processEx(MALFORMED_REQUEST,
                good_cookie, useTLocExParam);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }
}

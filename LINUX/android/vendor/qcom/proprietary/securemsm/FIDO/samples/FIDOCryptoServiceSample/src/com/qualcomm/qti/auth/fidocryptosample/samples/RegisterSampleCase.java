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

public class RegisterSampleCase extends AbstractSampleCase {
  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
      + RegisterSampleCase.class.getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public RegisterSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v, byte authIndex) throws RemoteException {
    Log.d(LOG_TAG, "test+");

    byte[] appId = APP_ID;

    byte[] registerCommand = getRegisterCommand(authIndex, appId,
        fidoCryptoClientActivity.encapsulatedResult);
    int res = fidoCryptoClientActivity.processEx(registerCommand, good_cookie,
        useTLocExParam);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(RegisterSampleCase.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
    Log.d(LOG_TAG, "test-");
  }

  private byte[] getRegisterCommand(byte authenticatorIndex, byte[] appID,
      byte[] userVerificationToken) {
    int registerCommandSize = 0;
    if (userVerificationToken != null) {
      registerCommandSize = (short) (0x0D + appID.length + 0x1D + userVerificationToken.length);
    } else {
      registerCommandSize = (short) (0x0D + appID.length + 0x19);
    }
    ByteBuffer byteBuffer = ByteBuffer.allocate(registerCommandSize);
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    byteBuffer.putShort(TAG_UAFV1_REGISTER_CMD);
    byteBuffer.putShort((short) (registerCommandSize - 4));
    writeByte(byteBuffer, TAG_AUTHENTICATOR_INDEX, authenticatorIndex);
    writeByteArray(byteBuffer, TAG_APPID, appID);
    writeByteArray(byteBuffer, TAG_FINAL_CHALLENGE, FidoCryptoClientActivity.finalChallenge);
    writeByteArray(byteBuffer, TAG_USERNAME, new byte[] { 0x1C });
    writeShort(byteBuffer, TAG_ATTESTATION_TYPE, (short) 0x3E07);
    writeByteArray(byteBuffer, TAG_KEYHANDLE_ACCESS_TOKEN, new byte[] { 0x27,
        0x28, 0x29 });
    writeByteArray(byteBuffer, TAG_USERVERIFY_TOKEN, userVerificationToken);
    return byteBuffer.array();
  }

}

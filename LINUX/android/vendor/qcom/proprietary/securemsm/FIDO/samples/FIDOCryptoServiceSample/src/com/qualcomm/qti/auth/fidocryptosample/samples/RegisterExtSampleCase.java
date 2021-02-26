/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.samples;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoResultCode;

public class RegisterExtSampleCase extends AbstractSampleCase {
  private static final String LOG_TAG = RegisterExtSampleCase.class
      .getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public RegisterExtSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v, byte authIndex) throws RemoteException {

    if (fidoCryptoClientActivity.extensionReturned == null) {
      Toast.makeText(v.getContext(), "Get extension UVT first",
          Toast.LENGTH_SHORT).show();
      return;
    }
    byte[] appId = APP_ID;
    byte[] extensionId = "EXT1".getBytes();
    byte[] registerCommand = getRegisterCommand(authIndex,
        appId,
        fidoCryptoClientActivity.encapsulatedResult,
        fidoCryptoClientActivity.extensionReturned, extensionId);
    int res = fidoCryptoClientActivity.processEx(registerCommand,
                good_cookie, useTLocExParam);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(RegisterExtSampleCase.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }

  private static byte[] getRegisterCommand(byte authenticatorIndex,
      byte[] appID, byte[] userVerificationToken, byte[] extensionData,
      byte[] extensionId) {
    short registerCommandSize = 0;
    if (userVerificationToken != null) {
      registerCommandSize = (short) (0x0D + appID.length + 0x1D + userVerificationToken.length + 0x09 + extensionId.length + 0x04 + extensionData.length);
    } else {
      registerCommandSize = (short) (0x0D + appID.length + 0x19 + 0x09 + extensionId.length + 0x04 + extensionData.length);
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
    byteBuffer.putShort(TAG_EXTENSION_CRITICAL);
    byteBuffer
        .putShort((short) (8 + extensionData.length + extensionId.length));
    writeByteArray(byteBuffer, TAG_EXTENSION_ID, extensionId);
    writeByteArray(byteBuffer, TAG_EXTENSION_DATA, extensionData);
    return byteBuffer.array();
  }
}

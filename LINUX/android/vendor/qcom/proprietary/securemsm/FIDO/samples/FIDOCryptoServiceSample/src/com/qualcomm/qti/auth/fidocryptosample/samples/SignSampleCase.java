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
import com.qualcomm.qti.auth.fidocryptosample.persistence.UnableToReadKeyHandleException;

public class SignSampleCase extends AbstractSampleCase {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SignSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  private byte[] getSignCommand(byte[] keyHandle, byte authIndex, byte[] appId) {
    byte[] signCmdTag = new byte[] {
      getByte(TAG_UAFV1_SIGN_CMD, 0),
      getByte(TAG_UAFV1_SIGN_CMD, 1),
      0x00,
      0x00
    };
    byte[] authTag = new byte[] {
      getByte(TAG_AUTHENTICATOR_INDEX, 0),
      getByte(TAG_AUTHENTICATOR_INDEX, 1),
      0x01,
      0x00,
      authIndex
    };
    byte[] finalChallenge = new byte[] {
      getByte(TAG_FINAL_CHALLENGE, 0),
      getByte(TAG_FINAL_CHALLENGE, 1),
      0x03,
      0x00,
      FidoCryptoClientActivity.finalChallenge[0],
      FidoCryptoClientActivity.finalChallenge[1],
      FidoCryptoClientActivity.finalChallenge[2]
    };
    byte[] keyHandleAccessToken = new byte[] {
      getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 0),
      getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 1),
      0x03,
      0x00,
      0x27,
      0x28,
      0x29
    };
    byte[] sampleTXTags = new byte[] {
      getByte(TAG_TRANSACTION_CONTENT, 0),
      getByte(TAG_TRANSACTION_CONTENT, 1),
      getByte(fidoCryptoClientActivity.transactionContent.length,0),
      getByte(fidoCryptoClientActivity.transactionContent.length,1)
    };

    int bufferSize = signCmdTag.length + authTag.length + finalChallenge.length + keyHandleAccessToken.length
            + (appId.length + 4) // + HEADERS
            + (keyHandle.length + 4); // + HEADERS
    if(fidoCryptoClientActivity.fidoOpSpinner.getSelectedItemPosition() == FidoCryptoClientActivity.SIGN_TX_ITEM_POSITION) {
      bufferSize += sampleTXTags.length;
      bufferSize += fidoCryptoClientActivity.transactionContent.length;
    }
    if (fidoCryptoClientActivity.encapsulatedResult != null) {
      bufferSize += fidoCryptoClientActivity.encapsulatedResult.length + 4;
    }

    ByteBuffer byteBuffer = ByteBuffer.allocate(bufferSize); // + 15 if TRANSACTION_CONFIRMATION
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);

    byteBuffer.put(signCmdTag);
    byteBuffer.put(authTag);
    byteBuffer.putShort(TAG_APPID);
    byteBuffer.putShort((short) appId.length);
    byteBuffer.put(appId);
    byteBuffer.put(finalChallenge);

    if(fidoCryptoClientActivity.fidoOpSpinner.getSelectedItemPosition() == FidoCryptoClientActivity.SIGN_TX_ITEM_POSITION) {
      byteBuffer.put(sampleTXTags);
      byteBuffer.put(fidoCryptoClientActivity.transactionContent);
    }

    byteBuffer.put(keyHandleAccessToken);


    if (fidoCryptoClientActivity.encapsulatedResult != null) {
      byteBuffer.putShort(TAG_USERVERIFY_TOKEN);
      byteBuffer.putShort((short) (fidoCryptoClientActivity.encapsulatedResult.length));
      byteBuffer.put(fidoCryptoClientActivity.encapsulatedResult);
    }

    byteBuffer.putShort(TAG_KEYHANDLE);
    byteBuffer.putShort((short) keyHandle.length);
    byteBuffer.put(keyHandle);

    byte[] signCommand = byteBuffer.array();

    signCommand[2] = getByte(signCommand.length - 4, 0);
    signCommand[3] = getByte(signCommand.length - 4, 1);
    return signCommand;

  }

  public void test(View v, byte authIndex) throws RemoteException {
    try {
      byte[] keyHandle = fidoCryptoClientActivity.persistenceManager
          .readKeyHandleFromFile();
      byte[] appId = APP_ID;
      byte[] signCommand = getSignCommand(keyHandle, authIndex, appId);
      int res = fidoCryptoClientActivity.processEx(signCommand, good_cookie,
                  useTLocExParam);
      String resName = FidoCryptoResultCode.values()[res].name();
      Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
      Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
    } catch (UnableToReadKeyHandleException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(), "Unable to read key handle",
          Toast.LENGTH_SHORT).show();
    }

  }
}

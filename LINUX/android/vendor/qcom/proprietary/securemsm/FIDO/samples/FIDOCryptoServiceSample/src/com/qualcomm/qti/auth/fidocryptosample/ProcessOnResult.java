/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.persistence.UnableToWriteKeyHandleException;
import com.qualcomm.qti.auth.fidocryptosample.samples.AbstractSampleCase;

public class ProcessOnResult implements Runnable {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  private final byte[] uafResponse;

  public ProcessOnResult(
      final FidoCryptoClientActivity fidoCryptoClientActivity,
      final byte[] uafResponse) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
    this.uafResponse = uafResponse;
  }

  @Override
  public void run() {
    StringBuilder stringBuilder = new StringBuilder();
    Log.d(FidoCryptoClientActivity.LOG_TAG, "uafResponse=[");
    for (int i = 0; i < uafResponse.length; i++) {
      if (i % 30 == 0) {
        Log.d(FidoCryptoClientActivity.LOG_TAG, "" + stringBuilder.toString());
        stringBuilder = new StringBuilder();
      }
      stringBuilder.append(String.format("0x%02X ", uafResponse[i]));
    }
    Log.d(FidoCryptoClientActivity.LOG_TAG, stringBuilder.toString());
    Log.d(FidoCryptoClientActivity.LOG_TAG, "]");

    try {
      PartiallyParsedResponse response = PartialResponseParser
          .parse(uafResponse);
      if (response.isARegisterResponse()) {
        processRegisterResponse(response);
      } else if (response.isADeregisterResponse()) {
        processDeregisterResponse();
      } else if (response.isAGetInfoResponse()) {
        processGetInfoResponse(response);
      } else if (response.isASignResponse()) {
        processSignResponse(response);
      } else if (response.isAOpenSettingsResponse()) {
        processOpenSettingsResponse();
      } else {
        processUnrecognizedResponse();
      }
    } catch (MalformedResponseException e) {
      final String message = "malformed response error";
      Log.d(FidoCryptoClientActivity.LOG_TAG, Log.getStackTraceString(e));
      Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
          Toast.LENGTH_SHORT).show();
    }
  }

  private void processUnrecognizedResponse() {
    final String message = "internal error: unrecognized response";
    Log.d(FidoCryptoClientActivity.LOG_TAG, message);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
        Toast.LENGTH_SHORT).show();
  }

  private void processOpenSettingsResponse() {
    final String message = "opensettings not supported";
    Log.d(FidoCryptoClientActivity.LOG_TAG, message);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
        Toast.LENGTH_SHORT).show();
  }

  private void processSignResponse(PartiallyParsedResponse signResponse) {
    String message;
    if (signResponse.getStatusCode() == AbstractSampleCase.UAF_CMD_STATUS_OK) {
      message = "sign ok";
    } else {
      message = "sign failed";
    }
    Log.d(FidoCryptoClientActivity.LOG_TAG, message);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
        Toast.LENGTH_SHORT).show();
  }

  private void processGetInfoResponse(PartiallyParsedResponse getInfoResponse) {
    String message;
    if (getInfoResponse.getStatusCode() == AbstractSampleCase.UAF_CMD_STATUS_OK) {
      message = "getinfo ok";
    } else {
      message = "getinfo failed";
    }
    Log.d(FidoCryptoClientActivity.LOG_TAG, message);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
        Toast.LENGTH_SHORT).show();
  }

  private void processDeregisterResponse() {
    final String message = "deregister not supported";
    Log.d(FidoCryptoClientActivity.LOG_TAG, message);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
        Toast.LENGTH_SHORT).show();
  }

  private void processRegisterResponse(PartiallyParsedResponse registerResponse) {
    if (registerResponse.getStatusCode() != AbstractSampleCase.UAF_CMD_STATUS_OK) {
      String message = "register failed";
      Log.d(FidoCryptoClientActivity.LOG_TAG, message);
      Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
          Toast.LENGTH_SHORT).show();
      return;
    }
    try {
      fidoCryptoClientActivity.persistenceManager
          .writeKeyHandleToFile(registerResponse.getKeyHandle());
      String message = "register ok";
      Log.d(FidoCryptoClientActivity.LOG_TAG, message);
      Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
          Toast.LENGTH_SHORT).show();
    } catch (UnableToWriteKeyHandleException e) {
      e.printStackTrace();
      String message = "register failed: key handle not valid";
      Log.d(FidoCryptoClientActivity.LOG_TAG, message);
      Toast.makeText(fidoCryptoClientActivity.getBaseContext(), message,
          Toast.LENGTH_SHORT).show();
    }
  }

  private static class PartialResponseParser {
    private static final int MIN_LENGTH = 10;
    private static final int RESPONSE_TAG_INDEX = 0;
    private static final int STATUS_CODE_INDEX = 8;
    private static final int KEY_HANDLE_LENGTH = 253;

    public static PartiallyParsedResponse parse(byte[] responseTLV)
        throws MalformedResponseException {
      if (null == responseTLV || responseTLV.length < MIN_LENGTH) {
        throw new MalformedResponseException();
      }
      ByteBuffer responseBuffer = ByteBuffer.wrap(responseTLV);
      responseBuffer.order(ByteOrder.LITTLE_ENDIAN);

      /* 0 */short responseTag = responseBuffer.getShort(RESPONSE_TAG_INDEX);
      /* 1 */
      /* 2 */// length
      /* 3 */
      /* 4 */// status code tag
      /* 5 */
      /* 6 */// length
      /* 7 */
      /* 8 */short statusCode = responseBuffer.getShort(STATUS_CODE_INDEX);
      /* 9 */

      if (statusCode == 0
          && AbstractSampleCase.TAG_UAFV1_REGISTER_CMD_RESPONSE == responseTag) {
        if (responseTLV.length < KEY_HANDLE_LENGTH) {
          throw new MalformedResponseException();
        }
        byte[] keyHandle = new byte[KEY_HANDLE_LENGTH];
        System.arraycopy(responseTLV, responseTLV.length - KEY_HANDLE_LENGTH,
            keyHandle, 0, KEY_HANDLE_LENGTH);
        return new PartiallyParsedResponse(responseTag, statusCode, keyHandle);
      } else {
        return new PartiallyParsedResponse(responseTag, statusCode);
      }
    }
  }

  private static class PartiallyParsedResponse {

    private final short responseCode;
    private final short statusCode;
    private byte[] keyHandle;

    public PartiallyParsedResponse(short responseCode, short statusCode,
        byte[] keyHandle) throws MalformedResponseException {
      this(responseCode, statusCode);
      if (null == keyHandle) {
        throw new MalformedResponseException();
      }
      this.keyHandle = keyHandle;
    }

    public PartiallyParsedResponse(short responseCode, short statusCode) {
      this.responseCode = responseCode;
      this.statusCode = statusCode;
    }

    public boolean isARegisterResponse() {
      return AbstractSampleCase.TAG_UAFV1_REGISTER_CMD_RESPONSE == responseCode;
    }

    public boolean isADeregisterResponse() {
      return AbstractSampleCase.TAG_UAFV1_DEREGISTER_CMD_RESPONSE == responseCode;
    }

    public boolean isAGetInfoResponse() {
      return AbstractSampleCase.TAG_UAFV1_GETINFO_CMD_RESPONSE == responseCode;
    }

    public boolean isASignResponse() {
      return AbstractSampleCase.TAG_UAFV1_SIGN_CMD_RESPONSE == responseCode;
    }

    public boolean isAOpenSettingsResponse() {
      return AbstractSampleCase.TAG_UAFV1_OPEN_SETTINGS_CMD_RESPONSE == responseCode;
    }

    public byte[] getKeyHandle() {
      return keyHandle;
    }

    public short getStatusCode() {
      return statusCode;
    }
  }

  private static class MalformedResponseException extends Throwable {

  }
}

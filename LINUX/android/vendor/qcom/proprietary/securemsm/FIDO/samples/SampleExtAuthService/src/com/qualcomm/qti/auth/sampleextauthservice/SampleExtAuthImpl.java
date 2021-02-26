/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.sampleextauthservice;

import android.os.Binder;
import android.util.Log;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.security.MessageDigest;

import java.util.Arrays;

public class SampleExtAuthImpl extends ISampleExtAuth.Stub {

  private static final String LOG_TAG = "SampleExtAuthImpl";

  static {
    System.loadLibrary("SampleExtAuthJNI");
  }

  private static native GetTokenResponse nativeGetToken(byte[] nonce,
      String secAppName, byte[] authId);

  private static native byte[] nativeCancel(byte[] nonce);

  @Override
  public int getToken(
   byte[] nonce, final String secureAppName, ISampleExtAuthListener iSampleExtAuthListener) {

    try{
      byte[] randAuthNum = SampleExtAuthService.getAppRandomAuth();
      if(randAuthNum == null) {
        return 1;
      }
      int uid = Binder.getCallingUid();
      MessageDigest digest = MessageDigest.getInstance("SHA-256");
      if(digest == null) {
        return 1;
      }
      digest.update(randAuthNum);
      digest.update(BigInteger.valueOf(uid).toByteArray());
      byte[] authId = digest.digest();

      GetTokenResponse getTokenRsp = nativeGetToken(nonce, secureAppName, authId);

      if(getTokenRsp != null){
        iSampleExtAuthListener.onGetTokenResult(nonce,getTokenRsp.result, getTokenRsp.authenticatorName,
              getTokenRsp.encapsulatedResult);
      } else {
        return 1;
      }
    } catch(Exception e){
      return 1;
    }
    return 0;
  }

  @Override
  public int cancel(byte[] nonce, ISampleExtAuthListener iSampleExtAuthListener){
    try{
      nativeCancel(nonce);
      iSampleExtAuthListener.onCancel(nonce);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }
}


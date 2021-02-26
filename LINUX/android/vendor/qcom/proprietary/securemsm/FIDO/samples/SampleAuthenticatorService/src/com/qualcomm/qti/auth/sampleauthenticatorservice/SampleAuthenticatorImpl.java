/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.sampleauthenticatorservice;

import android.os.Binder;
import android.util.Log;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.security.MessageDigest;

public class SampleAuthenticatorImpl extends ISampleAuthenticator.Stub {

  private static final String LOG_TAG = "SampleAuthenticatorImpl";

  static {
    System.loadLibrary("SampleAuthJNI");
  }

  private static native VerifyUserResponse nativeVerifyUser(byte[] nonce,
      String secAppName, byte[] authId);

  private static native byte[] nativeCancel(byte[] nonce);

  private static native boolean nativeGetEnrollmentStatus(long userId);

  @Override
  public int verifyUser(
   byte[] nonce, final String secureAppName, ISampleAuthenticatorListener iSampleAuthenticatorListener) {

    try{
      byte[] randAuthNum = SampleAuthenticatorService.getAppRandomAuth();
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

      VerifyUserResponse verifyUserRsp = nativeVerifyUser(nonce, secureAppName, authId);

      if(verifyUserRsp != null){
        iSampleAuthenticatorListener.onUserVerificationResult(nonce,verifyUserRsp.result, verifyUserRsp.authenticatorName,
              verifyUserRsp.userId, verifyUserRsp.userEntityId, verifyUserRsp.encapsulatedResult);
      } else {
        return 1;
      }
    } catch(Exception e){
      return 1;
    }
    return 0;
  }

  @Override
  public int cancel(byte[] nonce, ISampleAuthenticatorListener iSampleAuthenticatorListener){
    try{
      nativeCancel(nonce);
      iSampleAuthenticatorListener.onCancel(nonce);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }

  @Override
  public int getEnrollmentStatus(long userId, ISampleAuthenticatorListener iSampleAuthenticatorListener) {
    boolean status = false;
    try {
      status = nativeGetEnrollmentStatus(userId);
      iSampleAuthenticatorListener.onEnrollmentStatus(userId, status);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }
}


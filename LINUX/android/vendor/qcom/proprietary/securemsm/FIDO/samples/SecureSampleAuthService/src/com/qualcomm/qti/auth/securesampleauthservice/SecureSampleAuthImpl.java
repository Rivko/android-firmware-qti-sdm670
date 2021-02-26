/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

public class SecureSampleAuthImpl extends ISecureSampleAuth.Stub {

  static {
    System.loadLibrary("SecureSampleAuthJNI");
  }

  static native VerifyUserResponse nativeVerifyUser(byte[] nonce,
      String secAppName, boolean useBiolib);

  static native byte[] nativeCancel(byte[] nonce);

  static native boolean nativeGetEnrollmentStatus(long userId);

  @Override
  public int verifyUser(
   byte[] nonce, final String secureAppName, ISecureSampleAuthListener iSecureSampleAuthListener) {

    try{
      // Set the last parameter to true to use the biolib API, to false to use IAuth
      VerifyUserResponse verifyUserRsp = nativeVerifyUser(nonce, secureAppName, false);

      if(verifyUserRsp != null) {
        iSecureSampleAuthListener.onUserVerificationResult(nonce,verifyUserRsp.result, verifyUserRsp.authenticatorName,
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
  public int cancel(byte[] nonce, ISecureSampleAuthListener iSecureSampleAuthListener){
    try{
      nativeCancel(nonce);
      iSecureSampleAuthListener.onCancel(nonce);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }

  @Override
  public int getEnrollmentStatus(long userId, ISecureSampleAuthListener iSecureSampleAuthListener) {
    boolean status = false;
    try {
      status = nativeGetEnrollmentStatus(userId);
      iSecureSampleAuthListener.onEnrollmentStatus(userId, status);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }
}


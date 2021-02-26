/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

public class SecureSampleAuth2Impl extends ISecureSampleAuth2.Stub {

  static {
    System.loadLibrary("SecureSampleAuthJNI");
  }

  @Override
  public int verifyUser(ISecureSampleAuth2Listener iSecureSampleAuth2Listener) {

    try{
      // Set the last parameter to true to use the biolib API, to false to use IAuth
      VerifyUserResponse verifyUserRsp = SecureSampleAuthImpl.nativeVerifyUser(null, null, true);

      if(verifyUserRsp != null) {
        iSecureSampleAuth2Listener.onUserVerificationResult(verifyUserRsp.result, verifyUserRsp.authenticatorName,
              verifyUserRsp.userId, verifyUserRsp.userEntityId);
      } else {
        return 1;
      }
    } catch(Exception e){
      return 1;
    }
    return 0;
  }

  @Override
  public int cancel(byte[] nonce, ISecureSampleAuth2Listener iSecureSampleAuth2Listener){
    try{
      SecureSampleAuthImpl.nativeCancel(nonce);
      iSecureSampleAuth2Listener.onCancel(nonce);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }

  @Override
  public int getEnrollmentStatus(long userId, ISecureSampleAuth2Listener iSecureSampleAuth2Listener) {
    boolean status = false;
    try {
      status = SecureSampleAuthImpl.nativeGetEnrollmentStatus(userId);
      iSecureSampleAuth2Listener.onEnrollmentStatus(userId, status);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }
}


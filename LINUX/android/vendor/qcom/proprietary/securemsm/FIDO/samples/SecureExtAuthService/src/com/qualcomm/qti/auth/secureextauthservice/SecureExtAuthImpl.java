/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.secureextauthservice;

public class SecureExtAuthImpl extends ISecureExtAuth.Stub {

  static {
    System.loadLibrary("SecureExtAuthJNI");
  }

  private static native GetTokenResponse nativeGetToken(byte[] nonce,
      String secAppName);

  private static native byte[] nativeCancel(byte[] nonce);

  @Override
  public int getToken(
   byte[] nonce, final String secureAppName, ISecureExtAuthListener iSecureExtAuthListener) {

    try{
      GetTokenResponse getTokenRsp = nativeGetToken(nonce, secureAppName);

      if(getTokenRsp != null) {
        iSecureExtAuthListener.onGetTokenResult(nonce,getTokenRsp.result, getTokenRsp.authenticatorName,
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
  public int cancel(byte[] nonce, ISecureExtAuthListener iSecureExtAuthListener){
    try{
      nativeCancel(nonce);
      iSecureExtAuthListener.onCancel(nonce);
    } catch(Exception e){
      return -1;
    }
    return 0;
  }

}


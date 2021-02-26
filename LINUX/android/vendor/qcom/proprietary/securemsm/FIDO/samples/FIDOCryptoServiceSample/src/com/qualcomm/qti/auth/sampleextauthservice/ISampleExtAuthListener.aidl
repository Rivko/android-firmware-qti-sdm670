/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.sampleextauthservice;

oneway interface ISampleExtAuthListener {
  void onGetTokenResult(in byte[] nonce, int result, String authenticatorName, in byte[] encapsulatedResult);
  void onCancel(in byte[] nonce);
}

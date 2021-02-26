/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.mockauthservice;

oneway interface IMockAuthListener {
  void onUserVerificationResult(int result, String authenticatorName, long userId, long userEntityId);
  void onCancel(in byte[] nonce);
}

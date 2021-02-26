/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

oneway interface ISecureSampleAuthListener {
  void onUserVerificationResult(in byte[] nonce, int result, String authenticatorName, long userId, long userEntityId, in byte[] encapsulatedResult);
  void onCancel(in byte[] nonce);
  void onEnrollmentStatus(long userId, boolean status);
}

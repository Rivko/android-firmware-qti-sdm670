/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.sampleauthenticatorservice;

public class VerifyUserResponse {
  public int result;
  public String authenticatorName;
  public long userId;
  public long userEntityId;
  public byte[] encapsulatedResult;
}

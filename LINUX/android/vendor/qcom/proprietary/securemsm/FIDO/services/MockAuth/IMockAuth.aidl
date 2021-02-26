/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.secureservice.mockauthservice;

import com.qualcomm.qti.secureservice.mockauthservice.IMockAuthListener;

oneway interface IMockAuth {
  void giveAuthToken(int versionMajor, int versionMinor, int result, long userId, long uptime, int extDataLength, in byte[] extData, long userEntityId);
  void verifyUser(in byte[] nonce, in String secAppName, in IMockAuthListener iMockAuthListener);
  void cancel(in byte[] nonce, in IMockAuthListener iMockAuthListener);
}

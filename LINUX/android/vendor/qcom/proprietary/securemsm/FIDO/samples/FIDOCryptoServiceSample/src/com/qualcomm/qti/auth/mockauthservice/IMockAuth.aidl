/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.mockauthservice;

import com.qualcomm.qti.auth.mockauthservice.IMockAuthListener;

interface IMockAuth {
  int giveAuthToken(int versionMajor, int versionMinor, int result, long userId, long uptime, in byte[] extData, long userEntityId);
  int verifyUser(in IMockAuthListener iMockAuthListener);
  int cancel(in byte[] nonce, in IMockAuthListener iMockAuthListener);
}

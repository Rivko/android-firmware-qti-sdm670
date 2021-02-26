/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth;

import com.qualcomm.qti.auth.IFidoCryptoListener;

import java.util.Map;

interface IFidoCrypto {
  int process(in byte[] uafRequest, in String appId, in byte[] cookie, in IFidoCryptoListener iFidoCryptoListener);
  int processEx(in byte[] uafRequest, in String appId, in byte[] cookie, in IFidoCryptoListener iFidoCryptoListener, in Map exParams);
  int setChallenge(in byte[] finalChallenge, in IFidoCryptoListener iFidoCryptoListener);
}

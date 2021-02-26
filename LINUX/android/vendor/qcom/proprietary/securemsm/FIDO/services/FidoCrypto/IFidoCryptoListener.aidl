/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth;

oneway interface IFidoCryptoListener {
  void onResult(in int resultCode, in byte[] uafResponse);
}

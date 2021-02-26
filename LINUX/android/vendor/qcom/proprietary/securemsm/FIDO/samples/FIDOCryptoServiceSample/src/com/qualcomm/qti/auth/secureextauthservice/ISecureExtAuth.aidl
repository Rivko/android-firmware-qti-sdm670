/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.secureextauthservice;

import com.qualcomm.qti.auth.secureextauthservice.ISecureExtAuthListener;

interface ISecureExtAuth {
  int getToken(in byte[] nonce, in String secAppName, in ISecureExtAuthListener iSampleAuthListener);
  int cancel(in byte[] nonce, in ISecureExtAuthListener iSecureExtAuthListener);
}

/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.sampleextauthservice;

import com.qualcomm.qti.auth.sampleextauthservice.ISampleExtAuthListener;

interface ISampleExtAuth {
  int getToken(in byte[] nonce, in String secAppName, in ISampleExtAuthListener iSampleExtAuthListener);
  int cancel(in byte[] nonce, in ISampleExtAuthListener iSampleExtAuthListener);
}

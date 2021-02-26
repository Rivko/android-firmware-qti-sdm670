/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

import com.qualcomm.qti.auth.securesampleauthservice.ISecureSampleAuth2Listener;

interface ISecureSampleAuth2 {
  int verifyUser(in ISecureSampleAuth2Listener iSampleAuth2Listener);
  int cancel(in byte[] nonce, in ISecureSampleAuth2Listener iSecureSampleAuth2Listener);
  int getEnrollmentStatus(long userId, in ISecureSampleAuth2Listener iSecureSampleAuth2Listener);
}

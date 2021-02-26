/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class SecureSampleAuth2Service extends Service {
  private ISecureSampleAuth2.Stub mSecureSampleAuthBinder;

  public SecureSampleAuth2Service() {
    this.mSecureSampleAuthBinder = new SecureSampleAuth2Impl();
  }

  @Override
  public IBinder onBind(Intent intent) {
    return this.mSecureSampleAuthBinder;
  }

  @Override
  public void onDestroy() {
  }

}

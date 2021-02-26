/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.securesampleauthservice;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class SecureSampleAuthService extends Service {
  private ISecureSampleAuth.Stub mSecureSampleAuthBinder;

  public SecureSampleAuthService() {
    this.mSecureSampleAuthBinder = new SecureSampleAuthImpl();
  }

  @Override
  public IBinder onBind(Intent intent) {
    return this.mSecureSampleAuthBinder;
  }

  @Override
  public void onDestroy() {
  }

}

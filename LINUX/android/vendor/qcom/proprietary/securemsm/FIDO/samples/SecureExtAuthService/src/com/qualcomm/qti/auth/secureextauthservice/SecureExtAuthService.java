/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.auth.secureextauthservice;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class SecureExtAuthService extends Service {
  private ISecureExtAuth.Stub mSecureExtAuthBinder;

  public SecureExtAuthService() {
    this.mSecureExtAuthBinder = new SecureExtAuthImpl();
  }

  @Override
  public IBinder onBind(Intent intent) {
    return this.mSecureExtAuthBinder;
  }

  @Override
  public void onDestroy() {
  }

}

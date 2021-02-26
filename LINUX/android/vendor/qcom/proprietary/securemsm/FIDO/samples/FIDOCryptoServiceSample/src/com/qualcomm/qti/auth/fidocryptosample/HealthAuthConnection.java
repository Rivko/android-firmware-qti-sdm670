/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.auth.healthauthservice.IHealthAuth;

public class HealthAuthConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public IHealthAuth iHealthAuthServiceConnection;

  public HealthAuthConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iHealthAuthServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, HealthAuthConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iHealthAuthServiceConnection = IHealthAuth.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "HealthAuthService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, HealthAuthConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iHealthAuthServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "HealthAuthService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iHealthAuthServiceConnection = null;
  }

}

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

import com.qualcomm.qti.auth.mockauthservice.IMockAuth;

public class MaConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public IMockAuth iMockAuthServiceConnection;

  public MaConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iMockAuthServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, MaConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iMockAuthServiceConnection = IMockAuth.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "MockAuthService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, MaConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iMockAuthServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "IMockAuthService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iMockAuthServiceConnection = null;
  }

}

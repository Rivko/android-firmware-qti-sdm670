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

import com.qualcomm.qti.auth.securesampleauthservice.ISecureSampleAuth;

public class SSAConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public ISecureSampleAuth iSecureSampleAuthServiceConnection;

  public SSAConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iSecureSampleAuthServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SSAConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iSecureSampleAuthServiceConnection = ISecureSampleAuth.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SecureSampleAuthService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SSAConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iSecureSampleAuthServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SecureSampleAuthService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iSecureSampleAuthServiceConnection = null;
  }

}

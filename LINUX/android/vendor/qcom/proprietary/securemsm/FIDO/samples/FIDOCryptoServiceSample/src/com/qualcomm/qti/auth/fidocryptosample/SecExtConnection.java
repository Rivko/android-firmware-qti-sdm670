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

import com.qualcomm.qti.auth.secureextauthservice.ISecureExtAuth;

public class SecExtConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public ISecureExtAuth iSecureExtAuthServiceConnection;

  public SecExtConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iSecureExtAuthServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SecExtConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iSecureExtAuthServiceConnection = ISecureExtAuth.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SecureExtAuthService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SecExtConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iSecureExtAuthServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SecureExtAuthService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iSecureExtAuthServiceConnection = null;
  }

}

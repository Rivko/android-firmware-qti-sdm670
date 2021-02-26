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

import com.qualcomm.qti.auth.securesampleauthservice.ISecureSampleAuth2;

public class SSA2Connection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public ISecureSampleAuth2 iSecureSampleAuth2ServiceConnection;

  public SSA2Connection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iSecureSampleAuth2ServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SSA2Connection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iSecureSampleAuth2ServiceConnection = ISecureSampleAuth2.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SecureSampleAuth2Service connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SSA2Connection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iSecureSampleAuth2ServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SecureSampleAuth2Service disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iSecureSampleAuth2ServiceConnection = null;
  }

}

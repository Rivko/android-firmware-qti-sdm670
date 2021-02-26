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

import com.qualcomm.qti.auth.sampleauthenticatorservice.ISampleAuthenticator;

public class SAConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public ISampleAuthenticator iSampleAuthenticatorServiceConnection;

  public SAConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iSampleAuthenticatorServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SAConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iSampleAuthenticatorServiceConnection = ISampleAuthenticator.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SampleAuthenticatorService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SAConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iSampleAuthenticatorServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SampleAuthenticatorService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iSampleAuthenticatorServiceConnection = null;
  }

}

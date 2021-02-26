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

import com.qualcomm.qti.auth.sampleextauthservice.ISampleExtAuth;

public class SEAConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  public ISampleExtAuth iSampleExtAuthServiceConnection;

  public SEAConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return iSampleExtAuthServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SEAConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    iSampleExtAuthServiceConnection = ISampleExtAuth.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SampleExtAuthService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, SEAConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    iSampleExtAuthServiceConnection = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "SampleExtAuthService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbound() {
    this.iSampleExtAuthServiceConnection = null;
  }

}

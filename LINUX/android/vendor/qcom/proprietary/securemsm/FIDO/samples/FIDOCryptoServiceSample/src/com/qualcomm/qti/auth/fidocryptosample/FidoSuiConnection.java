/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;
//import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidosuiservice.IFidoSui;

public class FidoSuiConnection implements ServiceConnection {

  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
      + FidoSuiConnection.class.getSimpleName();
  private final FidoCryptoClientActivity activity;
  public IFidoSui fidoSuiServiceConnection;

  public FidoSuiConnection(FidoCryptoClientActivity clientActivity) {
    this.activity = clientActivity;
  }

  public boolean isBound() {
    return fidoSuiServiceConnection != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(LOG_TAG, "onServiceConnected+ " + name.toString());
    if (service == null) {
      Toast.makeText(activity.getBaseContext(),
          "FidoSuiService connection failed", Toast.LENGTH_SHORT).show();
      return;
    }
    fidoSuiServiceConnection = IFidoSui.Stub.asInterface(service);
    Toast.makeText(activity.getBaseContext(), "FidoSuiService connected",
        Toast.LENGTH_SHORT).show();
    Log.d(LOG_TAG, "onServiceConnected- ");
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(LOG_TAG, "onServiceDisconnected+ " + className.toString());
    fidoSuiServiceConnection = null;
    Toast.makeText(activity.getBaseContext(), "FidoSuiService disconnected",
        Toast.LENGTH_SHORT).show();
    Log.d(LOG_TAG, "onServiceDisconnected- " + className.toString());
  }

  public void unbind() {
    Log.d(LOG_TAG, "unbind+");
    fidoSuiServiceConnection = null;
    Log.d(LOG_TAG, "unbind-");
  }
}

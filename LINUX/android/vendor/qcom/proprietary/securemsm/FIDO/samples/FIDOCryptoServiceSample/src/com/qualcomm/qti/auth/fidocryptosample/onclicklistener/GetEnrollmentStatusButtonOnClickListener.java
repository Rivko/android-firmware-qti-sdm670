/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.onclicklistener;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class GetEnrollmentStatusButtonOnClickListener implements OnClickListener {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public GetEnrollmentStatusButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        GetEnrollmentStatusButtonOnClickListener.class.getSimpleName() + ".onClick()");
    int authIndex = fidoCryptoClientActivity.spinner.getSelectedItemPosition();
    switch(authIndex){
      case 0:
      case 3:
      case 4:
        Toast.makeText(v.getContext(), "Not supported for this authenticator",
              Toast.LENGTH_SHORT).show();
        break;
      case 1:
        if (fidoCryptoClientActivity.saConnection.iSampleAuthenticatorServiceConnection == null) {
          Toast.makeText(v.getContext(), "SampleAuthenticatorService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGetEnrollmentStatusSA(v);
        break;
      case 2:
        if (fidoCryptoClientActivity.ssaConnection.iSecureSampleAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "SecureSampleAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGetEnrollmentStatusSSA(v);
        break;
      case 6:
        if (fidoCryptoClientActivity.ssa2Connection.iSecureSampleAuth2ServiceConnection == null) {
          Toast.makeText(v.getContext(), "SecureSampleAuth2Service not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGetEnrollmentStatusSSA2(v);
        break;
      default:
        Toast.makeText(v.getContext(), "Unknown authenticator",
              Toast.LENGTH_SHORT).show();
        break;
    }
  }

  private void testGetEnrollmentStatusSA(View v) {
    try {
      long userId = 12345;
      fidoCryptoClientActivity.saConnection.iSampleAuthenticatorServiceConnection.getEnrollmentStatus(
          userId, fidoCryptoClientActivity.iSampleAuthenticatorListener);
      Toast.makeText(v.getContext(), "Getting enrollment status", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testGetEnrollmentStatusSSA(View v) {
    try {
      long userId = 12345;
      fidoCryptoClientActivity.ssaConnection.iSecureSampleAuthServiceConnection.getEnrollmentStatus(
          userId, fidoCryptoClientActivity.iSecureSampleAuthListener);
      Toast.makeText(v.getContext(), "Getting enrollment status", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }
  private void testGetEnrollmentStatusSSA2(View v) {
    try {
      long userId = 12345;
      fidoCryptoClientActivity.ssa2Connection.iSecureSampleAuth2ServiceConnection.getEnrollmentStatus(
          userId, fidoCryptoClientActivity.iSecureSampleAuth2Listener);
      Toast.makeText(v.getContext(), "Getting enrollment status", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }
}

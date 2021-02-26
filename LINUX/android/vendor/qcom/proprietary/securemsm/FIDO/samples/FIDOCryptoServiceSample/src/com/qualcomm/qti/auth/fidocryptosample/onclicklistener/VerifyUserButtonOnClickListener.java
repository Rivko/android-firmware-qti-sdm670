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
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class VerifyUserButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  static byte[] Sha256(byte[] message) {
    byte[] out = null;
    MessageDigest digest = null;
    try {
      digest = MessageDigest.getInstance("SHA-256");
      if (message != null) {
        digest.update(message);
      }
      out = digest.digest();
    } catch (NoSuchAlgorithmException e1) {
      Log.e(FidoCryptoClientActivity.LOG_TAG, "Error in Sha256");
    }
    return out;
  }

  public VerifyUserButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        VerifyUserButtonOnClickListener.class.getSimpleName() + ".onClick()");
    int authIndex = fidoCryptoClientActivity.spinner.getSelectedItemPosition();

    switch(authIndex){
      case 0:
        if (fidoCryptoClientActivity.maConnection.iMockAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "MockAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGiveAuthToken(v);
        testVerifyUser(v);
        break;
      case 1:
        if (fidoCryptoClientActivity.saConnection.iSampleAuthenticatorServiceConnection == null) {
          Toast.makeText(v.getContext(), "SampleAuthenticatorService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testVerifyUserSA(v);
        break;
      case 2:
        if (fidoCryptoClientActivity.ssaConnection.iSecureSampleAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "SecureSampleAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testVerifyUserSSA(v);
        break;
      case 3:
        if (fidoCryptoClientActivity.seaConnection.iSampleExtAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "SampleExtAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGetTokenSEA(v);
        break;
      case 4:
        if (fidoCryptoClientActivity.secExtConnection.iSecureExtAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "SecureExtAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGetTokenSecExt(v);
        break;
      case 5:
        if (fidoCryptoClientActivity.haConnection.iHealthAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "HealthtAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGetTokenHA(v);
        break;
      case 6:
        if (fidoCryptoClientActivity.ssa2Connection.iSecureSampleAuth2ServiceConnection == null) {
          Toast.makeText(v.getContext(), "SecureSampleAuth2Service not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testVerifyUserSSA2(v);
        break;
    }
  }

  private void testVerifyUser(View v) {
    try {
      Log.d(FidoCryptoClientActivity.LOG_TAG, "Sending verify user request");
      fidoCryptoClientActivity.maConnection.iMockAuthServiceConnection
          .verifyUser(fidoCryptoClientActivity.iMockAuthListener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT)
          .show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testGiveAuthToken(View v) {
    try {
      int mVersionMajor = 1;
      int mVersionMinor = 0;
      int mResult = 0;
      long mUserId = 12345;
      long mUptime = 0;
      byte[] mExtData = { 0x03, 0x01, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04 };
      long mUserEntityId = 987654321;

      Log.d(FidoCryptoClientActivity.LOG_TAG, "Sample token to be set");
      fidoCryptoClientActivity.maConnection.iMockAuthServiceConnection
          .giveAuthToken(mVersionMajor, mVersionMinor, mResult, mUserId,
              mUptime, mExtData, mUserEntityId);
      Toast.makeText(v.getContext(), "token given", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testVerifyUserSA(View v) {
    try {

      String secAppName = "fidocrypto";
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending verify user request for " + secAppName);
      fidoCryptoClientActivity.saConnection.iSampleAuthenticatorServiceConnection.verifyUser(
          nonce, secAppName, fidoCryptoClientActivity.iSampleAuthenticatorListener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testVerifyUserSSA(View v) {
    try {

      String secAppName = "fidocrypto";
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending verify user request for " + secAppName);
      fidoCryptoClientActivity.ssaConnection.iSecureSampleAuthServiceConnection.verifyUser(
          nonce, secAppName, fidoCryptoClientActivity.iSecureSampleAuthListener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testVerifyUserSSA2(View v) {
    try {
      fidoCryptoClientActivity.ssa2Connection.iSecureSampleAuth2ServiceConnection.verifyUser(
          fidoCryptoClientActivity.iSecureSampleAuth2Listener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testGetTokenSEA(View v) {
    try {
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      String secAppName = "fidocrypto";
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending getToken request for " + secAppName);
      fidoCryptoClientActivity.seaConnection.iSampleExtAuthServiceConnection.getToken(
          nonce, secAppName, fidoCryptoClientActivity.iSampleExtAuthListener);
      Toast.makeText(v.getContext(), "Obtaining token", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testGetTokenSecExt(View v) {
    try {
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      String secAppName = "fidocrypto";
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending getToken request for " + secAppName);
      fidoCryptoClientActivity.secExtConnection.iSecureExtAuthServiceConnection.getToken(
          nonce, secAppName, fidoCryptoClientActivity.iSecureExtAuthListener);
      Toast.makeText(v.getContext(), "Obtaining token", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testGetTokenHA(View v) {
    try {
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      String secAppName = "fidocrypto";
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending request for " + secAppName);
      fidoCryptoClientActivity.haConnection.iHealthAuthServiceConnection.getDeviceHealthReport(
          nonce, secAppName, fidoCryptoClientActivity.iHealthAuthListener);
      Toast.makeText(v.getContext(), "Obtaining token", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }
}

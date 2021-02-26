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
import com.qualcomm.qti.auth.fidocryptosample.samples.DeregisterSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.GetInfoSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.MalformedSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.OpenSettingsSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.RegisterSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.SignSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.RegisterExtSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.SignExtSampleCase;

public class ProcessButtonOnClickListener implements OnClickListener {

  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
      + ProcessButtonOnClickListener.class.getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public ProcessButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(LOG_TAG, ProcessButtonOnClickListener.class.getSimpleName()
        + ".onClick()");
    if (!fidoCryptoClientActivity.fidoCryptoConnection.isBound()) {
      Toast.makeText(v.getContext(), "FidoCryptoService not connected",
          Toast.LENGTH_SHORT).show();
      Log.d(LOG_TAG, "service not connected");
      return;
    }
    try {
      switch (fidoCryptoClientActivity.fidoOpSpinner.getSelectedItemPosition()) {
      case FidoCryptoClientActivity.GET_INFO_ITEM_POSITION: {
        new GetInfoSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case FidoCryptoClientActivity.REGISTER_ITEM_POSITION: {
        int authIndex = fidoCryptoClientActivity.spinner
            .getSelectedItemPosition();
        switch (authIndex) {
        case 0:
          if(fidoCryptoClientActivity.useExtensions){
            Toast.makeText(v.getContext(),
              "Not supported for this authenticator", Toast.LENGTH_SHORT)
              .show();
          } else {
            new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x02);
          }
          break;
        case 1:
          if(fidoCryptoClientActivity.useExtensions) {
            new RegisterExtSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x03);
            } else {
              new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x03);
            }
          break;
        case 2:
          // Set the authIndex 0 to use the biolib API, to 4 to use IAuth
          if(fidoCryptoClientActivity.useExtensions) {
            new RegisterExtSampleCase(fidoCryptoClientActivity).test(v,
              (byte) 0x04);
          } else {
            new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x04);
          }
          break;
        case 6:
          // Set the authIndex 0 to use the biolib API, to 4 to use IAuth
          if(fidoCryptoClientActivity.useExtensions) {
            new RegisterExtSampleCase(fidoCryptoClientActivity).test(v,
              (byte) 0x00);
          } else {
            new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x00);
          }
          break;
        case 3:
        case 4:
        case 5:
          Toast.makeText(v.getContext(), "Extension only authenticator",
              Toast.LENGTH_SHORT).show();
          break;
        }
        break;
      }
      case FidoCryptoClientActivity.DEREGISTER_ITEM_POSITION: {
        new DeregisterSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case FidoCryptoClientActivity.OPEN_SETTINGS_ITEM_POSITION: {
        new OpenSettingsSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case FidoCryptoClientActivity.SIGN_ITEM_POSITION:
      case FidoCryptoClientActivity.SIGN_TX_ITEM_POSITION: {
        int authIndex = fidoCryptoClientActivity.spinner
            .getSelectedItemPosition();
        switch (authIndex) {
        case 0:
          if(fidoCryptoClientActivity.useExtensions) {
            Toast.makeText(v.getContext(),
              "Not supported for this authenticator", Toast.LENGTH_SHORT)
              .show();
          } else {
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x02);
          }
          break;
        case 1:
          if(fidoCryptoClientActivity.useExtensions) {
            new SignExtSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x03);
          } else {
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x03);
          }
          break;
        case 2:
          // Set the authIndex 0 to use the biolib API, to 4 to use IAuth
          if(fidoCryptoClientActivity.useExtensions) {
            new SignExtSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x04);
          } else {
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x04);
          }
          break;
        case 6:
          // Set the authIndex 0 to use the biolib API, to 4 to use IAuth
          if(fidoCryptoClientActivity.useExtensions) {
            new SignExtSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x00);
          } else {
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte) 0x00);
          }
          break;
        case 3:
        case 4:
        case 5:
          Toast.makeText(v.getContext(), "Extension only authenticator",
              Toast.LENGTH_SHORT).show();
          break;
        }
        break;
      }
      case FidoCryptoClientActivity.MALFORMED_REQUEST_ITEM_POSITION: {
        new MalformedSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case -1: {
        Toast.makeText(v.getContext(), "check a function", Toast.LENGTH_SHORT)
            .show();
      }
      default: {
        Toast.makeText(v.getContext(), "select fido operation",
            Toast.LENGTH_SHORT).show();
      }
      }
    } catch (RemoteException e) {
      fidoCryptoClientActivity.fidoCryptoConnection.unbind();
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

}

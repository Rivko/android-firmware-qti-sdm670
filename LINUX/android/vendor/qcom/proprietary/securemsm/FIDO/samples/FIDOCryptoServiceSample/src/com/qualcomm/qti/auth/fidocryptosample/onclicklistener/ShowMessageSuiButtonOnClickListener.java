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
import com.qualcomm.qti.auth.fidocryptosample.samples.AbstractSampleCase;

public class ShowMessageSuiButtonOnClickListener implements OnClickListener {

  private static final String LOG_TAG = FidoCryptoClientActivity.LOG_TAG_PREF
       + ShowMessageSuiButtonOnClickListener.class.getSimpleName();

  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public ShowMessageSuiButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    if ((fidoCryptoClientActivity.fidoSuiConnection == null) ||
        (fidoCryptoClientActivity.fidoSuiConnection.fidoSuiServiceConnection == null)){
      Toast.makeText(fidoCryptoClientActivity.getApplicationContext(),
          "FIDOSuiService not bound", Toast.LENGTH_LONG).show();
      return;
    }
    try {
      Log.d(FidoCryptoClientActivity.LOG_TAG, "selectLayoutId");
      int layoutId = selectLayoutId();
      String appId = AbstractSampleCase.APP_ID_STR;
      String secAppName = "fidosui";
      byte[] nonce = VerifyUserButtonOnClickListener.Sha256(FidoCryptoClientActivity.finalChallenge);

      byte[] content = null;
      if (FidoCryptoClientActivity.SIGN_TX_ITEM_POSITION == fidoCryptoClientActivity.fidoOpSpinner.getSelectedItemPosition()) {
        content = fidoCryptoClientActivity.transactionContent;
      }
      Toast.makeText(v.getContext(), "Calling ShowMessageSui",
          Toast.LENGTH_SHORT).show();
      fidoCryptoClientActivity.fidoSuiController = fidoCryptoClientActivity.fidoSuiConnection.fidoSuiServiceConnection
          .showMessageSui(secAppName, appId, nonce, layoutId, content,
              fidoCryptoClientActivity.iFidoSuiListener);
    } catch (RemoteException e) {
      Log.e(FidoCryptoClientActivity.LOG_TAG,
          "fido sui service is unreachable", e);
      Toast.makeText(v.getContext(), "ShowMessageSui Failed",
          Toast.LENGTH_SHORT).show();
    }
  }

  private int selectLayoutId() {
    int authIndex = fidoCryptoClientActivity.spinner.getSelectedItemPosition();
    switch (fidoCryptoClientActivity.fidoOpSpinner.getSelectedItemPosition()) {
    case FidoCryptoClientActivity.REGISTER_ITEM_POSITION: {
      switch (authIndex) {
      case 0:
      case 1:
      case 6:
        return 0x0A000101; // LAYOUT_ID_SAMPLE_REG
      case 2:
        return 0x08000011; // LAYOUT_ID_FP_REG
      case 3:
      case 4:
      case 5:
        return 0;
      }
    }
    case FidoCryptoClientActivity.SIGN_ITEM_POSITION: {
      switch (authIndex) {
      case 0:
      case 1:
      case 6:
        return 0x0A000102; // LAYOUT_ID_SAMPLE_AUTH
      case 2:
        return 0x08000012; // LAYOUT_ID_FP_AUTH
      case 3:
      case 4:
      case 5:
        return 0;
      }
    }
    case FidoCryptoClientActivity.SIGN_TX_ITEM_POSITION: {
      switch (authIndex) {
      case 0:
      case 1:
      case 6:
        if(fidoCryptoClientActivity.usePNGContent) {
          return 0x0F000103; //LAYOUT_ID_SAMPLE_TX_PNG
        } else {
          return 0x0E000103; // LAYOUT_ID_SAMPLE_TX
        }
      case 2:
        if(fidoCryptoClientActivity.usePNGContent) {
          return 0x05000013; //LAYOUT_ID_FP_TX_PNG
        } else {
          return 0x04000013; // LAYOUT_ID_FP_TX
        }
      case 3:
      case 4:
      case 5:
        return 0;
      }
    }
    default: {
      return 0;
    }

    }
  }
}

/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.PorterDuff;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.VoicePrintDemoApp;

public class VoicePrintActivity extends Activity {

  protected static final String TAG = VoicePrintActivity.class.getSimpleName();
  protected VoicePrintManager mManager;


  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    mManager =  getVoicePrintApp().getManager();
  }

  @Override
  protected void onPause() {
    super.onPause();
  }

  public interface KeyboardDoneListener {
    public void onDone();
  }

  public VoicePrintDemoApp getVoicePrintApp() {
    return (VoicePrintDemoApp) getApplication();
  }

  public VoicePrintManager getManager() {
    return getVoicePrintApp().getManager();
  }

  protected void forceShowKeyboard() {
    final InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
    imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, 0);
  }

  protected void showKeyboard(View v) {
    final InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
    imm.showSoftInput(v, InputMethodManager.SHOW_IMPLICIT);
  }


  protected void hideKeyboard() {
    final InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
    imm.toggleSoftInput(0, 0);

  }

  protected void applyEditTextStyle(EditText edit) {
    edit.getBackground().setColorFilter(0xFF009688, PorterDuff.Mode.SRC_ATOP);
  }

  protected void applyEditTextStyle(EditText edit, final KeyboardDoneListener doneListener) {
    edit.getBackground().setColorFilter(0xFF009688, PorterDuff.Mode.SRC_ATOP);

    if (doneListener != null) {
      edit.setOnEditorActionListener(new TextView.OnEditorActionListener() {
        @Override
        public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
          if (actionId == EditorInfo.IME_ACTION_DONE) {
            doneListener.onDone();
            return true;
          }
          return false;
        }
      });
    }
  }

  protected void showDialog(final Context context,
                            String title,
                            String message,
                            String positiveText,
                            String negativeText,
                            DialogInterface.OnClickListener positiveHandler,
                            DialogInterface.OnClickListener negativeHandler) {
    if (context instanceof Activity) {
      new AlertDialog.Builder(context).setMessage(message).setTitle(title)
      .setPositiveButton(positiveText, positiveHandler)
      .setNegativeButton(negativeText, negativeHandler)
      .create().show();
    } else {
      Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
    }
  }

  protected void showDialog(final Context context,
                            int title,
                            int message,
                            int positiveText,
                            int negativeText,
                            DialogInterface.OnClickListener positiveHandler,
                            DialogInterface.OnClickListener negativeHandler) {
    showDialog(context, getString(title), getString(message), getString(positiveText),
               getString(negativeText), positiveHandler, negativeHandler);
  }

  protected void showDialog(final Context context,
                            String title,
                            String message,
                            int positiveText,
                            int negativeText,
                            DialogInterface.OnClickListener positiveHandler,
                            DialogInterface.OnClickListener negativeHandler) {
    showDialog(context, title, message, getString(positiveText),
               getString(negativeText), positiveHandler, negativeHandler);
  }

  protected void showDialog(final Context context,
                            int title,
                            int message,
                            String positiveText,
                            String negativeText,
                            DialogInterface.OnClickListener positiveHandler,
                            DialogInterface.OnClickListener negativeHandler) {
    showDialog(context, getString(title), getString(message), positiveText,
               negativeText, positiveHandler, negativeHandler);
  }


  protected void showDialog(final Context context,
                            String title,
                            String message,
                            String positiveText,
                            DialogInterface.OnClickListener positiveHandler) {
    if (context instanceof Activity) {
      new AlertDialog.Builder(context).setMessage(message).setTitle(title)
      .setPositiveButton(positiveText, positiveHandler)
      .create().show();
    } else {
      Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
    }

  }

  protected void showDialog(final Context context,
                            int title,
                            int message,
                            int positiveText,
                            DialogInterface.OnClickListener positiveHandler) {
    showDialog(context, getString(title), getString(message),
               getString(positiveText), positiveHandler);
  }

  protected void showDialog(final Context context,
                            int title,
                            int message,
                            String positiveText,
                            DialogInterface.OnClickListener positiveHandler) {
    showDialog(context, getString(title), getString(message), positiveText, positiveHandler);
  }

  /*
  * (non-Javadoc)
  *
  * @see android.app.Activity#onKeyLongPress(int, android.view.KeyEvent)
  */
  @Override
  public boolean onKeyLongPress(int keyCode, KeyEvent event) {

    if (keyCode == KeyEvent.KEYCODE_BACK) {

      // Long Press back will switch mode
      getVoicePrintApp().onLongBackPress();
      return true;
    }
    return super.onKeyLongPress(keyCode, event);
  }
}

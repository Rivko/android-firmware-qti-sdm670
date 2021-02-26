/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.antitheft;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.qti.csm.R;

public class InitCheck extends Activity implements TextView.OnEditorActionListener {
    private static final int ANTITHEFT_VERIFIER_PERMISSION_REQUEST = 99;
    private static final String[] RUNTIME_PERMISSIONS = {Manifest.permission.RECEIVE_SMS, Manifest.permission.SEND_SMS,
            Manifest.permission.READ_PHONE_STATE};
    private String password;

    @Override
    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        if (actionId == EditorInfo.IME_ACTION_DONE) {
            hideKeyboard((EditText) v);
            String value = v.getText().toString();
            switch (v.getId()) {
                case R.id.askConfirmationCode:
                    if (!value.equals(password)) {
                        Toast.makeText(this, R.string.text_wrong_confirmationCode_submit, Toast.LENGTH_SHORT).show();
                    } else {
                        startConfigurationActivity();
                    }
                    v.setText(null);
                    break;
            }

            return true;
        }
        return false;
    }

    private void checkPermission() {
        if (Configuration.getPermission(InitCheck.this) == false) {
            Resources res = getResources();
            AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
            alertDialogBuilder.setTitle(res.getString(R.string.info_title));
            alertDialogBuilder.setMessage(res.getString(R.string.text_permission));
            alertDialogBuilder.setCancelable(false);
            alertDialogBuilder.setNegativeButton(res.getString(R.string.text_denied),
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            Configuration.setPermission(InitCheck.this, false);
                            InitCheck.this.finish();
                        }
                    });
            alertDialogBuilder.setPositiveButton(res.getString(R.string.text_allowed),
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            Configuration.setPermission(InitCheck.this, true);
                            dialog.cancel();
                            chooseActivity();
                        }
                    });
            AlertDialog alertDialog = alertDialogBuilder.create();
            alertDialog.show();
        } else {
            chooseActivity();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        boolean granted = false;

        if (requestCode == ANTITHEFT_VERIFIER_PERMISSION_REQUEST) {

            for (String str : permissions) {
                Log.v(Common.TAG, "req :." + str);
            }

            if (grantResults.length > 0) {
                granted = true;

                for (int grantResult : grantResults) {
                    if (grantResult != PackageManager.PERMISSION_GRANTED) {
                        granted = false;
                        break;
                    }
                }
            }

            if (granted) {
                continueCreate();
                return;
            } else {
                Log.v(Common.TAG, "Permission err.");
                Toast.makeText(this, R.string.runtime_permissions_error, Toast.LENGTH_SHORT).show();
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        for (String runtimePermission : RUNTIME_PERMISSIONS) {
            Log.v(Common.TAG, "Checking permissions for: " + runtimePermission);
            if (checkSelfPermission(runtimePermission) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(RUNTIME_PERMISSIONS, ANTITHEFT_VERIFIER_PERMISSION_REQUEST);
                return;
            }
        }
        continueCreate();
    }

    private void continueCreate() {
        setContentView(R.layout.check);
        configureEditTextView();
        checkPermission();
    }

    private void configureEditTextView() {
        EditText askConfrimationCodeEditText = (EditText) this.findViewById(R.id.askConfirmationCode);
        askConfrimationCodeEditText.setOnEditorActionListener(this);
    }

    private void hideKeyboard(EditText editText) {
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(editText.getWindowToken(), 0);
    }

    private void chooseActivity() {
        password = Configuration.getConfirmationCode(this);
        if (password == null) {
            startConfigurationActivity();
        }
    }

    private void startConfigurationActivity() {
        Intent i = new Intent(this, SystemSettings.class);
        startActivity(i);
        finish();
    }
}

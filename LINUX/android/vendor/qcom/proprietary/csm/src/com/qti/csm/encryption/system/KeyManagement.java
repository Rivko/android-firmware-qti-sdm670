/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption.system;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.storage.StorageManager;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.qti.csm.R;
import com.qti.csm.encryption.EncryptKey;

import static com.qti.csm.encryption.system.ProcessInfo.logd;

public class KeyManagement extends Activity implements
        android.view.View.OnClickListener {
    private Button okBtn, cancelBtn;
    private Context mContext;
    private EditText setKey, confirmKey;

    @Override
    public void onClick(View view) {
        StorageManager storageManager = (StorageManager) getSystemService(Context.STORAGE_SERVICE);
        String[] s = storageManager.getVolumePaths();
        if (view.getId() == R.id.create_password_ok) {
            String setString = setKey.getText().toString();
            String confirmString = confirmKey.getText().toString();
            if (setString.length() == 0) {
                ProcessInfo.toast(mContext,
                        getString(R.string.empty_password_hint));
                return;
            }
            if (confirmString.length() == 0) {
                ProcessInfo.toast(mContext,
                        getString(R.string.empty_confirm_password_hint));
                return;
            }
            if (!setString.equals(confirmString)) {
                ProcessInfo.toast(mContext,
                        getString(R.string.inconsistent_password_hint));
                return;
            }

            EncryptKey.writePasswordToFile(setString,
                    EncryptKey.INTERNAL_INDEX);
            EncryptKey.writePasswordToFile(setString,
                    EncryptKey.SDCARD_INDEX);
            finish();

        } else if (view.getId() == R.id.create_password_cancel) {
            finish();
        }
    }

    private void setKey() {
        mContext = getApplicationContext();
        getActionBar().setDisplayShowHomeEnabled(true);
        Intent intent = getIntent();
        if (intent.hasExtra("mode")) {
            String mode = intent.getStringExtra("mode");
            if (SystemInfo.INTENT_CREATE_PASSWORD.equals(mode) ||
                    SystemInfo.INTENT_CHANGE_PASSWORD.equals(mode)) {
                setContentView(R.layout.status_init);
                if (SystemInfo.INTENT_CREATE_PASSWORD.equals(mode)) {
                    setTitle(getString(R.string.create_password));
                } else if (SystemInfo.INTENT_CHANGE_PASSWORD.equals(mode)) {
                    setTitle(getString(R.string.change_password));
                }

                setKey = (EditText) findViewById(R.id.set_password);
                confirmKey = (EditText) findViewById(R.id.confirm_password);
                okBtn = (Button) findViewById(R.id.create_password_ok);
                cancelBtn = (Button) findViewById(R.id.create_password_cancel);

                okBtn.setOnClickListener(this);
                cancelBtn.setOnClickListener(this);
            }
        } else {
            finish();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setKey();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        logd("");
        super.onDestroy();
    }

}

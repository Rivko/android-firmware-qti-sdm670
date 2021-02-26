/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.Utils;
import com.qualcomm.qti.sva.R.id;
import com.qualcomm.qti.sva.R.layout;
import com.qualcomm.qti.sva.R.menu;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

public class TrainingTipsActivity extends Activity {
    private final static String TAG = "ListenLog.TrainingTipsActivity";

    private String keyphraseName = null;
    private boolean isUdk = false;

    // UI elements
    private TextView uiKeyphraseName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_trainingtips);

        // Get data from intent.
        boolean isErrorState = false;
        Bundle extras = this.getIntent().getExtras();
        if (null != extras) {
            // Get keyphrase name.
            if (extras.containsKey(Global.EXTRA_DATA_KEYPHRASE_NAME)) {
                keyphraseName = extras.getString(Global.EXTRA_DATA_KEYPHRASE_NAME);
                Log.v(TAG, "onCreate: intent extra EXTRA_KEYPHRASE_NAME= " + keyphraseName);
            } else {
                isErrorState = true;
            }
            // Get whether to train a UDK SM.
            isUdk = extras.getBoolean(Global.TAG_TRAINING_IS_UDK, false);
            Log.v(TAG, "onCreate: intent extra TAG_TRAINING_IS_UDK= " + String.valueOf(isUdk));
        } else {
            isErrorState = true;

        }
        if (isErrorState) {
            Utils.openAlertDialog(TrainingTipsActivity.this, TAG,
                    getString(R.string.error), getString(R.string.cannot_continue));
            return;
        }

        // Set UI text
        uiKeyphraseName = (TextView) findViewById(R.id.trainingtips_tv_keyphrase);
        uiKeyphraseName.setText("\"" + keyphraseName + "\"");
    }

    @Override
    protected void onResume() {
        Log.v(TAG, "onResume");
        Global.getInstance().incrementNumActivitiesShowing();
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.v(TAG, "onPause");
        Global.getInstance().decrementNumActivitiesShowing();
        super.onPause();
    }

    public void BackOrNextClicked(View view) {
        Log.v(TAG, "BackOrNextClicked");
        if (view.getId() == R.id.bottombar_tv_back) {
            Log.v(TAG, "BackOrNextClicked- back was clicked.");
            finish();
        } else if (view.getId() == R.id.bottombar_tv_next) {
            Log.v(TAG, "BackOrNextClicked- next was clicked.");
            Intent intent = new Intent(TrainingTipsActivity.this, TrainingActivity.class);
            intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
            intent.putExtra(Global.TAG_TRAINING_IS_UDK, isUdk);
            startActivity(intent);
        } else {
            Log.e(TAG, "BackOrNextClicked- Unknown button called this function. " +
                    "Neither back nor next was clicked.");
        }
    }
}

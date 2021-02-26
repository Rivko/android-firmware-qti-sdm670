/**
 * Copyright (c) 2015-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.qualcomm.deviceinfo;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.EditText;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;

public class MetaInfoDialog extends Activity {

    private static final String LOG_TAG = "MetaInfoDialog";
    private static final String FILENAME_META_VERSION = "/firmware/verinfo/ver_info.txt";

    private TextView mTextView;

    @Override
    public void onCreate (Bundle savedInstanceState) {
        super.onCreate (savedInstanceState);

        final Context appContext = getApplicationContext();

        // Text box
        mTextView = new TextView(appContext);
        mTextView.setLayoutParams(
                new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT));
        String metaId = "";
        try {
            String meta = DeviceInfo.readFile(FILENAME_META_VERSION);
            try {
                JSONObject metaInfo = new JSONObject(meta).getJSONObject("Metabuild_Info");;
                logd("metaInfo: " + metaInfo.toString());
                metaId = metaInfo.getString("Meta_Build_ID");
            } catch (JSONException e) {
                logd("Invalid JSON");
                metaId = DeviceInfo.readLine(FILENAME_META_VERSION);
            }

        } catch (IOException ioe) {
            // Fail quietly, as the file may not exist on some devices.
        } catch (NumberFormatException nfe) {
            // Fail quietly, returning empty string should be sufficient
        }

       if (TextUtils.isEmpty(metaId)) {
           mTextView.setText(R.string.device_info_unknown);
       } else {
           logd(" metaId: " + metaId);
           mTextView.setText(metaId);
       }

        // Dialog box
        AlertDialog.Builder builder = new AlertDialog.Builder (this, AlertDialog.THEME_HOLO_DARK);
        builder.setTitle(R.string.meta_info)
               .setCancelable(true)
               .setView(mTextView)
               .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                   public void onClick(DialogInterface dialog, int id) {
                       MetaInfoDialog.this.finish();
                   }
               });
        AlertDialog alertDialog = builder.create();
        alertDialog.show();
    }

    private static void logd(String msg) {
        Log.d(LOG_TAG, msg);
    }
}


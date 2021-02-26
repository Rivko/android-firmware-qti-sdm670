/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.R.layout;
import com.qualcomm.qti.sva.Utils;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.RelativeLayout;

public class AddKeyphraseActivity extends Activity {
    private final static String TAG = "ListenLog.AddKeyphraseActivity";

    String enteredKeyphraseName = null;

    // UI elements
    private EditText uiEtKeyphrase;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_addkeyphrase);

        uiEtKeyphrase = (EditText) findViewById(R.id.addkeyphrase_et_userkeyphrase);

        /*// Check for null string and show error dialog if null.
        if (inputText.equalsIgnoreCase("")) {
            Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: username is invalid");
            String inavlidTitle = getString(R.string.soundmodels_dialog_invalidsm_title);
            String inavlidContent = getString(R.string.soundmodels_dialog_invalidsm_message);
            openAlertDialog(inavlidTitle, inavlidContent);
            return;
        }

        String newSoundModelName = Global.getInstance().getSmRepo().
                getUserSoundModelName(inputText);
        Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: new soundModelName: " +
                newSoundModelName);

        // Check if SM already exists, otherwise add it and show it in SMDetails
        if (Global.getInstance().getSmRepo().soundModelExists(newSoundModelName)) {
            Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: showDuplicateSoundModelDialog");
            String duplicateTitle = getString(R.string.soundmodels_dialog_duplicate_title);
            String duplicateMessage = getString(R.string.soundmodels_dialog_duplicate_message);
            openAlertDialog(duplicateTitle, duplicateMessage);
        }*/
    }

    public void BackOrNextClicked(View view) {
        Log.v(TAG, "BackOrNextClicked");
        if (view.getId() == R.id.bottombar_tv_back) {
            Log.v(TAG, "BackOrNextClicked- back was clicked.");
            finish();
        } else if (view.getId() == R.id.bottombar_tv_next) {
            Log.v(TAG, "BackOrNextClicked- next was clicked.");
            enteredKeyphraseName = uiEtKeyphrase.getText().toString();
            Log.v(TAG, "BackOrNextClicked: enteredKeyphraseName= \"" + enteredKeyphraseName + "\".");

            // Prompt user if keyphrase has not been entered and do not continue.
            if (enteredKeyphraseName.equalsIgnoreCase("")) {
                Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: username is invalid");
                String inavlidKeyphraseTitle =
                        getString(R.string.user_addkeyphrase_dialog_invalidkeyphrase_title);
                String inavlidKeyphraseContent =
                        getString(R.string.user_addkeyphrase_dialog_invalidkeyphrase_message);
                Utils.openAlertDialog(AddKeyphraseActivity.this, TAG, inavlidKeyphraseTitle,
                        inavlidKeyphraseContent);
                return;
            }

            // Go to SelectActionActivity and send keyphraseName.
            Intent intent = new Intent(AddKeyphraseActivity.this, SelectActionActivity.class);
            intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, enteredKeyphraseName);
            intent.putExtra(Global.EXTRA_SELECTACTION_MODE, Global.MODE_SELECTACTION_TRAINING);
            startActivity(intent);
        } else {
            Log.e(TAG, "BackOrNextClicked- Unknown button called this function. " +
                    "Neither back nor next was clicked.");
        }
    }
}

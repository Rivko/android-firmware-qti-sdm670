/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseAction;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;

public class ChangeKeyPhraseActionActivity extends VoicePrintActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.set_key_phrase_action);

        // Default is NONE
        RadioButton none = (RadioButton) findViewById(R.id.none);
        none.setChecked(true);
        none.setTag(KeyPhraseAction.getDefault());

        int currentAction = getVoicePrintApp().getCurrentKeyPhrase().getActionId();

        // Create the icons
        KeyPhraseAction action = getVoicePrintApp().getAction(KeyPhraseAction.EMAIL);
        PackageManager pkgManager = getPackageManager();
        if (action != null) {
            ResolveInfo resolveInfo = action.getResolveInfo();
            Drawable drawable = resolveInfo.loadIcon(pkgManager);

            // left, top, right, bottom
            RadioButton email = (RadioButton) findViewById(R.id.email);
            Drawable[] compoundDrawables = email.getCompoundDrawables();
            email.setCompoundDrawablesWithIntrinsicBounds(drawable, null, compoundDrawables[2], null);
            email.setTag(action);

            if (currentAction == KeyPhraseAction.EMAIL) {
                email.setChecked(true);
            }
        }

        action = getVoicePrintApp().getAction(KeyPhraseAction.BROWSER);
        if (action != null) {
            ResolveInfo resolveInfo = action.getResolveInfo();
            Drawable drawable = resolveInfo.loadIcon(pkgManager);

            // left, top, right, bottom
            RadioButton google = (RadioButton) findViewById(R.id.google);
            Drawable[] compoundDrawables = google.getCompoundDrawables();
            google.setCompoundDrawablesWithIntrinsicBounds(drawable, null, compoundDrawables[2], null);
            google.setTag(action);
            if (currentAction == KeyPhraseAction.BROWSER) {
                google.setChecked(true);
            }
        }


        action = getVoicePrintApp().getAction(KeyPhraseAction.SMS);
        if (action != null) {
            ResolveInfo resolveInfo = action.getResolveInfo();
            Drawable drawable = resolveInfo.loadIcon(pkgManager);

            // left, top, right, bottom
            RadioButton sms = (RadioButton) findViewById(R.id.sms);
            Drawable[] compoundDrawables = sms.getCompoundDrawables();
            sms.setCompoundDrawablesWithIntrinsicBounds(drawable, null, compoundDrawables[2], null);
            sms.setTag(action);

            if (currentAction == KeyPhraseAction.SMS) {
                sms.setChecked(true);
            }
        }

        Button next = (Button) findViewById(R.id.next);
        next.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNext();
            }
        });

        RadioGroup group = (RadioGroup) findViewById(R.id.group);
        group.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                RadioButton button = (RadioButton) findViewById(group.getCheckedRadioButtonId());
                KeyPhraseAction action = button.getTag() == null ?
                                         KeyPhraseAction.getDefault() : (KeyPhraseAction) button.getTag();
                Log.d(TAG, "User has selected " + action.toString());
                getVoicePrintApp().setKeyPhraseAction(action.getId());

                try {
                    Log.d(TAG, "Saving to database...");
                    // Save to database
                    VoicePrintDatabase.getInstance().update(getVoicePrintApp().getCurrentKeyPhrase());
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();

    }

    @Override
    public void onPause() {

        super.onPause();
    }


    private void onNext() {
        Log.d(TAG, "Action=" + getVoicePrintApp().getCurrentActionName());

        startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
    }
}

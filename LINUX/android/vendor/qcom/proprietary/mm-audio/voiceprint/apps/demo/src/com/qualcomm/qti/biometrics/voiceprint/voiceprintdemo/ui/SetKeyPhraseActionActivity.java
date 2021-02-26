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
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseAction;

public class SetKeyPhraseActionActivity extends VoicePrintActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.set_key_phrase_action);

        // Default is NONE
        RadioButton none = (RadioButton) findViewById(R.id.none);
        none.setChecked(true);
        none.setTag(KeyPhraseAction.getDefault());

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

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.set_key_phrase_action, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle menu selection
        int id = item.getItemId();
        if (id == R.id.action_cancel) {
            startActivity(new Intent(this, MainActivity.class));
            return true;
        } else if (id == R.id.action_skip) {
            onNext();
        }
        return super.onOptionsItemSelected(item);
    }


    private void onNext() {
        Log.d(TAG, "Action=" + getVoicePrintApp().getCurrentActionName());

        startActivity(new Intent(this, RegisterPhraseHintActivity.class));
    }
}

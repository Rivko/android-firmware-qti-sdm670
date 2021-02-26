/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.Utils;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;


public class EditKeyphraseActivity extends Activity {
    private final static String TAG = "ListenLog.EditKeyphraseActivity";
    private final static int REQUEST_KEYPHRASE_ACTION = 1234;

    // UI
    private TextView uiKeyphraseName;
    private TextView uiActionDesc;
    private TextView uiUserVerificationDesc;
    private AlertDialog dialogSetUserVerification;
    private RadioGroup uiUserVerificationRadioGroup;
    private RadioButton uiUserVerificationDisabled;
    private RadioButton uiUserVerificationEnabled;

    String keyphraseName = null;
    String keyphraseActionName = null;
    Boolean isUdk = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");

        // Get keyphrase name and action from intent extras.
        Bundle extras = this.getIntent().getExtras();
        if (null != extras) {
            if (extras.containsKey(Global.EXTRA_DATA_KEYPHRASE_NAME)) {
                keyphraseName = extras.getString(Global.EXTRA_DATA_KEYPHRASE_NAME);
                Log.v(TAG, "onCreate: intent extra EXTRA_KEYPHRASE_NAME= " + keyphraseName);
            }
        } else {
            Log.e(TAG, "onCreate: Intent extras are null. Cannot proceed.");
        }

        keyphraseActionName = Global.getInstance().getSmRepo().getKeyphraseActionName(keyphraseName);
        if (null == keyphraseActionName) {
            Log.e(TAG, "showDialogDeleteKeyphrase: ERROR cannot find SM for keyphrase= " + keyphraseName);
            new AlertDialog.Builder(this)
                    .setTitle(R.string.error)
                    .setMessage(R.string.user_editkeyphrase_dialog_pdknotfound_message)
                    .setCancelable(true)
                    .setNegativeButton(R.string.back, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    })
                    .show();
            return;
        }

        // Get whether SM is UDK.
        isUdk = Global.getInstance().getSmRepo().isComUdk(keyphraseName);
        if (null == isUdk) {
            Log.e(TAG, "showDialogDeleteKeyphrase: ERROR cannot find SM for keyphrase= " + keyphraseName);
            new AlertDialog.Builder(this)
                    .setTitle(R.string.error)
                    .setMessage(R.string.user_editkeyphrase_dialog_pdknotfound_message)
                    .setCancelable(true)
                    .setNegativeButton(R.string.back, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    })
                    .show();
            return;
        }

        setContentView(R.layout.activity_user_editkeyphrase);
        initializeUserInterface();
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

    private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");

        // Get elements from XML.
        uiKeyphraseName = (TextView) findViewById(R.id.editkeyphrase_tv_keyphrasename);
        uiActionDesc = (TextView) findViewById(R.id.editkeyphrase_tv_actiondesc);
        uiUserVerificationDesc = (TextView) findViewById(R.id.editkeyphrase_tv_userverificationdesc);

        uiKeyphraseName.setText(getString(R.string.keyphrase) + " " + keyphraseName);
        uiActionDesc.setText(keyphraseActionName);
        // Show user verification text for non-UIMC and keyphrases that have UV enabled.
        if (isUdk || Global.getInstance().getSmRepo().
                getIsUserVerificationEnabledForKeyphrase(keyphraseName)) {
            uiUserVerificationDesc.setText(R.string.user_editkeyphrase_userverification_on);
        } else {
            uiUserVerificationDesc.setText(R.string.user_editkeyphrase_userverification_off);
        }
    }

    // Starts the SelectActionActivity when the user clicks the Sound Model Layout
    public void selectAction(View view) {
        Log.v(TAG, "selectAction");
        Intent intent = new Intent(EditKeyphraseActivity.this, SelectActionActivity.class);
        intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
        startActivityForResult(intent, REQUEST_KEYPHRASE_ACTION);
    }

    // Opens the User Verification dialog to allow the user to select whether to enabled/disable
    // user verification.
    public void showDialogSetUserVerification(View view) {
        Log.v(TAG, "showDialogSetUserVerification");

        if (isUdk) {
            Utils.openAlertDialog(EditKeyphraseActivity.this, TAG, "User Verification",
                    "This is your personalized keyphrase. To allow anyone to say a keyphrase, " +
                            "enable a pre-loaded keyphrase.");
            return;
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View setUserVerificationView = getLayoutInflater().inflate(R.layout.dialog_userverification, null);

        uiUserVerificationDisabled = (RadioButton) setUserVerificationView.
                findViewById(R.id.editkeyphrase_dialog_userverification_rb_disabled);
        uiUserVerificationEnabled = (RadioButton) setUserVerificationView.
                findViewById(R.id.editkeyphrase_dialog_userverification_rb_enabled);

        if (Global.getInstance().getSmRepo().
                getIsUserVerificationEnabledForKeyphrase(keyphraseName)) {
            if (uiUserVerificationEnabled != null) {
                uiUserVerificationEnabled.setChecked(true);
            }
        } else {
            if (uiUserVerificationDisabled != null) {
                uiUserVerificationDisabled.setChecked(true);
            }
        }

        builder.setView(setUserVerificationView)
                .setTitle(R.string.editkeyphrase_userverification)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.dialog_save, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        uiUserVerificationRadioGroup = (RadioGroup) dialogSetUserVerification.
                                findViewById(R.id.editkeyphrase_dialog_userverification_rg);

                        int checkedRadioButtonId = uiUserVerificationRadioGroup.getCheckedRadioButtonId();
                        // User wants to disable user verification.
                        if (checkedRadioButtonId == R.id.editkeyphrase_dialog_userverification_rb_disabled) {
                            Log.v(TAG, "showDialogSetUserVerification: user clicked to disable user" +
                                    "verification.");
                            Global.getInstance().getSmRepo().setIsUserVerificationEnabled(
                                    EditKeyphraseActivity.this, keyphraseName, false);
                            uiUserVerificationDesc.setText(R.string.user_editkeyphrase_userverification_off);
                        } else if (checkedRadioButtonId == R.id.editkeyphrase_dialog_userverification_rb_enabled) {
                            // User wants to enable user verification.
                            Log.v(TAG, "showDialogSetUserVerification: user clicked to enable user" +
                                    "verification.");
                            if (Global.getInstance().getSmRepo().getPdkSmIsTrained(keyphraseName) == false) {
                                Log.v(TAG, "showDialogSetUserVerification: keyphrase= " + keyphraseName
                                        + " isn't trained. Start training.");
                                // For PDKs, null is passed for keyphraseAction, and
                                // keyphraseActionIntent because they'll both be populated in the
                                // function from the corresponding UIMC.
                                beginToTraining();

                            } else {
                                Log.v(TAG, "showDialogSetUserVerification: keyphrase= " + keyphraseName
                                        + " is trained.");
                                resetToTraining();
                            }
                        }
                    }
                });
        if (false == ((Activity) EditKeyphraseActivity.this).isFinishing()) {
            dialogSetUserVerification = builder.show();
        }
    }


    private void beginToTraining() {
        Global.getInstance().getSmRepo().setTempTrainingComKeyphrase(keyphraseName,
                null, null, false);
        Global.getInstance().getSmRepo().setComSoundModelNameToExtend(keyphraseName);
        Intent intent = new Intent(EditKeyphraseActivity.this,
                TrainingTipsActivity.class);
        intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
        intent.putExtra(Global.TAG_TRAINING_IS_UDK, false);
        startActivity(intent);
    }

    private void resetToTraining() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        String title = getResources().getString(
                R.string.editkeyphrase_diaglog_userverification_enabled);
        String message = getResources().getString(
                R.string.user_editkeyphrase_dialog_overwrite_message);

        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Global.getInstance().getSmRepo().setIsUserVerificationEnabled(
                                EditKeyphraseActivity.this, keyphraseName, true);
                        uiUserVerificationDesc.setText(
                                R.string.user_editkeyphrase_userverification_on);
                    }
                })
                .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        beginToTraining();
                    }
                });

        if (false == ((Activity) EditKeyphraseActivity.this).isFinishing()) {
            builder.show();
        }

    }


    // Opens the Delete Keyphrase dialog to prompt the user as to whether they're sure they want to
    // delete the keyphrase.
    public void showDialogDeleteKeyphrase(View view) {
        Log.v(TAG, "showDialogDeleteKeyphrase");

        // If PDK, tell user PDKs can't be deleted.
        if (isUdk == false) {
            Utils.openAlertDialog(EditKeyphraseActivity.this, TAG, getString(R.string.cannot_delete),
                    getString(R.string.user_editkeyphrase_pdknodeletion));
            return;
        }

        // Prompt user to confirm deletion.
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        String title = getString(R.string.user_editkeyphrase_dialog_confirmdelete_title);
        String message = getString(R.string.user_editkeyphrase_dialog_confirmdelete_message) +
                " \" " + keyphraseName + "\"";
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.dialog_delete, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.v(TAG, "showDialogDeleteKeyphrase: calling finish()");
                        Global.getInstance().getSmRepo().deleteComKeyphrase(keyphraseName);
                        Intent intent = new Intent();
                        intent.putExtra(Global.EXTRA_ACTION_DELETE_KEYPHRASE, true);
                        intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
                        setResult(RESULT_OK, intent);
                        finish();
                        Log.v(TAG, "showDialogDeleteKeyphrase: finish() called");
                    }
                });

        if (false == ((Activity) EditKeyphraseActivity.this).isFinishing()) {
            builder.show();
        }
    }

    // Updates the actionName in the UI when the user gets back from setting it.
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.v(TAG, "onActivityResult");
        if (REQUEST_KEYPHRASE_ACTION == requestCode) {
            if (RESULT_OK == resultCode) {
                Log.v(TAG, "onActivityResult: action selected so update action.");
                String updatedKeyphraseActionName = Global.getInstance().getSmRepo().
                        getKeyphraseActionName(keyphraseName);
                if (null == updatedKeyphraseActionName) {
                    Log.e(TAG, "onActivityResult: actionName not found for keyphrase= " + keyphraseName);
                    return;
                }
                // If the selected keyphraseAction is different from the existing one, update
                // MainActivity upon return.
                if (updatedKeyphraseActionName.equals(keyphraseActionName) == false) {
                    uiActionDesc.setText(updatedKeyphraseActionName);
                    Intent intent = new Intent();
                    intent.putExtra(Global.EXTRA_ACTION_UPDATE_KEYPHRASEACTION, true);
                    intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
                    intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_ACTION, updatedKeyphraseActionName);
                    setResult(RESULT_OK, intent);
                }
            } else {
                Log.v(TAG, "onActivityResult: no need to update since action wasn't changed.");
            }
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        //Fix set action fail when doing portrait /landscape switch.
        Intent intent = new Intent();
        intent.putExtra(Global.EXTRA_ACTION_UPDATE_KEYPHRASEACTION, true);
        intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
        intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_ACTION,
                Global.getInstance().getSmRepo().getKeyphraseActionName(keyphraseName));
        setResult(RESULT_OK, intent);
    }
}

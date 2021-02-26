/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.Keyphrase;
import com.qualcomm.qti.sva.MessageType;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.SoundModel;
import com.qualcomm.qti.sva.User;
import com.qualcomm.qti.sva.VwuService;

public class SoundModelDetailsActivity extends Activity {
    private final static String TAG = "ListenLog.SoundModelDetailsActivity";
    private final static int REQUEST_TRAINING_NEWUSER = 4451;
    public final static String EXTRA_SMNAME = "extra.soundmodeldetails.smname";

    // Listed in order of appearance.
    private Messenger sendToServiceMessenger;
    private String soundModelName = null;
    private boolean isTrainingRequired = false;
    private SoundModel soundModel;
    private boolean isKeyphraseSoundModel = false;
    private TextView uiSmName;
    private TextView uiSmType;
    private LinearLayout uiSoundModelLinearLayout;
    private AlertDialog dialogSetConfidenceLevel;
    private AlertDialog dialogKeywordSelection = null;
    private boolean thisActivityIsShowing = true;
    boolean[] checkedItems;
    ArrayList<String> selectedSoundModels = new ArrayList<String>();
    private AlertDialog addNewUserDialog;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dev_soundmodeldetails);

        Intent intent = new Intent(this, VwuService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);

        uiSmName = (TextView) findViewById(R.id.soundmodeldetails_smname);
        uiSmType = (TextView) findViewById(R.id.soundmodeldetails_smtype);
        uiSoundModelLinearLayout = (LinearLayout) findViewById(R.id.soundmodeldetails_soundmodellinearlayout);

        Intent passedIntent = getIntent();
        this.soundModelName = passedIntent.getExtras().getString(Global.TAG_SOUND_MODEL_NAME);
        this.isTrainingRequired = passedIntent.getExtras().getBoolean(Global.TAG_REQUIRES_TRAINING);
        if (isTrainingRequired == false) {
            Global.getInstance().getSmRepo().setSoundModelNameToQuery(soundModelName);
        }

        String prettySmName = Global.getInstance().getSmRepo().getSmPrettyName(soundModelName);
        uiSmName.setText(prettySmName);
        if (Global.getInstance().getSmRepo().isKeyphraseSoundModel(soundModelName)) {
            this.isKeyphraseSoundModel = true;
            uiSmType.setText("User-independent");
        }

        // If training is required, get the username and start training.
        // This should be after the UI is set up in onCreate.
        if (isTrainingRequired) {
            String keyphrase = Global.getInstance().getSmRepo().getSoundModelNameFromLongName(
                    soundModelName);
            showDialogAddUser(keyphrase);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.soundmodeldetails_menu, menu);
        return true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");
        thisActivityIsShowing = true;
        Global.getInstance().getSmRepo().setSoundModelNameToQuery(this.soundModelName);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.soundmodeldetails_popup_copy:
                copySoundModel();
                return true;

            case R.id.soundmodeldetails_popup_merge:
                showMergingSelection();
                return true;

            case R.id.soundmodeldetails_popup_delete:
                showDeleteSoundModelDialog();
                return true;

            default:
                return false;
        }
    }

    // Updates the UI when the user gets back from training.
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.v(TAG, "onActivityResult");
        if (REQUEST_TRAINING_NEWUSER == requestCode) {
            if (RESULT_OK == resultCode) {
                isTrainingRequired = false;
                sendReply(MessageType.MSG_QUERY_SM, soundModelName);
            } else {
                // If training was unsuccessful, close the SMDetails activity since
                // no SM was made.
                finish();
            }
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");
        thisActivityIsShowing = false;
    }

    ;

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "onDestroy");

        sendReply(MessageType.MSG_UNREGISTER_CLIENT, null);
        if (null != sendToServiceMessenger) {
            unbindService(mConnection);
        }
    }

    // Connects to the Service.
    private ServiceConnection mConnection = new ServiceConnection() {
        // Registers as a client to receive messages.
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.v(TAG, "onServiceConnected");
            sendToServiceMessenger = new Messenger(service);
            // Register clients.
            sendReply(MessageType.MSG_REGISTER_CLIENT, null);
            Log.v(TAG, "Registering as a client.");
        }

        // Unregisters as a client to receive messages.
        public void onServiceDisconnected(ComponentName name) {
            Log.v(TAG, "onServiceDisconnected");
            sendReply(MessageType.MSG_UNREGISTER_CLIENT, null);
            sendToServiceMessenger = null;
            Log.v(TAG, "disconnected service");
        }
    };

    // Sends messages to the Service
    private void sendReply(int what, Object obj) {
        Log.v(TAG, "sendReply");
        if (null == sendToServiceMessenger) {
            return;
        }

        Message msg = Message.obtain(null, what, obj);
        msg.replyTo = mMessenger;
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    // Sends a message with what to delete to the service
    private void sendDeleteData(String keyPhrase, String user) {
        Log.v(TAG, "sendDeleteData");
        Bundle bundle = new Bundle();
        bundle.putString(Global.TAG_KEYPHRASE_TO_DELETE, keyPhrase);
        bundle.putString(Global.TAG_USER_TO_DELETE, user);

        if (null == sendToServiceMessenger) {
            return;
        }
        Message msg = Message.obtain(null, MessageType.MSG_DELETE_FROM_SOUNDMODEL);
        msg.setData(bundle);
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    // Handles incoming messages from the Service
    public Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            Log.v(TAG, "handleMessage");
            switch (msg.what) {
                //Add back in once done with
                /*case MessageType.MSG_QUERY_SM:
                    Log.v(TAG, "handleMessage: MSG_QUERY_SM");
                    if (null == msg.obj) {
                        Log.e(TAG, "handleMessage: MSG_QUERY_SM- querying SM failed");
                        break;
                    }
                    String smInfo = (String) msg.obj;
                    initializeUserInterface(smInfo);
                    break;*/

                case MessageType.MSG_CLIENT_REGISTERED:
                    Log.v(TAG, "handleMessage: MSG_CLIENT_REGISTERED");
                    // If training is required, it means the SM currently being viewed is a UDK SM
                    // that needs training before it can be created. Therefore, we don't have any
                    // SM to query.
                    if (isTrainingRequired == false) {
                        sendReply(MessageType.MSG_QUERY_SM, soundModelName);
                    }
                    break;

                // Once SM has been queried, we can display the SM details.
                case MessageType.MSG_QUERY_COMPLETED:
                    Log.v(TAG, "handleMessage: MSG_TEST_ONE");
                    if (thisActivityIsShowing) {
                        initializeUserInterface();
                    }
                    break;

                case MessageType.MSG_DELETE_FROM_SOUNDMODEL:
                    Log.v(TAG, "handleMessage: MSG_DELETE_FROM_SOUNDMODEL");
                    String toastMessage = "";
                    if (msg.arg1 == Global.SUCCESS) {
                        toastMessage = "Deletion successful";
                        sendReply(MessageType.MSG_QUERY_SM, soundModelName);
                    } else {
                        toastMessage = "Deletion failed";
                    }
                    Toast.makeText(getApplicationContext(),
                            toastMessage,
                            Toast.LENGTH_SHORT).show();
                    break;

                default:
                    Log.v(TAG, "handleMessage: no such case: " + msg.what);
            }
        }
    };
    private final Messenger mMessenger = new Messenger(mHandler);

    private void showDeleteSoundModelDialog() {
        Log.v(TAG, "showDeleteUserDialog");

        View view = getLayoutInflater().inflate(R.layout.dialog_deletesoundmodel, null);
        TextView uiContent = (TextView) view.findViewById(R.id.soundmodels_deletesoundmodel);
        String content = "Are you sure you want to delete the sound model: " + soundModelName + "?";
        uiContent.setText(content);

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setView(view)
                .setTitle(R.string.soundmodels_dialog_deletesoundmodel_title)
                .setCancelable(false)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.soundmodels_dialog_option_deletesoundmodel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        deleteSoundModel();
                    }
                });
        builder.show();
    }

    private void deleteSoundModel() {
        Log.v(TAG, "deleteSoundModel");
        if (Global.getInstance().getSmRepo().deleteSoundModel(soundModelName)) {
            Intent intent = new Intent();
            intent.putExtra(Global.TAG_DELETED_SOUND_MODEL_NAME, soundModelName);
            setResult(RESULT_OK, intent);
        } else {
            setResult(RESULT_CANCELED);
        }
        finish();
    }

    private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");

        soundModel = Global.getInstance().getSmRepo().getQueriedSoundModel();

        uiSoundModelLinearLayout.removeAllViews();
        uiSoundModelLinearLayout.invalidate();

        //TODO udk change the keyphraseUser layout to include a text field under the keyphrase
        // that can be optionally populated to user-defined
        //TODO udk remove launch from keyphraseUser layout if udk
        // Programmatically inflate xml views and fill with keyword and user info.
        for (Keyphrase keyphrase : soundModel.getKeyphrases()) {
            Log.v(TAG, "initializeUserInterface- keyphrase: " + keyphrase.getName());
            //TODO UDK make sure this works
            boolean isUdk = keyphrase.getIsUdk();
            View keyphraseView = getLayoutInflater().inflate(R.layout.soundmodeldetails_keyworduser, null);
            keyphraseView.setTag(keyphrase.getName());
            TextView uiKeyphrase = (TextView) keyphraseView.findViewById(R.id.soundmodeldetails_keyword);
            uiKeyphrase.setText(keyphrase.getName());
            boolean launchPreference = Global.getInstance().getSmRepo().getLaunchPreference(
                    getApplicationContext(), soundModel.getName(), keyphrase.getName(), null);
            Log.v(TAG, "initializeUserInterface- launchPreference: " + launchPreference);
            if (true == launchPreference) {
                Log.v(TAG, "initializeUserInterface- changing initial launch background");
                ImageView uiKeyphraseLaunch = (ImageView) keyphraseView
                        .findViewById(R.id.soundmodeldetails_keyword_launch);
                uiKeyphraseLaunch.setBackgroundColor(getResources().getColor(R.color.text_gray));
            }
            uiSoundModelLinearLayout.addView(keyphraseView);
            for (User user : keyphrase.getUsers()) {
                Log.v(TAG, "user: " + user.getName());
                View userView = getLayoutInflater().inflate(R.layout.soundmodeldetails_keyworduser, null);
                TextView uiUser = (TextView) userView.findViewById(R.id.soundmodeldetails_keyword);
                uiUser.setTypeface(Typeface.DEFAULT);
                uiUser.setTextColor(getResources().getColor(R.color.text_gray));
                userView.setTag(keyphrase.getName() + "." + user.getName());
                uiUser.setText(user.getName());
                launchPreference = Global.getInstance().getSmRepo().getLaunchPreference(
                        getApplicationContext(), soundModel.getName(), keyphrase.getName(), user.getName());
                Log.v(TAG, "initializeUserInterface- launchPreference: " + launchPreference);
                if (true == launchPreference) {
                    Log.v(TAG, "initializeUserInterface- changing initial launch background");
                    ImageView uiUserLaunch = (ImageView) userView
                            .findViewById(R.id.soundmodeldetails_keyword_launch);
                    uiUserLaunch.setBackgroundColor(getResources().getColor(R.color.text_gray));
                }
                uiSoundModelLinearLayout.addView(userView);
            }
            //TODO Somehow set in sound model creation whether it's a UDK and that field when
            // building layout.
            /*
            In the STApp, after you add a new sound model based on
            the HeySnapdragon.uim there is no "add user option that
            leads you to the training stage. This was possible before
             */
            if (isKeyphraseSoundModel == false && isUdk == false) {
                Log.v(TAG, "initializeUserInterface- adduser");
                View addUserView = getLayoutInflater().inflate(R.layout.soundmodeldetails_adduser, null);
                addUserView.setTag(keyphrase.getName());
                addUserView.setOnClickListener(new OnClickListener() {

                    @Override
                    public void onClick(View v) {
                        Log.v(TAG, "new user onClick- keyword:" + v.getTag());
                        showDialogAddUser(v.getTag().toString());
                    }
                });
                uiSoundModelLinearLayout.addView(addUserView);
            }

            Log.v(TAG, "initializeUserInterface- separator");
            View separatorView = getLayoutInflater().inflate(R.layout.soundmodeldetails_separator, null);
            uiSoundModelLinearLayout.addView(separatorView);
        }
    }

    public void showDialogSetConfidenceLevel(View view) {
        View parentView = (View) view.getParent();
        final String identifier = (String) parentView.getTag();
        Log.v(TAG, "showDialogSetConfidenceLevel clicked for: " + identifier);
        final int currentConfidenceLevel = getCurrentConfidenceLevel(identifier);
        String currentConfidenceLevelString = Integer.toString(currentConfidenceLevel);

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View addUserView = getLayoutInflater().inflate(R.layout.dialog_setconfidencelevel, null);
        EditText editText = (EditText) addUserView.findViewById(
                R.id.soundmodeldetails_dialog_setconfidencelevel_input);
        editText.setText(currentConfidenceLevelString);
        builder.setView(addUserView)
                .setTitle(R.string.soundmodeldetails_dialog_setconfidencelevel_title)
                .setCancelable(false)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.dialog_save, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        EditText editText = (EditText) dialogSetConfidenceLevel.findViewById(
                                R.id.soundmodeldetails_dialog_setconfidencelevel_input);
                        int updatedConfidenceLevel = Integer.parseInt(editText.getText().toString());
                        Log.v(TAG, "showDialogSetConfidenceLevel: user input updatedConfidenceLevel as: " +
                                updatedConfidenceLevel);
                        if (updatedConfidenceLevel != currentConfidenceLevel) {
                            setConfidenceLevel(identifier, updatedConfidenceLevel);
                        }
                    }
                });
        dialogSetConfidenceLevel = builder.show();
        editText = (EditText) dialogSetConfidenceLevel.findViewById(
                R.id.soundmodeldetails_dialog_setconfidencelevel_input);
        editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    dialogSetConfidenceLevel.getWindow().setSoftInputMode(
                            WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
                }
            }
        });
    }

    private int getCurrentConfidenceLevel(String identifier) {
        int currentConfidenceLevel = -1;
        String[] keywordAndUser = identifier.split("\\.");
        if (keywordAndUser.length == 1) {
            currentConfidenceLevel = Global.getInstance().getSmRepo().getConfidenceLevel(
                    getApplicationContext(), soundModel.getName(), identifier, null);
        } else if (keywordAndUser.length == 2) {
            currentConfidenceLevel = Global.getInstance().getSmRepo().getConfidenceLevel(
                    getApplicationContext(), soundModel.getName(), keywordAndUser[0], keywordAndUser[1]);
        } else {
            Log.e(TAG, "getCurrentConfidenceLevel- unrecognized identifier");
        }
        return currentConfidenceLevel;
    }

    private void setConfidenceLevel(String identifier, int inConfidenceLevel) {
        String[] keywordAndUser = identifier.split("\\.");
        if (keywordAndUser.length == 1) {
            Global.getInstance().getSmRepo().setConfidenceLevel(getApplicationContext(),
                    soundModel.getName(), identifier, null, inConfidenceLevel);
        } else if (keywordAndUser.length == 2) {
            Global.getInstance().getSmRepo().setConfidenceLevel(getApplicationContext(),
                    soundModel.getName(), keywordAndUser[0], keywordAndUser[1], inConfidenceLevel);
        } else {
            Log.e(TAG, "setConfidenceLevel- unrecognized identifier");
        }
    }

    public void setLaunch(View view) {
        Log.v(TAG, "setLaunch");
        View parentView = (View) view.getParent();
        String identifier = (String) parentView.getTag();
        String[] keywordAndUser = identifier.split("\\.");
        // If length of the array after splitting is 1, launch was set for a keyword.
        if (keywordAndUser.length == 1) {
            Log.v(TAG, "setLaunch: keyword= " + identifier);
            Global.getInstance().getSmRepo().toggleLaunchPreference(getApplicationContext(),
                    soundModel.getName(), identifier, null);
            boolean launchPreference = Global.getInstance().getSmRepo().getLaunchPreference(
                    getApplicationContext(), soundModel.getName(), identifier, null);
            if (true == launchPreference) {
                Log.v(TAG, "setLaunch: true launchPreference= " + launchPreference);
                view.setBackgroundColor(getResources().getColor(R.color.text_gray));
                Log.v(TAG, "setLaunch: true launchPreference=" + launchPreference + " set");
            } else {
                Log.v(TAG, "setLaunch: false launchPreference= " + launchPreference);
                view.setBackgroundColor(getResources().getColor(R.color.transparent));
                Log.v(TAG, "setLaunch: false launchPreference= " + launchPreference + " set");
            }
        } else if (keywordAndUser.length == 2) {
            // If length of the array after splitting is 2, launch was set for a user.
            Log.v(TAG, "setLaunch: keyword= " + keywordAndUser[0] + ", user= " + keywordAndUser[1]);
            Global.getInstance().getSmRepo().toggleLaunchPreference(getApplicationContext(),
                    soundModel.getName(), keywordAndUser[0], keywordAndUser[1]);
            boolean launchPreference = Global.getInstance().getSmRepo().getLaunchPreference(
                    getApplicationContext(), soundModel.getName(), keywordAndUser[0], keywordAndUser[1]);
            if (true == launchPreference) {
                Log.v(TAG, "setLaunch: true launchPreference= " + launchPreference);
                view.setBackgroundColor(getResources().getColor(R.color.text_gray));
                Log.v(TAG, "setLaunch: true launchPreference=" + launchPreference + " set");
            } else {
                Log.v(TAG, "setLaunch: false launchPreference= " + launchPreference);
                view.setBackgroundColor(getResources().getColor(R.color.transparent));
                Log.v(TAG, "setLaunch: false launchPreference= " + launchPreference + " set");
            }
        } else {
            Log.e(TAG, "setLaunch- unrecognized identifier");
        }
    }

    public void showDialogDeleteKeyphraseOrUser(View view) {
        View parentView = (View) view.getParent();
        String identifier = (String) parentView.getTag();
        String[] keyPhraseAndUser = identifier.split("\\.");
        String keyphrase = null;
        String user = null;
        if (1 == keyPhraseAndUser.length) {
            keyphrase = keyPhraseAndUser[0];
        } else if (2 == keyPhraseAndUser.length) {
            keyphrase = keyPhraseAndUser[0];
            user = keyPhraseAndUser[1];
        }
        if (keyphrase != null && user == null) {
            Log.v(TAG, "showDialogDeleteKeyphraseOrUser: keyPhrase= " + keyphrase);
            showDialogConfirmDeleteKeyPhrase(keyphrase);
        } else if (keyphrase != null && user != null) {
            Log.v(TAG, "showDialogDeleteKeyphraseOrUser: keyPhrase= " + keyphrase + ", user: " + user);
            showDialogDeleteUser(keyphrase, user);
        } else {
            Log.v(TAG, "showDialogDeleteKeyphraseOrUser- unrecognized identifier");
        }
    }

    private void showDialogConfirmDeleteKeyPhrase(final String keyPhrase) {
        Log.v(TAG, "showDialogConfirmDeleteKeyPhrase: keyPhrase: " + keyPhrase);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        //TODO add these to strings
        String title = "Confirm Deletion";
        String message = "Are you sure you want to delete the key phrase: " + keyPhrase;
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.dialog_delete, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Global.getInstance().getSmRepo().setSoundModelNameToDeleteFrom(soundModelName);
                        sendDeleteData(keyPhrase, null);
                    }
                });

        if (false == ((Activity) SoundModelDetailsActivity.this).isFinishing()) {
            builder.show();
        }
    }

    private void showDialogDeleteUser(final String keyPhrase, final String user) {
        Log.v(TAG, "showDialogDeleteUser: keyPhrase= " + keyPhrase + ", user: " + user);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        //TODO add these to strings
        String title = "Delete User";
        String message = "Do you want to delete the user \"" + user + "\" from all key phrases in this " +
                "sound model or just from the key phrase \"" + keyPhrase + "\"?";
        String negativeResponse = "All key phrases";
        String positiveResponse = "Just this key phrase";
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(negativeResponse, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Global.getInstance().getSmRepo().setSoundModelNameToDeleteFrom(soundModelName);
                        sendDeleteData(null, user);
                    }
                })
                .setPositiveButton(positiveResponse, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Global.getInstance().getSmRepo().setSoundModelNameToDeleteFrom(soundModelName);
                        sendDeleteData(keyPhrase, user);
                    }
                });

        if (false == ((Activity) SoundModelDetailsActivity.this).isFinishing()) {
            builder.show();
        }
    }

    private void showDialogGeneric(String title, String message) {
        Log.v(TAG, "showDialogGeneric: title= " + title + ", message= " + message);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, null);

        if (false == ((Activity) SoundModelDetailsActivity.this).isFinishing()) {
            builder.show();
        }
    }

    private void copySoundModel() {
        selectedSoundModels.clear();
        selectedSoundModels.add(this.soundModelName);
        showNameSoundModelDialog(selectedSoundModels, true);
    }


    /*** Merge Functions ***/

    // Shows a dialog of all the keywords and allows the user to select one for training a new
    // sound model.
    private void showMergingSelection() {
        Log.v(TAG, "showMergingSelection");

        // get all the sound model names
        ArrayList<String> soundModelNamesArrayListToCull =
                Global.getInstance().getSmRepo().getSoundModelNames(false);

        // remove the current sound model in this activity from the list
        for (int i = 0; i < soundModelNamesArrayListToCull.size(); i++) {
            Log.v(TAG, "showMergingSelection: SoundModelNamesArrayListToCull: " +
                    soundModelNamesArrayListToCull.get(i));
            if (soundModelNamesArrayListToCull.get(i).equals(this.soundModelName)) {
                soundModelNamesArrayListToCull.remove(i);
            }
        }
        final CharSequence[] soundModelNamesList = soundModelNamesArrayListToCull.toArray(
                new CharSequence[soundModelNamesArrayListToCull.size()]);
        if (null == soundModelNamesList) {
            showDialogGeneric("Error", "No other sound models on device.");
            return;
        }

        boolean[] booleanPrimativeArray = new boolean[soundModelNamesList.length];
        checkedItems = new boolean[soundModelNamesList.length];
        selectedSoundModels.clear();

        AlertDialog.Builder dialog = new AlertDialog.Builder(this);
        dialog.setTitle(R.string.soundmodels_dialog_selectsm_title)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        dialogKeywordSelection = null;
                    }
                })
                .setMultiChoiceItems(soundModelNamesList, booleanPrimativeArray, new DialogInterface.OnMultiChoiceClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                        checkedItems[which] = isChecked;
                    }
                })
                .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        for (int i = 0; i < checkedItems.length; i++) {
                            if (checkedItems[i]) {
                                selectedSoundModels.add((String) soundModelNamesList[i]);
                            }
                        }
                        showNameSoundModelDialog(selectedSoundModels, false);
                        dialog.dismiss();
                        dialogKeywordSelection = null;
                    }
                });

        dialogKeywordSelection = dialog.show();
    }

    private void showNameSoundModelDialog(final ArrayList<String> selectedSoundModels,
                                          final boolean isCopy) {
        Log.v(TAG, "showNameSoundModelDialog");
        String selectedKeyPhrasesString = "";
        for (String soundModelName : selectedSoundModels) {
            selectedKeyPhrasesString += soundModelName + ", ";
        }
        Log.v(TAG, "showNameSoundModelDialog: selected key phrases: " + selectedKeyPhrasesString);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        View view = getLayoutInflater().inflate(R.layout.dialog_edittext, null);
        String message = getString(R.string.soundmodels_dialog_namesoundmodel_message) +
                " merged sound model.";

        if (isCopy) {
            EditText nameField = (EditText) view.findViewById(R.id.edittext);
            String currentSMName = Global.getInstance().getSmRepo().getSoundModelNameFromLongName(
                    selectedSoundModels.get(0));
            nameField.setText(currentSMName + "_copy");
        }

        builder.setView(view)
                .setTitle(R.string.soundmodels_dialog_namesoundmodel_title)
                .setMessage(message)
                .setCancelable(false)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.dialog_save, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        EditText editText = (EditText) addNewUserDialog.findViewById(R.id.edittext);
                        String inputText = editText.getText().toString();

                        if (inputText.equalsIgnoreCase("")) {
                            Log.v(TAG, "showNameSoundModelDialog: sound model name is invalid");
                            String inavlidTitle = getString(R.string.soundmodels_dialog_invalidsm_title);
                            String inavlidContent = getString(R.string.soundmodels_dialog_invalidsm_message);
                            showDialogGeneric(inavlidTitle, inavlidContent);
                            return;
                        }

                        String newSoundModelName = Global.getInstance().getSmRepo().
                                getUserSoundModelName(inputText);
                        Log.v(TAG, "showNameSoundModelDialog: new soundModelName: " +
                                newSoundModelName);

                        if (Global.getInstance().getSmRepo().soundModelExists(newSoundModelName)) {
                            Log.v(TAG, "showNameSoundModelDialog: showDuplicateSoundModelDialog");
                            String duplicateTitle = getString(R.string.soundmodels_dialog_duplicate_title);
                            String duplicateMessage = getString(R.string.soundmodels_dialog_duplicate_message);
                            showDialogGeneric(duplicateTitle, duplicateMessage);
                        } else {
                            Log.v(TAG, "showNameSoundModelDialog: adding new sound model= " +
                                    newSoundModelName);
                            addSoundModel(selectedSoundModels, newSoundModelName, isCopy);
                        }
                    }
                });

        addNewUserDialog = builder.show();
        EditText editText = (EditText) addNewUserDialog.findViewById(R.id.edittext);
        editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    addNewUserDialog.getWindow().setSoftInputMode(
                            WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
                }
            }
        });
    }

    private void addSoundModel(ArrayList<String> selectedSoundModels, String newSoundModelName,
                               boolean isCopy) {
        Log.v(TAG, "addSoundModel: newSoundModelName= " + newSoundModelName);

        ArrayList<String> selectedSoundModelNames = new ArrayList<String>();
        for (String selectedSoundModelName : selectedSoundModels) {
            Log.v(TAG, "addSoundModel: selectedSoundModelName= " + selectedSoundModelName);
            Log.v(TAG, "addSoundModel: current soundModelName= " + soundModelName);
            selectedSoundModelNames.add(selectedSoundModelName);
        }
        if (!isCopy) {
            selectedSoundModelNames.add(this.soundModelName);
        }
        if (Global.getInstance().getSmRepo().createNewSoundModel(selectedSoundModelNames,
                newSoundModelName)) {
            Log.v(TAG, "addSoundModel: sending to service to merge new SM");
            sendReply(MessageType.MSG_CREATE_MERGED_SM, null);
        } else if (!isCopy) {
            Log.e(TAG, "addSoundModel: no sound models to merge");
        } else if (isCopy) {
            Log.v(TAG, "addSoundModel: sending to service to merge new SM");
            Intent intent = new Intent(SoundModelDetailsActivity.this,
                    SoundModelDetailsActivity.class);
            intent.putExtra(Global.TAG_SOUND_MODEL_NAME, newSoundModelName);
            intent.putExtra(Global.TAG_REQUIRES_TRAINING, false);
            startActivity(intent);
        }
    }


    // Shows a dialog that allows the user to create a new username for training a new sound model
    // when the user clicks New User in the keyword list.
    public void showDialogAddUser(final String inKeyword) {
        Log.v(TAG, "showDialogAddUser");
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        View view = getLayoutInflater().inflate(R.layout.dialog_edittext, null);
        builder.setView(view)
                .setTitle(R.string.soundmodels_dialog_adduser_title)
                .setMessage(R.string.soundmodels_dialog_adduser_message)
                .setCancelable(false)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.soundmodels_dialog_adduser_starttrainging, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        EditText editText = (EditText) dialogSetConfidenceLevel.findViewById(R.id.edittext);
                        String username = editText.getText().toString();
                        Log.v(TAG, "showDialogAddUser: user input username as: " + editText.getText().toString());

                        if (isTrainingRequired == false) {
                            Log.v(TAG, "showDialogAddUser: isTrainingRequired is false.");
                        } else {
                            Log.v(TAG, "showDialogAddUser: isTrainingRequired is true.");
                        }

                        // Check if user data exists for that keyword in the sound model,
                        // but only if training isn't required since the queried SM wouldn't exist yet
                        if (isTrainingRequired == false && Global.getInstance().getSmRepo().
                                getQueriedSoundModel().userExistsForKeyword(inKeyword, username)) {
                            showDuplicateUserDialog();
                        } else if (username.equalsIgnoreCase("") || username.equalsIgnoreCase("<Add New User>")) {
                            Log.v(TAG, "showDialogAddUser: username is invalid");
                            String inavlidTitle = getString(R.string.soundmodels_dialog_invaliduser_title);
                            String inavlidContent = getString(R.string.soundmodels_dialog_invaliduser_message);
                            showDialogGeneric(inavlidTitle, inavlidContent);
                        } else {
                            Log.v(TAG, "showDialogAddUser: new username= " + editText.getText().toString());
                            Global.getInstance().getSmRepo().setSoundModelNameToExtend(soundModelName);
                            Intent intent = new Intent(SoundModelDetailsActivity.this, TrainingActivity.class);
                            intent.putExtra(Global.TAG_TRAINING_KEYPHRASE, inKeyword);
                            intent.putExtra(Global.TAG_TRAINING_USER, username);
                            intent.putExtra(Global.TAG_TRAINING_IS_UDK, isTrainingRequired);
                            startActivityForResult(intent, REQUEST_TRAINING_NEWUSER);
                        }
                    }
                });

        dialogSetConfidenceLevel = builder.show();
        EditText editText = (EditText) dialogSetConfidenceLevel.findViewById(R.id.edittext);
        editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    dialogSetConfidenceLevel.getWindow().setSoftInputMode(
                            WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
                }
            }
        });
    }

    private void showDuplicateUserDialog() {
        Log.v(TAG, "showDuplicateSoundModelDialog");
        String duplicateTitle = getString(R.string.soundmodeldetails_dialog_duplicateuser_title);
        String duplicateMessage = getString(R.string.soundmodeldetails_dialog_duplicateuser_message);
        showDialogGeneric(duplicateTitle, duplicateMessage);
    }
}

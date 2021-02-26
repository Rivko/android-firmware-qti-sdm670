/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import java.util.ArrayList;
import java.util.Collections;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Toast;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.Global.SmState;
import com.qualcomm.qti.sva.MessageType;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.SoundModel;
import com.qualcomm.qti.sva.VwuService;
import com.qualcomm.qti.sva.ux.dev.SoundModelListAdapter.SoundModelItem;


public class SoundModelsActivity extends Activity {
    private final static String TAG = "ListenLog.SoundModelsActivity";
    public final static String EXTRA_SELECTSOUNDMODELS = "extra.SoundModelsActivity.selectSoundModels";

    private Messenger sendToServiceMessenger;
    private AlertDialog nameSmOrKeyphraseDialog;
    private AlertDialog dialogSoundModelSelection = null;
    private ArrayList<SoundModelListAdapter.SoundModelItem> soundModelList =
            new ArrayList<SoundModelListAdapter.SoundModelItem>();
    private SoundModelListAdapter soundModelListAdapter;
    private ListView uiSoundModelsListView;
    private ImageButton uiAddSoundModel;
    //boolean[] checkedItems;
    boolean[] soundModelCheckBoxes;
    private String createYourOwn;
    ArrayList<String> selectedKeyPhrases = new ArrayList<String>();
    private Button load;
    private Button start;
    private Button stop;
    private Button unload;
    private String selectedSmName = null;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "onCreate");
        super.onCreate(savedInstanceState);

        // Bind to service.
        Intent intent = new Intent(this, VwuService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);

        //TODO transitions get working
        /*// Get SM names to register from adb command
        Bundle extras = this.getIntent().getExtras();
        if (null != extras) {
            if (extras.containsKey(EXTRA_SELECTSOUNDMODELS)) {
                Log.v(TAG, "onCreate: intent extra selectSoundModels= " + extras.getString(
                        EXTRA_SELECTSOUNDMODELS));
                String soundModelsToSelectFromExtra = extras.getString(EXTRA_SELECTSOUNDMODELS);
                ArrayList<String> soundModelsToSelect = new ArrayList<String>(Arrays.asList(
                        soundModelsToSelectFromExtra.split(",")));
                Log.v(TAG, "onCreate: parsed selectSoundModels= " +
                        soundModelsToSelect.toString());
                if (soundModelsToSelect.size() <= Global.getInstance().NUM_MAX_SESSIONS) {
                    Global.getInstance().getSmRepo().saveSmNamesForEachSession(
                            getApplicationContext(), soundModelsToSelect);
                }
            }
        }*/

        // Initialize UI
        setContentView(R.layout.activity_dev_soundmodels);
        initializeUserInterface();

        // Initialize globally-scoped variable
        createYourOwn = getResources().getString(R.string.soundmodels_dialog_createyourown);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");
        initializeUserInterface();
    }

    // Updates the UI when the user gets back from the SoundModelDetailsActivity
    /*@Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.v(TAG, "onActivityResult");
        if (REQUEST_DELETE_SOUND_MODEL == requestCode) {
            if (RESULT_OK == resultCode) {
                String soundModelName = data.getExtras().getString(Global.TAG_DELETED_SOUND_MODEL_NAME);
                Log.v(TAG, "onActivityResult: sound model: " + soundModelName + " was deleted");
                Log.v(TAG, "onActivityResult: queriedSoundModel: " + queriedSoundModel.getSoundModelName());
                soundModelList.remove(this.queriedSoundModel);
                soundModelListAdapter.notifyDataSetChanged();
            }
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }*/

    @Override
    protected void onPause() {
        Log.v(TAG, "onPause");
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        Log.v(TAG, "onDestroy");
        super.onDestroy();

        sendReply(MessageType.MSG_UNREGISTER_CLIENT, null);
        if (null != sendToServiceMessenger) {
            unbindService(mConnection);
        }
    }

    // Sets up menu options
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.v(TAG, "onCreateOptionsMenu");
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.soundmodels, menu);
        return true;
    }

    // Sets up menu options
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.v(TAG, "onOptionsItemSelected");
        switch (item.getItemId()) {

            case R.id.soundmodels_menu_add:
                showSoundModelSelection();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public void onStateButtonClicked(View v) {
        Log.v(TAG, "onStateButtonClicked");
        switch (v.getId()) {

            case R.id.soundmodels_button_load:
                Log.v(TAG, "onStateButtonClicked: load");
                sendReply(MessageType.MSG_LOAD_SM, selectedSmName);
                break;
            case R.id.soundmodels_button_start:
                Log.v(TAG, "onStateButtonClicked: start");
                sendReply(MessageType.MSG_START_RECOGNITION, selectedSmName);
                break;
            case R.id.soundmodels_button_stop:
                Log.v(TAG, "onStateButtonClicked: stop");
                sendReply(MessageType.MSG_STOP_RECOGNITION, selectedSmName);
                break;
            case R.id.soundmodels_button_unload:
                Log.v(TAG, "onStateButtonClicked: unload");
                sendReply(MessageType.MSG_UNLOAD_SM, selectedSmName);
                break;

            default:
                Log.e(TAG, "onStateButtonClicked: unrecognized button pressed");
                Toast.makeText(getApplicationContext(), "unrecognized button pressed",
                        Toast.LENGTH_SHORT).show();
                break;
        }
    }

    /*** Service Functions ***/

    // Connects to the Service
    private ServiceConnection mConnection = new ServiceConnection() {
        // Registers as a client to receive messages.
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.v(TAG, "onServiceConnected");
            sendToServiceMessenger = new Messenger(service);
            // Register clients
            sendReply(MessageType.MSG_REGISTER_CLIENT, null);
            Log.v(TAG, "connected service");
            //querySM();
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

                case MessageType.MSG_CREATE_MERGED_SM:
                    Log.v(TAG, "handleMessage: MSG_CREATE_MERGED_SM");
                    String toastMessage = "";
                    if (msg.arg1 == Global.SUCCESS) {
                        toastMessage = "Merge successful";
                        String mergedSmName = Global.getInstance().getSmRepo().getMergedSoundModelName();
                        addSoundModelToList(mergedSmName);
                        showNewSmDetails(mergedSmName, false);
                    } else {
                        toastMessage = "Merge failed";
                    }
                    Toast.makeText(getApplicationContext(),
                            toastMessage,
                            Toast.LENGTH_SHORT).show();
                    break;

                case MessageType.MSG_LOAD_SM:
                    Log.v(TAG, "handleMessage: load");
                    String loadMessage;
                    if (msg.arg1 == Global.SUCCESS) {
                        loadMessage = "Loaded successfully";
                        Log.v(TAG, "handleMessage: " + loadMessage);
                        enterState(SmState.LOADED);
                    } else {
                        loadMessage = "Loading unsuccessful";
                        Log.e(TAG, "handleMessage: " + loadMessage);
                    }
                    Toast.makeText(getApplicationContext(), loadMessage, Toast.LENGTH_SHORT).show();
                    break;

                case MessageType.MSG_START_RECOGNITION:
                    Log.v(TAG, "handleMessage: start");
                    String startMessage;
                    if (msg.arg1 == Global.SUCCESS) {
                        startMessage = "Started successfully";
                        Log.v(TAG, "handleMessage: " + startMessage);
                        enterState(SmState.STARTED);
                    } else {
                        startMessage = "Starting unsuccessful";
                        Log.e(TAG, "handleMessage: " + startMessage);
                    }
                    Toast.makeText(getApplicationContext(), startMessage, Toast.LENGTH_SHORT).show();
                    break;

                case MessageType.MSG_STOP_RECOGNITION:
                    Log.v(TAG, "handleMessage: stop");
                    String stopMessage;
                    if (msg.arg1 == Global.SUCCESS) {
                        stopMessage = "Stopped successfully";
                        Log.v(TAG, "handleMessage: " + stopMessage);
                        enterState(SmState.STOPPED);
                    } else {
                        stopMessage = "Stopping unsuccessful";
                        Log.e(TAG, "handleMessage: " + stopMessage);
                    }
                    Toast.makeText(getApplicationContext(), stopMessage, Toast.LENGTH_SHORT).show();
                    break;

                case MessageType.MSG_UNLOAD_SM:
                    Log.v(TAG, "handleMessage: unload");
                    String unloadMessage;
                    if (msg.arg1 == Global.SUCCESS) {
                        unloadMessage = "Unloaded successfully";
                        Log.v(TAG, "handleMessage: " + unloadMessage);
                        enterState(SmState.UNLOADED);
                    } else {
                        unloadMessage = "Unloading unsuccessful";
                        Log.e(TAG, "handleMessage: " + unloadMessage);
                    }
                    Toast.makeText(getApplicationContext(), unloadMessage, Toast.LENGTH_SHORT).show();
                    break;

                case MessageType.MSG_QUERY_SM_STATE:
                    Log.v(TAG, "handleMessage: query state");
                    if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: Queried successfully");
                        if (msg.arg2 != -1) {
                            enterState(SmState.values()[msg.arg2]);
                        } else {
                            enterState(SmState.UNLOADED);
                        }
                    } else {
                        Log.e(TAG, "handleMessage: Query unsuccessful");
                        openAlertDialog("Error", "Could not query sound model.");
                    }
                    break;

                default:
                    Log.v(TAG, "handleMessage: no such case: " + msg.what);
            }
        }
    };
    private final Messenger mMessenger = new Messenger(mHandler);


    private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");
        uiSoundModelsListView = (ListView) findViewById(R.id.list_soundmodels);
        //uiAddSoundModel = (ImageButton)findViewById(R.id.button_add_soundmodel);

        fillSoundModelListAdapter();
        soundModelListAdapter = new SoundModelListAdapter(this, soundModelList,
                onItemLeftActionListener, onItemRightActionListener);
        uiSoundModelsListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        uiSoundModelsListView.setAdapter(soundModelListAdapter);

        load = (Button) findViewById(R.id.soundmodels_button_load);
        start = (Button) findViewById(R.id.soundmodels_button_start);
        stop = (Button) findViewById(R.id.soundmodels_button_stop);
        unload = (Button) findViewById(R.id.soundmodels_button_unload);

		/*uiAddSoundModel.setOnClickListener(new OnClickListener() {
            @Override
			public void onClick(View v) {
			    showSoundModelSelection();
			}
		});*/
    }

    // When a SM is clicked, get the current state of that SM (e.g. UNLOADED, LOADED, STARTED,
    // STOPPED).
    private SoundModelListAdapter.OnItemActionListener onItemLeftActionListener =
            new SoundModelListAdapter.OnItemActionListener() {
                @Override
                public void onOptionClicked(View v, SoundModelItem item) {
                    Log.v(TAG, "onItemLeftActionListener.onOptionClicked");
                    selectedSmName = item.getSoundModelName();
                    sendReply(MessageType.MSG_QUERY_SM_STATE, selectedSmName);
                }
            };

    // Opens the SoundModelDetailsActivity for the selected sound model
    private SoundModelListAdapter.OnItemActionListener onItemRightActionListener =
            new SoundModelListAdapter.OnItemActionListener() {
                @Override
                public void onOptionClicked(View v, SoundModelItem item) {
                    Log.v(TAG, "onItemRightActionListener.onOptionClicked");
                    showNewSmDetails(item.getSoundModelName(), false);
                }
            };

    // Shows a dialog of all the sound models and allows the user to select one to create a new
    // sound model.
    private void showSoundModelSelection() {
        Log.v(TAG, "showSoundModelSelection");

        ArrayList<String> tempSoundModelList =
                Global.getInstance().getSmRepo().getSoundModelNames(true);
        if (null == tempSoundModelList) {
            openAlertDialog("Error", "No sound models on device.");
            return;
        }
        tempSoundModelList.add(createYourOwn);
        final CharSequence[] soundModelList = tempSoundModelList.toArray(
                new CharSequence[tempSoundModelList.size()]);
        boolean[] booleanPrimativeArray = new boolean[soundModelList.length];
        soundModelCheckBoxes = new boolean[soundModelList.length];
        selectedKeyPhrases.clear();

        AlertDialog.Builder dialog = new AlertDialog.Builder(this);
        dialog.setTitle(R.string.soundmodels_dialog_selectakeyphrase_title)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        dialogSoundModelSelection = null;
                    }
                })
                .setMultiChoiceItems(soundModelList, booleanPrimativeArray, new DialogInterface.OnMultiChoiceClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                        soundModelCheckBoxes[which] = isChecked;
                    }
                })
                .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        for (int i = 0; i < soundModelCheckBoxes.length; i++) {
                            if (soundModelCheckBoxes[i]) {
                                selectedKeyPhrases.add((String) soundModelList[i]);
                            }
                        }
                        showDialogNameSoundModelOrKeyphrase(selectedKeyPhrases);
                        dialog.dismiss();
                        dialogSoundModelSelection = null;
                    }
                });
        dialogSoundModelSelection = dialog.show();
    }

    enum NewSmType {
        SINGLE_KEYPHRASE,
        MULTI_KEYPHRASE,
        UDK
    }

    private void showDialogNameSoundModelOrKeyphrase(final ArrayList<String> selectedKeyphrases) {
        Log.v(TAG, "showDialogNameSoundModelOrKeyphrase");
        String selectedKeyPhrasesString = "";
        for (String keyPhrase : selectedKeyphrases) {
            selectedKeyPhrasesString += keyPhrase + ", ";
        }
        Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: selected key phrases: " + selectedKeyPhrasesString);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        View view = getLayoutInflater().inflate(R.layout.dialog_edittext, null);
        String message;
        String title;
        //boolean isUdkSmVersionMutable = false;
        NewSmType newSmTypeMutable;
        if (selectedKeyphrases.size() == 1) { // Only one keyphrase was selected.
            if (selectedKeyphrases.get(0).equals(createYourOwn)) {
                // "Create your own..." (keyphrase) was selected.
                newSmTypeMutable = NewSmType.UDK;
                title = getString(R.string.soundmodels_dialog_namekeyphrase_title);
                message = getString(R.string.soundmodels_dialog_namekeyphrase_message);
            } else {
                newSmTypeMutable = NewSmType.SINGLE_KEYPHRASE;
                title = getString(R.string.soundmodels_dialog_namesoundmodel_title);
                message = getString(R.string.soundmodels_dialog_namesoundmodel_message) + " " +
                        selectedKeyphrases.get(0) + " sound model.";
            }
        } else {
            // Make sure create your own keyphrase wasn't selected as part of the multple
            // selected keyphrases.
            if (selectedKeyPhrasesString.contains(createYourOwn)) {
                String inavlidSelectionTitle = getString(R.string.soundmodels_dialog_invalidselection);
                String createYourOwnMultiSelectError = getString(
                        R.string.soundmodels_dialog_createyourownmultiselecterror);
                openAlertDialog(inavlidSelectionTitle, createYourOwnMultiSelectError);
                return;
            }
            newSmTypeMutable = NewSmType.MULTI_KEYPHRASE;
            title = getString(R.string.soundmodels_dialog_namesoundmodel_title);
            message = getString(R.string.soundmodels_dialog_namesoundmodel_message) +
                    " multi key phrase sound model.";
        }
        final NewSmType newSmType = newSmTypeMutable;

        builder.setView(view)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setNegativeButton(R.string.dialog_cancel, null)
                .setPositiveButton(R.string.dialog_save, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        EditText editText = (EditText) nameSmOrKeyphraseDialog.findViewById(R.id.edittext);
                        String inputText = editText.getText().toString();

                        // Check for null string and show error dialog if null.
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
                        } else {
                            Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: adding new sound model= " +
                                    newSoundModelName);
                            Log.v(TAG, "showDialogNameSoundModelOrKeyphrase: newSmType= " + newSmType);
                            switch (newSmType) {
                                case SINGLE_KEYPHRASE:
                                    createSoundModel(selectedKeyphrases, newSoundModelName,
                                            NewSmType.SINGLE_KEYPHRASE);
                                    addSoundModelToList(newSoundModelName);
                                    showNewSmDetails(newSoundModelName, false);
                                    break;
                                case MULTI_KEYPHRASE:
                                    createSoundModel(selectedKeyphrases, newSoundModelName,
                                            NewSmType.MULTI_KEYPHRASE);
                                    break;
                                case UDK:
                                    addSoundModelToList(newSoundModelName);
                                    showNewSmDetails(newSoundModelName, true);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                });

        nameSmOrKeyphraseDialog = builder.show();
        EditText editText = (EditText) nameSmOrKeyphraseDialog.findViewById(R.id.edittext);
        editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    nameSmOrKeyphraseDialog.getWindow().setSoftInputMode(
                            WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
                }
            }
        });
    }

    // Returns true if adding to the list is required.
    private void createSoundModel(ArrayList<String> selectedKeyphrases, String newSoundModelName,
                                  NewSmType smType) {
        Log.v(TAG, "createSoundModel: newSoundModelName= " + newSoundModelName);
        boolean isAddingToSmListRequired = false;

        ArrayList<String> selectedSoundModelNames = new ArrayList<String>();
        for (String keyphrase : selectedKeyphrases) {
            Log.v(TAG, "createSoundModel: keyphrase= " + keyphrase);
            String soundModelName = Global.getInstance().getSmRepo().getSystemSoundModelName(keyphrase);
            selectedSoundModelNames.add(soundModelName);
        }
        // Create the SM.
        Global.getInstance().getSmRepo().createNewSoundModel(selectedSoundModelNames, newSoundModelName);
        if (smType == NewSmType.MULTI_KEYPHRASE) {
            sendReply(MessageType.MSG_CREATE_MERGED_SM, null);
        }
    }

    // adds a SM to the SM list adapter after merging or before training
    private void addSoundModelToList(String newSoundModelName) {
        String prettyName = Global.getInstance().getSmRepo().getSmPrettyName(newSoundModelName);
        SoundModelListAdapter.SoundModelItem item = new SoundModelListAdapter.SoundModelItem(
                true, newSoundModelName, prettyName);

        clearCheckedSoundModels();
        soundModelList.add(item);
        Collections.sort(soundModelList);
        soundModelListAdapter.notifyDataSetChanged();
    }

    // Shows the sm details of the passed in SM. Requests training from SMDetails if
    // it's a UDK SM.
    private void showNewSmDetails(String smNameToShowDetails, boolean isUdkSm) {
        Log.v(TAG, "showNewSmDetails");
        Intent intent = new Intent(SoundModelsActivity.this, SoundModelDetailsActivity.class);
        intent.putExtra(Global.TAG_SOUND_MODEL_NAME, smNameToShowDetails);
        intent.putExtra(Global.TAG_REQUIRES_TRAINING, isUdkSm);
        startActivity(intent);
    }

    private void fillSoundModelListAdapter() {
        Log.v(TAG, "fillSoundModelListAdapter");

        soundModelList.clear();

        ArrayList<SoundModel> soundModels = Global.getInstance().getSmRepo().getAllSoundModels();
        if (null == soundModels) {
            return;
        }
        for (SoundModel soundModel : soundModels) {
            boolean checked = false;
            Log.v(TAG, "fillSoundModelListAdapter: soundModelName= " + soundModel.getName() +
                    ", checked= " + checked);
            SoundModelListAdapter.SoundModelItem listItem = new SoundModelListAdapter.SoundModelItem(
                    checked, soundModel.getName(), soundModel.getPrettyName());
            soundModelList.add(listItem);
        }

        Collections.sort(soundModelList);
        return;
    }

    private void clearCheckedSoundModels() {
        Log.v(TAG, "clearCheckedSoundModels");
        for (SoundModelListAdapter.SoundModelItem item : soundModelList) {
            item.setChecked(false);
        }
    }

    private void openAlertDialog(String title, String message) {
        Log.v(TAG, "openAlertMessage: title= " + title + ", message= " + message);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, null);

        if (false == ((Activity) SoundModelsActivity.this).isFinishing()) {
            builder.show();
        }
    }

    public void enterState(SmState state) {
        Log.v(TAG, "enterState");
        switch (state) {

            case UNLOADED:
                Log.v(TAG, "enterState: UNLOADED");
                load.setEnabled(true);
                start.setEnabled(false);
                stop.setEnabled(false);
                unload.setEnabled(false);
                break;
            case LOADED:
                Log.v(TAG, "enterState: LOADED");
                load.setEnabled(false);
                start.setEnabled(true);
                stop.setEnabled(false);
                unload.setEnabled(true);
                break;
            case STARTED:
                Log.v(TAG, "enterState: STARTED");
                load.setEnabled(false);
                start.setEnabled(false);
                stop.setEnabled(true);
                unload.setEnabled(false);
                break;
            case STOPPED:
                Log.v(TAG, "enterState: STOPPED");
                load.setEnabled(false);
                start.setEnabled(true);
                stop.setEnabled(false);
                unload.setEnabled(true);
                break;

            default:
                Log.e(TAG, "enterState: unrecognized state= " + state.name());
                break;
        }
    }
}
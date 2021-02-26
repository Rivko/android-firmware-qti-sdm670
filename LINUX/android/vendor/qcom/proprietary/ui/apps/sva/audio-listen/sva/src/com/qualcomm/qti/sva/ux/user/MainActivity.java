/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashSet;
import java.util.Timer;
import java.util.TimerTask;

import com.qualcomm.listen.ListenTypes;
import com.qualcomm.listen.ListenTypes.VWUKeywordConfLevel;
import com.qualcomm.listen.ListenTypes.VWUUserKeywordPairConfLevel;
import com.qualcomm.listen.ListenTypes.VoiceWakeupDetectionDataV2;
import com.qualcomm.qti.sva.wrapper.SystemPropertiesWrapper;
import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.Keyphrase;
import com.qualcomm.qti.sva.MessageType;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.ServiceActivity;
import com.qualcomm.qti.sva.Utils;
import com.qualcomm.qti.sva.Global.DetectionContainer;
import com.qualcomm.qti.sva.ux.user.KeyphraseListAdapter;
import com.qualcomm.qti.sva.ux.user.KeyphraseListAdapter.KeyphraseListItem;
import com.qualcomm.qti.sva.ux.user.KeyphraseListAdapter.RowViewHolder;

import android.Manifest.permission;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;

import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.text.TextUtils;

import com.qualcomm.qti.sva.ux.dev.DetectionActivity;

public class MainActivity extends ServiceActivity {
    private final static String TAG = "ListenLog.MainActivity";
    private final static int MSG_RESET_RESULT_VALUES = 1;
    private final static int SHOW_DETECTION_RESULT_LENGTH = 3000;
    public final static int REQUEST_UPDATE_NOTIFICATION = 0;
    private final static String STAPP_IN_MENU = "persist.vendor.stapp.display";
    private final static String STAPP_ENABLED = "1";
    private final static String STAPP_DISABLED = "0";

    private ArrayList<KeyphraseListAdapter.KeyphraseListItem> keyphraseList =
            new ArrayList<KeyphraseListAdapter.KeyphraseListItem>();
    private KeyphraseListAdapter keyphraseListAdapter;
    private RelativeLayout uiRlInstructionalText;
    private LinearLayout uiLlDetection;
    private TextView uiTvSvaOnOff;
    private Switch uiSwSvaOnOff;
    private TextView uiTvInstructionaltext;
    private TextView uiTvKeyphraseDetected;
    private TextView uiTvKeyphraseDetectedAction;
    private ListView uiLvKeyphrases;
    private int numKeyphrasesChecked = 0; // Scope is outside of onClickListener to maintain state across clicks.
    private int enablingKeyphrasesCounter = 0;
    private int disablingKeyphrasesCounter = 0;
    private int detectionSuccessCounter = 0;
    private int detectionFailureCounter = 0;
    private Timer detectedInformationTimer = null;
    private boolean isUserPressedSwitch = false;
    final Messenger mMessenger;
    final Handler mHandler;

    private final static String[] REQUIRE_PERMISSIONS = {
            permission.READ_EXTERNAL_STORAGE,
            permission.WRITE_EXTERNAL_STORAGE,
            permission.READ_PHONE_STATE,
            permission.RECORD_AUDIO,
            permission.ACCESS_COARSE_LOCATION,
            permission.ACCESS_FINE_LOCATION,
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");
        if (!PermissionActivity.checkAndRequestPermission(this, REQUIRE_PERMISSIONS)) {

            // Initialize UI.
            setContentView(R.layout.activity_user_main);

            // Start service.
            Intent intent = new Intent(this, com.qualcomm.qti.sva.VwuService.class);
            startService(intent);
            bindService(intent, mConnection, Context.BIND_AUTO_CREATE);

            if (TextUtils.isEmpty(SystemPropertiesWrapper.get(STAPP_IN_MENU))) {
                Log.v(TAG, "onCreate  STAPP_IN_MENU empty ");
                try {
                    SystemPropertiesWrapper.set(STAPP_IN_MENU, STAPP_DISABLED);
                } catch (RuntimeException e) {
                    Log.e(TAG, "onCreate  STAPP_IN_MENU RuntimeException : " + e.getMessage());
                }
            }

            // Get, save, and output version number.
            try {
                String versionNumber = this.getPackageManager().getPackageInfo(this.getPackageName(), 0).versionName;
                Global.getInstance().setVersionNumber(versionNumber);
                Log.v(TAG, "onCreate: version number= " + Global.getInstance().getVersionNumber());
            } catch (NameNotFoundException e) {
                Log.v(TAG, "onCreate: error getting version number: " + e.getMessage());
            }
        } else {
            finish();
        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_user_main, menu);
        return true;
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        super.onPrepareOptionsMenu(menu);

        boolean enableSTapp = false;
        enableSTapp = STAPP_ENABLED.equals(SystemPropertiesWrapper.get(STAPP_IN_MENU));

        final MenuItem stapp = menu.findItem(R.id.main_action_stapp);
        Log.v(TAG, "onPrepareOptionsMenu STAPP_IN_MENU enableSTapp=" + enableSTapp);
        stapp.setVisible(enableSTapp);

        return true;
    }


    // Handle action bar item clicks.
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.main_action_addkeyphrase) {
            if (isSvaOn()) {
                Utils.openAlertDialog(MainActivity.this, TAG, "Voice Activation",
                        "To create a new keyphase, please turn off voice activation.");
            } else {
                Intent intent = new Intent(MainActivity.this, AddKeyphraseActivity.class);
                startActivity(intent);
            }
            return true;
        } else if (id == R.id.main_action_tutorial) {
            Intent intent = new Intent(MainActivity.this, Tutorial1Activity.class);
            startActivity(intent);
            return true;
        } else if (id == R.id.main_action_version) {
            Utils.openAlertDialog(MainActivity.this, TAG, "Version Number",
                    Global.getInstance().getVersionNumber());
            return true;
        } else if (id == R.id.main_action_stapp) {
            if (isSvaOn()) {
                Utils.openAlertDialog(MainActivity.this, TAG, "Voice Activation",
                        "To use the ST App, please turn off voice activation.");
            } else {
                Intent intent = new Intent(MainActivity.this, DetectionActivity.class);
                startActivity(intent);
                finish();

                //clear the memory of soundModels to update the new added sound models
                Global.getInstance().getSmRepo().clearAllSoundModels();
            }
            return true;
        }
        return super.onOptionsItemSelected(item);
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

        if (null != keyphraseListAdapter) {
            Global.getInstance().getSmRepo().saveSelectedKeyphrases(MainActivity.this,
                    keyphraseListAdapter.getCheckedKeyphrases());
        }

        super.onPause();
    }

    // Updates the actionName in the UI when the user gets back from setting it.
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        Log.v(TAG, "onActivityResult");
        if (REQUEST_UPDATE_NOTIFICATION == requestCode) {
            if (RESULT_OK == resultCode) {
                Log.v(TAG, "onActivityResult: action selected so update action.");
                Bundle extras = intent.getExtras();
                String keyphraseName = null;
                String keyphraseAction = null;
                if (null != extras) {
                    // A keyphrase was deleted so update the list.
                    if (extras.containsKey(Global.EXTRA_ACTION_DELETE_KEYPHRASE)) {
                        if (extras.containsKey(Global.EXTRA_DATA_KEYPHRASE_NAME)) {
                            keyphraseName = extras.getString(Global.EXTRA_DATA_KEYPHRASE_NAME);
                            Log.v(TAG, "onActivityResult: intent extra EXTRA_KEYPHRASE_NAME= " + keyphraseName);
                        }
                        keyphraseListAdapter.deleteKeyphrase(keyphraseName);
                        keyphraseListAdapter.notifyDataSetChanged();
                    }

                    // A keyphrase action was updated so update it in the list.
                    if (extras.containsKey(Global.EXTRA_ACTION_UPDATE_KEYPHRASEACTION)) {
                        if (extras.containsKey(Global.EXTRA_DATA_KEYPHRASE_NAME)) {
                            keyphraseName = extras.getString(Global.EXTRA_DATA_KEYPHRASE_NAME);
                            Log.v(TAG, "onActivityResult: intent extra EXTRA_KEYPHRASE_NAME= " + keyphraseName);
                        }
                        if (extras.containsKey(Global.EXTRA_DATA_KEYPHRASE_ACTION)) {
                            keyphraseAction = extras.getString(Global.EXTRA_DATA_KEYPHRASE_ACTION);
                            Log.v(TAG, "onActivityResult: intent extra EXTRA_KEYPHRASE_ACTION= " + keyphraseAction);
                        }
                        if (null == keyphraseName || null == keyphraseAction) {
                            Log.e(TAG, "onActivityResult: intent extra did not contain keyphraseName and action.");
                            return;
                        }
                        keyphraseListAdapter.setKeyphraseAction(keyphraseName, keyphraseAction);
                        keyphraseListAdapter.notifyDataSetChanged();
                    }
                }
            } else {
                Log.v(TAG, "onActivityResult: no need to update since action wasn't changed.");
            }
            return;
        }
        super.onActivityResult(requestCode, resultCode, intent);
    }

    private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");
        uiRlInstructionalText = (RelativeLayout) findViewById(R.id.main_rl_instructionaltext);
        uiLlDetection = (LinearLayout) findViewById(R.id.main_ll_detection);
        uiTvSvaOnOff = (TextView) findViewById(R.id.main_tv_onoff);
        uiSwSvaOnOff = (Switch) findViewById(R.id.main_sw_onoff);
        uiTvInstructionaltext = (TextView) findViewById(R.id.main_tv_instructionaltext);
        uiTvKeyphraseDetected = (TextView) findViewById(R.id.main_tv_keyphrasedetected);
        uiTvKeyphraseDetectedAction = (TextView) findViewById(R.id.main_tv_keyphrasedetectedaction);
        uiLvKeyphrases = (ListView) findViewById(R.id.main_list_keyphrases);

        fillKeyphraseListAdapter();
        keyphraseListAdapter = new KeyphraseListAdapter(this, keyphraseList,
                onItemLeftActionListener, onItemRightActionListener);
        uiLvKeyphrases.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        uiLvKeyphrases.setAdapter(keyphraseListAdapter);

        uiSwSvaOnOff.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                Log.v(TAG, "uiSvaOnOff: isChecked = " + uiSwSvaOnOff.isChecked());
                // Disable switch while SVA is turning on/off.
                uiSwSvaOnOff.setEnabled(false);
                // Undo switch changed state of switch so that its checked state only changes upon
                // success returned.
                uiSwSvaOnOff.setChecked(!(uiSwSvaOnOff.isChecked()));
                // Set boolean that signals the user has pressed the switch. This is used to know
                // when to turn the switch off after an UNLOAD of a SM.
                isUserPressedSwitch = true;
                if (uiSwSvaOnOff.isChecked() == false) {
                    Log.v(TAG, "uiSvaOnOff: clicked when not checked (SVA was off).");
                    turnSvaOnOrOff(true);
                } else {
                    Log.v(TAG, "uiSvaOnOff: clicked when checked (SVA was on).");
                    turnSvaOnOrOff(false);
                }
            }
        });
    }

    private void turnSvaOnOrOff(boolean isUserTurningSwitchOn) {
        ArrayList<String> checkedKeyphrasesNames = keyphraseListAdapter.getCheckedKeyphrases();
        // If no selected keyphrases, just complete user action of turning the switch on or off.
        int number = 0;
        if (checkedKeyphrasesNames.size() == 0) {
            Log.v(TAG, "turnSvaOnOrOff: checkedKeyphrassNames.size() == 0");
            updateUiSvaOnorOff(!uiSwSvaOnOff.isChecked());
            return;
        }

        for (String keyphraseName : checkedKeyphrasesNames) {
            boolean isSendSuccessful = sendStartOrStopToService(isUserTurningSwitchOn, keyphraseName);
            if (isSendSuccessful == false) {
                Log.v(TAG, "turnSvaOnOrOff: error sending start or stop. Finishing without selecting" +
                        " keyphrase.");
                break;
            } else {
                number++;
            }

            if (isUserTurningSwitchOn) {
                enablingKeyphrasesCounter++;
                numKeyphrasesChecked++;
            } else {
                disablingKeyphrasesCounter++;
            }

            Log.v(TAG, "turnSvaOnOrOff: num=" + numKeyphrasesChecked + " number=" + number
                    + " max=" + Global.getInstance().getMaxActiveSoundModels()
                    + " =" + keyphraseName);
            if (number >= Global.getInstance().getMaxActiveSoundModels()) {
                Log.v(TAG, "turnSvaOnOrOff: reach max num =" + numKeyphrasesChecked);
                break;
            }
        }

        if (isUserTurningSwitchOn) {
            disablingKeyphrasesCounter = 0;
        } else {
            numKeyphrasesChecked = 0;
            enablingKeyphrasesCounter = 0;
        }

    }

    // Sends a message to the service to start or stop detection for the
    // provided keyphrase.
    // isTurningOnDetection is true for start, false for stop.
    // @return true for success, false otherwise.
    private boolean sendStartOrStopToService(boolean isTurningOnDetection, String inKeyphraseName) {
        int message;
        if (isTurningOnDetection) {
            message = MessageType.MSG_LOAD_COM_SM;
        } else {
            message = MessageType.MSG_STOP_RECOGNITION;
        }
        String smNameToSend = Global.getInstance().getSmRepo().
                getComSoundModelNameToSendFromKeyphrase(inKeyphraseName);
        if (null == smNameToSend) {
            Log.e(TAG, "sendStartOrStopToService: could not find CSM containing keyphrase= " +
                    inKeyphraseName);
            return false;
        }
        sendReply(message, smNameToSend);
        return true;
    }

    private void updateUiSvaOnorOff(boolean isSvaOn) {
        Log.v(TAG, "updateUiSvaOnorOff");
        uiSwSvaOnOff.setEnabled(true);
        uiSwSvaOnOff.setChecked(isSvaOn);
        if (isSvaOn()) {
            uiTvSvaOnOff.setText(R.string.on);
            uiTvInstructionaltext.setText(R.string.user_main_instructionaltext_on);
            Log.v(TAG, "updateUiSvaOnorOff: updated text to on.");
        } else {
            uiTvSvaOnOff.setText(R.string.off);
            uiTvInstructionaltext.setText(R.string.user_main_instructionaltext_off);
            Log.v(TAG, "updateUiSvaOnorOff: updated text to off.");
        }
    }

    private boolean isSvaOn() {
        Log.v(TAG, "isSvaOn= " + uiSwSvaOnOff.isChecked());
        return uiSwSvaOnOff.isChecked();
    }

    private void fillKeyphraseListAdapter() {
        Log.v(TAG, "fillKeyphraseListAdapter");

        keyphraseList.clear();
        Log.v(TAG, "fillKeyphraseListAdapter: list cleared. keyphraseList.size= " +
                keyphraseList.size());

        LinkedHashSet<Keyphrase> keyphrases =
                Global.getInstance().getSmRepo().getAllComKeyphrases(getApplicationContext());

        ArrayList<String> selectedKeyphrases = Global.getInstance().getSmRepo().
                getSelectedKeyphrases(MainActivity.this);

        for (Keyphrase keyphrase : keyphrases) {
            boolean checked = selectedKeyphrases == null ? false :
                    selectedKeyphrases.contains(keyphrase.getName());
            Log.v(TAG, "fillKeyphraseListAdapter: keyphraseName= " + keyphrase.getName() +
                    ", checked= " + checked + ", keyphraseActionName= " + keyphrase.getActionName());
            KeyphraseListItem listItem = new KeyphraseListAdapter.KeyphraseListItem(
                    checked, keyphrase.getName(), keyphrase.getActionName());
            keyphraseList.add(listItem);
        }

        Collections.sort(keyphraseList);
        return;
    }

    // Set behavior for when a keyphrase is clicked.
    private KeyphraseListAdapter.OnItemActionListener onItemLeftActionListener =
            new KeyphraseListAdapter.OnItemActionListener() {
                @Override
                public void onOptionClicked(RowViewHolder rowViewHolder) {
                    Log.v(TAG, "onItemLeftActionListener.onOptionClicked");

                    final boolean checked = !rowViewHolder.listItem.getChecked();

                    int maxSessionNumSupported = Global.getInstance().getMaxActiveSoundModels();
                    if (maxSessionNumSupported < 1) {
                        Log.e(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "maxSessionNumSupported < 1");
                        return;
                    }

                    // If SVA is off, just prompt user to turn SVA on and return.
                    if (false == isSvaOn()) {
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "checked and SVA is off. Prompt user to turn on SVA.");
                        showToast(getString(R.string.user_main_toast_turnonvatoselectkeyphrases),
                                Toast.LENGTH_LONG);
                        return;
                    }

                    Log.v(TAG, "onItemLeftActionListener num=" + numKeyphrasesChecked +
                            " maxSessionNumSupported=" + maxSessionNumSupported);
                    // Count the selected keyphrases so it doesn't exceed the max.
                    if (checked && numKeyphrasesChecked >= maxSessionNumSupported) {
                        // If the max # of keyphrases is already selected, just prompt the user and return.
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "checked and numKeyphrasesChecked == getMaxActiveSoundModels. " +
                                "numKeyphrasesChecked = " + numKeyphrasesChecked +
                                ", maxSessionNumSupported = " + maxSessionNumSupported);
                        showToast(getString(R.string.user_main_toast_max_keyphrase_warning),
                                Toast.LENGTH_LONG);
                        return;
                    } else if (checked && numKeyphrasesChecked < maxSessionNumSupported) {
                        // If under max # of selected keyphrases, add one keyphrase.
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "checked and numKeyphrasesChecked < getMaxActiveSoundModels. " +
                                "numKeyphrasesChecked = " + numKeyphrasesChecked +
                                ", maxSessionNumSupported = " + maxSessionNumSupported);
                        numKeyphrasesChecked++;
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "checked. numKeyphrasesChecked= " + numKeyphrasesChecked);
                    } else if (checked == false) {
                        // Remove one keyphrase.
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "unchecked");
                        numKeyphrasesChecked--;
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "unchecked. numKeyphrasesChecked= " + numKeyphrasesChecked);
                    }

                    String keyphraseName = rowViewHolder.listItem.getKeyphraseName();
                    boolean isSendSuccessful = sendStartOrStopToService(checked, keyphraseName);
                    if (isSendSuccessful == false) {
                        Log.v(TAG, "getView: error sending start or stop. Finishing without selecting" +
                                " keyphrase.");
                        return;
                    }

                    keyphraseListAdapter.setKeyphraseChecked(rowViewHolder, checked);
                    if (checked) {
                        // The user has just checked the checkbox, so start recognition for the selected keyphrase.
                        enablingKeyphrasesCounter++;
                    } else {
                        // The user has just unchecked the checkbox, so stop recognition for the selected keyphrase.
                        disablingKeyphrasesCounter++;
                    }
                }
            };

    // Opens the EditKeyphrase activity for the selected keyphrase
    private KeyphraseListAdapter.OnItemActionListener onItemRightActionListener =
            new KeyphraseListAdapter.OnItemActionListener() {
                @Override
                public void onOptionClicked(RowViewHolder rowViewHolder) {
                    Log.v(TAG, "onItemRightActionListener.onOptionClicked");
                    if (rowViewHolder.uiCheckBox.isChecked()) {
                        showToast(getString(R.string.user_main_toast_turnkeyphraseoffbeforeediting),
                                Toast.LENGTH_LONG);
                        return;
                    }
                    // Open the EditKeyphrase activity to edit the keyphrase.
                    Intent intent = new Intent(MainActivity.this, EditKeyphraseActivity.class);
                    intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, rowViewHolder.listItem.getKeyphraseName());
                    intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_ACTION, rowViewHolder.listItem.getAction());
                    startActivityForResult(intent, REQUEST_UPDATE_NOTIFICATION);
                }
            };

    public void showToast(String text, int duration) {
        Toast.makeText(getApplicationContext(), text, duration).show();
    }

    protected void updateUiWithDetection() {
        startDetectedInformationTimer();

        // Output detection data.
        DetectionContainer detectionContainer = Global.getInstance().getDetectionContainer();
        if (null == detectionContainer) {
            Utils.openAlertDialog(MainActivity.this, TAG, "Error", "Received event but no detection data");
            Log.e(TAG, "updateUiWithDetection: getDetectionContainer return null");
            return;
        }
        int sessionNum = detectionContainer.sessionNum;
        String smName = detectionContainer.smName;
        VoiceWakeupDetectionDataV2 vwuDetectionData = detectionContainer.vwuDetectionData;
        boolean detectSuccessEvent = (ListenTypes.STATUS_SUCCESS == vwuDetectionData.status);
        Log.v(TAG, "   vwuDetectionData detection status= " + vwuDetectionData.status);
        Log.v(TAG, "   vwuDetectionData sessionNum= " + sessionNum);
        Log.v(TAG, "   vwuDetectionData SM name= " + smName);
        if (detectSuccessEvent) {
            Log.i(TAG, "updateAdvancedDetail: detectionSuccessCounter= " + ++detectionSuccessCounter);
        } else {
            Log.i(TAG, "updateAdvancedDetail: detectionFailureCounter= " + ++detectionFailureCounter);
            return;
        }
        // Output keyword conf levels.
        Log.v(TAG, "vwuDetectionData.nonzeroKWConfLevels.length= " +
                vwuDetectionData.nonzeroKWConfLevels.length);
        String keyphraseName = null;
        for (VWUKeywordConfLevel kwConfLevel : vwuDetectionData.nonzeroKWConfLevels) {
            keyphraseName = kwConfLevel.keyword;
            Log.v(TAG, "kwConfLevel.keyword= " + kwConfLevel.keyword + ", with " +
                    "kwConfLevel.confLevel= " + kwConfLevel.confLevel);
        }
        // Output user conf levels.
        Log.v(TAG, "vwuDetectionData.nonzeroUserKWPairConfLevels.length= " +
                vwuDetectionData.nonzeroUserKWPairConfLevels.length);
        for (VWUUserKeywordPairConfLevel pairConfLevel : vwuDetectionData.nonzeroUserKWPairConfLevels) {
            Log.v(TAG, "pairConfLevel.keyword|pairConfLevel.user= " + pairConfLevel.keyword + "|" +
                    pairConfLevel.user + " with pairConfLevel.confLevel= " + pairConfLevel.confLevel);
        }

        // Update UI
        uiRlInstructionalText.setVisibility(View.GONE);
        uiLlDetection.setVisibility(View.VISIBLE);
        uiTvKeyphraseDetected.setText(keyphraseName);
        uiTvKeyphraseDetectedAction.setText(Global.getInstance().getSmRepo().
                getKeyphraseActionName(keyphraseName));
    }

    // Resets keyword and user detection bars after SHOW_DETECTION_RESULT_LENGTH milliseconds
    private void startDetectedInformationTimer() {
        Log.v(TAG, "startDetectedInformationTimer");
        stopDetectedInformationTimer();

        Log.v(TAG, "startDetectedInformationTimer: starting new timer");
        detectedInformationTimer = new Timer();
        TimerTask timerTask = new TimerTask() {
            @Override
            public void run() {
                uiHandler.sendEmptyMessage(MSG_RESET_RESULT_VALUES);
            }
        };
        detectedInformationTimer.schedule(timerTask, SHOW_DETECTION_RESULT_LENGTH);
    }

    private void stopDetectedInformationTimer() {
        Log.v(TAG, "stopDetectedInformationTimer: stopping previous timer");
        if (null != detectedInformationTimer) {
            detectedInformationTimer.cancel();
            detectedInformationTimer = null;
        }
    }

    // Resets the screen to the default listening state. Called when the detection timer times out.
    private Handler uiHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (MSG_RESET_RESULT_VALUES == msg.what) {
                Log.v(TAG, "uiHandler: MSG_RESET_RESULT_VALUES");

                uiLlDetection.setVisibility(View.GONE);
                uiRlInstructionalText.setVisibility(View.VISIBLE);
            } else {
                Log.e(TAG, "uiHandler  unexpected msg=" + msg.what);
            }
        }

        ;
    };

    public MainActivity() {
        // Handles messages coming from the Service
        /** The msg handler. */
        mHandler = new Handler() {
            public void handleMessage(Message msg) {
                Log.v(TAG, "handleMessage");
                switch (msg.what) {
                    case MessageType.MSG_ATTACH_MODULE:
                        Log.v(TAG, "MSG_ATTACH_MODULE");
                        if (msg.arg1 != Global.SUCCESS) {
                            Log.e(TAG, "MSG_ATTACH_MODULE: Attach module failed.");
                            Utils.openAlertDialog(MainActivity.this, TAG, null, "Attach Module Failed");
                        }
                        break;

                    case MessageType.MSG_LOAD_SM:
                    case MessageType.MSG_LOAD_COM_SM:
                        Log.v(TAG, "MSG_LOAD_SM: msg.arg1= " + msg.arg1);
                        // If the load failed, tell the user, disable the on/off switch, and set
                        // the on/off switch to off.
                        if (msg.arg1 != Global.SUCCESS) {
                            Log.v(TAG, "MSG_LOAD_SM: load failed.");
                            uiSwSvaOnOff.setEnabled(false);
                            uiSwSvaOnOff.setChecked(false);
                            Utils.openAlertDialog(MainActivity.this, TAG, null, "Turning on SVA failed.");
                        }
                        break;

                    case MessageType.MSG_START_RECOGNITION:
                        Log.v(TAG, "MSG_START_RECOGNITION: msg.arg1= " + msg.arg1);
                        // If the start failed, tell the user, disable the on/off switch, and set
                        // the on/off switch to off.
                        if (msg.arg1 != Global.SUCCESS) {
                            Log.v(TAG, "MSG_START_RECOGNITION: start failed.");
                            uiSwSvaOnOff.setEnabled(false);
                            uiSwSvaOnOff.setChecked(false);
                            Utils.openAlertDialog(MainActivity.this, TAG, null, "Turning on SVA failed.");
                            return;
                        }

                        if (enablingKeyphrasesCounter > 0) {
                            --enablingKeyphrasesCounter;
                        }
                        if (enablingKeyphrasesCounter == 0) {
                            Log.v(TAG, "MSG_START_RECOGNITION: start succeeded.");
                            // Only enable the switch and turn it on when all keyphrases have been
                            // successfully started.
                            // This will also be executed when a keyphrase is selected when the
                            // switch is already on but won't have any detrimental side effects since
                            // it just puts the switch in the same state it's already in.
                            updateUiSvaOnorOff(true);

                            // Turn back off now that the switch has been set.
                            isUserPressedSwitch = false;
                        }
                        break;

                    case MessageType.MSG_STOP_RECOGNITION:
                        Log.v(TAG, "MSG_STOP_RECOGNITION: msg.arg1= " + msg.arg1);
                        // If the stop failed, tell the user, disable the on/off switch, and set
                        // the on/off switch to on.
                        if (msg.arg1 != Global.SUCCESS) {
                            Log.v(TAG, "MSG_STOP_RECOGNITION: stop failed.");
                            uiSwSvaOnOff.setEnabled(false);
                            uiSwSvaOnOff.setChecked(true);
                            Utils.openAlertDialog(MainActivity.this, TAG, null, "Stopping SVA failed.");
                        }
                        break;

                    case MessageType.MSG_UNLOAD_SM:
                        Log.v(TAG, "MSG_UNLOAD_SM: msg.arg1= " + msg.arg1);
                        // If the unload failed, tell the user, disable the on/off switch, and set
                        // the on/off switch to on.
                        if (msg.arg1 != Global.SUCCESS) {
                            Log.v(TAG, "MSG_UNLOAD_SM: unload failed.");
                            uiSwSvaOnOff.setEnabled(false);
                            uiSwSvaOnOff.setChecked(true);
                            Utils.openAlertDialog(MainActivity.this, TAG, null, "Stopping SVA failed.");
                            return;
                        }
                        if (disablingKeyphrasesCounter > 0) {
                            --disablingKeyphrasesCounter;
                        }
                        if (disablingKeyphrasesCounter == 0) {
                            Log.v(TAG, "MSG_UNLOAD_SM: unload succeeded.");
                            // Only enable the switch and turn it off when all keyphrases have been
                            // successfully unloaded.
                            // This will also be executed when a keyphrase is unselected but won't
                            // have any detrimental side effects since it just puts the switch in
                            // the same state it's already in.

                            if (isUserPressedSwitch) {
                                // only if the user pressed the switch to turn off SVA should we
                                // turn off SVA. Otherwise, the switch would be turned off whenever
                                // the 1 selected keyphrase gets unselected.
                                updateUiSvaOnorOff(false);
                            }

                            // Turn back off now that the switch has been set.
                            isUserPressedSwitch = false;
                        }
                        break;

                    case MessageType.MSG_DETECT_SUCCEEDED:
                        Log.v(TAG, "MSG_DETECT_SUCCEEDED");
                        if (isSvaOn()) {
                            updateUiWithDetection();
                        }
                        break;

                    case MessageType.MSG_TAKE_ACTION:
                        Log.v(TAG, "MSG_TAKE_ACTION");
                        String keyphraseName = null;
                        if (null != msg.obj) {
                            keyphraseName = (String) msg.obj;
                            Log.v(TAG, "MSG_TAKE_ACTION keyphraseName=" + keyphraseName);
                        }
                        if (null != keyphraseName) {
                            Intent intentToLaunch = Global.getInstance().getSmRepo().
                                    getKeyphraseActionIntent(getApplicationContext(), keyphraseName);
                            if (null != intentToLaunch) {
                                int flag = intentToLaunch.getFlags();
                                intentToLaunch.setFlags(flag & ~Intent.FLAG_ACTIVITY_NEW_TASK);
                                startActivity(intentToLaunch);
                            }
                        }
                        break;

                    default:
                        Log.v(TAG, "handleMessage: no such case: " + msg.what);
                        break;
                }
            }
        };
        mMessenger = new Messenger(mHandler);
    }

    @Override
    protected Messenger getMMessenger() {
        return mMessenger;
    }

    @Override
    protected void activityOnServiceConnected() {
        initializeUserInterface();
        super.activityOnServiceConnected();
    }
}

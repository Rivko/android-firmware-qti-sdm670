/*
 *Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.networksetting.uplmn;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.text.InputType;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.EditText;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.provider.Settings;
import android.os.Bundle;
import android.util.Log;

import com.android.internal.telephony.TelephonyIntents;
import com.qualcomm.qti.networksetting.R;

import java.util.List;

public class UPLMNEditor extends PreferenceActivity implements
        Preference.OnPreferenceChangeListener, TextWatcher {
    private static final String LOG_TAG = "UPLMNEditor";
    private static final int MENU_DELETE_OPTION = Menu.FIRST;
    private static final int MENU_SAVE_OPTION = Menu.FIRST + 1;
    private static final int MENU_CANCEL_OPTION = Menu.FIRST + 2;
    private static final int NWID_DIALOG_ID = 0;

    private static final String BUTTON_NETWORK_ID_KEY = "network_id_key";
    private static final String BUTTON_PRIORITY_KEY = "priority_key";
    private static final String BUTTON_NEWWORK_MODE_KEY = "network_mode_key";

    public static final String UPLMN_CODE = "uplmn_code";
    public static final String UPLMN_PRIORITY = "uplmn_priority";
    public static final String UPLMN_SERVICE = "uplmn_service";
    public static final String UPLMN_SIZE = "uplmn_size";
    public static final String UPLMN_ADD = "uplmn_add";

    public static final int RESULT_CODE_EDIT = 101;
    public static final int RESULT_CODE_DELETE = 102;

    private static final int GSM = 0;
    private static final int WCDMA_TDSCDMA = 1;
    private static final int LTE = 2;
    private static final int TRIPLE_MODE = 3;
    private static final int MODE_2G = 0x1;
    private static final int MODE_3G = 0x4;
    private static final int MODE_LTE = 0x8;
    private static final int MODE_TRIPLE = 0xd;

    private String mNoSet = null;
    private boolean mAirplaneModeOn = false;
    private IntentFilter mIntentFilter;
    private Preference mNWIDPref = null;
    private EditTextPreference mPRIpref = null;
    private ListPreference mNWMPref = null;
    private EditText mNWIDText;
    private AlertDialog mNWIDDialog = null;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (Intent.ACTION_AIRPLANE_MODE_CHANGED.equals(action)) {
                mAirplaneModeOn = intent.getBooleanExtra("state", false);
                setScreenEnabled();
            }
        }
    };

    private OnClickListener mNWIDPrefListener = new OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                String summary = genText(mNWIDText.getText().toString());
                Log.d(LOG_TAG, "input network id is " + summary);
                mNWIDPref.setSummary(summary);
            }
        }
    };

    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        addPreferencesFromResource(R.xml.uplmn_editor);
        mNoSet = getResources().getString(R.string.voicemail_number_not_set);

        mNWIDPref = (Preference) findPreference(BUTTON_NETWORK_ID_KEY);
        mPRIpref = (EditTextPreference) findPreference(BUTTON_PRIORITY_KEY);
        mNWMPref = (ListPreference) findPreference(BUTTON_NEWWORK_MODE_KEY);

        mPRIpref.setOnPreferenceChangeListener(this);
        mNWMPref.setOnPreferenceChangeListener(this);

        mIntentFilter = new IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        registerReceiver(mReceiver, mIntentFilter);
    }

    protected void onResume() {
        super.onResume();
        displayNetworkInfo(getIntent());
        mAirplaneModeOn =
            Settings.System.getInt(getContentResolver(), Settings.Global.AIRPLANE_MODE_ON, 0) == 1;
        setScreenEnabled();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    public boolean onPreferenceChange(Preference preference, Object object) {
        String value = object.toString();
        if (preference == mPRIpref) {
            mPRIpref.setSummary(genText(value));
        } else if (preference == mNWMPref) {
            mNWMPref.setValue(value);
            String summary = "";
            int index = Integer.parseInt(value);
            summary = getResources().getStringArray(
                    selectNetworkChoices(mNWIDPref.getSummary().toString()))[index];
            mNWMPref.setSummary(summary);
        }
        return true;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        if (!getIntent().getBooleanExtra(UPLMN_ADD, false)) {
            menu.add(0, MENU_DELETE_OPTION, 0,
                    com.android.internal.R.string.delete);
        }
        menu.add(0, MENU_SAVE_OPTION, 0, R.string.save);
        menu.add(0, MENU_CANCEL_OPTION, 0, com.android.internal.R.string.cancel);
        return true;
    }

    @Override
    public boolean onMenuOpened(int featureId, Menu menu) {
        super.onMenuOpened(featureId, menu);
        boolean isEmpty = mNoSet.equals(mNWIDPref.getSummary())
                || mNoSet.equals(mPRIpref.getSummary());
        if (menu != null) {
            menu.setGroupEnabled(0, !mAirplaneModeOn);
            //only show the save and delete option menu when radio on
            //and edit text is not empty.
            if (getIntent().getBooleanExtra(UPLMN_ADD, true)) {
                menu.getItem(0).setEnabled((!mAirplaneModeOn) && !isEmpty);
            } else {
                menu.getItem(1).setEnabled((!mAirplaneModeOn) && !isEmpty);
            }
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case MENU_SAVE_OPTION:
            setSavedNWInfo();
            break;
        case MENU_DELETE_OPTION:
            setRemovedNWInfo();
            break;
        case MENU_CANCEL_OPTION:
            break;
        case android.R.id.home:
            finish();
            return true;
        default:
            break;
        }
        finish();
        return super.onOptionsItemSelected(item);
    }

    private void setSavedNWInfo() {
        Intent intent = new Intent(this, UserPLMNListPreference.class);
        genNWInfoToIntent(intent);
        setResult(RESULT_CODE_EDIT, intent);
    }

    private void genNWInfoToIntent(Intent intent) {
        int priority = 0;
        int plmnListSize = getIntent().getIntExtra(UPLMN_SIZE, 0);
        try {
            priority = Integer.parseInt(String.valueOf(mPRIpref.getSummary()));
        } catch (NumberFormatException e) {
            Log.d(LOG_TAG, "parse value of basband error");
        }
        if (getIntent().getBooleanExtra(UPLMN_ADD, false)) {
            if (priority > plmnListSize) {
                priority = plmnListSize;
            }
        } else {
            if (priority >= plmnListSize) {
                priority = plmnListSize - 1;
            }
        }
        intent.putExtra(UPLMNEditor.UPLMN_PRIORITY, priority);

        try {
            intent.putExtra(UPLMNEditor.UPLMN_SERVICE, convertApMode2EF(Integer
                    .parseInt(String.valueOf(mNWMPref.getValue()))));
        } catch (NumberFormatException e) {
            intent.putExtra(UPLMNEditor.UPLMN_SERVICE, convertApMode2EF(0));
        }

        intent.putExtra(UPLMNEditor.UPLMN_CODE, mNWIDPref.getSummary());
    }

    private void setRemovedNWInfo() {
        Intent intent = new Intent(this, UserPLMNListPreference.class);
        genNWInfoToIntent(intent);
        setResult(RESULT_CODE_DELETE, intent);
    }

    public static int convertEFMode2Ap(int mode) {
        int result = 0;
        if (mode == MODE_TRIPLE) {
            result = TRIPLE_MODE;
        } else if (mode == MODE_3G) {
            result = WCDMA_TDSCDMA;
        } else if (mode == MODE_LTE) {
            result = LTE;
        } else {
            result = GSM;
        }
        return result;
    }

    public static int convertApMode2EF(int mode) {
        int result = 0;
        if (mode == TRIPLE_MODE) {
            result = MODE_TRIPLE;
        } else if (mode == LTE) {
            result = MODE_LTE;
        } else if (mode == WCDMA_TDSCDMA) {
            result = MODE_3G;
        } else {
            result = MODE_2G;
        }
        return result;
    }

    private void displayNetworkInfo(Intent intent) {
        String number = intent.getStringExtra(UPLMN_CODE);
        mNWIDPref.setSummary(genText(number));
        int priority = intent.getIntExtra(UPLMN_PRIORITY, 0);
        mPRIpref.setSummary(String.valueOf(priority));
        mPRIpref.setText(String.valueOf(priority));
        int act = intent.getIntExtra(UPLMN_SERVICE, 0);

        Log.d(LOG_TAG, "act = " + act);

        act = convertEFMode2Ap(act);
        if (act < GSM || act > TRIPLE_MODE) {
            act = GSM;
        }
        String summary = "";
        mNWMPref.setEntries(getResources().getTextArray(
                selectNetworkChoices(number)));
        summary = getResources().getStringArray(
                selectNetworkChoices(number))[act];
        mNWMPref.setSummary(summary);
        mNWMPref.setValue(String.valueOf(act));
    }

    public int selectNetworkChoices(String plmn) {
        Log.d(LOG_TAG, "plmn = " + plmn);
        String[] CuPlmnArray = getResources().getStringArray(R.array.uplmn_cu_mcc_mnc_values);
        for (String CuPlmn : CuPlmnArray) {
            if (plmn.equals(CuPlmn)) return R.array.uplmn_prefer_network_mode_w_choices;
        }
        return R.array.uplmn_prefer_network_mode_td_choices;
    }

    private String genText(String value) {
        if (value == null || value.length() == 0) {
            return mNoSet;
        } else {
            return value;
        }
    }

    public void buttonEnabled() {
        int len = mNWIDText.getText().toString().length();
        boolean state = true;
        if (len <5 || len > 6) {
            state = false;
        }
        if (mNWIDDialog != null) {
            mNWIDDialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(
                    state);
        }
    }

    private void setScreenEnabled() {
        getPreferenceScreen().setEnabled(!mAirplaneModeOn);
        invalidateOptionsMenu();
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen screen,
            Preference preference) {
        if (preference == mNWIDPref) {
            removeDialog(NWID_DIALOG_ID);
            showDialog(NWID_DIALOG_ID);
            buttonEnabled();
        }
        return super.onPreferenceTreeClick(screen, preference);
    }

    @Override
    public Dialog onCreateDialog(int id) {
        if (id == NWID_DIALOG_ID) {
            mNWIDText = new EditText(this);
            if (!mNoSet.equals(mNWIDPref.getSummary())) {
                mNWIDText.setText(mNWIDPref.getSummary());
            }
            mNWIDText.addTextChangedListener(this);
            mNWIDText.setInputType(InputType.TYPE_CLASS_NUMBER);
            mNWIDDialog = new AlertDialog.Builder(this)
                    .setTitle(getResources().getString(R.string.network_id))
                    .setView(mNWIDText)
                    .setPositiveButton(
                            getResources().getString(
                                    com.android.internal.R.string.ok),
                            mNWIDPrefListener)
                    .setNegativeButton(
                            getResources().getString(
                                    com.android.internal.R.string.cancel), null)
                    .create();
            mNWIDDialog.getWindow().setSoftInputMode(
                    WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE);
            return mNWIDDialog;
        }
        return null;
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count,
            int after) {
    }

    @Override
    public void afterTextChanged(Editable s) {
        buttonEnabled();
    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
    }

}

/*
 *Copyright (c) 2016 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *Copyright (c) 2013-2014, The Linux Foundation. All Rights Reserved.
 *
 *Not a Contribution.
 *Apache license notifications and license are retained
 *for attribution purposes only.
 *
 *  Copyright (C) 2008 The Android Open Source Project

 *  Licensed under the Apache License, Version 2.0 (the "License");
 *you may not use this file except in compliance with the License.
 *You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *See the License for the specific language governing permissions and
 *limitations under the License.
 */

package com.qualcomm.qti.phonefeature.callbarring;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PersistableBundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.Toast;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.CommandsInterface;
import com.qualcomm.qti.phonefeature.R;

public class CallBarring extends PreferenceActivity implements DialogInterface.OnClickListener,
        EditPinPreference.OnPinEnteredListener {
    private static final String  LOG_TAG = "CallBarring";
    private static final boolean DBG = true;

    private static final String  CALL_BARRING_OUTGOING_KEY = "call_barring_outgoing_key";
    private static final String  CALL_BARRING_INCOMING_KEY = "call_barring_incoming_key";
    private static final String  CALL_BARRING_CANCEL_ALL_KEY = "call_barring_cancel_all_key";
    private static final String  CALL_BARRING_CHANGE_PSW_KEY = "call_barring_change_psw_key";

    private static final String  SETOUTGOING_KEY = "SETOUTGOING_KEY";
    private static final String  SETINCOMING_KEY = "SETINCOMING_KEY";
    private static final String  DIALOGSTATE_KEY = "DIALOGSTATE_KEY";
    private static final String  CBDATASTALE_KEY = "CBDATASTALE_KEY";
    private static final String  ISBUSYDIALOGAVAILABLE_KEY = "ISBUSYDIALOGAVAILABLE_KEY";
    private static final String  PASSWORD_KEY = "PASSWORD_KEY";
    private static final String  NEW_PSW_KEY = "NEW_PSW_KEY";
    private static final String  ERROR_KEY = "ERROR_KEY";

    private static final int MIN_PSW_LENGTH = 4;
    private static final int MAX_PSW_LENGTH = 8;

    // call barring facility
    private static final int CB_STATE_OFF = 0;
    private static final int CB_BAOC = (1 << 0);
    private static final int CB_BAOIC = (1 << 1);
    private static final int CB_BAOICxH = (1 << 2);
    private static final int CB_BAIC = (1 << 3);
    private static final int CB_BAICr = (1 << 4);
    private static final int CB_BA_ALL = (1 << 5);
    private static final int CB_INVALID = -1;
    private static final int CB_OUTGOING_COUNT = 3;
    private static final int CB_INCOMING_COUNT = 2;
    private static final int CB_INCOMING_SHIFT = CB_OUTGOING_COUNT;
    private static final int CB_OUTGOING_BOUNDARY = CB_OUTGOING_COUNT;
    private static final int CB_INCOMING_BOUNDARY = CB_OUTGOING_BOUNDARY + CB_INCOMING_COUNT;

    private static final int EVENT_CB_QUERY_ALL = 100;
    private static final int EVENT_CB_CANCEL_QUERY = 200;
    private static final int EVENT_CB_CANCEL_ALL = 300;
    private static final int EVENT_CB_SET_COMPLETE = 400;
    private static final int EVENT_CB_CHANGE_PSW = 500;
    private static final int EVENT_CB_SET_OUTGOING_SERIALLY = 600;
    private static final int EVENT_CB_SET_INCOMING_SERIALLY = 700;

    /* Class bit for voice service as per
     * Spec 27.007 sec 7.4 */
    private static final int VOICE_SERVICE = 0x00000001;

    // dialog id for create
    private static final int BUSY_DIALOG = 100;
    private static final int EXCEPTION_ERROR = 200;
    private static final int RESPONSE_ERROR = 300;
    private static final int RADIO_OFF_ERROR = 400;
    private static final int INITIAL_BUSY_DIALOG = 500;
    private static final int INPUT_PSW_DIALOG = 600;
    private static final int IMS_UT_REQUEST = 700;
    private static final int IMS_UT_DATA_ROAMING_REQUEST = 800;
    private static final int FDN_CHECK_FAILURE = 900;
    private static final int OUTGOING_SET_DIALOG = 1000;
    private static final int INCOMING_SET_DIALOG = 1100;

    // status message sent back from handlers
    private static final int MSG_OK = 100;
    private static final int MSG_EXCEPTION = 200;
    private static final int MSG_UNEXPECTED_RESPONSE = 300;
    private static final int MSG_RADIO_OFF = 400;

    private static final int OFF_MODE = 0;
    private static final int CB_OUTGOING_MODE = 1;
    private static final int CB_INCOMING_MODE = 2;
    private static final int OLD_PSW_MODE = 3;
    private static final int NEW_PSW_MODE = 4;
    private static final int REENTER_PSW_MODE = 5;

    private int mOutgoingState = CB_STATE_OFF;
    private int mIncomingState = CB_STATE_OFF;
    private int mSetOutgoing = CB_INVALID;
    private int mSetIncoming = CB_INVALID;
    private int mOutSetShift;
    private int mInSetShift;
    private int mDialogState = OFF_MODE;
    private boolean mCBDataStale = true;
    private boolean mIsBusyDialogAvailable = false;
    private boolean mIsShowUTDialog = false;
    private String mPassword = null;
    private String mNewPsw = null;
    private String mError = null;

    private CallBarringHelper mCallBarringHelper;

    private PreferenceScreen  mListOutgoing = null;
    private PreferenceScreen  mListIncoming = null;
    private EditPinPreference mDialogCancelAll = null;
    private EditPinPreference mDialogChangePSW = null;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        if (DBG) log("onCreate");
        mCallBarringHelper = new CallBarringHelper(getIntent());
        // check the active data sub.
        int sub = mCallBarringHelper.getSubId();
        int defaultDataSub = SubscriptionManager.getDefaultDataSubscriptionId();
        if (mCallBarringHelper.isUtEnabled() && checkMobileDataForCf()) {
             int activeNetworkType = getActiveNetworkType();
             boolean isDataRoaming = mCallBarringHelper.isDataRoaming();
             boolean isDataRoamingEnabled = mCallBarringHelper.isDataRoamingEnabled();
             boolean promptForDataRoaming = isDataRoaming && !isDataRoamingEnabled;
             Log.d(LOG_TAG, "activeNetworkType = " + getActiveNetworkType() + ", sub = " + sub +
                     ", defaultDataSub = " + defaultDataSub + ", isDataRoaming = " +
                     isDataRoaming + ", isDataRoamingEnabled= " + isDataRoamingEnabled);
             if ((activeNetworkType != ConnectivityManager.TYPE_MOBILE
                     || sub != defaultDataSub)
                     && !(activeNetworkType == ConnectivityManager.TYPE_NONE
                     && promptForDataRoaming)) {
                 mIsShowUTDialog = true;
                 if (DBG) Log.d(LOG_TAG, "please open mobile network for UT settings!");
                 showDialog(IMS_UT_REQUEST);
             } else if (promptForDataRoaming) {
                 if (DBG) Log.d(LOG_TAG, "please open data roaming for UT settings!");
                 mIsShowUTDialog = true;
                 showDialog(IMS_UT_DATA_ROAMING_REQUEST);
            }
        }

        addPreferencesFromResource(R.xml.call_barring);
        PreferenceScreen prefSet = getPreferenceScreen();

        mListOutgoing = (PreferenceScreen) prefSet.findPreference(CALL_BARRING_OUTGOING_KEY);
        mListIncoming = (PreferenceScreen) prefSet.findPreference(CALL_BARRING_INCOMING_KEY);

        mDialogCancelAll = (EditPinPreference) prefSet.findPreference(CALL_BARRING_CANCEL_ALL_KEY);
        mDialogChangePSW = (EditPinPreference) prefSet.findPreference(CALL_BARRING_CHANGE_PSW_KEY);
        mDialogCancelAll.setOnPinEnteredListener(this);
        mDialogChangePSW.setOnPinEnteredListener(this);

        if (icicle != null) {
            mOutgoingState = icicle.getInt(CALL_BARRING_OUTGOING_KEY);
            mIncomingState = icicle.getInt(CALL_BARRING_INCOMING_KEY);
            mSetOutgoing = icicle.getInt(SETOUTGOING_KEY);
            mSetIncoming = icicle.getInt(SETINCOMING_KEY);
            mDialogState = icicle.getInt(DIALOGSTATE_KEY);
            mCBDataStale = icicle.getBoolean(CBDATASTALE_KEY);
            mIsBusyDialogAvailable = icicle.getBoolean(ISBUSYDIALOGAVAILABLE_KEY);
            mPassword = icicle.getString(PASSWORD_KEY);
            mNewPsw = icicle.getString(NEW_PSW_KEY);
            mError = icicle.getString(ERROR_KEY);
        } else {
            mOutgoingState = CB_STATE_OFF;
            mIncomingState = CB_STATE_OFF;
            mSetOutgoing = CB_INVALID;
            mSetIncoming = CB_INVALID;
            mDialogState = OLD_PSW_MODE;
            mCBDataStale = true;
            mIsBusyDialogAvailable = false;
            mPassword = null;
            mNewPsw = null;
            mError = null;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (DBG) log("onResume");

        if (mCBDataStale && !mIsShowUTDialog) {
            // If airplane mode is on, do not bother querying.
            if (Settings.System.getInt(getContentResolver(),
                    Settings.System.AIRPLANE_MODE_ON, 0) == 0) {
                mOutgoingState = CB_STATE_OFF;
                mIncomingState = CB_STATE_OFF;
                queryAllCBOptions();
            } else {
                if (DBG) log("onResume: airplane mode on");
                showDialog (RADIO_OFF_ERROR);
            }
        } else {
            mListOutgoing.setSummary(convertToStatusString(mOutgoingState));
            mListIncoming.setSummary(convertToStatusString(mIncomingState));
            if (mCallBarringHelper.isUtEnabled() && isDisableOutcbOverIms()) {
                mListOutgoing.setEnabled(false);
            } else {
                mListOutgoing.setEnabled(true);
            }
            if (mCallBarringHelper.isUtEnabled()) {
                mDialogChangePSW.setEnabled(false);
                mDialogCancelAll.setEnabled(false);
            } else {
                mDialogChangePSW.setEnabled(true);
                mDialogCancelAll.setEnabled(true);
            }
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        if (DBG) log("onSaveInstanceState: saving relevant UI state.");

        outState.putInt(CALL_BARRING_OUTGOING_KEY, mOutgoingState);
        outState.putInt(CALL_BARRING_INCOMING_KEY, mIncomingState);
        outState.putInt(SETOUTGOING_KEY, mSetOutgoing);
        outState.putInt(SETINCOMING_KEY, mSetIncoming);
        outState.putInt(DIALOGSTATE_KEY, mDialogState);
        outState.putBoolean(CBDATASTALE_KEY, mCBDataStale);
        outState.putBoolean(ISBUSYDIALOGAVAILABLE_KEY, mIsBusyDialogAvailable);
        outState.putString(PASSWORD_KEY, mPassword);
        outState.putString(NEW_PSW_KEY, mNewPsw);
        outState.putString(ERROR_KEY, mError);
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference.getKey().equals(CALL_BARRING_OUTGOING_KEY)) {
            showDialog(OUTGOING_SET_DIALOG);
        } else if (preference.getKey().equals(CALL_BARRING_INCOMING_KEY)) {
            showDialog(INCOMING_SET_DIALOG);
        }
        return super.onPreferenceTreeClick(preferenceScreen, preference);
    }

    private int findNextOutgoingFacility() {
        while (mOutSetShift < CB_OUTGOING_BOUNDARY) {
            if (((1 << mOutSetShift) & (mOutgoingState ^ mSetOutgoing)) != 0)
                break;
            mOutSetShift++;
        }
        // If found, return and shift to the next
        if (mOutSetShift < CB_OUTGOING_BOUNDARY)
            return (1 << mOutSetShift++);
        return 0;
    }

    private int findNextIncomingFacility() {
        while (mInSetShift < CB_INCOMING_BOUNDARY) {
            if (((1 << mInSetShift) & (mIncomingState ^ mSetIncoming)) != 0)
                break;
            mInSetShift++;
        }
        // If found, return and shift to the next
        if (mInSetShift < CB_INCOMING_BOUNDARY)
            return (1 << mInSetShift++);
        return 0;
    }

    // convert outgoing state to a boolean table
    private boolean[] getOutgoingCheckedTable() {
        boolean[] values = new boolean[CB_OUTGOING_COUNT];

        for (int shift = 0; shift < CB_OUTGOING_COUNT; shift++) {
            values[shift] = ((mOutgoingState & (1 << shift)) != 0);
        }

        return values;
    }

    // convert incoming state to a boolean table
    private boolean[] getIncomingCheckedTable() {
        boolean[] values = new boolean[CB_INCOMING_COUNT];

        for (int shift = 0; shift < CB_INCOMING_COUNT; shift++) {
            values[shift] = ((mIncomingState & (1 << (shift + CB_INCOMING_SHIFT))) != 0);
        }

        return values;
    }

    private int setNextOutgoingSerially() {
        int next = findNextOutgoingFacility();
        if (next != 0) {
            String facility = getBarringFacility(next);
            boolean state = ((next & mSetOutgoing) != 0);
            if (DBG) log("facility: " + facility + ", state: " + state);
            mCallBarringHelper.setCallBarringOption(facility, state, mPassword,
                    Message.obtain(mSetOptionComplete, EVENT_CB_SET_OUTGOING_SERIALLY));
        }
        return next;
    }

    private int setNextIncomingSerially() {
        int next = findNextIncomingFacility();
        if (next != 0) {
            String facility = getBarringFacility(next);
            boolean state = ((next & mSetIncoming) != 0);
            if (DBG) log("facility: " + facility + ", state: " + state);
            mCallBarringHelper.setCallBarringOption(facility, state, mPassword,
                    Message.obtain(mSetOptionComplete, EVENT_CB_SET_INCOMING_SERIALLY));
        }
        return next;
    }

    private int startSetOutgoingSerially() {
        mOutSetShift = 0;
        return setNextOutgoingSerially();
    }

    private int startSetIncomingSerially() {
        mInSetShift = CB_INCOMING_SHIFT;
        return setNextIncomingSerially();
    }

    private String convertToStatusString(int value) {
        if (value == CB_STATE_OFF)
            return "OFF";

        String statusString = "";
        if ((value & CB_BAOC) != 0)
            statusString += "All,";
        if ((value & CB_BAOIC) != 0)
            statusString += "International,";
        if ((value & CB_BAOICxH) != 0)
            statusString += "International Roaming,";
        if ((value & CB_BAIC) != 0)
            statusString += "All,";
        if ((value & CB_BAICr)!= 0)
            statusString += "Roaming,";

        // remove the ',' at the end of String
        if (!TextUtils.isEmpty(statusString))
            return statusString.substring(0, statusString.length() - 1);

        return statusString;
    }

    private boolean checkMobileDataForCf() {
        CarrierConfigManager configManager = (CarrierConfigManager)getSystemService(
                Context.CARRIER_CONFIG_SERVICE);
        PersistableBundle pb = configManager.getConfigForSubId(mCallBarringHelper.getSubId());
        return pb != null ? pb.getBoolean("check_mobile_data_for_cf") : false;
    }

    private boolean isDisableOutcbOverIms() {
        CarrierConfigManager configManager = (CarrierConfigManager)getSystemService(
                Context.CARRIER_CONFIG_SERVICE);
        PersistableBundle pb = configManager.getConfigForSubId(mCallBarringHelper.getSubId());

        return pb != null ? pb.getBoolean("config_enable_callbarring_over_ims") : false;
    }

    // Request to begin querying for call barring.
    private void queryAllCBOptions() {
        showDialog(INITIAL_BUSY_DIALOG);
        mCallBarringHelper.getCallBarringOption (CommandsInterface.CB_FACILITY_BAIC, "",
                Message.obtain(mGetAllCBOptionsComplete, EVENT_CB_QUERY_ALL, CB_BAIC, 0));
    }

    // callback after each step of querying for all options.
    private Handler mGetAllCBOptionsComplete = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            int status = MSG_OK;

            switch (msg.what) {
                case EVENT_CB_CANCEL_QUERY:
                    removeMessages(EVENT_CB_QUERY_ALL);
                    removeDialog(INITIAL_BUSY_DIALOG);
                    finish();
                    break;
                case EVENT_CB_QUERY_ALL:
                    status = handleGetCBMessage(ar, msg.arg1);
                    if (status != MSG_OK) {
                        Log.d("CallBarring","EXCEPTION_ERROR!");
                        if (ar.exception != null) {
                            CommandException ce = (CommandException)ar.exception;
                            if (ce.getCommandError()
                                    == CommandException.Error.FDN_CHECK_FAILURE) {
                                showDialog(FDN_CHECK_FAILURE);
                                return;
                            }
                        }
                    }

                    switch (msg.arg1) {
                        case CB_BAIC:
                            mCallBarringHelper.getCallBarringOption(CommandsInterface.
                                    CB_FACILITY_BAICr, "",
                                    Message.obtain(mGetAllCBOptionsComplete,
                                    EVENT_CB_QUERY_ALL, CB_BAICr, 0));
                            break;
                        case CB_BAICr:
                            if (mCallBarringHelper.isUtEnabled() && isDisableOutcbOverIms()) {
                                mCBDataStale = false;
                                syncUiWithState();
                                removeDialog(INITIAL_BUSY_DIALOG);
                            } else {
                                mCallBarringHelper.getCallBarringOption(CommandsInterface.
                                        CB_FACILITY_BAOC, "",
                                        Message.obtain(mGetAllCBOptionsComplete,
                                        EVENT_CB_QUERY_ALL, CB_BAOC, 0));
                            }
                            break;
                        case CB_BAOC:
                            mCallBarringHelper.getCallBarringOption(CommandsInterface.
                                    CB_FACILITY_BAOIC, "",
                                    Message.obtain(mGetAllCBOptionsComplete,
                                    EVENT_CB_QUERY_ALL, CB_BAOIC, 0));
                            break;
                        case CB_BAOIC:
                            mCallBarringHelper.getCallBarringOption(CommandsInterface.
                                    CB_FACILITY_BAOICxH, "",
                                    Message.obtain(mGetAllCBOptionsComplete,
                                    EVENT_CB_QUERY_ALL, CB_BAOICxH, 0));
                            break;
                        case CB_BAOICxH:
                            mCBDataStale = false;
                            syncUiWithState();
                            removeDialog(INITIAL_BUSY_DIALOG);
                            break;
                        default:
                    }
                    break;
                default:
                    break;
            }
        }
    };

    private int handleGetCBMessage(AsyncResult ar, int reason) {
        if (ar.exception != null) {
            Log.e(LOG_TAG, "handleGetCBMessage: Error getting CB enable state.");
            return MSG_EXCEPTION;
        } else if (ar.userObj instanceof Throwable) {
            Log.e(LOG_TAG, "handleGetCBMessage: Error during set call barring, reason: " + reason +
                    " exception: " + ((Throwable) ar.userObj).toString());
            return MSG_UNEXPECTED_RESPONSE;
        } else {
            int cbState = (((int[])ar.result)[0] & VOICE_SERVICE);
            Log.i(LOG_TAG, "reason: " + reason + " state: " + cbState);
            if ((cbState != 0) && (cbState != 1)) {
                Log.e(LOG_TAG, "handleGetCBMessage: Error getting CB state, unexpected value.");
                return MSG_UNEXPECTED_RESPONSE;
            }

            switch (reason) {
                case CB_BAOC:
                case CB_BAOIC:
                case CB_BAOICxH:
                    if (cbState == 1)
                        mOutgoingState |= reason;
                    else
                        mOutgoingState &= ~reason;
                    break;
                case CB_BAIC:
                case CB_BAICr:
                    if (cbState == 1)
                        mIncomingState |= reason;
                    else
                        mIncomingState &= ~reason;
                    break;
                default:
            }
        }
        return MSG_OK;
    }

    private void syncUiWithState() {
        mListOutgoing.setSummary(convertToStatusString(mOutgoingState));
        mListIncoming.setSummary(convertToStatusString(mIncomingState));
        mSetOutgoing = CB_INVALID;
        mSetIncoming = CB_INVALID;
        // only when we get the CB data and Ut is enabled, disable the preference
        // for other cases, just enable it in case any function broken
        if (mCallBarringHelper.isUtEnabled() && !mCBDataStale && isDisableOutcbOverIms()) {
            mListOutgoing.setEnabled(false);
        } else {
            mListOutgoing.setEnabled(true);
        }

        if (mCallBarringHelper.isUtEnabled()) {
            mDialogChangePSW.setEnabled(false);
            mDialogCancelAll.setEnabled(false);
        } else {
            mDialogChangePSW.setEnabled(true);
            mDialogCancelAll.setEnabled(true);
        }
    }

    private void resetDialogState(int mode) {
        mDialogState = mode;
        mPassword = null;
        mNewPsw = null;
        mError = null;
        mDialogChangePSW.setDialogMessage(getResources().getString(R.string.psw_enter_old));
    }

    private void showPswDialog() {
        if (mDialogState == OFF_MODE) {
            return;
        }

        String message = null;
        switch (mDialogState) {
            case CB_OUTGOING_MODE:
            case CB_INCOMING_MODE:
                mDialogChangePSW.setDialogTitle(
                        getResources().getString(R.string.input_password));
                break;
            case OLD_PSW_MODE:
                message = getResources().getString(R.string.psw_enter_old);
                mDialogChangePSW.setDialogTitle(
                        getResources().getString(R.string.labelCbChangePassword));
                break;
            case NEW_PSW_MODE:
                message = getResources().getString(R.string.psw_enter_new);
                mDialogChangePSW.setDialogTitle(
                        getResources().getString(R.string.labelCbChangePassword));
                break;
            case REENTER_PSW_MODE:
                message = getResources().getString(R.string.psw_reenter_new);
                mDialogChangePSW.setDialogTitle(
                        getResources().getString(R.string.labelCbChangePassword));
                break;
            default:
        }

        if (mError != null) {
            if (message != null) {
                message = mError + "\n" + message;
            } else {
                message = mError;
            }
            mError = null;
        }

        mDialogChangePSW.setText("");
        mDialogChangePSW.setDialogMessage(message);
        mDialogChangePSW.showPinDialog();
    }

    public void onPinEntered(EditPinPreference preference, boolean positiveResult) {
        if (!positiveResult) {
            mSetOutgoing = CB_INVALID;
            mSetIncoming = CB_INVALID;
            resetDialogState(OLD_PSW_MODE);
            return;
        }

        // read and clear the preference value for next input.
        String tmpPsw = preference.getText();
        preference.setText("");

        if (preference == mDialogCancelAll) {
            if ((mOutgoingState == CB_STATE_OFF) && (mIncomingState == CB_STATE_OFF)) {
                showToast(getResources().getString(R.string.no_call_barring));
                return;
            }

            if (!reasonablePSW(tmpPsw)) {
                mError = getResources().getString(R.string.invalidPsw);
                showCancelDialog();
                return;
            }

            showDialog(BUSY_DIALOG);
            mCallBarringHelper.setCallBarringOption(CommandsInterface.
                    CB_FACILITY_BA_ALL, false, tmpPsw,
                    Message.obtain(mSetOptionComplete, EVENT_CB_CANCEL_ALL));
        } else if (preference == mDialogChangePSW) {
            switch (mDialogState) {
                case CB_OUTGOING_MODE:
                case CB_INCOMING_MODE:
                    if (!reasonablePSW(tmpPsw)) {
                        mError = getResources().getString(R.string.invalidPsw);
                        showPswDialog();
                        return;
                    }
                    showDialog(BUSY_DIALOG);
                    mPassword = tmpPsw;

                    if (mDialogState == CB_OUTGOING_MODE) {
                        if (mSetOutgoing != mOutgoingState) {
                            startSetOutgoingSerially();
                            resetDialogState(OLD_PSW_MODE);
                        } else {
                            dismissBusyDialog();
                            showToast(getResources().getString(R.string.no_call_barring));
                            Log.e(LOG_TAG, "no Call barring setting changed!");
                            return;
                        }
                    } else {
                        if (mSetIncoming != mIncomingState) {
                            startSetIncomingSerially();
                            resetDialogState(OLD_PSW_MODE);
                        } else {
                            dismissBusyDialog();
                            showToast(getResources().getString(R.string.no_call_barring));
                            Log.e(LOG_TAG, "no Call barring setting changed!");
                            return;
                        }
                    }
                    break;
                case OLD_PSW_MODE:
                    if (!reasonablePSW(tmpPsw)) {
                        mError = getResources().getString(R.string.invalidPsw);
                        showPswDialog();
                        return;
                    } else {
                        mPassword = tmpPsw;
                        mDialogState = NEW_PSW_MODE;
                        showPswDialog();
                    }
                    break;
                case NEW_PSW_MODE:
                    if (!reasonablePSW(tmpPsw)) {
                        mError = getResources().getString(R.string.invalidPsw);
                        showPswDialog();
                        return;
                    } else {
                        mNewPsw = tmpPsw;
                        mDialogState = REENTER_PSW_MODE;
                        showPswDialog();
                    }
                    break;
                case REENTER_PSW_MODE:
                    if (!tmpPsw.equals(mNewPsw)) {
                        mError = getResources().getString(R.string.cb_psw_dont_match);
                        mNewPsw = null;
                        mDialogState = NEW_PSW_MODE;
                        showPswDialog();
                    } else {
                        mError = null;
                        showDialog(BUSY_DIALOG);
                        mCallBarringHelper.requestChangeCbPsw(CommandsInterface.CB_FACILITY_BA_ALL,
                                mPassword, mNewPsw,
                                Message.obtain(mSetOptionComplete, EVENT_CB_CHANGE_PSW));
                        resetDialogState(OLD_PSW_MODE);
                    }
                    break;
                case OFF_MODE:
                default:
            }
        }
    }

    private Handler mSetOptionComplete = new Handler() {
        @Override
        public void handleMessage(Message msg) {

            AsyncResult ar = (AsyncResult) msg.obj;
            String facility = null;
            boolean state = true;
            boolean found;
            int next;

            switch (msg.what) {
                case EVENT_CB_CANCEL_ALL:
                    dismissBusyDialog();
                    if (ar.exception != null) {
                        showToast(getResources().getString(R.string.response_error));
                    } else {
                        mOutgoingState = CB_STATE_OFF;
                        mIncomingState = CB_STATE_OFF;
                        syncUiWithState();
                        showToast(getResources().getString(R.string.operation_successfully));
                    }
                    break;
                case EVENT_CB_SET_COMPLETE:
                    dismissBusyDialog();
                    if (ar.exception != null) {
                        mSetOutgoing = CB_INVALID;
                        mSetIncoming = CB_INVALID;
                        showToast(getResources().getString(R.string.response_error));
                    } else {
                        if (mOutgoingState != mSetOutgoing && mSetOutgoing != CB_INVALID) {
                            mOutgoingState = mSetOutgoing;
                        }
                        if (mIncomingState != mSetIncoming && mSetIncoming != CB_INVALID) {
                            mIncomingState = mSetIncoming;
                        }
                        syncUiWithState();
                        showToast(getResources().getString(R.string.operation_successfully));
                    }
                    break;

                case EVENT_CB_SET_OUTGOING_SERIALLY:
                    if (ar.exception != null) {
                        showToast(getResources().getString(R.string.response_error));
                    } else {
                        // update the mOutgoing state if set successfuly
                        // The shift value needs to minus 1 since it points to next
                        int outSetMask = (1 << (mOutSetShift - 1));
                        if ((outSetMask & mSetOutgoing) != 0) {
                            mOutgoingState |= outSetMask;
                        } else {
                            mOutgoingState &= ~outSetMask;
                        }
                    }

                    if (setNextOutgoingSerially() == 0) {
                        dismissBusyDialog();
                        syncUiWithState();
                    }
                    break;

                case EVENT_CB_SET_INCOMING_SERIALLY:
                    if (ar.exception != null) {
                        showToast(getResources().getString(R.string.response_error));
                    } else {
                        // update the Incoming state if set successfully
                        // The shift value needs to minus 1 since it points to next
                        int inSetMask = (1 << (mInSetShift - 1));
                        if ((inSetMask & mSetIncoming) != 0) {
                            mIncomingState |= inSetMask;
                        } else {
                            mIncomingState &= ~inSetMask;
                        }
                    }

                    if (setNextIncomingSerially() == 0) {
                        dismissBusyDialog();
                        syncUiWithState();
                    }
                    break;

                case EVENT_CB_CHANGE_PSW:
                    dismissBusyDialog();
                    if (ar.exception != null) {
                        showToast(getResources().getString(R.string.response_error));
                    } else {
                        showToast(getResources().getString(R.string.operation_successfully));
                    }
                default:
                    break;
            }
        }
    };

    private void showCancelDialog() {
        mDialogCancelAll.setText(mPassword);
        mDialogCancelAll.setDialogMessage(mError);
        mDialogCancelAll.showPinDialog();
    }

    private boolean reasonablePSW(String psw) {
        if (psw == null || psw.length() < MIN_PSW_LENGTH || psw.length() > MAX_PSW_LENGTH) {
            return false;
        } else {
            return true;
        }
    }

    public void onClick(DialogInterface dialog, int which) {
        dialog.dismiss();
    }

    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    @Override
    protected Dialog onCreateDialog(int id) {

        if ((id == BUSY_DIALOG) || (id == INITIAL_BUSY_DIALOG)) {
            ProgressDialog dialog = new ProgressDialog(this);
            dialog.setTitle(getText(R.string.updating_title));
            dialog.setIndeterminate(true);

            switch (id) {
                case BUSY_DIALOG:
                    mIsBusyDialogAvailable = true;
                    dialog.setCancelable(false);
                    dialog.setMessage(getText(R.string.updating_settings));
                    break;
                case INITIAL_BUSY_DIALOG:
                    dialog.setCancelable(true);
                    dialog.setCancelMessage(
                            mGetAllCBOptionsComplete.obtainMessage(EVENT_CB_CANCEL_QUERY));
                    dialog.setMessage(getText(R.string.reading_settings));
                    break;
            }

            return dialog;

        // Handle error dialog codes
        } else if ((id == RESPONSE_ERROR) || (id == EXCEPTION_ERROR)
                || (id == RADIO_OFF_ERROR) || id == FDN_CHECK_FAILURE) {
            int msgId;
            int titleId = R.string.error_updating_title;
            AlertDialog.Builder b = new AlertDialog.Builder(this);

            switch (id) {
                case RESPONSE_ERROR:
                    msgId = R.string.response_error;
                    // Set Button 2, tells the activity that the error is
                    // recoverable on dialog exit.
                    b.setNegativeButton(R.string.close_dialog, this);
                    break;
                case RADIO_OFF_ERROR:
                    msgId = R.string.radio_off_error;
                    // Set Button 3
                    b.setNeutralButton(R.string.close_dialog, this);
                    break;
                case FDN_CHECK_FAILURE:
                    msgId = R.string.fdn_check_failure;
                    b.setNeutralButton(R.string.close_dialog, this);
                    break;
                case EXCEPTION_ERROR:
                default:
                    msgId = R.string.exception_error;
                    // Set Button 3, tells the activity that the error is
                    // not recoverable on dialog exit.
                    b.setNeutralButton(R.string.close_dialog, this);
                    break;
            }

            b.setTitle(getText(titleId));
            b.setMessage(getText(msgId));
            b.setNeutralButton(R.string.close_dialog,
                    new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int id) {
                                finish();
                            }
                    });
            b.setCancelable(false);
            AlertDialog dialog = b.create();
            // make the dialog more obvious by bluring the background.
            dialog.getWindow().addFlags(WindowManager.LayoutParams.FLAG_BLUR_BEHIND);
            return dialog;
        } else if (id == IMS_UT_REQUEST) {
            Dialog dialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.no_mobile_data)
                    .setMessage(R.string.setting_mobile_data_alert)
                    .setIconAttribute(android.R.attr.alertDialogIcon)
                    .setPositiveButton(android.R.string.ok,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    Intent newIntent = new Intent("android.settings.SETTINGS");
                                    newIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                                    startActivity(newIntent);
                                    mIsShowUTDialog = false;
                                    finish();
                                }
                            })
                    .setNegativeButton(android.R.string.cancel,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    mIsShowUTDialog = false;
                                    dialog.dismiss();
                                    finish();
                                }
                            })
                    .create();
            return dialog;
        } else if (id == IMS_UT_DATA_ROAMING_REQUEST) {
            Dialog dialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.no_mobile_data_roaming)
                    .setMessage(R.string.setting_mobile_data_roaming_alert)
                    .setIconAttribute(android.R.attr.alertDialogIcon)
                    .setPositiveButton(android.R.string.ok,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    Intent newIntent = new Intent("android.settings.SETTINGS");
                                    newIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                                    startActivity(newIntent);
                                    mIsShowUTDialog = false;
                                    finish();
                                }
                            })
                    .setNegativeButton(android.R.string.cancel,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    mIsShowUTDialog = false;
                                    dialog.dismiss();
                                    finish();
                                }
                            })
                    .create();
            return dialog;
        } else if (id == OUTGOING_SET_DIALOG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            boolean[] checkedValues = getOutgoingCheckedTable();
            mSetOutgoing = mOutgoingState;
            builder.setTitle(R.string.set_outgoing)
                   .setMultiChoiceItems(R.array.call_barring_outgoing_display_values, checkedValues,
                            new DialogInterface.OnMultiChoiceClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which,
                                        boolean isChecked) {
                                    if (isChecked) {
                                        mSetOutgoing |= (1 << which);
                                    } else {
                                        mSetOutgoing &= ~(1 << which);
                                    }
                                }
                            })
                   .setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    if (mSetOutgoing == mOutgoingState) {
                                        showToast(getResources().getString(
                                                R.string.no_call_barring));
                                        Log.e(LOG_TAG, "no Call barring setting changed!");
                                    } else if (mCallBarringHelper.isUtEnabled()) {
                                        startSetOutgoingSerially();
                                        resetDialogState(OLD_PSW_MODE);
                                    } else {
                                        resetDialogState(CB_OUTGOING_MODE);
                                        showPswDialog();
                                    }
                                    removeDialog(OUTGOING_SET_DIALOG);
                                }
                            })
                   .setNegativeButton(R.string.cancel,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    // keep the same value as what is current
                                    mSetOutgoing = mOutgoingState;
                                    removeDialog(OUTGOING_SET_DIALOG);
                                }
                            });
            return builder.create();
        } else if (id == INCOMING_SET_DIALOG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            boolean[] checkedValues = getIncomingCheckedTable();
            mSetIncoming = mIncomingState;
            builder.setTitle(R.string.set_incoming)
                   .setMultiChoiceItems(R.array.call_barring_incoming_display_values, checkedValues,
                            new DialogInterface.OnMultiChoiceClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which,
                                        boolean isChecked) {
                                    if (isChecked) {
                                        mSetIncoming |= (1 << (which + CB_INCOMING_SHIFT));
                                    } else {
                                        mSetIncoming &= ~(1 << (which + CB_INCOMING_SHIFT));
                                    }
                                }
                            })
                   .setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    if (mSetIncoming == mIncomingState) {
                                        showToast(getResources().getString(
                                                R.string.no_call_barring));
                                        Log.e(LOG_TAG, "no Call barring setting changed!");
                                    } else  if (mCallBarringHelper.isUtEnabled()) {
                                        startSetIncomingSerially();
                                        resetDialogState(OLD_PSW_MODE);
                                    } else {
                                        resetDialogState(CB_INCOMING_MODE);
                                        showPswDialog();
                                    }
                                    removeDialog(INCOMING_SET_DIALOG);
                                }
                            })
                   .setNegativeButton(R.string.cancel,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    // keep the same value as what is curent
                                    mSetIncoming = mIncomingState;
                                    removeDialog(INCOMING_SET_DIALOG);
                                }
                            });
            return builder.create();
        }
        return null;
    }

    private final void dismissBusyDialog() {
        if (mIsBusyDialogAvailable) {
            removeDialog(BUSY_DIALOG);
            mIsBusyDialogAvailable = false;
        }
    }

    private String getBarringFacility(int cb) {
        String cbName = null;

        switch (cb) {
            case CB_BAOC:
                cbName = CommandsInterface.CB_FACILITY_BAOC;
                break;
            case CB_BAOIC:
                cbName = CommandsInterface.CB_FACILITY_BAOIC;
                break;
            case CB_BAOICxH:
                cbName = CommandsInterface.CB_FACILITY_BAOICxH;
                break;
            case CB_BAIC:
                cbName = CommandsInterface.CB_FACILITY_BAIC;
                break;
            case CB_BAICr:
                cbName = CommandsInterface.CB_FACILITY_BAICr;
                break;
            case CB_BA_ALL:
                cbName = CommandsInterface.CB_FACILITY_BA_ALL;
                break;
            default:
        }
        return cbName;
    }

    private int getActiveNetworkType() {
        ConnectivityManager cm = (ConnectivityManager) getSystemService(
                Context.CONNECTIVITY_SERVICE);
        if (cm != null) {
            NetworkInfo ni = cm.getActiveNetworkInfo();
            if ((ni == null) || !ni.isConnected()){
                return ConnectivityManager.TYPE_NONE;
            }
            return ni.getType();
        }
        return ConnectivityManager.TYPE_NONE;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        final int itemId = item.getItemId();
        if (itemId == android.R.id.home) {
            onBackPressed();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private static void log(String msg) {
        if (DBG) Log.d(LOG_TAG, msg);
    }
}

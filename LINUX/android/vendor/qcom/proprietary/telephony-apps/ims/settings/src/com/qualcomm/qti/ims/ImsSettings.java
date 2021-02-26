/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (C) 2015, The Linux Foundation. All rights reserved.
 */

package com.qualcomm.qti.ims;

import android.app.Activity;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.provider.MediaStore;
import android.provider.Settings;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.MenuItem;
import android.widget.Toast;

import com.android.ims.ImsConfig;
import com.android.ims.ImsException;
import com.android.ims.ImsManager;

import org.codeaurora.ims.utils.QtiCallUtils;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.QtiImsException;
import org.codeaurora.ims.QtiImsUnsupportedImageFormatException;
import org.codeaurora.ims.QtiImsExtListenerBaseImpl;
import org.codeaurora.ims.QtiImsExtManager;
import org.codeaurora.ims.utils.QtiImsExtUtils;

/**
 * The activity class for editing a new or existing IMS profile.
 */
public class ImsSettings extends PreferenceActivity
        implements Preference.OnPreferenceChangeListener {

    private static final String TAG = ImsSettings.class.getSimpleName();
    public static final String SUB_ID_EXTRA =
            "com.android.phone.settings.SubscriptionInfoHelper.SubscriptionId";
    private static final int LOAD_IMAGE = 1;

    /* Request code for activity that handles intent defined in selectImage API.
       This code will be returned in onActivityResult() when activity exits */
    public static final int RESULT_SELECT_IMAGE = 0;
    public static final String BUTTON_SET_STATIC_IMAGE_KEY = "ims_vt_call_static_image";
    public static final String BUTTON_VT_CALL_QUALITY_KEY = "ims_vt_call_quality";
    public static final String KEY_TOGGLE_RTT = "toggle_rtt";

    private ListPreference mVideoCallQuality;
    private ImsConfig mImsConfig;

    private PreferenceScreen mScreen = null;
    private Preference mPcPreference = null;
    private Preference mPerPreference = null;
    private Preference mStaticImagePreference = null;

    private EditTextPreference mDeflectNum = null;
    private SwitchPreference mButtonCsRetry = null;
    private int[] mCallState = null;
    private PhoneStateListener[] mPhoneStateListener = null;
    int mPhoneId = QtiCallConstants.INVALID_PHONE_ID;

    private SwitchPreference mRttModePreference = null;

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");

        registerPhoneStateListeners();
        displayVideoQualityOptions();
        displayPcPerOptions();
        displayDeflectNumEditor();
        displayCsRetryOpions();
        displayStaticImageOptions();

        displayRttOption();
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d(TAG, "onPause");
        unRegisterPhoneStateListeners();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");

        mPcPreference = null;
        mPerPreference = null;
        mStaticImagePreference = null;
        mDeflectNum = null;
        mScreen = null;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "start ImsSettings");
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.ims_settings);
        mScreen = getPreferenceScreen();

        if (mScreen == null) {
            Log.e(TAG, "PreferenceScreen is invalid");
            return;
        }

        mPhoneId = getIntent().getIntExtra(QtiCallConstants.EXTRA_PHONE_ID,
                QtiCallConstants.INVALID_PHONE_ID);
        Log.d(TAG, "ImsSetting mPhoneId = " + mPhoneId);

        mVideoCallQuality = (ListPreference) mScreen
                .findPreference(BUTTON_VT_CALL_QUALITY_KEY);

        mPcPreference = (Preference) mScreen.findPreference(getString(R.string.ims_pc_count));
        mPerPreference = (Preference) mScreen.findPreference(getString(R.string.ims_per_count));

        mStaticImagePreference = (Preference) mScreen.
                findPreference(BUTTON_SET_STATIC_IMAGE_KEY);

        mButtonCsRetry = (SwitchPreference)mScreen.findPreference(
                getString(R.string.ims_to_cs_retry));

        initRttOption();

        try {
            ImsManager imsManager = ImsManager.getInstance(getBaseContext(), mPhoneId);
            mImsConfig = imsManager.getConfigInterface();
        } catch (ImsException e) {
            mImsConfig = null;
            Log.e(TAG, "ImsService is not running");
        }

        mPhoneStateListener = new PhoneStateListener[TelephonyManager.getDefault().getPhoneCount()];
        mCallState = new int[mPhoneStateListener.length];

        mDeflectNum = (EditTextPreference) mScreen.findPreference(getString(
                R.string.qti_ims_call_deflect));
        mDeflectNum.setOnPreferenceChangeListener(this);

        getActionBar().setDisplayHomeAsUpEnabled(true);
    }

    private void registerPhoneStateListeners() {
        TelephonyManager tm =
                (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        SubscriptionManager subMgr = SubscriptionManager.from(this);
        if (tm == null || subMgr == null) {
            Log.e(TAG, "TelephonyManager or SubscriptionManager is null");
            return;
        }

        for (int i = 0; i < mPhoneStateListener.length; i++) {
            final SubscriptionInfo subInfo =
                    subMgr.getActiveSubscriptionInfoForSimSlotIndex(i);
            if (subInfo == null) {
                Log.e(TAG, "registerPhoneStateListener subInfo : " + subInfo +
                        " for phone Id: " + i);
                continue;
            }

            final int phoneId = i;
            mPhoneStateListener[i]  = new PhoneStateListener(subInfo.getSubscriptionId()) {
                /*
                * Enable/disable the "set static image" and "VT call quality" button
                * when in/out of a call
                * @see android.telephony.PhoneStateListener#onCallStateChanged(int,
                * java.lang.String)
                */
                @Override
                public void onCallStateChanged(int state, String incomingNumber) {
                    Log.d(TAG, "PhoneStateListener onCallStateChanged: state is " + state +
                            " SubId: " + mSubId);
                    mCallState[phoneId] = state;
                    Preference pref = getPreferenceScreen().
                            findPreference(BUTTON_SET_STATIC_IMAGE_KEY);
                    if (pref != null) {
                        pref.setEnabled(isCallStateIdle());
                    }
                    Preference vtQualityPref = getPreferenceScreen().
                            findPreference(BUTTON_VT_CALL_QUALITY_KEY);
                    if (vtQualityPref != null) {
                        vtQualityPref.setEnabled(isCallStateIdle());
                    }
                }
            };
            Log.d(TAG, "Register for call state change for phone Id: " + i);
            tm.listen(mPhoneStateListener[i], PhoneStateListener.LISTEN_CALL_STATE);
        }
    }

    private void unRegisterPhoneStateListeners() {
        TelephonyManager tm =
               (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        for (int i = 0; i < mPhoneStateListener.length; i++) {
            if (mPhoneStateListener[i] != null) {
                Log.d(TAG, "unRegister for call state change for phone Id: " + i);
                tm.listen(mPhoneStateListener[i], PhoneStateListener.LISTEN_NONE);
                mPhoneStateListener[i] = null;
            }
        }
    }

    private void displayVideoQualityOptions() {
        if (mScreen == null || mVideoCallQuality == null) {
            Log.e(TAG, "displayVideoQualityOptions PreferenceScreen/VideoQuality is invalid");
            return;
        }

        //Remove the preference by default and add only if supported
        mScreen.removePreference(mVideoCallQuality);
        mVideoCallQuality.setOnPreferenceChangeListener(null);

        int videoQuality = getVideoQuality();
        //Show the video quality prefernce only if modem reported valid video quality and
        //config video quality UI is enabled.
        if (QtiImsExtUtils.shallShowVideoQuality(mPhoneId, this) &&
                videoQuality != ImsConfig.OperationStatusConstants.UNKNOWN) {
            mScreen.addPreference(mVideoCallQuality);
            mVideoCallQuality.setOnPreferenceChangeListener(this);
            /* If High video quality is not supported, then remove High option */
            if (!(CameraUtil.getInstance().isHighVideoQualitySupported(this))) {
                mVideoCallQuality.setEntries(
                        R.array.ims_vt_call_quality_entries_high_not_supported);
                mVideoCallQuality.setEntryValues(
                        R.array.ims_vt_call_quality_values_high_not_supported);
            }
            loadVideoCallQualityPrefs(videoQuality);
        }
    }

    private void initRttOption() {
        // Check is carrier supports, only then display the option
        mRttModePreference = (SwitchPreference) findPreference(KEY_TOGGLE_RTT);
        if (QtiImsExtUtils.isRttSupported(mPhoneId, getApplicationContext())) {
            mRttModePreference.setPersistent(true);
            mRttModePreference.setOnPreferenceChangeListener(this);
        } else {
            Log.d(TAG, "RTT: not supported");
        }
    }

    private void displayCsRetryOpions() {
        if (mScreen == null || mButtonCsRetry == null) {
            Log.e(TAG, "displayCsRetryOptions PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mButtonCsRetry);
        mButtonCsRetry.setOnPreferenceChangeListener(null);

        //Enable CS Retry settings depending on CS Retry Config
        if (QtiImsExtUtils.isCsRetryConfigEnabled(mPhoneId, getApplicationContext())) {
            mScreen.addPreference(mButtonCsRetry);
            mButtonCsRetry.setOnPreferenceChangeListener(this);
            mButtonCsRetry.setChecked(QtiCallUtils.isCsRetryEnabledByUser(
                    getApplicationContext()));
        }
    }

    private void displayStaticImageOptions() {
        if (mScreen == null || mStaticImagePreference == null) {
            Log.e(TAG, "displayStaticImageOptions PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mStaticImagePreference);
        mStaticImagePreference.setOnPreferenceClickListener(null);

        //Enable static image options if static image config is enabled
        if (QtiImsExtUtils.shallShowStaticImageUi(mPhoneId, getApplicationContext())) {
            mScreen.addPreference(mStaticImagePreference);
            mStaticImagePreference.setOnPreferenceClickListener(prefClickListener);
        }
    }

    private void displayPcPerOptions() {
        if (mScreen == null) {
            Log.e(TAG, "displayPcPerOptions PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mPcPreference);
        mScreen.removePreference(mPerPreference);
        mPcPreference.setOnPreferenceClickListener(null);
        mPerPreference.setOnPreferenceClickListener(null);

        if (isRtpStatisticsQueryEnabled()) {
            //Show Packet Count/Packet Error Count UI only if the config is enabled
            mScreen.addPreference(mPcPreference);
            mScreen.addPreference(mPerPreference);
            mPcPreference.setOnPreferenceClickListener(prefClickListener);
            mPerPreference.setOnPreferenceClickListener(prefClickListener);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
            onBackPressed();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void setCsRetry(boolean turnOn) {
        final int value = (turnOn) ? 1:0;
        android.provider.Settings.Global.putInt(
                  getApplicationContext().getContentResolver(),
                  QtiCallConstants.IMS_TO_CS_RETRY_ENABLED, value);
    }

    private void displayToast(String displayStr) {
        Toast.makeText(getApplicationContext(), displayStr, Toast.LENGTH_SHORT).show();
    }

    private QtiImsExtListenerBaseImpl QtiImsExtListener =
            new QtiImsExtListenerBaseImpl() {

        @Override
        public void onGetPacketCount(int phoneId, int status, long packetCount) {
            mPcPreference.setEnabled(true);
            String sPc = getApplicationContext().getResources().getString(
                    R.string.ims_get_packet_count_failed);
            if (hasRequestFailed(status)) {
                Log.e(TAG, "onGetPacketCount: get failed. errorCode = " + status);
            } else {
                Log.d(TAG, "onGetPacketCount: packetCount = " + packetCount);
                Long pc = new Long(packetCount);
                sPc = "packetCount = " + pc.toString();
            }
            displayToast(sPc);
        }

        @Override
        public void onGetPacketErrorCount(int phoneId, int status, long packetErrorCount) {
            mPerPreference.setEnabled(true);
            String sPec = getApplicationContext().getResources().getString(
                    R.string.ims_get_packet_error_count_failed);
            if (hasRequestFailed(status)) {
                Log.e(TAG, "onGetPacketErrorCount: get failed. errorCode = " + status);
            } else {
                Log.d(TAG, "onGetPacketErrorCount: packetErrorCount = " + packetErrorCount);
                Long pec = new Long(packetErrorCount);
                sPec = "packetErrorCount = " + pec.toString();
            }
            displayToast(sPec);
        }
    };

    private void onPcPrefClicked() {
        Log.d(TAG, "onPcPrefClicked");

        try {
            new QtiImsExtManager(getApplicationContext()).getPacketCount(
                    mPhoneId, QtiImsExtListener);
            mPcPreference.setEnabled(false);
        } catch (QtiImsException e) {
            Log.e(TAG, "getPacketCount failed. Exception = " + e);
        }
    }

    private void onPerPrefClicked() {
        Log.d(TAG, "onPerPrefClicked");

        try {
            new QtiImsExtManager(getApplicationContext()).getPacketErrorCount(
                    mPhoneId, QtiImsExtListener);
            mPerPreference.setEnabled(false);
        } catch (QtiImsException e) {
            Log.e(TAG, "getPacketErrorCount failed. Exception = " + e);
        }
    }

    /* This API prepares an intent with which an activity is launched.
       User then can select the image from that activity */
    private void selectImage() {
        Log.d(TAG, "selectImage");

        final String[] ACCEPT_MIME_TYPES = {
            "image/jpeg",
            "image/jpg",
            "image/png"
        };

        Intent intent = new Intent(
                Intent.ACTION_GET_CONTENT,
                android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        /* allow the user to select data that is already on the device
           not requiring it be downloaded from a remote service when opened */
        intent.putExtra(Intent.EXTRA_LOCAL_ONLY, true);
        // restrict activity to show images of only ACCEPT_MIME_TYPES
        intent.putExtra(Intent.EXTRA_MIME_TYPES, ACCEPT_MIME_TYPES);

        // check if there is any activity that can handle this intent
        ComponentName cName = intent.resolveActivity(getPackageManager());
        Log.d(TAG, "selectImage cName : " + cName);
        if (cName != null) {
            startActivityForResult(intent, RESULT_SELECT_IMAGE);
        } else {
            Log.w(TAG, "UE cannot handle this intent");
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.d(TAG, "onActivityResult requestCode = " + requestCode);
        if (requestCode == RESULT_SELECT_IMAGE) {
            if (data == null) {
                Log.w(TAG, "possibly user didn't select any image");
                return;
            }

            TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
            if (mPhoneId != QtiCallConstants.INVALID_PHONE_ID &&
                    tm.getCallStateForSlot(mPhoneId) != TelephonyManager.CALL_STATE_IDLE) {
                Log.w(TAG, "call state is not idle so ignore user selected image");
                Toast.makeText(this, getResources().getString(
                        R.string.ims_vt_call_static_image_ignore).toString(),
                        Toast.LENGTH_SHORT).show();
                return;
            }

            Uri uri = data.getData();
            String imageUriStr = null;

            try {
                imageUriStr = ImageUtils.getFilePathFromUri(uri, this);
            } catch (QtiImsUnsupportedImageFormatException ex) {
                Log.e(TAG, "onActivityResult ex = " + ex);
                Toast.makeText(this, getResources().getString(
                        R.string.ims_vt_call_static_image_error).toString(),
                        Toast.LENGTH_SHORT).show();
                return;
            }

            Log.d(TAG, "uri = " + uri + " imageUriStr = " + imageUriStr);

            if (imageUriStr != null) {
                //store the imageUriStr in DB
                saveStaticImageUriStr(getContentResolver(),
                        imageUriStr);
                Toast.makeText(this, getResources().getString(
                        R.string.ims_vt_call_static_image_okay).toString(),
                        Toast.LENGTH_SHORT).show();
            }
        }
    }

    /* Saves the file path of static image selected by the user in DB */
    private void saveStaticImageUriStr(ContentResolver contentResolver, String uri) {
        android.provider.Settings.Global.putString(contentResolver,
                QtiImsExtUtils.QTI_IMS_STATIC_IMAGE_SETTING, uri);
    }

    Preference.OnPreferenceClickListener prefClickListener =
            new Preference.OnPreferenceClickListener() {
        @Override
        public boolean onPreferenceClick(Preference pref) {
            Log.d(TAG, "onPreferenceClick");
            if (pref.equals(mPcPreference)) {
                onPcPrefClicked();
            } else if (pref.equals(mPerPreference)) {
                onPerPrefClicked();
            } else if (pref.equals(mStaticImagePreference)) {
                selectImage();
            }
            return true;
        }
    };

    private boolean isRtpStatisticsQueryEnabled() {
        return getApplicationContext().getResources().getBoolean(
                R.bool.config_ims_enable_rtp_statistics);
    }

    @Override
    public boolean onPreferenceChange(Preference pref, Object newValue) {
        if (pref.equals(mVideoCallQuality)) {
            if (newValue == null) {
                Log.e(TAG, "onPreferenceChange invalid value received");
            } else {
                final int quality = Integer.parseInt(newValue.toString());
                boolean result = setVideoQuality(quality);
                if (result) {
                    loadVideoCallQualityPrefs(quality);
                }
                return result;
            }
        } else if (pref.equals(mDeflectNum)) {
            if (newValue == null) {
                Log.e(TAG, "onPreferenceChange Deflect number invalid value received");
            } else {
                QtiImsExtUtils.setCallDeflectNumber(
                        getApplicationContext().getContentResolver(), newValue.toString());
                displayDeflectNumEditor();
            }
        } else if (pref.equals(mButtonCsRetry)) {
            SwitchPreference csRetryPref = (SwitchPreference)pref;
            csRetryPref.setChecked(!csRetryPref.isChecked());
            setCsRetry(csRetryPref.isChecked());
        } else if (pref.equals(mRttModePreference)) {
            SwitchPreference rttPref = (SwitchPreference)pref;
            rttPref.setChecked(!rttPref.isChecked());
            setRttMode(rttPref.isChecked());
            Log.d(TAG, "RTT: onPreferenceChange mode = " + rttPref.isChecked());
        }
        return true;
    }

    private void loadVideoCallQualityPrefs(int vqValue) {
        Log.d(TAG, "loadVideoCallQualityPrefs, vqValue = " + vqValue);
        final String videoQuality = videoQualityToString(vqValue);
        mVideoCallQuality.setValue(String.valueOf(vqValue));
        mVideoCallQuality.setSummary(videoQuality);
    }

    private int getVideoQuality() {
        int videoQuality = ImsConfig.OperationStatusConstants.UNKNOWN;
        try {
            if (mImsConfig != null) {
                videoQuality = mImsConfig.getProvisionedValue(
                        ImsConfig.ConfigConstants.VIDEO_QUALITY);
            }
        } catch (ImsException e) {
            Log.e(TAG, "getVideoQuality failed. Exception = " + e);
        }
        return videoQuality;
    }

    private boolean setVideoQuality(int quality) {
        int result = ImsConfig.OperationStatusConstants.FAILED;
        try {
            if (mImsConfig != null) {
                result = mImsConfig.setProvisionedValue(ImsConfig.ConfigConstants.VIDEO_QUALITY,
                        quality);
            }
        } catch (ImsException e) {
            Log.e(TAG, "setVideoQuality failed. Exception = " + e);
        }
        Log.d(TAG, "setVideoQuality, result = " + result);
        return !hasRequestFailed(result);
    }

    private static boolean hasRequestFailed(int result) {
        return (result != ImsConfig.OperationStatusConstants.SUCCESS);
    }

    private String videoQualityToString(int quality) {
        switch (quality) {
            case QtiImsExtUtils.VideoQualityFeatureValuesConstants.HIGH:
                return getString(R.string.ims_vt_call_quality_high);
            case QtiImsExtUtils.VideoQualityFeatureValuesConstants.MEDIUM:
                return getString(R.string.ims_vt_call_quality_medium);
            case QtiImsExtUtils.VideoQualityFeatureValuesConstants.LOW:
                return getString(R.string.ims_vt_call_quality_low);
            case ImsConfig.OperationStatusConstants.UNKNOWN:
            default:
                return getString(R.string.ims_vt_call_quality_unknown);
        }
    }

    private boolean isCallDeflectionOrTransferEnabled() {
        return QtiImsExtUtils.isCallDeflectionSupported(mPhoneId, this) ||
                QtiImsExtUtils.isCallTransferEnabled(this);
    }

    private void displayDeflectNumEditor() {
        if (mScreen == null || mDeflectNum == null) {
            Log.e(TAG, "displayDeflectNumEditor PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mDeflectNum);
        mDeflectNum.setOnPreferenceChangeListener(null);

        //Enable Call deflection editor depending on platform support
        if (isCallDeflectionOrTransferEnabled()) {
            mScreen.addPreference(mDeflectNum);
            mDeflectNum.setOnPreferenceChangeListener(this);

            String number = QtiImsExtUtils.getCallDeflectNumber(
                    getApplicationContext().getContentResolver());
            if (number != null) {
                mDeflectNum.setText(number);
                mDeflectNum.setSummary(number);
            } else {
                mDeflectNum.setText("");
                mDeflectNum.setSummary(getString(R.string.qti_ims_number_not_set));
            }
        }
    }

    private void displayRttOption() {
        if (mRttModePreference == null) {
            Log.e(TAG, "displayRttOption PreferenceScreen is invalid");
            return;
        }

        mScreen.removePreference(mRttModePreference);
        mRttModePreference.setOnPreferenceChangeListener(null);

        //Enable RTT settings depending on RTT Config
        if (QtiImsExtUtils.isRttSupported(mPhoneId, getApplicationContext())) {
            mScreen.addPreference(mRttModePreference);
            mRttModePreference.setOnPreferenceChangeListener(this);
            mRttModePreference.setChecked(QtiImsExtUtils.isRttOn(getApplicationContext()));
            Log.d(TAG, "RTT: UI Option = " + mRttModePreference.isChecked());
        }
    }

    // Update the RTT mode in required places
    private void setRttMode(boolean enabling) {
        Log.d(TAG, "RTT: setRttMode value = " + enabling);

        // Update the system setting
        QtiImsExtUtils.setRttMode(enabling, getApplicationContext());

        // Update the UI to reflect system setting
        mRttModePreference.setChecked(enabling);

        // Update the NV via config
        try {
            if (mImsConfig != null) {
                mImsConfig.setProvisionedValue(ImsConfig.ConfigConstants.RTT_SETTING_ENABLED,
                        enabling ? 1 : 0);
            }
        } catch (ImsException e) {
            Log.e(TAG, "RTT: setRttMode failed. Exception = " + e);
            //Re-set this back due to failure
            mRttModePreference.setChecked(!enabling);
        }
    }

    private boolean isCallStateIdle() {
        for (int i = 0; i < mCallState.length; i++) {
            if (TelephonyManager.CALL_STATE_IDLE != mCallState[i]) {
                return false;
            }
        }
        return true;
    }

}

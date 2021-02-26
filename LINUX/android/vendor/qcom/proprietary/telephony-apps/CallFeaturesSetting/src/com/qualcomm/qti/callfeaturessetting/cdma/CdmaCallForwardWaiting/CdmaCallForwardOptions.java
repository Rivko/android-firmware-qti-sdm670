/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.callfeaturessetting.cdma.CdmaCallForwardWaiting;

import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.preference.PreferenceActivity;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.MenuItem;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.SubscriptionController;

import java.util.ArrayList;
import com.qualcomm.qti.callfeaturessetting.R;

public class CdmaCallForwardOptions extends PreferenceActivity {
    private static final String LOG_TAG = "CdmaCallForwardOptions";
    private final boolean DBG = true;

    private static final String NUM_PROJECTION[] = {
            Phone.NUMBER
    };

    private static final int CATEGORY_NORMAL = 1;

    private static final String BUTTON_CFU_KEY = "button_cfu_key";
    private static final String BUTTON_CFB_KEY = "button_cfb_key";
    private static final String BUTTON_CFNRY_KEY = "button_cfnry_key";
    private static final String BUTTON_CFNRC_KEY = "button_cfnrc_key";

    private static final String BUTTON_CFU_DEACT_KEY = "button_cfu_deact_key";
    private static final String BUTTON_CFB_DEACT_KEY = "button_cfb_deact_key";
    private static final String BUTTON_CFNRY_DEACT_KEY = "button_cfnry_deact_key";
    private static final String BUTTON_CFNRC_DEACT_KEY = "button_cfnrc_deact_key";
    private static final String BUTTON_CF_DEACT_ALL_KEY = "button_cf_deact_all_key";

    private CdmaCallForwardEditPreference mButtonCFU;
    private CdmaCallForwardEditPreference mButtonCFB;
    private CdmaCallForwardEditPreference mButtonCFNRy;
    private CdmaCallForwardEditPreference mButtonCFNRc;

    private PreferenceScreen mCfuDeactPref;
    private PreferenceScreen mCfbDeactPref;
    private PreferenceScreen mCfnryDeactPref;
    private PreferenceScreen mCfnrcDeactPref;
    private PreferenceScreen mCfDeactAllPref;

    private final ArrayList<CdmaCallForwardEditPreference> mPreferences =
            new ArrayList<CdmaCallForwardEditPreference>();
    private final ArrayList<PreferenceScreen> mDeactPreScreens =
            new ArrayList<PreferenceScreen>();

    private boolean mFirstResume;
    private int mSubId = 0;
    private PhoneAccountHandle mAccountHandle;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        addPreferencesFromResource(R.xml.callforward_cdma_options);

        // getting selected subscription
        mSubId = getIntent().getIntExtra(PhoneConstants.SUBSCRIPTION_KEY,
                SubscriptionManager.DEFAULT_SUBSCRIPTION_ID);
        Log.d(LOG_TAG, "Inside CF options, Getting subscription = " + mSubId);

        PreferenceScreen prefSet = getPreferenceScreen();

        mButtonCFU = (CdmaCallForwardEditPreference) prefSet.findPreference(BUTTON_CFU_KEY);
        mButtonCFB = (CdmaCallForwardEditPreference) prefSet.findPreference(BUTTON_CFB_KEY);
        mButtonCFNRy = (CdmaCallForwardEditPreference) prefSet.findPreference(BUTTON_CFNRY_KEY);
        mButtonCFNRc = (CdmaCallForwardEditPreference) prefSet.findPreference(BUTTON_CFNRC_KEY);

        mButtonCFU.setParentActivity(this, mButtonCFU.reason);
        mButtonCFB.setParentActivity(this, mButtonCFB.reason);
        mButtonCFNRy.setParentActivity(this, mButtonCFNRy.reason);
        mButtonCFNRc.setParentActivity(this, mButtonCFNRc.reason);

        mPreferences.add(mButtonCFU);
        mPreferences.add(mButtonCFB);
        mPreferences.add(mButtonCFNRy);
        mPreferences.add(mButtonCFNRc);

        mCfuDeactPref = (PreferenceScreen) prefSet.findPreference(BUTTON_CFU_DEACT_KEY);
        mCfbDeactPref = (PreferenceScreen) prefSet.findPreference(BUTTON_CFB_DEACT_KEY);
        mCfnryDeactPref = (PreferenceScreen) prefSet.findPreference(BUTTON_CFNRY_DEACT_KEY);
        mCfnrcDeactPref = (PreferenceScreen) prefSet.findPreference(BUTTON_CFNRC_DEACT_KEY);
        mCfDeactAllPref = (PreferenceScreen) prefSet.findPreference(BUTTON_CF_DEACT_ALL_KEY);

        mDeactPreScreens.add(mCfuDeactPref);
        mDeactPreScreens.add(mCfbDeactPref);
        mDeactPreScreens.add(mCfnryDeactPref);
        mDeactPreScreens.add(mCfnrcDeactPref);
        mDeactPreScreens.add(mCfDeactAllPref);

        mFirstResume = true;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mFirstResume) {
            int phoneId = SubscriptionController.getInstance().getPhoneId(mSubId);
            mAccountHandle = GlobalUtils.getPhoneAccountHandle(this, phoneId);
            for (int i = CommandsInterface.CF_REASON_UNCONDITIONAL;
                    i < CommandsInterface.CF_REASON_ALL_CONDITIONAL; i++) {
                initChildPreference(i, phoneId);
                setChildPreference(i);
                if (DBG) Log.d(LOG_TAG, "call option on type: " + i + " Getting deact num ="
                        + GlobalUtils.getDeactivateNumber(this, mSubId, i));
            }
            mFirstResume = false;
        }
    }

    private void initChildPreference(int index, int phoneId) {
        if ((mPreferences != null) && (index < mPreferences.size())) {
            mPreferences.get(index).init(this, phoneId, GlobalUtils
                    .getActivateNumber(this, mSubId, index));
        }
    }

    private void setChildPreference(int index) {
        if (mDeactPreScreens != null) {
            mDeactPreScreens.get(index).setSummary(
                    GlobalUtils.getDeactivateNumber(this, mSubId, index));
            mDeactPreScreens.get(index).getIntent().setData(Uri.fromParts("tel",
                    GlobalUtils.getDeactivateNumber(this, mSubId, index), null));
            if (mAccountHandle != null) {
                mDeactPreScreens.get(index).getIntent()
                        .putExtra(TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE, mAccountHandle);
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (DBG) Log.d(LOG_TAG, "onActivityResult: done");
        if (resultCode != RESULT_OK) {
            if (DBG) Log.d(LOG_TAG, "onActivityResult: contact picker result not OK.");
            return;
        }
        Cursor cursor = getContentResolver().query(data.getData(),
                NUM_PROJECTION, null, null, null);
        if ((cursor == null) || (!cursor.moveToFirst())) {
            if (DBG) Log.d(LOG_TAG, "onActivityResult: bad contact data, no results found.");
            return;
        }

        switch (requestCode) {
            case CommandsInterface.CF_REASON_UNCONDITIONAL:
                mButtonCFU.onPickActivityResult(cursor.getString(0));
                break;
            case CommandsInterface.CF_REASON_BUSY:
                mButtonCFB.onPickActivityResult(cursor.getString(0));
                break;
            case CommandsInterface.CF_REASON_NO_REPLY:
                mButtonCFNRy.onPickActivityResult(cursor.getString(0));
                break;
            case CommandsInterface.CF_REASON_NOT_REACHABLE:
                mButtonCFNRc.onPickActivityResult(cursor.getString(0));
                break;
            default:
                break;

        }
        if (cursor != null) {
            cursor.close();
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            onBackPressed();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}

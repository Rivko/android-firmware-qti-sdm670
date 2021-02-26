/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2008 The Android Open Source Project
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

package com.qualcomm.qti.networksetting;

import android.content.Intent;
import android.os.PersistableBundle;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.text.TextUtils;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;

/**
 * List of Phone-specific settings screens.
 */
public class CdmaOptions {
    private static final String LOG_TAG = "CdmaOptions";

    private CdmaSystemSelectListPreference mButtonCdmaSystemSelect;
    private CdmaSubscriptionListPreference mButtonCdmaSubscription;
    private Preference mButtonAPNExpand;
    private Preference mCategoryAPNExpand;
    private Preference mButtonCarrierSettings;

    private static final String BUTTON_CDMA_SYSTEM_SELECT_KEY = "cdma_system_select_key";
    private static final String BUTTON_CDMA_SUBSCRIPTION_KEY = "cdma_subscription_key";
    private static final String BUTTON_CARRIER_SETTINGS_KEY = "carrier_settings_key";
    private static final String BUTTON_APN_EXPAND_KEY = "button_cdma_apn_key";
    private static final String CATEGORY_APN_EXPAND_KEY = "category_cdma_apn_key";

    private PreferenceFragment mPrefFragment;
    private PreferenceScreen mPrefScreen;
    private Phone mPhone;
    private CarrierConfigManager mConfigManager;

    public CdmaOptions(PreferenceFragment prefFragment, PreferenceScreen prefScreen, Phone phone) {
        mPrefFragment = prefFragment;
        mPrefScreen = prefScreen;
        mPrefFragment.addPreferencesFromResource(R.xml.cdma_options);

        // Initialize preferences.
        mButtonCdmaSystemSelect = (CdmaSystemSelectListPreference) mPrefScreen
                .findPreference(BUTTON_CDMA_SYSTEM_SELECT_KEY);
        mButtonCdmaSubscription = (CdmaSubscriptionListPreference) mPrefScreen
                .findPreference(BUTTON_CDMA_SUBSCRIPTION_KEY);
        mButtonCarrierSettings = mPrefScreen.findPreference(BUTTON_CARRIER_SETTINGS_KEY);
        mButtonAPNExpand = mPrefScreen.findPreference(BUTTON_APN_EXPAND_KEY);
        mCategoryAPNExpand = mPrefScreen.findPreference(CATEGORY_APN_EXPAND_KEY);
        mConfigManager = (CarrierConfigManager) mPrefFragment
                .getActivity().getSystemService(mPrefFragment.getContext().CARRIER_CONFIG_SERVICE);
        update(phone);
    }

    // Unlike mPrefFragment or mPrefScreen, mPhone may change during lifecycle of CdmaOptions.
    // For example, a new sim card is inserted. When that happens, we update CdmaOptions with new
    // phone.
    protected void update(Phone phone) {
        boolean addAPNExpand = false;
        mPhone = phone;

        PersistableBundle carrierConfig =
                mConfigManager.getConfigForSubId(mPhone.getSubId());
        // Some CDMA carriers want the APN settings.
        if ((mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) &&
                mCategoryAPNExpand != null &&
                carrierConfig != null &&
                carrierConfig.getBoolean(
                CarrierConfigManager.KEY_SHOW_APN_SETTING_CDMA_BOOL)) {
            addAPNExpand = true;
        }
        boolean addCdmaSubscription =
                deviceSupportsNvAndRuim();
        // Read platform settings for carrier settings
        boolean addCarrierSettings =
                carrierConfig.getBoolean(CarrierConfigManager.KEY_CARRIER_SETTINGS_ENABLE_BOOL);

        mPrefScreen.addPreference(mButtonCdmaSystemSelect);
        mButtonCdmaSystemSelect.setEnabled(true);

        // Making no assumptions of whether they are added or removed at this point.
        // Calling add or remove explicitly to make sure they are updated.

        if (addAPNExpand) {
            mButtonAPNExpand.setOnPreferenceClickListener(
                    new Preference.OnPreferenceClickListener() {
                        @Override
                        public boolean onPreferenceClick(Preference preference) {
                            // We need to build the Intent by hand as the Preference Framework
                            // does not allow to add an Intent with some extras into a Preference
                            // XML file
                            final Intent intent = new Intent(Settings.ACTION_APN_SETTINGS);
                            // This will setup the Home and Search affordance
                            intent.putExtra(":settings:show_fragment_as_subsetting", true);
                            intent.putExtra("sub_id", mPhone.getSubId());
                            mPrefFragment.startActivity(intent);
                            return true;
                        }
            });
            mPrefScreen.addPreference(mCategoryAPNExpand);
        } else {
            mPrefScreen.removePreference(mCategoryAPNExpand);
        }

        if (addCdmaSubscription) {
            log("Both NV and Ruim supported, ENABLE subscription type selection");
            mPrefScreen.addPreference(mButtonCdmaSubscription);
            mButtonCdmaSubscription.setEnabled(true);
        } else {
            log("Both NV and Ruim NOT supported, REMOVE subscription type selection");
            mPrefScreen.removePreference(mButtonCdmaSubscription);
        }

        if (addCarrierSettings) {
            mPrefScreen.addPreference(mButtonCarrierSettings);
        } else {
            mPrefScreen.removePreference(mButtonCarrierSettings);
        }
    }

    private boolean deviceSupportsNvAndRuim() {
        // retrieve the list of subscription types supported by device.
        String subscriptionsSupported = SystemProperties.get("ril.subscription.types");
        boolean nvSupported = false;
        boolean ruimSupported = false;

        log("deviceSupportsnvAnRum: prop=" + subscriptionsSupported);
        if (!TextUtils.isEmpty(subscriptionsSupported)) {
            // Searches through the comma-separated list for a match for "NV"
            // and "RUIM" to update nvSupported and ruimSupported.
            for (String subscriptionType : subscriptionsSupported.split(",")) {
                subscriptionType = subscriptionType.trim();
                if (subscriptionType.equalsIgnoreCase("NV")) {
                    nvSupported = true;
                }
                if (subscriptionType.equalsIgnoreCase("RUIM")) {
                    ruimSupported = true;
                }
            }
        }

        log("deviceSupportsnvAnRum: nvSupported=" + nvSupported +
                " ruimSupported=" + ruimSupported);
        return (nvSupported && ruimSupported);
    }

    public boolean preferenceTreeClick(Preference preference) {
        if (preference.getKey().equals(BUTTON_CDMA_SYSTEM_SELECT_KEY)) {
            log("preferenceTreeClick: return BUTTON_CDMA_ROAMING_KEY true");
            return true;
        }
        if (preference.getKey().equals(BUTTON_CDMA_SUBSCRIPTION_KEY)) {
            log("preferenceTreeClick: return CDMA_SUBSCRIPTION_KEY true");
            return true;
        }
        return false;
    }

    public void showDialog(Preference preference) {
        if (preference.getKey().equals(BUTTON_CDMA_SYSTEM_SELECT_KEY)) {
            mButtonCdmaSystemSelect.showDialog(null);
        } else if (preference.getKey().equals(BUTTON_CDMA_SUBSCRIPTION_KEY)) {
            mButtonCdmaSubscription.showDialog(null);
        }
    }

    protected void log(String s) {
        android.util.Log.d(LOG_TAG, s);
    }
}

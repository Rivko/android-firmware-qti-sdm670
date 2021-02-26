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
 * Copyright (C) 2006 The Android Open Source Project
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

import android.app.ActionBar;
import android.app.Activity;
import android.app.DialogFragment;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ResolveInfo;

import android.database.ContentObserver;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PersistableBundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.os.UserManager;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceCategory;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;

import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;

import android.telephony.CarrierConfigManager;
import android.telephony.euicc.EuiccManager;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.euicc.EuiccManager;
import android.telephony.ims.feature.ImsFeature;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TabHost;

import com.android.ims.ImsConfig;
import com.android.ims.ImsException;
import com.android.ims.ImsManager;
import com.android.internal.logging.MetricsLogger;
import com.android.internal.logging.nano.MetricsProto.MetricsEvent;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccController;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.codeaurora.internal.IExtTelephony;

/**
 * "Mobile network settings" screen.  This preference screen lets you
 * enable/disable mobile data, and control data roaming and other
 * network-specific mobile data features.  It's used on non-voice-capable
 * tablets as well as regular phone devices.
 *
 * Note that this PreferenceActivity is part of the phone app, even though
 * you reach it from the "Wireless & Networks" section of the main
 * Settings app.  It's not part of the "Call settings" hierarchy that's
 * available from the Phone app (see CallFeaturesSetting for that.)
 */

public class MobileNetworkSettings extends Activity  {

    private enum TabState {
        NO_TABS, UPDATE, DO_NOTHING
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.network_setting);

        FragmentManager fragmentManager = getFragmentManager();
        Fragment fragment = fragmentManager.findFragmentById(R.id.network_setting_content);
        if (fragment == null) {
            fragmentManager.beginTransaction()
                    .add(R.id.network_setting_content, new MobileNetworkFragment())
                    .commit();
        }
    }

    @Override
    public boolean onOptionsItemSelected(final MenuItem item) {
        final int itemId = item.getItemId();
        switch (itemId) {
            // Respond to the action bar's Up/Home button
            case android.R.id.home:
                finish();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }


    /**
     * Whether to show the entry point to eUICC settings.
     *
     * <p>We show the entry point on any device which supports eUICC as long as either the eUICC
     * was ever provisioned (that is, at least one profile was ever downloaded onto it), or if
     * the user has enabled development mode.
     */
    public static boolean showEuiccSettings(Context context) {
        EuiccManager euiccManager =
                (EuiccManager) context.getSystemService(Context.EUICC_SERVICE);
        if (!euiccManager.isEnabled()) {
            return false;
        }
        ContentResolver cr = context.getContentResolver();
        return Settings.Global.getInt(cr, Settings.Global.EUICC_PROVISIONED, 0) != 0
                || Settings.Global.getInt(
                cr, Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, 0) != 0;
    }


    public static class MobileNetworkFragment extends PreferenceFragment implements
            Preference.OnPreferenceChangeListener, RoamingDialogFragment.RoamingDialogListener {

        // debug data
        private static final String LOG_TAG = "MobileNetworkSettings";
        private static final boolean DBG = true;
        public static final int REQUEST_CODE_EXIT_ECM = 17;

        // Number of active Subscriptions to show tabs
        private static final int TAB_THRESHOLD = 2;

        // Number of last phone number digits shown in Euicc Setting tab
        private static final int NUM_LAST_PHONE_DIGITS = 4;
            // fragment tag for roaming data dialog
            private static final String ROAMING_TAG = "RoamingDialogFragment";
        //String keys for preference lookup
        private static final String BUTTON_PREFERED_NETWORK_MODE = "preferred_network_mode_key";
        private static final String BUTTON_PREFERED_NETWORK_CMCC_MODE =
                "preferred_network_mode_cmcc_key";
        private static final String BUTTON_ROAMING_KEY = "button_roaming_key";
        private static final String BUTTON_CDMA_LTE_DATA_SERVICE_KEY = "cdma_lte_data_service_key";
        private static final String BUTTON_UPLMN_KEY = "button_uplmn_key";
        private static final String BUTTON_ENABLED_NETWORKS_KEY = "enabled_networks_key";
        private static final String BUTTON_4G_LTE_KEY = "enhanced_4g_lte";
        private static final String BUTTON_ENABLE_4G_KEY = "enable_4g_settings";
        private static final String BUTTON_CELL_BROADCAST_SETTINGS = "cell_broadcast_settings";
        private static final String BUTTON_CARRIER_SETTINGS_KEY = "carrier_settings_key";
        private static final String BUTTON_CDMA_SYSTEM_SELECT_KEY = "cdma_system_select_key";

        private static final String BUTTON_CARRIER_SETTINGS_EUICC_KEY =
                "carrier_settings_euicc_key";
        private static final String BUTTON_MOBILE_DATA_ENABLE_KEY = "mobile_data_enable";
        private static final String BUTTON_DATA_USAGE_KEY = "data_usage_summary";
        private static final String BUTTON_ADVANCED_OPTIONS_KEY = "advanced_options";
        private static final String CATEGORY_GSM_APN_EXPAND_KEY = "category_gsm_apn_key";
        private static final String CATEGORY_CDMA_APN_EXPAND_KEY = "category_cdma_apn_key";

        private static final String PRIMARY_4G_CARD_PROPERTY_NAME = "persist.radio.detect4gcard";
        private static final String PRIMARY_CARD_PROPERTY_NAME = "persist.radio.primarycard";
        private static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";

        //Carrier mode
        private static final String CARRIER_MODE_CT_CLASS_A = "ct_class_a";
        private static final String CARRIER_MODE_CT_CLASS_C = "ct_class_c";

        // value for subsidy lock resticted state
        private static final int SUBSIDYLOCK_UNLOCKED = 103;
        private static final int PERMANENTLY_UNLOCKED = 100;
        private static final String SUBSIDY_STATUS = "subsidy_status";
        private SubsidySettingsObserver mSubsidySettingsObserver;
        private static final String SUBSIDY_LOCK_SYSTEM_PROPERY
                = "ro.radio.subsidylock";

        private static final String CARRIER_MODE_CMCC = "cmcc";
        private String mCarrierMode = SystemProperties.get("persist.radio.carrier_mode", "default");
        private boolean mIsCTClassC = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_C);
        private boolean mIsCMCC = mCarrierMode.equals(CARRIER_MODE_CMCC);

        public static final String EXTRA_EXIT_ECM_RESULT = "exit_ecm_result";

        private static final boolean PRIMCARYCARD_L_W_ENABLED =
                SystemProperties.getBoolean("persist.radio.lw_enabled", false);

        private final BroadcastReceiver mPhoneChangeReceiver = new PhoneChangeReceiver();

        private int preferredNetworkMode = Phone.PREFERRED_NT_MODE;

        //Information about logical "up" Activity
        private static final String UP_ACTIVITY_PACKAGE = "com.android.settings";
        private static final String UP_ACTIVITY_CLASS =
                "com.android.settings.Settings$WirelessSettingsActivity";

        //Information that needs to save into Bundle.
        private static final String EXPAND_ADVANCED_FIELDS = "expand_advanced_fields";

        private static final String LEGACY_ACTION_CONFIGURE_PHONE_ACCOUNT =
            "android.telecom.action.CONNECTION_SERVICE_CONFIGURE";

        private CarrierConfigManager mConfigManager;
        private SubscriptionManager mSubscriptionManager;
        private TelephonyManager mTelephonyManager;

        private String tabDefaultLabel = "SIM slot ";

        //UI objects
        private AdvancedOptionsPreference mAdvancedOptions;
        private ListPreference mButtonPreferredNetworkMode;
        private ListPreference mButtonEnabledNetworks;
        private ListPreference mButtonPreferredCMCCNetworkMode;
        private RestrictedSwitchPreference mButtonDataRoam;
        private SwitchPreference mButton4glte;
        private SwitchPreference mButtonEnable4g;
        private Preference mLteDataServicePref;
        private Preference mButtonUplmnPref;

        private Preference mEuiccSettingsPref;
        private NetworkSelectListPreference mButtonNetworkSelect;
        private MobileDataPreference mMobileDataPref;
        private DataUsagePreference mDataUsagePref;

        private static final String iface = "rmnet0"; //TODO: this will go away
        private List<SubscriptionInfo> mActiveSubInfos;

        private UserManager mUm;
        private Phone mPhone;
        private static ImsManager mImsMgr;
        private MyHandler mHandler;
        private boolean mOkClicked;
        private boolean mExpandAdvancedFields;

        // We assume the the value returned by mTabHost.getCurrentTab() == slotId
        private TabHost mTabHost;

        //GsmUmts options and Cdma options
        GsmUmtsOptions mGsmUmtsOptions;
        CdmaOptions mCdmaOptions;

        private Preference mClickedPreference;
        private boolean mShow4GForLTE;
        private boolean mIsGlobalCdma;
        private boolean mUnavailable;
        private IExtTelephony mExtTelephony = IExtTelephony.Stub.
                asInterface(ServiceManager.getService("extphone"));
        private static final int NOT_PROVISIONED = 0;

        private int[] mCallState = null;
        private PhoneStateListener[] mPhoneStateListener = null;

        private final BroadcastReceiver mCarrierConfigReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                // Refresh UI after carrier config is changed
                updateBody();
            }
        };

        private boolean isDetect4gCardEnabled() {
            return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) &&
                SystemProperties.getBoolean(PRIMARY_4G_CARD_PROPERTY_NAME, false);
        }

        private void setScreenState() {
            if (mPhone != null) {
                int phoneId = mPhone.getPhoneId();
                int simState = TelephonyManager.getDefault().getSimState(phoneId);
                boolean screenState = simState != TelephonyManager.SIM_STATE_ABSENT;
                log("set sub screenState phoneId=" + phoneId + ", simState=" + simState);
                if (screenState && isDetect4gCardEnabled()) {
                    //primary card feature is enabled
                    int provStatus = NOT_PROVISIONED;
                    try{
                        provStatus = mExtTelephony.
                            getCurrentUiccCardProvisioningStatus(phoneId);
                    } catch (RemoteException ex){
                        loge("RemoteException @setScreenState=" + ex + ", phoneId=" + phoneId);
                    } catch (NullPointerException ex) {
                        loge("NullPointerException @setScreenState=" + ex + ", phoneId=" + phoneId);
                    }
                    screenState = provStatus != NOT_PROVISIONED;
                    log("set sub screenState provStatus=" + provStatus +
                        ", screenState=" + screenState);
                }
                getPreferenceScreen().setEnabled(screenState);
            }
        }
        /**
         * Service connection code for the NetworkQueryService.
         * Handles the work of binding to a local object so that we can make
         * the appropriate service calls.
         */

        /** Local service interface */
        private INetworkQueryService mNetworkQueryService = null;

        private void setNetworkQueryService() {
            mButtonNetworkSelect = (NetworkSelectListPreference) getPreferenceScreen()
                    .findPreference(NetworkOperators.BUTTON_NETWORK_SELECT_KEY);
            if (mButtonNetworkSelect != null) {
                mButtonNetworkSelect.setNetworkQueryService(mNetworkQueryService);
            }

        }
        /** Service connection */
        private final ServiceConnection mNetworkQueryServiceConnection = new ServiceConnection() {

            /** Handle the task of binding the local object to the service */
            public void onServiceConnected(ComponentName className, IBinder service) {
                if (DBG) log("connection created, binding local service.");
                mNetworkQueryService = ((NetworkQueryService.LocalBinder) service).getService();
                setNetworkQueryService();
            }
            /** Handle the task of cleaning up the local binding */
            public void onServiceDisconnected(ComponentName className) {
                if (DBG) log("connection disconnected, cleaning local binding.");
                mNetworkQueryService = null;
                setNetworkQueryService();
            }
        };

        private void bindNetworkQueryService() {
            getContext().startService(new Intent(getContext(), NetworkQueryService.class));
            getContext().bindService(new Intent(getContext(), NetworkQueryService.class).setAction(
                        NetworkQueryService.ACTION_LOCAL_BINDER),
                        mNetworkQueryServiceConnection, Context.BIND_AUTO_CREATE);
        }

        private void unbindNetworkQueryService() {
            // unbind the service.
            getContext().unbindService(mNetworkQueryServiceConnection);
        }

        @Override
        public void onPositiveButtonClick(DialogFragment dialog) {
            mPhone.setDataRoamingEnabled(true);
            mButtonDataRoam.setChecked(true);
        }

        @Override
        public void onViewCreated(View view, Bundle savedInstanceState) {
            if (getListView() != null) {
                getListView().setDividerHeight(1);
            }
        }

        /**
         * Invoked on each preference click in this hierarchy, overrides
         * PreferenceActivity's implementation.  Used to make sure we track the
         * preference click events.
         */
        @Override
        public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
                                                 Preference preference) {
            sendMetricsEventPreferenceClicked(preferenceScreen, preference);
            /** TODO: Refactor and get rid of the if's using subclasses */
            final int phoneSubId = mPhone.getSubId();
            if (preference.getKey().equals(BUTTON_4G_LTE_KEY)) {
                return true;
            } else if (preference.getKey().equals(BUTTON_ENABLE_4G_KEY)) {
                return true;
            } else if (mGsmUmtsOptions != null &&
                    mGsmUmtsOptions.preferenceTreeClick(preference) == true) {
                return true;
            } else if (mCdmaOptions != null &&
                       mCdmaOptions.preferenceTreeClick(preference) == true) {
                if (mPhone.isInEcm()) {

                    mClickedPreference = preference;

                    // In ECM mode launch ECM app dialog
                    startActivityForResult(
                        new Intent(TelephonyIntents.ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, null),
                        REQUEST_CODE_EXIT_ECM);
                }
                return true;
            } else if (preference == mButtonPreferredNetworkMode) {
                //displays the value taken from the Settings.System
                int settingsNetworkMode = getPreferredNetworkModeForSubId();
                mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
                return true;
            }  else if (preference == mButtonPreferredCMCCNetworkMode) {
                //displays the value taken from the Settings.System
                int settingsNetworkMode = getPreferredNetworkModeForSubId();
                mButtonPreferredCMCCNetworkMode.setValue(Integer.toString(settingsNetworkMode));
                return true;
            } else if (preference == mLteDataServicePref) {
                String tmpl = android.provider.Settings.Global.getString(
                            getContext().getContentResolver(),
                            android.provider.Settings.Global.SETUP_PREPAID_DATA_SERVICE_URL);
                if (!TextUtils.isEmpty(tmpl)) {
                    String imsi = mTelephonyManager.getSubscriberId();
                    if (imsi == null) {
                        imsi = "";
                    }
                    final String url = TextUtils.isEmpty(tmpl) ? null
                            : TextUtils.expandTemplate(tmpl, imsi).toString();
                    Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                    startActivity(intent);
                } else {
                    android.util.Log.e(LOG_TAG, "Missing SETUP_PREPAID_DATA_SERVICE_URL");
                }
                return true;
            }  else if (preference == mButtonEnabledNetworks) {
                int settingsNetworkMode = getPreferredNetworkModeForSubId();
                mButtonEnabledNetworks.setValue(Integer.toString(settingsNetworkMode));
                return true;
            } else if (preference == mButtonDataRoam) {
                // Do not disable the preference screen if the user clicks Data roaming.
                return true;
            } else if (preference == mEuiccSettingsPref) {
                Intent intent = new Intent(EuiccManager.ACTION_MANAGE_EMBEDDED_SUBSCRIPTIONS);
                startActivity(intent);
                return true;
            } else if (preference == mMobileDataPref || preference == mDataUsagePref) {
                return false;
            } else if (preference == mAdvancedOptions) {
                mExpandAdvancedFields = true;
                updateBody();
                return true;
            } else {
                // if the button is anything but the simple toggle preference,
                // we'll need to disable all preferences to reject all click
                // events until the sub-activity's UI comes up.
                preferenceScreen.setEnabled(false);
                // Let the intents be launched by the Preference manager
                return false;
            }
        }

        private final SubscriptionManager.OnSubscriptionsChangedListener
                mOnSubscriptionsChangeListener
                = new SubscriptionManager.OnSubscriptionsChangedListener() {
            @Override
            public void onSubscriptionsChanged() {
                if (DBG) log("onSubscriptionsChanged:");
                List<SubscriptionInfo> newSil =
                        mSubscriptionManager.getActiveSubscriptionInfoList();
                if (DBG) log("onSubscriptionsChanged: newSil: " + newSil +
                        " mActiveSubInfos: " + mActiveSubInfos);
                if (newSil == null) {
                    initializeSubscriptions();
                    return;
                }
                // Update UI when there is a change in number of active subscriptions or
                // there is a change in display name or subID
                if (mActiveSubInfos == null || mActiveSubInfos.size() != newSil.size()) {
                    initializeSubscriptions();
                } else {
                    boolean subsChanged = false;
                    outer:
                    for (SubscriptionInfo si : mActiveSubInfos) {
                        for(SubscriptionInfo newSi : newSil) {
                            //Compare SubscriptionInfo of same slot
                            if (si.getSimSlotIndex() == newSi.getSimSlotIndex()) {
                                if (DBG) log("onSubscriptionsChanged: Slot matched SimSlotIndex: "
                                        + si.getSimSlotIndex());
                                if (!newSi.getDisplayName().equals(si.getDisplayName()) ||
                                        newSi.getSubscriptionId() != si.getSubscriptionId()) {
                                    if (DBG) log("onSubscriptionsChanged: subs changed ");
                                    subsChanged = true;
                                    break outer;
                                }
                            }
                        }
                    }
                    if (subsChanged) {
                        initializeSubscriptions();
                    }
                }
            }
        };

        private void initializeSubscriptions() {
            final Activity activity = getActivity();
            if (activity == null || activity.isDestroyed()) {
                    // Process preferences in activity only if its not destroyed
                return;
            }
            int currentTab = 0;
            if (DBG) log("initializeSubscriptions:+");

            // Before updating the the active subscription list check
            // if tab updating is needed as the list is changing.
            List<SubscriptionInfo> sil = mSubscriptionManager.getActiveSubscriptionInfoList();
            // Display all tabs according to configuration in Multi Sim mode
            TelephonyManager tm = (TelephonyManager) activity.getSystemService(
                    Context.TELEPHONY_SERVICE);
            int phoneCount = tm.getPhoneCount();
            MobileNetworkSettings.TabState state = TabState.UPDATE;
            if (phoneCount < 2) {
                state = isUpdateTabsNeeded(sil);
            }

            // Update to the active subscription list
            mActiveSubInfos.clear();
            if (sil != null) {
                mActiveSubInfos.addAll(sil);
                // If there is only 1 sim then currenTab should represent slot no. of the sim.
                if (sil.size() == 1) {
                    currentTab = sil.get(0).getSimSlotIndex();
                }
            }

            switch (state) {
                case UPDATE: {
                    if (DBG) log("initializeSubscriptions: UPDATE");
                    currentTab = mTabHost != null ? mTabHost.getCurrentTab() : 0;

                    mTabHost = (TabHost) activity.findViewById(android.R.id.tabhost);
                    mTabHost.setup();

                    if (mTabHost != null) {
                        if (DBG) log("initializeSubscriptions: mTabHost clearTabs");
                        mTabHost.clearAllTabs();
                    }

                    // Update the tabName. Get tab name from SubscriptionInfo,
                    // if SubscriptionInfo not available get default tabName for that slot.
                    for (int simSlotIndex = 0; simSlotIndex < phoneCount; simSlotIndex++) {
                        String tabName = null;
                        for(SubscriptionInfo si : mActiveSubInfos) {
                            if (DBG) log("initializeSubscriptions: si: " + si);
                            if (si != null && si.getSimSlotIndex() == simSlotIndex) {
                                // Slot is not empty and we match
                                tabName = String.valueOf(si.getDisplayName());
                                break;
                            }
                        }
                        if (tabName == null) {
                            try {
                                Context con = activity.
                                        createPackageContext("com.android.settings", 0);
                                int id = con.getResources().getIdentifier("sim_editor_title",
                                        "string", "com.android.settings");
                                tabName = con.getResources().getString(id, simSlotIndex + 1);
                            } catch (NameNotFoundException e) {
                                loge("NameNotFoundException for sim_editor_title");
                                tabName = tabDefaultLabel + simSlotIndex;
                            }
                        }
                        if (DBG) {
                            log("initializeSubscriptions: tab=" +
                                    simSlotIndex + " name=" + tabName);
                        }

                        mTabHost.addTab(buildTabSpec(String.valueOf(simSlotIndex), tabName));
                    }

                    mTabHost.setOnTabChangedListener(mTabListener);
                    mTabHost.setCurrentTab(currentTab);
                    break;
                }
                case NO_TABS: {
                    if (DBG) log("initializeSubscriptions: NO_TABS");

                    if (mTabHost != null) {
                        mTabHost.clearAllTabs();
                        mTabHost = null;
                    }
                    break;
                }
                case DO_NOTHING: {
                    if (DBG) log("initializeSubscriptions: DO_NOTHING");
                    if (mTabHost != null) {
                        currentTab = mTabHost.getCurrentTab();
                    }
                    break;
                }
            }
            updatePhone(currentTab);
            updateBody();
            if (DBG) log("initializeSubscriptions:-");
        }

        private MobileNetworkSettings.TabState isUpdateTabsNeeded(List<SubscriptionInfo> newSil) {
            TabState state = MobileNetworkSettings.TabState.DO_NOTHING;
            if (newSil == null) {
                if (mActiveSubInfos.size() >= TAB_THRESHOLD) {
                    if (DBG) log("isUpdateTabsNeeded: NO_TABS, size unknown and was tabbed");
                    state = MobileNetworkSettings.TabState.NO_TABS;
                }
            } else if (newSil.size() < TAB_THRESHOLD && mActiveSubInfos.size() >= TAB_THRESHOLD) {
                if (DBG) log("isUpdateTabsNeeded: NO_TABS, size went to small");
                state = MobileNetworkSettings.TabState.NO_TABS;
            } else if (newSil.size() >= TAB_THRESHOLD && mActiveSubInfos.size() < TAB_THRESHOLD) {
                if (DBG) log("isUpdateTabsNeeded: UPDATE, size changed");
                state = MobileNetworkSettings.TabState.UPDATE;
            } else if (newSil.size() >= TAB_THRESHOLD) {
                Iterator<SubscriptionInfo> siIterator = mActiveSubInfos.iterator();
                for(SubscriptionInfo newSi : newSil) {
                    SubscriptionInfo curSi = siIterator.next();
                    if (!newSi.getDisplayName().equals(curSi.getDisplayName())) {
                        if (DBG) log("isUpdateTabsNeeded: UPDATE, new name="
                                    + newSi.getDisplayName());
                        state = MobileNetworkSettings.TabState.UPDATE;
                        break;
                    }
                }
            }
            if (DBG) {
                Log.i(LOG_TAG, "isUpdateTabsNeeded:- " + state
                    + " newSil.size()=" + ((newSil != null) ? newSil.size() : 0)
                    + " mActiveSubInfos.size()=" + mActiveSubInfos.size());
            }
            return state;
        }

        private TabHost.OnTabChangeListener mTabListener = new TabHost.OnTabChangeListener() {
            @Override
            public void onTabChanged(String tabId) {
                if (DBG) log("onTabChanged: " + tabId);
                // The User has changed tab; update the body.
                updatePhone(Integer.parseInt(tabId));
                updateBody();
            }
        };

        private void updatePhone(int slotId) {
            mPhone = PhoneFactory.getPhone(slotId);
            if (mPhone == null) {
                // Do the best we can
                mPhone = PhoneFactory.getDefaultPhone();
            }
            preferredNetworkMode = getPreferredNetworkModeForPhoneId();
            Log.i(LOG_TAG, "updatePhone:- slotId=" + slotId);
            mImsMgr = ImsManager.getInstance(mPhone.getContext(), mPhone.getPhoneId());
            if (mImsMgr == null) {
                Log.i(LOG_TAG, "updatePhone :: Could not get ImsManager instance!");
            } else if (DBG) {
                Log.i(LOG_TAG, "updatePhone :: mImsMgr=" + mImsMgr);
            }
        }

        private TabHost.TabContentFactory mEmptyTabContent = new TabHost.TabContentFactory() {
            @Override
            public View createTabContent(String tag) {
                return new View(mTabHost.getContext());
            }
        };

        private TabHost.TabSpec buildTabSpec(String tag, String title) {
            return mTabHost.newTabSpec(tag).setIndicator(title).setContent(
                    mEmptyTabContent);
        }

        @Override
        public void onSaveInstanceState(Bundle outState) {
            super.onSaveInstanceState(outState);

            // If advanced fields are already expanded, we save it and expand it
            // when it's re-created.
            outState.putBoolean(EXPAND_ADVANCED_FIELDS, mExpandAdvancedFields);
        }

        @Override
        public void onCreate(Bundle icicle) {
            Log.i(LOG_TAG, "onCreate:+");
            super.onCreate(icicle);

            final Activity activity = getActivity();
            activity.setTheme(android.R.style.Theme_Material_Settings);
            if (activity == null || activity.isDestroyed()) {
                Log.e(LOG_TAG, "onCreate:- with no valid activity.");
                return;
            }

            mHandler = new MyHandler();
            mUm = (UserManager) activity.getSystemService(Context.USER_SERVICE);
            mSubscriptionManager = SubscriptionManager.from(activity);
            mTelephonyManager = (TelephonyManager) activity.getSystemService(
                            Context.TELEPHONY_SERVICE);

            if (icicle != null) {
                mExpandAdvancedFields = icicle.getBoolean(EXPAND_ADVANCED_FIELDS, false);
            }

            bindNetworkQueryService();

            mConfigManager = (CarrierConfigManager)
                    activity.getSystemService(Context.CARRIER_CONFIG_SERVICE);
            if (mUm.hasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
                mUnavailable = true;
                activity.setContentView(R.layout.telephony_disallowed_preference_screen);
                return;
            }

            addPreferencesFromResource(R.xml.network_setting_fragment);

            mButton4glte = (SwitchPreference)findPreference(BUTTON_4G_LTE_KEY);
            mButton4glte.setOnPreferenceChangeListener(this);
            mButtonEnable4g = (SwitchPreference)findPreference(BUTTON_ENABLE_4G_KEY);
            mMobileDataPref = (MobileDataPreference) findPreference(BUTTON_MOBILE_DATA_ENABLE_KEY);
            mDataUsagePref = (DataUsagePreference) findPreference(BUTTON_DATA_USAGE_KEY);

            try {
                Context con = activity.createPackageContext("com.android.systemui", 0);
                int id = con.getResources().getIdentifier("config_show4GForLTE",
                        "bool", "com.android.systemui");
                mShow4GForLTE = con.getResources().getBoolean(id);
            } catch (PackageManager.NameNotFoundException e) {
                Log.e(LOG_TAG, "NameNotFoundException for show4GFotLTE");
                mShow4GForLTE = false;
            }

            //get UI object references
            PreferenceScreen prefSet = getPreferenceScreen();

            mButtonDataRoam = (RestrictedSwitchPreference) prefSet.findPreference(
                    BUTTON_ROAMING_KEY);
            mButtonPreferredNetworkMode = (ListPreference) prefSet.findPreference(
                    BUTTON_PREFERED_NETWORK_MODE);
            mButtonEnabledNetworks = (ListPreference) prefSet.findPreference(
                    BUTTON_ENABLED_NETWORKS_KEY);
            mAdvancedOptions = (AdvancedOptionsPreference) prefSet.findPreference(
                    BUTTON_ADVANCED_OPTIONS_KEY);

            mButtonPreferredCMCCNetworkMode = (ListPreference) prefSet.findPreference(
                    BUTTON_PREFERED_NETWORK_CMCC_MODE);
            mButtonDataRoam.setOnPreferenceChangeListener(this);

            mLteDataServicePref = prefSet.findPreference(BUTTON_CDMA_LTE_DATA_SERVICE_KEY);

            mEuiccSettingsPref = prefSet.findPreference(BUTTON_CARRIER_SETTINGS_EUICC_KEY);
            mEuiccSettingsPref.setOnPreferenceChangeListener(this);

            mButtonUplmnPref = prefSet.findPreference(BUTTON_UPLMN_KEY);

            // Initialize mActiveSubInfo
            int max = mSubscriptionManager.getActiveSubscriptionInfoCountMax();
            mActiveSubInfos = new ArrayList<SubscriptionInfo>(max);

            if (mIsCTClassC) {
                mButtonEnable4g.setOnPreferenceChangeListener(this);
            } else {
                prefSet.removePreference(mButtonEnable4g);
                mButtonEnable4g = null;
            }

            IntentFilter intentFilter = new IntentFilter(
                    TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED);
            intentFilter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
            intentFilter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
            activity.registerReceiver(mPhoneChangeReceiver, intentFilter);
            activity.registerReceiver(mCarrierConfigReceiver, new IntentFilter(
                    CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED));

            mPhoneStateListener =
                    new PhoneStateListener[TelephonyManager.getDefault().getPhoneCount()];
            mCallState = new int[mPhoneStateListener.length];

            if (isSubsidyLockFeatureEnabled()) {
                mSubsidySettingsObserver = new SubsidySettingsObserver();
                getContext().getContentResolver().registerContentObserver(
                        Settings.Secure.getUriFor(SUBSIDY_STATUS),
                        false, mSubsidySettingsObserver);
            }
            Log.i(LOG_TAG, "onCreate:-");
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            return inflater.inflate(com.android.internal.R.layout.common_tab_settings,
                    container, false);
        }

        @Override
        public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);
            initializeSubscriptions();
        }

        private class PhoneChangeReceiver extends BroadcastReceiver {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (action.equals(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED)) {
                    int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                            SubscriptionManager.INVALID_PHONE_INDEX);
                    if (DBG) log("onReceive: phoneId: " + phoneId);
                    //Update UI if RAT change is on current slot/phone TAB.
                    if (mPhone.getPhoneId() == phoneId) {
                        // When the radio changes (ex: CDMA->GSM), refresh all options.
                        updateBody();
                    }
                } else if (action.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED) ||
                        action.equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                    setScreenState();
                }
            }
        }

        private class SubsidySettingsObserver extends ContentObserver {

            public SubsidySettingsObserver() {
                super(null);
            }

            @Override
            public void onChange(boolean selfChange) {
                mSubsidySettingsHandler.sendEmptyMessage(0);
            }
        }

        private Handler mSubsidySettingsHandler = new Handler() {
            public void handleMessage(Message msg) {
                updateBody();
            }
        };

        private static boolean isSubsidyLockFeatureEnabled() {
            int propVal = SystemProperties.getInt(SUBSIDY_LOCK_SYSTEM_PROPERY, 0);
            return (propVal == 1);
        }

        private boolean checkForCtCard(String iccId) {
            // iin length is 6 or 7
            if (iccId == null || iccId.length() < 6) {
                return false;
            }
            boolean isCtCard = false;
            String ctIccIdPrefix = iccId.substring(0, 6);
            String[] ctIccIdList = getResources().getStringArray(R.array.ct_iccid_prefix_list);
            if (ctIccIdList != null) {
                for (String iccidPrefix : ctIccIdList) {
                    if (ctIccIdPrefix.equals(iccidPrefix)) {
                        isCtCard = true;
                        break;
                    }
                }
            }
            return isCtCard;
        }

        private boolean checkForCmccCard(String iccId) {
            // iin length is 6 or 7
            if (iccId == null || iccId.length() < 6) {
            return false;
            }
            boolean isCmccCard = false;
            String cmccIccIdPrefix = iccId.substring(0, 6);
            String[] cmccIccIdList = getResources().getStringArray(R.array.cmcc_iccid_prefix_list);
            if (cmccIccIdPrefix != null) {
                for (String iccidPrefix : cmccIccIdList) {
                    if (cmccIccIdPrefix.equals(iccidPrefix)) {
                        isCmccCard = true;
                        break;
                    }
                }
            }
            return isCmccCard;
        }

        private boolean isCmccCardBySubId (int subId) {
            SubscriptionInfo subInfo =
                    SubscriptionManager.from(mPhone.getContext()).getActiveSubscriptionInfo(subId);
            boolean isCmccCard = false;
            if (subInfo != null ) {
                String iccId = subInfo.getIccId();
                isCmccCard = checkForCmccCard(iccId);
            }
            return isCmccCard;
        }

        private void handleEnable4gChange() {
            int networkType = Phone.NT_MODE_GSM_ONLY;
            boolean isCtCard = false;

            //Disable the option until response is received
            mButtonEnable4g.setEnabled(false);

            int subId = mPhone.getSubId();
            List<SubscriptionInfo> sirList =
                    SubscriptionManager.from(mPhone.getContext()).getActiveSubscriptionInfoList();
            if (sirList != null ) {
                for (SubscriptionInfo sir : sirList) {
                    if (sir != null && sir.getSimSlotIndex() >= 0
                            && sir.getSubscriptionId() == subId) {
                        String iccId = sir.getIccId();
                        isCtCard = checkForCtCard(iccId);
                        break;
                    }
                }
            }

            if (isCtCard) {
                networkType = mButtonEnable4g.isChecked() ? Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA
                        : Phone.NT_MODE_GLOBAL;
            } else {
                networkType = mButtonEnable4g.isChecked() ? Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA
                        : Phone.NT_MODE_TDSCDMA_GSM_WCDMA;
            }
            log("Enable 4G option: isCtCard - " + isCtCard + ", set networkType - " + networkType);
            if (getPreferenceScreen().findPreference(BUTTON_PREFERED_NETWORK_MODE) != null)  {
                mButtonPreferredNetworkMode.setValue(Integer.toString(networkType));
            }
            if (getPreferenceScreen().findPreference(BUTTON_ENABLED_NETWORKS_KEY) != null)  {
                mButtonEnabledNetworks.setValue(Integer.toString(networkType));
            }
            mPhone.setPreferredNetworkType(networkType,
                    mHandler.obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
        }

        private boolean isNwModeLte() {
            //if present nwMode is LTE return true.
            int type = getPreferredNetworkModeForSubId();
            if (type == Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA
                    || type == Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA
                    || type == Phone.NT_MODE_LTE_TDSCDMA_WCDMA
                    || type == Phone.NT_MODE_LTE_TDSCDMA_GSM
                    || type == Phone.NT_MODE_LTE_TDSCDMA
                    || type == Phone.NT_MODE_LTE_WCDMA
                    || type == Phone.NT_MODE_LTE_ONLY
                    || type == Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA
                    || type == Phone.NT_MODE_LTE_GSM_WCDMA
                    || type == Phone.NT_MODE_LTE_CDMA_AND_EVDO
                    ) {
                return true;
            }
            return false;
        }

        private void updateButtonEnable4g() {
            if (mButtonEnable4g == null) {
                return;
            }
            boolean enabled = false;

            mButtonEnable4g.setChecked(isNwModeLte());

            int simState = TelephonyManager.getDefault().getSimState(mPhone.getPhoneId());
            enabled = (android.provider.Settings.System.getInt(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.System.AIRPLANE_MODE_ON, 0) == 0)
                    && (simState == TelephonyManager.SIM_STATE_READY);

            //if there is no valid subid to get IccId info, disable the option.
            int subId = mPhone.getSubId();
            List<SubscriptionInfo> sirList =
                    SubscriptionManager.from(mPhone.getContext()).getActiveSubscriptionInfoList();
            if (enabled && sirList != null ) {
                for (SubscriptionInfo sir : sirList) {
                    if (sir != null && sir.getSimSlotIndex() >= 0
                            && sir.getSubscriptionId() == subId) {
                        String iccId = sir.getIccId();
                        enabled = (iccId != null && iccId.length() >= 6) ? true : false;
                        break;
                    }
                }
            }
            mButtonEnable4g.setEnabled(enabled);
        }

        @Override
        public void onDestroy() {
            unbindNetworkQueryService();
            super.onDestroy();
            if (getActivity() != null/* && !getActivity().isDestroyed()*/) {
                getActivity().unregisterReceiver(mPhoneChangeReceiver);
                getActivity().unregisterReceiver(mCarrierConfigReceiver);
            }
            if (mSubsidySettingsObserver != null) {
                getContext().getContentResolver().
                        unregisterContentObserver(mSubsidySettingsObserver);
            }
        }

        @Override
        public void onResume() {
            super.onResume();
            Log.i(LOG_TAG, "onResume:+");

            if (mUnavailable) {
                Log.i(LOG_TAG, "onResume:- ignore mUnavailable == false");
                return;
            }
            final Activity activity = getActivity();
            if (activity == null || activity.isDestroyed()) {
                Log.e(LOG_TAG, "onResume:- with no valid activity.");
                return;
            }
            final int currentTab = mTabHost != null ? mTabHost.getCurrentTab() : 0;
            updatePhone(currentTab);
            setScreenState();
            preferredNetworkMode = getPreferredNetworkModeForPhoneId();

            // Set UI state in onResume because a user could go home, launch some
            // app to change this setting's backend, and re-launch this settings app
            // and the UI state would be inconsistent with actual state
            mButtonDataRoam.setChecked(mPhone.getDataRoamingEnabled());
            updateButtonEnable4g();

            if (getPreferenceScreen().findPreference(BUTTON_PREFERED_NETWORK_MODE) != null
                    || getPreferenceScreen().
                    findPreference(BUTTON_ENABLED_NETWORKS_KEY) != null)  {
                updatePreferredNetworkUIFromDb();
            }

            registerPhoneStateListeners();

            // NOTE: Buttons will be enabled/disabled in mPhoneStateListener.
            // The check for allowing Enhanced 4G toggle is added since the
            // setting is per-slot.
            mButton4glte.setChecked(isEnhanced4gLteEnabled());

            mSubscriptionManager.addOnSubscriptionsChangedListener(mOnSubscriptionsChangeListener);

            Log.i(LOG_TAG, "onResume:-");

        }

        private boolean isEnhanced4gLteEnabled() {
            return mImsMgr.isEnhanced4gLteModeSettingEnabledByUserForSlot()
                    && mImsMgr.isNonTtyOrTtyOnVolteEnabledForSlot();
        }

        private boolean isSubsidyUnlocked() {
            boolean subsidyLocked = Settings.Secure.getInt(
                    getContext().getContentResolver(),
                    SUBSIDY_STATUS, -1) == SUBSIDYLOCK_UNLOCKED;
            return subsidyLocked;
        }

        private boolean isPermanentlyUnlocked() {
            boolean unlocked = Settings.Secure.getInt(
                    getContext().getContentResolver(),
                    SUBSIDY_STATUS, -1) == PERMANENTLY_UNLOCKED;
            return unlocked;
        }

        private boolean hasActiveSubscriptions() {
            boolean isActive = false;
            int subId = mPhone.getSubId();
            for(SubscriptionInfo si : mActiveSubInfos) {
                if (si.getSubscriptionId() == subId) {
                    isActive = true;
                }
            }
            return isActive;
        }

        private void updateBodyBasicFields(Activity activity, PreferenceScreen prefSet,
                int phoneSubId, boolean hasActiveSubscriptions) {
            Context context = activity.getApplicationContext();

            ActionBar actionBar = activity.getActionBar();
            if (actionBar != null) {
                // android.R.id.home will be triggered in onOptionsItemSelected()
                actionBar.setDisplayHomeAsUpEnabled(true);
            }

            prefSet.addPreference(mMobileDataPref);
            prefSet.addPreference(mButtonDataRoam);
            prefSet.addPreference(mDataUsagePref);

            // Customized preferences needs to be initialized with subId.
            mMobileDataPref.initialize(phoneSubId);
            mDataUsagePref.initialize(phoneSubId);

            mMobileDataPref.setEnabled(hasActiveSubscriptions);
            mButtonDataRoam.setEnabled(hasActiveSubscriptions);
            mDataUsagePref.setEnabled(hasActiveSubscriptions);

            // Initialize states of mButtonDataRoam.
            mButtonDataRoam.setChecked(mPhone.getDataRoamingEnabled());
            mButtonDataRoam.setDisabledByAdmin(false);
            if (mButtonDataRoam.isEnabled()) {
                if (RestrictedLockUtils.hasBaseUserRestriction(context,
                        UserManager.DISALLOW_DATA_ROAMING, UserHandle.myUserId())) {
                    mButtonDataRoam.setEnabled(false);
                } else {
                    mButtonDataRoam.checkRestrictionAndSetDisabled(
                            UserManager.DISALLOW_DATA_ROAMING);
                }
            }
        }

        private void updateBody() {
            final Activity activity = getActivity();
            final PreferenceScreen prefSet = getPreferenceScreen();
            final int phoneSubId = mPhone.getSubId();
            final boolean hasActiveSubscriptions = hasActiveSubscriptions();

            if (activity == null || activity.isDestroyed()) {
                Log.e(LOG_TAG, "updateBody with no valid activity.");
                return;
            }

            if (prefSet == null) {
                Log.e(LOG_TAG, "updateBody with no null prefSet.");
                return;
            }

            prefSet.removeAll();

            updateBodyBasicFields(activity, prefSet, phoneSubId, hasActiveSubscriptions);

            if (mExpandAdvancedFields) {
                updateBodyAdvancedFields(activity, prefSet, phoneSubId, hasActiveSubscriptions);
            } else {
                prefSet.addPreference(mAdvancedOptions);
                mAdvancedOptions.setEnabled(hasActiveSubscriptions);
            }
            setScreenState();
        }

        /**
         * Whether to show the Enhanced 4G LTE settings.
         *
         * <p>We show this settings if the VoLTE can be enabled by this device and the carrier app
         * doesn't set {@link CarrierConfigManager.KEY_HIDE_ENHANCED_4G_LTE_BOOL} to false.
         */
        public static boolean hideEnhanced4gLteSettings(Context context,
                    PersistableBundle carrierConfig) {
            boolean hideSettings = false;
            try {
                if ((mImsMgr.getImsServiceStatus() != ImsFeature.STATE_READY)
                        || !(mImsMgr.isVolteEnabledByPlatformForSlot()
                        && mImsMgr.isVolteProvisionedOnDeviceForSlot())
                        || carrierConfig.getBoolean(
                            CarrierConfigManager.KEY_HIDE_ENHANCED_4G_LTE_BOOL)) {
                     hideSettings = true;
                }
            } catch (ImsException ex) {
                log("Exception when trying to get ImsServiceStatus: " + ex);
                hideSettings = true;
            }
            return hideSettings;
        }

        private void updateBodyAdvancedFields(Activity activity, PreferenceScreen prefSet,
                int phoneSubId, boolean hasActiveSubscriptions) {

            Context context = activity.getApplicationContext();
            boolean isLteOnCdma = mPhone.getLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE;
            final int phoneId = mPhone.getPhoneId();
            final int currentPrimarySlot = Settings.Global.getInt(context.getContentResolver(),
                    CONFIG_CURRENT_PRIMARY_SUB,
                    SubscriptionManager.INVALID_SIM_SLOT_INDEX);
            final int dataPhoneId = mSubscriptionManager.getDefaultDataPhoneId();
            int settingsNetworkMode = getPreferredNetworkModeForSubId();
            PersistableBundle carrierConfig =
                    mConfigManager.getConfigForSubId(phoneSubId);

            if (DBG) {
                log("updateBody: isLteOnCdma = " + isLteOnCdma + " phoneSubId = " + phoneSubId +
                        " phoneId = " + phoneId + " currentPrimarySlot= " + currentPrimarySlot +
                        " dataPhoneId = " + dataPhoneId +
                        " NW mode is: " + settingsNetworkMode + " isCmccCard: "
                        + isCmccCardBySubId(phoneSubId));
            }

            if (getResources().getBoolean(R.bool.config_uplmn_for_usim) ||
                SystemProperties.getBoolean("persist.vendor.uplmn.enabled", false)) {
                mButtonUplmnPref.getIntent().putExtra(PhoneConstants.PHONE_KEY,
                            mPhone.getPhoneId());
                prefSet.addPreference(mButtonUplmnPref);
            }
            prefSet.addPreference(mButtonPreferredNetworkMode);
            prefSet.addPreference(mButtonEnabledNetworks);
            prefSet.addPreference(mButton4glte);

            if (showEuiccSettings(getActivity())) {
                prefSet.addPreference(mEuiccSettingsPref);
                String spn = mTelephonyManager.getSimOperatorName();
                if (TextUtils.isEmpty(spn)) {
                    mEuiccSettingsPref.setSummary(null);
                } else {
                    mEuiccSettingsPref.setSummary(spn);
                }
            }

            // NOTE: Buttons will be enabled/disabled in mPhoneStateListener
            // The check for allowing Enhanced 4G toggle is added since the
            // setting is per-slot.$
            mButton4glte.setChecked(isEnhanced4gLteEnabled());

            /** Some carriers required that network mode UI need to be hidden in below conditions:
                    * 1. The absence of sim cards.
                    * 2. Non-USIM card is inserted.
                    * 3. Network type is GSM only or Global
                    */
            if (mIsCTClassC) {
                prefSet.addPreference(mButtonEnable4g);
                updateButtonEnable4g();
            } else if (mIsCMCC && !SystemProperties.getBoolean
                    (PRIMARY_4G_CARD_PROPERTY_NAME, false)) {
                UiccCard uiccCard = UiccController.getInstance().getUiccCard(phoneId);
                if (uiccCard!= null && dataPhoneId != SubscriptionManager.INVALID_PHONE_INDEX
                        && dataPhoneId == phoneId && isCmccCardBySubId(phoneSubId) &&
                        uiccCard.isApplicationOnIcc(AppType.APPTYPE_USIM)) {
                    prefSet.addPreference(mButtonPreferredCMCCNetworkMode);
                }
                prefSet.removePreference(mButtonPreferredNetworkMode);
                prefSet.removePreference(mButtonEnabledNetworks);
            }

            mIsGlobalCdma = isLteOnCdma
                    && carrierConfig.getBoolean(CarrierConfigManager.KEY_SHOW_CDMA_CHOICES_BOOL);
            if (carrierConfig.getBoolean(
                    CarrierConfigManager.KEY_HIDE_CARRIER_NETWORK_SETTINGS_BOOL)) {
                prefSet.removePreference(mButtonPreferredNetworkMode);
                prefSet.removePreference(mButtonEnabledNetworks);
                prefSet.removePreference(mLteDataServicePref);
            } else if (carrierConfig.getBoolean(CarrierConfigManager
                    .KEY_HIDE_PREFERRED_NETWORK_TYPE_BOOL)
                    && !mPhone.getServiceState().getRoaming()
                    && mPhone.getServiceState().getDataRegState()
                    == ServiceState.STATE_IN_SERVICE) {
                prefSet.removePreference(mButtonPreferredNetworkMode);
                prefSet.removePreference(mButtonEnabledNetworks);

                final int phoneType = mPhone.getPhoneType();
                if (phoneType == PhoneConstants.PHONE_TYPE_CDMA) {
                    updateCdmaOptions(this, prefSet, mPhone);
                } else if (phoneType == PhoneConstants.PHONE_TYPE_GSM) {
                    updateGsmUmtsOptions(this, prefSet, phoneSubId, mNetworkQueryService);
                } else {
                    throw new IllegalStateException("Unexpected phone type: " + phoneType);
                }
                // Since pref is being hidden from user, set network mode to default
                // in case it is currently something else. That is possible if user
                // changed the setting while roaming and is now back to home network.
                settingsNetworkMode = preferredNetworkMode;
            } else if (carrierConfig.getBoolean(
                    CarrierConfigManager.KEY_WORLD_PHONE_BOOL) == true) {
                prefSet.removePreference(mButtonEnabledNetworks);
                // set the listener for the mButtonPreferredNetworkMode list preference so we can
                // issue change Preferred Network Mode.
                if (PRIMCARYCARD_L_W_ENABLED) {
                    mButtonPreferredNetworkMode.setEntries(
                            R.array.preferred_network_mode_choices);
                    mButtonPreferredNetworkMode.setEntryValues(
                            R.array.preferred_network_mode_values);
                }
                mButtonPreferredNetworkMode.setOnPreferenceChangeListener(this);

                updateCdmaOptions(this, prefSet, mPhone);
                updateGsmUmtsOptions(this, prefSet, phoneSubId, mNetworkQueryService);
            } else {
                prefSet.removePreference(mButtonPreferredNetworkMode);
                final int phoneType = mPhone.getPhoneType();
                if (phoneType == PhoneConstants.PHONE_TYPE_CDMA) {
                    int lteForced = android.provider.Settings.Global.getInt(
                            mPhone.getContext().getContentResolver(),
                            android.provider.Settings.Global.LTE_SERVICE_FORCED +
                            mPhone.getSubId(),
                            0);

                    if (isLteOnCdma) {
                        if (lteForced == 0) {
                            mButtonEnabledNetworks.setEntries(
                                    R.array.enabled_networks_cdma_choices);
                            mButtonEnabledNetworks.setEntryValues(
                                    R.array.enabled_networks_cdma_values);
                        } else {
                            switch (settingsNetworkMode) {
                                case Phone.NT_MODE_CDMA:
                                case Phone.NT_MODE_CDMA_NO_EVDO:
                                case Phone.NT_MODE_EVDO_NO_CDMA:
                                    mButtonEnabledNetworks.setEntries(
                                            R.array.enabled_networks_cdma_no_lte_choices);
                                    mButtonEnabledNetworks.setEntryValues(
                                            R.array.enabled_networks_cdma_no_lte_values);
                                    break;
                                case Phone.NT_MODE_GLOBAL:
                                case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                                case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                                case Phone.NT_MODE_LTE_ONLY:
                                    mButtonEnabledNetworks.setEntries(
                                            R.array.enabled_networks_cdma_only_lte_choices);
                                    mButtonEnabledNetworks.setEntryValues(
                                            R.array.enabled_networks_cdma_only_lte_values);
                                    break;
                                default:
                                    mButtonEnabledNetworks.setEntries(
                                            R.array.enabled_networks_cdma_choices);
                                    mButtonEnabledNetworks.setEntryValues(
                                            R.array.enabled_networks_cdma_values);
                                    break;
                            }
                        }
                    }
                    updateCdmaOptions(this, prefSet, mPhone);

                } else if (phoneType == PhoneConstants.PHONE_TYPE_GSM) {
                    if (isSupportTdscdma()) {
                        mButtonEnabledNetworks.setEntries(
                                R.array.enabled_networks_tdscdma_choices);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_tdscdma_values);
                    } else if (!carrierConfig.getBoolean(CarrierConfigManager.KEY_PREFER_2G_BOOL)
                            && !getResources().getBoolean(R.bool.config_enabled_lte)) {
                        mButtonEnabledNetworks.setEntries(
                                R.array.enabled_networks_except_gsm_lte_choices);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_except_gsm_lte_values);
                    } else if (!carrierConfig.getBoolean(CarrierConfigManager.KEY_PREFER_2G_BOOL)) {
                        int select = (mShow4GForLTE == true) ?
                                R.array.enabled_networks_except_gsm_4g_choices
                                : R.array.enabled_networks_except_gsm_choices;
                        mButtonEnabledNetworks.setEntries(select);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_except_gsm_values);
                    } else if (!getResources().getBoolean(R.bool.config_enabled_lte)) {
                        mButtonEnabledNetworks.setEntries(
                                R.array.enabled_networks_except_lte_choices);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_except_lte_values);
                    } else if (mIsGlobalCdma) {
                        mButtonEnabledNetworks.setEntries(
                                R.array.enabled_networks_cdma_choices);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_cdma_values);
                    } else {
                        int select = (mShow4GForLTE == true) ? R.array.enabled_networks_4g_choices
                                : R.array.enabled_networks_choices;
                        mButtonEnabledNetworks.setEntries(select);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_values);
                    }
                    updateGsmUmtsOptions(this, prefSet, phoneSubId, mNetworkQueryService);
                } else {
                    throw new IllegalStateException("Unexpected phone type: " + phoneType);
                }
                if (isWorldMode()) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.preferred_network_mode_choices_world_mode);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.preferred_network_mode_values_world_mode);
                }
                mButtonEnabledNetworks.setOnPreferenceChangeListener(this);
                if (DBG) log("settingsNetworkMode: " + settingsNetworkMode);
            }

            final boolean missingDataServiceUrl = TextUtils.isEmpty(
                    android.provider.Settings.Global.getString(activity.getContentResolver(),
                            android.provider.Settings.Global.SETUP_PREPAID_DATA_SERVICE_URL));
            if (!isLteOnCdma || missingDataServiceUrl) {
                prefSet.removePreference(mLteDataServicePref);
            } else {
                android.util.Log.d(LOG_TAG, "keep ltePref");
            }
            if (hideEnhanced4gLteSettings(getActivity(), carrierConfig)) {
                Preference pref = prefSet.findPreference(BUTTON_4G_LTE_KEY);
                if (pref != null) {
                    prefSet.removePreference(pref);
                }
            }


            // Enable link to CMAS app settings depending on the value in config.xml.
            final boolean isCellBroadcastAppLinkEnabled = activity.getResources().getBoolean(
                    com.android.internal.R.bool.config_cellBroadcastAppLinks);
            if (!mUm.isAdminUser() || !isCellBroadcastAppLinkEnabled
                    || mUm.hasUserRestriction(UserManager.DISALLOW_CONFIG_CELL_BROADCASTS)) {
                PreferenceScreen root = getPreferenceScreen();
                Preference ps = findPreference(BUTTON_CELL_BROADCAST_SETTINGS);
                if (ps != null) {
                    root.removePreference(ps);
                }
            }

            // Get the networkMode from Settings.System and displays it
            mButtonEnabledNetworks.setValue(Integer.toString(settingsNetworkMode));
            mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
            mButtonPreferredCMCCNetworkMode.setValue(Integer.toString(settingsNetworkMode));
            UpdatePreferredNetworkModeSummary(settingsNetworkMode);
            UpdateEnabledNetworksValueAndSummary(settingsNetworkMode);
            UpdatePreferredCMCCNetworkModeSummary(settingsNetworkMode);

            /**
             * Enable/disable depending upon if there are any active subscriptions.
             *
             * I've decided to put this enable/disable code at the bottom as the
             * code above works even when there are no active subscriptions, thus
             * putting it afterwards is a smaller change. This can be refined later,
             * but you do need to remember that this all needs to work when subscriptions
             * change dynamically such as when hot swapping sims.
             */
            boolean canChange4glte = is4gLtePrefEnabled(activity.getApplicationContext(),
                    carrierConfig);
            boolean useVariant4glteTitle = carrierConfig.getBoolean(
                    CarrierConfigManager.KEY_ENHANCED_4G_LTE_TITLE_VARIANT_BOOL);
            int enhanced4glteModeTitleId = useVariant4glteTitle ?
                    R.string.enhanced_4g_lte_mode_title_variant :
                    R.string.enhanced_4g_lte_mode_title;

            mButtonPreferredNetworkMode.setEnabled(hasActiveSubscriptions);
            mButtonEnabledNetworks.setEnabled(hasActiveSubscriptions);
            mButton4glte.setTitle(enhanced4glteModeTitleId);
            mButton4glte.setEnabled(hasActiveSubscriptions && canChange4glte);
            mLteDataServicePref.setEnabled(hasActiveSubscriptions);
            Preference ps;
            PreferenceScreen root = getPreferenceScreen();
            ps = findPreference(BUTTON_CELL_BROADCAST_SETTINGS);
            if (ps != null) {
                ps.setEnabled(hasActiveSubscriptions);
            }
            ps = findPreference(CATEGORY_GSM_APN_EXPAND_KEY);
            if (ps != null) {
                ps.setEnabled(hasActiveSubscriptions);
            }
            ps = findPreference(CATEGORY_CDMA_APN_EXPAND_KEY);
            if (ps != null) {
                ps.setEnabled(hasActiveSubscriptions);
            }

            ps = findPreference(NetworkOperators.CATEGORY_NETWORK_OPERATORS_KEY);
            if (ps != null) {
                ps.setEnabled(hasActiveSubscriptions && mPhone.getPhoneType()
                    == PhoneConstants.PHONE_TYPE_GSM);
            }
            ps = findPreference(BUTTON_CARRIER_SETTINGS_KEY);
            if (ps != null) {
                ps.setEnabled(hasActiveSubscriptions);
            }
            ps = findPreference(BUTTON_CDMA_SYSTEM_SELECT_KEY);
            if (ps != null) {
                ps.setEnabled(hasActiveSubscriptions);
            }

            // Disable nwMode option in UI if current SIM is non-primary card with GSM only.
            if (SystemProperties.getBoolean(PRIMARY_4G_CARD_PROPERTY_NAME, false)) {
                if (SubscriptionManager.isValidSlotIndex(currentPrimarySlot)
                        && (phoneId != currentPrimarySlot)) {
                    if (PRIMCARYCARD_L_W_ENABLED) {
                        mButtonPreferredNetworkMode.setEntries(
                                R.array.preferred_network_mode_gsm_wcdma_choices);
                        mButtonPreferredNetworkMode.setEntryValues(
                                R.array.preferred_network_mode_gsm_wcdma_values);
                        mButtonEnabledNetworks.setEntries(
                                R.array.preferred_network_mode_gsm_wcdma_choices);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.preferred_network_mode_gsm_wcdma_values);
                    } else if (getPreferredNetworkModeForPhoneId() == Phone.NT_MODE_GSM_ONLY) {
                        mButtonPreferredNetworkMode.setEnabled(false);
                        mButtonEnabledNetworks.setEnabled(false);
                    }
                }
            }

            if (isSubsidyLockFeatureEnabled()) {
                if (isSubsidyUnlocked()) {
                    if (SubscriptionManager.isValidSlotIndex(currentPrimarySlot)
                                && (phoneId == currentPrimarySlot)) {
                        mButtonPreferredNetworkMode.setEnabled(hasActiveSubscriptions);
                        mButtonEnabledNetworks.setEnabled(hasActiveSubscriptions);
                        mButtonPreferredNetworkMode.setEntries(
                                R.array.enabled_networks_choices_subsidy_locked);
                        mButtonPreferredNetworkMode.setEntryValues(
                                R.array.enabled_networks_values_subsidy_locked);
                        mButtonEnabledNetworks.setEntries(
                                R.array.enabled_networks_choices_subsidy_locked);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_values_subsidy_locked);
                    } else {
                        mButtonPreferredNetworkMode.setEnabled(false);
                        mButtonEnabledNetworks.setEnabled(false);
                    }
                }
            }
        }

        @Override
        public void onPause() {
            super.onPause();
            if (DBG) log("onPause:+");

            unRegisterPhoneStateListeners();

            mSubscriptionManager
                .removeOnSubscriptionsChangedListener(mOnSubscriptionsChangeListener);
            if (DBG) log("onPause:-");
        }

        private void registerPhoneStateListeners() {
            final Activity activity = getActivity();
            if (activity == null || activity.isDestroyed()) {
                    // Process preferences in activity only if its not destroyed
                return;
            }
            TelephonyManager tm = (TelephonyManager)
                    activity.getSystemService(Context.TELEPHONY_SERVICE);
            SubscriptionManager subMgr = SubscriptionManager.from(mPhone.getContext());
            if (tm == null || subMgr == null) {
                    Log.e(LOG_TAG, "TelephonyManager or SubscriptionManager is null");
                return;
            }

            for (int i = 0; i < mPhoneStateListener.length; i++) {
                final SubscriptionInfo subInfo =
                        subMgr.getActiveSubscriptionInfoForSimSlotIndex(i);
                if (subInfo == null) {
                    loge("registerPhoneStateListener subInfo : " + subInfo +
                            " for phone Id: " + i);
                    continue;
                }

                final int phoneId = i;
                mPhoneStateListener[i]  = new PhoneStateListener(subInfo.getSubscriptionId()) {
                    /*
                    * Enable/disable the 'Enhanced 4G LTE Mode' when in/out of a call
                    * and depending on TTY mode and TTY support over VoLTE.
                    * @see android.telephony.PhoneStateListener#onCallStateChanged(int,
                    * java.lang.String)
                    */
                    @Override
                    public void onCallStateChanged(int state, String incomingNumber) {
                        if (DBG) {
                            log("PhoneStateListener onCallStateChanged: state=" + state +
                                    " SubId: " + mSubId);
                        }
                        Activity activity = getActivity();
                        if (activity == null) {
                            return;
                        }
                        mCallState[phoneId] = state;
                        PersistableBundle carrierConfig =
                                mConfigManager.getConfigForSubId(mSubId);
                        boolean enabled = is4gLtePrefEnabled(activity.getApplicationContext(),
                                carrierConfig);
                        Preference pref = getPreferenceScreen().findPreference(BUTTON_4G_LTE_KEY);
                        if (pref != null) pref.setEnabled(enabled && hasActiveSubscriptions());
                    }
                };
                log("Register for call state change for phone Id: " + i);
                tm.listen(mPhoneStateListener[i], PhoneStateListener.LISTEN_CALL_STATE);
            }
        }

        private void unRegisterPhoneStateListeners() {
            final Activity activity = getActivity();
            if (activity == null || activity.isDestroyed()) {
                    // Process preferences in activity only if its not destroyed
                return;
            }
            TelephonyManager tm = (TelephonyManager)
                    activity.getSystemService(Context.TELEPHONY_SERVICE);
            for (int i = 0; i < mPhoneStateListener.length; i++) {
                if (mPhoneStateListener[i] != null) {
                    log("unRegister for call state change for phone Id: " + i);
                    tm.listen(mPhoneStateListener[i], PhoneStateListener.LISTEN_NONE);
                    mPhoneStateListener[i] = null;
                }
            }
        }

        /**
         * Implemented to support onPreferenceChangeListener to look for preference
         * changes specifically on CLIR.
         *
         * @param preference is the preference to be changed, should be mButtonCLIR.
         * @param objValue should be the value of the selection, NOT its localized
         * display value.
         */
        public boolean onPreferenceChange(Preference preference, Object objValue) {
            if (DBG) log("onPreferenceChange");
            Context context = getActivity().getApplicationContext();
            final int phoneSubId = mPhone.getSubId();
            if (preference == mButtonPreferredNetworkMode) {
                //NOTE onPreferenceChange seems to be called even if there is no change
                //Check if the button value is changed from the System.Setting
                mButtonPreferredNetworkMode.setValue((String) objValue);
                int buttonNetworkMode;
                buttonNetworkMode = Integer.parseInt((String) objValue);
                int settingsNetworkMode = getPreferredNetworkModeForSubId();
                 if (DBG) log("buttonNetworkMode: " + buttonNetworkMode +
                        " settingsNetworkMode: " + settingsNetworkMode);
                if (buttonNetworkMode != settingsNetworkMode) {
                    int modemNetworkMode;
                    // if new mode is invalid ignore it
                    switch (buttonNetworkMode) {
                        case Phone.NT_MODE_WCDMA_PREF:
                        case Phone.NT_MODE_GSM_ONLY:
                        case Phone.NT_MODE_WCDMA_ONLY:
                        case Phone.NT_MODE_GSM_UMTS:
                        case Phone.NT_MODE_CDMA:
                        case Phone.NT_MODE_CDMA_NO_EVDO:
                        case Phone.NT_MODE_EVDO_NO_CDMA:
                        case Phone.NT_MODE_GLOBAL:
                        case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                        case Phone.NT_MODE_LTE_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_ONLY:
                        case Phone.NT_MODE_LTE_WCDMA:
                        case Phone.NT_MODE_TDSCDMA_ONLY:
                        case Phone.NT_MODE_TDSCDMA_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA:
                        case Phone.NT_MODE_TDSCDMA_GSM:
                        case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                        case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                        case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                            // This is one of the modes we recognize
                            modemNetworkMode = buttonNetworkMode;
                            break;
                        default:
                            loge("Invalid Network Mode (" + buttonNetworkMode +
                                    ") chosen. Ignore.");
                            return true;
                    }
                    UpdatePreferredNetworkModeSummary(buttonNetworkMode);
                    //Set the modem network mode
                    mPhone.setPreferredNetworkType(modemNetworkMode, mHandler
                            .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
                }
            } else if (preference == mButtonEnabledNetworks) {
                mButtonEnabledNetworks.setValue((String) objValue);
                int buttonNetworkMode;
                buttonNetworkMode = Integer.parseInt((String) objValue);
                if (DBG) log("buttonNetworkMode: " + buttonNetworkMode);
                int settingsNetworkMode = getPreferredNetworkModeForSubId();
                if (buttonNetworkMode != settingsNetworkMode) {
                    int modemNetworkMode;
                    // if new mode is invalid ignore it
                    switch (buttonNetworkMode) {
                        case Phone.NT_MODE_WCDMA_PREF:
                        case Phone.NT_MODE_GSM_ONLY:
                        case Phone.NT_MODE_LTE_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                        case Phone.NT_MODE_CDMA:
                        case Phone.NT_MODE_CDMA_NO_EVDO:
                        case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                        case Phone.NT_MODE_TDSCDMA_ONLY:
                        case Phone.NT_MODE_TDSCDMA_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA:
                        case Phone.NT_MODE_TDSCDMA_GSM:
                        case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                        case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                        case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                        case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                            // This is one of the modes we recognize
                            modemNetworkMode = buttonNetworkMode;
                            break;
                        default:
                            loge("Invalid Network Mode (" + buttonNetworkMode +
                                    ") chosen. Ignore.");
                            return true;
                    }

                    UpdateEnabledNetworksValueAndSummary(buttonNetworkMode);

                    //Set the modem network mode
                    mPhone.setPreferredNetworkType(modemNetworkMode, mHandler
                            .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
                }
            } else if (preference == mButton4glte) {
                if (DBG) log("onPreferenceTreeClick: preference == mButton4glte.");
                SwitchPreference enhanced4gModePref = (SwitchPreference) preference;
                boolean enhanced4gMode = !enhanced4gModePref.isChecked();
                enhanced4gModePref.setChecked(enhanced4gMode);
                mImsMgr.setEnhanced4gLteModeSettingForSlot(enhanced4gModePref.isChecked());
                Intent intent = new Intent("org.codeaurora.intent.action.ACTION_ENHANCE_4G_SWITCH");
                intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
                context.sendBroadcast(intent);
            } else if (mButtonEnable4g != null && preference == mButtonEnable4g) {
                SwitchPreference enable4g = (SwitchPreference)preference;
                enable4g.setChecked(!enable4g.isChecked());
                handleEnable4gChange();
            } else if (preference == mButtonDataRoam) {
                if (DBG) log("onPreferenceTreeClick: preference == mButtonDataRoam.");

                //normally called on the toggle click
                if (!mButtonDataRoam.isChecked()) {
                    // First confirm with a warning dialog about charges
                    mOkClicked = false;
                    RoamingDialogFragment fragment = new RoamingDialogFragment();
                    fragment.show(getFragmentManager(), ROAMING_TAG);
                    // Don't update the toggle unless the confirm button is actually pressed.
                    return false;
                } else {
                    mPhone.setDataRoamingEnabled(false);
                }
                return true;
            }

            // always let the preference setting proceed.
            return true;
        }

        // Update network mode in DB for both SubId  and phoneId
        private void setPreferredNetworkMode(int nwMode) {
            final int phoneSubId = mPhone.getSubId();
            final int phoneId = mPhone.getPhoneId();
            if (DBG) log("setPreferredNetworkMode: nwMode = " + nwMode +
                    " phoneSubId = " + phoneSubId + " phoneId = " + phoneId);
            preferredNetworkMode = nwMode;
            android.provider.Settings.Global.putInt(mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE + phoneSubId,
                    nwMode );
            TelephonyManager.putIntAtIndex(mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE, phoneId,
                    nwMode );
        }

        //Get preferred network mode based on phoneId
        private int getPreferredNetworkModeForPhoneId() {
            final int phoneId = mPhone.getPhoneId();
            int phoneNwMode;

            try {
                phoneNwMode = android.telephony.TelephonyManager.getIntAtIndex(
                        mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE, phoneId);
            } catch (SettingNotFoundException snfe) {
                log("getPreferredNetworkModeForPhoneId: Could not find PREFERRED_NETWORK_MODE");
                phoneNwMode = Phone.PREFERRED_NT_MODE;
            }
            if (DBG) log("getPreferredNetworkModeForPhoneId: phoneNwMode = " + phoneNwMode +
                    " phoneId = " + phoneId);
            return phoneNwMode;
        }

        //Get preferred network mode based on subId
        private int getPreferredNetworkModeForSubId() {
            final int subId = mPhone.getSubId();
            int phoneNwMode;
            int nwMode;

            nwMode = android.provider.Settings.Global.getInt(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId,
                    preferredNetworkMode);
            if (DBG) log("getPreferredNetworkModeForSubId: phoneNwMode = " + nwMode +
                    " subId = "+ subId);
            return nwMode;
        }

        private boolean is4gLtePrefEnabled(Context context, PersistableBundle carrierConfig) {

            boolean enabled = true;
            for (int i = 0; i < mCallState.length; i++) {
                if (DBG) log("callstate: " + mCallState[i]);
                enabled = (mCallState[i] == TelephonyManager.CALL_STATE_IDLE);
                if (!enabled) break;
            }
            return enabled && mImsMgr.isNonTtyOrTtyOnVolteEnabledForSlot()
                    && carrierConfig.getBoolean(
                            CarrierConfigManager.KEY_EDITABLE_ENHANCED_4G_LTE_BOOL);
        }
        private class MyHandler extends Handler {

            static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE = 0;

            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
                        handleSetPreferredNetworkTypeResponse(msg);
                        break;
                }
            }

            private void handleSetPreferredNetworkTypeResponse(Message msg) {
                final Activity activity = getActivity();
                if (activity == null || activity.isDestroyed()) {
                    // Access preferences of activity only if it is not destroyed
                    // or if fragment is not attached to an activity.
                    return;
                }

                AsyncResult ar = (AsyncResult) msg.obj;

                if (ar.exception == null) {
                log("handleSetPreferredNetworkTypeResponse: Sucess");
                    int networkMode;
                    if (getPreferenceScreen().findPreference(
                            BUTTON_PREFERED_NETWORK_MODE) != null)  {
                        networkMode =  Integer.parseInt(mButtonPreferredNetworkMode.getValue());
                        setPreferredNetworkMode(networkMode);
                    }
                    if (getPreferenceScreen().findPreference(
                            BUTTON_ENABLED_NETWORKS_KEY) != null)  {
                        networkMode = Integer.parseInt(mButtonEnabledNetworks.getValue());
                        setPreferredNetworkMode(networkMode);
                    }
                    updateButtonEnable4g();
                } else {
                    if (DBG) {
                        log("handleSetPreferredNetworkTypeResponse:" +
                                " exception in setting network mode.");
                    }
                    updatePreferredNetworkUIFromDb();
                }
            }
        }

        private void updatePreferredNetworkUIFromDb() {
            final int phoneSubId = mPhone.getSubId();

            int settingsNetworkMode = getPreferredNetworkModeForSubId();

            if (DBG) {
                log("updatePreferredNetworkUIFromDb: settingsNetworkMode = " +
                        settingsNetworkMode);
            }

            UpdatePreferredNetworkModeSummary(settingsNetworkMode);
            UpdateEnabledNetworksValueAndSummary(settingsNetworkMode);
            // changes the mButtonPreferredNetworkMode accordingly to settingsNetworkMode
            mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
        }

        private void UpdatePreferredCMCCNetworkModeSummary(int NetworkMode) {
            if (Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA == NetworkMode)
                mButtonPreferredCMCCNetworkMode.setSummary
                    (R.string.preferred_network_mode_cmcc_summary);
        }

        private void UpdatePreferredNetworkModeSummary(int NetworkMode) {
            switch(NetworkMode) {
                case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_tdscdma_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_TDSCDMA_GSM:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_tdscdma_gsm_summary);
                    break;
                case Phone.NT_MODE_WCDMA_PREF:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_wcdma_perf_summary);
                    break;
                case Phone.NT_MODE_GSM_ONLY:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_gsm_only_summary);
                    break;
                case Phone.NT_MODE_TDSCDMA_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_tdscdma_wcdma_summary);
                    break;
                case Phone.NT_MODE_WCDMA_ONLY:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_wcdma_only_summary);
                    break;
                case Phone.NT_MODE_GSM_UMTS:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_CDMA:
                    switch (mPhone.getLteOnCdmaMode()) {
                        case PhoneConstants.LTE_ON_CDMA_TRUE:
                            mButtonPreferredNetworkMode.setSummary(
                                R.string.preferred_network_mode_cdma_summary);
                        break;
                        case PhoneConstants.LTE_ON_CDMA_FALSE:
                        default:
                            mButtonPreferredNetworkMode.setSummary(
                                R.string.preferred_network_mode_cdma_evdo_summary);
                            break;
                    }
                    break;
                case Phone.NT_MODE_CDMA_NO_EVDO:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_cdma_only_summary);
                    break;
                case Phone.NT_MODE_EVDO_NO_CDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_evdo_only_summary);
                    break;
                case Phone.NT_MODE_LTE_TDSCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_tdscdma_summary);
                    break;
                case Phone.NT_MODE_LTE_ONLY:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_summary);
                    break;
                case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_tdscdma_gsm_summary);
                    break;
                case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_tdscdma_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_LTE_GSM_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_cdma_evdo_summary);
                    break;
                case Phone.NT_MODE_TDSCDMA_ONLY:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_tdscdma_summary);
                    break;
                case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.
                            preferred_network_mode_lte_tdscdma_cdma_evdo_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                    if (mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ||
                            mIsGlobalCdma ||
                            isWorldMode()) {
                        mButtonPreferredNetworkMode.setSummary(
                                R.string.preferred_network_mode_global_summary);
                    } else {
                        mButtonPreferredNetworkMode.setSummary(
                                R.string.preferred_network_mode_lte_summary);
                    }
                    break;
                case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_tdscdma_cdma_evdo_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_GLOBAL:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_cdma_evdo_gsm_wcdma_summary);
                    break;
                case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_tdscdma_wcdma_summary);
                    break;
                case Phone.NT_MODE_LTE_WCDMA:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_wcdma_summary);
                    break;
                default:
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_global_summary);
            }
        }

        private void UpdateEnabledNetworksValueAndSummary(int NetworkMode) {
            switch (NetworkMode) {
                case Phone.NT_MODE_TDSCDMA_WCDMA:
                case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                case Phone.NT_MODE_TDSCDMA_GSM:
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_TDSCDMA_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_3G);
                    break;
                case Phone.NT_MODE_WCDMA_ONLY:
                case Phone.NT_MODE_GSM_UMTS:
                case Phone.NT_MODE_WCDMA_PREF:
                    if (!mIsGlobalCdma) {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_WCDMA_PREF));
                        mButtonEnabledNetworks.setSummary(R.string.network_3G);
                    } else {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                        mButtonEnabledNetworks.setSummary(R.string.network_global);
                    }
                    break;
                case Phone.NT_MODE_GSM_ONLY:
                    if (!mIsGlobalCdma) {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_GSM_ONLY));
                        mButtonEnabledNetworks.setSummary(R.string.network_2G);
                    } else {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                        mButtonEnabledNetworks.setSummary(R.string.network_global);
                    }
                    break;
                case Phone.NT_MODE_LTE_GSM_WCDMA:
                    if (isWorldMode()) {
                        mButtonEnabledNetworks.setSummary(
                                R.string.preferred_network_mode_lte_gsm_umts_summary);
                        controlCdmaOptions(false);
                        controlGsmOptions(true);
                        break;
                    }
                case Phone.NT_MODE_LTE_ONLY:
                case Phone.NT_MODE_LTE_WCDMA:
                    if (!mIsGlobalCdma) {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_GSM_WCDMA));
                        mButtonEnabledNetworks.setSummary((mShow4GForLTE == true)
                                ? R.string.network_4G : R.string.network_lte);
                    } else {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                        mButtonEnabledNetworks.setSummary(R.string.network_global);
                    }
                    break;
                case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                    if (isWorldMode()) {
                        mButtonEnabledNetworks.setSummary(
                                R.string.preferred_network_mode_lte_cdma_summary);
                        controlCdmaOptions(true);
                        controlGsmOptions(false);
                    } else {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_CDMA_AND_EVDO));
                        mButtonEnabledNetworks.setSummary(R.string.network_lte);
                    }
                    break;
                case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_3G);
                    break;
                case Phone.NT_MODE_CDMA:
                case Phone.NT_MODE_EVDO_NO_CDMA:
                case Phone.NT_MODE_GLOBAL:
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_CDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_3G);
                    break;
                case Phone.NT_MODE_CDMA_NO_EVDO:
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_CDMA_NO_EVDO));
                    mButtonEnabledNetworks.setSummary(R.string.network_1x);
                    break;
                case Phone.NT_MODE_TDSCDMA_ONLY:
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_TDSCDMA_ONLY));
                    mButtonEnabledNetworks.setSummary(R.string.network_3G);
                    break;
                case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                case Phone.NT_MODE_LTE_TDSCDMA:
                case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                    if (isSupportTdscdma()) {
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA));
                        mButtonEnabledNetworks.setSummary(R.string.network_lte);
                    } else {
                        if (isWorldMode()) {
                            controlCdmaOptions(true);
                            controlGsmOptions(false);
                        }
                        mButtonEnabledNetworks.setValue(
                                Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                        if (mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ||
                                mIsGlobalCdma ||
                                isWorldMode()) {
                            mButtonEnabledNetworks.setSummary(R.string.network_global);
                        } else {
                            mButtonEnabledNetworks.setSummary((mShow4GForLTE == true)
                                    ? R.string.network_4G : R.string.network_lte);
                        }
                    }
                    break;
                default:
                    String errMsg = "Invalid Network Mode (" + NetworkMode + "). Ignore.";
                    loge(errMsg);
                    mButtonEnabledNetworks.setSummary(errMsg);
            }
        }

        @Override
        public void onActivityResult(int requestCode, int resultCode, Intent data) {
            switch(requestCode) {
            case REQUEST_CODE_EXIT_ECM:
                Boolean isChoiceYes =
                    data.getBooleanExtra(EXTRA_EXIT_ECM_RESULT, false);
                if (isChoiceYes) {
                    // If the phone exits from ECM mode, show the CDMA Options
                    mCdmaOptions.showDialog(mClickedPreference);
                } else {
                    // do nothing
                }
                break;

            default:
                break;
            }
        }

        private static void log(String msg) {
            Log.d(LOG_TAG, msg);
        }

        private static void loge(String msg) {
            Log.e(LOG_TAG, msg);
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
            final int itemId = item.getItemId();
            if (itemId == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
                // Commenting out "logical up" capability. This is a workaround for issue 5278083.
                //
                // Settings app may not launch this activity via UP_ACTIVITY_CLASS but the other
                // Activity that looks exactly same as UP_ACTIVITY_CLASS ("SubSettings" Activity).
                // At that moment, this Activity launches UP_ACTIVITY_CLASS on top of the Activity.
                // which confuses users.
                // TODO: introduce better mechanism for "up" capability here.
                /*Intent intent = new Intent(Intent.ACTION_MAIN);
                intent.setClassName(UP_ACTIVITY_PACKAGE, UP_ACTIVITY_CLASS);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);*/
                getActivity().finish();
                return true;
            }
            return super.onOptionsItemSelected(item);
        }

        private boolean isWorldMode() {
            boolean worldModeOn = false;
            final String configString = getResources().getString(R.string.config_world_mode);

            if (!TextUtils.isEmpty(configString)) {
                String[] configArray = configString.split(";");
                // Check if we have World mode configuration set to True only or config is
                // set to True and SIM GID value is also set and matches to the current SIM GID.
                if (configArray != null &&
                        ((configArray.length == 1 && configArray[0].equalsIgnoreCase("true"))
                                || (configArray.length == 2 && !TextUtils.isEmpty(configArray[1])
                                && mTelephonyManager != null
                                && configArray[1].equalsIgnoreCase(
                                        mTelephonyManager.getGroupIdLevel1())))) {
                    worldModeOn = true;
                }
            }

                Log.d(LOG_TAG, "isWorldMode=" + worldModeOn);

            return worldModeOn;
        }

        private void controlGsmOptions(boolean enable) {
            PreferenceScreen prefSet = getPreferenceScreen();
            if (prefSet == null) {
                return;
            }

            updateGsmUmtsOptions(this, prefSet, mPhone.getSubId(), mNetworkQueryService);

            PreferenceCategory apnExpand =
                    (PreferenceCategory) prefSet.findPreference(CATEGORY_GSM_APN_EXPAND_KEY);
            PreferenceCategory networkOperatorCategory =
                    (PreferenceCategory) prefSet.findPreference(
                            NetworkOperators.CATEGORY_NETWORK_OPERATORS_KEY);
            PreferenceScreen carrierSettings =
                    (PreferenceScreen) prefSet.findPreference(BUTTON_CARRIER_SETTINGS_KEY);
            if (apnExpand != null) {
                apnExpand.setEnabled(isWorldMode() || enable);
            }
            if (networkOperatorCategory != null) {
                if (enable) {
                    networkOperatorCategory.setEnabled(true);
                } else {
                    prefSet.removePreference(networkOperatorCategory);
                }
            }
            if (carrierSettings != null) {
                prefSet.removePreference(carrierSettings);
            }
        }

        private void controlCdmaOptions(boolean enable) {
            PreferenceScreen prefSet = getPreferenceScreen();
            if (prefSet == null) {
                return;
            }
            updateCdmaOptions(this, prefSet, mPhone);
            CdmaSystemSelectListPreference systemSelect =
                    (CdmaSystemSelectListPreference)prefSet.findPreference
                            (BUTTON_CDMA_SYSTEM_SELECT_KEY);
            if (systemSelect != null) {
                systemSelect.setEnabled(enable);
            }
        }

        private boolean isSupportTdscdma() {
            final int subId = mPhone.getSubId();
            if (SubscriptionManager.getResourcesForSubId(getActivity(), subId)
                    .getBoolean(R.bool.config_support_tdscdma)) {
                log("Supports Tdscdma, subId " + subId);
                return true;
            }

            String operatorNumeric = mPhone.getServiceState().getOperatorNumeric();
            String[] numericArray =
                    SubscriptionManager.getResourcesForSubId(getActivity(), subId).getStringArray(
                    R.array.config_support_tdscdma_roaming_on_networks);
            log("isSupportTdscdma, operatorNumeric " + operatorNumeric + " subId " + subId);
            if (numericArray.length == 0 || operatorNumeric == null) {
                return false;
            }
            for (String numeric : numericArray) {
                if (operatorNumeric.equals(numeric)) {
                    return true;
                }
            }
            return false;
        }

        private void sendMetricsEventPreferenceClicked(
                PreferenceScreen preferenceScreen, Preference preference) {
            if (preference == mMobileDataPref) {
                MetricsLogger.action(getContext(),
                        MetricsEvent.ACTION_MOBILE_NETWORK_MOBILE_DATA_TOGGLE,
                        ((MobileDataPreference) preference).mChecked);
            } else if (preference == mDataUsagePref) {
                MetricsLogger.action(getContext(),
                        MetricsEvent.ACTION_MOBILE_NETWORK_DATA_USAGE);
            }
            // TODO: add Metrics constants for other preferences and send events here accordingly.
        }

        private void updateGsmUmtsOptions(PreferenceFragment prefFragment,
                PreferenceScreen prefScreen, final int subId, INetworkQueryService queryService) {
            // We don't want to re-create GsmUmtsOptions if already exists. Otherwise, the
            // preferences inside it will also be re-created which causes unexpected behavior.
            // For example, the open dialog gets dismissed or detached after pause / resume.
            if (mGsmUmtsOptions == null) {
                mGsmUmtsOptions = new GsmUmtsOptions(
                        prefFragment, prefScreen, subId, queryService);
            } else {
                mGsmUmtsOptions.update(subId, queryService);
            }
        }

        private void updateCdmaOptions(PreferenceFragment prefFragment, PreferenceScreen prefScreen,
                Phone phone) {
            // We don't want to re-create CdmaOptions if already exists. Otherwise, the preferences
            // inside it will also be re-created which causes unexpected behavior. For example,
            // the open dialog gets dismissed or detached after pause / resume.
            if (mCdmaOptions == null) {
                mCdmaOptions = new CdmaOptions(prefFragment, prefScreen, phone);
            } else {
                mCdmaOptions.update(phone);
            }
        }
    }
}

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
 * Copyright (C) 2010 The Android Open Source Project
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

package com.qualcomm.qti.setupwizard.wifi;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AppGlobals;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.Fragment;
import android.app.admin.DevicePolicyManager;
import android.content.ContentResolver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageManager;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.os.HandlerThread;
import android.os.Process;
import android.os.RemoteException;
import android.os.UserHandle;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;
import android.provider.Settings;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.logging.MetricsLogger;
import com.android.internal.logging.MetricsProto;
import com.android.settingslib.wifi.AccessPoint;
import com.android.settingslib.wifi.AccessPoint.AccessPointListener;
import com.android.settingslib.wifi.WifiTracker;
import com.android.settingslib.RestrictedLockUtils;
import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;
import com.qualcomm.qti.setupwizard.ui.DialogCreatable;
import com.qualcomm.qti.setupwizard.wifi.AccessPointPreference.UserBadgeCache;

import java.util.Collection;

public class WifiSettingFragment extends PreferenceFragment
        implements WifiDialog.WifiDialogListener, WifiTracker.WifiListener,
        AccessPointListener, DialogCreatable {
    private static final String TAG = "WifiSettingFragment";
    private static final String WIFI_SETTING = "wifi_setting";
    private static final int MENU_ID_CONNECT = Menu.FIRST + 6;
    private static final int MENU_ID_FORGET = Menu.FIRST + 7;
    private static final int MENU_ID_MODIFY = Menu.FIRST + 8;
    private static final int MENU_ID_WRITE_NFC = Menu.FIRST + 9;
    private static final int MENU_ID_DISCONNECT = Menu.FIRST + 10;

    public static final int WIFI_DIALOG_ID = 1;
    private static final int WRITE_NFC_DIALOG_ID = 6;

    // Instance state keys
    private static final String SAVE_DIALOG_MODE = "dialog_mode";
    private static final String SAVE_DIALOG_ACCESS_POINT_STATE = "wifi_ap_state";
    private static final String SAVED_WIFI_NFC_DIALOG_STATE = "wifi_nfc_dlg_state";

    private static boolean savedNetworksExist;

    protected WifiManager mWifiManager;
    private WifiManager.ActionListener mConnectListener;
    private WifiManager.ActionListener mSaveListener;
    private WifiManager.ActionListener mForgetListener;

    // An access point being editted is stored here.
    private AccessPoint mSelectedAccessPoint;
    private WifiDialog mDialog;
    private WriteWifiConfigToNfcDialog mWifiToNfcDialog;

    private TextView mEmptyView;

    // this boolean extra specifies whether to disable the Next button when not connected. Used by
    // account creation outside of setup wizard.
    //private static final String EXTRA_ENABLE_NEXT_ON_CONNECT = "wifi_enable_next_on_connect";
    // This string extra specifies a network to open the connect dialog on, so the user can enter
    // network credentials.  This is used by quick settings for secured networks.
    private static final String EXTRA_START_CONNECT_SSID = "wifi_start_connect_ssid";

    // Save the dialog details
    private int mDialogMode;
    private AccessPoint mDlgAccessPoint;
    private Bundle mAccessPointSavedState;
    private Bundle mWifiNfcDialogSavedState;

    private WifiTracker mWifiTracker;
    private String mOpenSsid;

    private HandlerThread mBgThread;

    private UserBadgeCache mUserBadgeCache;

    private View view;

    private WifiDialogFragment mDialogFragment;
    private FragmentCallbacks mCallbacks;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.wifi_access_point);
        onBackLabel(WIFI_SETTING);
        mUserBadgeCache = new UserBadgeCache(getActivity().getPackageManager());

        mBgThread = new HandlerThread(TAG, Process.THREAD_PRIORITY_BACKGROUND);
        mBgThread.start();
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(String currentPage) {
        if (mCallbacks != null) {
            mCallbacks.onFragmentInteraction(currentPage);
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof FragmentCallbacks) {
            mCallbacks = (FragmentCallbacks) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDestroy() {
        mBgThread.quit();
        super.onDestroy();
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        mWifiTracker =
                new WifiTracker(getActivity(), this, mBgThread.getLooper(), true, true, false);
        mWifiManager = mWifiTracker.getManager();
        mWifiManager.setWifiEnabled(true);

        mConnectListener = new WifiManager.ActionListener() {
            @Override
            public void onSuccess() {
            }
            @Override
            public void onFailure(int reason) {
                Activity activity = getActivity();
                if (activity != null) {
                    Toast.makeText(activity,
                            R.string.wifi_failed_connect_message,
                            Toast.LENGTH_SHORT).show();
                }
            }
        };

        mSaveListener = new WifiManager.ActionListener() {
            @Override
            public void onSuccess() {
            }
            @Override
            public void onFailure(int reason) {
                Activity activity = getActivity();
                if (activity != null) {
                    Toast.makeText(activity,
                            R.string.wifi_failed_save_message,
                            Toast.LENGTH_SHORT).show();
                }
            }
        };

        mForgetListener = new WifiManager.ActionListener() {
            @Override
            public void onSuccess() {
            }
            @Override
            public void onFailure(int reason) {
                Activity activity = getActivity();
                if (activity != null) {
                    Toast.makeText(activity,
                            R.string.wifi_failed_forget_message,
                            Toast.LENGTH_SHORT).show();
                }
            }
        };

        if (savedInstanceState != null) {
            mDialogMode = savedInstanceState.getInt(SAVE_DIALOG_MODE);
            if (savedInstanceState.containsKey(SAVE_DIALOG_ACCESS_POINT_STATE)) {
                mAccessPointSavedState =
                        savedInstanceState.getBundle(SAVE_DIALOG_ACCESS_POINT_STATE);
            }

            if (savedInstanceState.containsKey(SAVED_WIFI_NFC_DIALOG_STATE)) {
                mWifiNfcDialogSavedState =
                        savedInstanceState.getBundle(SAVED_WIFI_NFC_DIALOG_STATE);
            }
        }

        // if we're supposed to enable/disable the Next button based on our current connection
        // state, start it off in the right state
        Intent intent = getActivity().getIntent();

        final ConnectivityManager connectivity = (ConnectivityManager)
                getActivity().getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity != null) {
            NetworkInfo info = connectivity.getNetworkInfo(
                    ConnectivityManager.TYPE_WIFI);
            mCallbacks.isWifiConnected(info.isConnected());
        }

        mEmptyView = initEmptyView();
        registerForContextMenu(getListView());
        setHasOptionsMenu(true);

        if (intent.hasExtra(EXTRA_START_CONNECT_SSID)) {
            mOpenSsid = intent.getStringExtra(EXTRA_START_CONNECT_SSID);
            onAccessPointsChanged();
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        // If the dialog is showing, save its state.
        if (mDialog != null && mDialog.isShowing()) {
            outState.putInt(SAVE_DIALOG_MODE, mDialogMode);
            if (mDlgAccessPoint != null) {
                mAccessPointSavedState = new Bundle();
                mDlgAccessPoint.saveWifiState(mAccessPointSavedState);
                outState.putBundle(SAVE_DIALOG_ACCESS_POINT_STATE, mAccessPointSavedState);
            }
        }

        if (mWifiToNfcDialog != null && mWifiToNfcDialog.isShowing()) {
            Bundle savedState = new Bundle();
            mWifiToNfcDialog.saveState(savedState);
            outState.putBundle(SAVED_WIFI_NFC_DIALOG_STATE, savedState);
        }
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View view, ContextMenuInfo info) {
        if (info instanceof AdapterContextMenuInfo) {
            Preference preference = (Preference) getListView().getItemAtPosition(
                    ((AdapterContextMenuInfo) info).position);

            if (preference instanceof AccessPointPreference) {
                mSelectedAccessPoint = ((AccessPointPreference) preference).getAccessPoint();
                menu.setHeaderTitle(mSelectedAccessPoint.getSsid());
                if (mSelectedAccessPoint.isConnectable()) {
                    menu.add(Menu.NONE, MENU_ID_CONNECT, 0, R.string.wifi_menu_connect);
                }

                WifiConfiguration config = mSelectedAccessPoint.getConfig();
                // Some configs are ineditable
                if (isEditabilityLockedDown(getActivity(), config)) {
                    return;
                }

                if (mSelectedAccessPoint.isSaved() || mSelectedAccessPoint.isEphemeral()) {
                    // Allow forgetting a network if either the network is saved or ephemerally
                    // connected. (In the latter case, "forget" blacklists the network so it won't
                    // be used again, ephemerally).
                    menu.add(Menu.NONE, MENU_ID_FORGET, 0, R.string.wifi_menu_forget);
                }
                // current connected AP, add a disconnect option to it
                if (getResources().getBoolean(R.bool.config_auto_connect_wifi_enabled)) {
                    if (mSelectedAccessPoint.isActive()) {
                        menu.add(Menu.NONE, MENU_ID_DISCONNECT, 0, R.string.wifi_menu_disconnect);
                    }
                }
                if (mSelectedAccessPoint.isSaved()) {
                    menu.add(Menu.NONE, MENU_ID_MODIFY, 0, R.string.wifi_menu_modify);
                    NfcAdapter nfcAdapter = NfcAdapter.getDefaultAdapter(getActivity());
                    if (nfcAdapter != null && nfcAdapter.isEnabled() &&
                            mSelectedAccessPoint.getSecurity() != AccessPoint.SECURITY_NONE) {
                        // Only allow writing of NFC tags for password-protected networks.
                        menu.add(Menu.NONE, MENU_ID_WRITE_NFC, 0, R.string.wifi_menu_write_to_nfc);
                    }
                }
            }
        }
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        if (mSelectedAccessPoint == null) {
            return super.onContextItemSelected(item);
        }
        switch (item.getItemId()) {
            case MENU_ID_CONNECT: {
                if (mSelectedAccessPoint.isSaved()) {
                    connect(mSelectedAccessPoint.getConfig());
                } else if (mSelectedAccessPoint.getSecurity() == AccessPoint.SECURITY_NONE) {
                    /** Bypass dialog for unsecured networks */
                    mSelectedAccessPoint.generateOpenNetworkConfig();
                    connect(mSelectedAccessPoint.getConfig());
                } else {
                    showDialog(mSelectedAccessPoint, WifiConfigUiBase.MODE_CONNECT);
                }
                return true;
            }
            case MENU_ID_FORGET: {
                forget();
                return true;
            }
            case MENU_ID_MODIFY: {
                showDialog(mSelectedAccessPoint, WifiConfigUiBase.MODE_MODIFY);
                return true;
            }
            case MENU_ID_DISCONNECT: {
                mWifiManager.disconnect();
                return true;
            }
            case MENU_ID_WRITE_NFC:
                showDialog(WRITE_NFC_DIALOG_ID);
                return true;

        }
        return super.onContextItemSelected(item);
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        if (preference instanceof AccessPointPreference) {
            mSelectedAccessPoint = ((AccessPointPreference) preference).getAccessPoint();
            if (mSelectedAccessPoint == null) {
                return false;
            }

            /** Bypass dialog for unsecured, unsaved, and inactive networks */
            if (mSelectedAccessPoint.getSecurity() == AccessPoint.SECURITY_NONE &&
                    !mSelectedAccessPoint.isSaved() && !mSelectedAccessPoint.isActive()) {
                mSelectedAccessPoint.generateOpenNetworkConfig();
                if (!savedNetworksExist) {
                    savedNetworksExist = true;
                    getActivity().invalidateOptionsMenu();
                }
                connect(mSelectedAccessPoint.getConfig());
            } else if (mSelectedAccessPoint.isSaved()) {
                showDialog(mSelectedAccessPoint, WifiConfigUiBase.MODE_VIEW);
            } else {
                showDialog(mSelectedAccessPoint, WifiConfigUiBase.MODE_CONNECT);
            }
        } else {
            return super.onPreferenceTreeClick(screen, preference);
        }
        return true;
    }

    private void showDialog(AccessPoint accessPoint, int dialogMode) {
        if (accessPoint != null) {
            WifiConfiguration config = accessPoint.getConfig();
            if (isEditabilityLockedDown(getActivity(), config) && accessPoint.isActive()) {
                RestrictedLockUtils.sendShowAdminSupportDetailsIntent(getActivity(),
                        RestrictedLockUtils.getDeviceOwner(getActivity()));
                return;
            }
        }

        if (mDialog != null) {
            removeDialog(WIFI_DIALOG_ID);
            mDialog = null;
        }

        // Save the access point and edit mode
        mDlgAccessPoint = accessPoint;
        mDialogMode = dialogMode;

        showDialog(WIFI_DIALOG_ID);
    }

    /**
     * Shows the latest access points available with supplemental information like
     * the strength of network and the security for it.
     */
    @Override
    public void onAccessPointsChanged() {
        // Safeguard from some delayed event handling
        if (getActivity() == null) return;

        final int wifiState = mWifiManager.getWifiState();

        switch (wifiState) {
            case WifiManager.WIFI_STATE_ENABLED:
                accessPointsSorted();
                break;

            case WifiManager.WIFI_STATE_ENABLING:
                getPreferenceScreen().removeAll();
                break;

            case WifiManager.WIFI_STATE_DISABLING:
                break;

            case WifiManager.WIFI_STATE_DISABLED:
                break;

            case WifiManager.WIFI_STATE_UNKNOWN:
                break;
        }

        // Update "Saved Networks" menu option.
        if (savedNetworksExist != mWifiTracker.doSavedNetworksExist()) {
            savedNetworksExist = !savedNetworksExist;
            getActivity().invalidateOptionsMenu();
        }
    }

    /**
     * AccessPoints are automatically sorted with TreeSet.
     */
    private void accessPointsSorted() {
        final Collection<AccessPoint> accessPoints =
                mWifiTracker.getAccessPoints();
        getPreferenceScreen().removeAll();

        boolean hasAvailableAccessPoints = false;
        int index = 0;
        for (AccessPoint accessPoint : accessPoints) {
            // Ignore access points that are out of range.
            if (accessPoint.getLevel() != -1) {
                hasAvailableAccessPoints = true;
                if (accessPoint.getTag() != null) {
                    final Preference pref = (Preference) accessPoint.getTag();
                    pref.setOrder(index++);
                    getPreferenceScreen().addPreference(pref);
                    continue;
                }
                AccessPointPreference preference = new AccessPointPreference(accessPoint,
                        getActivity(), mUserBadgeCache, false);
                preference.setOrder(index++);

                if (mOpenSsid != null && mOpenSsid.equals(accessPoint.getSsidStr())
                        && !accessPoint.isSaved()
                        && accessPoint.getSecurity() != AccessPoint.SECURITY_NONE) {
                    onPreferenceTreeClick(getPreferenceScreen(), preference);
                    mOpenSsid = null;
                }
                getPreferenceScreen().addPreference(preference);
                accessPoint.setListener(this);
            }
        }

        mCallbacks.updateSetupRes(hasAvailableAccessPoints);
        if (!hasAvailableAccessPoints) {
            mEmptyView.setVisibility(View.INVISIBLE);
            getPreferenceScreen().removeAll();
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        mWifiTracker.startTracking();
    }

    @Override
    public void onPause() {
        super.onPause();

        mWifiTracker.stopTracking();
    }

    protected TextView initEmptyView() {
        TextView emptyView = (TextView) getActivity().findViewById(android.R.id.empty);
        getListView().setEmptyView(emptyView);
        return emptyView;
    }

    @Override
    public void onWifiStateChanged(int state) {
        Activity activity = getActivity();
        if (activity != null) {
            activity.invalidateOptionsMenu();
        }
    }

    @Override
    public void onConnectedChanged() {
        mCallbacks.isWifiConnected(mWifiTracker.isConnected());
    }

    @Override
    public void onForget(WifiDialog dialog) {
        forget();
    }

    @Override
    public void onSubmit(WifiDialog dialog) {
        if (mDialog != null) {
            submit(mDialog.getController());
        }
    }

    /* package */ void submit(WifiConfigController configController) {

        final WifiConfiguration config = configController.getConfig();

        if (config == null) {
            if (mSelectedAccessPoint != null
                    && mSelectedAccessPoint.isSaved()) {
                connect(mSelectedAccessPoint.getConfig());
            }
        } else if (configController.getMode() == WifiConfigUiBase.MODE_MODIFY) {
            mWifiManager.save(config, mSaveListener);
        } else {
            mWifiManager.save(config, mSaveListener);
            if (mSelectedAccessPoint != null) { // Not an "Add network"
                connect(config);
            }
        }

        mWifiTracker.resumeScanning();
    }

    /* package */ void forget() {
        MetricsLogger.action(getActivity(), MetricsProto.MetricsEvent.ACTION_WIFI_FORGET);
        if (!mSelectedAccessPoint.isSaved()) {
            if (mSelectedAccessPoint.getNetworkInfo() != null &&
                    mSelectedAccessPoint.getNetworkInfo().getState() != State.DISCONNECTED) {
                // Network is active but has no network ID - must be ephemeral.
                mWifiManager.disableEphemeralNetwork(
                        AccessPoint.convertToQuotedString(mSelectedAccessPoint.getSsidStr()));
            } else {
                // Should not happen, but a monkey seems to trigger it
                Log.e(TAG, "Failed to forget invalid network " + mSelectedAccessPoint.getConfig());
                return;
            }
        } else {
            mWifiManager.forget(mSelectedAccessPoint.getConfig().networkId, mForgetListener);
        }

        mWifiTracker.resumeScanning();
        mCallbacks.isWifiConnected(false);
    }

    protected void connect(final WifiConfiguration config) {
        MetricsLogger.action(getActivity(), MetricsProto.MetricsEvent.ACTION_WIFI_CONNECT);
        mWifiManager.connect(config, mConnectListener);
    }

    protected void connect(final int networkId) {
        MetricsLogger.action(getActivity(), MetricsProto.MetricsEvent.ACTION_WIFI_CONNECT);
        mWifiManager.connect(networkId, mConnectListener);
    }

    /* package */ int getAccessPointsCount() {
        final boolean wifiIsEnabled = mWifiTracker.isWifiEnabled();
        if (wifiIsEnabled) {
            return getPreferenceScreen().getPreferenceCount();
        } else {
            return 0;
        }
    }

    @Override
    public void onAccessPointChanged(AccessPoint accessPoint) {
        ((AccessPointPreference) accessPoint.getTag()).refresh();
    }

    @Override
    public void onLevelChanged(AccessPoint accessPoint) {
        ((AccessPointPreference) accessPoint.getTag()).onLevelChanged();
    }

    /**
     * Returns true if the config is not editable through Settings.
     * @param context Context of caller
     * @param config The WiFi config.
     * @return true if the config is not editable through Settings.
     */
    static boolean isEditabilityLockedDown(Context context, WifiConfiguration config) {
        return !canModifyNetwork(context, config);
    }

    /**
     * This method is a stripped version of WifiConfigStore.canModifyNetwork.
     * TODO: refactor to have only one method.
     * @param context Context of caller
     * @param config The WiFi config.
     * @return true if Settings can modify the config.
     */
    static boolean canModifyNetwork(Context context, WifiConfiguration config) {
        if (config == null) {
            return true;
        }

        final DevicePolicyManager dpm = (DevicePolicyManager) context.getSystemService(
                Context.DEVICE_POLICY_SERVICE);

        // Check if device has DPM capability. If it has and dpm is still null, then we
        // treat this case with suspicion and bail out.
        final PackageManager pm = context.getPackageManager();
        if (pm.hasSystemFeature(PackageManager.FEATURE_DEVICE_ADMIN) && dpm == null) {
            return false;
        }

        boolean isConfigEligibleForLockdown = false;
        if (dpm != null) {
            final String deviceOwnerPackageName = dpm.getDeviceOwner();
            if (deviceOwnerPackageName != null) {
                try {
                    final int deviceOwnerUid = pm.getPackageUid(deviceOwnerPackageName,
                            UserHandle.USER_OWNER);
                    isConfigEligibleForLockdown = deviceOwnerUid == config.creatorUid;
                } catch (NameNotFoundException e) {
                    // don't care
                }
            }
        }
        if (!isConfigEligibleForLockdown) {
            return true;
        }

        final ContentResolver resolver = context.getContentResolver();
        final boolean isLockdownFeatureEnabled = Settings.Global.getInt(resolver,
                Settings.Global.WIFI_DEVICE_OWNER_CONFIGS_LOCKDOWN, 0) != 0;
        return !isLockdownFeatureEnabled;
    }

    @Override
    public void onDetach() {
        if (isRemoving()) {
            if (mDialogFragment != null) {
                mDialogFragment.dismiss();
                mDialogFragment = null;
            }
        }
        super.onDetach();
    }

    protected void showDialog(int dialogId) {
        if (mDialogFragment != null) {
            Log.e(TAG, "Old dialog fragment not null!");
        }
        mDialogFragment = new WifiDialogFragment(this, dialogId);
        mDialogFragment.show(getChildFragmentManager(), Integer.toString(dialogId));
    }

    public Dialog onCreateDialog(int dialogId) {
        switch (dialogId) {
            case WIFI_DIALOG_ID:
                AccessPoint ap = mDlgAccessPoint; // For manual launch
                if (ap == null) { // For re-launch from saved state
                    if (mAccessPointSavedState != null) {
                        ap = new AccessPoint(getActivity(), mAccessPointSavedState);
                        // For repeated orientation changes
                        mDlgAccessPoint = ap;
                        // Reset the saved access point data
                        mAccessPointSavedState = null;
                    }
                }
                // If it's null, fine, it's for Add Network
                mSelectedAccessPoint = ap;
                mDialog = new WifiDialog(getActivity(), this, ap, mDialogMode,
                        /* no hide submit/connect */ false);
                return mDialog;
            case WRITE_NFC_DIALOG_ID:
                if (mSelectedAccessPoint != null) {
                    mWifiToNfcDialog = new WriteWifiConfigToNfcDialog(
                            getActivity(), mSelectedAccessPoint.getConfig().networkId,
                            mSelectedAccessPoint.getSecurity(),
                            mWifiManager);
                } else if (mWifiNfcDialogSavedState != null) {
                    mWifiToNfcDialog = new WriteWifiConfigToNfcDialog(
                            getActivity(), mWifiNfcDialogSavedState, mWifiManager);
                }

                return mWifiToNfcDialog;
        }
        return null;
    }

    protected void removeDialog(int dialogId) {
        // mDialogFragment may not be visible yet in parent fragment's onResume().
        // To be able to dismiss dialog at that time, don't check
        // mDialogFragment.isVisible().
        if (mDialogFragment != null && mDialogFragment.getDialogId() == dialogId) {
            mDialogFragment.dismissAllowingStateLoss();
        }
        mDialogFragment = null;
    }

    /**
     * Sets the OnCancelListener of the dialog shown. This method can only be
     * called after showDialog(int) and before removeDialog(int). The method
     * does nothing otherwise.
     */
    protected void setOnCancelListener(DialogInterface.OnCancelListener listener) {
        if (mDialogFragment != null) {
            mDialogFragment.mOnCancelListener = listener;
        }
    }

    /**
     * Sets the OnDismissListener of the dialog shown. This method can only be
     * called after showDialog(int) and before removeDialog(int). The method
     * does nothing otherwise.
     */
    protected void setOnDismissListener(DialogInterface.OnDismissListener listener) {
        if (mDialogFragment != null) {
            mDialogFragment.mOnDismissListener = listener;
        }
    }

    public void onDialogShowing() {
        // override in subclass to attach a dismiss listener, for instance
    }

    public static class WifiDialogFragment extends DialogFragment {
        private static final String KEY_DIALOG_ID = "key_dialog_id";
        private static final String KEY_PARENT_FRAGMENT_ID = "key_parent_fragment_id";

        private int mDialogId;

        private Fragment mParentFragment;

        private DialogInterface.OnCancelListener mOnCancelListener;
        private DialogInterface.OnDismissListener mOnDismissListener;

        public WifiDialogFragment() {
            /* do nothing */
        }

        public WifiDialogFragment(DialogCreatable fragment, int dialogId) {
            mDialogId = dialogId;
            if (!(fragment instanceof Fragment)) {
                throw new IllegalArgumentException("fragment argument must be an instance of "
                        + Fragment.class.getName());
            }
            mParentFragment = (Fragment) fragment;
        }

        @Override
        public void onSaveInstanceState(Bundle outState) {
            super.onSaveInstanceState(outState);
            if (mParentFragment != null) {
                outState.putInt(KEY_DIALOG_ID, mDialogId);
                outState.putInt(KEY_PARENT_FRAGMENT_ID, mParentFragment.getId());
            }
        }

        @Override
        public void onStart() {
            super.onStart();

            if (mParentFragment != null && mParentFragment instanceof WifiSettingFragment) {
                ((WifiSettingFragment) mParentFragment).onDialogShowing();
            }
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            if (savedInstanceState != null) {
                mDialogId = savedInstanceState.getInt(KEY_DIALOG_ID, 0);
                mParentFragment = getParentFragment();
                int mParentFragmentId = savedInstanceState.getInt(KEY_PARENT_FRAGMENT_ID, -1);
                if (mParentFragment == null) {
                    mParentFragment = getFragmentManager().findFragmentById(mParentFragmentId);
                }
                if (!(mParentFragment instanceof DialogCreatable)) {
                    throw new IllegalArgumentException(
                            (mParentFragment != null
                                    ? mParentFragment.getClass().getName()
                                    : mParentFragmentId)
                                    + " must implement "
                                    + DialogCreatable.class.getName());
                }
                // This dialog fragment could be created from non-SettingsPreferenceFragment
                if (mParentFragment instanceof WifiSettingFragment) {
                    // restore mDialogFragment in mParentFragment
                    ((WifiSettingFragment) mParentFragment).mDialogFragment = this;
                }
            }
            return ((DialogCreatable) mParentFragment).onCreateDialog(mDialogId);
        }

        @Override
        public void onCancel(DialogInterface dialog) {
            super.onCancel(dialog);
            if (mOnCancelListener != null) {
                mOnCancelListener.onCancel(dialog);
            }
        }

        @Override
        public void onDismiss(DialogInterface dialog) {
            super.onDismiss(dialog);
            if (mOnDismissListener != null) {
                mOnDismissListener.onDismiss(dialog);
            }
        }

        public int getDialogId() {
            return mDialogId;
        }

        @Override
        public void onDetach() {
            super.onDetach();

            // This dialog fragment could be created from non-SettingsPreferenceFragment
            if (mParentFragment instanceof WifiSettingFragment) {
                // in case the dialog is not explicitly removed by removeDialog()
                if (((WifiSettingFragment) mParentFragment).mDialogFragment == this) {
                    ((WifiSettingFragment) mParentFragment).mDialogFragment = null;
                }
            }
        }
    }
}

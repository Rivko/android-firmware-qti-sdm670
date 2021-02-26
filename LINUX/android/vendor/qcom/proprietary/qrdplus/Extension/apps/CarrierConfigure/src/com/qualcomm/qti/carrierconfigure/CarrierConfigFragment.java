/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import java.util.ArrayList;
import java.util.Collections;
import java.lang.StringBuilder;
import java.lang.Exception;
import java.io.IOException;


import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.Loader;
import android.content.Loader.OnLoadCompleteListener;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.android.internal.os.RegionalizationEnvironment;
import com.android.internal.os.IRegionalizationService;

import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Actions.ActionCallback;
import com.qualcomm.qti.carrierconfigure.Actions.SwitchCarrierTask;
import com.qualcomm.qti.carrierconfigure.Actions.UpdateNVItemsTask;
import com.qualcomm.qti.carrierconfigure.Carrier.CarriersStorage;
import com.qualcomm.qti.carrierconfigure.Carrier.SwitchData;
import com.qualcomm.qti.carrierconfigure.Utils.CopyAndUnzipOtaFileThread;
import com.qualcomm.qti.carrierconfigure.Utils.MyAlertDialog;
import com.qualcomm.qti.carrierconfigure.Utils.WaitDialog;
import com.qualcomm.qti.loadcarrier.ILoadCarrierService;

public class CarrierConfigFragment extends RadioPreferenceFragment
        implements MyAlertDialog.OnAlertDialogButtonClick, ActionCallback {
    private static final String TAG = "CarrierConfigFragment";

    private static final int MSG_START_SWITCH = MSG_BASE + 1;
    private static final int MSG_COPY_AND_UNZIPFILE = MSG_BASE + 2;

    private static final int REQUEST_UPDATE_NV_ITEMS = 0;
    private static final int REQUEST_SWITCH_CARRIER = 1;

    private static final int MAX_CHAR_NUM_OF_ONE_LINE = 25;

    private boolean mDisplayHierarchy = false;
    private IRegionalizationService mRegionalizationService = null;

    private CarriersLoader mLoader = null;
    private Carrier mSwitchToDefaultCarrier = null;
    private CarriersStorage mStorage;
    private OnLoadCompleteListener<CarriersStorage> mLoaderListener =
            new OnLoadCompleteListener<CarriersStorage>() {
        @Override
        public void onLoadComplete(Loader<CarriersStorage> loader, CarriersStorage storage) {
            if (storage != null) {
                mStorage = storage;
                buildPreferences(mStorage);
            }
        }
    };

    private ILoadCarrierService mService = null;
    private ICarrierAccessCacheService mAccessCacheService = null;
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            if (Utils.DEBUG) Log.i(TAG, "Service Connected to LoadCarrierService" +
                    name.getShortClassName());
            if (name.getShortClassName().equals(".LoadCarrierService")) {
                mService = ILoadCarrierService.Stub.asInterface(service);
            } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = ICarrierAccessCacheService.Stub.asInterface(service);
            }
            if (mLoader != null) {
                mLoader.notifyCarrierLoadServiceChanged(mService, mAccessCacheService);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            if (name.getShortClassName().equals(".LoadCarrierService")) {
                mService = null;
            } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = null;
            }
            if (mLoader != null) {
                mLoader.notifyCarrierLoadServiceChanged(mService, mAccessCacheService);
            }
        }
    };

    public CarrierConfigFragment() {
        super();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (mServiceConnection != null) {
            if (mService == null) {
                // Bind the service to get the carriers stored in SD card.
                Intent intent = new Intent(ILoadCarrierService.class.getName());
                intent.setPackage(ILoadCarrierService.class.getPackage().getName());
                getActivity().bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
            if (mAccessCacheService == null) {
                // Bind the service to access cache dir.
                Intent intent = new Intent(ICarrierAccessCacheService.class.getName());
                intent.setPackage(ICarrierAccessCacheService.class.getPackage().getName());
                getActivity().bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
        }

        mLoader = new CarriersLoader(getActivity(), mService, mAccessCacheService);
        mLoader.registerListener(0, mLoaderListener);
        mLoader.startLoading();

        ArrayList<String> carriers = Carrier.getCurrentCarriers();
        if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
            mCurrentPreferenceKey = carriers.get(carriers.size() - 1);
        } else {
            mCurrentPreferenceKey = Carrier.getCurrentCarriersName(carriers);
        }
        mSelectedPreferenceKey = mCurrentPreferenceKey;

        // Load the preferences from an XML resource
        addPreferencesFromResource(R.xml.carrier_config_preference);
    }

    @Override
    public void onResume() {
        super.onResume();
        mLoader.startLoading();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mLoader != null) {
            mLoader.unregisterListener(mLoaderListener);
            mLoader.stopLoading();
            mLoader.reset();
        }

        if (mServiceConnection != null) {
            getActivity().unbindService(mServiceConnection);
        }
    }

    @Override
    public void onDisplayModeChanged(int newMode) {
        mDisplayHierarchy = (newMode == Utils.HIERARCHY_VIEW);
        buildPreferences(mStorage);
    }

    @Override
    public void onAlertDialogButtonClick(int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                Utils.resetSimsInfo(getActivity());
                // For user press OK, then try to switch the carrier.
                sendEmptyMessage(MSG_START_SWITCH);
                break;
            case DialogInterface.BUTTON_NEGATIVE:
                // For user press CANCEL, then reset the value and update the display.
                resetSelection();
                break;
        }
    }

    @Override
    protected void onSelectedChanged() {
        // Show the dialog to alert the user.
        MyAlertDialog dialog = MyAlertDialog.newInstance(this, R.string.alert_switch_title,
                R.string.alert_switch_text);
        dialog.show(getFragmentManager(), MyAlertDialog.TAG_LABEL);
    }

    @Override
    protected void handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_START_SWITCH:
                if (Utils.DEBUG) Log.d(TAG, "Start switch! For user press yes.");
                // Show the "Please wait ..." dialog.
                WaitDialog wait = WaitDialog.newInstance();
                wait.show(getFragmentManager(), WaitDialog.TAG_LABEL);

                onActionFinished(REQUEST_UPDATE_NV_ITEMS);
                break;
            case MSG_COPY_AND_UNZIPFILE:
                try {
                    SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                            REQUEST_SWITCH_CARRIER, this,
                            (ArrayList<SwitchData>)msg.obj, mAccessCacheService);
                    switchTask.execute();
                } catch (IllegalArgumentException ex) {
                    Log.e(TAG, "Catch the IllegalArgumentException: " + ex.getMessage());
                    onActionError();
                }
                break;
        }
    }

    @Override
    public void onActionFinished(int requestCode) {
        switch (requestCode) {
            case REQUEST_UPDATE_NV_ITEMS:
                // After the update NV items action finished, start the switch action.
                try {
                    Carrier info = null;
                    Preference preference = getPreferenceManager().findPreference(
                            mSelectedPreferenceKey);
                    if (preference instanceof RadioButtonPreference) {
                        info = (Carrier) ((RadioButtonPreference) preference).getTag();
                    } else {
                        Log.wtf(TAG, "onActionFinished()->Not a RadioButtonPreference!");
                    }

                    if (info == null) {
                        Log.wtf(TAG, "onActionFinished()->can not get Carrier!");
                        return;
                    }

                    if (info != null) {
                        //String type = info.getDeviceType();
                        TriggerRebootFragment.showCarrier = info.mName;
                        /*if (RegionalizationEnvironment.isSupported()) {
                            mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
                        }
                        if(mRegionalizationService != null){
                            try{
                                mRegionalizationService.writeFile(Utils.DEVICE_TYPE, type + "\n", false);
                                Log.d(TAG,".........write over ........");
                            }catch (Exception e) {
                                Log.e(TAG, "IOException"+ e.getMessage());
                            }
                        }
                        Log.d("WYM",".........deviceType="+type);*/
                    }
                    if (Utils.isFileBasedSwitchEnabled()
                            || Utils.isPresetMode()) {
                        ArrayList<Carrier> carrierlist = getCheckedCarrierList(info);
                        new CopyAndUnzipOtaFileThread(getActivity(), mHandler,
                                MSG_COPY_AND_UNZIPFILE, mService, mAccessCacheService,
                                carrierlist).start();
                    } else {
                        SwitchData data = info.getSwitchData(mService, mAccessCacheService);
                        SwitchCarrierTask switchTask = new SwitchCarrierTask(getActivity(),
                                REQUEST_SWITCH_CARRIER, this, mAccessCacheService);
                        if (mSwitchToDefaultCarrier == null
                                || info.getBaseCarrierName().equals(mCurrentPreferenceKey))
                            switchTask.execute(data);
                        else switchTask.execute(mSwitchToDefaultCarrier.getSwitchData(mService,
                                mAccessCacheService), data);
                    }
                } catch (IllegalArgumentException ex) {
                    Log.e(TAG, "Catch the IllegalArgumentException: " + ex.getMessage());
                    onActionError();
                }
                break;
            case REQUEST_SWITCH_CARRIER:
                getActivity().finish();
                break;
        }
    }

    @Override
    public void onActionError(int requestCode, int resultCode, Exception ex) {
        Log.e(TAG, "Get the request[ " + requestCode + "] error, ex is " + ex.getMessage());
        onActionError();
    }

    private void onActionError() {
        // There is some error when we get the switch intent, prompt one toast.
        ((WaitDialog) getFragmentManager().findFragmentByTag(WaitDialog.TAG_LABEL)).dismiss();
        Toast.makeText(getActivity(),R.string.alert_switch_error,Toast.LENGTH_LONG).show();
        // Reset the value and display.
        resetSelection();
    }

    private void buildPreferences(CarriersStorage storage) {
        if (Utils.DEBUG) Log.i(TAG, "Try to build the preferences.");

        if (storage == null || storage.isEmpty()) {
            Log.w(TAG, "Try to build the preferences, but couldn't find any carrier.");
            return;
        }

        mSwitchToDefaultCarrier = null;

        if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
            travelCarriersStorage_fileBased(storage);
            if (!mHasDefaultCarrier) {
                getPreferenceScreen().removePreference(mDefaultCategory);
            }
        } else {
            travelCarrierStorage(storage);
            if (mSwitchToDefaultCarrier == null
                    && !mCurrentPreferenceKey.equalsIgnoreCase("Default")) {
                getPreferenceScreen().removePreference(mDefaultCategory);
            }
        }

        if (!mHasChinaCarrier) {
            getPreferenceScreen().removePreference(mChinaCarrierCategory);
        }
        if (!mHasRegionalCarrier) {
            getPreferenceScreen().removePreference(mRegionalCategory);
        }

        // Set the checked value.
        PreferenceScreen preferenceScreen = getPreferenceScreen();
        int preferenceCount = preferenceScreen.getPreferenceCount();
        for (int i = 0; i < preferenceCount; i++) {
            Preference tmpPreference = preferenceScreen.getPreference(i);
            if (tmpPreference instanceof RadioGroupPreferenceCategory) {
                ((RadioGroupPreferenceCategory) tmpPreference).setCheckedPreference(
                        mSelectedPreferenceKey);
            }
        }
    }

    private void travelCarriersStorage_fileBased(CarriersStorage storage) {
        mChinaCarrierCategory.removeAll();
        mRegionalCategory.removeAll();
        Carrier currentCarrier = storage.getCarrier(mCurrentPreferenceKey);
        if (currentCarrier != null) {
            RadioButtonPreference preference = new RadioButtonPreference(getActivity());
            preference.setKey(currentCarrier.mName);
            preference.setTitle(mDisplayHierarchy ?
                    currentCarrier.mName : currentCarrier.getDisplayNameOnUI());
            preference.setSummary(mDisplayHierarchy ?
                    getCarrierDisplayName(currentCarrier, mStorage) : null);
            preference.setTag(currentCarrier);
            if (!TextUtils.isEmpty(currentCarrier.getHierarchy())) {
                mRegionalCategory.addPreference(preference);
                mHasRegionalCarrier = true;
            } else if(!currentCarrier.mName.equalsIgnoreCase("Default")){
                mChinaCarrierCategory.addPreference(preference);
                mHasChinaCarrier = true;
            }
        }
        Collections.sort(storage.mListCarrier);
        boolean setBackgroundColor = true;
        for (Carrier carrier : storage.mListCarrier) {
            if (carrier.mName.equalsIgnoreCase("Default")) {
                mDefaultRadioButton.setTag(carrier);
                mHasDefaultCarrier = true;
                continue;
            }

            if (carrier == currentCarrier) continue;

            // Create the preference of this carrier.
            RadioButtonPreference preference = new RadioButtonPreference(getActivity());
            preference.setKey(carrier.mName);
            preference.setTitle(mDisplayHierarchy ?
                    carrier.mName : carrier.getDisplayNameOnUI());
            preference.setSummary(mDisplayHierarchy ?
                    getCarrierDisplayName(carrier, mStorage) : null);
            preference.setTag(carrier);
            if (TextUtils.isEmpty(carrier.getHierarchy())) {
                // Hierarchy is empty, this is a China Carrier
                // Remove China Preset Packages
                mChinaCarrierCategory.addPreference(preference);
                mHasChinaCarrier = true;
            } else {
                if (setBackgroundColor) {
                    preference.setBackgroundColor(getActivity().getResources().getColor(
                            R.color.carrierconfig_preference_background));
                }
                mRegionalCategory.addPreference(preference);
                mHasRegionalCarrier = true;
                setBackgroundColor = !setBackgroundColor;
            }
        }
    }

    private String getCarrierDisplayName(Carrier carrier, CarriersStorage storage) {
        if ((carrier == null) || (TextUtils.isEmpty(carrier.getDependency()))) return null;

        StringBuilder display = new StringBuilder();
        String dependency = carrier.getDependency();
        int blankLength = 2;
        while (!TextUtils.isEmpty(dependency)) {
            Carrier dependentCarrier = storage.getCarrier(dependency);
            if (dependentCarrier == null) {
                break;
            }
            for (int i = 0; i < blankLength; i++) {
                display.append(" ");
            }
            display.append("- ");
            display.append(dependentCarrier.mName);
            dependency = dependentCarrier.getDependency();
            if (!TextUtils.isEmpty(dependency)) {
                display.append("\n");
            }
            blankLength += 2;
        }

        return display.toString();
    }

    private void travelCarrierStorage(CarriersStorage storage) {
        for (Carrier carrier : storage.mListCarrier) {
            Carrier currentCarrier = Carrier.getCurrentCarrierInstance();
            if (carrier.getSwitchCarrierName().equalsIgnoreCase("Default")) {
                if (carrier.getBaseCarrierName().equalsIgnoreCase(currentCarrier.mName)){
                    mSwitchToDefaultCarrier = carrier;
                    mDefaultRadioButton.setTag(carrier);
                }
                // do not show Carrier2Default
                continue;
            }
            if (mCurrentPreferenceKey.equalsIgnoreCase("Default")) {
                addCarrierPreference(carrier);
                // If current carrier is Default, show all the valid carriers
            } else {
                if (getPreferenceScreen().findPreference(currentCarrier.mName) == null) {
                    // Add current carrier's preference
                    addCarrierPreference(currentCarrier);
                }
                // If current carrier is not Default, we need current carrier's
                // Carrier2Default.ota.zip file to switch to Default
                String carrier2DefaultName = mCurrentPreferenceKey + "2Default";
                if (storage.getCarrier(carrier2DefaultName) != null
                        && !carrier.mName.equals(currentCarrier.mName)) {
                    // Carrier2Default.ota.zip file exists, show all the valid carriers
                    // Create the preference of this carrier.
                    addCarrierPreference(carrier);
                }
            }
        }
    }

    private void addCarrierPreference(Carrier carrier) {
        RadioButtonPreference preference = new RadioButtonPreference(getActivity());
        preference.setKey(carrier.mName);
        preference.setTitle(carrier.getRegionalCarrierTitle());
        preference.setTag(carrier);
        if (TextUtils.isEmpty(carrier.getHierarchy())) {
            // Hierarchy is empty, this is a China Carrier
            // Remove China OTA packages.
            //mChinaCarrierCategory.addPreference(preference);
            //mHasChinaCarrier = true;
        } else {
            mRegionalCategory.addPreference(preference);
            mHasRegionalCarrier = true;
        }
    }

    private ArrayList<Carrier> getCheckedCarrierList(Carrier carrier) {
        ArrayList<Carrier> list = new ArrayList<Carrier>();
        list.add(carrier);
        findDependencyCarriers(carrier, list);

        return list;
    }

    private void findDependencyCarriers(Carrier carrier, ArrayList<Carrier> list) {
        String carriername = carrier.getDependency();
        if (!TextUtils.isEmpty(carriername)) {
            RadioButtonPreference preference =
                    (RadioButtonPreference) mRegionalCategory.findPreference(carriername);
            if (preference != null) {
                Carrier info = (Carrier)preference.getTag();
                list.add(info);
                findDependencyCarriers(info, list);
            }
        }
    }
}

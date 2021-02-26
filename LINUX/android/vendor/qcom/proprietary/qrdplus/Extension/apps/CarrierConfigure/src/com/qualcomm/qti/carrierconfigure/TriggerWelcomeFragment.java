/**
 * Copyright (c) 2014,2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.content.ComponentName;
import android.os.Bundle;
import android.os.SystemProperties;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.text.TextUtils;
import android.provider.Settings;
import android.content.res.Resources;
import android.content.Context;
import android.widget.TextView;
import android.util.Log;

import com.android.internal.app.LocalePicker;
import com.android.internal.app.LocalePicker.LocaleInfo;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Collections;
import java.util.Locale;


public class TriggerWelcomeFragment extends Fragment implements OnClickListener {
    private static final String ARG_CARRIER_LIST = "carrier_list";
    private static final int REQUEST_START_TRIGGER = 1;

    private Spinner mSpinner;
    private int mCurrentIndex;
    private ArrayList<Carrier> mCarriers;
    private static final String[] fakeLocales = { "en-XA", "ar-XB" };
    private static final String TAG = "TriggerWelcomeFragment";
    private static final boolean DEBUG = true;

    public TriggerWelcomeFragment() {
    }

    public static TriggerWelcomeFragment newInstance(ArrayList<Carrier> carriers) {
        final TriggerWelcomeFragment fragment = new TriggerWelcomeFragment();

        final Bundle args = new Bundle(1);
        args.putParcelableArrayList(ARG_CARRIER_LIST, carriers);
        fragment.setArguments(args);

        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mCarriers = getArguments().getParcelableArrayList(ARG_CARRIER_LIST);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        View rootView = inflater.inflate(R.layout.trigger_welcome_fragment, container, false);

        // Set the click listener for these two button.
        rootView.findViewById(R.id.start_trigger).setOnClickListener(this);
        rootView.findViewById(R.id.emergency_call).setOnClickListener(this);

        mSpinner = (Spinner) rootView.findViewById(R.id.language_spinner);

        ArrayList<LocaleInfo> list = new ArrayList<LocaleInfo>();
        mCurrentIndex = getLocaleInfos(createAdapter(getActivity(), com.android.internal.R.layout.locale_picker_item,
		        com.android.internal.R.id.locale), list);
        mSpinner.setAdapter(
                new ArrayAdapter<LocaleInfo>(getActivity(), R.layout.spinner_item, list));
        mSpinner.setSelection(mCurrentIndex, true);

        return rootView;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == Activity.RESULT_OK
                && requestCode == REQUEST_START_TRIGGER) {
            getActivity().finish();
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.start_trigger:
                if (mSpinner.getSelectedItemPosition() != mCurrentIndex) {
                    new Thread(){
                       @Override
                       public void run() {
                          super.run();
                          LocaleInfo info = (LocaleInfo) mSpinner.getSelectedItem();
                          LocalePicker.updateLocale(info.getLocale());
                       }
                    }.start();
                }
                Intent intent = new Intent();
                intent.setAction(Utils.ACTION_TRIGGER_START);
                intent.setClass(getActivity(), ConfigurationActivity.class);
                intent.putParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST, mCarriers);
                startActivityForResult(intent, REQUEST_START_TRIGGER);
                break;
            case R.id.emergency_call:
                // TODO: start the emergency call.
                //com.android.dialer/.DialtactsActivity
                Intent intentemergency = new Intent();
                intentemergency.setComponent(new ComponentName("com.android.dialer",
                    "com.android.dialer.DialtactsActivity"));
                getActivity().startActivity(intentemergency);
                break;
        }
    }

    public static ArrayAdapter<LocaleInfo> createAdapter(Context context,
            int layoutId, int viewId) {

        boolean developerEnable = Settings.Global.getInt(context.getContentResolver(),
                Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, 0) != 0;
        List<LocaleInfo> localeInfos = getAllSupportedLocales(context, developerEnable);

        LayoutInflater layoutInflater =
                (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        return new ArrayAdapter<LocaleInfo>(context, layoutId, viewId, localeInfos) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                View localView;
                TextView viewText;

                if (convertView == null) {
                    localView = layoutInflater.inflate(layoutId, parent, false);
                    viewText = (TextView) localView.findViewById(viewId);
                    localView.setTag(viewText);
                } else {
                    localView = convertView;
                    viewText = (TextView) localView.getTag();
                }

                LocaleInfo localInfoItem = getItem(position);
                viewText.setText(localInfoItem.toString());
                viewText.setTextLocale(localInfoItem.getLocale());
                return localView;
            }
        };
    }

    private static String getName(
            Locale local, String[] localeCodes, String[] localeNames) {
        String code = local.toString();
        for (int j = 0; j < localeCodes.length; j++) {
            if (localeCodes[j].equals(code)) {
                return localeNames[j];
            }
        }
        return local.getDisplayName(local);
    }

    private static String getTitleCase(String string) {
        if (string.length() == 0) {
            return string;
        }
		String Str = Character.toUpperCase(string.charAt(0)) + string.substring(1);
        return Str;
    }

    public static List<LocaleInfo> getAllSupportedLocales(Context context, boolean developerEnable) {
        final Resources resources = context.getResources();

        String[] locales = null;
        ArrayList<String> localeList = new ArrayList<String>();
        //String localeCodes = resources.getString(com.android.internal.R.string.locale_codes);

        locales = LocalePicker.getSupportedLocales(context);
        localeList = new ArrayList<String>(locales.length);
        Collections.addAll(localeList, locales);
	    if (DEBUG) {
	        Log.d(TAG,"supportedLocales length : " +locales.length);
	    }

        if (!developerEnable) {
            for (String fakeLocale : fakeLocales) {
                localeList.remove(fakeLocale);
            }
        }

        Collections.sort(localeList);

        String[] specialLocaleCodes = resources.getStringArray(com.android.internal.R.array.special_locale_codes);
        String[] specialLocaleNames = resources.getStringArray(com.android.internal.R.array.special_locale_names);
        ArrayList<LocaleInfo> localeInfos = new ArrayList<LocaleInfo>(localeList.size());
        for (String locale : localeList) {
            final Locale local = Locale.forLanguageTag(locale.replace('_', '-'));

            if (local == null || "und".equals(local.getLanguage()) || local.getLanguage().isEmpty()
                    || local.getCountry().isEmpty()) {
                continue;
            }

            if (!localeInfos.isEmpty()) {
                LocaleInfo preLocaleInfo = localeInfos.get(localeInfos.size() - 1);
                if (preLocaleInfo.getLocale().getLanguage().equals(local.getLanguage()) &&
                        !preLocaleInfo.getLocale().getLanguage().equals("zz")) {
                    String preLocaleInfoNameNew = getName(preLocaleInfo.getLocale(), specialLocaleCodes, specialLocaleNames);

                    Log.v(TAG, "preLocaleInfo" + preLocaleInfo.getLabel() + " to " + preLocaleInfoNameNew);
                    localeInfos.remove(localeInfos.size() - 1);
                    localeInfos.add(new LocaleInfo(getTitleCase(preLocaleInfoNameNew),preLocaleInfo.getLocale()));

					String LocaleInfoNameNew = getName(local, specialLocaleCodes, specialLocaleNames);
                    Log.v(TAG, "  and adding "+ getTitleCase(LocaleInfoNameNew));
                    localeInfos.add(new LocaleInfo(getTitleCase(LocaleInfoNameNew), local));
                } else {
                    String showName = getTitleCase(local.getDisplayLanguage(local));
                    Log.v(TAG, "adding "+showName);
                    localeInfos.add(new LocaleInfo(showName, local));
                }
            } else {
                Log.v(TAG, "adding initial language : "+ getTitleCase(local.getDisplayLanguage(local)));
                localeInfos.add(new LocaleInfo(getTitleCase(local.getDisplayLanguage(local)), local));
            }
        }
        Collections.sort(localeInfos);
        return localeInfos;
    }

    private int getLocaleInfos(ArrayAdapter<LocaleInfo> from, ArrayList<LocaleInfo> to) {
        if (from == null || to == null) return 0;
        String localLanguage = SystemProperties.get("persist.sys.locale", null);
        if(TextUtils.isEmpty(localLanguage)){
            localLanguage = SystemProperties.get("ro.product.locale", null);
        }
        String [] lan = localLanguage.split("-");
        String newlan = null;
        if(lan.length == 2){
           newlan = lan[0]+"_"+lan[1];
        }else if(lan.length == 3){//zh_Hans_CN
           newlan = lan[0]+"_"+lan[2];
        }
        int selectedIndex = 0;
        int defaultSystemLanagerNum = 0;
        int packageSystemLanagerNum = 0;
        boolean ishavelanagage = false;
        boolean ispackageLanager = false;
        ArrayList<String> carriers = Carrier.getCurrentCarriers();
        for (int i = 0; i < from.getCount(); i++) {
            LocaleInfo info = from.getItem(i);
            to.add(i, info);
            String laninfo = info.getLocale().toString();
            if (laninfo.equals(newlan) || laninfo.startsWith(newlan)) {
                 defaultSystemLanagerNum = i;
                 ishavelanagage = true;
            }
             for (int k = 0; k < mCarriers.size(); k++) {
                 Carrier carrier = mCarriers.get(k);
                 String switchlan = Carrier.getCarrierLanguage(
                               carrier.mName);
                 if (switchlan == null){ continue;}
                 String [] swlan = switchlan.split("-");
                 String switchlannew = null;
                 if (swlan.length == 2){
                    switchlannew = swlan[0]+"_"+swlan[1];
                 }else if (swlan.length == 3){//zh_Hans_CN
                    switchlannew = swlan[0]+"_"+swlan[2];
                 }
                if (info.getLocale().toString().equals(switchlannew)) {
                   packageSystemLanagerNum = i;
                   ispackageLanager = true;
                   break;
                }
             }
        }

        if(ishavelanagage){
           selectedIndex = defaultSystemLanagerNum;
        }else if(ispackageLanager){
           selectedIndex = packageSystemLanagerNum;
        }

        return selectedIndex;
    }
}

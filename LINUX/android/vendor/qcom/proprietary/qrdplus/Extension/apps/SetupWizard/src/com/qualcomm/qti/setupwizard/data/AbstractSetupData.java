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
 * Copyright (C) 2013 The CyanogenMod Project
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

package com.qualcomm.qti.setupwizard.data;

import android.content.Context;
import android.os.Bundle;

import java.util.ArrayList;

import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.protect.SetupProPage;
import com.qualcomm.qti.setupwizard.moreprotect.SetupMoreProPage;
import com.qualcomm.qti.setupwizard.sim.SetupSimPage;
import com.qualcomm.qti.setupwizard.wifi.SetupWifiPage;

public class AbstractSetupData implements SetupDataCallbacks {

    private static final String TAG = AbstractSetupData.class.getSimpleName();

    private Context mContext;
    private ArrayList<SetupDataCallbacks> mListeners = new ArrayList<SetupDataCallbacks>();
    private PageList mPageList;

    public AbstractSetupData(Context context) {
        mContext = context;
        mPageList = onNewPageList();
    }

    private PageList onNewPageList() {
        if (mContext.getResources().getBoolean(R.bool.config_hide_avg)) {
            return new PageList(new SetupWifiPage(mContext, this, R.string.wifi_page),
                new SetupSimPage(mContext, this, R.string.sim_page),
                new SetupProPage(mContext, this, R.string.protect_page)
         );
        } else {
            return new PageList(new SetupWifiPage(mContext, this, R.string.wifi_page),
                new SetupSimPage(mContext, this, R.string.sim_page),
                new SetupProPage(mContext, this, R.string.protect_page),
                new SetupMoreProPage(mContext, this, R.string.more_protection_page)
         );
        }
    }

    @Override
    public void onPageLoaded(Page page) {
        for (int i = 0; i < mListeners.size(); i++) {
            mListeners.get(i).onPageLoaded(page);
        }
    }

    @Override
    public void onPageTreeChanged() {
        for (int i = 0; i < mListeners.size(); i++) {
            mListeners.get(i).onPageTreeChanged();
        }
    }

    @Override
    public void onPageFinished(Page page) {
        for (int i = 0; i < mListeners.size(); i++) {
            mListeners.get(i).onPageFinished(page);
        }
    }

    @Override
    public Page getPage(String key) {
        return findPage(key);
    }

    @Override
    public Page getPage(int key) {
        return findPage(key);
    }

    public Page findPage(String key) {
        return mPageList.findPage(key);
    }

    public Page findPage(int key) {
        return mPageList.findPage(key);
    }

    public void load(Bundle savedValues) {
        for (String key : savedValues.keySet()) {
            mPageList.findPage(key).resetData(savedValues.getBundle(key));
        }
    }

    public Bundle save() {
        Bundle bundle = new Bundle();
        for (Page page : getPageList()) {
            bundle.putBundle(page.getKey(), page.getData());
        }
        return bundle;
    }

    public void addPage(int index, Page page) {
        mPageList.add(index, page);
        onPageTreeChanged();
    }

    public void removePage(Page page) {
        mPageList.remove(page);
        onPageTreeChanged();
    }

    public void registerListener(SetupDataCallbacks listener) {
        mListeners.add(listener);
    }

    public PageList getPageList() {
        return mPageList;
    }

    public void unregisterListener(SetupDataCallbacks listener) {
        mListeners.remove(listener);
    }
}

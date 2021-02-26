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

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;

public abstract class Page implements PageNode {

    public static final String KEY_PAGE_ARGUMENT = "key_arg";

    private SetupDataCallbacks mCallbacks;

    private Bundle mData = new Bundle();
    private String mPage;
    private int mPageResourceId;
    private boolean mRequired = false;
    private boolean mCompleted = false;

    protected Page(Context context, SetupDataCallbacks callbacks, int pageResourceId) {
        mCallbacks = callbacks;
        mPageResourceId = pageResourceId;
        mPage = context.getString(mPageResourceId);
    }

    public Bundle getData() {
        return mData;
    }

    /*public String getTitle() {
        return mTitle;
    }*/

    public boolean isRequired() {
        return mRequired;
    }

    @Override
    public Page findPage(String key) {
        return getKey().equals(key) ? this : null;
    }

    @Override
    public Page findPage(int id) {
        return getId() == id ? this : null;
    }

    public abstract Fragment createFragment();

    public abstract int getSetupBGResId();

    public abstract int getSetupTitleResId();

    public abstract int getNextButtonResId();

    public int getId() {
        return mPageResourceId;
    }

    public String getKey() {
        return mPage;
    }

    public boolean isCompleted() {
        return mCompleted;
    }

    public void setCompleted(boolean completed) {
        mCompleted = completed;
    }

    public void resetData(Bundle data) {
        mData = data;
        notifyDataChanged();
    }

    public void notifyDataChanged() {
        mCallbacks.onPageLoaded(this);
    }

    public Page setRequired(boolean required) {
        mRequired = required;
        return this;
    }
}

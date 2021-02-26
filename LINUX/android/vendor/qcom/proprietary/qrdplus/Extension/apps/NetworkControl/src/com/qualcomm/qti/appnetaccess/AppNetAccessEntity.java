/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import java.text.Collator;
import java.util.Comparator;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Parcel;
import android.os.Parcelable;

public class AppNetAccessEntity implements Parcelable {
    PackageInfo mPackageInfo;
    PackageManager mManager;
    String mAppLable;
    private int mWifiDisable = 0;
    private int mMobileDisable = 0;
    int mId;

    public AppNetAccessEntity() {

    }

    public AppNetAccessEntity(PackageInfo info, PackageManager pm) {
        this.mPackageInfo = info;
        this.mManager = pm;
        this.mAppLable = info.applicationInfo.loadLabel(mManager).toString();
    }

    public void setId(int id) {
        this.mId = id;
    }

    public int getId() {
        return mId;
    }

    public PackageInfo getPackageInfo() {
        return mPackageInfo;
    }

    public String getAppLable() {
        return mAppLable;
    }

    public void setAppLable(String appLable) {
        this.mAppLable = appLable;
    }

    public void setWifiType(int type) {
        this.mWifiDisable = type;
    }

    public int getWifiType() {
        return mWifiDisable;
    }

    public void setMobileType(int type) {
        this.mMobileDisable = type;
    }

    public int getMobileType() {
        return mMobileDisable;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel arg0, int arg1) {
        // Do nothing here.
    }

    public static class DisplayNameComparator implements
            Comparator<AppNetAccessEntity> {
        private final Collator mCollator = Collator.getInstance();

        public DisplayNameComparator() {
            mCollator.setStrength(Collator.PRIMARY);
        }

        public final int compare(AppNetAccessEntity a, AppNetAccessEntity b) {
            CharSequence sa = a.getAppLable().trim();
            if (sa == null)
                sa = a.getPackageInfo().applicationInfo.name;
            CharSequence sb = b.getAppLable().trim();
            if (sb == null)
                sb = b.getPackageInfo().applicationInfo.name;
            return mCollator.compare(sa.toString(), sb.toString());
        }
    }
}

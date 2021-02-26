/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
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

package com.qualcomm.qti.appnetaccess;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.NetworkInterface;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import com.qualcomm.qti.appnetaccess.R;

import android.Manifest;
import android.app.Fragment;
import android.app.LoaderManager;
import android.app.ProgressDialog;
import android.content.AsyncTaskLoader;
import android.content.BroadcastReceiver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.Loader;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.database.Cursor;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Bundle;
import android.os.INetworkManagementService;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;



public class AppNetAccessUI extends Fragment implements
        LoaderManager.LoaderCallbacks<List<PackageInfo>> {
    private static String TAG = "AppOpsPermiss";
    private static PackageManager mPackageManager = null;
    public static String KEY_MOBILE_NET = ".mobile";
    public static String KEY_WIFI_NET = ".wifi";
    private static final int RESTRICT_TYPE_DATA = 0;
    private static final int RESTRICT_TYPE_WLAN = 1;
    private static final int NETWORK_RESTRICT = 1;
    private static final int NETWORK_ALLAW = 0;
    private ListView mListview = null;
    private TextView mEmptyView = null;
    private BrowseApplicationInfoAdapter mBrowseAppAdapter = null;
    private boolean mIsBindingView = false;
    private ProgressDialog mPd;
    private AppNetAccessDao mAccessDao;
    private AppNetAccessSocket mSocket;
    private boolean mConnected = false;


    public AppNetAccessUI() {
        Bundle args = new Bundle();
        args.putString("template", "template");
        setArguments(args);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.app_ops_permiss, container, false);
        mListview = (ListView) v.findViewById(R.id.app_lv);
        mEmptyView = (TextView) v.findViewById(R.id.empty);
        return v;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mBrowseAppAdapter = new BrowseApplicationInfoAdapter(getActivity());
        mListview.setAdapter(mBrowseAppAdapter);
        mPackageManager = getActivity().getPackageManager();
        mAccessDao = AppNetAccessDao.getInstance(getActivity());
        mPd = new ProgressDialog(getActivity());
        mPd.setMessage(getString(R.string.loading));
        mPd.show();
        getLoaderManager().initLoader(0, null, this);
    }

    @Override
    public void onResume() {
        super.onResume();
        mSocket = new AppNetAccessSocket();
        mConnected = mSocket.connectToSocket();
    }

    @Override
    public void onPause() {
        mSocket.closeSocket();
        mConnected = false;
        super.onPause();
    }

    @Override
    public Loader<List<PackageInfo>> onCreateLoader(int id, Bundle args) {
        return new AppListCustLoader(getActivity());
    }

    @Override
    public void onLoadFinished(Loader<List<PackageInfo>> loader,
            List<PackageInfo> data) {
        if (data == null || data.size() < 0) {
            mEmptyView.setVisibility(View.VISIBLE);
            mListview.setVisibility(View.GONE);
        } else {
            mEmptyView.setVisibility(View.GONE);
            mListview.setVisibility(View.VISIBLE);
            setPackageslist(data);
        }
        if (mPd != null && mPd.isShowing()) {
            mPd.dismiss();
        }
    }

    private void setPackageslist(List<PackageInfo> data) {
        List<AppNetAccessEntity> list = new ArrayList<AppNetAccessEntity>();
        for (PackageInfo info : data) {
            AppNetAccessEntity item = new AppNetAccessEntity(info, mPackageManager);
            list.add(item);
        }
        Collections.sort(list,
                new AppNetAccessEntity.DisplayNameComparator());
        mBrowseAppAdapter.setData(list);
        mAccessDao.insertOrUpdate(list);
    }

    @Override
    public void onLoaderReset(Loader<List<PackageInfo>> loader) {
        mBrowseAppAdapter.setData(null);
    }

    public static class AppListCustLoader extends AsyncTaskLoader<List<PackageInfo>> {

        List<PackageInfo> mApps;

        public AppListCustLoader(Context context) {
            super(context);
        }

        @Override
        public List<PackageInfo> loadInBackground() {
            List<PackageInfo> lst = buildPackageName(0, null);
            return lst;
        }

        @Override
        public void deliverResult(List<PackageInfo> apps) {
            if (isReset()) {
                if (apps != null) {
                    onReleaseResources(apps);
                }
            }
            List<PackageInfo> oldApps = apps;
            mApps = apps;

            if (isStarted()) {
                super.deliverResult(apps);
            }
            if (oldApps != null) {
                onReleaseResources(oldApps);
            }
        }
        @Override
        protected void onStartLoading() {
            onContentChanged();

            if (mApps != null) {
                deliverResult(mApps);
            }

            if (takeContentChanged() || mApps == null) {
                forceLoad();
            }
        }

        @Override
        protected void onStopLoading() {
            cancelLoad();
        }

        @Override
        public void onCanceled(List<PackageInfo> apps) {
            super.onCanceled(apps);
            onReleaseResources(apps);
        }
        @Override
        protected void onReset() {
            super.onReset();
            onStopLoading();
            if (mApps != null) {
                onReleaseResources(mApps);
                mApps = null;
            }

        }
        protected void onReleaseResources(List<PackageInfo> apps) {
        }
    }

    public static List<PackageInfo> buildPackageName(int uid, String packageName) {
        List<PackageInfo> listContents = mPackageManager
                .getInstalledPackages(PackageManager.GET_PERMISSIONS);
        List<PackageInfo> buildList = new ArrayList<PackageInfo>();
        mPackageManager.getInstalledPackages(PackageManager.GET_PERMISSIONS);
        String[] permisionsStrings;
        for (int i = 0; i < listContents.size(); i++) {
            try {
                permisionsStrings = mPackageManager.getPackageInfo(listContents.get(i).packageName,
                        PackageManager.GET_PERMISSIONS).requestedPermissions;
                if ((listContents.get(i).applicationInfo.flags
                        & ApplicationInfo.FLAG_SYSTEM) != 0) {
                    continue;
                }
                // ignore local application
                if (permisionsStrings != null) {
                    for (String permissionString : permisionsStrings) {
                        if (permissionString.equals(Manifest.permission.INTERNET)) {
                            buildList.add(listContents.get(i));
                            break;
                        }
                    }

                }
            } catch (NameNotFoundException e) {
                Log.w(TAG, e.toString());
            }
        }
        return buildList;
    }

    @Override
    public void onStart() {
        super.onStart();
    }

    class BrowseApplicationInfoAdapter extends BaseAdapter {

        private LayoutInflater mInflater = null;
        private List<AppNetAccessEntity> mListAppInfo = null;
        private List<Integer> mIsMobileSelected = null;
        private List<Integer> mIsWifiSelected = null;
        PackageManager mPm;

        public BrowseApplicationInfoAdapter(Context context) {
            mInflater = (LayoutInflater) context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mIsMobileSelected = new ArrayList<Integer>();
            mIsWifiSelected = new ArrayList<Integer>();
            mPm = context.getPackageManager();
        }

        public void setData(List<AppNetAccessEntity> data) {
            mListAppInfo = data;
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return mListAppInfo != null ? mListAppInfo.size() : 0;
        }

        @Override
        public AppNetAccessEntity getItem(int position) {
            return mListAppInfo.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertview, ViewGroup parent) {
            View view = convertview;
            if (convertview == null) {
                view = mInflater.inflate(R.layout.app_ops_permiss_item, parent,
                        false);
            }
            bindView(position, view);
            return view;
        }

        private void bindView(final int position, View view) {
            mIsBindingView = true;
            final AppNetAccessEntity appInfo = (AppNetAccessEntity) getItem(position);
            final ImageView appIcon = (ImageView) view
                    .findViewById(R.id.app_icon);
            final TextView tvAppLabel = (TextView) view
                    .findViewById(R.id.app_name);
            appIcon.setImageDrawable(appInfo.getPackageInfo().applicationInfo
                    .loadIcon(mPm));
            tvAppLabel.setText(appInfo.getAppLable());
            final CheckBox mobile_cb = (CheckBox) view
                    .findViewById(R.id.app_on_mobile_net);
            final CheckBox wifi_cb = (CheckBox) view
                    .findViewById(R.id.app_on_wifi_net);
            boolean wifiDisabled = false;
            boolean mobileDisabled = false;
            Cursor cursor = null;
            try {
                int uid = appInfo.getPackageInfo().applicationInfo.uid;
                cursor = mAccessDao.query(uid);
                if (cursor != null && cursor.moveToFirst()) {
                    wifiDisabled = cursor.getInt(cursor
                            .getColumnIndexOrThrow(AppNetAccessDao.COL_WIFI)) == 0 ? false
                            : true;
                    mobileDisabled = cursor.getInt(cursor
                            .getColumnIndexOrThrow(AppNetAccessDao.COL_MOBILE)) == 0 ? false
                            : true;
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }

            if (mobileDisabled) {
                mobile_cb.setChecked(true);
            } else {
                mobile_cb.setChecked(mIsMobileSelected.contains(Integer
                        .valueOf(position)));
            }
            mobile_cb.setOnCheckedChangeListener(new OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView,
                        boolean isChecked) {
                    if (mIsBindingView) {
                        // Just return when called during scroll list.
                        return;
                    }
                    if (mConnected) {
                        int uid = appInfo.getPackageInfo().applicationInfo.uid;
                        mSocket.setAppRestrictMobile(isChecked, uid);
                        updateMobileToogle(uid, isChecked);
                        if (isChecked) {
                            mIsMobileSelected.add(Integer.valueOf(position));
                        } else {
                            mIsMobileSelected.remove(Integer.valueOf(position));
                        }
                    }
                }
            });
            if (wifiDisabled) {
                wifi_cb.setChecked(true);
            } else {
                wifi_cb.setChecked(mIsWifiSelected.contains(Integer
                        .valueOf(position)));
            }

            wifi_cb.setOnCheckedChangeListener(new OnCheckedChangeListener() {

                @Override
                public void onCheckedChanged(CompoundButton buttonView,
                        boolean isChecked) {
                    if (mIsBindingView) {
                        // Just return when called during scroll list.
                        return;
                    }
                    if (mConnected) {
                        int uid = appInfo.getPackageInfo().applicationInfo.uid;
                        mSocket.setAppRestrictWlan(isChecked, uid);
                        updateWifiToogle(uid, isChecked);
                        if (isChecked) {
                            mIsWifiSelected.add(Integer.valueOf(position));
                        }else {
                            mIsWifiSelected.remove(Integer.valueOf(position));
                        }
                    }
                }
            });
            mIsBindingView = false;
        }
    }

    private void updateMobileToogle(int uid, boolean tag) {
        try {
            ContentValues values = new ContentValues();
            values.put(AppNetAccessDao.COL_MOBILE, tag ? NETWORK_RESTRICT : NETWORK_ALLAW);
            mAccessDao.update(uid, values);
        } catch (Exception e) {
            Log.w(TAG, e.toString());
        }
    }

    private void updateWifiToogle(int uid, boolean tag) {
        try {
            ContentValues values = new ContentValues();
            values.put(AppNetAccessDao.COL_WIFI, tag ? NETWORK_RESTRICT : NETWORK_ALLAW);
            mAccessDao.update(uid, values);
        } catch (Exception e) {
            Log.w(TAG, e.toString());
        }

    }
}

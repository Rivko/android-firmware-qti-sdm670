/*
 * Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import com.qti.csm.R;
import com.qti.csm.permission.bean.AppPermissionInfo;
import com.qti.csm.permission.utils.PermissionUtils;
import com.qti.csm.utils.L;
import com.qti.csm.utils.Utils;
import com.qti.csm.utils.UtilsSystem;

import java.util.HashMap;
import java.util.List;

public class PermissionActivity extends BaseActivity implements OnCheckedChangeListener {
    private PermissionAdapter mAdapter;
    private HashMap<Integer, AppPermissionInfo> mAppDataInfos;
    private HashMap<Integer, AppPermissionInfo> mAppMmsInfos;
    private List<PackageInfo> mAllApps = null;
    private static final String CHANGE_REQ = "perm changed";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mTvNotice.setVisibility(View.GONE);
        mTvBoot.setText(R.string.mms);
        mTvAuto.setText(R.string.data);
        showDialog();
        MyThread myThread = new MyThread();
        myThread.start();
    }

    class MyThread extends Thread {

        @Override
        public void run() {
            try {
                Utils.loadAllApps(PermissionActivity.this);
                mAllApps = Utils.getAllApps(PermissionActivity.this);
                mAppDataInfos = PermissionUtils.getAppMaps(PermissionActivity.this, mAllApps, "data");
                mAppMmsInfos = PermissionUtils.getAppMaps(PermissionActivity.this, mAllApps, "mms");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAdapter = new PermissionAdapter(PermissionActivity.this, mAllApps);
                        mListView.setAdapter(mAdapter);
                        dialog.dismiss();
                    }
                });

            } catch (Exception ex) {
                Log.i(L.TAG, "getReceivers: " + ex);
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        PermissionUtils.closeSocket();
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        final int uid = (Integer) buttonView.getTag();
        AppPermissionInfo appInfo;
        ActivityManager activity = this.getSystemService(ActivityManager.class);
        switch (buttonView.getId()) {
            case R.id.auto_data:
                appInfo = mAppDataInfos.get(uid);
                if (appInfo != null) {
                    appInfo.isAllow = isChecked;
                    PermissionUtils.saveNewApps(PermissionActivity.this, "data");
                    if (mAppMmsInfos.get(uid).isAllow != isChecked) {
                        mAppMmsInfos.get(uid).isAllow = isChecked;
                        mAdapter.notifyDataSetChanged();
                        PermissionUtils.saveNewApps(PermissionActivity.this, "mms");
                    }
                }
                break;
            case R.id.boot_mms:
                appInfo = mAppMmsInfos.get(uid);
                if (appInfo != null) {
                    appInfo.isAllow = isChecked;
                    if (UtilsSystem.strictOpMode() && uid != -1) {
                        activity.killUid(uid, CHANGE_REQ);
                    }
                    PermissionUtils.saveNewApps(PermissionActivity.this, "mms");
                }
                break;
        }
    }

    /**********************************
     * Permission Adapt
     ******************************/
    class PermissionAdapter extends BaseAdapter {

        private Context mContext;
        private List<PackageInfo> mPermissionApps;

        PermissionAdapter(Context context, List<PackageInfo> permissionApps) {
            mContext = context;
            mPermissionApps = permissionApps;
        }

        @Override
        public int getCount() {
            return mPermissionApps.size();
        }

        @Override
        public Object getItem(int position) {
            return mPermissionApps.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            final PackageInfo res = mPermissionApps.get(position);
            int uid = res.applicationInfo.uid;
            AppPermissionInfo appInfo;

            convertView = LayoutInflater.from(mContext).inflate(R.layout.list_item, null);
            ImageView icon = (ImageView) convertView.findViewById(R.id.icon);
            icon.setImageDrawable(res.applicationInfo.loadIcon(mPackageManager));

            TextView title = (TextView) convertView.findViewById(R.id.title);
            title.setText(res.applicationInfo.loadLabel(mPackageManager));

            appInfo = mAppMmsInfos.get(uid);
            Switch mmsSwitch = (Switch) convertView.findViewById(R.id.boot_mms);
            mmsSwitch.setChecked(appInfo.isAllow);
            mmsSwitch.setTag(uid);
            mmsSwitch.setOnCheckedChangeListener(PermissionActivity.this);

            appInfo = mAppDataInfos.get(uid);
            Switch dataSwitch = (Switch) convertView.findViewById(R.id.auto_data);
            dataSwitch.setTag(uid);
            dataSwitch.setChecked(appInfo.isAllow);
            dataSwitch.setOnCheckedChangeListener(PermissionActivity.this);
            return convertView;
        }
    }

}

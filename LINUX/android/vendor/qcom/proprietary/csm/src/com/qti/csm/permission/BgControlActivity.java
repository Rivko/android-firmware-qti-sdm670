/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission;

import android.app.AppOpsManager;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.List;

import com.qti.csm.permission.bean.AppReceiverInfo;
import com.qti.csm.permission.utils.PermissionUtils;
import com.qti.csm.utils.L;
import com.qti.csm.utils.Utils;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.SystemProperties;
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

public class BgControlActivity extends BaseActivity implements OnCheckedChangeListener {
    PermissionAdapter mAdapter;
    private HashMap<String, List<AppReceiverInfo>> mAutoReceivers = new HashMap<String, List<AppReceiverInfo>>();
    private AppOpsManager mAppOps;

    private static final String CTL_RUN_IN_BACKGROUND = "OP_RUN_IN_BACKGROUND";
    private static final String CTL_AUTO_START = "OP_AUTO_START";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (SystemProperties.getBoolean(Utils.app_restrict, true)) {
            mTvBoot.setText(R.string.background);
            mTvAuto.setText(R.string.correlation_start);
        }
        mTvNotice.setVisibility(View.GONE);
        mAppOps = (AppOpsManager) this.getSystemService(Context.APP_OPS_SERVICE);

        showDialog();
        MyThread myThread = new MyThread();
        myThread.start();
    }

    class MyThread extends Thread {
        @Override
        public void run() {
            try {
                Utils.loadThirdPartyApps(BgControlActivity.this);
                PermissionUtils.loadReceivers(BgControlActivity.this, Utils.getAllApps(BgControlActivity.this));
                mAutoReceivers = PermissionUtils.getAutoAppsMaps();
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        mAdapter = new PermissionAdapter(BgControlActivity.this, PermissionUtils.mAutoApps);
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
    };

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()) {
        case R.id.auto_data:
            // checked item is not allowed auto start
            String packageName = (String) buttonView.getTag();
            List<AppReceiverInfo> autoInfos = mAutoReceivers.get(packageName);
            if (SystemProperties.getBoolean(Utils.app_restrict, true)) {
                setMode(CTL_AUTO_START, packageName, autoInfos, isChecked);
            }
            break;
        case R.id.boot_mms:
            String packageName2 = (String) buttonView.getTag();
            List<AppReceiverInfo> bootInfos = mAutoReceivers.get(packageName2);
                if (SystemProperties.getBoolean(Utils.app_restrict, true)) {
                    setMode(CTL_RUN_IN_BACKGROUND, packageName2, bootInfos, isChecked);
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

            convertView = LayoutInflater.from(mContext).inflate(R.layout.list_item, null);
            ImageView icon = (ImageView) convertView.findViewById(R.id.icon);
            icon.setImageDrawable(res.applicationInfo.loadIcon(mPackageManager));

            TextView title = (TextView) convertView.findViewById(R.id.title);
            title.setText(res.applicationInfo.loadLabel(mPackageManager));

            List<AppReceiverInfo> autoInfos = mAutoReceivers.get(res.packageName);
            // public static final int COMPONENT_ENABLED_STATE_ENABLED = 1;
            Switch bootSwitch = (Switch) convertView.findViewById(R.id.boot_mms);
            bootSwitch.setTag(res.packageName);
            bootSwitch.setVisibility(View.GONE);
            boolean ischecked = true;
            if (autoInfos != null) {
                if (SystemProperties.getBoolean(Utils.app_restrict, true)) {
                    bootSwitch.setVisibility(View.VISIBLE);
                    ischecked = updateSwitch(CTL_RUN_IN_BACKGROUND, res.packageName, autoInfos,
                            bootSwitch);
                }
            }
            bootSwitch.setChecked(ischecked);
            bootSwitch.setOnCheckedChangeListener(BgControlActivity.this);

            Switch autoSwitch = (Switch) convertView.findViewById(R.id.auto_data);
            autoSwitch.setTag(res.packageName);

            if (autoInfos != null) {
                if (SystemProperties.getBoolean(Utils.app_restrict, true)) {
                    ischecked = updateSwitch(CTL_AUTO_START, res.packageName, autoInfos,
                            autoSwitch);
                }
            }
            autoSwitch.setChecked(ischecked);
            autoSwitch.setOnCheckedChangeListener(BgControlActivity.this);

            return convertView;
        }
    }

    private Boolean updateSwitch(String mode, String packageName, List<AppReceiverInfo> autoInfos,
        Switch switchView){
        // Set checked as true by default
        boolean ischecked = true;
        if (packageName ==null || packageName.isEmpty()) return ischecked;

        try {
            Field f = mAppOps.getClass().getDeclaredField(mode);
            if (f != null) {
                int op = f.getInt(mAppOps);
                Log.d(L.RECEIVER_TAG, "updateSwitch op " + op);
                Log.d(L.RECEIVER_TAG, "updateSwitch " + mode + " opToName is "
                        + AppOpsManager.opToName(op));

                PackageInfo packageInfo = null;
                try {
                    packageInfo = mPackageManager.getPackageInfo(packageName,
                            PackageManager.GET_DISABLED_COMPONENTS |
                                    PackageManager.GET_UNINSTALLED_PACKAGES);
                } catch (NameNotFoundException e) {
                    Log.e(L.RECEIVER_TAG, "Exception when retrieving package:" + packageName, e);
                    packageInfo = null;
                    return ischecked;
                }

                for (AppReceiverInfo autoInfo : autoInfos) {
                    ischecked = mAppOps.checkOp(op, packageInfo.applicationInfo.uid, packageName)
                            == AppOpsManager.MODE_ALLOWED;
                }
            } else {
                Log.d(L.RECEIVER_TAG, "updateSwitch get null " + mode + " field");
                switchView.setEnabled(false);
            }
        } catch (Exception e) {
            Log.d(L.RECEIVER_TAG, "e ==" + e);
            switchView.setEnabled(false);
        }
        return ischecked;
    }

    private void setMode(String mode, String packageName, List<AppReceiverInfo> appReceiverInfos,
        boolean isChecked) {
        if (packageName == null || packageName.isEmpty()) return;

        try {
            Field f = mAppOps.getClass().getDeclaredField(mode);
            if (f != null) {
                int op = f.getInt(mAppOps);
                Log.d(L.RECEIVER_TAG, "setMode op " + op);
                Log.d(L.RECEIVER_TAG, "setMode " + mode + " opToName is "
                        + AppOpsManager.opToName(op));

                PackageInfo packageInfo = null;
                try {
                    packageInfo = mPackageManager.getPackageInfo(packageName,
                            PackageManager.GET_DISABLED_COMPONENTS |
                            PackageManager.GET_UNINSTALLED_PACKAGES);
                } catch (NameNotFoundException e) {
                    Log.e(L.RECEIVER_TAG, "Exception when retrieving package:" + packageName, e);
                    packageInfo = null;
                    return;
                }

                for (AppReceiverInfo appReceiverInfo : appReceiverInfos) {
                    Log.i(L.RECEIVER_TAG,
                            "click, receiverInfo:" + packageName + ",component:"
                                    + appReceiverInfo.componentName);
                    mAppOps.setMode(op, packageInfo.applicationInfo.uid, packageName,
                            isChecked ? AppOpsManager.MODE_ALLOWED : AppOpsManager.MODE_IGNORED);
                    mAdapter.notifyDataSetChanged();
                }
            } else {
                Log.d(L.RECEIVER_TAG, "setMode get null " + mode + " field");
            }
        } catch (Exception e) {
            Log.d(L.RECEIVER_TAG, "Exception =" + e);
        }
    }
}

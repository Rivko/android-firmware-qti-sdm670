/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission;

import java.util.HashMap;
import java.util.List;

import com.qti.csm.permission.bean.AppReceiverInfo;
import com.qti.csm.permission.utils.PermissionUtils;
import com.qti.csm.utils.L;
import com.qti.csm.utils.Utils;

import android.Manifest.permission;
import android.content.ComponentName;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
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

public class AutoStartActivity extends BaseActivity implements OnCheckedChangeListener {
    PermissionAdapter mAdapter;
    private HashMap<String, List<AppReceiverInfo>> mAutoReceivers = new HashMap<String, List<AppReceiverInfo>>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mTvBoot.setText(R.string.boot);
        mTvAuto.setText(R.string.auto);
        showDialog();
        MyThread myThread = new MyThread();
        myThread.start();
    }

    class MyThread extends Thread {
        @Override
        public void run() {
            try {
                Utils.loadAllApps(AutoStartActivity.this);
                PermissionUtils.loadReceivers(AutoStartActivity.this, Utils.getAllApps(AutoStartActivity.this));
                mAutoReceivers = PermissionUtils.getAutoAppsMaps();
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        mAdapter = new PermissionAdapter(AutoStartActivity.this, PermissionUtils.mAutoApps);
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
            for (AppReceiverInfo autoInfo : autoInfos) {
                Log.i(L.RECEIVER_TAG,
                        "click, receiverInfo:" + autoInfo.packageName + ",component:" + autoInfo.componentName);
                if (this.checkCallingOrSelfPermission(
                        permission.CHANGE_COMPONENT_ENABLED_STATE) == PackageManager.PERMISSION_GRANTED) {
                    ComponentName c = new ComponentName(autoInfo.packageName, autoInfo.componentName);
                    mPackageManager.setComponentEnabledSetting(c,
                            isChecked ? PackageManager.COMPONENT_ENABLED_STATE_ENABLED
                                    : PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                            0);
                    autoInfo.state = mPackageManager.getComponentEnabledSetting(c);
                    Log.i(L.RECEIVER_TAG, "click, receiverInfo:" + autoInfo.packageName + ",component:"
                            + autoInfo.componentName + ".state:" + autoInfo.state);
                    mAdapter.notifyDataSetChanged();
                }
            }
            break;
        case R.id.boot_mms:
            String packageName2 = (String) buttonView.getTag();
            List<AppReceiverInfo> bootInfos = mAutoReceivers.get(packageName2);
            for (AppReceiverInfo bootInfo : bootInfos) {
                if (bootInfo.hasBoot) {
                    if (this.checkCallingOrSelfPermission(
                            permission.CHANGE_COMPONENT_ENABLED_STATE) == PackageManager.PERMISSION_GRANTED) {
                        ComponentName c = new ComponentName(bootInfo.packageName, bootInfo.componentName);
                        mPackageManager.setComponentEnabledSetting(c,
                                isChecked ? PackageManager.COMPONENT_ENABLED_STATE_ENABLED
                                        : PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                                0);
                        bootInfo.state = mPackageManager.getComponentEnabledSetting(c);
                        Log.i(L.RECEIVER_TAG, "boot click, receiverInfo:" + bootInfo.packageName + ",component:"
                                + bootInfo.componentName + ".state:" + bootInfo.state);
                        mAdapter.notifyDataSetChanged();
                    }
                }
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
                for (AppReceiverInfo autoInfo : autoInfos) {
                    if (autoInfo.hasBoot) {
                        if (autoInfo.state == PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
                            ischecked = false;
                        }
                        bootSwitch.setVisibility(View.VISIBLE);
                        break;
                    }
                }
            }
            bootSwitch.setChecked(ischecked);
            bootSwitch.setOnCheckedChangeListener(AutoStartActivity.this);

            Switch autoSwitch = (Switch) convertView.findViewById(R.id.auto_data);
            autoSwitch.setTag(res.packageName);
            if (autoInfos != null) {
                for (AppReceiverInfo autoInfo : autoInfos) {
                    if (autoInfo.state != PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
                        ischecked = true;
                        break;
                    } else {
                        ischecked = false;
                    }
                }
            }
            autoSwitch.setChecked(ischecked);
            autoSwitch.setOnCheckedChangeListener(AutoStartActivity.this);

            return convertView;
        }
    }

}

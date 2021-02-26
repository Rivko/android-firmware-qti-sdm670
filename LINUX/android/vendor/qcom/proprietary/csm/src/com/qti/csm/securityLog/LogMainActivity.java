/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import java.util.List;

import com.qti.csm.securityLog.utils.LogUtils;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import com.qti.csm.R;

@SuppressLint({ "InflateParams", "ViewHolder" })
public class LogMainActivity extends Activity {
    public static final int TAB_LENGTH = 5;
    private PackageManager mPackageManager = null;
    private String mLogs;
    private View view_log;

    private ListView mList_log = null;
    private List<PackageInfo> mLogApps = null;
    private LogAdapter mLogAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_log);
        mLogApps = LogUtils.getLogApps(this);
        mLogAdapter = new LogAdapter(this, mLogApps);
        mPackageManager = getPackageManager();
        mLogs = LogUtils.getLogs();
        mList_log = (ListView) findViewById(R.id.list_log);
        mList_log.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
                String packageName = mLogApps.get(arg2).packageName;
                String pkg = mLogApps.get(arg2).applicationInfo.loadLabel(mPackageManager).toString();

                ComponentName mComponentName = new ComponentName("com.qti.csm", "com.qti.csm.securityLog.LogDetailsActivity");
                Intent intent = new Intent();
                intent.setComponent(mComponentName);
                intent.putExtra("packageName", packageName);
                intent.putExtra("pkg", pkg);
                intent.putExtra("log", mLogs);
                startActivity(intent);
            }
        });
        mList_log.setAdapter(mLogAdapter);
    }

    /**********************************
     * Log Adapt
     ******************************/
    class LogAdapter extends BaseAdapter {

        private Context mContext;
        private List<PackageInfo> mLogApps;

        LogAdapter(Context context, List<PackageInfo> logApps) {
            mContext = context;
            mLogApps = logApps;
        }

        @Override
        public int getCount() {
            return mLogApps.size();
        }

        @Override
        public Object getItem(int position) {
            return mLogApps.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            final PackageInfo res = mLogApps.get(position);
            convertView = LayoutInflater.from(mContext).inflate(R.layout.list_log, null);
            ImageView icon = (ImageView) convertView.findViewById(R.id.icon);
            icon.setImageDrawable(res.applicationInfo.loadIcon(mPackageManager));

            TextView title = (TextView) convertView.findViewById(R.id.title);
            title.setText(res.applicationInfo.loadLabel(mPackageManager));

            ImageView log = (ImageView) convertView.findViewById(R.id.log);
            log.setImageDrawable(getDrawable(R.drawable.next));
            return convertView;
        }
    }
}

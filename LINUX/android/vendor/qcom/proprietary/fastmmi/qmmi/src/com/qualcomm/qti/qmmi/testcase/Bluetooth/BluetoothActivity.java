/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.DeviceInfo;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class BluetoothActivity extends BaseActivity {
    List<DeviceInfo> mDeviceList = new ArrayList<DeviceInfo>();
    Set<BluetoothDevice> bondedDevices;
    ListView mListView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LogUtils.logi("Bluetooth Activity run");
        mListView = (ListView) findViewById(R.id.devices_list);
        mListView.setAdapter(mAdapter);
    }

    public void updateAdapter() {
        mAdapter.notifyDataSetChanged();
    }

    @Override
    public void updateView(Bundle bundle) {
        // TODO Auto-generated method stub

        super.updateView(bundle);

        mDeviceList = (ArrayList) bundle.getParcelableArrayList("BluetoothList");
        LogUtils.logi("size : " + mDeviceList.size());

        if ( mDeviceList != null && mDeviceList.size() > 0){

            /** for debug
            for(int i=0; i < mDeviceList.size(); i++){
                DeviceInfo info =  mDeviceList.get(i);
                LogUtils.logi("============= ++++ : " + info.getName()
                + " : " + info.getAddress() + " : " + info.getRssi());
            }**/

            updateAdapter();
        }
    }

    BaseAdapter mAdapter = new BaseAdapter() {

        @Override
        public DeviceInfo getItem(int i) {
            return mDeviceList != null ? mDeviceList.get(i) : null;
        }

        @Override
        public int getCount() {
            return mDeviceList != null ? mDeviceList.size() : 0;
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int index, View convertView, ViewGroup parent) {

            if (convertView == null){
                //convertView = mInflater.inflate(R.layout.bluetooth_item, null);
                LayoutInflater inflater = LayoutInflater.from(parent.getContext());
                convertView = inflater.inflate(R.layout.bluetooth_item, parent, false);
            }

            DeviceInfo deviceInfo = getItem(index);
            if (deviceInfo == null) return convertView;

            ImageView image = (ImageView) convertView.findViewById(R.id.bluetooth_image);
            TextView name = (TextView) convertView.findViewById(R.id.bluetooth_text);
            TextView rssi = (TextView) convertView.findViewById(R.id.bluetooth_rssi);

            name.setText(deviceInfo.getName());
            rssi.setText(Integer.toString(deviceInfo.getRssi()));
            image.setImageResource(android.R.drawable.stat_sys_data_bluetooth);

            return convertView;
        }

    };

    protected void onResume() {
        super.onResume();
        updateAdapter();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    public void finish() {
        super.finish();
    }

    @Override
    protected int getLayoutId() {
        return R.layout.bluetooth_act;
    }
}
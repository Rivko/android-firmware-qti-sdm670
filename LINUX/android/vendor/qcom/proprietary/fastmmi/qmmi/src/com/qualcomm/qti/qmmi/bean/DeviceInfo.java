/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.bean;

import android.os.Parcel;
import android.os.Parcelable;

public class DeviceInfo implements Parcelable {

    private String name = "";
    private String address = "";
    private int rssi;

    public DeviceInfo(String name, String address, int rssi) {
        super();
        this.name = name;
        this.address = address;
        this.rssi = rssi;
    }

    public DeviceInfo(Parcel source) {
        this.name = source.readString();
        this.address = source.readString();
        this.rssi = source.readInt();
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public int getRssi() {
        return rssi;
    }

    public void setRssi(int rssi) {
        this.rssi = rssi;
    }

    @Override
    public int describeContents() {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int arg1) {
        // TODO Auto-generated method stub
        dest.writeString(name);
        dest.writeString(address);
        dest.writeInt(rssi);
    }

    public static final Parcelable.Creator<DeviceInfo> CREATOR = new Creator<DeviceInfo>(){

        @Override
        public DeviceInfo createFromParcel(Parcel source) {
            // TODO Auto-generated method stub
            DeviceInfo info = new DeviceInfo(source.readString(), source.readString(), source.readInt());
            return info;
        }

        @Override
        public DeviceInfo[] newArray(int size) {
            // TODO Auto-generated method stub
            return new DeviceInfo[size];
        }

    };
}

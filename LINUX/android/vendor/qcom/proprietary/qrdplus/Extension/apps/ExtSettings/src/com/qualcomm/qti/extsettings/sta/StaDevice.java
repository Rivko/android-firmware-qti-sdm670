/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * Copyright (c) 2014, The Linux Foundation. All Rights Reserved.
 */

package com.qualcomm.qti.extsettings.sta;

import android.net.wifi.WifiDevice;

public class StaDevice {

    // The device MAC address is the unique id of a Wi-Fi STA
    public String deviceAddress = "";

    // The device name is a readable string of a Wi-Fi STA
    public String deviceName = "";

    // The device state is the state of a Wi-Fi STA
    public int deviceState;

    // These definitions are for deviceState
    public static final int DISCONNECTED = 0;
    public static final int CONNECTED = 1;
    public static final int BLACKLISTED = 2;

    /**
     * For example: android-cdf2c050157591a1 01:28:27:bf:8c:36:ff 1
     * First one corresponds to deviceName and second one deviceAddress and
     * third one indicate deviceState, deviceState shall be 0(Disconnected) or 1(Connected).
     * <p>
     * Note: The events formats can be looked up in the hotpot code
     *
     * @param dataString Original data
     */
    public StaDevice(String dataString) throws IllegalArgumentException {
        String[] tokens = dataString.split(" ");

        if (tokens.length < 3) {
            throw new IllegalArgumentException();
        }
        deviceName = tokens[0];
        deviceAddress = tokens[1];
        deviceState = Integer.parseInt(tokens[2]);

    }

    public StaDevice(WifiDevice device) {
        deviceName = device.deviceName;
        deviceAddress = device.deviceAddress;
        deviceState = device.deviceState;
    }

    /**
     * Only compare deviceAddress to decide whether StaDevices are equal.
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null || !(obj instanceof StaDevice)) {
            return false;
        }

        StaDevice other = (StaDevice) obj;
        if (deviceAddress == null) {
            return (other.deviceAddress == null);
        } else {
            return deviceAddress.equals(other.deviceAddress);
        }
    }
}

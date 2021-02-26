/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * This file is originated from Android Open Source Project,
 * platform/frameworks/opt/net/wifi.git /service/java/com/android/server/wifi/ConfigurationMap.java
 */

package com.qualcomm.qti.server.wigig;

import android.net.wifi.WifiConfiguration;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class ConfigurationMap {
    private final Map<Integer, WifiConfiguration> mPerID = new HashMap<>();
    private final Map<Integer, WifiConfiguration> mPerConfigKey = new HashMap<>();
    private final Map<String, Integer> mPerFQDN = new HashMap<>();

    // RW methods:
    public WifiConfiguration put(int netid, WifiConfiguration config) {
        WifiConfiguration current = mPerID.put(netid, config);
        mPerConfigKey.put(config.configKey().hashCode(), config);   // This is ridiculous...
        if (config.FQDN != null && config.FQDN.length() > 0) {
            mPerFQDN.put(config.FQDN, netid);
        }
        return current;
    }

    public WifiConfiguration remove(int netID) {
        WifiConfiguration config = mPerID.remove(netID);
        if (config == null) {
            return null;
        }
        mPerConfigKey.remove(config.configKey().hashCode());

        Iterator<Map.Entry<String, Integer>> entries = mPerFQDN.entrySet().iterator();
        while (entries.hasNext()) {
            if (entries.next().getValue() == netID) {
                entries.remove();
                break;
            }
        }
        return config;
    }

    public void clear() {
        mPerID.clear();
        mPerConfigKey.clear();
        mPerFQDN.clear();
    }

    // RO methods:
    public WifiConfiguration get(int netid) {
        return mPerID.get(netid);
    }

    public int size() {
        return mPerID.size();
    }

    public boolean isEmpty() {
        return mPerID.size() == 0;
    }

    public WifiConfiguration getByFQDN(String fqdn) {
        Integer id = mPerFQDN.get(fqdn);
        return id != null ? mPerID.get(id) : null;
    }

    public WifiConfiguration getByConfigKey(String key) {
        if (key == null) {
            return null;
        }
        for (WifiConfiguration config : mPerID.values()) {
            if (config.configKey().equals(key)) {
                return config;
            }
        }
        return null;
    }

    public WifiConfiguration getByConfigKeyID(int id) {
        return mPerConfigKey.get(id);
    }

    public Collection<WifiConfiguration> getEnabledNetworks() {
        List<WifiConfiguration> list = new ArrayList<>();
        for (WifiConfiguration config : mPerID.values()) {
            if (config.status != WifiConfiguration.Status.DISABLED) {
                list.add(config);
            }
        }
        return list;
    }

    public WifiConfiguration getEphemeral(String ssid) {
        for (WifiConfiguration config : mPerID.values()) {
            if (ssid.equals(config.SSID) && config.ephemeral) {
                return config;
            }
        }
        return null;
    }

    public Collection<WifiConfiguration> values() {
        return mPerID.values();
    }
}

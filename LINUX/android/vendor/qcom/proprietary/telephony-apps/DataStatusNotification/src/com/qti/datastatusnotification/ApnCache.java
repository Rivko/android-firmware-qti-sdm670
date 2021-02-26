/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.qualcomm.datastatusnotification;

import android.util.Log;
import android.util.ArraySet;

import java.util.Set;
import java.util.ArrayList;
import java.util.HashMap;

public class ApnCache {
    private static final String TAG = "ApnCache";
    private static final boolean DBG = true;

    HashMap<String, ArraySet<String>> mApnHashMap =
        new HashMap<String, ArraySet<String>>();

    /**
     * Return true if this cache contains a mapping for the specified key.
     */
    public boolean containsKey(String key) {
        return mApnHashMap.containsKey(key);
    }

    /**
     * Map key to value and store the association.
     * If the key already exists then insert the value in to the current
     * Set of values.
     */
    public void put(String key, String value) {
        if (mApnHashMap.containsKey(key)) {
            mApnHashMap.get(key).add(value);
        } else {
            ArraySet<String> values = new ArraySet<String>();
            values.add(value);
            mApnHashMap.put(key, values);
        }
    }

    /**
     * Returns the value to which the specified key is mapped, or null if
     * this cache contains no mapping for the key.
     */
    public String get(String key) {
        ArraySet<String> values = mApnHashMap.get(key);
        if (values != null) {
            StringBuilder sb = new StringBuilder();
            int size = values.size();
            int index = 0;
            for (int i = 0; i < size; i++) {
                String value = values.valueAt(i);
                sb.append(value);
                if (i != size-1) {
                    sb.append(",");
                }
            }
            return sb.toString();
        } else {
            return null;
        }
    }

    /**
     * Return list of key not present in otherCache.
     */
    public ArraySet<String> getMissingKeys(ApnCache otherCache) {
        ArraySet<String> missingKeyList = new ArraySet<String>();

        for (String key : mApnHashMap.keySet()) {
            if (!otherCache.containsKey(key)) {
                missingKeyList.add(key);
            }
        }

        log("Missing key size " + missingKeyList.size());
        return missingKeyList;
    }

    /**
     * Returns a Set of the keys contained in this cache.
     */
    public Set<String> keySet() {
        return mApnHashMap.keySet();
    }

    private void log(String str) {
        if (DBG) {
            Log.d(TAG, str);
        }
    }
}

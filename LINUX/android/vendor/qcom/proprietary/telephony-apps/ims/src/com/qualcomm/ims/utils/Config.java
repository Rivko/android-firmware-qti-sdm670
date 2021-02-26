/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.utils;

import android.content.Context;

/**
 * Used for retrieving IMS configuration values.
 */
public class Config {
    private static final String LOG_TAG = "Config";

    private Config() {
        // Empty private constructor.
    }

    /*
     * Method to obtain the boolean value of a specified config.
     * @param context The Context.
     * @param config The configuration to be queried.
     *
     * @return The boolean value of the configuration.
     */
    public static boolean isConfigEnabled(Context context, int config) {
        try {
            return context.getResources().getBoolean(config);
        } catch (NullPointerException npe) {
            Log.e(LOG_TAG, "isConfigEnabled :: Null context! config=" + config);
        } catch (Exception exc) {
            Log.e(LOG_TAG, "isConfigEnabled :: Error getting boolean value for config="
                    + config + "\n" + exc);
        }
        return false;
    }

}


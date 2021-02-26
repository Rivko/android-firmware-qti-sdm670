/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.oma.drm.ui.DrmRightsInstallerActivity;

public class DrmRightsReceiver extends BroadcastReceiver {
    private static final String TAG = "OmaDrm:DrmRightsReceiver";
    public static final String WAP_PUSH = "android.provider.Telephony.WAP_PUSH_RECEIVED";
    public static final String DRM_MIMETYPE_RIGHTS_XML_STRING = "application/vnd.oma.drm.rights+xml";

    /**
     * The "application/vnd.oma.drm.rights+wbxml" mime type.
     */
    public static final String DRM_MIMETYPE_RIGHTS_WBXML_STRING = "application/vnd.oma.drm.rights+wbxml";

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        if (action.equals(WAP_PUSH)) {
            Log.d(TAG, "Receiving DrmIntent.WAP_PUSH");
            Intent drmIntent = new Intent(context,
                    DrmRightsInstallerActivity.class);
            drmIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            drmIntent.setAction("android.intent.action.VIEW");
            drmIntent.setDataAndType(intent.getData(), intent.getType());
            context.startActivity(drmIntent);
        }
    }
}

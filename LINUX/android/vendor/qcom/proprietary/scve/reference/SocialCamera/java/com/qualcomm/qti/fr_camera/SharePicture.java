/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera;

import android.content.ActivityNotFoundException;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.fr_camera.fr.UserDatabase;
import com.qualcomm.qti.fr_camera.ui.FaceLayoutManager;

/**
 * This class contains utility methods for sharing pictures
 */
public final class SharePicture {
    private static final String TAG = "SocialCamera";

    // Not constructable
    private SharePicture() {}

    /**
     * Send the specified picture to the specified set of users
     * @param context the active {@link Context}
     * @param picture the {@link Uri} (resolvable via {@link ContentResolver}) of the picture to
     *                send
     * @param users the set of {@link FaceLayoutManager.SelectedUser}s to send the picture to, and
     *              the method via which the picture will be shared with these users
     */
    public static void send(Context context, Uri picture, FaceLayoutManager.SelectedUser[] users) {
        boolean sendEmail = false;
        boolean sendSms = false;
        StringBuilder emails = new StringBuilder();
        StringBuilder sms = new StringBuilder();
        for (FaceLayoutManager.SelectedUser u : users) {
            if (u.shareMethod == UserDatabase.ShareMethod.EMAIL) {
                sendEmail = true;
                emails.append(u.userRecord.email).append(',');
            } else if (u.shareMethod == UserDatabase.ShareMethod.SMS) {
                sendSms = true;
                sms.append(u.userRecord.sms).append(',');
            }
        }

        if (sendEmail) {
            Uri emailUri = Uri.fromParts("mailto", emails.toString(), null);
            Intent emailIntent = new Intent(Intent.ACTION_SENDTO, emailUri);
            emailIntent.putExtra(Intent.EXTRA_SUBJECT,
                    context.getResources().getString(R.string.social_sharing_email_subject));
            emailIntent.putExtra(Intent.EXTRA_STREAM, picture);

            try {
            context.startActivity(emailIntent);
            } catch (ActivityNotFoundException e) {
                Log.e(TAG, "Failed to share picture via email", e);
                Toast.makeText(context, R.string.social_sharing_error_no_email_app,
                        Toast.LENGTH_SHORT).show();
            }
        }

        if (sendSms) {
            // TODO: really send SMS
            Uri smsUri = Uri.fromParts("tel", sms.toString(), null);
            Toast.makeText(context, "Simulated SMS send to [" + smsUri.toString() + "]",
                    Toast.LENGTH_SHORT).show();
        }
    }
}

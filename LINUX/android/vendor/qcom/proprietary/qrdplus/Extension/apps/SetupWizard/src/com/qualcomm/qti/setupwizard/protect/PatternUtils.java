/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.setupwizard.protect;

import android.annotation.TargetApi;
import android.app.ActivityManager;
import android.app.admin.DevicePolicyManager;
import android.app.trust.TrustManager;
import android.content.ContentResolver;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.os.storage.IMountService;
import android.os.storage.StorageManager;
import android.provider.Settings;
import android.util.Log;

import com.android.internal.widget.ILockSettings;
import com.google.android.collect.Lists;

import java.util.List;

/**
 * Utilities for the lock pattern and its settings.
 */
public class PatternUtils {

    private static final String TAG = "LockPatternUtils";

    /**
     * The minimum number of dots in a valid pattern.
     */
    public static final int MIN_LOCK_PATTERN_SIZE = 4;

    /**
     * The minimum size of a valid password.
     */

    @Deprecated
    public final static String PATTERN_EVER_CHOSEN_KEY = "lockscreen.patterneverchosen";
    public final static String PASSWORD_TYPE_KEY = "lockscreen.password_type";

    // Maximum allowed number of repeated or ordered characters in a sequence before we'll
    // consider it a complex PIN/password.

    private final Context mContext;
    private final ContentResolver mContentResolver;
    private DevicePolicyManager mDevicePolicyManager;
    private ILockSettings mLockSettingsService;


    public DevicePolicyManager getDevicePolicyManager() {
        if (mDevicePolicyManager == null) {
            mDevicePolicyManager =
                    (DevicePolicyManager) mContext.getSystemService(Context.DEVICE_POLICY_SERVICE);
            if (mDevicePolicyManager == null) {
                Log.e(TAG, "Can't get DevicePolicyManagerService: is it running?",
                        new IllegalStateException("Stack trace:"));
            }
        }
        return mDevicePolicyManager;
    }

    private TrustManager getTrustManager() {
        TrustManager trust = (TrustManager) mContext.getSystemService(Context.TRUST_SERVICE);
        if (trust == null) {
            Log.e(TAG, "Can't get TrustManagerService: is it running?",
                    new IllegalStateException("Stack trace:"));
        }
        return trust;
    }

    public PatternUtils(Context context) {
        mContext = context;
        mContentResolver = context.getContentResolver();
    }

    private ILockSettings getLockSettings() {
        if (mLockSettingsService == null) {
            mLockSettingsService = ILockSettings.Stub.asInterface(
                    ServiceManager.getService("lock_settings"));
        }
        return mLockSettingsService;
    }

    /**
     * Save a lock pattern.
     *
     * @param pattern The new pattern to save.
     * @param userId  the user whose pattern is to be saved.
     */
    public void saveLockPattern(List<LockView.Cell> pattern, int userId) {
        this.saveLockPattern(pattern, null, userId);
    }

    /**
     * Save a lock pattern.
     *
     * @param pattern      The new pattern to save.
     * @param savedPattern The previously saved pattern, converted to String format
     * @param userId       the user whose pattern is to be saved.
     */
    public void saveLockPattern(List<LockView.Cell> pattern, String savedPattern, int userId) {
        try {
            if (pattern == null || pattern.size() < MIN_LOCK_PATTERN_SIZE) {
                throw new IllegalArgumentException("pattern must not be null and at least "
                        + MIN_LOCK_PATTERN_SIZE + " dots long.");
            }

            getLockSettings().setLockPattern(patternToString(pattern), savedPattern, userId);
            DevicePolicyManager dpm = getDevicePolicyManager();

            // Update the device encryption password.
            if (userId == UserHandle.USER_OWNER
                    && PatternUtils.isDeviceEncryptionEnabled()) {
                if (!shouldEncryptWithCredentials(true)) {
                    clearEncryptionPassword();
                } else {
                    String stringPattern = patternToString(pattern);
                    updateEncryptionPassword(StorageManager.CRYPT_TYPE_PATTERN, stringPattern);
                }
            }

            setBoolean(PATTERN_EVER_CHOSEN_KEY, true, userId);

            setLong(PASSWORD_TYPE_KEY, DevicePolicyManager.PASSWORD_QUALITY_SOMETHING, userId);
            dpm.setActivePasswordState(DevicePolicyManager.PASSWORD_QUALITY_SOMETHING,
                    pattern.size(), 0, 0, 0, 0, 0, 0, userId);
            onAfterChangingPassword(userId);
        } catch (RemoteException re) {
            Log.e(TAG, "Couldn't save lock pattern " + re);
        }
    }

    /**
     * Update the encryption password if it is enabled
     **/
    private void updateEncryptionPassword(final int type, final String password) {
        if (!isDeviceEncryptionEnabled()) {
            return;
        }
        final IBinder service = ServiceManager.getService("mount");
        if (service == null) {
            Log.e(TAG, "Could not find the mount service to update the encryption password");
            return;
        }

        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... dummy) {
                IMountService mountService = IMountService.Stub.asInterface(service);
                try {
                    mountService.changeEncryptionPassword(type, password);
                } catch (RemoteException e) {
                    Log.e(TAG, "Error changing encryption password", e);
                }
                return null;
            }
        }.execute();
    }

    /**
     * Determine if the device supports encryption, even if it's set to default. This
     * differs from isDeviceEncrypted() in that it returns true even if the device is
     * encrypted with the default password.
     *
     * @return true if device encryption is enabled
     */
    public static boolean isDeviceEncryptionEnabled() {
        final String status = SystemProperties.get("ro.crypto.state", "unsupported");
        return "encrypted".equalsIgnoreCase(status);
    }

    /**
     * Clears the encryption password.
     */
    public void clearEncryptionPassword() {
        updateEncryptionPassword(StorageManager.CRYPT_TYPE_DEFAULT, null);
    }

    /**
     * Deserialize a pattern.
     *
     * @param string The pattern serialized with {@link #patternToString}
     * @return The pattern.
     */
    public static List<LockView.Cell> stringToPattern(String string) {
        if (string == null) {
            return null;
        }

        List<LockView.Cell> result = Lists.newArrayList();

        final byte[] bytes = string.getBytes();
        for (int i = 0; i < bytes.length; i++) {
            byte b = (byte) (bytes[i] - '1');
            result.add(LockView.Cell.of(b / 3, b % 3));
        }
        return result;
    }

    /**
     * Serialize a pattern.
     *
     * @param pattern The pattern.
     * @return The pattern in string form.
     */
    public static String patternToString(List<LockView.Cell> pattern) {
        if (pattern == null) {
            return "";
        }
        final int patternSize = pattern.size();

        byte[] res = new byte[patternSize];
        for (int i = 0; i < patternSize; i++) {
            LockView.Cell cell = pattern.get(i);
            res[i] = (byte) (cell.getRow() * 3 + cell.getColumn() + '1');
        }
        return new String(res);
    }

    private void setBoolean(String secureSettingKey, boolean enabled, int userId) {
        try {
            getLockSettings().setBoolean(secureSettingKey, enabled, userId);
        } catch (RemoteException re) {
            // What can we do?
            Log.e(TAG, "Couldn't write boolean " + secureSettingKey + re);
        }
    }

    private void setLong(String secureSettingKey, long value, int userHandle) {
        try {
            getLockSettings().setLong(secureSettingKey, value, userHandle);
        } catch (RemoteException re) {
            // What can we do?
            Log.e(TAG, "Couldn't write long " + secureSettingKey + re);
        }
    }

    private void onAfterChangingPassword(int userHandle) {
        getTrustManager().reportEnabledTrustAgentsChanged(userHandle);
    }

    public boolean isCredentialRequiredToDecrypt(boolean defaultValue) {
        final int value = Settings.Global.getInt(mContentResolver,
                Settings.Global.REQUIRE_PASSWORD_TO_DECRYPT, -1);
        return value == -1 ? defaultValue : (value != 0);
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    public void setCredentialRequiredToDecrypt(boolean required) {
        if (ActivityManager.getCurrentUser() != UserHandle.USER_OWNER) {
            Log.w(TAG, "Only device owner may call setCredentialRequiredForDecrypt()");
            return;
        }

        if (isDeviceEncryptionEnabled()) {
            Settings.Global.putInt(mContext.getContentResolver(),
                    Settings.Global.REQUIRE_PASSWORD_TO_DECRYPT, required ? 1 : 0);
        }
    }

    private boolean isDoNotAskCredentialsOnBootSet() {
        return mDevicePolicyManager.getDoNotAskCredentialsOnBoot();
    }

    private boolean shouldEncryptWithCredentials(boolean defaultValue) {
        return isCredentialRequiredToDecrypt(defaultValue) && !isDoNotAskCredentialsOnBootSet();
    }
}

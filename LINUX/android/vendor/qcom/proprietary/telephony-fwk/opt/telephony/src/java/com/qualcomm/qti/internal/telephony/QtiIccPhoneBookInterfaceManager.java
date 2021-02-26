/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2006 The Android Open Source Project
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

package com.qualcomm.qti.internal.telephony;

import android.content.ContentValues;
import android.content.pm.PackageManager;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.IccPhoneBookInterfaceManager;
import com.android.internal.telephony.IccProvider;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.uicc.AdnRecord;
import com.android.internal.telephony.uicc.IccConstants;

import com.qualcomm.qti.internal.telephony.uicccontact.QtiSimPhoneBookAdnRecordCache;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

public class QtiIccPhoneBookInterfaceManager extends IccPhoneBookInterfaceManager {
    private static final String LOG_TAG = "QtiIccPhoneBookInterfaceManager";
    private static final boolean DBG = true;
    private QtiSimPhoneBookAdnRecordCache mSimPbAdnCache;

    public QtiIccPhoneBookInterfaceManager(Phone phone) {
        super(phone);

        if (isSimPhoneBookEnabled()) {
            if (mSimPbAdnCache == null) {
                mSimPbAdnCache = new QtiSimPhoneBookAdnRecordCache(
                        phone.getContext(), phone.getPhoneId(), phone.mCi);
            }
        }
    }

    private boolean isSimPhoneBookEnabled() {
        if (mPhone.getContext().getResources().getBoolean(
            com.android.internal.R.bool.config_sim_phonebook_batch_operation)) {
            return true;
        }
        return false;
    }

    @Override
    public void dispose() {
        if (mRecords != null) {
            mRecords.clear();
        }
    }

    @Override
    public List<AdnRecord> getAdnRecordsInEf(int efid) {

        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.READ_CONTACTS)
                != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.READ_CONTACTS permission");
        }

        efid = updateEfForIccType(efid);
        if (DBG) logd("getAdnRecordsInEF: efid=0x" + Integer.toHexString(efid).toUpperCase());

        synchronized(mLock) {
            checkThread();
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_LOAD_DONE, status);

            if (isSimPhoneBookEnabled() &&
                (efid == IccConstants.EF_PBR || efid == IccConstants.EF_ADN)) {
                if (mSimPbAdnCache != null) {
                    mSimPbAdnCache.requestLoadAllAdnLike(response);
                    waitForResult(status);
                } else {
                    loge("Failure while trying to load from SIM due to uninit  sim pb adncache");
                }
            } else {
                if (mAdnCache != null) {
                    mAdnCache.requestLoadAllAdnLike(
                            efid, mAdnCache.extensionEfForEf(efid), response);
                    waitForResult(status);
                } else {
                    loge("Failure while trying to load from SIM due to uninitialised adncache");
                }
            }
        }
        return mRecords;
    }

    @Override
    public boolean updateAdnRecordsWithContentValuesInEfBySearch(int efid, ContentValues values,
            String pin2) {

        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException("Requires android.permission.WRITE_CONTACTS permission");
        }

        String oldTag = values.getAsString(IccProvider.STR_TAG);
        String newTag = values.getAsString(IccProvider.STR_NEW_TAG);
        String oldPhoneNumber = values.getAsString(IccProvider.STR_NUMBER);
        String newPhoneNumber = values.getAsString(IccProvider.STR_NEW_NUMBER);
        String oldEmail = values.getAsString(IccProvider.STR_EMAILS);
        String newEmail = values.getAsString(IccProvider.STR_NEW_EMAILS);
        String oldAnr = values.getAsString(IccProvider.STR_ANRS);
        String newAnr = values.getAsString(IccProvider.STR_NEW_ANRS);
        String[] oldEmailArray = TextUtils.isEmpty(oldEmail) ? null : getStringArray(oldEmail);
        String[] newEmailArray = TextUtils.isEmpty(newEmail) ? null : getStringArray(newEmail);
        String[] oldAnrArray = TextUtils.isEmpty(oldAnr) ? null : getAnrStringArray(oldAnr);
        String[] newAnrArray = TextUtils.isEmpty(newAnr) ? null : getAnrStringArray(newAnr);
        efid = updateEfForIccType(efid);

        if (DBG)
            logd("updateAdnRecordsWithContentValuesInEfBySearch: efid=" + efid + ", values = " +
                values + ", pin2=" + pin2);
        synchronized (mLock) {
            checkThread();
            mSuccess = false;
            AtomicBoolean status = new AtomicBoolean(false);
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, status);
            AdnRecord oldAdn = new AdnRecord(oldTag, oldPhoneNumber, oldEmailArray, oldAnrArray);
            AdnRecord newAdn = new AdnRecord(newTag, newPhoneNumber, newEmailArray, newAnrArray);
            if (isSimPhoneBookEnabled() &&
                (efid == IccConstants.EF_PBR || efid == IccConstants.EF_ADN)) {
                if (mSimPbAdnCache != null) {
                    mSimPbAdnCache.updateSimPbAdnBySearch(oldAdn, newAdn, response);
                    waitForResult(status);
                } else {
                    loge("Failure while trying to update by search due to uninit sim pb adncache");
                }
            } else {
                if (mAdnCache != null) {
                    mAdnCache.updateAdnBySearch(efid, oldAdn, newAdn, pin2, response);
                    waitForResult(status);
                } else {
                    loge("Failure while trying to update by search due to uninitialised adncache");
                }
            }
        }
        return mSuccess;
    }

    @Override
    public int[] getAdnRecordsCapacity() {
        int capacity[] = new int[10];

        if (isSimPhoneBookEnabled()) {
            if (mSimPbAdnCache != null) {
                capacity[0] = mSimPbAdnCache.getAdnCount();
                capacity[1] = mSimPbAdnCache.getUsedAdnCount();
                capacity[2] = mSimPbAdnCache.getEmailCount();
                capacity[3] = mSimPbAdnCache.getUsedEmailCount();
                capacity[4] = mSimPbAdnCache.getAnrCount();
                capacity[5] = mSimPbAdnCache.getUsedAnrCount();
                capacity[6] = mSimPbAdnCache.getMaxNameLen();
                capacity[7] = mSimPbAdnCache.getMaxNumberLen();
                capacity[8] = mSimPbAdnCache.getMaxEmailLen();
                capacity[9] = mSimPbAdnCache.getMaxAnrLen();
            } else {
                loge("mAdnCache is NULL when getAdnRecordsCapacity.");
            }
        }
        if (DBG) logd("getAdnRecordsCapacity: max adn=" + capacity[0]
            + ", used adn=" + capacity[1] + ", max email=" + capacity[2]
            + ", used email=" + capacity[3] + ", max anr=" + capacity[4]
            + ", used anr=" + capacity[5] + ", max name length =" + capacity[6]
            + ", max number length =" + capacity[7]  + ", max email length =" + capacity[8]
            + ", max anr length =" + capacity[9]);

        return capacity;
    }
}

/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.uicccontact;

import android.text.TextUtils;
import android.telephony.PhoneNumberUtils;
import android.telephony.Rlog;

import java.util.Arrays;


/**
 *
 * Used to store ADNs (Abbreviated Dialing Numbers).
 *
 * {@hide}
 *
 */
public class QtiSimPhoneBookAdnRecord {
    static final String LOG_TAG = "QtiSimPhoneBookAdnRecord";

    //***** Instance Variables
    public int mRecordIndex = 0;

    public String mAlphaTag = null;
    public String mNumber = null;

    public int mEmailCount = 0;
    public String[] mEmails = null;

    public int mAdNumCount = 0;
    public String[] mAdNumbers = null;
    //***** Constants


    //***** Instance Methods

    public int getRecordIndex() {
        return mRecordIndex;
    }

    public String getAlphaTag() {
        return mAlphaTag;
    }

    public String getNumber() {
        return mNumber;
    }

    public int getNumEmails() {
        return mEmailCount;
    }

    public String[] getEmails() {
        return mEmails;
    }

    public int getNumAdNumbers() {
        return mAdNumCount;
    }

    public String[] getAdNumbers() {
        return mAdNumbers;
    }

    public static String ConvertToPhoneNumber(String input) {
        return input == null ? null : input.replace( 'e', PhoneNumberUtils.WAIT )
                                            .replace( 'T', PhoneNumberUtils.PAUSE )
                                            .replace( '?', PhoneNumberUtils.WILD );
    }

    public static String ConvertToRecordNumber(String input) {
        return input == null ? null : input.replace( PhoneNumberUtils.WAIT, 'e' )
                                            .replace( PhoneNumberUtils.PAUSE, 'T' )
                                            .replace( PhoneNumberUtils.WILD, '?' );
    }

    public boolean isEmpty() {
        return TextUtils.isEmpty(mAlphaTag)
                && TextUtils.isEmpty(mNumber)
                && mEmails == null
                && mAdNumbers == null;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append("SimPhoneBookAdnRecord{").append("index =").append(mRecordIndex);

        sb.append(", name = ").append(mAlphaTag == null ? "null" : mAlphaTag);
        sb.append(", number = ").append(mNumber == null ? "null" : mNumber);

        sb.append(", email count = ").append(mEmailCount);
        sb.append(", email = ").append(Arrays.toString(mEmails));

        sb.append(", ad number count = ").append(mAdNumCount);
        sb.append(", ad number = ").append(Arrays.toString(mAdNumbers));

        sb.append("}");
        return sb.toString();
    }
}

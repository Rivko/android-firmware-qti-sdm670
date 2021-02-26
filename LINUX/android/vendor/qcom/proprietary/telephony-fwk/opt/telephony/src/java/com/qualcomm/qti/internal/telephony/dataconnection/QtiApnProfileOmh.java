/*
 * Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.dataconnection;

import java.util.ArrayList;

import android.text.TextUtils;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.RILConstants;

public class QtiApnProfileOmh extends QtiApnSetting {

    /**
     *  OMH spec 3GPP2 C.S0023-D defines the application types in terms of a
     *  32-bit mask where each bit represents one application
     *
     *  Application bit and the correspondign app type is listed below:
     *  1 Unspecified (all applications use the same profile)
     *  2 MMS
     *  3 Browser
     *  4 BREW
     *  5 Java
     *  6 LBS
     *  7 Terminal (tethered mode for terminal access)
     *  8-32 Reserved for future use
     *
     *  From this list all the implemented app types are listed in the enum
     */
    enum ApnProfileTypeModem {
        /* Static mapping of OMH profiles to Android Service Types */
        PROFILE_TYPE_UNSPECIFIED(0x00000001, PhoneConstants.APN_TYPE_DEFAULT),
        PROFILE_TYPE_MMS(0x00000002, PhoneConstants.APN_TYPE_MMS),
        PROFILE_TYPE_LBS(0x00000020, PhoneConstants.APN_TYPE_SUPL),
        PROFILE_TYPE_TETHERED(0x00000040, PhoneConstants.APN_TYPE_DUN);

        int id;
        String serviceType;

        private ApnProfileTypeModem(int i, String serviceType) {
            this.id = i;
            this.serviceType = serviceType;
        }

        public int getid() {
            return id;
        }

        public String getDataServiceType() {
            return serviceType;
        }

        public static ApnProfileTypeModem getApnProfileTypeModem(String serviceType) {

            if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_DEFAULT)) {
                return PROFILE_TYPE_UNSPECIFIED;
            } else if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_MMS)) {
                return PROFILE_TYPE_MMS;
            } else if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_SUPL)) {
                return PROFILE_TYPE_LBS;
            } else if (TextUtils.equals(serviceType, PhoneConstants.APN_TYPE_DUN)) {
                return PROFILE_TYPE_TETHERED;
            } else {
                /* For all other service types, return unspecified */
                return PROFILE_TYPE_UNSPECIFIED;
            }
        }
    }

    private static final int DATA_PROFILE_OMH_PRIORITY_LOWEST = 255;

    private static final int DATA_PROFILE_OMH_PRIORITY_HIGHEST = 0;

    private ApnProfileTypeModem mApnProfileModem;

    private int mServiceTypeMasks = 0;

    /* Priority of this profile in the modem */
    private int mPriority = 0;

    public QtiApnProfileOmh(int profileId, int priority) {
        /**
         * Default values if the profile is being used for only selective
         * fields e.g: just profileId and Priority. use case is when rest of the
         * fields can be read and processed only by the modem
         */
        super(0, "", null, "", null, null, null, null, null,
                null, null, RILConstants.SETUP_DATA_AUTH_PAP_CHAP,
                new String[0], "IP", "IP", true, 0, 0, profileId, false, 0,
                0, 0, 0, "", "");
        this.mPriority = priority;
    }

    @Override
    public boolean canHandleType(String serviceType) {
        return ( 0 != (mServiceTypeMasks & ApnProfileTypeModem.
                getApnProfileTypeModem(serviceType).getid()));
    }

    @Override
    public ApnProfileType getApnProfileType() {
        return ApnProfileType.PROFILE_TYPE_OMH;
    }

    @Override
    public String toShortString() {
        return "ApnProfile OMH";
    }

    @Override
    public String toHash() {
        return this.toString();
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append(super.toString())
                .append(profileId)
                .append(", ").append(mPriority);
        sb.append("]");
        return sb.toString();
    }

    public void setApnProfileTypeModem(ApnProfileTypeModem modemProfile) {
        mApnProfileModem = modemProfile;
    }

    public ApnProfileTypeModem getApnProfileTypeModem() {
        return mApnProfileModem;
    }

    public void setPriority(int priority) {
        mPriority = priority;
    }

    /* priority defined from 0..255; 0 is highest */
    public boolean isPriorityHigher(int priority) {
        return isValidPriority(priority) && (mPriority < priority);
    }

    /* priority defined from 0..255; 0 is highest */
    public boolean isPriorityLower(int priority) {
        return isValidPriority(priority) && mPriority > priority;
    }

    public boolean isValidPriority() {
        return isValidPriority(mPriority);
    }

    /* NOTE: priority values are reverse, lower number = higher priority */
    private boolean isValidPriority(int priority) {
        return priority >= DATA_PROFILE_OMH_PRIORITY_HIGHEST &&
                priority <= DATA_PROFILE_OMH_PRIORITY_LOWEST;
    }

    public int getProfileId() {
        return profileId;
    }

    public int getPriority() {
        return mPriority;
    }

    public void addServiceType(ApnProfileTypeModem modemProfile) {
        mServiceTypeMasks |= modemProfile.getid();

        // Update the types
        ArrayList<String> serviceTypes = new ArrayList<String>();
        for (ApnProfileTypeModem apt : ApnProfileTypeModem.values()) {
            if (0 != (mServiceTypeMasks & apt.getid())) {
                serviceTypes.add(apt.getDataServiceType());
            }
        }
        types = serviceTypes.toArray(new String[0]);
    }
}


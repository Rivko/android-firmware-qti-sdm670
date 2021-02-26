/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.os.SystemProperties;
import android.telephony.ServiceState;
import android.telephony.PhoneNumberUtils;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;

/**
 * This class has logic to return the primary stack phone id or the phone id
 * most suitable for emergency call
 */
public class QtiEmergencyCallHelper {
    private static final String LOG_TAG = "QtiEmergencyCallHelper";
    private static QtiEmergencyCallHelper sInstance = null;
    private static boolean isDeviceInDualStandBy = true;
    private static String emerNum = null;
    private static final int INVALID = -1;
    private static final int PRIMARY_STACK_MODEMID = 0;
    private static final int PROVISIONED = 1;
    private static boolean isEmergencyCallHelperInUse = false;
    private static final String ALLOW_ECALL_ENHANCEMENT_PROPERTY =
        "persist.radio.enhance_ecall";

    /**
      * Pick the best possible phoneId for emergency call
      * Following are the conditions applicable when deciding the sub/phone for dial
      * 1. Place Ecall on a sub(i.e Phone) whichever has the emergency number in it's
      *    ecclist irrespective of service state.
      * 2. If emergency number is available in both SUB's ecclist and both subs are
      *    activated and both subs are In service/Limited service/Out of service then
      *    place call on voice pref sub.
      * 3. If both subs are not activated(i.e NO SIM/PIN/PUK lock state) then choose
      *    the sub mapped to primary stack.
      */
    public static int getPhoneIdForECall() {
        QtiSubscriptionController scontrol = QtiSubscriptionController.getInstance();

        int voicePhoneId = scontrol.getPhoneId(scontrol.getDefaultVoiceSubId());
        int phoneId = INVALID;

        TelephonyManager tm = TelephonyManager.getDefault();
        int phoneCount = tm.getPhoneCount();

        if (isDeviceInDualStandBy) {
            for (int phId = 0; phId < phoneCount; phId++) {
                Phone phone = PhoneFactory.getPhone(phId);
                int subId = phone.getSubId();
                if (!isEmergencyNumInternal(subId, emerNum)) {
                    Log.d(LOG_TAG, "For sub specific number, return pref voice phone id:"
                            + voicePhoneId);
                    return voicePhoneId;
                }
            }
            if (tm.getMultiSimConfiguration() != TelephonyManager.MultiSimVariants.DSDA) {
                for (Phone phone: PhoneFactory.getPhones()) {
                    if (phone.getState() == PhoneConstants.State.OFFHOOK) {
                        Log.d(LOG_TAG, "Call already active on phoneId: " + phone.getPhoneId());
                        // Since already call is active on phone, send ecall also on
                        // same phone to avoid delay in emergency call setup by modem
                        return phone.getPhoneId();
                    }
                }
            }
        }

        for (int phId = 0; phId < phoneCount; phId++) {
            Phone phone = PhoneFactory.getPhone(phId);
            int ss = phone.getServiceState().getState();
            if (ss == ServiceState.STATE_IN_SERVICE) {
                phoneId = phId;
                if (phoneId == voicePhoneId) break;
            }
        }
        Log.d(LOG_TAG, "Voice phoneId in service = "+ phoneId);

        if (phoneId == INVALID) {
            for (int phId = 0; phId < phoneCount; phId++) {
                Phone phone = PhoneFactory.getPhone(phId);
                int ss = phone.getServiceState().getState();
                if (phone.getServiceState().isEmergencyOnly()) {
                    phoneId = phId;
                    if (phoneId == voicePhoneId) break;
                }
            }
        }
        Log.d(LOG_TAG, "Voice phoneId in Limited service = "+ phoneId);

        if (phoneId == INVALID) {
            for (int phId = 0; phId < phoneCount; phId++) {

                QtiUiccCardProvisioner uiccProvisioner =
                        QtiUiccCardProvisioner.getInstance();

                if ((tm.getSimState(phId) == TelephonyManager.SIM_STATE_READY)
                        // phone id can be mapped to slot id
                        && (uiccProvisioner.getCurrentUiccCardProvisioningStatus(phId)
                        == PROVISIONED)) {
                    phoneId = phId;
                    if (phoneId == voicePhoneId) break;
                }
            }
            if (phoneId == INVALID) {
                phoneId = getPrimaryStackPhoneId();
            }
        }
        Log.d(LOG_TAG, "Voice phoneId in service = "+ phoneId +
                " preferred phoneId =" + voicePhoneId);

        return phoneId;
    }

    public static int getPrimaryStackPhoneId() {
        String modemUuId = null;
        Phone phone = null;
        int primayStackPhoneId = INVALID;

        for (int i = 0; i < TelephonyManager.getDefault().getPhoneCount(); i++) {

            phone = PhoneFactory.getPhone(i);
            if (phone == null) continue;

            Log.d(LOG_TAG, "Logical Modem id: " + phone.getModemUuId() + " phoneId: " + i);
            modemUuId = phone.getModemUuId();
            if ((modemUuId == null) || (modemUuId.length() <= 0) ||
                    modemUuId.isEmpty()) {
                continue;
            }
            // Select the phone id based on modemUuid
            // if modemUuid is 0 for any phone instance, primary stack is mapped
            // to it so return the phone id as the primary stack phone id.
            if (Integer.parseInt(modemUuId) == PRIMARY_STACK_MODEMID) {
                primayStackPhoneId = i;
                Log.d(LOG_TAG, "Primay Stack phone id: " + primayStackPhoneId + " selected");
                break;
            }
        }

        // never return INVALID
        if( primayStackPhoneId == INVALID){
            Log.d(LOG_TAG, "Returning default phone id");
            primayStackPhoneId = 0;
        }

        return primayStackPhoneId;
    }

    public static boolean isDeviceInSingleStandby() {
        if (!SystemProperties.getBoolean(ALLOW_ECALL_ENHANCEMENT_PROPERTY, false)) {
            Log.d(LOG_TAG, "persist.radio.enhance_ecall not enabled" );
            return false;
        }

        TelephonyManager tm = TelephonyManager.getDefault();
        int phoneCnt = tm.getPhoneCount();

        // If phone count is 1, then it is single sim device.
        // return true
        if (phoneCnt == 1)
            return true;

        for (int phoneId = 0; phoneId < phoneCnt; phoneId++) {
            QtiUiccCardProvisioner uiccProvisioner =
                        QtiUiccCardProvisioner.getInstance();

            if ((tm.getSimState(phoneId) != TelephonyManager.SIM_STATE_READY) ||
                    (uiccProvisioner.getCurrentUiccCardProvisioningStatus(phoneId)
                    != PROVISIONED)) {
                Log.d(LOG_TAG, "modem is in single standby mode" );
                isDeviceInDualStandBy = false;
                return true;
            }
        }

        Log.d(LOG_TAG, "modem is in dual standby mode" );
        return false;
    }

    public static boolean isEmergencyNumber(String number) {
        boolean isEmergencyNum = false;
        int subscriptionId = -1;
        QtiSubscriptionController scontrol = QtiSubscriptionController.getInstance();
        Phone[] phones = PhoneFactory.getPhones();
        emerNum = number;
        if (isDeviceInSingleStandby()) {
            for (Phone phone : phones) {
                subscriptionId = phone.getSubId();
                isEmergencyNum |= PhoneNumberUtils.isEmergencyNumber(subscriptionId, number);
            }
        } else {
            isEmergencyNum = PhoneNumberUtils.isEmergencyNumber(
                    scontrol.getDefaultVoiceSubId(), number);
        }

        return isEmergencyNum;
    }

    public static boolean isLocalEmergencyNumber(Context context, String number) {
        boolean isLocalEmergencyNum = false;
        int subscriptionId = -1;
        QtiSubscriptionController scontrol = QtiSubscriptionController.getInstance();
        Phone[] phones = PhoneFactory.getPhones();
        emerNum = number;

        if (isDeviceInSingleStandby()) {
            for (Phone phone : phones) {
                subscriptionId = phone.getSubId();
                isLocalEmergencyNum |=
                        PhoneNumberUtils.isLocalEmergencyNumber(context, subscriptionId, number);
            }
        } else {
            isLocalEmergencyNum = PhoneNumberUtils.isLocalEmergencyNumber(context,
                    scontrol.getDefaultVoiceSubId(), number);
        }

        return isLocalEmergencyNum;
    }

    public static boolean isPotentialEmergencyNumber(String number) {
        boolean isPotentialEmergencyNum = false;
        int subscriptionId = -1;
        QtiSubscriptionController scontrol = QtiSubscriptionController.getInstance();
        Phone[] phones = PhoneFactory.getPhones();

        if (isDeviceInSingleStandby()) {
            for (Phone phone : phones) {
                subscriptionId = phone.getSubId();
                isPotentialEmergencyNum |=
                        PhoneNumberUtils.isPotentialEmergencyNumber(subscriptionId, number);
            }
        } else {
            isPotentialEmergencyNum = PhoneNumberUtils.isPotentialEmergencyNumber(
                    scontrol.getDefaultVoiceSubId(), number);
        }

        return isPotentialEmergencyNum;
    }

    public static boolean isPotentialLocalEmergencyNumber(Context context, String number) {
        boolean isPotentialLocalEmergencyNum = false;
        int subscriptionId = -1;
        QtiSubscriptionController scontrol = QtiSubscriptionController.getInstance();
        Phone[] phones = PhoneFactory.getPhones();

        if (isDeviceInSingleStandby()) {
            for (Phone phone : phones) {
                subscriptionId = phone.getSubId();
                isPotentialLocalEmergencyNum |=
                        PhoneNumberUtils.isPotentialLocalEmergencyNumber(
                        context, subscriptionId, number);
            }
        } else {
            isPotentialLocalEmergencyNum = PhoneNumberUtils.isPotentialLocalEmergencyNumber(
                    context, scontrol.getDefaultVoiceSubId(), number);
        }

        return isPotentialLocalEmergencyNum;
    }

    public static boolean isEmergencyNumInternal(int subId, String number) {
        return PhoneNumberUtils.isEmergencyNumber(subId, number);
    }
}

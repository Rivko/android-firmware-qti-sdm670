/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved. Qualcomm Technologies Confidential and Proprietary.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (c) 2012 Code Aurora Forum. All rights reserved.
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

package org.codeaurora.ims;

import com.android.ims.ImsReasonInfo;
import com.android.internal.telephony.ATParseEx;
import com.android.internal.telephony.DriverCall;

import java.util.Objects;

/**
 * {@hide}
 */
public class DriverCallIms extends DriverCall {
    static final String LOG_TAG = "DRIVERCALL-IMS";

    /**
     * Bit-mask values to indicate the {@link DriverCallIms} properties which changed during a
     * call to {@link #update(DriverCallIms)}.
     */
    public static final int UPDATE_NONE = 0x00;
    public static final int UPDATE_STATE = 0x01;
    public static final int UPDATE_INDEX = 0x02;
    public static final int UPDATE_NUMBER = 0x04;
    public static final int UPDATE_IS_MT = 0x08;
    public static final int UPDATE_IS_MPTY = 0x10;
    public static final int UPDATE_CALL_DETAILS = 0x20;
    public static final int UPDATE_CALL_TRANSFER = 0x40;
    public static final int UPDATE_ENCRYPTION = 0x80;
    public static final int UPDATE_HISTORY_INFO = 0x100;
    public static final int UPDATE_CONF_SUPPORT = 0x200;

    public enum State {
        ACTIVE,
        HOLDING,
        DIALING, // MO call only
        ALERTING, // MO call only
        INCOMING, // MT call only
        WAITING, // MT call only
        END;
    }

    /**
     * Bit-mask values to indicate the conference support information
     * for this call instance, as received from lower layers.
     */
    /* Default disable mask */
    public static final int CONF_SUPPORT_NONE = 0x00;
    /* Enabled when there is a valid conference support information from lower layers */
    public static final int CONF_SUPPORT_INDICATED = 0x01;
    /* Enabled when VT conference support is indicated as supported */
    public static final int CONF_VIDEO_SUPPORTED = 0x02;

    public CallDetails callDetails;
    public State state;
    public ImsReasonInfo callFailCause;
    /* A mask of the possible call transfers supported by this call instance */
    public int mEctMask;
    public boolean isEncrypted;
    public String historyInfo;
    public int mCallFailReason;
    /* A mask indicating the conference support information of this call instance */
    public int mConfSupported = CONF_SUPPORT_NONE;

    // Copy Constructor
    public DriverCallIms(DriverCallIms dc) {
        callDetails = new CallDetails(dc.callDetails);
        callFailCause = new ImsReasonInfo(dc.callFailCause.mCode,
                dc.callFailCause.mExtraCode,
                dc.callFailCause.mExtraMessage);
        state = dc.state;
        index = dc.index;
        number = dc.number;
        isMT = dc.isMT;
        TOA = dc.TOA;
        isMpty = dc.isMpty;
        mEctMask = dc.mEctMask;
        als = dc.als;
        isVoice = dc.isVoice;
        isVoicePrivacy = dc.isVoicePrivacy;
        numberPresentation = dc.numberPresentation;
        name = dc.name;
        namePresentation = dc.namePresentation;
        isEncrypted = dc.isEncrypted;
        historyInfo = dc.historyInfo;
        mConfSupported = dc.mConfSupported;
    }

    public DriverCallIms() {
        callDetails = new CallDetails();
        mEctMask = 0;
    }

    public static State stateFromCall(int state) throws ATParseEx {
        switch (state) {
            case ImsQmiIF.CALL_ACTIVE:
                return State.ACTIVE;
            case ImsQmiIF.CALL_HOLDING:
                return State.HOLDING;
            case ImsQmiIF.CALL_DIALING:
                return State.DIALING;
            case ImsQmiIF.CALL_ALERTING:
                return State.ALERTING;
            case ImsQmiIF.CALL_INCOMING:
                return State.INCOMING;
            case ImsQmiIF.CALL_WAITING:
                return State.WAITING;
            case ImsQmiIF.CALL_END:
                return State.END;
            default:
                throw new ATParseEx("illegal call state " + state);
        }
    }

    /**
     * Updates members of the {@link DriverCallIms} with the update.  Bitmask describes
     * which attributes have changed.
     *
     * @param update The update.
     * @return Bit-mask describing the attributes of the {@link DriverCallIms} which changed.
     */
    public int update(DriverCallIms update) {
        if (update == null) {
            return UPDATE_NONE;
        }
        int changed = UPDATE_NONE;
        if (state != update.state) {
            state = update.state;
            changed |= UPDATE_STATE;
        }
        if (index != update.index) {
            index = update.index;
            changed |= UPDATE_INDEX;
        }
        if (number != update.number) {
            number = update.number;
            changed |= UPDATE_NUMBER;
        }
        if (isMT != update.isMT) {
            isMT = update.isMT;
            changed |= UPDATE_IS_MT;
        }
        if (isMpty != update.isMpty) {
            isMpty = update.isMpty;
            changed |= UPDATE_IS_MPTY;
        }
        if (update.callFailCause != null) {
            if (callFailCause == null) {
                callFailCause = new ImsReasonInfo(update.callFailCause.mCode,
                        update.callFailCause.mExtraCode,
                        update.callFailCause.mExtraMessage);
            } else {
                if (callFailCause.mCode != update.callFailCause.mCode) {
                    callFailCause.mCode = update.callFailCause.mCode;
                }
                if (callFailCause.mExtraCode != update.callFailCause.mExtraCode) {
                    callFailCause.mExtraCode = update.callFailCause.mExtraCode;
                }
                if (callFailCause.mExtraMessage != update.callFailCause.mExtraMessage) {
                    callFailCause.mExtraMessage = update.callFailCause.mExtraMessage;
                }
            }
        }
        if(callDetails.update(update.callDetails)) {
            changed |= UPDATE_CALL_DETAILS;
        }
        if (mEctMask != update.mEctMask) {
            mEctMask = update.mEctMask;
            changed |= UPDATE_CALL_TRANSFER;
        }
        if (isEncrypted != update.isEncrypted) {
            isEncrypted = update.isEncrypted;
            changed |= UPDATE_ENCRYPTION;
        }
        if (!Objects.equals(update.historyInfo, historyInfo)) {
            historyInfo = update.historyInfo;
            changed |= UPDATE_HISTORY_INFO;
        }
        if (mConfSupported != update.mConfSupported) {
            mConfSupported = update.mConfSupported;
            changed |= UPDATE_CONF_SUPPORT;
        }
        return changed;
    }

    public boolean isConfSupportIndicated() {
        return (mConfSupported & CONF_SUPPORT_INDICATED) == CONF_SUPPORT_INDICATED;
    }

    public boolean isVideoConfSupported() {
        return (mConfSupported & CONF_VIDEO_SUPPORTED) == CONF_VIDEO_SUPPORTED;
    }

    public String toString() {
        return "id=" + index + "," + state + "," + "toa=" + TOA + ","
                + (isMpty ? "conf" : "norm") + "," + (isMT ? "mt" : "mo") + ","
                + als + "," + (isVoice ? "voc" : "nonvoc") + ","
                + (isVoicePrivacy ? "evp" : "noevp") + ","
                /* + "number=" + number */+ ",cli=" + numberPresentation + ","
                /* + "name="+ name */+ "," + namePresentation
                + "Call Details =" + callDetails + "," + "CallFailCause Code= "
                + callFailCause.mCode + "," + "CallFailCause String= "
                + callFailCause.mExtraMessage
                + ", ECT mask: " + mEctMask
                + ", isEncrypted=" + isEncrypted
                + ", historyInfo=" + historyInfo
                + ", Conf. Support=" + mConfSupported;
    }
}

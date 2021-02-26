/*
 *Copyright (c) 2016 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.phonefeature.callbarring;

import android.content.Context;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Message;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.RILConstants;
import com.qualcomm.qti.phonefeature.R;

public class CallBarringHelper {
    private static final String  LOG_TAG = "CallBarring";
    private static final boolean DBG = true;
    private Phone mPhone;
    private int mSubId = SubscriptionManager.DEFAULT_SUBSCRIPTION_ID;

    public CallBarringHelper(Intent intent) {
        mSubId = intent.getIntExtra(PhoneConstants.SUBSCRIPTION_KEY,
                SubscriptionManager.DEFAULT_SUBSCRIPTION_ID);
        log("sub id = " + mSubId);
        if (!SubscriptionManager.isValidSubscriptionId(mSubId)) {
            mSubId = SubscriptionManager.getDefaultSubscriptionId();
            log("change sub id to = " + mSubId);
        }
        mPhone = PhoneFactory.getPhone(SubscriptionManager.getSlotId(mSubId));
    }

    private boolean isValidFacilityString(String facility) {
        if ((facility.equals(CommandsInterface.CB_FACILITY_BAOC))
                || (facility.equals(CommandsInterface.CB_FACILITY_BAOIC))
                || (facility.equals(CommandsInterface.CB_FACILITY_BAOICxH))
                || (facility.equals(CommandsInterface.CB_FACILITY_BAIC))
                || (facility.equals(CommandsInterface.CB_FACILITY_BAICr))
                || (facility.equals(CommandsInterface.CB_FACILITY_BA_ALL))
                || (facility.equals(CommandsInterface.CB_FACILITY_BA_MO))
                || (facility.equals(CommandsInterface.CB_FACILITY_BA_MT))
                || (facility.equals(CommandsInterface.CB_FACILITY_BA_SIM))
                || (facility.equals(CommandsInterface.CB_FACILITY_BA_FD))) {
            return true;
        }
        log(" Invalid facility String : " + facility);
        return false;
    }

    private boolean isCallBarringFacilitySupportedOverImsPhone(String facility) {
        if (mPhone != null) {
            CarrierConfigManager configManager = (CarrierConfigManager)mPhone.getContext().
                    getSystemService(Context.CARRIER_CONFIG_SERVICE);
            PersistableBundle pb = configManager.getConfigForSubId(getSubId());
            boolean isDisableOutcbOverIms = pb != null ?
                    pb.getBoolean("config_enable_callbarring_over_ims") : false;
            ImsPhone imsPhone = (ImsPhone)mPhone.getImsPhone();

            return (facility != null)
                    && (!isDisableOutcbOverIms
                    || (!facility.equals(CommandsInterface.CB_FACILITY_BAOC))
                    && (!facility.equals(CommandsInterface.CB_FACILITY_BAOIC))
                    && (!facility.equals(CommandsInterface.CB_FACILITY_BAOICxH)))
                    && ((imsPhone != null)
                    && (imsPhone.getServiceState().getState() == ServiceState.STATE_IN_SERVICE
                    || imsPhone.isUtEnabled()));
        }
        return false;
    }

    public void getCallBarringOption(String facility, String password, Message onComplete) {
        if (mPhone != null && isPhoneTypeGsm()) {
            if (isValidFacilityString(facility)) {
                if (isCallBarringFacilitySupportedOverImsPhone(facility)) {
                    log("Trying IMS PS get call barring");
                    ImsPhone imsPhone = (ImsPhone)mPhone.getImsPhone();
                    imsPhone.getCallBarring(facility, onComplete);
                } else {
                    mPhone.mCi.queryFacilityLock(facility, password,
                            CommandsInterface.SERVICE_CLASS_NONE, onComplete);
                }
            }
        } else {
            log(" getCallBarringOption is not supported for CDMA phone");
            replyFailureToUi(onComplete);
        }
    }

    public void setCallBarringOption(String facility, boolean lockState, String password,
            Message onComplete) {
        if (mPhone != null && isPhoneTypeGsm()) {
            if (isValidFacilityString(facility)) {
                if (isCallBarringFacilitySupportedOverImsPhone(facility)) {
                    log("Trying IMS PS set call barring");
                    ImsPhone imsPhone = (ImsPhone)mPhone.getImsPhone();
                    imsPhone.setCallBarring(facility, lockState, password, onComplete);
                } else {
                    mPhone.mCi.setFacilityLock(facility, lockState, password,
                            CommandsInterface.SERVICE_CLASS_VOICE, onComplete);
                }
            }
        } else {
            log(" setCallBarringOption is not supported for CDMA phone");
            replyFailureToUi(onComplete);
        }
    }

    public void requestChangeCbPsw(String facility, String oldPwd, String newPwd, Message result) {
        if (mPhone != null && isPhoneTypeGsm()) {
            mPhone.mCi.changeBarringPassword(facility, oldPwd, newPwd, result);
        } else {
            log(" requestChangeCbPsw is not supported for CDMA phone");
            replyFailureToUi(result);
        }
    }

    private void replyFailureToUi(Message onComplete) {
        if (onComplete != null) {
            Exception e = CommandException.fromRilErrno(RILConstants.GENERIC_FAILURE);
            AsyncResult.forMessage(onComplete, null, e);
            onComplete.sendToTarget();
        }
    }

    public boolean isUtEnabled() {
        return mPhone != null ? mPhone.isUtEnabled() : false;
    }

    public int getSubId() {
        return mSubId;
    }

    public boolean isDataRoaming() {
        return mPhone != null ? mPhone.getServiceState().getDataRoaming() : false;
    }

    public boolean isDataRoamingEnabled() {
        return mPhone != null ? mPhone.getDataRoamingEnabled() : false;
    }

    private boolean isPhoneTypeGsm() {
        return mPhone != null ? mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM : false;
    }

    private static void log(String msg) {
        if (DBG) Log.d(LOG_TAG, msg);
    }
}

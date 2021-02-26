/*
 * Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.dataconnection;

import android.database.Cursor;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.SystemProperties;
import android.provider.Telephony;
import android.text.TextUtils;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;

import com.android.internal.telephony.cdma.CdmaSubscriptionSourceManager;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.RILConstants;

import com.qualcomm.qti.internal.telephony.QtiRilInterface;
import com.qualcomm.qti.internal.telephony.dataconnection.QtiApnProfileOmh;
import com.qualcomm.qti.internal.telephony.dataconnection.QtiApnProfileOmh.ApnProfileTypeModem;

/**
 * {@hide}
 */
public final class QtiCdmaApnProfileTracker extends Handler {
    protected final String LOG_TAG = "QtiCdmaApnProfileTracker";

    private Phone mPhone;
    private CdmaSubscriptionSourceManager mCdmaSsm;

    /**
     * mApnProfilesList holds all the Apn profiles for cdma
     */
    private ArrayList<QtiApnSetting> mApnProfilesList = new ArrayList<QtiApnSetting>();

    private static final String[] mSupportedApnTypes = {
            PhoneConstants.APN_TYPE_DEFAULT,
            PhoneConstants.APN_TYPE_MMS,
            PhoneConstants.APN_TYPE_SUPL,
            PhoneConstants.APN_TYPE_DUN,
            PhoneConstants.APN_TYPE_HIPRI,
            PhoneConstants.APN_TYPE_FOTA,
            PhoneConstants.APN_TYPE_IMS,
            PhoneConstants.APN_TYPE_CBS };

    private static final String[] mDefaultApnTypes = {
            PhoneConstants.APN_TYPE_DEFAULT,
            PhoneConstants.APN_TYPE_MMS,
            PhoneConstants.APN_TYPE_SUPL,
            PhoneConstants.APN_TYPE_HIPRI,
            PhoneConstants.APN_TYPE_FOTA,
            PhoneConstants.APN_TYPE_IMS,
            PhoneConstants.APN_TYPE_CBS };

    // if we have no active ApnProfile this is null
    protected QtiApnSetting mActiveApn;

    /*
     * Context for read profiles for OMH.
     */
    private int mOmhReadProfileContext = 0;

    /*
     * Count to track if all read profiles for OMH are completed or not.
     */
    private int mOmhReadProfileCount = 0;

    // Temp. ApnProfile list from the modem.
    ArrayList<QtiApnSetting> mTempOmhApnProfilesList = new ArrayList<QtiApnSetting>();

    // Map of the service type to its priority
    HashMap<String, Integer> mOmhServicePriorityMap;

    /* Registrant list for objects interested in modem profile related events */
    private RegistrantList mModemApnProfileRegistrants = new RegistrantList();

    private static final int EVENT_READ_MODEM_PROFILES = 0;
    private static final int EVENT_GET_DATA_CALL_PROFILE_DONE = 1;
    private static final int EVENT_LOAD_PROFILES = 2;

    /* Constructor */

    QtiCdmaApnProfileTracker(Phone phone) {
        mPhone = phone;
        mCdmaSsm = CdmaSubscriptionSourceManager.getInstance (phone.getContext(), phone.mCi, this,
                EVENT_LOAD_PROFILES, null);

        mOmhServicePriorityMap = new HashMap<String, Integer>();
    }

    /**
     * Load the CDMA profiles
     */
    void loadProfiles() {
        log("loadProfiles...");
        mApnProfilesList.clear();

        readApnProfilesFromModem();
    }


    /**
     * @param types comma delimited list of data service types
     * @return array of data service types
     */
    private String[] parseTypes(String types) {
        String[] result;
        // If unset, set to DEFAULT.
        if (types == null || types.equals("")) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types.split(",");
        }
        return result;
    }

    protected void finalize() {
        Log.d(LOG_TAG, "QtiCdmaApnProfileTracker finalized");
    }

    public void registerForModemProfileReady(Handler h, int what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mModemApnProfileRegistrants.add(r);
    }

    public void unregisterForModemProfileReady(Handler h) {
        mModemApnProfileRegistrants.remove(h);
    }

    public void handleMessage (Message msg) {
        switch (msg.what) {
            case EVENT_LOAD_PROFILES:
                loadProfiles();
                break;
            case EVENT_READ_MODEM_PROFILES:
                onReadApnProfilesFromModem();
                break;

            case EVENT_GET_DATA_CALL_PROFILE_DONE:
                onGetDataCallProfileDone((AsyncResult) msg.obj, (int)msg.arg1);
                break;

            default:
                break;
        }
    }

    /*
     * Trigger modem read for data profiles
     */
    private void readApnProfilesFromModem() {
        sendMessage(obtainMessage(EVENT_READ_MODEM_PROFILES));
    }

    /*
     * Reads all the data profiles from the modem
     */
    private void onReadApnProfilesFromModem() {
        log("OMH: onReadApnProfilesFromModem()");
        mOmhReadProfileContext++;

        mOmhReadProfileCount = 0; // Reset the count and list(s)
        /* Clear out the modem profiles lists (main and temp) which were read/saved */
        mTempOmhApnProfilesList.clear();
        mOmhServicePriorityMap.clear();

        // For all the service types known in modem, read the data profies
        for (ApnProfileTypeModem p : ApnProfileTypeModem.values()) {
            log("OMH: Reading profiles for:" + p.getid());
            mOmhReadProfileCount++;


            QtiRilInterface qtiRilInterface = QtiRilInterface.getInstance(mPhone.getContext());
            if (!qtiRilInterface.isServiceReady()) {
                log("Oem hook service is not ready yet " + mPhone.getPhoneId());
                return;
            }

            Message callbackMsg = obtainMessage(EVENT_GET_DATA_CALL_PROFILE_DONE, //what
                    mOmhReadProfileContext, //arg1
                    0 , //arg2  -- ignore
                    p);

            qtiRilInterface.getOmhCallProfile(p.getid(),
                    callbackMsg,
                    mPhone.getPhoneId());
        }

    }

    /*
     * Process the response for the RIL request GET_DATA_CALL_PROFILE.
     * Save the profile details received.
     */
    private void onGetDataCallProfileDone(AsyncResult ar, int context) {
        if (context != mOmhReadProfileContext) {
            //we have other onReadOmhDataprofiles() on the way.
            return;
        }

        if (ar.exception != null) {
            log("OMH: Exception in onGetDataCallProfileDone:" + ar.exception);
            mOmhReadProfileCount--;
            return;
        }

        // ApnProfile list from the modem for a given SERVICE_TYPE. These may
        // be from RUIM in case of OMH
        ArrayList<QtiApnSetting> dataProfileListModem = (ArrayList<QtiApnSetting>)ar.result;

        ApnProfileTypeModem modemProfile = (ApnProfileTypeModem)ar.userObj;

        mOmhReadProfileCount--;

        if (dataProfileListModem != null && dataProfileListModem.size() > 0) {
            String serviceType;

            /* For the modem service type, get the android DataServiceType */
            serviceType = modemProfile.getDataServiceType();

            log("OMH: # profiles returned from modem:" + dataProfileListModem.size()
                    + " for " + serviceType);

            mOmhServicePriorityMap.put(serviceType,
                    omhListGetArbitratedPriority(dataProfileListModem, serviceType));

            for (QtiApnSetting apn : dataProfileListModem) {

                /* Store the modem profile type in the data profile */
                ((QtiApnProfileOmh)apn).setApnProfileTypeModem(modemProfile);

                /* Look through mTempOmhApnProfilesList for existing profile id's
                 * before adding it. This implies that the (similar) profile with same
                 * priority already exists.
                 */
                QtiApnProfileOmh omhDuplicateDp = getDuplicateProfile(apn);
                if (null == omhDuplicateDp) {
                    mTempOmhApnProfilesList.add(apn);
                    ((QtiApnProfileOmh)apn).addServiceType(ApnProfileTypeModem.
                            getApnProfileTypeModem(serviceType));
                } else {
                    /*  To share the already established data connection
                     * (say between SUPL and DUN) in cases such as below:
                     *  Ex:- SUPL+DUN [profile id 201, priority 1]
                     *  'apn' instance is found at this point. Add the non-provisioned
                     *   service type to this 'apn' instance
                     */
                    log("OMH: Duplicate Profile " + omhDuplicateDp);
                    omhDuplicateDp.addServiceType(ApnProfileTypeModem.
                            getApnProfileTypeModem(serviceType));
                }
            }
        }

        //(Re)Load APN List
        if (mOmhReadProfileCount == 0) {
            log("OMH: Modem omh profile read complete.");
            addServiceTypeToUnSpecified();
            mApnProfilesList.addAll(mTempOmhApnProfilesList);
            if (mApnProfilesList.size() > 0) {
                log("OMH: Found some OMH profiles.");
                mModemApnProfileRegistrants.notifyRegistrants();
            }
        }

        return;
    }

    /*
     * returns the object 'OMH dataProfile' if a match with the same profile id
     * exists in the enumerated list of OMH profile list
     */
    private QtiApnProfileOmh getDuplicateProfile(QtiApnSetting apn) {
        for (QtiApnSetting dataProfile : mTempOmhApnProfilesList) {
            if (((QtiApnProfileOmh)apn).getProfileId() ==
                ((QtiApnProfileOmh)dataProfile).getProfileId()){
                return (QtiApnProfileOmh)dataProfile;
            }
        }
        return null;
    }

    public QtiApnSetting getApnProfile(String serviceType) {
        log("getApnProfile: serviceType="+serviceType);
        QtiApnSetting profile = null;

        // Go through all the profiles to find one
        for (QtiApnSetting apn: mApnProfilesList) {
            if (apn.canHandleType(serviceType)) {
                profile = apn;
                break;
            }
        }

        log("getApnProfile: return profile=" + profile);
        return profile;
    }

    public ArrayList<QtiApnSetting> getOmhApnProfilesList() {
        log("getOmhApnProfilesList:" + mApnProfilesList);
        return mApnProfilesList;
    }

    /* For all the OMH service types not present in the card, add them to the
     * UNSPECIFIED/DEFAULT data profile.
     */
    private void addServiceTypeToUnSpecified() {
        for (String apntype : mSupportedApnTypes) {
            if(!mOmhServicePriorityMap.containsKey(apntype)) {

                // ServiceType :apntype is not provisioned in the card,
                // Look through the profiles read from the card to locate
                // the UNSPECIFIED profile and add the service type to it.
                for (QtiApnSetting apn : mTempOmhApnProfilesList) {
                    if (((QtiApnProfileOmh)apn).getApnProfileTypeModem() ==
                                ApnProfileTypeModem.PROFILE_TYPE_UNSPECIFIED) {
                        ((QtiApnProfileOmh)apn).addServiceType(ApnProfileTypeModem.
                                getApnProfileTypeModem(apntype));
                        log("OMH: Service Type added to UNSPECIFIED is : " +
                                ApnProfileTypeModem.getApnProfileTypeModem(apntype));
                        break;
                    }
                }
            }
        }
    }

    /*
     * Retrieves the highest priority for all APP types except SUPL. Note that
     * for SUPL, retrieve the least priority among its profiles.
     */
    private int omhListGetArbitratedPriority(
            ArrayList<QtiApnSetting> dataProfileListModem,
            String serviceType) {
        QtiApnSetting profile = null;

        for (QtiApnSetting apn : dataProfileListModem) {
            if (!((QtiApnProfileOmh) apn).isValidPriority()) {
                log("[OMH] Invalid priority... skipping");
                continue;
            }

            if (profile == null) {
                profile = apn; // first hit
            } else {
                if (serviceType == PhoneConstants.APN_TYPE_SUPL) {
                    // Choose the profile with lower priority
                    profile = ((QtiApnProfileOmh) apn).isPriorityLower(((QtiApnProfileOmh) profile)
                            .getPriority()) ? apn : profile;
                } else {
                    // Choose the profile with higher priority
                    profile = ((QtiApnProfileOmh) apn).isPriorityHigher(((QtiApnProfileOmh) profile)
                            .getPriority()) ? apn : profile;
                }
            }
        }
        return ((QtiApnProfileOmh) profile).getPriority();
    }

    public void clearActiveApnProfile() {
        mActiveApn = null;
    }

    public boolean isApnTypeActive(String type) {
        return mActiveApn != null && mActiveApn.canHandleType(type);
    }

    protected boolean isApnTypeAvailable(String type) {
        for (String s : mSupportedApnTypes) {
            if (TextUtils.equals(type, s)) {
                return true;
            }
        }
        return false;
    }

    protected void log(String s) {
        Log.d(LOG_TAG, s);
    }

    protected void loge(String s) {
        Log.e(LOG_TAG, s);
    }
}


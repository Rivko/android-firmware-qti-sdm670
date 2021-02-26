/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.provider.Settings.SettingNotFoundException;
import android.telephony.RadioAccessFamily;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.ProxyController;
import com.android.internal.telephony.RadioCapability;
import com.android.internal.telephony.RILConstants;
import com.android.internal.telephony.TelephonyIntents;

import java.util.HashMap;

/*
 * QtiRadioCapabilityController: Class used to determine stack binding policy based on
 * Network mode selected by the user.
 *
 * Before sending NwMode to RIL, checks if the NwMode is supported on the Stack and
 * if possible initiate a cross mapping sequence.
 * Includes:
 *    Current preferred NwModes
 *    Logic for determining NwMode supported on particular Stack
 *    Logic for determining stack binding policy based on Network mode
 *    selected by user.
 *    Send request for Stack binding update based on above policy.
 */
public class QtiRadioCapabilityController extends Handler {
    private static final String LOG_TAG = "QtiRadioCapabilityController";
    private static final boolean VDBG = false;
    private static final boolean DBG = true;

    //***** Events
    private static final int EVENT_RADIO_NOT_AVAILABLE = 1;
    private static final int EVENT_RADIO_CAPS_AVAILABLE = 2;
    private static final int EVENT_UPDATE_BINDING_DONE = 3;

    //*****Constants
    private static final int SUCCESS = 1;
    private static final int FAILURE = 0;

    //***** Class Variables
    private static QtiRadioCapabilityController sInstance;
    private static final int mNumPhones = TelephonyManager.getDefault().getPhoneCount();
    private static Object sSetNwModeLock = new Object();
    static final String ALLOW_FLEX_MAPPING_ON_INACTIVE_SUB_PROPERTY =
            "persist.radio.flex_map_inactive";

    private Context mContext;
    private boolean mIsSetPrefNwModeInProgress = false;
    // This used to store the Message objects which received from clients
    // as part of setPreferredNetworkMode() request.
    // This info is stored per phoneId basis.
    private HashMap<Integer, Message> mStoredResponse = new HashMap<Integer, Message>();
    private QtiSubscriptionController mQtiSubscriptionController = null;
    private boolean bothPhonesMappedToSameStack = false;

    private CommandsInterface[] mCi;
    private Phone[] mPhone;
    private int[] mPreferredStackId = new int[mNumPhones];
    private int[] mCurrentStackId = new int[mNumPhones];
    private int[] mPrefNwMode = new int[mNumPhones];
    // Radio access family as per StackId
    private int[] mRadioAccessFamily = new int[mNumPhones];
    private boolean mNeedSetDds = false;


    public static QtiRadioCapabilityController make(Context context,
            Phone[] phone, CommandsInterface[] ci) {
        Rlog.d(LOG_TAG, "getInstance");
        if (sInstance == null) {
            sInstance = new QtiRadioCapabilityController(context, phone, ci);
        } else {
            Log.wtf(LOG_TAG, "QtiRadioCapabilityController.make() should be called once");
        }
        return sInstance;
    }

    public static QtiRadioCapabilityController getInstance() {
        if (sInstance == null) {
            Log.e(LOG_TAG, "QtiRadioCapabilityController.getInstance called before make");
        }
        return sInstance;
    }

    private QtiRadioCapabilityController(Context context,
            Phone[] phone, CommandsInterface[] ci) {
        mCi = ci;
        mContext = context;
        mPhone = phone;

        mQtiSubscriptionController = QtiSubscriptionController.getInstance();

        for (int i = 0; i < mCi.length; i++) {
            mCi[i].registerForNotAvailable(this, EVENT_RADIO_NOT_AVAILABLE, new Integer(i));
            mStoredResponse.put(i, null);
        }

        IntentFilter filter = new IntentFilter(TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE);
        filter.addAction(TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_FAILED);
        context.registerReceiver(mReceiver, filter);

        logd("Constructor - Exit");
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Rlog.d(LOG_TAG, "mReceiver: action " + action);
            if (action.equals(TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE)) {
                sendMessage(obtainMessage(EVENT_UPDATE_BINDING_DONE, SUCCESS, -1));
            } else if (action.equals(TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_FAILED)) {
                sendMessage(obtainMessage(EVENT_UPDATE_BINDING_DONE, FAILURE, -1));
            }
        }
    };

    @Override
    public void handleMessage(Message msg) {
        AsyncResult ar;
        Integer phoneId;

        switch(msg.what) {
            case EVENT_RADIO_CAPS_AVAILABLE:
                handleRadioCapsAvailable();
                break;

            case EVENT_UPDATE_BINDING_DONE:
                logv(" EVENT_UPDATE_BINDING_DONE ");
                handleUpdateBindingDone(msg.arg1);
                break;

            case EVENT_RADIO_NOT_AVAILABLE:
                if (msg.obj != null) {
                    ar = (AsyncResult)msg.obj;
                    if (ar.userObj != null) {
                        phoneId = (Integer)ar.userObj;
                        logd("EVENT_RADIO_NOT_AVAILABLE, phoneId = " + phoneId);
                        processRadioNotAvailable(ar, phoneId);
                    } else {
                        loge("Invalid user obj");
                    }
                } else {
                    loge("Invalid msg obj");
                }
                break;

            default:
                break;
        }
    }

    private boolean areAllModemCapInfoReceived() {
        for (int i = 0; i < mNumPhones; i++) {
            if (mPhone[i].getRadioCapability() == null) return false;
        }
        return true;
    }

    // flex map on inactive sub is enabled when system property
    // persist.radio.flex_map_inactive set to true.
    private boolean isFlexMappingAllowedOnInactiveSub() {
        return SystemProperties.getBoolean(ALLOW_FLEX_MAPPING_ON_INACTIVE_SUB_PROPERTY, false);
    }

    private void handleUpdateBindingDone(int result) {
        if (bothPhonesMappedToSameStack && (result == SUCCESS)) {
            bothPhonesMappedToSameStack = false;

            String flexMapSupportType =
                    SystemProperties.get("persist.radio.flexmap_type", "nw_mode");

            if (flexMapSupportType.equals("dds")) {
                logd("handleUpdateBindingDone: set dds ");
                // If the flex map type is "dds" and there was a normal mapping initiated,
                // call setDds request to initiate flex map if required.
                mQtiSubscriptionController.setDefaultDataSubId(
                        mQtiSubscriptionController.getDefaultDataSubId());
            } else {
                for (int i = 0; i < mNumPhones; i++) {
                    Message resp = mStoredResponse.get(i);
                    if (resp != null) {
                        logd("handleUpdateBindingDone: try initiate pending flex map req ");
                        if (updateStackBindingIfRequired(true)) {
                            return;
                        }
                    }
                }
            }
        }

        if (result == SUCCESS) {
            // Flex map completed succesfully
            // 1. Update user requested N/W mode to persistance DB
            // 2. Sync the RadioCapability local cache with latest values.
            // 3. Query IMEI.
            updateNewNwModeToDB();
            for (int i = 0; i < mNumPhones; i++) {
                ((QtiGsmCdmaPhone)mPhone[i]).fetchIMEI();
            }
        }
        // Re-send sub preferences on all phone objects
        for (int i = 0; i < mNumPhones; i++) {
            sendSubscriptionSettings(i);
        }

        setDdsIfRequired(true);

        // clean N/W mode global flag
        setNWModeInProgressFlag(false);

        // Broadcast intent to clients to nofity radio caps updated.
        notifyRadioCapsUpdated(result == SUCCESS);

        // Send setNetworkModeResponse to clients
        for (int i = 0; i < mNumPhones; i++) {
            int errorCode = RILConstants.SUCCESS;
            Message resp = mStoredResponse.get(i);
            if (resp != null) {
                if (result != SUCCESS) {
                    errorCode = RILConstants.GENERIC_FAILURE;
                }
                sendResponseToTarget(resp, errorCode);
                mStoredResponse.put(i, null);
            }
        }
    }

    private void handleRadioCapsAvailable() {
        logd("handleRadioCapsAvailable... " );
        if (updateStackBindingIfRequired(false)) {
            setNWModeInProgressFlag(true);
        } else {
            notifyRadioCapsUpdated(false);
        }
    }

    private void processRadioNotAvailable(AsyncResult ar, int phoneId) {
        logd("processRadioNotAvailable on phoneId = " + phoneId);
        // After modem reset we need to set DDS
        mNeedSetDds = true;
    }

    private void syncCurrentStackInfo() {
        //Get Current phoneId to stack mapping.
        for (int i = 0; i < mNumPhones; i++) {
            mCurrentStackId[i] = Integer.valueOf(mPhone[i].getModemUuId());
            mRadioAccessFamily[mCurrentStackId[i]] = mPhone[i].getRadioAccessFamily();

            //reset Preferred to current.
            mPreferredStackId[i] = (mCurrentStackId[i] >= 0) ? mCurrentStackId[i] : i;
            logv("syncCurrentStackInfo, current stackId[" + i + "] = " + mCurrentStackId[i]
                    + " raf = " + mRadioAccessFamily[mCurrentStackId[i]]);
        }
    }

    /*
    * updateStackBindingIfRequired: Method used to update stack binding if required.
    * @return: boolean i.e. Request acepted or not.
    *   true, if update stack binding request is sent and it returns SUCCESS
    *   false, if update stack binding request is not sent or
    *       if update stack binding request is sent and it returns FAILURE.
    */
    synchronized private boolean updateStackBindingIfRequired(boolean isNwModeRequest) {
        boolean isUpdateStackBindingRequired = false;
        boolean response = false;
        boolean callInProgress = isAnyCallsInProgress();
        boolean isInEcmState = isAnyPhoneInEcmState();
        String flexMapSupportType =
                SystemProperties.get("persist.radio.flexmap_type", "nw_mode");

        logd("updateStackBindingIfRequired");
        if ((mNumPhones == 1) || !flexMapSupportType.equals("nw_mode")) {
            loge("No need to update Stack Bindingm prop = "
                    + flexMapSupportType + " ph count = " + mNumPhones);
            return response;
        }

        if (callInProgress || isInEcmState || !areAllModemCapInfoReceived()) {
            loge("Error: Call state = " + callInProgress + ", ecm state = " + isInEcmState);
            return response;
        }

        if (!isNwModeRequest) {
            for (int i = 0; i < mNumPhones; i++) {
                int[] subId = mQtiSubscriptionController.getSubId(i);
                // In any slot if SIM card present and subId not available yet,
                // do not initiate flex map. Once subId available
                // setPreferedNetworkMode() call would re-initiate flex map.
                if (!isCardAbsent(i) && ((subId == null) || (subId.length <= 0) ||
                            !mQtiSubscriptionController.isActiveSubId(subId[0]))) {
                    loge("Error: subId not generated yet " + i);
                    return response;
                }
            }
        }

        if (isBothPhonesMappedToSameStack() || is7Plus7DeviceFlexMapped()) {
            return initNormalMappingRequest();
        }

        updatePreferredStackIds(isNwModeRequest);
        for (int i = 0; i < mNumPhones; i++) {
            logv(" pref stack[" + i + "] = " + mPreferredStackId[i] +
                    " current stack[" + i + "] = " + mCurrentStackId[i]);
            if (mPreferredStackId[i] != mCurrentStackId[i]) {
                //if preferred stackId is different from current, bindupdate is required.
                isUpdateStackBindingRequired = true;
                break;
            }
        }

        logd(" updateStackBindingIfRequired, required =  " + isUpdateStackBindingRequired);
        if (isUpdateStackBindingRequired) {
            RadioAccessFamily[] rafs = new RadioAccessFamily[mNumPhones];
            for (int i = 0; i < mNumPhones; i++) {
                rafs[i] = new RadioAccessFamily(i, mRadioAccessFamily[mPreferredStackId[i]]);
            }
            response = ProxyController.getInstance().setRadioCapability(rafs);
        }
        return response;
    }

    /*
    * updatePreferredStackIds: Method used to get isCrossBindingRequired and set the Preferred
    * StackIds as per Network Mode for phoneId.
    * @return: boolean true/false based on crossMapping is required.
    *
    * Description: used to determine the preferred stackIds based on Network Mode selected
    * and return if CrossBindingRequired.
    * Logic:
    * 1. For each phoneId, check if current stack supports the present Network Mode.
    * 2. If supported, continue to next phoneId. If stacks on all phoneId support their respective
    *     Network Mode then crossmapping is not required so preferredStackIds will be same as
    *     CurrentStackIds
    * 3. If stack of any of the phoneId is not supporting their Network Mode, then check if stack of
    *     other phoneIds supports present NetworkMode.
    *    a. If none of the stacks support the present Network Mode, cross mapping is not required
    *       and preferredStackIds will be same as CurrentStackIds.
    *    b. If some other stack supports, check if network mode on other phoneId is supported by
    *       stack after cross mapping.
    *           i. if supported cross mapping is required and set preferredStackIds accordingly.
    *           ii. if not supported cross mapping is not required and preferredStackIds will be
    *              same as CurrentStackIds.
    */
    private void updatePreferredStackIds(boolean isNwModeRequest) {
        if (!areAllModemCapInfoReceived()) {
            loge("updatePreferredStackIds: Modem Caps not Available, request =" + isNwModeRequest);
            return;
        }

        if (!isNwModeRequest) {
            //Get current prefNwMode and Stack info before updating pref Stack
            syncPreferredNwModeFromDB();
        }
        syncCurrentStackInfo();

        for (int curPhoneId = 0; curPhoneId < mNumPhones; curPhoneId++) {
            //Continue if current stack supports Network Mode.
            if (isNwModeSupportedOnStack(mPrefNwMode[curPhoneId], mCurrentStackId[curPhoneId])) {
                logd("updatePreferredStackIds: current stack[" + mCurrentStackId[curPhoneId] +
                        "]supports NwMode[" + mPrefNwMode[curPhoneId] + "] on phoneId["
                        + curPhoneId + "]");
                continue;
            }
            logd("updatePreferredStackIds:  current stack[" + mCurrentStackId[curPhoneId] +
                    "],  NwMode[" + mPrefNwMode[curPhoneId] + "] on phoneId["
                    + curPhoneId + "]");

            // Current stack id do not support present Network Mode.
            // Check if any other stack supports the present Network Mode
            for (int otherPhoneId = 0; otherPhoneId < mNumPhones; otherPhoneId++) {
                //continue for same stack, check only on other stacks
                if (otherPhoneId == curPhoneId) continue;
                logd("updatePreferredStackIds:  other stack[" + mCurrentStackId[otherPhoneId] +
                        "],  NwMode[" + mPrefNwMode[curPhoneId] + "] on phoneId["
                        + curPhoneId + "]");

                if (isNwModeSupportedOnStack(mPrefNwMode[curPhoneId],
                        mCurrentStackId[otherPhoneId])) {
                    //Some other stack supports present Network Mode, Check if Network Mode
                    //on other PhoneId is supported on current Stack after Cross binding or
                    //if there is no sim card in the other slot and sim card present on
                    //current slot then allow cross binding.
                    if ((isCardAbsent(otherPhoneId) && !isCardAbsent(curPhoneId)) ||
                            isNwModeSupportedOnStack(mPrefNwMode[otherPhoneId],
                            mCurrentStackId[curPhoneId])) {
                        logd("updatePreferredStackIds: Cross Binding is possible between phoneId["
                                + curPhoneId + "] and phoneId[" + otherPhoneId + "]");
                        //set preferred stackid of curPhoneId as current stack id of otherPhoneId
                        //and preferred stackid of otherPhoneId as current stack id of curPhoneId
                        mPreferredStackId[curPhoneId] = mCurrentStackId[otherPhoneId];
                        mPreferredStackId[otherPhoneId] = mCurrentStackId[curPhoneId];
                    }
                }
            }
        }
    }

    private boolean isNwModeSupportedOnStack(int nwMode, int stackId) {
        int[] numRafSupported = new int[mNumPhones];
        int maxNumRafSupported = 0;
        boolean isSupported = false;

        //Get num of RAFs supported for this NwMode on all Stacks
        for (int i = 0; i < mNumPhones; i++) {
            numRafSupported[i] = getNumOfRafSupportedForNwMode(nwMode, mRadioAccessFamily[i]);
            if (maxNumRafSupported < numRafSupported[i]) maxNumRafSupported = numRafSupported[i];
        }

        //if current stackId supports Max RATs of all other stacks, then return true.
        if (numRafSupported[stackId] == maxNumRafSupported) isSupported = true;

        logd("nwMode:" + nwMode + ", on stack:" + stackId + " is " +
                (isSupported ? "Supported" : "Not Supported"));

        return isSupported;
    }

    private void syncPreferredNwModeFromDB() {
        for (int i = 0; i < mNumPhones; i++) {
            mPrefNwMode[i] = getNetworkModeFromDB(i);
        }
    }

    private int getNetworkModeFromDB(int phoneId) {
        int networkMode;
        int[] subId = mQtiSubscriptionController.getSubId(phoneId);

        // If subId is valid pick sub based N/W mode else pick slot based
        // network mode for performing flex map.
        try {
            networkMode = TelephonyManager.getIntAtIndex(mContext.getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE, phoneId);
        } catch (SettingNotFoundException snfe) {
            loge("getNwMode: " + phoneId + " ,Could not find PREFERRED_NETWORK_MODE!!!");
            networkMode = Phone.PREFERRED_NT_MODE;
        }

        if ((subId != null) && (subId.length > 0) &&
                 mQtiSubscriptionController.isActiveSubId(subId[0])) {
            networkMode = android.provider.Settings.Global.getInt(mContext.getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId[0],
                    networkMode);
            logi(" get sub based N/W mode, val[" + phoneId + "] = " + networkMode);
        } else {
            logi(" get slotId based N/W mode, val[" + phoneId + "] = " + networkMode);
        }
        return networkMode;
    }

    // This would be called after successful completion of flex map, it updates the DB
    // with latest N/W mode which received from user and caused trigger for flex map.
    private void updateNewNwModeToDB() {
        int nwModeFromDB = -1;
        for (int i = 0; i < mNumPhones; i++) {
            nwModeFromDB = getNetworkModeFromDB(i);

            // If N/W mode in DB is not equals to the local cache, update
            // DB with N/W mode from local cache, which received from user.
            if (mPrefNwMode[i] != nwModeFromDB) {
                int[] subId = mQtiSubscriptionController.getSubId(i);

                logi("updateNewNwModeToDB: subId[" + i + "] = " + subId + " new Nw mode = "
                        + mPrefNwMode[i] + " old n/w mode = " + nwModeFromDB);
                if (mQtiSubscriptionController.isActiveSubId(subId[0])) {
                    android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                            android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId[0],
                            mPrefNwMode[i]);
                }
                TelephonyManager.putIntAtIndex(mContext.getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE, i,
                        mPrefNwMode[i]);
            }
        }
    }

    // This would be called from telephony framework and applications to set network mode.
    // Internally this method initiates flex map if required else sends N/W mode to RIL.
    synchronized public void setPreferredNetworkType(int phoneId,
            int networkType, Message response) {
        //if binding is in progress return failure for this request
        if (isSetNWModeInProgress() || isUiccProvisionInProgress()) {
            loge("setPreferredNetworkType: In Progress, nwmode[" + phoneId + "] = " + networkType);
            sendResponseToTarget(response, RILConstants.GENERIC_FAILURE);
            return;
        }
        int[] subId = mQtiSubscriptionController.getSubId(phoneId);
        boolean isSubActive = false;

        if ((subId != null) && (subId.length > 0)) {
            isSubActive = mQtiSubscriptionController.isActiveSubId(subId[0]);
        }

        logd("setPreferredNetworkType: nwMode[" +  phoneId + "] = "
                + networkType + " isActive = " + isSubActive);

        setNWModeInProgressFlag(true);
        syncPreferredNwModeFromDB();
        // cache received N/W mode locally and update in DB once after
        // successful completion of flex map.
        mPrefNwMode[phoneId] = networkType;

        // 1. If SIM card is not present related to input phoneId, do not initiate cross
        //    binding instead just send N/W mode request to RIL unless flex map is enabled
        //    explicitly in same case.
        // 2. If CrossBinding request is not accepted send N/W mode request directly to RIL.
        // 3. If cross binding is possible, i.e if updateStackBindingIfRequired() returns true
        //    N/W mode request will be sent to RIL after completion of modem re-binding.
        if ((isFlexMappingAllowedOnInactiveSub() || isSubActive)
                && updateStackBindingIfRequired(true)) {
            logv("setPreferredNetworkType: store msg, nwMode[" + phoneId + "] = " + networkType);
            mStoredResponse.put(phoneId, response);
        } else {
            logv("setPreferredNetworkType: sending nwMode[" + phoneId + "] = " + networkType);
            mCi[phoneId].setPreferredNetworkType(networkType, response);
            setNWModeInProgressFlag(false);
        }
    }

    private int getNumOfRafSupportedForNwMode(int nwMode, int radioAccessFamily) {
        int supportedRafMaskForNwMode = 0;
        if (radioAccessFamily == RadioAccessFamily.RAF_UNKNOWN) {
            loge(" Modem Capabilites are null. Return!!, N/W mode " + nwMode);
            return supportedRafMaskForNwMode;
        }

        int  nwModeRaf = RadioAccessFamily.getRafFromNetworkType(nwMode);

        supportedRafMaskForNwMode = radioAccessFamily & nwModeRaf;

        logv("getNumOfRATsSupportedForNwMode: nwMode[" + nwMode + " nwModeRaf = " + nwModeRaf +
                "] raf = " + radioAccessFamily +
                " supportedRafMaskForNwMode:" + supportedRafMaskForNwMode);
        return Integer.bitCount(supportedRafMaskForNwMode);
    }

    private void sendSubscriptionSettings(int phoneId) {
        Phone phone = mPhone[phoneId];
        int type = getNetworkModeFromDB(phoneId);
        mCi[phoneId].setPreferredNetworkType(type, null);

        int[] subId = mQtiSubscriptionController.getSubId(phoneId);
        if ((subId != null) && (subId.length > 0) &&
                mQtiSubscriptionController.isActiveSubId(subId[0])) {
            phone.mDcTracker.setDataEnabled(
                    phone.mDcTracker.getDataEnabled());
        }
    }

    private void notifyRadioCapsUpdated(boolean isCrossMapDone) {
        logd("notifyRadioCapsUpdated: radio caps updated " + isCrossMapDone);

        if (isCrossMapDone) {
            for (int i = 0; i < mNumPhones; i++) {
                //update the current stackIds
                mCurrentStackId[i] = mPreferredStackId[i];
            }
        }
        Intent intent = new Intent("org.codeaurora.intent.action.ACTION_RADIO_CAPABILITY_UPDATED");
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void sendResponseToTarget(Message response, int responseCode) {
        if (response != null) {
            Exception e = CommandException.fromRilErrno(responseCode);
            AsyncResult.forMessage(response, null, e);
            response.sendToTarget();
        }
    }

    private boolean isAnyCallsInProgress() {
        boolean isCallInProgress = false;
        for (int i = 0; i < mNumPhones; i++) {
            if (mPhone[i].getState() != PhoneConstants.State.IDLE) {
                isCallInProgress = true;
                break;
            }
        }
        return isCallInProgress;
    }

    private boolean isAnyPhoneInEcmState() {
        boolean isInEcmState = false;
        for (int i = 0; i < mNumPhones; i++) {
            if (mPhone[i].isInEcm()) {
                isInEcmState = true;
                break;
            }
        }
        return isInEcmState;
    }

    private boolean isUiccProvisionInProgress() {
        boolean retVal = false;
        QtiUiccCardProvisioner uiccProvisioner = QtiUiccCardProvisioner.getInstance();

        if (uiccProvisioner != null) {
            retVal = uiccProvisioner.isAnyProvisionRequestInProgress();
            logd("isUiccProvisionInProgress: retVal =  " + retVal);
        }
        return retVal;
    }

    private boolean isCardAbsent(int phoneId) {
        int provisionStatus = QtiUiccCardProvisioner.UiccProvisionStatus.INVALID_STATE;

        QtiUiccCardProvisioner uiccProvisioner = QtiUiccCardProvisioner.getInstance();

        if (uiccProvisioner != null) {
            provisionStatus = uiccProvisioner.getCurrentUiccCardProvisioningStatus(phoneId);
            logd("provisionStatus[" + phoneId +"] : " + provisionStatus);
        }
        return (provisionStatus == QtiUiccCardProvisioner.UiccProvisionStatus.CARD_NOT_PRESENT);
    }

    private void setNWModeInProgressFlag(boolean newStatus) {
        synchronized(sSetNwModeLock) {
            mIsSetPrefNwModeInProgress = newStatus;
        }
    }

    public boolean isSetNWModeInProgress() {
        boolean retVal = false;

        synchronized(sSetNwModeLock) {
            retVal = mIsSetPrefNwModeInProgress;
        }
        return retVal;
    }

    public void radioCapabilityUpdated(int phoneId, RadioCapability rc) {
        if (SubscriptionManager.isValidPhoneId(phoneId) && !isSetNWModeInProgress()) {
            logd(" radioCapabilityUpdated phoneId[" + phoneId + "] rc = " + rc);

            // Wait till we get Modem Capabilities on all subs.
            if (areAllModemCapInfoReceived()) {
                sendMessage(obtainMessage(EVENT_RADIO_CAPS_AVAILABLE));
            }
        } else {
            loge("radioCapabilityUpdated: Invalid phoneId=" + phoneId + " or SetNWModeInProgress");
        }
    }

    public void setDdsIfRequired(boolean forceSetDds) {
        int ddsSubId = mQtiSubscriptionController.getDefaultDataSubId();
        int ddsPhoneId = mQtiSubscriptionController.getPhoneId(ddsSubId);
        logd("setDdsIfRequired: ddsSub = " + ddsSubId + " ddsPhone = " + ddsPhoneId
                + " force = " + forceSetDds + " needSetDds = " + mNeedSetDds);
        if (SubscriptionManager.isValidPhoneId(ddsPhoneId) &&
                (forceSetDds || mNeedSetDds)) {
            mCi[ddsPhoneId].setDataAllowed(true, null);
            if (mNeedSetDds) mNeedSetDds = false;
        }
    }

    boolean is7Plus7DeviceFlexMapped() {
        boolean retVal = false;

        if ((mNumPhones > 1) && areAllModemCapInfoReceived()) {
            if ((Integer.valueOf(mPhone[0].getModemUuId()) == 1) &&
                    (mPhone[0].getRadioAccessFamily() == mPhone[1].getRadioAccessFamily()) &&
                    (mPhone[0].getRadioAccessFamily() != 0)) {
                logi("Device is flex mapped: " + mPhone[0].getModemUuId() +
                        " raf = " + mPhone[0].getRadioAccessFamily());
                retVal = true;
            }
        }
        return retVal;
    }

    boolean isBothPhonesMappedToSameStack() {
        boolean retVal = false;

        if ((mNumPhones > 1) && areAllModemCapInfoReceived()) {
            // If logical modemId's of both phones same return true
            if (Integer.valueOf(mPhone[0].getModemUuId()) ==
                    Integer.valueOf(mPhone[1].getModemUuId())) {
                loge("Error: both Phones mapped same stackId: " + mPhone[0].getModemUuId() +
                        " raf = " + mPhone[0].getRadioAccessFamily());
                retVal = true;
                bothPhonesMappedToSameStack = true;
            }
        }
        return retVal;
    }

    // This method would be called to normal map the Phones to modem stacks
    // when both Phone objects mapped to single logical modem(i.e both Phones
    // mapped to '0' or '1').
    // To normal map, it would first set the both Phones RadioCapability in 5+7
    // configuration and then calls setRadioCapability() with 7+5 combination request.
    boolean initNormalMappingRequest() {
        RadioCapability[] oldRadioCapability = new RadioCapability[mNumPhones];
        int maxRaf = RadioAccessFamily.getRafFromNetworkType(
                RILConstants.NETWORK_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA);
        int minRaf = RadioAccessFamily.getRafFromNetworkType(RILConstants.NETWORK_MODE_GSM_ONLY);

        logd(" initNormalMappingRequest  ");
        setNWModeInProgressFlag(true);

        // Update RadioCapability of Phone1 to modemId '1' and Phone2 to modemId '0'.
        // There is a condition check in ProxyController to do not allow flex map
        // request if the Phone object RAF and received RAF are same, to pass that condition
        // check first we are updating this.
        for (int i = 0; i < mNumPhones; i++) {
            oldRadioCapability[i] =  mPhone[i].getRadioCapability();
            RadioCapability rc = new RadioCapability(i, 0, 0, (i == 0) ? minRaf : maxRaf,
                    (i == 0) ? "1" : "0", RadioCapability.RC_STATUS_SUCCESS);
            mPhone[i].radioCapabilityUpdated(rc);
        }

        RadioAccessFamily[] rafs = new RadioAccessFamily[mNumPhones];
        for (int i = 0; i < mNumPhones; i++) {
            rafs[i] = new RadioAccessFamily(i, (i == 0) ? maxRaf : minRaf);
        }

        // If flex map request not accepted reset the flag to allow
        // request from other places
        if (!ProxyController.getInstance().setRadioCapability(rafs)) {
            // Flex map request failed, re-store the capabilities back.
            for (int i = 0; i < mNumPhones; i++) {
                mPhone[i].radioCapabilityUpdated(oldRadioCapability[i]);
            }
            logd(" initNormalMappingRequest:  Fail, request in progress ");
            setNWModeInProgressFlag(false);
            return false;
        }
        return true;
    }

    private void logd(String string) {
        if (DBG) Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }

    private void logv(String string) {
        if (VDBG) Rlog.d(LOG_TAG, string);
    }
}

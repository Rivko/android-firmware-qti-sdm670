/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.primarycard;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.Rlog;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.RILConstants;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.TelephonyIntents;

import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner;
import com.qualcomm.qti.internal.telephony.QtiSubscriptionController;
import com.qualcomm.qti.internal.telephony.QtiRilInterface;

/**
 * This class has logic to set primary card on highest priority card.
 */
public class QtiPrimaryCardController extends Handler{
    private static final String LOG_TAG = "QtiPcController";
    private static final boolean VDBG = false;
    private static final boolean DBG = true;


    //*********Events*********
    private static final int EVENT_ALL_CARDS_INFO_AVAILABLE = 1;
    private static final int EVENT_SET_RADIO_CAPABILITY_DONE = 2;
    private static final int EVENT_SET_NWMODE_DONE = 3;
    private static final int EVENT_GET_NWMODE_DONE = 4;
    private static final int EVENT_PRIMARY_CARD_SET_DONE = 5;
    private static final int EVENT_SET_PRIMARY_SUB = 6;
    private static final int EVENT_SERVICE_STATE_CHANGED = 7;
    private static final int EVENT_OEM_HOOK_SERVICE_READY = 8;

    //*********Class Variables*********
    private static QtiPrimaryCardController sInstance;
    private CommandsInterface[] mCi;
    private Phone[] mPhone;
    QtiCardInfoManager mCardInfoMgr;
    QtiPrimaryCardPriorityHandler mPriorityHandler;
    QtiPrimaryCardUtils mPcUtils;
    private final Context mContext;
    private QtiRilInterface mQtiRilInterface;
    private int mPrefPrimarySlot = QtiPrimaryCardUtils.SLOT_INVALID;
    private int[] mPrefNwModes;
    private Message mCmdMessage;
    private boolean mCardChanged = false;
    private boolean mPriorityMatch = false;
    private boolean mPcTriggeredFlexMapDone = false;
    private String[] mCurrentIccIds;
    private int mRetryCount;
    private int mRetryPrimarySlot;
    private int[] mRetryArray = {2, 5, 10, 20, 30};
    private static final int FWK_PRIMARY_CARD_REQUEST = 1000;
    private static final int USER_PRIMARY_CARD_REQUEST = 1001;
    private boolean mIsLpluslSupport = false;

    private static final String ACTION_SET_PRIMARY_CARD_DONE =
            "org.codeaurora.intent.action.ACTION_SET_PRIMARY_CARD_DONE";
    private static final String ACTION_RADIO_CAPABILITY_UPDATED =
            "org.codeaurora.intent.action.ACTION_RADIO_CAPABILITY_UPDATED";
    private static final String PRIMARY_CARD_RESULT = "result";
    private static final String PRIMARYCARD_SUBSCRIPTION_KEY = "primarycard_sub";
    private static final String SETTING_USER_PREF_DATA_SUB = "user_preferred_data_sub";
    private static final int[] sCtIins = {898603, 898611};
    private static final int[] sCmccIins = {898600, 898602, 898607, 898608,898521, 898212};

    // check current carrier if is CMCC
    private static final String CARRIER_MODE_CMCC = "cmcc";
    private String mCarrierMode = SystemProperties.get("persist.radio.carrier_mode", "default");
    private boolean mIsCMCC = mCarrierMode.equals(CARRIER_MODE_CMCC);

    SubsidyLockSettingsObserver mSubsidyLockSettingsObserver;
    /**
     * This intent would be broadcasted when ever primary card slotId changed and its
     * registered onto network.
     */
    private static final String ACTION_PRIMARY_CARD_CHANGED_IN_SERVICE =
            "org.codeaurora.intent.action.PRIMARY_CARD_CHANGED_IN_SERVICE";

    public enum PrimaryCardState{
        IDLE,
        IN_PROGRESS,
        PENDING_DUE_TO_PC_IN_PROGRESS,
        PENDING_DUE_TO_FLEXMAP_IN_PROGRESS
    }

    private PrimaryCardState mPrimaryCardState = PrimaryCardState.IDLE;

    public static void init(Context context, Phone[] phones,
            CommandsInterface[] ci) {
        synchronized (QtiPrimaryCardController.class) {
            if (sInstance == null && QtiPrimaryCardUtils.isPrimaryCardFeatureEnabled()) {
                sInstance = new QtiPrimaryCardController(context, phones, ci);
            }
        }
    }

    public static QtiPrimaryCardController getInstance() {
        synchronized (QtiPrimaryCardController.class) {
            if (sInstance == null) {
                throw new RuntimeException("QtiPrimaryCardController was not initialized!");
            }
            return sInstance;
        }
    }

    //******Constructor********
    private QtiPrimaryCardController(Context context, Phone[] phones,
            CommandsInterface[] ci) {
        mContext = context;
        mPhone = phones;
        mCi = ci;
        mPcUtils = QtiPrimaryCardUtils.init(mContext);
        mPriorityHandler = new QtiPrimaryCardPriorityHandler(mContext);
        mCardInfoMgr =  QtiCardInfoManager.init(mContext, ci);
        mCardInfoMgr.registerAllCardsInfoAvailable(this, EVENT_ALL_CARDS_INFO_AVAILABLE, null);
        mPrefNwModes = new int[QtiPrimaryCardUtils.PHONE_COUNT];
        mCurrentIccIds = new String[QtiPrimaryCardUtils.PHONE_COUNT];

        resetPrimaryCardParams();

        IntentFilter intentFilter = new IntentFilter(
                TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE);
        if (SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled()) {
            mSubsidyLockSettingsObserver = new SubsidyLockSettingsObserver(mContext);
            mSubsidyLockSettingsObserver.observe(mCardInfoMgr, mPriorityHandler);
        }
        mContext.registerReceiver(mReceiver, intentFilter);

        mQtiRilInterface = QtiRilInterface.getInstance(context);
        mQtiRilInterface.registerForServiceReadyEvent(this, EVENT_OEM_HOOK_SERVICE_READY, null);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            logd("Recieved intent " + action);
            if (TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE.equals(action)) {
                sendMessage(obtainMessage(EVENT_SET_RADIO_CAPABILITY_DONE));
            } else if (ACTION_RADIO_CAPABILITY_UPDATED.equals(action)) {
                triggerPrimaryCardIfRequired();
            }
        }
    };

    private void triggerPrimaryCardIfRequired() {
        if (mQtiRilInterface.isServiceReady() && (mIsLpluslSupport !=
                mQtiRilInterface.getLpluslSupportStatus())) {
            mPriorityHandler.reloadPriorityConfig();
            setPrimaryCardIfRequired(true);
            mIsLpluslSupport = mQtiRilInterface.getLpluslSupportStatus();
        }
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case EVENT_ALL_CARDS_INFO_AVAILABLE:
                logd("on EVENT_ALL_CARDS_INFO_AVAILABLE");
                setPrimaryCardIfRequired(false);
                break;
            case EVENT_SET_RADIO_CAPABILITY_DONE:
                logd("on EVENT_SET_RADIO_CAPABILITY_DONE");
                handleSetRadioCapsDone();
                break;
            case EVENT_SET_NWMODE_DONE:
                logd("on EVENT_SET_NWMODE_DONE");
                handleSetNwModeDone(msg);
                break;
            case EVENT_GET_NWMODE_DONE:
                logd("on EVENT_GET_NWMODE_DONE");
                handleGetNwModeDone(msg);
                break;
            case EVENT_PRIMARY_CARD_SET_DONE:
                logd("on EVENT_PRIMARY_CARD_SET_DONE");
                handleOnSetPrimaryCardDone(msg);
                break;
            case EVENT_OEM_HOOK_SERVICE_READY:
                mPriorityHandler.reloadPriorityConfig();
                if (mQtiRilInterface.isServiceReady()) {
                    mIsLpluslSupport = mQtiRilInterface.getLpluslSupportStatus();
                    if (!mIsLpluslSupport) {
                        IntentFilter intentFilter = new IntentFilter(
                                ACTION_RADIO_CAPABILITY_UPDATED);
                        mContext.registerReceiver(mReceiver, intentFilter);
                    }
                }
                logd("on EVENT_OEM_HOOK_SERVICE_READY and mIsLpluslSupport = " + mIsLpluslSupport);
                break;
            case EVENT_SET_PRIMARY_SUB:
                logd("on EVENT_SET_PRIMARY_SUB ");
                setPrimaryCardIfRequired(true);
            case EVENT_SERVICE_STATE_CHANGED:
                logd("on EVENT_SERVICE_STATE_CHANGED ");
                handleServiceStateChanged(msg);
                break;
        }
    }

    private void handleSetRadioCapsDone() {
        if (mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_FLEXMAP_IN_PROGRESS) {
            //If primary card is setting is pending due to explicit flex map in progress,
            //try again now.
            mPrimaryCardState = PrimaryCardState.IDLE;
            logd("Flex mapping completed, try setting primary card now");
            setPrimaryCardIfRequired(false);
        } else if (mPrimaryCardState == PrimaryCardState.IN_PROGRESS
                || mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS) {
            //Implicit flex mapping trigerred due to primary card setting is completed.
            logd("Primary card trigerred Flex Mapping completed.");
            mPcTriggeredFlexMapDone = true;
        }
    }

    private void handleSetNwModeDone(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) msg.arg1;
        int requestType = (Integer) msg.arg2;
        logd("set " + mPrefNwModes[index] + " for slot " + index + " done, " + ar.exception);
        if (ar.exception != null) {
            if (++mRetryCount <= mRetryArray.length &&
                    requestType == FWK_PRIMARY_CARD_REQUEST) {
                int delay = mRetryArray[mRetryCount - 1] * 1000;
                logd("Scheduling retry for failed set primary card request: " + delay + " ms");
                mRetryPrimarySlot = mPrefPrimarySlot;
                postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (mRetryPrimarySlot == mPrefPrimarySlot) {
                            logd("Retrying setPrimaryCardIfRequired request");
                            setPrimaryCardIfRequired(false, true);
                        } else {
                            logd("Primary card slot changed, skip retry");
                            resetSetNwModeFailureCount();
                        }
                    }}, delay);
            } else {
                resetSetNwModeFailureCount();
                for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                    mPhone[i].getPreferredNetworkType(
                            obtainMessage(EVENT_GET_NWMODE_DONE, i));
                }
                sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
            }
            return;
        } else {
            if (mRetryCount > 0) {
                resetSetNwModeFailureCount();
            }
        }

        if (mPcTriggeredFlexMapDone || (index == mPrefPrimarySlot)) {
            //Primary Card Setting is completed, set DDS and send SUCCESS to target.
            setDdsOnPrimaryCardIfRequired();
            sendSetPrimaryCardResult(RILConstants.SUCCESS);
        } else {
            logd("set NwMode[" + mPrefNwModes[mPrefPrimarySlot] +
                    "] on Primarycard:" + mPrefPrimarySlot);
            mPhone[mPrefPrimarySlot].setPreferredNetworkType(mPrefNwModes[mPrefPrimarySlot],
                    obtainMessage(EVENT_SET_NWMODE_DONE, mPrefPrimarySlot, requestType));
        }
    }

    private void resetSetNwModeFailureCount() {
        mRetryCount = 0;
    }

    private void handleGetNwModeDone(Message msg) {
        int modemNwMode = -1;
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        if (ar.exception == null) {
            modemNwMode = ((int[]) ar.result)[0];
            saveNwModesToDB(modemNwMode, index);
        }
        logd("got nwMode:" + modemNwMode + " on slot" + index + ", saved to DB, " + ar.exception);
    }

    private void setDdsOnPrimaryCardIfRequired() {
        SubscriptionController subCtrlr = SubscriptionController.getInstance();

        int subId = subCtrlr.getSubIdUsingPhoneId(mPrefPrimarySlot);
        subCtrlr.setDefaultDataSubId(subId);
        android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                SETTING_USER_PREF_DATA_SUB, subId);
        logd("Cofigure DDS on " + subId);
    }

    private void sendSetPrimaryCardResult(int responseCode) {
        if (mCmdMessage != null) {
            Exception e = CommandException.fromRilErrno(responseCode);
            AsyncResult.forMessage(mCmdMessage, null, e);
            mCmdMessage.sendToTarget();
            mCmdMessage = null;
        }

        if (responseCode == RILConstants.SUCCESS) {
            saveCardIccIdInfoInSp();
            notifySetPrimaryCardDone(true);
            QtiPrimaryCardUtils.savePrimarySlotToDB(mPrefPrimarySlot);
            broadcastPrimarySlotServiceChanged(mPrefPrimarySlot);
        } else {
            notifySetPrimaryCardDone(false);
        }

        if (mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS) {
            mPrimaryCardState = PrimaryCardState.IDLE;
            logi("Primary Card request completed, check for pending reqeusts");
            setPrimaryCardIfRequired(false);
        } else if (mPrimaryCardState == PrimaryCardState.IN_PROGRESS) {
            mPrimaryCardState = PrimaryCardState.IDLE;
        }
    }

    void broadcastPrimarySlotServiceChanged(int slotId) {
        if (!SubscriptionManager.isValidSlotIndex(slotId)) {
            logd(" Error!!! Invalid slotId " + slotId);
            return;
        }

        mPhone[slotId].unregisterForServiceStateChanged(this);

        ServiceState ss = mPhone[slotId].getServiceState();
        if (ss != null &&
                (ss.getState() == ServiceState.STATE_IN_SERVICE ||
                (ss.getDataRegState() == ServiceState.STATE_IN_SERVICE
                && ss.getDataNetworkType() != TelephonyManager.NETWORK_TYPE_IWLAN))) {
            logd(" broadcastPrimarySlotServiceChanged, slotId " + slotId);

            Intent intent = new Intent(ACTION_PRIMARY_CARD_CHANGED_IN_SERVICE);
            intent.putExtra(PhoneConstants.PHONE_KEY, slotId);
            mContext.sendBroadcast(intent);
        } else {
            mPhone[slotId].registerForServiceStateChanged(this,
                    EVENT_SERVICE_STATE_CHANGED, new Integer(slotId));
        }
    }

    private void handleServiceStateChanged(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;

        if (ar != null) {
            int currentPrimarySlot
                    = QtiPrimaryCardUtils.getCurrentPrimarySlotFromDB(mContext);
            int slotId = (Integer) ar.userObj;

            if (SubscriptionManager.isValidSlotIndex(slotId)) {
                mPhone[slotId].unregisterForServiceStateChanged(this);
            } else {
                logd(" Error, Invalid slotId " + slotId);
            }
            broadcastPrimarySlotServiceChanged(currentPrimarySlot);
        }
    }

    private void notifySetPrimaryCardDone(boolean isPass) {
        logd("notifySetPrimaryCardDone: Set Primary Card SUCCESS: " + isPass);
        Intent intent = new Intent(ACTION_SET_PRIMARY_CARD_DONE);
        intent.putExtra(PRIMARY_CARD_RESULT, isPass ? 1 : 0);
        intent.putExtra(PhoneConstants.PHONE_KEY, mPrefPrimarySlot);
        mContext.sendBroadcast(intent);
    }

    private void handleOnSetPrimaryCardDone(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        if (ar.exception == null) {
            //Set Primary card is completed Successfully, save the value to DB.
            QtiPrimaryCardUtils.savePrimarySlotToDB(index);
            broadcastPrimarySlotServiceChanged(index);

            int userSelectionMode = QtiPrimaryCardUtils.getUserSelectionMode();
            boolean enableUserSelection = false;

            int numCmccCards = 0;
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                if (isCardMatchesIins(mCardInfoMgr.getCardInfo(i).getIccId(), sCmccIins)) {
                    numCmccCards++;
                }
            }

            //enable user selection if
            // 1.show user selection on every change is true or
            // 2.show user selection on priority match is true and currently there is priority
            //   match.
            logd("userSelectionMode = " + userSelectionMode + ", mPriorityMatch" + mPriorityMatch +
                    ", numCmccCards = " + numCmccCards);
            if (userSelectionMode == QtiPrimaryCardUtils.SHOW_USER_SELECTION_FOR_EVERY_CHANGE ||
                    (userSelectionMode == QtiPrimaryCardUtils.SHOW_USER_SELECTION_ON_PRIORITY_MATCH
                    && mPriorityMatch)) {
                enableUserSelection = true;
            }

            if (mIsCMCC){
                if (numCmccCards == 0)  enableUserSelection = true;
                QtiPrimaryCardUtils.saveEnableUserSelectioninDB(enableUserSelection);
                logd("handleOnSetPrimaryCardDone: enableUserSelection =" + enableUserSelection +
                        ", mCardChanged:" + mCardChanged + ", numCardsValid:" + numCardsValid());

                if (enableUserSelection && mCardChanged && numCardsValid() > 1 && mPriorityMatch) {
                    startLTEConifgActivity();
                }

                return;
            }

            QtiPrimaryCardUtils.saveEnableUserSelectioninDB(enableUserSelection);
            boolean subsidyLockFeatureEnabled = SubsidyLockSettingsObserver
                    .isSubsidyLockFeatureEnabled();
            boolean isPermanentlyUnlocked = true;
            if (subsidyLockFeatureEnabled) {
                isPermanentlyUnlocked =
                        mSubsidyLockSettingsObserver.isPermanentlyUnlocked(mContext);
            }

            if (enableUserSelection && mCardChanged && numCardsValid() > 1
                    && isPermanentlyUnlocked) {
                startLTEConifgActivity();
            }
        }
    }

    private void startLTEConifgActivity () {
        Intent intent = new Intent("codeaurora.intent.action.ACTION_LTE_CONFIGURE");
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
                | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
                | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        mContext.startActivity(intent);
    }

    private boolean haveCMCCSimCard() {
        for (int i = 0; i < numCardsValid(); i++) {
            String iccId = mCardInfoMgr.getCardInfo(i).getIccId();
            if (!TextUtils.isEmpty(iccId)) {
                String subIccId = iccId.substring(0,6);
                logd("iccId is: " + subIccId + " on slot" + i);
                if ("898600".equals(subIccId) || "898602".equals(subIccId) || "898607".equals
                        (subIccId) || "898608".equals(subIccId)) {
                    return true;
                }
            }

        }
        return false;
    }

    public void saveUserSelectionMode() {
        int userSelectionMode = QtiPrimaryCardUtils.getUserSelectionMode();
        boolean enableUserSelection = false;

        //enable user selection if
        // 1.show user selection on every change is true or
        // 2.show user selection on priority match is true and currently there is priority
        //   match.
        if (userSelectionMode == QtiPrimaryCardUtils.SHOW_USER_SELECTION_FOR_EVERY_CHANGE ||
                (userSelectionMode == QtiPrimaryCardUtils.SHOW_USER_SELECTION_ON_PRIORITY_MATCH
                && mPriorityMatch)) {
            enableUserSelection = true;
        }
        logd("saveUserSelectionMode: enableUserSelection =" + enableUserSelection);

        QtiPrimaryCardUtils.saveEnableUserSelectioninDB(enableUserSelection);
    }

    private int numCardsValid() {
        int numCount = 0;
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            if (mCardInfoMgr.getCardInfo(i).getIccId() != null) numCount++;
        }
        return numCount;
    }

    private void updateDdsPreferenceInDb() {
        boolean disableDds = false;
        if (QtiPrimaryCardUtils.isPrimaryCardFeatureEnabled() &&
                QtiPrimaryCardUtils.isPrimary7Plus5Enabled()) {
            int numCmccCards = 0;
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                if (isCardMatchesIins(mCardInfoMgr.getCardInfo(i).getIccId(), sCmccIins)) {
                    numCmccCards++;
                }
            }

            logi("numCmccCards: " + numCmccCards);
            //Disable DDS for CMCC + non-CMCC case
            if (numCmccCards ==1 && numCmccCards != 2) {
                logi("updateDdsPreferenceInDb: Disable DDS in UI.");
                disableDds = true;
            }
        }

        if (QtiPrimaryCardUtils.disableDds()) {
            disableDds = true;
        }

        QtiPrimaryCardUtils.saveDisableDdsPreferenceInDB(disableDds);
    }

    private boolean isCardMatchesIins(String iccId, int[] iins) {
        // iin length is 6
        if (iccId == null || iccId.length() < 6) {
            return false;
        }
        int cardIin = Integer.parseInt(iccId.substring(0, 6));
        for (int iin : iins) {
            if (iin == cardIin) return true;
        }
        return false;
    }

    private void resetPrimaryCardParams() {
        mPriorityMatch = false;
        mCmdMessage = null;
        mPcTriggeredFlexMapDone = false;
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            mPrefNwModes[i] = QtiPrimaryCardUtils.getDefaultNwMode();
        }
    }

    public void trySetPrimarySub() {
        sendMessage(obtainMessage(EVENT_SET_PRIMARY_SUB));
    }

    private void setPrimaryCardIfRequired(boolean force) {
        setPrimaryCardIfRequired(force, false);
    }

    /*
    * setPrimaryCardIfRequired: Method used to set primary card if required.
    */
    private void setPrimaryCardIfRequired(boolean force, boolean isRetryRequest) {
        logd("setPrimaryCardIfRequired: force: " + force);
        int slotId = QtiPrimaryCardUtils.SLOT_INVALID;

        if ((mPrimaryCardState == PrimaryCardState.IN_PROGRESS
                || mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS)
                && !isRetryRequest) {
            //If Primary card is in progrees or pending, set state as pending. So that once
            // primary card setting is completed it will check for pending state and initiate the
            // primary card sequence again.

            mPrimaryCardState = PrimaryCardState.PENDING_DUE_TO_PC_IN_PROGRESS;
            logi("Primary Card setting in progress. WAIT!");
            return;
        } else if (QtiUiccCardProvisioner.getInstance().isFlexMapInProgress()
                || mPrimaryCardState == PrimaryCardState.PENDING_DUE_TO_FLEXMAP_IN_PROGRESS) {
            //If we are here that means flex mapping is in progress and primary card is not in
            // progress. So set state to flex map pending.
            mPrimaryCardState = PrimaryCardState.PENDING_DUE_TO_FLEXMAP_IN_PROGRESS;
            logi("Flex Map in progress. WAIT!");
            return;
        } else if (QtiUiccCardProvisioner.getInstance().isAnyProvisionRequestInProgress()) {
            //Manual provsioning is in Progress, just ignore the event. We will anyways get the
            // indication again from cardInfoMgr once provisioning is completed.
            logi("Manual provisioning in progress. EXIT!");
            return;
        }

        boolean isCardChanged = isCardsInfoChanged();

        boolean override = SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled() || force;
        mPriorityHandler.loadCurrentPriorityConfigs(override || isCardChanged);

        //Get Pref Primary slot from Priority config.
        mPrefPrimarySlot = mPriorityHandler.getPrefPrimarySlot();
        loge("mPrefPrimarySlot: setPrimaryCardIfRequired: " + mPrefPrimarySlot);

        //Save Settable value in the DB.
        boolean isSetable = mQtiRilInterface.getLpluslSupportStatus() ? false :
                (mPrefPrimarySlot != QtiPrimaryCardUtils.SLOT_INVALID);
        QtiPrimaryCardUtils.savePrimarySetable(isSetable);

        if(!isCardChanged && !isRetryRequest && !force) {
            logd("primary card " + QtiPrimaryCardUtils.getCurrentPrimarySlotFromDB(mContext) +
                    " ,Cards not changed, IGNORE!!");
            return;
        }
        mCardChanged = isCardChanged;

        resetPrimaryCardParams();

        //Save disableDDS option in DB.
        updateDdsPreferenceInDb();

        if (mPrefPrimarySlot == QtiPrimaryCardUtils.SLOT_PRIORITY_MATCH) {
            mPrefPrimarySlot = QtiPrimaryCardUtils.getDefaultPrimarySlot();
            mPriorityMatch = true;
        } else if (mPrefPrimarySlot < 0) {
            logi("Both slots do not have cards with priority config defined. EXIT!");
            // During re-try operation, if there is no valid slot available to configure Primary
            // SIM perform the required clean-up.
            if (isRetryRequest) {
                sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
                resetSetNwModeFailureCount();
            }
            return;
        }

        setPrimaryCardOnSlot(mPrefPrimarySlot,
                obtainMessage(EVENT_PRIMARY_CARD_SET_DONE, mPrefPrimarySlot), isRetryRequest);

    }

    //This method will be invoked from AIDL call.
    public void setPrimaryCardOnSlot(int slotId) {
        if (!QtiSubscriptionController.getInstance().isRadioInValidState()) {
            loge("setPrimaryCardOnSlot[" + slotId + "]: Radio is in Invalid State, EXIT!!!");
            sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
            return;
        }

        setPrimaryCardOnSlot(slotId, null);
    }

    synchronized private void setPrimaryCardOnSlot(int slotId, Message msg) {
        setPrimaryCardOnSlot(slotId, msg, false);
    }

    synchronized private void setPrimaryCardOnSlot(int primarySlotId, Message msg,
             boolean isRetryRequest) {
        SubscriptionController subCtrlr = SubscriptionController.getInstance();
        int subId = subCtrlr.getSubIdUsingPhoneId(primarySlotId);

        logd("setPrimaryCardOnSlot: for slotId:" + primarySlotId + ", Start.");

        if ((mPrimaryCardState != PrimaryCardState.IDLE && !isRetryRequest)
                || !mPriorityHandler.isConfigLoadDone()
                || !SubscriptionManager.isValidSlotIndex(primarySlotId) || !subCtrlr.isActiveSubId(subId)) {
            loge("Primary Card State is not IDLE, mPrimaryCardState:" + mPrimaryCardState +
                    " subId: " + subId + ", or configs not yet loaded EXIT!");
            sendSetPrimaryCardResult(RILConstants.GENERIC_FAILURE);
            return;
        }

        // If request received from UI save IccIds in mCurrentIccIds cache, this would
        // be used in saveCardIccIdInfoInSp() to update in Shared pref.
        if (msg == null) {
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                mCurrentIccIds[i] = mCardInfoMgr.getCardInfo(i).getIccId();
            }
        }

        mPrimaryCardState = PrimaryCardState.IN_PROGRESS;

        //get pref nwModes on both slots as per config
        mPrefNwModes = mPriorityHandler.getNwModesFromConfig(primarySlotId);
        mPrefPrimarySlot = primarySlotId;
        mCmdMessage = msg;
        int isFwkRequest = (mCmdMessage != null) ? FWK_PRIMARY_CARD_REQUEST : USER_PRIMARY_CARD_REQUEST;
        mPcTriggeredFlexMapDone = false;

        //Save PrefNwMode of both slots to DB before sending setPref request to phone.
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            saveNwModesToDB(mPrefNwModes[i], i);
        }

        //First set nwMode on secondary card.Once secondary card nwMode is done set nwMode on
        //Primary card and DDS
        for (int index = 0; index < QtiPrimaryCardUtils.PHONE_COUNT; index++) {
            if (index != primarySlotId) {
                logd("set NwMode[" + mPrefNwModes[index] + "]  on Secondary card:" + index);
                mPhone[index].setPreferredNetworkType(mPrefNwModes[index],
                        obtainMessage(EVENT_SET_NWMODE_DONE, index, isFwkRequest));
            }
        }
    }

    private void saveNwModesToDB(int nwMode, int slotId) {
        int[] subId = QtiSubscriptionController.getInstance().getSubId(slotId);
        if (subId != null) {
            logi("saveNwModesToDB: subId[" + slotId + "] = " + subId[0]
                    + ", new Nw mode = " + nwMode);
            if (QtiSubscriptionController.getInstance().isActiveSubId(subId[0])) {
                android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId[0],
                        nwMode);
            }
        } else {
            loge("saveNwModesToDB: subId is null, do not save nwMode in subId based DB");
        }
        TelephonyManager.putIntAtIndex(mContext.getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE, slotId,
                nwMode);
    }

    private boolean isCardsInfoChanged() {
        boolean cardChanged = false;
        for (int index = 0; index < QtiPrimaryCardUtils.PHONE_COUNT; index++) {
            if (isCardsInfoChanged(index)) {
                cardChanged = true;
            }
        }
        return cardChanged;
    }

    protected boolean isCardsInfoChanged(int phoneId) {
        String iccId = mCurrentIccIds[phoneId] = mCardInfoMgr.getCardInfo(phoneId).getIccId();
        String iccIdInSP = PreferenceManager.getDefaultSharedPreferences(mContext).getString(
                PRIMARYCARD_SUBSCRIPTION_KEY + phoneId, null);
        logd(" phoneId " + phoneId + " icc id = " + iccId + ", icc id in sp=" + iccIdInSP);
        return !TextUtils.equals(iccId, iccIdInSP);
    }

    private void saveCardIccIdInfoInSp() {
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            String iccId = mCurrentIccIds[i];

            logd("save IccId: " + iccId + ", on slotId:" + i + ", in SP.");
            PreferenceManager.getDefaultSharedPreferences(mContext).edit()
                    .putString(PRIMARYCARD_SUBSCRIPTION_KEY + i, iccId).commit();
        }
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
}


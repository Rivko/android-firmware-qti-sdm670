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
import android.os.Registrant;
import android.os.RegistrantList;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.IccConstants;
import com.android.internal.telephony.uicc.IccFileHandler;
import com.android.internal.telephony.uicc.IccUtils;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccCardApplication;
import com.android.internal.telephony.uicc.UiccController;

import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner;
import com.qualcomm.qti.internal.telephony.QtiSubscriptionController;


/**
 * This class has logic to
 * 1. Get and update the info of presently inserted cards.
 * 2. Notify mAllCardsInfoAvailableRegistrants as and when applicable.
 */
public class QtiCardInfoManager extends Handler{
    private static final String LOG_TAG = "QtiPcCardInfoManager";
    private static final boolean VDBG = false;
    private static final boolean DBG = true;

    //*********Events*********
    private static final int EVENT_MANUAL_PROVISION_STATE_CHANGED = 1;
    private static final int EVENT_READ_EF_HPLMNWACT_DONE = 2;
    private static final int EVENT_ICC_CHANGED = 3;
    private static final int EVENT_SUBINFO_RECORD_ADDED = 4;

    //*********Class Variables*********
    private static QtiCardInfoManager sInstance;
    static final int PHONE_COUNT = TelephonyManager.getDefault().getPhoneCount();
    private static Context mContext;
    private QtiUiccCardProvisioner mQtiCardProvisioner;
    private RegistrantList mAllCardsInfoAvailableRegistrants = new RegistrantList();

    //Enum Defining Card Types
    public enum CardType{
        UNKNOWN,
        CARDTYPE_2G,    //2G Sim
        CARDTYPE_3G,    //3G sim
        CARDTYPE_4G     //4G sim
    }

    //Update Card Type States.
    private static final int UPDATE_CARDTYPE_INIT = 0;
    private static final int UPDATE_CARDTYPE_IN_PROGRESS = 1;
    private static final int UPDATE_CARDTYPE_COMPLETED = 2;
    private static final int UPDATE_CARDTYPE_NOT_NEEDED = 3;

    /* Data Length of contents in EF_HPLMNWACT is 5n bytes:
       n is num records and each record length is 5 bytes */
    private static final int HPLMN_SEL_DATA_LEN = 5;

    static class CardInfo {
        private String mIccId;
        private String mMccMnc;
        private CardType mCardType;
        private int mProvisionState;
        private int mUpdateCardTypeState;
        private boolean mMCCMNCLoaded;

        private void reset() {
            mCardType = CardType.UNKNOWN;
            mIccId = null;
            mMccMnc = null;
            mUpdateCardTypeState = UPDATE_CARDTYPE_INIT;
            mProvisionState = QtiUiccCardProvisioner.UiccProvisionStatus.INVALID_STATE;
        }

        public String getIccId() {
            if (QtiPrimaryCardUtils.setPrimaryCardOnDeAct() && mProvisionState
                    == QtiUiccCardProvisioner.UiccProvisionStatus.NOT_PROVISIONED) {
                return null;
            } else {
                return mIccId;
            }
        }

        public String getMccMnc() {
            if (QtiPrimaryCardUtils.setPrimaryCardOnDeAct() && mProvisionState
                    == QtiUiccCardProvisioner.UiccProvisionStatus.NOT_PROVISIONED) {
                return null;
            } else {
                return mMccMnc;
            }
        }

        public int getProvisionState() {
            return mProvisionState;
        }

        public boolean isCardTypeSame(String cardType) {
            if (QtiPrimaryCardUtils.setPrimaryCardOnDeAct() && mProvisionState
                    == QtiUiccCardProvisioner.UiccProvisionStatus.NOT_PROVISIONED) {
                return false;
            } else {
                return CardType.valueOf(cardType).equals(mCardType);
            }
        }

        public boolean isCardInfoAvailable(int slotId) {
            boolean isAvailable = !(mUpdateCardTypeState == UPDATE_CARDTYPE_INIT
                    || mUpdateCardTypeState == UPDATE_CARDTYPE_IN_PROGRESS);

            if (SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled() &&
                    !SubsidyLockSettingsObserver.isPermanentlyUnlocked(mContext)) {
                UiccCard uiccCard = UiccController.getInstance().getUiccCard(slotId);

                if ((mProvisionState == QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED) &&
                        ((uiccCard != null) &&
                        uiccCard.getApplication(UiccController.APP_FAM_3GPP) != null)) {

                    UiccCardApplication app = uiccCard.getApplication(UiccController.APP_FAM_3GPP);
                    // Do not wait for MCC-MNC info if the CARD is PERSO LOCKED.
                    if (app != null && !app.isPersoLocked()) {
                        isAvailable &= mMCCMNCLoaded;
                    }
                    // in case if mcc-mnc not loaded reset the existing values
                    if (!mMCCMNCLoaded) {
                        mMccMnc = null;
                    }
                }
            }
            return isAvailable;
        }
    }

    private CardInfo[] mCardInfos = new CardInfo[PHONE_COUNT];

    static QtiCardInfoManager init(Context context, CommandsInterface[] ci) {
        synchronized (QtiCardInfoManager.class) {
            if (sInstance == null) {
                sInstance = new QtiCardInfoManager(context, ci);
            }
        }
        return sInstance;
    }

    public static QtiCardInfoManager getInstance() {
        synchronized (QtiCardInfoManager.class) {
            if (sInstance == null) {
                throw new RuntimeException("QtiCardInfoManager was not initialized!");
            }
            return sInstance;
        }
    }

    //******Constructor********
    private QtiCardInfoManager(Context context, CommandsInterface[] ci){
        mContext = context;
        for (int index = 0; index < PHONE_COUNT; index++) {
            mCardInfos[index] = new CardInfo();
            ci[index].registerForAvailable(this, EVENT_MANUAL_PROVISION_STATE_CHANGED,
                    new Integer(index));
        }
        mQtiCardProvisioner = QtiUiccCardProvisioner.getInstance();
        mQtiCardProvisioner.registerForManualProvisionChanged(
                this, EVENT_MANUAL_PROVISION_STATE_CHANGED, null);
        UiccController.getInstance().registerForIccChanged(this, EVENT_ICC_CHANGED, null);

        QtiSubscriptionController.getInstance().registerForAddSubscriptionRecord(
                this, EVENT_SUBINFO_RECORD_ADDED, null);
        if (SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled()) {
            IntentFilter filter = new IntentFilter();
            filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
            context.registerReceiver(receiver, filter);
        }
    }

    private BroadcastReceiver receiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
             if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(intent.getAction())) {
                final int slotId = intent.getIntExtra(PhoneConstants.SLOT_KEY, 0);
                final String stateExtra = intent
                        .getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                logd(" SIM_STATE_CHANGED intent received state is " + stateExtra
                        + " slotId + " + slotId);

                if (!SubscriptionManager.isValidSlotIndex(slotId)) {
                    return;
                }

                if (IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(stateExtra)) {
                    if (!mCardInfos[slotId].mMCCMNCLoaded) {
                        mCardInfos[slotId].mMCCMNCLoaded = true;
                        updateCardInfo(slotId);
                    }
                } else {
                    mCardInfos[slotId].mMCCMNCLoaded = false;
                }
            }
        }
    };

    public void registerAllCardsInfoAvailable(Handler handler, int what, Object obj) {
        Registrant r = new Registrant(handler, what, obj);
        synchronized (mAllCardsInfoAvailableRegistrants) {
            mAllCardsInfoAvailableRegistrants.add(r);
            for (int index = 0; index < PHONE_COUNT; index++) {
                if (!mCardInfos[index].isCardInfoAvailable(index)) {
                    return;
                }
            }
            r.notifyRegistrant();
        }
    }

    public void unregisterAllCardsInfoAvailable(Handler handler) {
        synchronized (mAllCardsInfoAvailableRegistrants) {
            mAllCardsInfoAvailableRegistrants.remove(handler);
        }
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case EVENT_MANUAL_PROVISION_STATE_CHANGED:
                logd("on EVENT_MANUAL_PROVISION_STATE_CHANGED");
                onManualProvisionStateChanged((AsyncResult) msg.obj);
                break;
            case EVENT_READ_EF_HPLMNWACT_DONE:
                logd("on EVENT_READ_EF_HPLMNWACT_DONE");
                onEfLoaded((AsyncResult) msg.obj);
                break;
            case EVENT_ICC_CHANGED:
                logd("on EVENT_ICC_CHANGED");
                onIccChanged((AsyncResult) msg.obj);
                break;
            case EVENT_SUBINFO_RECORD_ADDED:
                logd("on EVENT_SUBINFO_RECORD_ADDED");
                onSubscriptionInfoChanged((AsyncResult) msg.obj);
                break;
        }
    }

    private void onSubscriptionInfoChanged(AsyncResult subInfoChange) {
        if (subInfoChange != null && subInfoChange.result != null) {
            int slotId = (Integer) subInfoChange.result;
            mCardInfos[slotId].reset();
            updateCardInfo((Integer) subInfoChange.result);
        }
    }

    private void onIccChanged(AsyncResult iccChangedResult) {
        if (iccChangedResult != null && iccChangedResult.result != null) {
            updateCardInfo((Integer) iccChangedResult.result);
        }
    }

    private void onManualProvisionStateChanged(AsyncResult manualProvisionResult) {
        if (manualProvisionResult == null || manualProvisionResult.result == null) {
            //if event is not clear, better to update card info on both the slots.
            for (int index = 0; index < PHONE_COUNT; index++) {
                updateCardInfo(index);
            }
        } else {
            updateCardInfo((Integer) manualProvisionResult.result);
        }
    }

    private void updateCardInfo(int slotId) {
        int currProvState = mQtiCardProvisioner.getCurrentUiccCardProvisioningStatus(slotId);
        String currIccId = mQtiCardProvisioner.getUiccIccId(slotId);
        String currMccMnc = null;
        logd("updateCardInfo[" + slotId + "]: Start!");

        //In case of shutdown, SSR and SIM power down in APM, treat the phone state as invalid
        // and ignore the event and on next event proper values is queried again.
        if (!QtiSubscriptionController.getInstance().isRadioInValidState()) {
            loge("updateCardInfo[" + slotId + "]: Radio is in Invalid State, IGNORE!!!");
            return;
        }

        // If the SIM is provisioned and a valid application not available, we cannot decide the
        // card type in such cases do not proceed further. Once SIM application available
        // we would get notified via onIccChanged() event.
        if ((QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED == currProvState)) {
            UiccCard uiccCard = UiccController.getInstance().getUiccCard(slotId);

            if ((uiccCard == null) ||
                    uiccCard.getApplication(UiccController.APP_FAM_3GPP) == null) {
                loge("updateCardInfo[" + slotId + "]: card not READY!! ");
                return;
            }

            SubscriptionController subCtrlr = SubscriptionController.getInstance();
            int subId = subCtrlr.getSubIdUsingPhoneId(slotId);
            if (!subCtrlr.isActiveSubId(subId)) {
                loge("updateCardInfo[" + slotId + "]: subId not added yet!! ");
                return;
            }

            // when subsidy-feature enabled and device in subsidy state wait
            // until vaid mcc-mnc values are available.
            if (SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled()) {
                SubscriptionInfo sir = SubscriptionManager.from(mContext)
                        .getActiveSubscriptionInfoForSimSlotIndex(slotId);
                if (sir != null) {
                    currMccMnc = "" + sir.getMcc() + sir.getMnc();
                }
            }
        }

        /*If provision state is
        *  1. Invalid, then reset the cardInfo values and query on next valid state.
        *  2. Card not present, then initialize to default values, update prov state and send
        *     notification.
        *  3. In other state update the current info and send notification.
        */
        if (currProvState == QtiUiccCardProvisioner.UiccProvisionStatus.INVALID_STATE) {
            //reset the cardInfo
            mCardInfos[slotId].reset();
            logd("updateCardInfo[" + slotId + "]: ProvStatus is Invalid, reset cardInfo!");
        } else {
            if (isUpdateCardInfoRequired(slotId, currIccId, currProvState, currMccMnc)) {
                if (currProvState == QtiUiccCardProvisioner.UiccProvisionStatus.CARD_NOT_PRESENT) {
                    //CARD ABSENT case, reset the cardInfo
                    mCardInfos[slotId].reset();
                    mCardInfos[slotId].mProvisionState = currProvState;
                    mCardInfos[slotId].mUpdateCardTypeState = UPDATE_CARDTYPE_NOT_NEEDED;
                    logd("updateCardInfo[" + slotId + "]: CardAbsent!!!");
                    notifyAllCardsInfoAvailableIfNeeded();
                } else {
                    //PROVISIONED/NOT_PROVISIONED case, fill card info
                    mCardInfos[slotId].reset();
                    logd("updateCardInfo[" + slotId + "]: Query current state is required!");
                    mCardInfos[slotId].mIccId = currIccId;
                    mCardInfos[slotId].mMccMnc = currMccMnc;
                    mCardInfos[slotId].mProvisionState = currProvState;
                    if (updateUiccCardType(slotId)) {
                        mCardInfos[slotId].mUpdateCardTypeState = UPDATE_CARDTYPE_COMPLETED;
                        notifyAllCardsInfoAvailableIfNeeded();
                    }
                }
            }
        }

        logi("updateCardInfo[" + slotId + "]: Exit! - UpdateCardTypeState: " + mCardInfos[slotId]
            .mUpdateCardTypeState + ", mCardType: " + mCardInfos[slotId].mCardType);
    }


    private boolean isSubsidyRestricted() {
        return SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled() &&
                !SubsidyLockSettingsObserver.isPermanentlyUnlocked(mContext);
    }

    private boolean isUpdateCardInfoRequired(int slotId, String currIccId,
            int currProvState, String currMccmnc) {
        //If cached and current values of iccid, mccMnc and prov state are not same,
        // then update CardInfo.
        //or if update Card type state is INIT.
        if (!TextUtils.equals(currIccId, mCardInfos[slotId].mIccId) ||
            (isSubsidyRestricted() && currMccmnc != null &&
            !TextUtils.equals(currMccmnc, mCardInfos[slotId].mMccMnc)) ||
            currProvState != mCardInfos[slotId].mProvisionState ||
            mCardInfos[slotId].mUpdateCardTypeState == UPDATE_CARDTYPE_INIT ||
            (mCardInfos[slotId].mCardType == CardType.UNKNOWN &&
            (mCardInfos[slotId].mUpdateCardTypeState != UPDATE_CARDTYPE_NOT_NEEDED))) {
            return true;
        }
        return false;
    }

    /*
    * updateUiccCardType: Method used to update the card type in local cache.
    *
    * Description:
    *    1. If USIM is absent mark cardType as CARDTYPE_2G.
    *    2. If USIM is present and read4gEF flag is disabled mark cardType as CARDTYPE_3G
    *    3. If USIM is present and read4gEF flag is enabled, mark UpdateCardTypeState as
    *       UPDATE_CARDTYPE_IN_PROGRESS, cardType as CARDTYPE_3G and proceed further to send
    *       request to RIL SIM I/O to read the EF_HPLMNWACT file.
    *
    * Returns: TRUE if update completed,
    *          FALSE if UPDATE_CARDTYPE_IN_PROGRESS.
    */
    private boolean updateUiccCardType(int slotId) {
        try {
            UiccCard uiccCard = UiccController.getInstance().getUiccCard(slotId);
            UiccCardApplication app = uiccCard.getApplication(UiccController.APP_FAM_3GPP);

            if (app.getType() != AppType.APPTYPE_USIM) {
                mCardInfos[slotId].mCardType = CardType.CARDTYPE_2G;
            } else {
                boolean read4gEf = QtiPrimaryCardUtils.read4gFlag();
                mCardInfos[slotId].mCardType = CardType.CARDTYPE_3G;
                if (read4gEf) {
                    IccFileHandler iccFh = app.getIccFileHandler();
                    //check 4g flag only if the card is provisioned.
                    if (mCardInfos[slotId].mProvisionState ==
                            QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED) {
                        mCardInfos[slotId].mUpdateCardTypeState = UPDATE_CARDTYPE_IN_PROGRESS;
                        iccFh.loadEFTransparent(IccConstants.EF_HPLMN_W_ACT,
                                    obtainMessage(EVENT_READ_EF_HPLMNWACT_DONE, slotId));
                        return false;
                    }
                }
            }
        } catch (Exception e) {
            // In case of UiccCard or UiccCardApp is null or any exception, ignore update
            // so that on next event proper values is updated again.
            loge("For slot " + slotId + " Exception while updateUiccCardType " + e.getMessage());
        }
        return true;
    }

    /*
    * onEfLoaded: Method used to read the contents of EF_HPLMNWACT file.
    *
    * Description: EF_HPLMNWACT file can contains n records and each record length is 5 bytes.
    *    Contents of each record is as follows:
    *        1 to 3  nth PLMN (highest priority)
    *        4 to 5  nth PLMN Access Technology Identifier
    *        7th bit in PLMN Access Technology Identifier is eUTRAN
    *    If eUTRAN bit is set mark cardType as CARDTYPE_4G.
    */
    private void onEfLoaded(AsyncResult ar) {
        int slotId = (Integer) ar.userObj;
        logd("onEfLoaded: Started");

        if (ar.exception != null) {
            logd("EF_HPLMNWACT read with exception = " + ar.exception);
        } else {
            byte[] data = (byte[]) ar.result;
            logd("result=" + IccUtils.bytesToHexString(data));
            int numRec = data.length / HPLMN_SEL_DATA_LEN;
            logd("number of Records=" + numRec);

            for (int i = 0; i < numRec; i++) {
                if ((data[i * HPLMN_SEL_DATA_LEN + 3] & 0x40) != 0) {
                    mCardInfos[slotId].mCardType = CardType.CARDTYPE_4G;
                    break;
                }
            }
        }
        mCardInfos[slotId].mUpdateCardTypeState = UPDATE_CARDTYPE_COMPLETED;
        notifyAllCardsInfoAvailableIfNeeded();
        logd("onEfLoaded(" + slotId + ") : mCardType = " + mCardInfos[slotId].mCardType);
    }

    /*
    * notifyAllCardInfoAvailableIfNeeded: Method used to notify registrants that all cards info
    * is available.
    *
    * Description:
    *     1. Card is ABSENT
    *     2. Card is PRESENT and iccid is not null and updateCardTypeState is completed.
    */
    private void notifyAllCardsInfoAvailableIfNeeded() {
        for (int index = 0; index < PHONE_COUNT; index++) {
            if (!mCardInfos[index].isCardInfoAvailable(index)) {
                logd(" card info not available " + index);
                return;
            }
        }
        mAllCardsInfoAvailableRegistrants.notifyRegistrants();
    }

    public CardInfo getCardInfo(int slotId) {
        return mCardInfos[slotId];
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


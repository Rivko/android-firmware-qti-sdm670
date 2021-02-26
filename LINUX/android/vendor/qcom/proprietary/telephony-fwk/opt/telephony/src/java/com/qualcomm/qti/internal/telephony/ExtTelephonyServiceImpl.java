/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import org.codeaurora.internal.IDepersoResCallback;
import org.codeaurora.internal.IDsda;
import org.codeaurora.internal.IExtTelephony;
import com.android.internal.telephony.IccCard;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.uicc.UiccController;
import com.android.internal.telephony.uicc.UiccCard;
import com.qualcomm.qti.internal.telephony.primarycard.QtiPrimaryCardController;
import com.qualcomm.qti.internal.telephony.primarycard.QtiPrimaryCardUtils;
import java.util.List;
/**
 * This class implements IExtTelephony aidl interface.
 */
public class ExtTelephonyServiceImpl extends IExtTelephony.Stub {
    private static final String LOG_TAG = "ExtTelephonyServiceImpl";
    private static final boolean DBG = true;

    private static final String TELEPHONY_SERVICE_NAME = "extphone";
    private static Context mContext;
    private static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";

    private static ExtTelephonyServiceImpl sInstance = null;

    private QtiSmscHelper mQtiSmscHelper;

    public static ExtTelephonyServiceImpl init(Context context) {
        synchronized (ExtTelephonyServiceImpl.class) {
            mContext = context;
            if (sInstance == null) {
                sInstance = new ExtTelephonyServiceImpl();
            } else {
                Log.wtf(LOG_TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static ExtTelephonyServiceImpl getInstance() {
        if (sInstance == null) {
            Log.wtf(LOG_TAG, "getInstance null");
        }
        return sInstance;
    }

    private ExtTelephonyServiceImpl() {
        if (DBG) logd("init constructor ");

        if (ServiceManager.getService(TELEPHONY_SERVICE_NAME) == null) {
            ServiceManager.addService(TELEPHONY_SERVICE_NAME, this);
        }
        mQtiSmscHelper = new QtiSmscHelper();
    }

    @Override
    public int getCurrentUiccCardProvisioningStatus(int slotId) {
        return QtiUiccCardProvisioner.getInstance()
                .getCurrentUiccCardProvisioningStatus(slotId);
    }

    @Override
    public int getUiccCardProvisioningUserPreference(int slotId) {
        return QtiUiccCardProvisioner.getInstance()
                .getUiccCardProvisioningUserPreference(slotId);
    }

    @Override
    public int activateUiccCard(int slotId) {
        return QtiUiccCardProvisioner.getInstance().activateUiccCard(slotId);
    }

    @Override
    public int deactivateUiccCard(int slotId) {
        return QtiUiccCardProvisioner.getInstance().deactivateUiccCard(slotId);
    }

    @Override
    public boolean isSMSPromptEnabled() {
        if (QtiSubscriptionController.getInstance() == null) {
            Log.wtf(LOG_TAG, "QtiSubscriptionController getInstance is null");
        }
        return QtiSubscriptionController.getInstance().isSMSPromptEnabled();
    }

    @Override
    public void setSMSPromptEnabled(boolean enabled) {
        if (QtiSubscriptionController.getInstance() == null) {
            Log.wtf(LOG_TAG, "QtiSubscriptionController getInstance is null");
        }
        QtiSubscriptionController.getInstance().setSMSPromptEnabled(enabled);
    }

    @Override
    public int getPhoneIdForECall() {
        return QtiEmergencyCallHelper.getPhoneIdForECall();
    }

    @Override
    public int getPrimaryStackPhoneId() {
        return QtiEmergencyCallHelper.getPrimaryStackPhoneId();
    }

    private IDsda mDsda = null;
    public void setDsdaAdapter(IDsda a) {
        if (DBG) logd("setDsdaAdapter:" + a);
        mDsda = a;
    }

    public void switchToActiveSub(int sub) {
        if (DBG) logd("switchToActiveSub:" + sub + " mDsda:" + mDsda);
        try {
            mDsda.switchToActiveSub(sub);
        } catch (RemoteException ex) {
            if (DBG) logd("switchToActiveSub:" + ex);
        }
    }

    public int getActiveSubscription() {
        if (DBG) logd("getActiveSubscription mDsda:" + mDsda);
        try {
            return mDsda.getActiveSubscription();
        } catch (RemoteException ex) {
            if (DBG) logd("getActiveSubscription:" + ex);
        }
        return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
    }
    @Override
    public boolean isFdnEnabled() {
        IccCard card = PhoneFactory.getDefaultPhone().getIccCard();
        if (card != null) {
            return card.getIccFdnEnabled();
        }
        return false;
    }

    public int getUiccApplicationCount(int slotId) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        int count = 0;
        if (card != null) {
            count = card.getNumApplications();
        }
        return count;
    }

    public void
    supplyIccDepersonalization(String netpin, String type, IDepersoResCallback callback,
            int phoneId) {
        logd("supplyIccDepersonalization");
        QtiDepersoSupplier.getInstance().
            supplyIccDepersonalization(netpin, type, callback, phoneId);
    }

    /**
     * Returns the application type as Integer.
     * Supported return values:
     * '0' - APPTYPE_UNKNOWN
     * '1' - APPTYPE_SIM
     * '2' - APPTYPE_USIM
     * '3' - APPTYPE_RUIM
     * '4' - APPTYPE_CSIM
     * '5' - APPTYPE_ISIM
     */
    public int getUiccApplicationType(int slotId, int appIndex) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        int appType = 0;  //AppType.APPTYPE_UNKNOWN
        if (card != null) {
            appType = card.getApplicationIndex(appIndex).getType().ordinal();
        }
        return appType;
    }

    /**
     * Returns the application type as Integer.
     * Supported return values:
     * '0' - APPSTATE_UNKNOWN
     * '1' - APPSTATE_DETECTED
     * '2' - APPSTATE_PIN
     * '3  - APPSTATE_PUK
     * '4' - APPSTATE_SUBSCRIPTION_PERSO
     * '5' - APPSTATE_READY
     */
    public int getUiccApplicationState(int slotId, int appIndex) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        int appState = 0;  //AppState.APPSTATE_UNKNOWN
        if (card != null) {
            appState = card.getApplicationIndex(appIndex).getState().ordinal();
        }
        return appState;
    }

    @Override
    public void setPrimaryCardOnSlot(int slotId) {
        QtiPrimaryCardController.getInstance().setPrimaryCardOnSlot(slotId);
    }

    @Override
    public int getCurrentPrimaryCardSlotId() {
        return QtiPrimaryCardUtils.getCurrentPrimarySlotFromDB(mContext);
    }

    public boolean isEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isEmergencyNumber(number);
    }

    @Override
    public boolean isLocalEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isLocalEmergencyNumber(mContext, number);
    }

    @Override
    public boolean isPotentialEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isPotentialEmergencyNumber(number);
    }

    @Override
    public boolean isPotentialLocalEmergencyNumber(String number) {
        return QtiEmergencyCallHelper.isPotentialLocalEmergencyNumber(mContext, number);
    }

    @Override
    public boolean isDeviceInSingleStandby() {
        return QtiEmergencyCallHelper.isDeviceInSingleStandby();
    }

    public boolean setLocalCallHold(int subscriptionId, boolean enable) {
        int phoneId = SubscriptionManager.getPhoneId(subscriptionId);
        Phone phone = PhoneFactory.getPhone(phoneId);
        if (DBG) logd("setLocalCallHold:" + phoneId + " enable:" + enable);
        return ((QtiGsmCdmaPhone) phone).setLocalCallHold(enable);
    }

    public boolean isDsdaEnabled() {
        return (TelephonyManager.getDefault().getMultiSimConfiguration()
                == TelephonyManager.MultiSimVariants.DSDA);
    }

    /** This method returns
    * -1  - When there is no SIM present on device.
    * -1  - When none of the inserted SIM card belongs to PrimaryCarrier
    * 0/1 - When one of the inserted SIM matches with PrimaryCarrier,
    *       this returns corresponding slotId.
    * PrimarySlotId - If both the SIM cards belongs to PrimaryCarrier
                     this would return primary slotId.
    */
    @Override
    public int getPrimaryCarrierSlotId() {
        int slotId = -1;
        List<SubscriptionInfo> subInfoList =
                SubscriptionManager.from(mContext).getActiveSubscriptionInfoList();
        int matchingCount = 0;

        if (subInfoList == null || subInfoList.size() < 1) {
            loge("No active subscriptions found!!");
            return slotId;
        }

        for (SubscriptionInfo subInfo : subInfoList) {
            String mccMnc = String.valueOf(subInfo.getMcc()) + String.valueOf(subInfo.getMnc());
            int provisionStatus = getCurrentUiccCardProvisioningStatus(subInfo.getSimSlotIndex());
            logd("provisionStatus : " + provisionStatus + " slotId " + subInfo.getSimSlotIndex());

            if ((provisionStatus == QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED) &&
                    isPrimaryCarrierMccMnc(mccMnc)) {
                logd("Found a matching combination, slotId  " + subInfo.getSimSlotIndex());
                slotId = subInfo.getSimSlotIndex();
                matchingCount++;
            }
        }

        // If both SIM cards belongs to PrimaryCarrier return primary slotId
        if (matchingCount > 1) {
            logd("Found multiple matches, returning primary slotid");
            slotId = Settings.Global.getInt(mContext.getContentResolver(),
                    CONFIG_CURRENT_PRIMARY_SUB, slotId);
        }

        return slotId;
    }

    // If input mcc-mnc matching with any of the PrimaryCarrier mcc-mnc
    // list this method returns true.
    // FIXME Explore the possibility of moving the list of mcc-mnc to a xml/config file
    private boolean isPrimaryCarrierMccMnc(String mccMnc) {
        String[] mccMncList = {"405840", "405854", "405855", "405856", "405857", "405858",
                "405859", "405860", "405861", "405862", "405863", "405864", "405865",
                "405866", "405867", "405868", "405869", "405870", "405871", "405872",
                "405873", "405874", "22201", "2221"};

        for (String mccmnc : mccMncList) {
            if(mccMnc.equals(mccmnc)) {
                logd("Found a matching combination  " + mccMnc);
                return true;
            }
        }
        logd("Not found a matching combination  " + mccMnc);
        return false;
    }

    /**
    * Check if slotId has PrimaryCarrier SIM card present or not.
    * @param - slotId
    * @return true or false
    */
    @Override
    public boolean isPrimaryCarrierSlotId(int slotId) {
        SubscriptionInfo subInfo = SubscriptionManager
                .from(mContext).getActiveSubscriptionInfoForSimSlotIndex(slotId);

        if (subInfo == null) {
            loge("No active subscription found on slot " + slotId);
            return false;
        }

        String mccMnc = String.valueOf(subInfo.getMcc()) + String.valueOf(subInfo.getMnc());

        if (isPrimaryCarrierMccMnc(mccMnc)) {
            logd("Found a matching combination, slotId  " + subInfo.getSimSlotIndex());
            return true;
        }
        return false;
    }

    @Override
    public boolean setSmscAddress(int slotId, String smsc) {
        return mQtiSmscHelper.setSmscAddress(slotId, smsc);
    }

    @Override
    public String getSmscAddress(int slotId) {
        return mQtiSmscHelper.getSmscAddress(slotId);
    }

    @Override
    public boolean isVendorApkAvailable(String packageName) {
        // check whether the target handler exist in system
        PackageManager pm = mContext.getPackageManager();
        boolean isApkAvailable = false;
        try {
            pm.getPackageInfo(packageName, 0);
            isApkAvailable = true;
        } catch (NameNotFoundException e) {
            logd("Vendor apk not available for " + packageName);
        }
        return isApkAvailable;
    }

    private void logd(String string) {
        Rlog.d(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}

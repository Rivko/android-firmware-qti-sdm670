/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.primarycard;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.util.XmlUtils;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.regex.Pattern;
import org.xmlpull.v1.XmlPullParser;
/**
 * This class has logic to retrieve Primary card policy
 *   1. Retrive Data from xml based on properties and store it in PriorityConfig Class.
 *
 *   2. Decides primary slot based on the cardInfo of inserted SIMs.
 */
public class QtiPrimaryCardPriorityHandler {
    private static final String LOG_TAG = "QtiPcPriorityHandler";
    private static final boolean VDBG = false;
    private static final boolean DBG = true;

    private final Context mContext;
    private static String packageName = "com.qualcomm.qti.simsettings";
    private int mPriorityCount = 0;
    private int mPrefPrimarySlot = QtiPrimaryCardUtils.SLOT_INVALID;
    private boolean mLoadingConfigCompleted = false;
    private boolean mLoadingCurrentConfigsDone = false;
    private static final int INVALID_PRIORITY = -1;
    private static final int INVALID_NETWORK = -1;

    //Hash map to store all priority configs as defined in appropriate operator config xml.
    private HashMap<Integer, PriorityConfig> mAllPriorityConfigs =
            new HashMap<Integer, PriorityConfig>();

    //Card priority config for each slot
    private PriorityConfig[] mCurrPriorityConfigs = null;

    static class PriorityConfig {
        int priority = INVALID_PRIORITY;
        Pattern pattern;
        String cardType;
        String mccmnc;
        int network1 = INVALID_NETWORK;
        int network2 = INVALID_NETWORK;

        @Override
        public String toString() {
            return "PriorityConfig: [priority = " + priority + ", pattern = "+ pattern +
                ", cardType = " + cardType + ", mccmnc = "+ mccmnc + ", network1 = " + network1 +
                ", network2 = "+ network2 +"]";
        }
    }

    QtiPrimaryCardPriorityHandler(Context context){
        mContext = context;

        mCurrPriorityConfigs = new PriorityConfig[QtiPrimaryCardUtils.PHONE_COUNT];

        readPriorityConfigFromXml();
    }

    public int[] getNwModesFromConfig(int primarySlotId) {
        int defaultNwMode = QtiPrimaryCardUtils.getDefaultNwMode();
        int[] prefNwModes = new int[QtiPrimaryCardUtils.PHONE_COUNT];

        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            if (mCurrPriorityConfigs[i] != null) {
                prefNwModes[i] = (i == primarySlotId) ? mCurrPriorityConfigs[i].network1 :
                        mCurrPriorityConfigs[i].network2;
            } else {
                prefNwModes[i] = defaultNwMode;
            }
        }

        if(getNumSlotsWithCdma(prefNwModes) > 1) {
            //If more than one slot has CDMA nwMode then set non-primary card nwModes to default
            logd("getNwModesFromConfig: More than one slot has CDMA nwMode set non-primary card" +
                    " nwModes to default nwMode");
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                logi("getNwModesFromConfig: nwMode from config on slot [" + i + "] is:"
                        + prefNwModes[i]);
                if (i != primarySlotId) prefNwModes[i] = QtiPrimaryCardUtils.getDefaultNwMode();
            }
        }

        return prefNwModes;
    }

    private int getNumSlotsWithCdma(int[] prefNwModes) {
        int numSlotsWithCdma = 0;
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            if (QtiPrimaryCardUtils.is3gpp2NwMode(prefNwModes[i])) numSlotsWithCdma++;
        }
        return numSlotsWithCdma;
    }

    public boolean isConfigLoadDone() {
        return mLoadingCurrentConfigsDone;
    }

    public void loadCurrentPriorityConfigs(boolean override) {
        if(!mLoadingConfigCompleted) {
            logd("getPrefPrimarySlot: All Config Loading not done. EXIT!!!");
            return;
        }

        if(override || !mLoadingCurrentConfigsDone) {
            //get Current Priority config applicable for each slot
            for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
                mCurrPriorityConfigs[i] = getPriorityConfig(i);
            }
            mLoadingCurrentConfigsDone = true;
        }
    }

    public int getPrefPrimarySlot() {
        //Reset the Pref Primary slot to invalid to calculate primary card below.
        mPrefPrimarySlot = QtiPrimaryCardUtils.SLOT_INVALID;
        logd("getPrefPrimarySlot:  Start!!!");

        if(!mLoadingCurrentConfigsDone) {
            logd("getPrefPrimarySlot: Current Config Loading not done. EXIT!!!");
            return mPrefPrimarySlot;
        }

        if (areConfigPrioritiesEqual()) {
            mPrefPrimarySlot = QtiPrimaryCardUtils.SLOT_PRIORITY_MATCH;
        } else {
            mPrefPrimarySlot = getMaxPrioritySlot();
        }

        logd("getPrefPrimarySlot: return mPrefPrimarySlot: " + mPrefPrimarySlot);

        return mPrefPrimarySlot;
    }

    private int getMaxPrioritySlot() {
        int slotId = QtiPrimaryCardUtils.SLOT_INVALID;
        int tempMaxPriority = INVALID_PRIORITY;

        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            if (mCurrPriorityConfigs[i] != null && tempMaxPriority <
                    mCurrPriorityConfigs[i].priority) {
                slotId = i;
                tempMaxPriority = mCurrPriorityConfigs[i].priority;
            }
        }
        logd("maxPriority: " + tempMaxPriority + ", maxPrioritySlot:" + slotId);

        return slotId;
    }

    private boolean areConfigPrioritiesEqual() {
        for (int i = 0; i < QtiPrimaryCardUtils.PHONE_COUNT; i++) {
            if (mCurrPriorityConfigs[i] == null ||
                    mCurrPriorityConfigs[i].priority != mCurrPriorityConfigs[0].priority) {
                return false;
            }
        }
        return true;
    }

    private PriorityConfig getPriorityConfig(int slotId) {
        int priorityConfigComparator = QtiPrimaryCardUtils.getPriorityConfigComparator();
        QtiCardInfoManager.CardInfo cardInfo = QtiCardInfoManager.getInstance()
                .getCardInfo(slotId);

        if (cardInfo.getIccId() == null) {
            //if iccid is null then card is not present, no need to get config for it.
            logd("getPriorityConfig: for slot:" + slotId + ": iccid is null, EXIT!!!");
            return null;
        }

        logd("getPriorityConfig: for slot:" + slotId + " mcc-mnc "
                + cardInfo.getMccMnc() + ", Start!!!");

        try {
            for (int i = 0; i < mPriorityCount; i++) {
                PriorityConfig pConfig = mAllPriorityConfigs.get(i);
                switch (priorityConfigComparator) {
                    case QtiPrimaryCardUtils.COMPARE_IIN_CARDTYPE: {
                        if (pConfig.pattern.matcher(cardInfo.getIccId()).find() &&
                                cardInfo.isCardTypeSame(pConfig.cardType)) {
                            logd("getPriorityConfig: Found for slot:" + slotId + ", " + pConfig);
                            return pConfig;
                        }
                        break;
                    }
                    case QtiPrimaryCardUtils.COMPARE_CARDTYPE: {
                        if (cardInfo.isCardTypeSame(pConfig.cardType)) {
                            logd("getPriorityConfig: Found for slot:" + slotId + ", " + pConfig);
                            return pConfig;
                        }
                        break;
                    }
                    case QtiPrimaryCardUtils.COMPARE_MCCMNC:
                        if (cardInfo.getMccMnc() != null
                                && pConfig.pattern.matcher(cardInfo.getMccMnc()).find() &&
                                cardInfo.isCardTypeSame(pConfig.cardType)) {
                            loge("getPriorityConfig: Found for slot:" + slotId + ", " + pConfig);
                            return pConfig;
                        }
                        break;
                    default:
                        break;
                }
            }
        } catch (Exception e) {
            Rlog.e(LOG_TAG, "getPriorityConfig:Exception:[" + slotId + "] " + e.getMessage(), e);
        }
        return null;
    }

    public void reloadPriorityConfig() {
        QtiPrimaryCardUtils.setConfigValue();
        readPriorityConfigFromXml();
    }

    private void readPriorityConfigFromXml() {
        XmlResourceParser parser = null;
        try {
            Resources res = mContext.getPackageManager().getResourcesForApplication(packageName);
            if (res == null) loge("res is null");
            int resId = res.getIdentifier(QtiPrimaryCardUtils.getConfigXml(), "xml", packageName);
            parser = res.getXml(resId);
            //Clear all old configs from hash map.
            mAllPriorityConfigs.clear();
            mPriorityCount = 0;

            XmlUtils.beginDocument(parser, "priority_config");
            //2TODO: here we can read the operator config and feed it to pcutils
            XmlUtils.nextElement(parser);
            while (parser.getEventType() != XmlPullParser.END_DOCUMENT) {
                savePriorityConfig(parser);
                XmlUtils.nextElement(parser);
            }
            mLoadingConfigCompleted = true;
       } catch (Exception e) {
            Rlog.e(LOG_TAG, "Exception while reading priority configs: " + e.getMessage(), e);
            mLoadingConfigCompleted = false;
       } finally {
            logi("mAllPriorityConfigs: " + mAllPriorityConfigs);
            if (parser != null) {
                parser.close();
            }
       }
    }

    private void savePriorityConfig(XmlResourceParser parser) throws Exception {
        PriorityConfig pConfig = new PriorityConfig();
        //Priority, network1 and network2 are mandatory so if they are not present in xml config
        // throw an exception. Pattern is optional, so if its not present set pattern to null.
        pConfig.priority = Integer.parseInt(parser.getAttributeValue(null, "priority"));
        pConfig.pattern = getPattern(parser);
        pConfig.cardType = parser.getAttributeValue(null, "card_type");
        pConfig.mccmnc = parser.getAttributeValue(null, "mccmnc");
        pConfig.network1 = Integer.parseInt(parser.getAttributeValue(null, "network1"));
        pConfig.network2 = Integer.parseInt(parser.getAttributeValue(null, "network2"));
        mAllPriorityConfigs.put(mPriorityCount, pConfig);
        logd("Added to mAllPriorityConfigs" + "[" + mPriorityCount + "], " + pConfig);
        mPriorityCount++;
    }

    private Pattern getPattern(XmlResourceParser parser) throws Exception {
        String regEx = parser.getAttributeValue(null, "iin_pattern");
        if (regEx != null) {
            return Pattern.compile(regEx);
        }
        return null;
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

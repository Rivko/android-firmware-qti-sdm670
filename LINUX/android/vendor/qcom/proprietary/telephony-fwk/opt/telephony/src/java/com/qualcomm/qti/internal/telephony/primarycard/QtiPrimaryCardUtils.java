/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.primarycard;

import android.content.Context;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.telephony.RadioAccessFamily;
import com.qualcomm.qti.internal.telephony.QtiRilInterface;

import com.android.internal.telephony.Phone;

/**
 * This class provides utility methods for other primary card classes.
 */
public class QtiPrimaryCardUtils {
    private static final String LOG_TAG = "QtiPcUtils";
    private static final boolean VDBG = true;
    private static final boolean DBG = true;

    //Config bit values
    public enum ConfigBits{
        RESERVED_1(0x1),                                //xxxx xxxx xxxx xxx1
        READ_4G_FLAG(0x2),                              //xxxx xxxx xxxx xx1x
        SET_PRIMARY_ON_DEACT(0x4),                      //xxxx xxxx xxxx x1xx
        DEFAULT_PRIMARY_SLOT_1(0x8),                    //xxxx xxxx xxxx 1xxx

        //Config for default nwMode,
        //length is 2 bits i.e. 4 possible values
        DEFAULT_NWMODE_GSM(0x10),                       //xxxx xxxx xx01 xxxx
        DEFAULT_NWMODE_GW(0x20),                        //xxxx xxxx xx10 xxxx
        DEFAULT_NWMODE_GCWTL(0x40),                     //xxxx xxxx x100 xxxx

        //Config for enable user selection,
        //length is 2 bits i.e. 4 possible values
        DISABLE_USER_SELECTION(0x40),                   //xxxx xxxx 01xx xxxx
        SHOW_USER_SELECTION_ON_PRIORITY_MATCH(0x80),    //xxxx xxxx 10xx xxxx
        SHOW_USER_SELECTION_FOR_EVERY_CHANGE(0xC0),     //xxxx xxxx 11xx xxxx

        //Configs for comparing priorityConfig,
        //length is 3 bits i.e. 8 possible values
        COMPARE_IIN_CARDTYPE(0x100),                    //xxxx x001 xxxx xxxx
        COMPARE_CARDTYPE(0x200),                        //xxxx x010 xxxx xxxx
        COMPARE_MCCMNC(0x300),                          //xxxx x011 xxxx xxxx

        //Config for Disabling DDS if nwMode on one of the card inserted is CDMA.
        DISABLE_DDS(0x800),                             //xxxx 1xxx xxxx xxxx

        //Config for priorityConfig xml to be used,
        //length is 4 bits i.e. 16 possible values
        PRIORITY_CONFIG_1(0x1000),                      //0001 xxxx xxxx xxxx
        PRIORITY_CONFIG_2(0x2000),                      //0010 xxxx xxxx xxxx
        PRIORITY_CONFIG_3(0x4000),                      //0100 xxxx xxxx xxxx
        PRIORITY_CONFIG_4(0x8000),                      //1000 xxxx xxxx xxxx
        PRIORITY_SUBSIDY_LOCKED_CONFIG(0xC000);         //1100 xxxx xxxx xxxx

        private int mValue;
        ConfigBits(int value) {
            this.mValue = value;
        }

        int value() {
            return mValue;
        }
    }


    //Constants for Slot Ids
    public static final int SLOT_PRIORITY_MATCH = -2;
    public static final int SLOT_INVALID = -1;
    public static final int SLOT_ID_0 = 0;
    public static final int SLOT_ID_1 = 1;

    public static final int PHONE_COUNT = TelephonyManager.getDefault().getPhoneCount();

    private static final String CONFIG_SUB_SELECT_MODE_MANUAL = "config_sub_select_mode_manual";
    private static final String CONFIG_PRIMARY_SUB_IS_SETABLE = "config_primary_sub_is_setable";
    private static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";
    private static final String CONFIG_DISABLE_DDS_PREFERENCE = "config_disable_dds_preference";

    private static final String PRIMARY_CARD_PROPERTY_NAME = "persist.radio.primarycard";
    private static final String DETECT_4G_CARD_PROPERTY_NAME = "persist.radio.detect4gcard";
    private static final String PRIMARY_CARD_7_5_PROPERTY_NAME = "persist.radio.primary_7_5_mode";
    private static final String PRIMARY_CARD_TYPE_2_PROPERTY_NAME = "persist.radio.primary_type_2";


    //parameter used for comparing and deciding priority.
    public static final int COMPARE_IIN_CARDTYPE = 1;
    public static final int COMPARE_CARDTYPE = 2;
    public static final int COMPARE_MCCMNC = 3;

    //parameter used for showing User selection.
    public static final int DISABLE_USER_SELECTION = 1;
    public static final int SHOW_USER_SELECTION_ON_PRIORITY_MATCH = 2;
    public static final int SHOW_USER_SELECTION_FOR_EVERY_CHANGE = 3;

    private static Context mContext;
    private static QtiPrimaryCardUtils sInstance;
    private static int mConfigValue = 0;

    private static final int RAF_CDMA = RadioAccessFamily.RAF_IS95A |
            RadioAccessFamily.RAF_IS95B |
            RadioAccessFamily.RAF_1xRTT;
    private static final int RAF_EVDO = RadioAccessFamily.RAF_EVDO_0 |
            RadioAccessFamily.RAF_EVDO_A |
            RadioAccessFamily.RAF_EVDO_B |
            RadioAccessFamily.RAF_EHRPD;
    private static final boolean PRIMCARYCARD_L_W_ENABLED =
            SystemProperties.getBoolean("persist.radio.lw_enabled", false);

    static QtiPrimaryCardUtils init(Context context) {
        synchronized (QtiPrimaryCardUtils.class) {
            if (sInstance == null) {
                sInstance = new QtiPrimaryCardUtils(context);
            }
        }
        return sInstance;
    }

    public static QtiPrimaryCardUtils getInstance() {
        synchronized (QtiPrimaryCardUtils.class) {
            if (sInstance == null) {
                throw new RuntimeException("QtiPrimaryCardUtils was not initialized!");
            }
            return sInstance;
        }
    }

    private QtiPrimaryCardUtils(Context context) {
        mContext = context;

        setConfigValue();
    }

    protected static void setConfigValue() {
        QtiRilInterface qtiRilInterface = QtiRilInterface.getInstance(mContext);
        boolean isLpluslSupport = false;
        logd("oemhook service status: " + qtiRilInterface.isServiceReady());
        if (qtiRilInterface.isServiceReady()) {
            isLpluslSupport = qtiRilInterface.getLpluslSupportStatus();
        }
        boolean isSubsidyLockFeatureEnabled
                = SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled();
        boolean isSubsidyLockedOrRestricted = isSubsidyLockFeatureEnabled
                && ((SubsidyLockSettingsObserver.isSubsidyLocked(mContext)
                || SubsidyLockSettingsObserver.isSubsidyUnlocked(mContext)));
        boolean isPermanentlyUnlocked = isSubsidyLockFeatureEnabled
                && SubsidyLockSettingsObserver.isPermanentlyUnlocked(mContext);

        //2TODO: store config in SP. if config is changed, delete iccIds as well from sp.
        if (isSubsidyLockedOrRestricted) {
            mConfigValue = ConfigBits.PRIORITY_SUBSIDY_LOCKED_CONFIG.value() |
                           ConfigBits.COMPARE_MCCMNC.value() |
                           ConfigBits.SET_PRIMARY_ON_DEACT.value() |
                           ConfigBits.COMPARE_CARDTYPE.value() |
                           ConfigBits.SHOW_USER_SELECTION_ON_PRIORITY_MATCH.value();
        } else if (isDetect4gCardEnabled() || isPermanentlyUnlocked) {
            int nwmodeConfig = PRIMCARYCARD_L_W_ENABLED ? ConfigBits.DEFAULT_NWMODE_GW.value() :
                    ConfigBits.DEFAULT_NWMODE_GSM.value();
            mConfigValue = ConfigBits.PRIORITY_CONFIG_2.value() |
                    ConfigBits.COMPARE_CARDTYPE.value() |
                    nwmodeConfig |
                    ConfigBits.READ_4G_FLAG.value() |
                    ConfigBits.SET_PRIMARY_ON_DEACT.value() |
                    ConfigBits.SHOW_USER_SELECTION_FOR_EVERY_CHANGE.value();
        } else if (isPrimary7Plus5Enabled()) {
            mConfigValue = ConfigBits.PRIORITY_CONFIG_3.value() |
                    ConfigBits.COMPARE_IIN_CARDTYPE.value() |
                    ConfigBits.DEFAULT_NWMODE_GSM.value() |
                    ConfigBits.SET_PRIMARY_ON_DEACT.value() |
                    ConfigBits.SHOW_USER_SELECTION_ON_PRIORITY_MATCH.value();
            if (isLpluslSupport) {
                mConfigValue = ConfigBits.PRIORITY_CONFIG_4.value() |
                        ConfigBits.COMPARE_IIN_CARDTYPE.value() |
                        ConfigBits.DEFAULT_NWMODE_GCWTL.value() |
                        ConfigBits.SET_PRIMARY_ON_DEACT.value() |
                        ConfigBits.SHOW_USER_SELECTION_ON_PRIORITY_MATCH.value();
            }
        } else if (isPrimaryCardFeatureEnabled()) {
            mConfigValue = ConfigBits.PRIORITY_CONFIG_1.value() |
                    ConfigBits.COMPARE_IIN_CARDTYPE.value() |
                    ConfigBits.DEFAULT_NWMODE_GSM.value() |
                    ConfigBits.SET_PRIMARY_ON_DEACT.value() |
                    ConfigBits.SHOW_USER_SELECTION_ON_PRIORITY_MATCH.value();
        } else {
            mConfigValue = 0;
        }

        logd("ConfigValue is:" + mConfigValue + ", in Binary:" + Integer.toString(mConfigValue,2));
    }

    public static String getConfigXml() {
        if (isBitSetInConfig(ConfigBits.PRIORITY_CONFIG_4)) {
            logd("use priority_config_4.xml");
            return "priority_config_4";
        } else if (isBitSetInConfig(ConfigBits.PRIORITY_SUBSIDY_LOCKED_CONFIG)) {
            return "subsidy_feature_config";
        } else if (isBitSetInConfig(ConfigBits.PRIORITY_CONFIG_3)) {
            return "priority_config_3";
        } else if (isBitSetInConfig(ConfigBits.PRIORITY_CONFIG_2)) {
            return "priority_config_2";
        } else if (isBitSetInConfig(ConfigBits.PRIORITY_CONFIG_1)) {
            return "priority_config_1";
        }
        return "priority_config_2";
    }

    public static int getDefaultNwMode() {
        int defNwMode = Phone.NT_MODE_GSM_ONLY;
        if (isBitSetInConfig(ConfigBits.DEFAULT_NWMODE_GSM)) {
            defNwMode = Phone.NT_MODE_GSM_ONLY;
        } else if (isBitSetInConfig(ConfigBits.DEFAULT_NWMODE_GW)) {
            defNwMode = Phone.NT_MODE_WCDMA_PREF;
        } else if (isBitSetInConfig(ConfigBits.DEFAULT_NWMODE_GCWTL)) {
            defNwMode = Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA;
        }
        logv("getDefaultNwMode: " + defNwMode);
        return defNwMode;
    }

    public static int getPriorityConfigComparator() {
        int comparator = COMPARE_CARDTYPE;
        if (isBitSetInConfig(ConfigBits.COMPARE_MCCMNC)) {
            comparator = COMPARE_MCCMNC;
        } else if (isBitSetInConfig(ConfigBits.COMPARE_CARDTYPE)) {
            comparator = COMPARE_CARDTYPE;
        } else if (isBitSetInConfig(ConfigBits.COMPARE_IIN_CARDTYPE)) {
            comparator = COMPARE_IIN_CARDTYPE;
        }
        logv("getPriorityConfigComparator: " + comparator);

        return comparator;
    }

    public static int getDefaultPrimarySlot() {
        int defPrimarySlot = (isBitSetInConfig(ConfigBits.DEFAULT_PRIMARY_SLOT_1) ? 1 : 0);
        logv("getDefaultPrimarySlot: " + defPrimarySlot);
        return defPrimarySlot;
    }

    public static boolean setPrimaryCardOnDeAct() {
        boolean setPcOnDeact = isBitSetInConfig(ConfigBits.SET_PRIMARY_ON_DEACT);
        logv("setPrimaryCardOnDeAct: " + setPcOnDeact);
        return setPcOnDeact;
    }

    public static boolean read4gFlag() {
        boolean read4g = isBitSetInConfig(ConfigBits.READ_4G_FLAG);
        logv("read4gFlag: " + read4g);
        return read4g;
    }

    public static boolean disableDds() {
        boolean disableDds = isBitSetInConfig(ConfigBits.DISABLE_DDS);
        logv("disableDds: " + disableDds);
        return disableDds;
    }


    public static int getUserSelectionMode() {
        int userSelMode = DISABLE_USER_SELECTION;
        if (isBitSetInConfig(ConfigBits.SHOW_USER_SELECTION_FOR_EVERY_CHANGE)) {
            userSelMode = SHOW_USER_SELECTION_FOR_EVERY_CHANGE;
        } else if (isBitSetInConfig(ConfigBits.SHOW_USER_SELECTION_ON_PRIORITY_MATCH)) {
            userSelMode = SHOW_USER_SELECTION_ON_PRIORITY_MATCH;
        } else if (isBitSetInConfig(ConfigBits.DISABLE_USER_SELECTION)) {
            userSelMode = DISABLE_USER_SELECTION;
        }
        logv("getUserSelectionMode: " + userSelMode);
        return userSelMode;
    }

    private static boolean isBitSetInConfig(ConfigBits config) {
        return ((mConfigValue & config.value()) == config.value());
    }

    // This method returns current configured primary slot.
    public static int getCurrentPrimarySlotFromDB(Context context) {
        int slotId = Settings.Global.getInt(context.getContentResolver(),
                CONFIG_CURRENT_PRIMARY_SUB, SLOT_INVALID);
        return slotId;
    }

    // This method saves current configured primary slot to DB.
    public static void savePrimarySlotToDB(int primarySlot) {
        Settings.Global.putInt(getInstance().mContext.getContentResolver(),
                CONFIG_CURRENT_PRIMARY_SUB, primarySlot);
    }

    public static void saveEnableUserSelectioninDB(boolean enableUserSel) {
        Settings.Global.putInt(getInstance().mContext.getContentResolver(),
                CONFIG_SUB_SELECT_MODE_MANUAL, (enableUserSel ? 1 : 0));
    }

    public static void saveDisableDdsPreferenceInDB(boolean disableDds) {
        Settings.Global.putInt(getInstance().mContext.getContentResolver(),
                CONFIG_DISABLE_DDS_PREFERENCE, (disableDds ? 1 : 0));
    }


    public static void savePrimarySetable(boolean isSetable) {
        Settings.Global.putInt(getInstance().mContext.getContentResolver(),
                CONFIG_PRIMARY_SUB_IS_SETABLE, isSetable ? 1 : 0);
    }

    public static boolean isPrimaryCardFeatureEnabled() {
        return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) && (PHONE_COUNT > 1);
    }

    public static boolean isDetect4gCardEnabled() {
        return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) &&
               SystemProperties.getBoolean(DETECT_4G_CARD_PROPERTY_NAME, false)
                && (PHONE_COUNT > 1);
    }

    public static boolean isPrimaryCardType2Enabled() {
        return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) &&
               SystemProperties.getBoolean(PRIMARY_CARD_TYPE_2_PROPERTY_NAME, false)
                && (PHONE_COUNT > 1);
    }

    public static boolean isPrimary7Plus5Enabled() {
        return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) &&
               SystemProperties.getBoolean(PRIMARY_CARD_7_5_PROPERTY_NAME, false)
                && (PHONE_COUNT > 1);
    }

    public static boolean is3gpp2NwMode(int nwMode) {
        int raf = RadioAccessFamily.getRafFromNetworkType(nwMode);
        if (((raf & RAF_CDMA) == RAF_CDMA) || ((raf & RAF_EVDO) == RAF_EVDO)) {
            return true;
        }
        return false;
    }

    private static void logd(String string) {
        if (DBG) Rlog.d(LOG_TAG, string);
    }

    private static void logv(String string) {
        if (VDBG) Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}


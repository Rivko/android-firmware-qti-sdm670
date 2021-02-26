/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.Fragment;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.os.storage.StorageManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;


import com.android.internal.os.IRegionalizationService;
import com.android.internal.os.RegionalizationEnvironment;
import com.android.internal.telephony.TelephonyProperties;
import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Carrier.SwitchData;
import com.qualcomm.qti.loadcarrier.ILoadCarrierService;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class Utils {
    private static final String TAG = "Utils";

    public static final boolean DEBUG = true;

    // The action and extra key for start activity.
    public static final String ACTION_TRIGGER =
            "com.qualcomm.qti.carrierconfigure.trigger";
    public static final String ACTION_TRIGGER_START =
            "com.qualcomm.qti.carrierconfigure.trigger.start";
    public static final String ACTION_TRIGGER_REBOOT =
            "com.qualcomm.qti.carrierconfigure.trigger.reboot";
    public static final String ACTION_TRIGGER_WELCOME =
            "com.qualcomm.qti.carrierconfigure.trigger.welcome";
    public static final String ACTION_CARRIER_RESET =
            "com.qualcomm.qti.carrierconfigure.reset";
    public static final String ACTION_TRIGGER_CONFIGUAGE_LATER =
            "com.qualcomm.qti.carrierconfigure.trigger.configuage.later";

    public static final String ACTION_SET_TRIGGER_PROPERTY =
            "com.qualcomm.qti.carrierconfigure.set.trigger.property";
    public static final String ENABLE_TRIGGER_NAME = "enable_trigger_prop";

    public static final String EXTRA_PATH_LIST    = "trigger_path_list";
    public static final String EXTRA_CARRIER_LIST = "trigger_carrier_list";

    public static final int STANDARD_VIEW = 0;
    public static final int HIERARCHY_VIEW = 1;

    //Special ROW package name. Should be sync with CarrierLoadService/Utils.java
    public static final String SPECIAL_ROW_PACKAGE_NAME = "ROW";

    public static final String CARRIER_TO_DEFAULT_NAME = "2Default";
    public static final String DEFAULT_SPEC_NAME = "Default";

    // The default sim mode.
    private static final String DEFAULT_SIM_MODE = "ssss";

    // The modem will use this path to load MBN files as default.
    private static final String DEFAULT_MBN_PATH = "/data/vendor/radio";
    // The prop key used to get the MBN file path.
    private static final String PROP_MBN_PATH = "persist.vendor.radio.mbn_path";
    // The prop control sim trigger switch
    public static final String PROP_KEY_TRIGGER = "persist.radio.trigger";

    // The carrier's MBN file path in OTA pack.
    private static final String CARRIER_MBN_PATH_IN_OTA = "modem_config/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8917_IN_OTA = "modem_config/msm8917/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8937_IN_OTA = "modem_config/msm8937/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8940_IN_OTA = "modem_config/msm8940/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8909_IN_OTA = "modem_config/msm8909/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8920_IN_OTA = "modem_config/msm8920/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8953_IN_OTA = "modem_config/msm8953/mcfg_sw";
    private static final String CARRIER_MBN_PATH_SDM660_IN_OTA  = "modem_config/sdm660/mcfg_sw";
    private static final String CARRIER_MBN_PATH_SDM630_IN_OTA  = "modem_config/sdm630/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8998_IN_OTA  = "modem_config/msm8998/mcfg_sw";

    // meta3.0
    private static final String CARRIER_MBN_PATH_MSM8909_3_IN_OTA = "modem_config/msm8909_3/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8917_3_IN_OTA = "modem_config/msm8917_3/mcfg_sw";
    private static final String CARRIER_MBN_PATH_MSM8937_3_IN_OTA = "modem_config/msm8937_3/mcfg_sw";

    // The target board plateform
    public static final String TARGET_BOARD_PLATFORM_MSM8917 = "MSM8917";
    public static final String TARGET_BOARD_PLATFORM_MSM8937 = "MSM8937";
    public static final String TARGET_BOARD_PLATFORM_MSM8940 = "MSM8940";
    public static final String TARGET_BOARD_PLATFORM_MSM8909 = "MSM8909";
    public static final String TARGET_BOARD_PLATFORM_MSM8920 = "MSM8920";
    public static final String TARGET_BOARD_PLATFORM_MSM8953 = "MSM8953";
    public static final String TARGET_BOARD_PLATFORM_SDM660  = "SDM660";
    public static final String TARGET_BOARD_PLATFORM_SDM630  = "SDM630";
    public static final String TARGET_BOARD_PLATFORM_MSM8998  = "MSM8998";
    public static final String TARGET_BOARD_PLATFORM_SDM845  = "SDM845";

    // The ROW MBN file path in OTA pack.
    private static final String ROW_MBN_PATH_IN_OTA = "modem_row_config/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8917_IN_OTA = "modem_row_config/msm8917/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8937_IN_OTA = "modem_row_config/msm8937/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8940_IN_OTA = "modem_row_config/msm8940/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8909_IN_OTA = "modem_row_config/msm8909/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8920_IN_OTA = "modem_row_config/msm8920/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8953_IN_OTA = "modem_row_config/msm8953/mcfg_sw";
    private static final String ROW_MBN_PATH_SDM660_IN_OTA  = "modem_row_config/sdm660/mcfg_sw";
    private static final String ROW_MBN_PATH_SDM630_IN_OTA  = "modem_row_config/sdm630/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8998_IN_OTA  = "modem_row_config/msm8998/mcfg_sw";

    //meta 3.0
    private static final String ROW_MBN_PATH_MSM8909_3_IN_OTA = "modem_row_config/msm8909_3/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8917_3_IN_OTA = "modem_row_config/msm8917_3/mcfg_sw";
    private static final String ROW_MBN_PATH_MSM8937_3_IN_OTA = "modem_row_config/msm8937_3/mcfg_sw";

    // The prop key used to enable file-based switch.
    public static final String PROP_KEY_FILE_BASED_ENABLED = "persist.radio.filebased.enabled";

    // The prop key to enable block-based package switch.
    public static final String PROP_KEY_BLOCK_BASED_ENABLED = "persist.radio.blockbased";

    // The mbn file max size.
    private static final int MBN_FILE_MAX_SIZE = 512 * 1024;

    public static final String INTENT_CARRIER_TARGET_SLOT = "target_slot";
    public static final int INVALID_SLOT_ID = -1;
    public static final int INVALID_SUB_MASK = 0;

    public static final String SUB_MASK_FILE_PATH = "/persist/speccfg/submask";
    public static final String SUB_MASK_PRE = "subMask=";

    public final static String SPEC_FILE_PATH                 = "/persist/speccfg/spec";
    public final static String RO_VENOR_PROP_FILE_PATH        = "/persist/speccfg/vendor_ro.prop";
    public final static String RO_VENOR_PROP_PRE              = "vendor_ro=";

    public final static String PERSIST_VENOR_PROP_FILE_PATH   = "/persist/speccfg/vendor_persist.prop";
    public final static String PERSIST_VENOR_PROP_PRE         = "vendor_persist=";

    public static final String PARTITION_INFO_FILE = "/persist/speccfg/partition";
    public static final String DEVICE_TYPE = "/persist/speccfg/devicetype";
    public static final String DEVICE_TYPE_PRE = "devicetype=";

    public static final String MBN_VERSION = "/persist/speccfg/mbnversion";
    public static final String QGP_VERSION = "/persist/speccfg/qgpversion";
    public static final String NOT_TRIGGERED = "/persist/speccfg/.not_triggered";
    public static final String DEFAULT_QGPVERSION = "Default_1.0_";

    private static final String SHARED_PREFERENCE_NAME = "trigger";
    public static final String SHARED_PREFERENCE_KEY_RESTORE_3RD_APKS = "restore_3rd_apks";
    public static final String SHARED_PREFERENCE_KEY_IS_FIRST_BOOT = "firstboot";
    public static final String SHARED_PREFERENCE_KEY_CURRENT_SUB_MASK = "submask";
    // Save the triggered package name of one slot.
    public static final String SHARED_PREFERENCE_KEY_CARRIER_SLOT_PRE = "carrier_of_slot";
    // Save the triggered times for current SIMs.
    public static final String SHARED_PREFERENCE_KEY_TIMES = "times";
    // Save the latest sub ID of one slot.
    public static final String SHARED_PREFERENCE_KEY_SUB_SLOT_PRE = "sub_of_slot";

    public static final String COM_MBN_FILE_PATH = "data/modem_config/mcfg_sw.mbn";

    public static final String SD_PATH_IN_RECOVERY = "/sdcard/";

    private static final String[] LEVEL_ALL_EXCLUDED_PACKAGE_LIST = {
            "com.android.launcher3",
            "com.android.phone",
            "com.android.providers.calendar",
            "com.android.providers.contacts",
            "com.android.providers.media",
            "com.android.providers.settings",
            "com.android.providers.telephony",
            "com.qualcomm.qti.carrierconfigure",
            "com.qualcomm.qti.loadcarrier",
            "com.qualcomm.qti.accesscache",
    };

    private static final String[] LEVEL_ALL_EXCLUDED_PACKAGE_PREFIX_LIST = {
            "com.google.android",
            "com.claroColombia",
            "com.amazon",
            "com.privacystar",
    };

    private static final String[] LEVEL_ALL_LAST_PACKAGE_LIST = {
            "com.android.providers.settings",
            "com.android.providers.media",
            "com.android.providers.calendar",
            "com.android.launcher3",
            //"com.android.providers.telephony",
            "com.android.phone",
            "com.android.providers.contacts",
    };

    private static final String[] LEVEL_DEFAULT_PACKAGE_LIST = {
            "android",
            "com.android.browser",
            "com.android.calendar",
            "com.android.camera2",
            "com.android.cellbroadcastreceiver",
            "com.android.contacts",
            "com.android.contacts.common",
            "com.android.deskclock",
            "com.android.dialer",
            "com.android.documentsui",
            "com.android.email",
            "com.android.frameworks.telresources",
            "com.android.gallery3d",
            "com.android.inputmethod.latin",
            "com.android.launcher",
            "com.android.mms",
            "com.android.music",
            "com.android.musicfx",
            "com.android.packageinstaller",
            "com.android.phone.common",
            "com.android.PhoneFeatures",
            "com.android.profile",
            "com.android.providers.contacts",
            "com.android.providers.settings",
            "com.android.server.telecom",
            "com.android.settings",
            "com.android.soundrecorder",
            "com.android.stk",
            "com.android.Stk",
            "com.android.systemui",
            "com.android.timerswitch",
            "com.android.wallpaper.livepicker",
            "com.borqs.videocall",
            "com.caf.fmradio",
            "com.codeaurora.fmrecording",
            "com.cyanogenmod.filemanager",
            "com.cyanogenmod.trebuchet",
            "com.qrd.omadownload",
            "com.qti.xdivert",
            "com.qualcomm.qti.explorer",
            "com.qualcomm.qti.phonefeature",
            "com.qualcomm.qti.tefnetworkinfodisplay",
            "com.qualcomm.qti.telephony.carrierpack",
            "com.qualcomm.qti.telephony.extcarrierpack",
            "com.qualcomm.simcontacts",
            "com.qualcomm.update",
            "com.wdstechnology.android.kryten",
            "org.codeaurora.bluetooth.pxpmonitor",
            "org.codeaurora.snapcam",
            "com.android.launcher3",
            "com.android.phone",
    };

    /**
     * Get the current multi-sim mode.
     */
    public static String getSimMode() {
        String res = SystemProperties.get(
                TelephonyProperties.PROPERTY_MULTI_SIM_CONFIG, DEFAULT_SIM_MODE);
        return res;
    }

    /**
     * Set the multi-sim configuration as newValue.
     * @param newValue
     */
    public static void setSimMode(String newValue) {
        SystemProperties.set(TelephonyProperties.PROPERTY_MULTI_SIM_CONFIG, newValue);
    }

    /**
     * Get the MBN file path which will be used to load the mbn file. The default path
     * is {@link DEFAULT_MBN_PATH}.
     */
    public static String getMBNPath() {
        return SystemProperties.get(PROP_MBN_PATH, DEFAULT_MBN_PATH);
    }

    /**
     * Get carrier's MBN file path in ota pack. The path is {@link CARRIER_MBN_PATH_IN_OTA}.
     */
    public static String getCarrierMBNPathInOta() {
        return(getCarrierMBNPath(true));
    }

	private static void createSpecfile(IRegionalizationService regionalizationService,ArrayList<String> contents,String filename) {

     try {
		 if (regionalizationService.checkFileExists(filename)) {
			return;
		 }
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		File file = new File(filename);
		try {
			file.createNewFile();
		}catch (IOException e) {
			Log.d("Carrier", "can't create file:" + filename + " error:" +  e);
		}

		try {
			//read content from spec,then write it into each file.
			if (contents != null && contents.size() > 0) {
				for (int i = 0; i < contents.size(); i++) {
					if(contents.get(i).startsWith(RO_VENOR_PROP_PRE)) {
						regionalizationService.writeFile(RO_VENOR_PROP_FILE_PATH, contents.get(i).substring(RO_VENOR_PROP_PRE.length())+"\n", false);
					} else if(contents.get(i).startsWith(PERSIST_VENOR_PROP_PRE)) {
						regionalizationService.writeFile(PERSIST_VENOR_PROP_FILE_PATH, contents.get(i).substring(PERSIST_VENOR_PROP_PRE.length())+"\n", false);
					} else if(contents.get(i).startsWith(DEVICE_TYPE_PRE)) {
						regionalizationService.writeFile(DEVICE_TYPE, contents.get(i).substring(DEVICE_TYPE_PRE.length())+"\n", false);
					}
				}
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}

	}

   public static void generateSpecFile(){
     try {
          IRegionalizationService regionalizationService =RegionalizationEnvironment.getRegionalizationService();
	      //read content from spec
		   ArrayList<String> contents = (ArrayList<String>)regionalizationService.readFile(SPEC_FILE_PATH, null);

		   //todo :if the specific file doesn't exist ,we'll create new one.
		   createSpecfile(regionalizationService,contents,RO_VENOR_PROP_FILE_PATH);
		   createSpecfile(regionalizationService,contents,PERSIST_VENOR_PROP_FILE_PATH);
		   createSpecfile(regionalizationService,contents,DEVICE_TYPE);
		   createSpecfile(regionalizationService,contents,SUB_MASK_FILE_PATH);
		   createSpecfile(regionalizationService,contents,MBN_VERSION);
		   createSpecfile(regionalizationService,contents,NOT_TRIGGERED);

		} catch (RemoteException e) {
			e.printStackTrace();
        }
   }

	public static boolean iSchipSetName(String chipset) {

		try {
			IRegionalizationService regionalizationService =
				RegionalizationEnvironment.getRegionalizationService();
			ArrayList<String> chipsetModelList = (ArrayList<String>)
				regionalizationService.readFile("/proc/device-tree/model", null);

			if (chipsetModelList != null && chipsetModelList.size() > 0) {
				String chipsetModel = chipsetModelList.get(0);
				ArrayList<String> chipList = new ArrayList<String>();
				getChipList(chipList,chipsetModel);
				if (chipList != null && chipList.size() > 0) {
					for(int i = 0 ; i < chipList.size(); i++) {
                        Log.d(TAG,"chipList(" + i +") :[" + chipList.get(i)+"]");
						if(chipList.get(i).equals(chipset)){
							return true;
						}
					}
				}
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		return false;
	}

    public static String getCarrierMBNPath(boolean isCommercialMbn) {
        try {
            IRegionalizationService regionalizationService =
                    RegionalizationEnvironment.getRegionalizationService();
            ArrayList<String> chipsetModelList = (ArrayList<String>)
                    regionalizationService.readFile("/proc/device-tree/model", null);
            String metaVersion = getNewMetaVersion();
            Log.d(TAG,"chipsetModelList " + chipsetModelList + "metaVersion "+metaVersion);
            if (chipsetModelList != null && chipsetModelList.size() > 0) {
                String chipsetModel = chipsetModelList.get(0);
                ArrayList<String> chipList = new ArrayList<String>();
                getChipList(chipList,chipsetModel);
                if (chipList != null && chipList.size() > 0) {
                    for(int i = 0 ; i < chipList.size(); i++) {
                        Log.d(TAG,"chipList(" + i +") : [" + chipList.get(i)+"]");
                        if(chipList.get(i).equals(TARGET_BOARD_PLATFORM_MSM8917)){
                           if(isCommercialMbn){
                                if(isMetaVersion3(metaVersion)) {
                                    return CARRIER_MBN_PATH_MSM8917_3_IN_OTA;
                                }
                                return CARRIER_MBN_PATH_MSM8917_IN_OTA;
                            } else {
                                if(isMetaVersion3(metaVersion)) {
                                    return ROW_MBN_PATH_MSM8917_3_IN_OTA;
                                }
                                return ROW_MBN_PATH_MSM8917_IN_OTA;
                            }
                        } else if(chipList.get(i).contains(TARGET_BOARD_PLATFORM_MSM8937)){
                            if(isCommercialMbn){
                                if(isMetaVersion3(metaVersion)) {
                                    return CARRIER_MBN_PATH_MSM8937_3_IN_OTA;
                                }
                                return CARRIER_MBN_PATH_MSM8937_IN_OTA;
                            } else {
                                if(isMetaVersion3(metaVersion)) {
                                    return ROW_MBN_PATH_MSM8937_3_IN_OTA;
                                }
                                return ROW_MBN_PATH_MSM8937_IN_OTA;
                            }
                        } else if(chipList.get(i).contains(TARGET_BOARD_PLATFORM_MSM8940)){
                            if(isCommercialMbn){
                                return CARRIER_MBN_PATH_MSM8940_IN_OTA;
                            } else {
                                return ROW_MBN_PATH_MSM8940_IN_OTA;
                            }
                        } else if(chipList.get(i).contains(TARGET_BOARD_PLATFORM_MSM8909)){
                            if(isCommercialMbn){
                                if(isMetaVersion3(metaVersion)) {
                                    return CARRIER_MBN_PATH_MSM8909_3_IN_OTA;
                                }
                                return CARRIER_MBN_PATH_MSM8909_IN_OTA;
                            } else {
                                if(isMetaVersion3(metaVersion)) {
                                    return ROW_MBN_PATH_MSM8909_3_IN_OTA;
                                }
                                return ROW_MBN_PATH_MSM8909_IN_OTA;
                            }
                        } else if(chipList.get(i).contains(TARGET_BOARD_PLATFORM_MSM8920)){
                            if(isCommercialMbn){
                                return CARRIER_MBN_PATH_MSM8920_IN_OTA;
                            } else {
                                return ROW_MBN_PATH_MSM8920_IN_OTA;
                            }
                        } else if(chipList.get(i).contains(TARGET_BOARD_PLATFORM_MSM8953)){
                            if(isCommercialMbn){
                                return CARRIER_MBN_PATH_MSM8953_IN_OTA;
                            } else {
                                return ROW_MBN_PATH_MSM8953_IN_OTA;
                            }
                        } else if(chipList.get(i).contains(TARGET_BOARD_PLATFORM_SDM660)){
                            if(isCommercialMbn){
                                return CARRIER_MBN_PATH_SDM660_IN_OTA;
                            } else {
                                return ROW_MBN_PATH_SDM660_IN_OTA;
                            }
                        } else if (chipList.get(i).contains(TARGET_BOARD_PLATFORM_SDM630)){
                            if(isCommercialMbn) {
                                return CARRIER_MBN_PATH_SDM630_IN_OTA;
                            } else {
                                return ROW_MBN_PATH_SDM630_IN_OTA;
                            }
                        } else if (chipList.get(i).contains(TARGET_BOARD_PLATFORM_MSM8998)){
                            if(isCommercialMbn) {
                                return CARRIER_MBN_PATH_MSM8998_IN_OTA;
                            } else {
                                return ROW_MBN_PATH_MSM8998_IN_OTA;
                            }
                        }
                    }
                }
            } else {
                String productName = SystemProperties.get("ro.board.platform", "");
                Log.d(TAG,"productName " + productName);
                if(TARGET_BOARD_PLATFORM_MSM8909.equalsIgnoreCase(productName)) {
                    if(isCommercialMbn){
                        if(isMetaVersion3(metaVersion)) {
                            return CARRIER_MBN_PATH_MSM8909_3_IN_OTA;
                        }
                        return CARRIER_MBN_PATH_MSM8909_IN_OTA;
                    } else {
                        if(isMetaVersion3(metaVersion)) {
                            return ROW_MBN_PATH_MSM8909_3_IN_OTA;
                        }
                        return ROW_MBN_PATH_MSM8909_IN_OTA;
                    }
                }
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        if(isCommercialMbn){
            return CARRIER_MBN_PATH_MSM8917_IN_OTA;
        } else {
            return ROW_MBN_PATH_MSM8917_IN_OTA;
        }
    }

	private static String dealFinalString(String str,String chiptype) {
		int index = str.indexOf(chiptype);
		if( str.substring(index,index+4).length() > 1){
		  String chipStr = str.substring(index+3,str.length()).replaceAll("[a-zA-Z]","");
		  return chiptype+chipStr;
		}

		return "Not Found";
	}

    private static void getChipList(ArrayList<String> chipList,String str) {
        if(chipList == null || str == null) {
            return;
        }
        String chipType;
        if(str.contains("MSM")){
            chipType = "MSM";
        } else if(str.contains("SDM")){
            chipType = "SDM";
        } else{
            return;
        }
        try {
                int index = str.indexOf(chipType);
                if (index != -1) {
                    if(str.length() >= str.substring(0,index).length() + 8){
                        String chipStr = str.substring(index ,index + 8);
                        chipList.add(dealFinalString(chipStr.trim().replaceAll(" ",""),chipType));
                    }
                    if(str.substring(index + 8).length() > 1){
                        String subStr = str.substring(index + 8);
                        if(subStr != null && subStr.indexOf(chipType) != -1){
                            getChipList(chipList,subStr);
                        }
                    }
                }

        } catch (Exception e) {
        }
    }

    private static boolean isMetaVersion3(String metaVersion) {
        return !TextUtils.isEmpty(metaVersion) && metaVersion.contains("LA.3");
    }

    /*Get the meta Version*/
    private static String getMetaVersion() {
        String meta = null;
        File metainfo = new File("/firmware/verinfo/ver_info.txt");
        if (metainfo.exists()){
            meta = Utils.readFirstLine(metainfo, null);
        }
        return meta;
    }

    public static String getNewMetaVersion() {
        String mMeta = null;
        File mMetainfo = new File("/firmware/verinfo/ver_info.txt");
        if (mMetainfo.exists()){
            ArrayList<String> res = readFile(mMetainfo, null, false);
            if (res == null || res.size() < 1) {
                return null;
            }

            if (res.size() == 1) {
                mMeta = res.get(0);
            }
            else
            {
                for(int i=0;i<res.size();i++)
                {
                    if(res.get(i).contains("Meta_Build_ID")){
                        mMeta = res.get(i);
                        break;
                    }
                }
                mMeta = mMeta.substring("Meta_Build_ID: \"".length()+2,mMeta.length()-2);
            }
        }
        return mMeta;
    }


    /**
     * Get ROW MBN file path in ota pack. The path is {@link ROW_MBN_PATH_IN_OTA}.
     */
    public static String getCarrierROWMBNPathInOta() {
        return(getCarrierMBNPath(false));
        //return ROW_MBN_PATH_IN_OTA;
    }

    /**
     * To read the first line from the given file. If the line do not match the regular
     * expression, return null.
     * @param regularExpression used to find the matched line, if it is null, do not check
     *                          if the line matched this expression.
     */
    public static String readFirstLine(File file, String regularExpression) {
        ArrayList<String> res = readFile(file, regularExpression, true);
        if (res == null || res.size() < 1) return null;
        return res.get(0);
    }

    /**
     * To read the content from the given file. If the lines do not match the regular
     * expression, do not add to result.
     * @param regularExpression used to find the matched line, if it is null, do not check
     *                          if the line matched this expression.
     */
    private static final String COMMENTS_TAG = "#";

    public static ArrayList<String> readFile(File file, String regularExpression,
            boolean onlyReadFirstLine) {
        if (file == null || !file.exists() || !file.canRead()) return null;

        ArrayList<String> contents = new ArrayList<String>();

        FileReader fr = null;
        BufferedReader br = null;
        try {
            fr = new FileReader(file);
            br = new BufferedReader(fr);
            // Read the lines, and get the current carrier.
            String line = null;
            while ((line = br.readLine()) != null && (line = line.trim()) != null) {
                if (line.isEmpty() || line.startsWith(COMMENTS_TAG)) {
                    continue;
                }
                if (!TextUtils.isEmpty(regularExpression)) {
                    if (line.matches(regularExpression)) {
                        contents.add(line);
                    }
                } else {
                    contents.add(line);
                }
                if (onlyReadFirstLine) break;
            }
        } catch (IOException e) {
            Log.e(TAG, "Read File error, caused by: " + e.getMessage());
        } finally {
            try {
                if (br != null) br.close();
                if (fr != null) fr.close();
            } catch (IOException e) {
                Log.e(TAG, "Close the reader error, caused by: " + e.getMessage());
            }
        }

        return contents;
    }

    /**
     * Write the content to this file with append value.
     */
    public static void writeFile(File file, String content, boolean append) throws IOException {
        if (file == null || !file.exists() || !file.canWrite()) {
            throw new IOException("Write file error. Please check the file status.");
        }

        if (TextUtils.isEmpty(content)) return;

        // Write the file with the content.
        FileWriter fw = new FileWriter(file, append);
        try {
            fw.write(content);
        } finally {
            if (fw != null) fw.close();
        }
    }

    /**
     * Return the file size for the given file.
     */
    public static int getFileSize(File file) throws IOException {
        int size = 0;
        if (file != null && file.exists()) {
            FileInputStream fis = new FileInputStream(file);
            try {
                size = fis.available();
            } finally {
                if (fis != null) fis.close();
            }
        }
        return size;
    }

    public static void copyFile(String srcPath, String dstPath) {
        FileInputStream fins = null;
        FileOutputStream fouts = null;
        try {
            fins = new FileInputStream(new File(srcPath));
            fouts = new FileOutputStream(new File(dstPath));
            int bytesRead = 0;
            byte[] buf = new byte[4 * 1024];
            while ((bytesRead = fins.read(buf)) != -1) {
                fouts.write(buf, 0, bytesRead);
            }
            fouts.flush();
        } catch (IOException ex) {
            ex.printStackTrace();
        } finally {
            if (fins != null)
                try {
                    fins.close();
                } catch (IOException e) {}
            if (fouts != null)
                try {
                    fouts.close();
                } catch (IOException e) {}
        }
    }

    public static boolean isFileBasedSwitchEnabled() {
        boolean result = isPresetMode() ?
                false : SystemProperties.getBoolean(PROP_KEY_FILE_BASED_ENABLED, false);
        return result;
    }

    /**
     * Prompt the alert dialog. If you want to handle the button click action, please
     * implements the OnAlertDialogButtonClick interface.
     */
    public static class MyAlertDialog extends DialogFragment implements OnClickListener {
        public static final String TAG_LABEL = "alert";

        private static final String TITLE = "title";
        private static final String MESSAGE = "message";

        public interface OnAlertDialogButtonClick {
            public void onAlertDialogButtonClick(int which);
        }

        public static MyAlertDialog newInstance(Fragment targetFragment, int titleResId,
                int messageResId) {
            MyAlertDialog dialog = new MyAlertDialog();

            Bundle bundle = new Bundle();
            bundle.putInt(TITLE, titleResId);
            bundle.putInt(MESSAGE, messageResId);
            dialog.setArguments(bundle);

            dialog.setCancelable(false);
            dialog.setTargetFragment(targetFragment, 0);
            return dialog;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int titleResId = getArguments().getInt(TITLE);
            final int messageResId = getArguments().getInt(MESSAGE);

            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(titleResId)
                    .setMessage(messageResId)
                    .setPositiveButton(android.R.string.ok, this)
                    .setNegativeButton(android.R.string.cancel, this);

            AlertDialog dialog = builder.create();
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onClick(DialogInterface dialog, int which) {
            Fragment target = getTargetFragment();
            if (target instanceof OnAlertDialogButtonClick) {
                ((OnAlertDialogButtonClick) target).onAlertDialogButtonClick(which);
            }
        }
    }

    /**
     * Prompt the notice dialog. If you want to handle the button click action, please
     * implements the OnNoticeDialogButtonClick interface.
     */
    public static class MyNoticeDialog extends DialogFragment implements OnClickListener {
        public static final String TAG_LABEL = "notification";

        private static final String ID = "id";
        private static final String TITLE = "title";
        private static final String MESSAGE = "message";

        private int mDialogId = -1;

        public interface OnNoticeDialogButtonClick {
            public void onNoticeDialogButtonClick(int dialogId);
        }

        public static MyNoticeDialog newInstance(Fragment targetFragment, int dialogId,
                int titleResId, int messageResId) {
            MyNoticeDialog dialog = new MyNoticeDialog();

            Bundle bundle = new Bundle();
            bundle.putInt(ID, dialogId);
            bundle.putInt(TITLE, titleResId);
            bundle.putInt(MESSAGE, messageResId);
            dialog.setArguments(bundle);

            dialog.setCancelable(false);
            dialog.setTargetFragment(targetFragment, 0);
            return dialog;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int titleResId = getArguments().getInt(TITLE);
            final int messageResId = getArguments().getInt(MESSAGE);
            mDialogId = getArguments().getInt(ID);

            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(titleResId)
                    .setMessage(messageResId)
                    .setPositiveButton(android.R.string.ok, this);

            AlertDialog dialog = builder.create();
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onClick(DialogInterface dialog, int which) {
            Fragment target = getTargetFragment();
            if (target instanceof OnNoticeDialogButtonClick) {
                ((OnNoticeDialogButtonClick) target).onNoticeDialogButtonClick(mDialogId);
            }
        }
    }

    /**
     * Prompt the wait progress dialog.
     */
    public static class WaitDialog extends DialogFragment {
        public static final String TAG_LABEL = "wait";

        public static WaitDialog newInstance() {
            WaitDialog dialog = new WaitDialog();
            dialog.setCancelable(false);
            return dialog;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            ProgressDialog dialog = new ProgressDialog(getActivity());
            dialog.setMessage(getString(R.string.progress_wait)+" "+(TriggerRebootFragment.showCarrier!= null?TriggerRebootFragment.showCarrier:"") +" "+getString(R.string.reboot_please_wait));
            android.util.Log.i("TAG", "progress_wait=");
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }
    }

    public static class CopyAndUnzipOtaFileThread extends Thread {
        public static final int MSG_UPDATE_PROGRESS_DIALOG = 100;

        private Context mContext;
        private Handler mHandler;
        private ILoadCarrierService mService = null;
        private ICarrierAccessCacheService mAccessCacheService = null;
        private ArrayList<Carrier> mCarrierList = null;
        private Carrier mCarrier =   null;
        private int mRequestCode = -1;

        public CopyAndUnzipOtaFileThread(Context context, Handler handler,
                int requestCode, ILoadCarrierService service,
                ICarrierAccessCacheService accessCacheService, ArrayList<Carrier> carrierList){
            this.mContext = context;
            this.mHandler = handler;
            this.mService = service;
            this.mAccessCacheService = accessCacheService;
            this.mCarrierList = carrierList;
            this.mRequestCode = requestCode;
        }
        public CopyAndUnzipOtaFileThread(Context context, Handler handler,
                int requestCode, ILoadCarrierService service,
                ICarrierAccessCacheService accessCacheService, Carrier carrier){
            this.mContext = context;
            this.mHandler = handler;
            this.mService = service;
            this.mAccessCacheService = accessCacheService;
            this.mCarrier = carrier;
            this.mRequestCode = requestCode;
        }
        @Override
        public void run() {
            if (mCarrierList != null) {
                ArrayList<SwitchData> dataList = new ArrayList<SwitchData>();
                boolean leaveOneOtaZip = false;
                for (Carrier carrier : mCarrierList) {
                    SwitchData switchData = null;
                    if (!leaveOneOtaZip && Carrier.TYPE_OTA_ZIP_FILE.equals(carrier.mType)) {
                        File srcFile = new File(carrier.getPath());
                        if (srcFile == null || !srcFile.exists()) continue;

                        String dstFilePath = null;
                        String secSdcardPath = Utils.getSecSdcardPath(mContext);
                        if ((!TextUtils.isEmpty(secSdcardPath))
                                && carrier.getPath().startsWith(secSdcardPath)) {
                            dstFilePath = Utils.SD_PATH_IN_RECOVERY + srcFile.getName();
                        } else {
                            dstFilePath = carrier.getPath();
                        }
                        switchData = new SwitchData(carrier.mName, dstFilePath);
                        leaveOneOtaZip = true;
                    } else {
                        switchData = carrier.getSwitchData(mService, mAccessCacheService);
                    }

                    if (switchData != null && !dataList.contains(switchData)) {
                        dataList.add(switchData);
                    }
                }
                mHandler.sendMessage(Message.obtain(mHandler, mRequestCode, dataList));
            } else if (mCarrier != null) {
                mHandler.sendMessage(Message.obtain(mHandler, mRequestCode,
                        mCarrier.getSwitchData(mService, mAccessCacheService)));
            }
        }
    }

    @SuppressWarnings("unchecked")
    public static String getMBNFileFromOta(String srcZipFileName, String dstOutputDirectory,
            String fileName) {
        Log.d(TAG, "fileName="+fileName);
        String resultString = null;
        JarFile jarFile = null;
        try {
            jarFile = new JarFile(srcZipFileName);
            Enumeration entries = jarFile.entries();
            JarEntry jarEntry = null;
            File outputDirectory = new File(dstOutputDirectory);
            outputDirectory.mkdirs();
            while (entries.hasMoreElements()) {
                jarEntry = (JarEntry) entries.nextElement();
                String entryName = jarEntry.getName();
                Log.d(TAG, "entryName="+entryName);
                if (!entryName.endsWith(fileName)) {
                    continue;
                }
                String filePath = dstOutputDirectory + File.separator
                        + entryName.substring(entryName.lastIndexOf("/") + 1);
                if (unzipOneFile(jarFile, jarEntry, filePath)) {
                    resultString = filePath;
                }
                Log.d(TAG,"filename="+filePath + "resultString="+resultString);
                break;
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        } finally {
            if (jarFile != null) {
                try {
                    jarFile.close();
                } catch (IOException e) {}
            }
        }
        return resultString;
    }

    private static boolean unzipOneFile(JarFile jarFile, JarEntry jarEntry, String filePath) {
        boolean result = true;
        InputStream in = null;
        FileOutputStream out = null;
        try {
            File file = new File(filePath);
            in = jarFile.getInputStream(jarEntry);
            out = new FileOutputStream(file);

            int byteCount;
            byte[] by = new byte[1024];

            while ((byteCount = in.read(by)) != -1) {
                out.write(by, 0, byteCount);
            }
            out.flush();
        } catch (IOException ex) {
            ex.printStackTrace();
            result = false;
        } finally {
            if (in != null)
                try {
                    in.close();
                } catch (IOException e) {}
            if (out != null)
                try {
                    out.close();
                } catch (IOException e) {}
        }
        return result;
    }

    public static String getMBNFileFromCacheOta(String srcZipFileName, String dstOutputDirectory,
            String fileName, ICarrierAccessCacheService accessCacheService) {
        Log.d(TAG, "getMBNFileFromCacheOTA fileName="+fileName);
        if (accessCacheService == null) {
            Log.d(TAG, "getMBNFileFromCacheOta accessCacheService == null");
            return null;
        }
        String resultString = null;
        byte[] fileData = new byte[MBN_FILE_MAX_SIZE];
        int fileDataSize = 0;
        try {
            fileDataSize = accessCacheService.getMBNFileDataFromOta(srcZipFileName,
                    fileName, fileData, MBN_FILE_MAX_SIZE);
        } catch (RemoteException e) {
            Log.e(TAG, "Get the mbn file from (" + srcZipFileName + ") error. e=" + e);
        }
        if (fileDataSize > 0) {
            String filePath = dstOutputDirectory + File.separator
                    + fileName.substring(fileName.lastIndexOf("/") + 1);
            if (writeOneFile(filePath, fileData, fileDataSize)) {
                resultString = filePath;
            }
            Log.d(TAG,"filename="+filePath + " resultString="+resultString);
        }
        return resultString;
    }

    private static boolean writeOneFile(String filePath, byte[] fileData, int fileDataSize) {
        boolean result = true;
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(filePath);
            out.write(fileData, 0, fileDataSize);
            out.flush();
        } catch (IOException ex) {
            ex.printStackTrace();
            result = false;
        } finally {
            if (out != null)
                try {
                    out.close();
                } catch (IOException e) {}
        }
        return result;
    }

    public static String getPresetMBNFile(String srcDirPath, String dstOutputDirectory,
            String fileName) {
        File srcDir = new File(srcDirPath);
        if (!srcDir.isDirectory()) {
            return null;
        }

        String mbnFilePathString = null;
        ArrayList<String> fileList = new ArrayList<String>();
        listAllFiles(srcDirPath, fileList);
        for(String file : fileList) {
            if (file.endsWith(fileName)) {
                mbnFilePathString = file;
                break;
            }
        }
        if (DEBUG) Log.d(TAG, "mbnFilePathString=" + mbnFilePathString);
        if (TextUtils.isEmpty(mbnFilePathString)) {
            return null;
        }

        String dstPathString = dstOutputDirectory + File.separator + fileName.substring(
                fileName.lastIndexOf("/") + 1);
        File dest = new File(dstPathString);
        try {
            dest.delete();
            dest.createNewFile();
            Runtime.getRuntime().exec("chmod 644 " + dstPathString);
            Runtime.getRuntime().exec("chown radio.radio " + dstPathString);
        } catch (IOException e) {
            Log.d(TAG, "can't create file:" + dstPathString + " error:" +  e);
        }
        if (!copyWithChannels(new File(mbnFilePathString), dest, true)) {
            Log.d(TAG, "copy failed");
            return null;
        }

        return dstPathString;
    }

    public static boolean copyWithChannels(File src, File dest, boolean append) {
        FileChannel inChannel = null;
        FileChannel outChannel = null;
        FileInputStream inStream = null;
        FileOutputStream outStream = null;
        try{
            try {
                inStream = new FileInputStream(src);
                inChannel = inStream.getChannel();
                outStream = new FileOutputStream(dest, append);
                outChannel = outStream.getChannel();
                long bytesTransferred = 0;
                while(bytesTransferred < inChannel.size()){
                    bytesTransferred += inChannel.transferTo(
                            0, inChannel.size(), outChannel);
                }
            }
            finally {
                if (inChannel != null) inChannel.close();
                if (outChannel != null) outChannel.close();
                if (inStream != null) inStream.close();
                if (outStream != null) outStream.close();
            }
        }
        catch (FileNotFoundException e){
            Log.e(TAG, "MbnTest_ File not found: " + e);
            return false;
        }
        catch (IOException e){
            Log.e(TAG, "MbnTest_ IO exception:" + e);
            return false;
        }

        return true;
    }

    private static void listAllFiles(String dirPath, ArrayList<String> fileList) {
        if (fileList == null) {
            return;
        }
        File dir = new File(dirPath);
        if (!dir.isDirectory()) {
            return;
        }
        File[] files = dir.listFiles();
        for (File file : files) {
            if (file.isDirectory()) {
                listAllFiles(file.getAbsolutePath(), fileList);
            } else {
                fileList.add(file.getAbsolutePath());
            }
        }
    }

    public static boolean deleteDir(File dir) {
        if (dir.isDirectory()) {
            String[] children = dir.list();
            for (int i = 0; i < children.length; i++) {
                boolean success = deleteDir(new File(dir, children[i]));
                if (!success) {
                    return false;
                }
            }
        }
        return dir.delete();
    }

    public static void setSharedPreference(Context context, String key, String value) {
        if (key == null) {
            Log.d(TAG, "setSharedPreference Key is null");
            return;
        }

        if (value == null) {
            value = "";
        }

        SharedPreferences mySharedPreferences = context.getSharedPreferences(
                SHARED_PREFERENCE_NAME, 0);
        SharedPreferences.Editor editor = mySharedPreferences.edit();
        editor.putString(key, value);
        editor.commit();
    }

    public static String getSharedPreference(Context context, String key) {
        if (key == null) {
            Log.d(TAG, "getSharedPreference Key is null");
            return "";
        }

        SharedPreferences mySharedPreferences = context.getSharedPreferences(
                SHARED_PREFERENCE_NAME, 0);
        return mySharedPreferences.getString(key, "");
    }

    /**
     * Get the prop value, if couldn't find the prop, return the default value.
     */
    public static String getValue(String propKey, String defValue) {
        return SystemProperties.get(propKey, defValue);
    }

    public static boolean getValue(String propKey, boolean defValue) {
        return SystemProperties.getBoolean(propKey, defValue);
    }

    /**
     * Set the prop value.
     */
    public static void setValue(String propKey, String newValue) {
        SystemProperties.set(propKey, newValue);
    }

    public static void saveCurrentSims(Context context) {
        if (DEBUG) Log.d(TAG, "Save current Sims' IMSI.");
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(
                Context.TELEPHONY_SERVICE);
        final int phoneCount = telephonyManager.getPhoneCount();
        final SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
        for (int i = 0; i < phoneCount; i++) {
            final SubscriptionInfo sir = SubscriptionManager.from(context)
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            if (sir == null) {
                if (DEBUG) Log.d(TAG, "Slot:" + i + " is null");
            } else {
                final int subId = sir.getSubscriptionId();
                String imsi = telephonyManager.getSubscriberId(subId);
                if (DEBUG) Log.d(TAG, "Slot:" + i + "'s subId:" + subId + " imsi:" + imsi);
                setSharedPreference(context, SHARED_PREFERENCE_KEY_SUB_SLOT_PRE + i, imsi);
            }
        }
    }

    public static void saveCurrentSims(Context context, int subMask) {
        if (subMask == INVALID_SUB_MASK) {
            saveCurrentSims(context);
            return;
        }

        if (DEBUG) Log.d(TAG, "Save current Sims' IMSI. subMask=" + subMask);
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(
                Context.TELEPHONY_SERVICE);
        final int phoneCount = telephonyManager.getPhoneCount();
        final SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
        for (int i = 0; i < phoneCount; i++) {
            if ((subMask & (0x0001 << i)) != INVALID_SUB_MASK) {
                final SubscriptionInfo sir = SubscriptionManager.from(context)
                        .getActiveSubscriptionInfoForSimSlotIndex(i);
                if (sir == null) {
                    if (DEBUG) Log.d(TAG, "Slot:" + i + " is null");
                } else {
                    final int subId = sir.getSubscriptionId();
                    String imsi = telephonyManager.getSubscriberId(subId);
                    if (DEBUG) Log.d(TAG, "Slot:" + i + "'s subId:" + subId + " imsi:" + imsi);
                    setSharedPreference(context, SHARED_PREFERENCE_KEY_SUB_SLOT_PRE + i, imsi);
                }
            }
        }
    }

    public static int getNewSimsSubMask(Context context) {
        if (DEBUG) Log.d(TAG, "Check whether there are new SIM cards.");
        int newSimsSubMask = INVALID_SUB_MASK;
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(
                Context.TELEPHONY_SERVICE);
        final int phoneCount = telephonyManager.getPhoneCount();
        final SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
        for (int i = 0; i < phoneCount; i++) {
            final SubscriptionInfo sir = SubscriptionManager.from(context)
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            if (sir == null) {
                if (DEBUG) Log.d(TAG, "Slot:" + i + " is null");
            } else {
                final int subId = sir.getSubscriptionId();
                String imsi = telephonyManager.getSubscriberId(subId);
                if (DEBUG) Log.d(TAG, "Slot:" + i + "'s subId:" + subId + " imsi:" + imsi);
                String lastImsi = getSharedPreference(context,
                        SHARED_PREFERENCE_KEY_SUB_SLOT_PRE + i);
                if (DEBUG) Log.d(TAG, "Slot:" + i + " last Imsi:" + lastImsi);
                if (TextUtils.isEmpty(lastImsi) || !lastImsi.equals(imsi)) {
                    if (DEBUG) Log.d(TAG, "Slot " + i + " has a new SIM card.");
                    newSimsSubMask |= 0x0001 << i;
                }
            }
        }

        return newSimsSubMask;
    }

    public static void resetSimsInfo(Context context) {
        if (DEBUG) Log.d(TAG, "Reset SIM cards' information.");
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(
                Context.TELEPHONY_SERVICE);
        final int phoneCount = telephonyManager.getPhoneCount();
        for (int i = 0; i < phoneCount; i++) {
            setSharedPreference(context, SHARED_PREFERENCE_KEY_SUB_SLOT_PRE + i, "");
        }
    }

    // Get the preset partitions of Carrier packages.
    public static ArrayList<String> getRegionalPartitions() {
        String systemPos = RegionalizationEnvironment.getStoragePos();
        ArrayList<String> partitions = new ArrayList<String>();
        partitions.add(systemPos);
        if (RegionalizationEnvironment.isSupported()) {
            try {
                IRegionalizationService regionalizationService =
                        RegionalizationEnvironment.getRegionalizationService();
                ArrayList<String> partitionInfo = (ArrayList<String>)
                        regionalizationService.readFile(PARTITION_INFO_FILE, null);
                if (partitionInfo != null && partitionInfo.size() > 0) {
                    for (String partition : partitionInfo) {
                        boolean exists =
                                regionalizationService.checkFileExists(
                                partition + "/flag");
                        if (exists) {
                            partitions.add(partition);
                        }
                    }
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        return partitions;
    }

    public synchronized static boolean isPresetMode() {
        return RegionalizationEnvironment.isSupported()
                && !getValue(PROP_KEY_BLOCK_BASED_ENABLED, false);
    }

    public static String getSecSdcardPath(Context context) {
        StorageManager sm = (StorageManager)context.getSystemService(context.STORAGE_SERVICE);
        String[] volumePaths = sm.getVolumePaths();
        File externalStorageDirectory = Environment.getExternalStorageDirectory();
        if (Utils.DEBUG) Log.d(TAG,
                "externalStorageDirectory=" + externalStorageDirectory.getAbsolutePath());
        for (String volumePath : volumePaths) {
            if (Utils.DEBUG) Log.d(TAG, "volumePath=" + volumePath);
            if (!volumePath.startsWith(externalStorageDirectory.getAbsolutePath())) {
                return volumePath;
            }
        }
        return null;
    }

    public static boolean isInExcludedPackageList(String packageName) {
        if (TextUtils.isEmpty(packageName)) {
            return false;
        }
        for (String prefix : LEVEL_ALL_EXCLUDED_PACKAGE_PREFIX_LIST) {
            if (packageName.startsWith(prefix)) {
                return true;
            }
        }
        for (String excludedPackageName : LEVEL_ALL_EXCLUDED_PACKAGE_LIST) {
            if (packageName.equals(excludedPackageName)) {
                return true;
            }
        }
        return false;
    }

    public static void addLastPackages(List<PackageInfo> packages) {
        for (String lastPackage : LEVEL_ALL_LAST_PACKAGE_LIST) {
            if (DEBUG) Log.d(TAG, "Add to the last of packages list: " + lastPackage);
            PackageInfo packageInfo = new PackageInfo();
            packageInfo.packageName = lastPackage;
            packages.add(packageInfo);
        }
    }

    public static ArrayList<PackageInfo> getLevelDefaultPackageList() {
        ArrayList<PackageInfo> packages = new ArrayList<PackageInfo>();
        for (String packageName : LEVEL_DEFAULT_PACKAGE_LIST) {
            PackageInfo packageInfo = new PackageInfo();
            packageInfo.packageName = packageName;
            packages.add(packageInfo);
        }
        return packages;
    }

    public static void sendMyBroadcast(Context context,Intent mIntent,String value){
        Bundle bundle=new Bundle();
        bundle.putString(ENABLE_TRIGGER_NAME, value);
        mIntent.putExtras(bundle);
        context.sendBroadcast(mIntent);
    }
}

/*************************************************************************
 Copyright (c)2012-2013,2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*************************************************************************/

package com.qualcomm.qti.presenceappSub2;

import java.util.ArrayList;
import java.util.Timer;


import com.android.ims.internal.uce.options.IOptionsService;
import com.android.ims.internal.uce.presence.IPresenceService;
import com.qualcomm.qti.presenceappSub2.MainActivity.ContactArrayAdapter;
import com.qualcomm.qti.presenceappSub2.Settings.SettingsMainThreadHandler;
import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.android.ims.internal.uce.common.UceLong;
import com.android.ims.internal.uce.common.CapInfo;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Looper;
import android.os.RemoteException;
import android.telephony.SubscriptionManager;
import java.util.HashMap;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionInfo;
import java.util.List;
import com.android.ims.ImsConfig;
import com.android.ims.ImsManager;
import com.android.ims.ImsServiceBase;
import android.util.Log;

public class AppGlobalState {

    public static final String IMS_PRESENCE_MY_INFO = "ImsPresencePrefMyInfo";
    public static final String IMS_PRESENCE_PAR = "ImsPresencePrefPar";
    public static final String IMS_PRESENCE_SETTINGS = "ImsPresencePrefSettings";
    public static final String IMS_PRESENCE_LIVE_LOGGING = "LiveLoggingActivity.txt";
    public static final String IMS_PRESENCE_NOTIFY_XML_FILE = "notify.xml";

    private static int operatorMode = 0;
    public static final int VZW_MODE = 0;
    public static final int ATT_MODE = 5;
    private static boolean cdByOption = false;
    private static boolean bCustomFtEnabled = false;
    public static final int TMO_MODE = 9;

    private static ListenerHandler mListenerHandler;
    private static Looper mListenerLooper;
    private static int mPendingSubscriptionUriIndex;
    private static ProgressDialog mProgressDialog;
    private static ArrayList<Contact> mContacts;
    private static ContactArrayAdapter<Contact> mAdapter;
    private static ContactInfo mContactInfo;
    private static IUceService iqRcsService;
    private static IPresenceService iqPresService;
    private static IOptionsService iqCdService;
    private static int presenceSerrviceHandle = 0;
    private static UceLong presenceListenerHandle = new UceLong();
    private static UceLong optionsListenerHandle = new UceLong();
    private static int cdServiceHandle = 0;
    private static Context mMainActivityContext;
    private static SettingsMainThreadHandler mSettingsHandler;
    private static String imsEnablerState;
    private static int pUserData = 0;
    private static CapInfo cdInfo = new CapInfo();
    private static ImsUceManager mUceManager = null;
    private static int iUceManagerId = 11001;

    public static boolean isDataSettingNvSame = false;

    final static int IMSP_ETAG_EXPIRED  = 0x01;
    final static int IMSP_RAT_CHANGE_LTE   = 0x02;
    final static int IMSP_RAT_CHANGE_EHRPD   = 0x3;
    final static int IMSP_AIRPLANE_MODE   = 0x04;

    final static int NOTIFY_FMT_XML = 0;
    final static int NOTIFY_FMT_STRUCT = 1;

    private static SharedPreferences mMyInfoSettingsHandle;

    private static Timer timerManager;

    private static long qrcsImsSettingsclienthandle  = 0;

    public static String mIccId = null;
    public static final int INVALID_PHONE_ID = -1;
    public static int mSubId = INVALID_PHONE_ID;
    public static int mPhoneid = INVALID_PHONE_ID;
    private static ImsConfig mImsConfig = null;

    public static ArrayList<RequestInfo> requestinfo = new ArrayList<RequestInfo>();
    public static ArrayList<RequestInfo> listCapPollingRequestinfo = new ArrayList<RequestInfo>();

    public static ArrayList<String> excludedNumberList = new ArrayList<String>();

    public static String[] exFeatureTags = { "+g.3gpp.iari-ref=\"urn:urn-7:3gpp-application.ims.iari.rcs.mnc01.mcc072.fullsfgroupchatMyflavor001\"",
                                             "+g.3gpp.iari-ref=\"urn:urn-7:3gpp-application.ims.iari.rcs.mnc01.mcc072.FileTransferMyflavor001\"",
                                             "+g.3gpp.iari-ref=\"urn:urn-7:3gpp-application.ims.iari.rcs.mnc01.mcc072.ShortMessageMyflavor001\"" };

    public static String[] exServiceDesc = { "org.openmobilealliance:ChatSession.mnc072.mcc001.MyGCFlavor001",
                                             "org.openmobilealliance:FileTransfer.mnc072.mcc001.MyGCFlavor001",
                                             "org.openmobilealliance:ShortMessage.mnc072.mcc001.MyGCFlavor001" };
    public static String[] exServiceId = { "My Flavor ChatSession",
                                           "My Flavor FileTransfer",
                                           "My Flavor ShortMessage" };

    static public ListenerHandler getListenerHandler() {
        return mListenerHandler;
    }

    static public void setListenerHandler(ListenerHandler listenerHandler) {
        mListenerHandler = listenerHandler;
    }

    public static void setListenerLooper(Looper listenerLooper) {
        mListenerLooper = listenerLooper;

    }

    public static  Looper getListenerLooper() {
        return mListenerLooper;

    }

    public static void setPendingSubscriptionUri(int contactIndex) {
        mPendingSubscriptionUriIndex = contactIndex;
    }

    public static int getPendingSubscriptionUri() {
        return mPendingSubscriptionUriIndex;
    }

    public static void setProgressDialog(ProgressDialog p) {
        mProgressDialog = p;
    }

    public static ProgressDialog getProgressDialog() {
        return mProgressDialog;
    }

    public static void setContacts(ArrayList<Contact> contacts) {
        mContacts = contacts;
    }

    public static ArrayList<Contact> getContacts() {
        return mContacts;
    }

    public static void setMainListAdapter(ContactArrayAdapter<Contact> adapter) {
        mAdapter = adapter;
    }

    public static ContactArrayAdapter<Contact> getMainListAdapter() {
        return mAdapter;
    }

    public static void setContactInfo(ContactInfo contactInfo) {
        mContactInfo = contactInfo;
    }

    public static ContactInfo getContactInfo( ) {
        return mContactInfo;
    }

    public static void setMyInfoSettingHandle(SharedPreferences setting) {
        mMyInfoSettingsHandle = setting;
    }
    public static SharedPreferences  getMyInfoSettingHandle( ) {
        return mMyInfoSettingsHandle;
    }

    public static void setPresenceService(IPresenceService p) {
        iqPresService = p;
    }
    public static IPresenceService getPresenceService() {
        return iqPresService;
    }

    public static void setTimerManager(Timer tm) {
        timerManager = tm;
    }
    public static Timer getTimerManager() {
        return timerManager;
    }

    public static void setMainActivityContext(Context mContext) {
        mMainActivityContext = mContext;
    }
    public static Context getMainActivityContext() {
        return mMainActivityContext;
    }

    public static void setSettingsHandler(
            SettingsMainThreadHandler settingsHandler) {
        mSettingsHandler = settingsHandler;
    }

    public static SettingsMainThreadHandler getSettingsHandler() {
        return mSettingsHandler;
    }

    public static IUceService getIqRcsService() {
        return iqRcsService;
    }

    public static void setIqRcsService(IUceService iqRcsService) {
        AppGlobalState.iqRcsService = iqRcsService;
    }

    public static int getPresenceSerrviceHandle() {
        return presenceSerrviceHandle;
    }

    public static void setPresenceSerrviceHandle(int presenceSerrviceHandle) {
        AppGlobalState.presenceSerrviceHandle = presenceSerrviceHandle;
    }
  public static ImsConfig getImsConfig() {
    if(mPhoneid == INVALID_PHONE_ID) {
        Log.d("AppGlobalState | Imsconfig","Invalid Phone ID");
        return null;
    }
    if(mImsConfig == null) {
        ImsManager imsManager = ImsManager.getInstance(getMainActivityContext(),mPhoneid);
      if(imsManager!=null){
        try{
          mImsConfig = imsManager.getConfigInterface();
        } catch(Exception e) {
          Log.d("AppGlobalState | Imsconfig","exception in init ");
          e.printStackTrace();
          }
        }
      }
    return mImsConfig;
  }

  public static boolean objectValidator(Object obj){
    if(obj == null)
      return false;
        if(obj instanceof String){
           if(obj == null || obj.equals("") || obj.toString().isEmpty() || obj.equals("null"))
           return false;
    }
    return true;
  }


    public static String getImsEnablerState() {
        return imsEnablerState;
    }

    public static void setImsEnablerState(String imsEnablerState) {
        AppGlobalState.imsEnablerState = imsEnablerState;
    }

    public static UceLong getOptionsListenerHandle() {
        return optionsListenerHandle;
    }
    public static void setOptionsListenerHandle(UceLong optionsListenerHandle) {
        AppGlobalState.optionsListenerHandle = optionsListenerHandle;
    }
    public static UceLong getPresenceListenerHandle() {
        return presenceListenerHandle;
    }

    public static void setPresenceListenerHandle(UceLong presenceListenerHandle) {
        AppGlobalState.presenceListenerHandle = presenceListenerHandle;
    }

    public static int getpUserData() {
        return pUserData++;
    }

    public static void setpUserData(int pUserData) {
        AppGlobalState.pUserData = pUserData;
    }

    public static int getpUserDataValue() {
        return pUserData;
    }

    public static long getQrcsImsSettingsclienthandle() {
        return qrcsImsSettingsclienthandle;
    }

    public static void setQrcsImsSettingsclienthandle(
            long qrcsImsSettingsclienthandle) {
        AppGlobalState.qrcsImsSettingsclienthandle = qrcsImsSettingsclienthandle;
    }

    public static int getOperatorMode() {
        return operatorMode;
    }

    public static void setOperatorMode(int operatorMode) {
        AppGlobalState.operatorMode = operatorMode;
    }

    public static boolean isCdByOption() {
        return cdByOption;
    }

    public static void setCdByOption(boolean cdByOption) {
        AppGlobalState.cdByOption = cdByOption;
    }

    public static IOptionsService getCdService() {
        return iqCdService;
    }

    public static void setCdService(IOptionsService iqCdService) {
        AppGlobalState.iqCdService = iqCdService;
    }

    public static int getCdServiceHandle() {
        return cdServiceHandle;
    }
    public static void SetCdInfo(CapInfo cdInfo) {
        AppGlobalState.cdInfo = cdInfo;
    }
    public static CapInfo GetCdInfo() {
        return AppGlobalState.cdInfo;
    }

    public static void setCdServiceHandle(int cdServiceHandle) {
        AppGlobalState.cdServiceHandle = cdServiceHandle;
    }
    public static void setCustomFTEnabled(boolean bCustomFtEnabled) {
        AppGlobalState.bCustomFtEnabled = bCustomFtEnabled;
    }
    public static boolean isCustomFTEnabled() {
        return AppGlobalState.bCustomFtEnabled;
    }

    public static String[] GetCustomFT() {
        return AppGlobalState.exFeatureTags;
    }

    public static String[] GetCustomFTSvcDesc() {
        return AppGlobalState.exServiceDesc;
    }
    public static String[] GetCustomFTSvcID() {
        return AppGlobalState.exServiceId;
    }
    public static ImsUceManager GetUceManager(){
        if(AppGlobalState.mUceManager == null)
        {
            AppGlobalState.mUceManager = ImsUceManager.getInstance(getMainActivityContext(), iUceManagerId);
        }
        return AppGlobalState.mUceManager;
    }

}

class RequestInfo
{
    public int userData = 0;
    public int requestID = 0;
    public String [] URI;
}

/*************************************************************************
 Copyright (c)2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
*************************************************************************/

package com.qualcomm.qti.presenceapp;


import java.util.ArrayList;

import com.android.ims.internal.uce.options.*;
import com.android.ims.internal.uce.presence.*;
import com.qualcomm.qti.presenceapp.MainActivity.ContactArrayAdapter;
import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.common.CapInfo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;
import java.lang.Thread;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

@SuppressLint("HandlerLeak")
public class ListenerHandler extends Handler
{

    private static String TAG = "ListenerHandler";
    Context mContext;
    private final short PRESENCE_IMS_UNSOL_PUBLISH_TRIGGER = 1;
    private final short PRESENCE_IMS_UNSOL_NOTIFY_UPDATE = 2;
    private final short PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_AVAILABILITY = 3;
    private final short PRESENCE_IMS_UNSOL_ENABLER_STATE = 4;
    private final short PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_CMDSTATUS = 5;
    private final short PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE = 6;
    private final short PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE404 = 7;
    private final short PRESENCE_IMS_CD_CMD_STATUS = 8;
    private final short PRESENCE_IMS_CD_INCOMING_OPTIONS = 9;
    private final short PRESENCE_IMS_CD_RESP_INCOMING_OPTIONS = 10;
    private final short PRESENCE_IMS_UNPUBLISH_MSG_SENT = 11;


    public static boolean errorSipHandle = false;

   public ListenerHandler(Context context, Looper looper) {
        super(looper);
        mContext = context;
    }

    @Override
    public void handleMessage(Message msg) {
        super.handleMessage(msg);

        Log.d(TAG, "Thread=" + Thread.currentThread().getName() + " received "
                + msg);
    }

    public IPresenceListener presListener = new IPresenceListener.Stub() {

        public boolean onTransact(int code, Parcel data, Parcel reply, int flags) throws RemoteException
        {
            try
            {
                    return super.onTransact(code, data, reply, flags);
            }
            catch (RuntimeException e)
            {
                Log.w("ListenerHandler", "Unexpected remote exception", e);
                throw e;
            }
        }

        public void sipResponseReceived(PresSipResponse pSipResponse)
                throws RemoteException {

            Log.d("PRESENCE_UI",
                    "ListenerHandler : SipResponseReceived : pSipResponse.getCmdId() "
                            + pSipResponse.getCmdId().getCmdId());
            Log.d("PRESENCE_UI",
                    "ListenerHandler : SipResponseReceived : pSipResponse.getReasonPhrase() "
                            + pSipResponse.getReasonPhrase());
            Log.d("PRESENCE_UI",
                    "ListenerHandler : SipResponseReceived : pSipResponse.getsRequestID() "
                            + pSipResponse.getRequestId());
            Log.d("PRESENCE_UI",
                    "ListenerHandler : SipResponseReceived : pSipResponse.getsSipResponseCode() "
                            + pSipResponse.getSipResponseCode());
            if (pSipResponse.getSipResponseCode() != 200) {

                switch (pSipResponse.getCmdId().getCmdId()) {

                case PresCmdId.UCE_PRES_CMD_PUBLISHMYCAP: {
                    Log.d(TAG,
                            "SipResponseCode:"
                                    + pSipResponse.getSipResponseCode());
                    if (pSipResponse.getSipResponseCode() == 999) {
                        Message updateUiMesgSipPub = uiThreadHandler
                                .obtainMessage(
                                        PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE,
                                        "Publish ignored - No capability change");
                        uiThreadHandler.sendMessage(updateUiMesgSipPub);
                    } else if (pSipResponse.getSipResponseCode() == 500
                            || pSipResponse.getSipResponseCode() == 503) {
                        Log.d(TAG,
                                "SipResponseCode:"
                                        + pSipResponse.getSipResponseCode());
                        Log.d(TAG,
                                "Retry Publish after "
                                        + pSipResponse.getRetryAfter()
                                        + " secs");

                    } else if (AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE
                            && pSipResponse.getSipResponseCode() == 489) {
                        if (AppGlobalState.getCdService() != null) {

                            Log.d("PRESENCE_UI",
                                    "ATT_MODE :AppGlobalState.getCdService() NOT NULL - QRCS_PRES_CMD_PUBLISHMYCAP");
                            CapInfo myCdInfo = AppGlobalState.GetCdInfo();

                            errorSipHandle = true;


                            Log.d("PRESENCE_UI","ATT_MODE :errorSipHandle:"+errorSipHandle);

                            /* AppGlobalState
                                    .getCdService()
                                    .QCDService_GetMyCdInfo(
                                            AppGlobalState.getCdServiceHandle(),
                                            AppGlobalState.getpUserData()); */
                            myCdInfo.setIpVoiceSupported(false);
                            myCdInfo.setIpVideoSupported(false);

                            myCdInfo.setFtSupported(false);
                            myCdInfo.setImSupported(false);
                            myCdInfo.setSmSupported(false);

                            myCdInfo.setCdViaPresenceSupported(false);


                            if(AppGlobalState.isCustomFTEnabled())
                            {
                                myCdInfo.setExts(AppGlobalState.GetCustomFT());
                            }

                            AppGlobalState
                                    .getCdService()
                                    .setMyInfo(
                                            AppGlobalState.getCdServiceHandle(),
                                            myCdInfo, AppGlobalState.getpUserData());
                        }
                    } else if (AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE &&
                               ((pSipResponse.getSipResponseCode() == 403 &&
                                 pSipResponse.getReasonPhrase().toString().equals("NOT AUTHORIZED FOR PRESENCE")) ||
                                 pSipResponse.getSipResponseCode() == 404)) {

                        if (AppGlobalState.getCdService() != null) {
                            Log.d("PRESENCE_UI",
                                    "VZW_MODE : AppGlobalState.getCdService() NOT NULL - QRCS_PRES_CMD_PUBLISHMYCAP");
                            CapInfo myCdInfo = AppGlobalState.GetCdInfo();

                            errorSipHandle = true;


                            Log.d("PRESENCE_UI","VZW_MODE :errorSipHandle:"+errorSipHandle);

                            /* AppGlobalState
                                    .getCdService()
                                    .QCDService_GetMyCdInfo(
                                            AppGlobalState.getCdServiceHandle(),
                                            AppGlobalState.getpUserData()); */
                            myCdInfo.setIpVoiceSupported(false);
                            myCdInfo.setIpVideoSupported(false);

                            myCdInfo.setFtSupported(false);
                            myCdInfo.setImSupported(false);
                            myCdInfo.setFullSnFGroupChatSupported(false);
                            myCdInfo.setCdViaPresenceSupported(false);

                            if(AppGlobalState.isCustomFTEnabled()) {
                                myCdInfo.setExts(AppGlobalState.GetCustomFT());
                            }
                            AppGlobalState
                                    .getCdService()
                                    .setMyInfo(
                                            AppGlobalState.getCdServiceHandle(),
                                            myCdInfo, AppGlobalState.getpUserData());
                        }
                    } else {
                        Message updateUiMesgSipPub = uiThreadHandler
                                .obtainMessage(
                                        PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE,
                                        "Publish Sip Response Code "
                                                + pSipResponse
                                                        .getSipResponseCode());
                        uiThreadHandler.sendMessage(updateUiMesgSipPub);
                    }
                    break;
                }
                case PresCmdId.UCE_PRES_CMD_GETCONTACTCAP: {
                    if ((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE && pSipResponse
                            .getSipResponseCode() == 404)
                            || (AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE && pSipResponse
                                    .getSipResponseCode() == 489)) {
                        updateStatus(null, null, null, null,
                                pSipResponse.getRequestId());
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : CMDStatus : index 11112222223");
                        Message updateUiMesgSipContactCap = uiThreadHandler
                                .obtainMessage(
                                        PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE404,
                                        "Availability Sip Response Code "
                                                + pSipResponse
                                                        .getSipResponseCode());
                        uiThreadHandler.sendMessage(updateUiMesgSipContactCap);
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : CMDStatus : index 34444444445");

                    } else {
                        Message updateUiMesgSipContactCap = uiThreadHandler
                                .obtainMessage(
                                        PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE,
                                        "Availability Sip Response Code "
                                                + pSipResponse
                                                        .getSipResponseCode());
                        uiThreadHandler.sendMessage(updateUiMesgSipContactCap);
                    }
                    break;
                }
                case PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP: {
                    for (int i = 0; i < AppGlobalState.listCapPollingRequestinfo
                            .size(); i++) {
                        if (pSipResponse.getRequestId() == AppGlobalState.listCapPollingRequestinfo
                                .get(i).requestID) {
                            Log.d(TAG, "listCapPollingRequestinfo.remove(i) "
                                    + i);
                            AppGlobalState.listCapPollingRequestinfo.remove(i);
                            ListCapabilityPollingTask
                                    .sendListCapPollingFromQueue();
                            break;
                        }
                    }
                    if ((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE && pSipResponse
                            .getSipResponseCode() == 404)
                            || (AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE && pSipResponse
                                    .getSipResponseCode() == 489)) {
                        updateStatus(null, null, null, null,
                                pSipResponse.getRequestId());
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : CMDStatus : index 111111111111");
                        Message updateUiMesgSipContactListCap = uiThreadHandler
                                .obtainMessage(
                                        PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE404,
                                        "Polling Sip Response Code "
                                                + pSipResponse
                                                        .getSipResponseCode());
                        uiThreadHandler
                                .sendMessage(updateUiMesgSipContactListCap);
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : CMDStatus : index 22222222222222");
                    } else {
                        Message updateUiMesgSipContactListCap = uiThreadHandler
                                .obtainMessage(
                                        PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE,
                                        "Polling Sip Response Code "
                                                + pSipResponse
                                                        .getSipResponseCode());
                        uiThreadHandler
                                .sendMessage(updateUiMesgSipContactListCap);
                    }
                    break;
                }
                case PresCmdId.UCE_PRES_CMD_SETNEWFEATURETAG: {
                    Message updateUiMesgSipNewTag = uiThreadHandler
                            .obtainMessage(
                                    PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE,
                                    "New Tag Sip Response Code "
                                            + pSipResponse.getSipResponseCode());
                    uiThreadHandler.sendMessage(updateUiMesgSipNewTag);
                    break;
                }
                default:
                    Log.d(TAG, "CMD ID for unknown value="
                            + pSipResponse.getCmdId().getCmdId());
                }
            }
            else if ((pSipResponse.getSipResponseCode() == 200) && (pSipResponse.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP))
            {
                for (int i = 0; i < AppGlobalState.listCapPollingRequestinfo
                            .size(); i++) {
                        if (pSipResponse.getRequestId() == AppGlobalState.listCapPollingRequestinfo
                                .get(i).requestID) {
                            Log.d(TAG, "listCapPollingRequestinfo.remove(i) "
                                    + i);
                            AppGlobalState.listCapPollingRequestinfo.remove(i);
                            ListCapabilityPollingTask
                                    .sendListCapPollingFromQueue();
                            break;
                        }
                    }
            }
        }

        public void serviceUnAvailable(StatusCode statusCode) throws RemoteException {

            Log.d("PRESENCE_UI", "ListenerHandler : ServiceUnAvailable : statusCode "+statusCode.getStatusCode());
            AppGlobalState.setImsEnablerState("UN-INITIALIZED");
            Message updateUiMesg = uiThreadHandler
                    .obtainMessage(
                            PRESENCE_IMS_UNSOL_ENABLER_STATE,
                            "UN-INITIALIZED");
            uiThreadHandler.sendMessage(updateUiMesg);
        }

        public void serviceAvailable(StatusCode statusCode) throws RemoteException {

            Log.d("PRESENCE_UI", "ListenerHandler : ServiceAvailable : statusCode "+statusCode.getStatusCode());
            AppGlobalState.setImsEnablerState("REGISTERED");
            Message updateUiMesg = uiThreadHandler
                    .obtainMessage(
                            PRESENCE_IMS_UNSOL_ENABLER_STATE,
                            "REGISTERED");
            uiThreadHandler.sendMessage(updateUiMesg);


      }

        private boolean getFeatureStatus(String key) {

            SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
            boolean featureSupported = preferences.getBoolean(key, false);
            Log.d("ListenerHandler", "Key:"+key+":featureSupported:"+featureSupported);
            return featureSupported;
         }


        public void publishTriggering(final PresPublishTriggerType publishTrigger) throws RemoteException {

            Log.d("PRESENCE_UI",
                    "ListenerHandler : PublishTriggering : publishTrigger.getPublishTrigeerType() "
                            + publishTrigger.getPublishTrigeerType());
            Log.d("PRESENCE_UI",
                    "ListenerHandler : PublishTriggering : publishTrigger.getPublishTrigeerType() "
                            + publishTrigger.getPublishTrigeerType());

            Runnable show_UI = new Runnable()
            {
                public void run()
                {
                    try
                    {
                        SharedPreferences preferences = mContext.getSharedPreferences(
                                "ImsPresencePrefMyInfo", Context.MODE_PRIVATE);
                        SharedPreferences.Editor editor = preferences.edit();

                        boolean m_networkTypeIWLAN = ContactInfo.networkTypeIWLAN;
                        boolean m_networkTypeLTE = ContactInfo.networkTypeLTE;
                        boolean m_vopsEnabled = ContactInfo.vopsEnabled;
                        boolean m_networkTypeEHRPD = ContactInfo.networkTypeEHRPD;

                        ContactInfo.networkTypeIWLAN = false;
                        ContactInfo.networkTypeLTE = false;
                        ContactInfo.vopsEnabled = false;
                        ContactInfo.networkTypeEHRPD = false;

                        Log.d(TAG, "Runnable ; Make all network type to defaults - false ");
                        switch (publishTrigger.getPublishTrigeerType()) {
                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_ETAG_EXPIRED: {
                                Log.d(TAG, "Publish trigger for IMSP_ETAG_EXPIRED");
                                Log.d(TAG, "Schedule publish.");

                                Log.d(TAG, "Reassign the saved values ");

                                ContactInfo.networkTypeIWLAN = m_networkTypeIWLAN;
                                ContactInfo.networkTypeLTE = m_networkTypeLTE;
                                ContactInfo.vopsEnabled = m_vopsEnabled;
                                ContactInfo.networkTypeEHRPD = m_networkTypeEHRPD;

                                invokePublish();
                                break;

                            }

                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN: {

                                 Log.d(TAG,
                                        "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN");

                            break;
                            }

                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN: {

                                Log.d(TAG, "Operator Mode:"+ AppGlobalState.getOperatorMode());
                                Log.d(TAG,
                                    "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN");

                                ContactInfo.networkTypeIWLAN = true;
                                ContactInfo.networkTypeLTE = false;
                                ContactInfo.vopsEnabled = false;
                                ContactInfo.networkTypeEHRPD = false;
                                editor.putString("Basic Status", "Open");
                                editor.commit();
                                Log.d(TAG, "Schedule publish:networkTypeIWLAN:"+ContactInfo.networkTypeIWLAN);
                                invokePublish();

                              break;
                            }

                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED: {
                                Log.d(TAG,
                                        "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED");
                                Log.d(TAG, "Status is OPEN");
                                ContactInfo.networkTypeLTE = true;
                                ContactInfo.vopsEnabled = true;
                                ContactInfo.networkTypeEHRPD = false;
                                editor.putString("Basic Status", "Open");
                                editor.commit();
                                Log.d(TAG, "Schedule publish.");
                                invokePublish();

                                break;

                            }
                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED: {
                                Log.d(TAG,
                                        "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED");
                                Log.d(TAG, "Status is CLOSED");
                                ContactInfo.networkTypeLTE = true;
                                ContactInfo.vopsEnabled = false;
                                ContactInfo.networkTypeEHRPD = false;
                                editor.putString("Basic Status", "Closed");
                                editor.commit();
                                Log.d(TAG, "Schedule publish.");
                                invokePublish();
                                break;
                            }
                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_EHRPD: {
                                Log.d(TAG, "Publish trigger for IMSP_RAT_CHANGE_EHRPD");
                                Log.d(TAG, "Status is CLOSED");
                                ContactInfo.networkTypeLTE = false;
                                ContactInfo.vopsEnabled = false;
                                ContactInfo.networkTypeEHRPD = true;
                                editor.putString("Basic Status", "Closed");
                                editor.commit();
                                Log.d(TAG, "Schedule publish.");
                                invokePublish();
                                break;

                            }
                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS:
                            {
                                Log.d(TAG, "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS");
                                Log.d(TAG, "Status is CLOSED");
                                ContactInfo.networkTypeLTE = false;
                                ContactInfo.vopsEnabled = false;
                                ContactInfo.networkTypeEHRPD = true;
                                editor.putString("Basic Status", "Closed");
                                editor.commit();
                                Log.d(TAG, "Schedule publish.");
                                invokePublish();
                                break;
                            }
                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_2G:
                            {
                                Log.d(TAG, "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_2G");
                                Log.d(TAG, "Status is CLOSED");
                                ContactInfo.networkTypeLTE = false;
                                ContactInfo.vopsEnabled = false;
                                ContactInfo.networkTypeEHRPD = true;
                                editor.putString("Basic Status", "Closed");
                                editor.commit();
                                Log.d(TAG, "Schedule publish.");
                                invokePublish();
                                break;
                            }
                            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_3G:
                            {
                                Log.d(TAG, "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_3G");
                                Log.d(TAG, "Status is CLOSED");
                                ContactInfo.networkTypeLTE = false;
                                ContactInfo.vopsEnabled = false;
                                ContactInfo.networkTypeEHRPD = true;
                                editor.putString("Basic Status", "Closed");
                                editor.commit();
                                Log.d(TAG, "Schedule publish.");
                                invokePublish();
                                break;
                            }
                            default:
                                Log.d(TAG,
                                        "Publish trigger for unknown value="
                                                + publishTrigger.getPublishTrigeerType());
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            };
            Log.d("PRESENCE_UI", "ListenerHandler : before posting to UI tread");
            MainActivity.mainActivityObject.runOnUiThread(show_UI);
            Log.d("PRESENCE_UI", "ListenerHandler : after posting to UI tread");

            Message updateUiMesg = uiThreadHandler.obtainMessage(
                    PRESENCE_IMS_UNSOL_PUBLISH_TRIGGER, publishTrigger);
            uiThreadHandler.sendMessage(updateUiMesg);

            Log.d("PRESENCE_UI", "ListenerHandler : after calling ui update");
        }

        public void listCapInfoReceived(PresRlmiInfo pRlmiInfo, PresResInfo[] pResInfo)
                throws RemoteException {

            if(pRlmiInfo != null)
            {
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getListName "+pRlmiInfo.getListName());
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.isFullState "+pRlmiInfo.isFullState());
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getUri "+pRlmiInfo.getUri());
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getVersion "+pRlmiInfo.getVersion());
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getRequestID "+pRlmiInfo.getRequestId());
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getPresSubscriptionState "+pRlmiInfo.getPresSubscriptionState().getPresSubscriptionStateValue());
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getSubscriptionExpireTime "+pRlmiInfo.getSubscriptionExpireTime());
                if(AppGlobalState.listCapPollingRequestinfo.size() > 0)
                {
                    if(pRlmiInfo.getSubscriptionExpireTime() > 0)
                    {
                        ListCapabilityPollingTask.subscriptionExpireTime = pRlmiInfo.getSubscriptionExpireTime();
                    }
                    else
                    {
                        ListCapabilityPollingTask.subscriptionExpireTime = 30;
                    }
                }
                Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pRlmiInfo.getSubscriptionTerminatedReason "+pRlmiInfo.getSubscriptionTerminatedReason());
                for(int i = 0; i < AppGlobalState.listCapPollingRequestinfo.size(); i++)
                {
                    if (pRlmiInfo.getRequestId() == AppGlobalState.listCapPollingRequestinfo.get(i).requestID
                            && pRlmiInfo.getPresSubscriptionState().getPresSubscriptionStateValue()
                                     == PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_TERMINATED )
                    {
                        Log.d(TAG, "listCapPollingRequestinfo.remove(i) " + i);
                        AppGlobalState.listCapPollingRequestinfo.remove(i);
                        ListCapabilityPollingTask.sendListCapPollingFromQueue();
                        break;
                    }
                }
            }
            if(pResInfo != null)
            {
                for(int i=0; i < pResInfo.length; i++ )
                {
                    Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getDisplayName() "+pResInfo[i].getDisplayName());
                    Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getResUri() "+pResInfo[i].getResUri());
                    Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo.getInstanceInfo().getPresentityUri() "+pResInfo[i].getInstanceInfo().getPresentityUri());
                    Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getResId() "+pResInfo[i].getInstanceInfo().getResId());
                    Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getsReason() "+pResInfo[i].getInstanceInfo().getReason());
                    Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getResInstanceState() "+pResInfo[i].getInstanceInfo().getResInstanceState());
                    if(pResInfo[i].getInstanceInfo() != null && pResInfo[i].getInstanceInfo().getTupleInfo() != null)
                    {
                        Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getTupleInfo().length "+pResInfo[i].getInstanceInfo().getTupleInfo().length);
                        for(int j = 0; j < pResInfo[i].getInstanceInfo().getTupleInfo().length; j++ )
                        {
                            Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getTupleInfo()[j].getFeatureTag "+pResInfo[i].getInstanceInfo().getTupleInfo()[j].getFeatureTag());
                            Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getTupleInfo()[j].getsContactURI "+pResInfo[i].getInstanceInfo().getTupleInfo()[j].getContactUri());
                            Log.d("PRESENCE_UI", "ListenerHandler : ListCapInfoReceived : pResInfo[i].getInstanceInfo().getTupleInfo()[j].getsTimestamp "+pResInfo[i].getInstanceInfo().getTupleInfo()[j].getTimestamp());
                        }
                    }
                }
            }
            updateStatus(pRlmiInfo, pResInfo, null, null , 0);
            Message updateUiMesg = uiThreadHandler
                    .obtainMessage(
                            PRESENCE_IMS_UNSOL_NOTIFY_UPDATE,
                            pResInfo);
            uiThreadHandler.sendMessage(updateUiMesg);

        }

        public void getVersionCb(String pVersion) throws RemoteException {

            Log.d("PRESENCE_UI", "ListenerHandler : GetVersion : pVersion "+pVersion);
        }

        public void capInfoReceived(String pPresentityURI, PresTupleInfo[] pTupleInfo)
                throws RemoteException {

            if(pPresentityURI != null)
            {
                Log.d("PRESENCE_UI", "ListenerHandler : CapInfoReceived : pPresentityURI "+pPresentityURI);
            }
            if(pTupleInfo != null)
            {
                for(int i = 0; i < pTupleInfo.length; i++)
                {
                    Log.d("PRESENCE_UI", "ListenerHandler : CapInfoReceived : pTupleInfo[i].getFeatureTag() "+pTupleInfo[i].getFeatureTag());
                    Log.d("PRESENCE_UI", "ListenerHandler : CapInfoReceived : pTupleInfo[i].getsContactURI() "+pTupleInfo[i].getContactUri());
                    Log.d("PRESENCE_UI", "ListenerHandler : CapInfoReceived : pTupleInfo[i].getsTimestamp() "+pTupleInfo[i].getTimestamp());
                }
            }

            updateStatus(null, null, pPresentityURI, pTupleInfo, 0);
            Message updateUiMesg = uiThreadHandler
                    .obtainMessage(
                            PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_AVAILABILITY,
                            pPresentityURI);
            uiThreadHandler.sendMessage(updateUiMesg);
        }

        public void unpublishMessageSent() throws RemoteException {
            Log.d("PRESENCE_UI", "ListenerHandler : unpublishMessageSent");
            Message updateUiMesg = uiThreadHandler
                    .obtainMessage(
                            PRESENCE_IMS_UNPUBLISH_MSG_SENT,
                            "UN-PUBLISH SENT");
            uiThreadHandler.sendMessage(updateUiMesg);
      }

        public void cmdStatus(PresCmdStatus pCmdStatus) throws RemoteException {

            Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : pCmdStatus.getRequestID() "+pCmdStatus.getRequestId());
            Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : pCmdStatus.getUserData() "+pCmdStatus.getUserData());

            for(int i = 0; i < AppGlobalState.requestinfo.size(); i++)
            {
                if(pCmdStatus.getUserData() == AppGlobalState.requestinfo.get(i).userData)
                {
                    AppGlobalState.requestinfo.get(i).requestID = pCmdStatus.getRequestId();
                    Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : AppGlobalState.requestinfo.get(i).requestID "+
                            AppGlobalState.requestinfo.get(i).requestID);
                    break;
                }
            }

            for(int i = 0; i < AppGlobalState.listCapPollingRequestinfo.size(); i++)
            {
                if(pCmdStatus.getUserData() == AppGlobalState.listCapPollingRequestinfo.get(i).userData)
                {
                    AppGlobalState.listCapPollingRequestinfo.get(i).requestID = pCmdStatus.getRequestId();
                    Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : AppGlobalState.listCapPollingRequestinfo.get(i).requestID "+
                            AppGlobalState.listCapPollingRequestinfo.get(i).requestID);
                    break;
                }
            }

            Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : pCmdStatus.getCmdId().toString() "+pCmdStatus.getCmdId().getCmdId());
            Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : pCmdStatus.getStatus().toString() "+pCmdStatus.getStatus().getStatusCode());
            if(pCmdStatus.getStatus().getStatusCode() != StatusCode.UCE_SUCCESS)
            {
                switch (pCmdStatus.getCmdId().getCmdId())
                {
                    case PresCmdId.UCE_PRES_CMD_PUBLISHMYCAP:
                    {
                        Message updateUiMesgCmdPub = uiThreadHandler
                        .obtainMessage(
                                PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_CMDSTATUS,
                                "Publish Cmd Response "+pCmdStatus.getStatus().getStatusCode());
                        uiThreadHandler.sendMessage(updateUiMesgCmdPub);
                        break;
                    }
                    case PresCmdId.UCE_PRES_CMD_GETCONTACTCAP:
                    {
                        Message updateUiMesgCmdContactCap = uiThreadHandler
                        .obtainMessage(
                                PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_CMDSTATUS,
                                "Availability Cmd Response "+pCmdStatus.getStatus().getStatusCode());
                        uiThreadHandler.sendMessage(updateUiMesgCmdContactCap);
                        break;
                    }
                    case PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP:
                    {

                        for(int i = 0; i < AppGlobalState.listCapPollingRequestinfo.size(); i++)
                        {
                            if(pCmdStatus.getUserData() == AppGlobalState.listCapPollingRequestinfo.get(i).userData)
                            {
                                Log.d(TAG, "listCapPollingRequestinfo.remove(i) "+i);
                                AppGlobalState.listCapPollingRequestinfo.remove(i);
                                ListCapabilityPollingTask.sendListCapPollingFromQueue();
                                break;
                            }
                        }

                        Message updateUiMesgCmdContactListCap = uiThreadHandler
                        .obtainMessage(
                                PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_CMDSTATUS,
                                "Polling Cmd Response "+pCmdStatus.getStatus().getStatusCode());
                        uiThreadHandler.sendMessage(updateUiMesgCmdContactListCap);
                        break;
                    }
                    case PresCmdId.UCE_PRES_CMD_SETNEWFEATURETAG:
                    {
                        Message updateUiMesgCmdNewTag = uiThreadHandler
                        .obtainMessage(
                                PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_CMDSTATUS,
                                "New Tag Cmd Response "+pCmdStatus.getStatus().getStatusCode());
                        uiThreadHandler.sendMessage(updateUiMesgCmdNewTag);
                        break;
                    }
                    default:
                        Log.d(TAG, "CMD ID for unknown value=" + pCmdStatus.getCmdId().getCmdId());
                }
            }
        }
    };

    private int publishTry = 0;

    private void invokePublish() {
        try
        {
            if(AppGlobalState.isDataSettingNvSame || AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
            {
                if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                    Utility.sendLogMesg(mContext, "Trigger publish ind recvd," +
                            " Publish fmt=STRUCT based.");
                    new PublishTask().execute(false);
                }
            }
            else
            {
                ContactInfo.firstPublish = true;
                Log.d("TAG", "invokePublish : out side runnable");
                try {
                    Log.d("TAG", "invokePublish : before runnable");
                    if(publishTry < 10)
                    {
                        if(Looper.myLooper() == null)
                        {
                            Log.d("TAG", "before Looper.prepare()");
                            Looper.prepare();
                            Log.d("TAG", "after Looper.prepare()");
                        }
                        new Handler().postDelayed(new Runnable() {

                            public void run() {
                                Log.d("TAG", "invokePublish : inside runnable runnable");
                                invokePublish();
                            }
                        }, 500);
                        publishTry++;
                        Looper.loop();
                    }
                    Log.d("TAG", "invokePublish : after runnable");
                } catch (Exception e) {
                    e.printStackTrace();
                }
                Log.d("TAG", "invokePublish : after runnable");
            }
        }catch(Exception e)
        {
            e.printStackTrace();
        }
    }


    private void updateStatus(PresRlmiInfo pRlmiInfo, PresResInfo[] pResInfo, String pPresentityURI, PresTupleInfo[] pTupleInfo , int requestID) {

        ArrayList<Contact> contacts = AppGlobalState.getContacts();


        if(pRlmiInfo == null && pResInfo == null && pTupleInfo == null && pPresentityURI == null && requestID != 0)
        {
            for(int i = 0; i < AppGlobalState.requestinfo.size(); i++)
            {
                if(requestID == AppGlobalState.requestinfo.get(i).requestID)
                {
                    Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : AppGlobalState.requestinfo.get(i).requestID "+AppGlobalState.requestinfo.get(i).requestID);
                    String uriValue;
                    //ArrayList<Contact> contacts = AppGlobalState.getContacts();
                    for(int k = 0; k < AppGlobalState.requestinfo.get(i).URI.length; k++)
                    {
                        //uriValue = getPhoneFromUri(AppGlobalState.requestinfo.get(i).URI[k]);
                        uriValue = AppGlobalState.requestinfo.get(i).URI[k];
                        Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : uriValue "+uriValue);
                        if (AppGlobalState.requestinfo.get(i).URI[k].contains("tel:")) {
                            uriValue = AppGlobalState.requestinfo.get(i).URI[k].substring(4);
                            for(int j = 0; j < uriValue.length(); j++)
                            {
                                if(uriValue.charAt(j) == '@')
                                {
                                    uriValue = uriValue.substring(0, j);
                                    break;
                                }
                            }
                            Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
                        }else if (AppGlobalState.requestinfo.get(i).URI[k].contains("sip:")) {
                            uriValue = AppGlobalState.requestinfo.get(i).URI[k].substring(4);
                            for(int j = 0; j < uriValue.length(); j++)
                            {
                                if(uriValue.charAt(j) == '@')
                                {
                                    uriValue = uriValue.substring(0, j);
                                    break;
                                }
                            }
                            Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
                        }else {
                            uriValue = AppGlobalState.requestinfo.get(i).URI[k];
                            for(int j = 0; j < uriValue.length(); j++)
                            {
                                if(uriValue.charAt(j) == '@')
                                {
                                    uriValue = uriValue.substring(0, j);
                                    break;
                                }
                            }
                            Log.e("PRESENCE_UI", "ELSE uriValue =" + uriValue);

                        }
                        int index = getIndex(contacts, uriValue);
                        Log.d("PRESENCE_UI", "ListenerHandler : CMDStatus : index "+index);
                        //Contact temp = new Contact(contacts.get(index).name, uriValue, 0, "Closed");
                        Contact temp = (Contact) contacts.get(index);
                        if (AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
                        {
                            temp = new Contact(contacts.get(index).name, uriValue, 0, "Closed" , "");
                        }
                        else
                        {
                            temp = new Contact(contacts.get(index).name, uriValue, 0, "Closed" , contacts.get(index).getIndividualContactURI());
                        }
                        contacts.set(index, temp);
                        Utility.rescheduleSubcribeTimer(index, temp);
                    }
                }
            }
        }

        if (pResInfo != null && pRlmiInfo != null) {
        for (PresResInfo c : pResInfo) {
        String uriValue1 = c.getResUri();
            Log.e("PRESENCE_UI", "uriValue1 =" + uriValue1);
            String uriValue;
            if (uriValue1.contains("tel:")) {
                uriValue = uriValue1.substring(4);
                Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
            } else {
                uriValue = uriValue1;
                Log.e("PRESENCE_UI", "ELSE uriValue =" + uriValue);

            }

            if (uriValue == null) {
                Log.e(TAG, "ContactUri is null, dont update on UI. c=" + c);
                continue;
            }

            int i = getIndex(contacts, uriValue);

            if (i < 0) {
                    Log.e(TAG, "Contact=" + uriValue
                            + " does not exist in phone book, dont update.");
                continue;
            }

                Contact temp = (Contact) contacts.get(i);
                temp.setAvailability(0);
                temp.setBasicStatus("Closed");
                temp.setListContactUri(pRlmiInfo.getUri());
                temp.setListName(pRlmiInfo.getListName());
                temp.setListVersion(""+pRlmiInfo.getVersion());
                temp.setListFullState(""+pRlmiInfo.isFullState());
                temp.setResourceUri(c.getResUri());
                temp.setResourceId(c.getInstanceInfo().getResId());
                temp.setResourceState(Integer.toString(c.getInstanceInfo().getResInstanceState()));
                temp.setResourceReason(c.getInstanceInfo().getReason());
                temp.setResourceCid(c.getInstanceInfo().getResId());
                temp.setServiceId(c.getInstanceInfo().getResId());

                temp.setAudio("False");
                temp.setVideo("False");
                if(c.getInstanceInfo().getReason().equals("rejected") || c.getInstanceInfo().getReason().equals("noresource") || c.getInstanceInfo().getReason().equals("giveup"))
                {
                Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : Not VoLTE contact");
                temp.setNote("Not VoLTE contact");
                }
                else if (!c.getInstanceInfo().getReason().equals(""))
                {
                Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : VoLTE contact");
                temp.setNote("VoLTE Contact");
                }
                temp.setIsVolteContact("false");

                temp.setAudioCapabilities("false");
                temp.setVideoCapabilities("false");
                temp.setVersion("false");
                temp.setDescription("false");
                int isChatEnabled = 0;

                if(c.getInstanceInfo() != null && c.getInstanceInfo().getTupleInfo() != null && c.getInstanceInfo().getTupleInfo().length > 0)
                {
                    for (int j = 0; j < c.getInstanceInfo().getTupleInfo().length; j++) {
                        temp.setTimeStamp(c.getInstanceInfo().getTupleInfo()[j].getTimestamp());
                        temp.setContactUri(c.getInstanceInfo().getTupleInfo()[j].getContactUri());
                        Log.e("PPRESENCE_UI",
                            "mContactUri ="    + c.getInstanceInfo().getTupleInfo()[j].getContactUri());
                        String tempFeatureTag = c.getInstanceInfo().getTupleInfo()[j].getFeatureTag();
                        Log.e("PPRESENCE_UI",
                                "tempFeatureTag ="    + tempFeatureTag);
                        if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\""))
                        {
                            Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 1111");
                            temp.setAvailability(1);
                            temp.setBasicStatus("Open");
                            temp.setAudio("True");
                            temp.setVideo("False");
                            temp.setNote("VoLTE Contact");
                            temp.setIsVolteContact("true");
                            temp.setAudioCapabilities("true");
                            temp.setVideoCapabilities("false");
                        }
                        if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\";video"))
                        {
                            Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 2222");
                            temp.setAvailability(1);
                            temp.setBasicStatus("Open");
                            temp.setAudio("True");
                            temp.setVideo("True");
                            temp.setNote("VoLTE Contact");
                            temp.setIsVolteContact("true");
                            temp.setAudioCapabilities("true");
                            temp.setVideoCapabilities("true");
                        }
                        if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.im\""))
                        {
                            isChatEnabled ++;
                            Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : isChatEnabled "+isChatEnabled);

                        }
                        if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gppapplication.ims.iari.rcs.fullsfgroupchat\""))
                        {
                            isChatEnabled ++;
                            Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : isChatEnabled "+isChatEnabled);
                        }
                        if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.ft\""))
                        {
                            temp.setAvailability(1);
                            temp.setBasicStatus("Open");
                            temp.setVersion("true");
                            Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 5555");
                        }
                        if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%urn-7%3gpp-application.ims.iari.rcse.dp\""))
                        {
                            temp.setAvailability(1);
                            temp.setBasicStatus("Open");
                            Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 6666");
                        }

                        if((isChatEnabled == 2) || (isChatEnabled == 1 && AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE))
                        {
                            temp.setAvailability(1);
                            temp.setBasicStatus("Open");
                            temp.setDescription("true");

                        }
                    }
                }
                else
                {
                    temp.setAvailability(0);
                    temp.setBasicStatus("Closed");
                    temp.setAudio("False");
                    temp.setVideo("False");
                    temp.setNote("Not VoLTE contact");
                    temp.setIsVolteContact("false");
                    temp.setAudioCapabilities("false");
                    temp.setVideoCapabilities("false");
                    temp.setVersion("false");
                    temp.setDescription("false");
                }

            Utility.rescheduleSubcribeTimer(i, temp);
        }
        }

        if(pPresentityURI != null /*pTupleInfo != null && pTupleInfo.length > 0*/)
        {


            //String uriValue1 = pTupleInfo[0].getContactURI();
            String uriValue1 = pPresentityURI;
            Log.e("PRESENCE_UI", "uriValue1 =" + uriValue1);
            String uriValue;
            if (uriValue1.contains("tel:")) {
                uriValue = uriValue1.substring(4);
                for(int j = 0; j < uriValue.length(); j++)
                {
                    if(uriValue.charAt(j) == '@')
                    {
                        uriValue = uriValue.substring(0, j);
                        break;
                    }
                }
                Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
            }else if (uriValue1.contains("sip:")) {
                uriValue = uriValue1.substring(4);
                for(int j = 0; j < uriValue.length(); j++)
                {
                    if(uriValue.charAt(j) == '@')
                    {
                        uriValue = uriValue.substring(0, j);
                        break;
                    }
                }
                Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
            }else {
                uriValue = uriValue1;
                for(int j = 0; j < uriValue.length(); j++)
                {
                    if(uriValue.charAt(j) == '@')
                    {
                        uriValue = uriValue.substring(0, j);
                        break;
                    }
                }
                Log.e("PRESENCE_UI", "ELSE uriValue =" + uriValue);

            }

            if (uriValue == null) {
            Log.e(TAG, "ContactUri is null, dont update on UI. c= null");
            return;
            }


            int i = getIndex(contacts, uriValue);

            if (i < 0) {
                Log.e(TAG, "Contact=" + uriValue
                        + " does not exist in phone book, dont update.");
            return;
            }

            Contact temp = (Contact) contacts.get(i);

            temp.setAvailability(0);
            temp.setBasicStatus("Closed");
            temp.setAudio("False");
            temp.setVideo("False");
            temp.setNote("Not VoLTE contact");
            temp.setIsVolteContact("false");
            temp.setAudioCapabilities("false");
            temp.setVideoCapabilities("false");
            temp.setVersion("false");
            temp.setDescription("false");
            temp.setContactUri(pPresentityURI);
            int isChatEnabled = 0;

            if(pTupleInfo != null && pTupleInfo.length > 0)
            {
                temp.setTimeStamp(pTupleInfo[0].getTimestamp());
                Log.e("PPRESENCE_UI", "mContactUri ="    + pTupleInfo[0].getContactUri());
                for (PresTupleInfo c : pTupleInfo)
                {
                    String tempFeatureTag = c.getFeatureTag();
                    Log.e("PPRESENCE_UI", "tempFeatureTag =" + tempFeatureTag);
                    if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\""))
                    {
                        Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 1111");
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setAudio("True");
                        //temp.setVideo("False");
                        temp.setNote("VoLTE Contact");
                        temp.setIsVolteContact("true");
                        temp.setAudioCapabilities("true");
                        //temp.setVideoCapabilities("false");
                    }
                    if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\";video"))
                    {
                        Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 2222");
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setAudio("True");
                        temp.setVideo("True");
                        temp.setNote("VoLTE Contact");
                        temp.setIsVolteContact("true");
                        temp.setAudioCapabilities("true");
                        temp.setVideoCapabilities("true");
                    }
                    if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.im\""))
                    {
                        isChatEnabled ++;
                        Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : isChatEnabled "+isChatEnabled);
                    }
                    if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gppapplication.ims.iari.rcs.fullsfgroupchat\""))
                    {
                        isChatEnabled ++;
                        Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : isChatEnabled "+isChatEnabled);
                    }
                    if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.ft\""))
                    {
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setVersion("true");
                        Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 5555");
                    }
                    if(tempFeatureTag != null && tempFeatureTag.equals("+g.3gpp.iari-ref=\"urn%urn-7%3gpp-application.ims.iari.rcse.dp\""))
                    {
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 6666");
                    }

                    if((isChatEnabled == 2) || (isChatEnabled == 1 && AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE))
                    {
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setDescription("true");
                    }

                    Utility.rescheduleSubcribeTimer(i, temp);
                }
            }
            else
            {
                Log.d("PRESENCE_UI", "ListenerHandler : updateStatus : 7777");
                temp.setAvailability(0);
                temp.setBasicStatus("Closed");
                temp.setAudio("False");
                temp.setVideo("False");
                temp.setNote("Not VoLTE contact");
                temp.setIsVolteContact("false");
                temp.setAudioCapabilities("false");
                temp.setVideoCapabilities("false");
                temp.setVersion("false");
                temp.setDescription("false");
            }
        }
    }

    private int getIndex(ArrayList<Contact> contacts, String uriValue) {

        String phone = getPhoneFromUri(uriValue);

        Log.d(TAG, "getIndex() phone from uri=" + phone);
        int i = 0;
        for (Contact c : contacts) {
            if (c.getPhone().equals(phone)) {
                Log.d(TAG, "Found phone=" + phone + " at index =" + i);
                return i;
            }
            i++;
        }
        return -1;
    }

    private String getPhoneFromUri(String uriValue) {
        int startIndex = uriValue.indexOf(":", 0);
        int endIndex = uriValue.indexOf("@", startIndex);

        if (startIndex == -1 || endIndex == -1) {
            return uriValue;
        } else {
            return uriValue.substring(startIndex + 1, endIndex);
        }
    }

    Handler uiThreadHandler = new Handler(Looper.getMainLooper()) {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            Log.d(TAG, "uiThreadHandler:Thread=" + Thread.currentThread().getName() + " received " + msg);

            ContactArrayAdapter<Contact> adapter = AppGlobalState.getMainListAdapter();

            switch (msg.what) {
                case 0:
                {
                    Log.d(TAG, "NOTIFY UPDATE with No data Received");
                    Toast.makeText(mContext, "NOTIFY UPDATE with No data Received",
                            Toast.LENGTH_SHORT).show();
                    removeProgressBar();
                    break;
                }

                case PRESENCE_IMS_UNSOL_PUBLISH_TRIGGER:
                {
                    PresPublishTriggerType val = (PresPublishTriggerType) msg.obj;
                    Log.d(TAG, "val.getPublishTrigeerType():"+val.getPublishTrigeerType());

                    switch (val.getPublishTrigeerType())
                    {
                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_ETAG_EXPIRED:
                        {
                            Log.d(TAG, "IMSP_ETAG_EXPIRED");
                            Toast.makeText(mContext,
                                    "Publish trigger for IMSP_ETAG_EXPIRED",
                                    Toast.LENGTH_SHORT).show();
                            break;

                        }

                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN: {

                            Log.d(TAG,
                                    "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN");

                        break;

                        }

                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN: {

                            Log.d(TAG, "uiThreadHandler : QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN:OperatorMode:"
                                 +AppGlobalState.getOperatorMode());
                            Toast.makeText(mContext,
                                    "Publish trigger for IMSP_RAT_CHANGE_IWLAN",
                                    Toast.LENGTH_SHORT).show();

                            ContactInfo.networkTypeIWLAN = true;

                            ContactInfo.networkTypeLTE = false;
                            ContactInfo.vopsEnabled = false;
                            ContactInfo.networkTypeEHRPD = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");

                            triggerUpdateOnDisplayedContactInfo();

                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            Log.d(TAG, "ContactInfo.networkTypeIWLAN:"+ContactInfo.networkTypeIWLAN);


                        break;
                        }

                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED:
                        {
                            Log.d(TAG, "IMSP_RAT_CHANGE_LTE");
                            Toast.makeText(mContext,
                                    "Publish trigger for IMSP_RAT_CHANGE_LTE_VOPS_ENABLED",
                                    Toast.LENGTH_SHORT).show();
                            ContactInfo.networkTypeLTE = true;
                            ContactInfo.vopsEnabled = true;
                            ContactInfo.networkTypeEHRPD = false;
                            ContactInfo.networkTypeIWLAN = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");
                            triggerUpdateOnDisplayedContactInfo();
                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            break;

                        }
                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED:
                        {
                            Toast.makeText(mContext,
                                    "Publish trigger for IMSP_RAT_CHANGE_LTE_VOPS_DISABLED",
                                    Toast.LENGTH_SHORT).show();
                            ContactInfo.networkTypeLTE = true;
                            ContactInfo.vopsEnabled = false;
                            ContactInfo.networkTypeEHRPD = false;
                            ContactInfo.networkTypeIWLAN = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");
                            triggerUpdateOnDisplayedContactInfo();
                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            break;
                        }
                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_EHRPD:
                        {
                            Log.d(TAG, "IMSP_RAT_CHANGE_EHRPD");
                            Toast.makeText(mContext,
                                    "Publish trigger for IMSP_RAT_CHANGE_EHRPD",
                                    Toast.LENGTH_SHORT).show();
                            ContactInfo.networkTypeLTE = false;
                            ContactInfo.vopsEnabled = false;
                            ContactInfo.networkTypeEHRPD = true;
                            ContactInfo.networkTypeIWLAN = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");
                            triggerUpdateOnDisplayedContactInfo();
                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            break;

                        }
                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS:
                        {
                            Log.d(TAG, "QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS");
                            Toast.makeText(mContext,
                                    "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS",
                                    Toast.LENGTH_SHORT).show();
                            ContactInfo.networkTypeLTE = false;
                            ContactInfo.vopsEnabled = false;
                            ContactInfo.networkTypeEHRPD = true;
                            ContactInfo.networkTypeIWLAN = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");
                            triggerUpdateOnDisplayedContactInfo();
                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            break;
                        }
                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_2G:
                        {
                            Log.d(TAG, "QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_2G");
                            Toast.makeText(mContext,
                                    "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_2G",
                                    Toast.LENGTH_SHORT).show();
                            ContactInfo.networkTypeLTE = false;
                            ContactInfo.vopsEnabled = false;
                            ContactInfo.networkTypeEHRPD = true;
                            ContactInfo.networkTypeIWLAN = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");
                            triggerUpdateOnDisplayedContactInfo();
                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            break;
                        }
                        case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_3G:
                        {
                            Log.d(TAG, "QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_3G");
                            Toast.makeText(mContext,
                                    "Publish trigger for QRCS_PRES_PUBLISH_TRIGGER_MOVE_TO_3G",
                                    Toast.LENGTH_SHORT).show();
                            ContactInfo.networkTypeLTE = false;
                            ContactInfo.vopsEnabled = false;
                            ContactInfo.networkTypeEHRPD = true;
                            ContactInfo.networkTypeIWLAN = false;

                            Log.d(TAG, "Before triggerUpdateOnDisplayedContactInfo");
                            triggerUpdateOnDisplayedContactInfo();
                            Log.d(TAG, "After triggerUpdateOnDisplayedContactInfo");
                            break;
                        }
                        default:
                        {
                            Toast.makeText(mContext, "Publish trigger for unknow val=" +
                                    val, Toast.LENGTH_SHORT).show();
                            Log.d(TAG, "NOTIFY UPDATE with No data Received");
                        }
                    }
                    break;
                }

                case PRESENCE_IMS_UNSOL_NOTIFY_UPDATE:
                {
                    PresResInfo [] parsedContactList = (PresResInfo[]) msg.obj;
                    if(parsedContactList != null && parsedContactList.length > 0)
                    {
                        int num = parsedContactList.length;
                            Log.d(TAG,"  aks2    num   =  "+num);
                        if (num > 1) {
                            Toast.makeText(mContext, "NOTIFY UPDATE Received for  " +
                                    num + " Contacts.", Toast.LENGTH_SHORT).show();
                        } else {
                        if(parsedContactList[0].getResUri() != null)
                        {
                            String phone = parsedContactList[0].getResUri();
                                Toast.makeText(mContext, "NOTIFY UPDATE Received for " +
                                        phone, Toast.LENGTH_SHORT).show();
                        }
                        else
                        {
                            Toast.makeText(mContext, "NOTIFY UPDATE Received for empty number"
                                                , Toast.LENGTH_SHORT).show();
                        }
                    }

                    adapter.notifyDataSetChanged();
                    removeProgressBar();
                    triggerUpdateOnDisplayedContactInfo();
                    }

                    break;
                }
                case PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_AVAILABILITY:
                {

                    String pPresentityURI = (String) msg.obj;
                    if(pPresentityURI != null)
                    {
                        String phone = pPresentityURI;
                        Toast.makeText(mContext, "NOTIFY UPDATE Received for " +
                                phone, Toast.LENGTH_SHORT).show();

                        adapter.notifyDataSetChanged();
                        removeProgressBar();
                        triggerUpdateOnDisplayedContactInfo();
                    }
                    else
                    {

                        Toast.makeText(mContext, "NOTIFY UPDATE Received for " +
                                "empty number", Toast.LENGTH_SHORT).show();
                    }
                    /*PresTupleInfo [] parsedContactList = (PresTupleInfo[]) msg.obj;
                    if(parsedContactList != null && parsedContactList.length > 0)
                    {

                        String phone = parsedContactList[0].getContactURI();
                        Toast.makeText(mContext, "NOTIFY UPDATE Received for " +
                                phone, Toast.LENGTH_SHORT).show();

                        adapter.notifyDataSetChanged();
                        removeProgressBar();
                        triggerUpdateOnDisplayedContactInfo();
                    }*/
                    break;
                }

                case PRESENCE_IMS_UNSOL_ENABLER_STATE:
                {
                    String enabler_state = (String) msg.obj;
                    Toast.makeText(mContext, mContext.getResources().getString(
                            R.string.ims_enabler_toast_text) + enabler_state,
                            Toast.LENGTH_SHORT).show();
                    break;
                }
                case PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_CMDSTATUS:
                {
                    String enabler_state = (String) msg.obj;
                    Toast.makeText(mContext, "" +
                            enabler_state,
                            Toast.LENGTH_SHORT).show();
                    break;
                }
                case PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE:
                {
                    String enabler_state = (String) msg.obj;
                    Toast.makeText(mContext, "" +
                            enabler_state,
                            Toast.LENGTH_SHORT).show();
                    break;
                }
                case PRESENCE_IMS_UNPUBLISH_MSG_SENT:
                {
                    String enabler_state = (String) msg.obj;
                    Toast.makeText(mContext, "" +
                            enabler_state,
                            Toast.LENGTH_SHORT).show();
                    break;
                }
                case PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_SIPRESPONSE404:
                {
                    Log.d(TAG, "ListenerHandler : 404 11111");
                    String sip_message_404 = (String) msg.obj;
                    Toast.makeText(mContext, "" +
                            sip_message_404,
                            Toast.LENGTH_SHORT).show();
                    adapter.notifyDataSetChanged();
                    Log.d(TAG, "ListenerHandler : 404 444444");
                    removeProgressBar();
                    Log.d(TAG, "ListenerHandler : 404 5555555");
                    triggerUpdateOnDisplayedContactInfo();
                    Log.d(TAG, "ListenerHandler : 404 222222");
                    break;
                }
                case PRESENCE_IMS_CD_CMD_STATUS:
                    {
                        String strCmdStatus = (String) msg.obj;
                        Toast.makeText(mContext, "CD CMDStatus: ["+ strCmdStatus + "]", Toast.LENGTH_SHORT).show();
                    }
                    break;
                case PRESENCE_IMS_CD_INCOMING_OPTIONS:
                    {
                        int iTid = (int) msg.obj;
                        Toast.makeText(mContext, "CD Incoming Options: TID["+ iTid + "]", Toast.LENGTH_SHORT).show();
                    }
                    break;
            case PRESENCE_IMS_CD_RESP_INCOMING_OPTIONS:
                    {
                        int iTid = (int)msg.obj;
                        Toast.makeText(mContext, "CD Incoming Options Resp: TID["+ iTid + "]", Toast.LENGTH_SHORT).show();
                    }
                break;
                default:
                    Toast.makeText(mContext, "Unknown mesg " + msg.what + " recieved.",
                            Toast.LENGTH_SHORT).show();

            }
        }

        private void triggerUpdateOnDisplayedContactInfo() {
            ContactInfo contactInfo = AppGlobalState.getContactInfo();
            Log.d(TAG, "ListenerHandler : triggerUpdateOnDisplayedContactInfo");
            if (contactInfo != null) {
                Log.d(TAG, "ListenerHandler : triggerUpdateOnDisplayedContactInfo contactInfo != null");
                Log.d(TAG, "ListenerHandler : triggerUpdateOnDisplayedContactInfo contactInfo.getIndexOfDisplayedContact() "+contactInfo.getIndexOfDisplayedContact());
                contactInfo.populateForm(contactInfo.getIndexOfDisplayedContact());
            }

        }

        private void removeProgressBar() {
            if (AppGlobalState.getProgressDialog() != null) {
                // only in case of subscribe polling request
                AppGlobalState.getProgressDialog().dismiss();
            }
        }
    };



    public IOptionsListener cdServiceListener = new IOptionsListener.Stub() {

        public boolean onTransact(int code, Parcel data, Parcel reply, int flags) throws RemoteException
        {
            try
            {
                    return super.onTransact(code, data, reply, flags);
            }
            catch (RuntimeException e)
            {
                Log.w("ListenerHandler", "Unexpected remote exception", e);
                throw e;
            }
        }

        @Override
        public void getVersionCb( String pVersion)
                throws RemoteException {
            Log.d(TAG, "QCDServiceListener_ServiceUnavailable");
        }

        @Override
        public void serviceUnavailable( StatusCode statusCode)
                throws RemoteException {
            Log.d(TAG, "QCDServiceListener_ServiceUnavailable");
        }

        @Override
        public void serviceAvailable( StatusCode statusCode)
                throws RemoteException {
            Log.d(TAG, "QCDServiceListener_ServiceAvailable");
        }

        /* @Override
        public void serviceCreated(
                long cdServiceListenerUserData, StatusCode statusCode,
                int cdServiceHandle) throws RemoteException {
            Log.d(TAG, "QCDServiceListener_ServiceCreated cdServiceHandle "+cdServiceHandle);
            AppGlobalState.setCdServiceHandle(cdServiceHandle);
            AppGlobalState.setCdService(AppGlobalState.getIqRcsService().getOptionsService());
            AppGlobalState.getCdService().getMyInfo(AppGlobalState.getCdServiceHandle(), AppGlobalState.getpUserData());
        }*/

        @Override
        public void sipResponseReceived(String URI,
                                      OptionsSipResponse cdSipResponse, OptionsCapInfo CDCapInfo)
                throws RemoteException {

            Log.d(TAG, "QCDServiceListener_CDUpdate");

            if (URI != null) {
                ArrayList<Contact> contacts = AppGlobalState.getContacts();

                String uriValue1 = URI;
                Log.e("PRESENCE_UI", "uriValue1 =" + uriValue1);
                String uriValue;
                if (uriValue1.contains("tel:")) {
                    uriValue = uriValue1.substring(4);
                    for (int j = 0; j < uriValue.length(); j++) {
                        if (uriValue.charAt(j) == '@') {
                            uriValue = uriValue.substring(0, j);
                            break;
                        }
                    }
                    Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
                } else if (uriValue1.contains("sip:")) {
                    uriValue = uriValue1.substring(4);
                    for (int j = 0; j < uriValue.length(); j++) {
                        if (uriValue.charAt(j) == '@') {
                            uriValue = uriValue.substring(0, j);
                            break;
                        }
                    }
                    Log.e("PRESENCE_UI", " IF uriValue =" + uriValue);
                } else {
                    uriValue = uriValue1;
                    for (int j = 0; j < uriValue.length(); j++) {
                        if (uriValue.charAt(j) == '@') {
                            uriValue = uriValue.substring(0, j);
                            break;
                        }
                    }
                    Log.e("PRESENCE_UI", "ELSE uriValue =" + uriValue);

                }

                if (uriValue == null) {
                    Log.e(TAG, "ContactUri is null, dont update on UI. c= null");
                    return;
                }

                int i = getIndex(contacts, uriValue);

                if (i < 0) {
                    Log.e(TAG, "Contact=" + uriValue
                            + " does not exist in phone book, dont update.");
                    return;
                }

                Contact temp = (Contact) contacts.get(i);

                temp.setAvailability(0);
                temp.setBasicStatus("Closed");
                temp.setAudio("False");
                temp.setVideo("False");
                temp.setNote("Not VoLTE contact");
                temp.setIsVolteContact("false");
                temp.setAudioCapabilities("false");
                temp.setVideoCapabilities("false");
                temp.setVersion("false");
                temp.setDescription("false");
                temp.setContactUri(URI);
                int isChatEnabled = 0;

                if(cdSipResponse != null) {
                    Log.d(TAG, "IQCDListener_CDUpdate : CMD-ID = " + cdSipResponse.getCmdId());
                    Log.d(TAG, "IQCDListener_CDUpdate : RequestID = " + cdSipResponse.getRequestId());
                    Log.d(TAG, "IQCDListener_CDUpdate : Response Code = " + cdSipResponse.getSipResponseCode());
                    if(cdSipResponse.getReasonPhrase() != null) {
                        Log.d(TAG, "IQCDListener_CDUpdate : Reason Phrase = " + cdSipResponse.getReasonPhrase());
                    }
                    Log.d(TAG, "IQCDListener_CDUpdate : retryAfter = " + cdSipResponse.getSipResponseCode());
                }
                if (CDCapInfo != null) {
                    Log.d(TAG, "IQCDListener_CDUpdate : SDP string = " + CDCapInfo.getSdp());
                    Log.d(TAG, "IQCDListener_CDUpdate : IM_CAPABILITY = " + CDCapInfo.getCapInfo().isImSupported());
                    Log.d(TAG, "IQCDListener_CDUpdate : FT_CAPABILITY = " + CDCapInfo.getCapInfo().isFtSupported());
                    Log.d(TAG, "IQCDListener_CDUpdate : IS_CAPABILITY = " + CDCapInfo.getCapInfo().isIsSupported());
                    Log.d(TAG, "IQCDListener_CDUpdate : VS_CAPABILITY = " + CDCapInfo.getCapInfo().isVsSupported());
                    Log.d(TAG, "IQCDListener_CDUpdate : isCDViaPresenceSupported = "
                            + CDCapInfo.getCapInfo().isCdViaPresenceSupported());
                    Log.d(TAG, "IQCDListener_CDUpdate : isGeoPullSupported= "+CDCapInfo.getCapInfo().isGeoPullSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isGeoPushSupported = "
                            + CDCapInfo.getCapInfo().isGeoPushSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isIPVideoSupported = "
                            + CDCapInfo.getCapInfo().isIpVideoSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isIPVoiceSupported = "
                            + CDCapInfo.getCapInfo().isIpVoiceSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isSMSupported = "
                            + CDCapInfo.getCapInfo().isSmSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isSpSupported = "
                            + CDCapInfo.getCapInfo().isSpSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isVsDuringCSSupported = "
                                    + CDCapInfo.getCapInfo().isVsDuringCSSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : isbFullSnFGroupChatSupported = "
                                    + CDCapInfo.getCapInfo().isFullSnFGroupChatSupported());
                    Log.d(TAG, " IQCDListener_CDUpdate : getCapTimestamp = "
                                    + CDCapInfo.getCapInfo().getCapTimestamp());

                    if (CDCapInfo.getCapInfo().isIpVoiceSupported()) {
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : updateStatus : 1111");
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setAudio("True");
                        temp.setVideo("False");
                        temp.setNote("VoLTE Contact");
                        temp.setIsVolteContact("true");
                        temp.setAudioCapabilities("true");
                        temp.setVideoCapabilities("false");
                    }
                    if (CDCapInfo.getCapInfo().isIpVideoSupported()) {
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : updateStatus : 2222");
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setAudio("True");
                        temp.setVideo("True");
                        temp.setNote("VoLTE Contact");
                        temp.setIsVolteContact("true");
                        temp.setAudioCapabilities("true");
                        temp.setVideoCapabilities("true");
                    }
                    if (((CDCapInfo.getCapInfo())).isImSupported()) {
                        isChatEnabled++;
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : updateStatus : isChatEnabled "
                                        + isChatEnabled);
                    }
                    if (CDCapInfo.getCapInfo().isFullSnFGroupChatSupported()) {
                        isChatEnabled++;
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : updateStatus : isChatEnabled "
                                        + isChatEnabled);
                    }
                    if (CDCapInfo.getCapInfo().isFtSupported()) {
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setVersion("true");
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : updateStatus : 5555");
                    }
                    /*if (tempFeatureTag != null
                            && tempFeatureTag
                                    .equals("+g.3gpp.iari-ref=\"urn%urn-7%3gpp-application.ims.iari.rcse.dp\"")) {
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        Log.d("PRESENCE_UI",
                                "ListenerHandler : updateStatus : 6666");
                    }*/

                    if ((isChatEnabled == 2)
                            || (isChatEnabled == 1 && AppGlobalState
                                    .getOperatorMode() == AppGlobalState.ATT_MODE)) {
                        temp.setAvailability(1);
                        temp.setBasicStatus("Open");
                        temp.setDescription("true");
                    }

                    CapInfo localCdInfo = CDCapInfo.getCapInfo();
                    String customStrings[] = localCdInfo.getExts();
                    Log.d("PRESENCE_UI", "ListenerHandler : update CD Found CustomTags");
                    if(customStrings != null)
                    {
                        for(int x=0; x < customStrings.length ; x++)
                        {
                            Log.d("PRESENCE_UI", "ListenerHandler : update CD Found CustomTags [" + x + "]-["+ customStrings[x] +"]");
                        }
                    }

                } else {
                    Log.d("PRESENCE_UI", "ListenerHandler : update CD Status : 7777");
                    temp.setAvailability(0);
                    temp.setBasicStatus("Closed");
                    temp.setAudio("False");
                    temp.setVideo("False");
                    temp.setNote("Not VoLTE contact");
                    temp.setIsVolteContact("false");
                    temp.setAudioCapabilities("false");
                    temp.setVideoCapabilities("false");
                    temp.setVersion("false");
                    temp.setDescription("false");
                }
                Message updateUiMesg = uiThreadHandler
                        .obtainMessage(
                    PRESENCE_IMS_UNSOL_NOTIFY_UPDATE_AVAILABILITY,
                    URI);
                uiThreadHandler.sendMessage(updateUiMesg);
                Utility.rescheduleSubcribeTimer(i, temp);
            }

        }

        @Override
        public void incomingOptions(String sUri, OptionsCapInfo CDCapInfo,
                                            int tID)throws RemoteException {
            Log.d(TAG, "IQCDServiceListener_IncomingOptions");
            Log.d(TAG, "IQCDServiceListener_IncomingOptions: URI[" + sUri + "]");
            Log.d(TAG, "IQCDServiceListener_IncomingOptions: TransactionId[" + tID + "]");
            if(CDCapInfo != null)
            {
                Log.d(TAG, "IQCDListener_CDUpdate : SDP string = " + CDCapInfo.getSdp());
                CapInfo tempCdInfo = CDCapInfo.getCapInfo();

                if (tempCdInfo != null) {
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : IM_CAPABILITY = " + tempCdInfo.isImSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : FT_CAPABILITY = " + tempCdInfo.isFtSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : IS_CAPABILITY = " + tempCdInfo.isIsSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : VS_CAPABILITY = " + tempCdInfo.isVsSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isCDViaPresenceSupported = " + tempCdInfo.isCdViaPresenceSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isGeoPullSupported= "+tempCdInfo.isGeoPullSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isGeoPushSupported = "+ tempCdInfo.isGeoPushSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isIPVideoSupported = " + tempCdInfo.isIpVideoSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isIPVoiceSupported = " + tempCdInfo.isIpVoiceSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isSMSupported = " + tempCdInfo.isSmSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isSpSupported = " + tempCdInfo.isSpSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isVsDuringCSSupported = " + tempCdInfo.isVsDuringCSSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : isbFullSnFGroupChatSupported = " + tempCdInfo.isFullSnFGroupChatSupported());
                    Log.d(TAG, "IQCDServiceListener_IncomingOptions : getCapTimestamp = " + tempCdInfo.getCapTimestamp());
                }
            }
            Message updateUiMesg = uiThreadHandler.obtainMessage(
                    PRESENCE_IMS_CD_INCOMING_OPTIONS, tID);
            uiThreadHandler.sendMessage(updateUiMesg);

            final OptionsCapInfo TempCapinfo = CDCapInfo;
            final int tempID = tID;

            Process procCd = null;
            String sModeCd = "";

            try {
                procCd = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.rcs.cd.SDP"
                });
                BufferedReader readerCd = new BufferedReader(new InputStreamReader(procCd.getInputStream()));
                sModeCd = readerCd.readLine();
            } catch (Exception e) {
                e.printStackTrace();
            }

            if(sModeCd != null && sModeCd.toString().equals("")) {
               sModeCd = "0";
            }
            Log.d(TAG, "After my sModeCd for SDP is: " + sModeCd);

            if (sModeCd != null && Integer.parseInt(sModeCd) == 1)
            {
               String pSdp = "v=0\r\no=Rms-10-206-76-10-8689610 1020610764279238656 869707400 IN IP6 FD00:976A:C206:58::1\r\ns=QC VOIP\r\nc=IN IP6 2002:c023:9c17:20cb:9a41:a0af:4fd9:ff1b\r\nt=0 0\r\nm=message 35294 TCP/MSRP *\r\na=accept-types:message/cpim application/im-iscomposing+xml\r\na=accept-wrapped-types:text/plain message/imdn+xml\r\na=sendrecv\r\na=chatroom:org.3gpp.urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.closedgroupchat\r\na=max-size:65535\r\na=path:msrp://10.63.92.0:35294/ice-session-id-6;tcp\r\na=setup:actpass\r\n";
               TempCapinfo.setSdp(pSdp);
            }
            if (sModeCd != null && Integer.parseInt(sModeCd) == 2)
            {
               String pSdp = "v=0\r\no=- 1001 1000 IN IP6 2002:c023:9c17:20cb:9a41:a0af:4fd9:ff1b\r\ns=QC VOIP\r\nc=IN IP6 2002:c023:9c17:20cb:9a41:a0af:4fd9:ff1b\r\nt=0 0\r\nm=audio 54874 RTP/AVP 96 97 0 8\r\na=rtpmap:96 PCMU-WB/16000\r\na=rtpmap:97 PCMA-WB/16000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\nm=video 60010 RTP/AVP 114 113\r\na=rtpmap:114 H263-1998/90000\r\na=rtpmap:113 H263-2000/90000\r\n";
               TempCapinfo.setSdp(pSdp);
            }
            if (sModeCd != null && Integer.parseInt(sModeCd) == 0)
            {
               String pSdp = "";
               TempCapinfo.setSdp(pSdp);
            }
            TempCapinfo.setCapInfo(AppGlobalState.GetCdInfo());


            try {
                procCd = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.rcs.cd.BL"
                });
                BufferedReader readerCd = new BufferedReader(new InputStreamReader(procCd.getInputStream()));
                sModeCd = readerCd.readLine();
            } catch (Exception e) {
                e.printStackTrace();
            }
            Log.d(TAG, "After my sModeCd for BL is: " + sModeCd);
            boolean btempBL = false;

            if(sModeCd != null && sModeCd.toString().equals("")) {
                sModeCd = "0";
             }

            if (sModeCd != null && Integer.parseInt(sModeCd) == 1) {
                btempBL = true;
            }
            final boolean bfinalBL = btempBL;

            Thread thread = new Thread() {

                    public void run() {
                        IOptionsService cdService = AppGlobalState.getCdService();
                        Log.d(TAG, "IQCDServiceListener_IncomingOptions : getCapTimestamp = " + TempCapinfo);
                        try
                        {
                            cdService.responseIncomingOptions(AppGlobalState.getCdServiceHandle(), tempID, 200, "OK", TempCapinfo, bfinalBL);
                        }catch (Exception e)
                        {
                            e.printStackTrace();
                        }
                    };
            };
            thread.start();

        }

        @Override
        public void cmdStatus(OptionsCmdStatus CdCMDStatus )throws RemoteException {
            Log.d(TAG, "IQCDServiceListener_CMDStatus");
            Log.d(TAG, "IQCDServiceListener_CMDStatus: CMD_ID[" + CdCMDStatus.getCmdId().getCmdId() + "]");
            Log.d(TAG, "IQCDServiceListener_CMDStatus: CMD_ID[" + CdCMDStatus.getUserData() + "]");
            Log.d(TAG, "IQCDServiceListener_CMDStatus: CMD_ID[" + CdCMDStatus.getStatus().getStatusCode() + "]");
            Log.d(TAG, "IQCDServiceListener_CMDStatus: CMD_ID[" + CdCMDStatus.getStatus().getStatusCode() + "]");
            CapInfo tempCdInfo = CdCMDStatus.getCapInfo();
            switch(CdCMDStatus.getCmdId().getCmdId())// == QRCS_CD_CMD_GETMYCDINFO)
            {
            case OptionsCmdId.UCE_OPTIONS_CMD_GETMYCDINFO:
                AppGlobalState.SetCdInfo(tempCdInfo);
                if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE) {
                    Log.d("PRESENCE_UI", "MainActivity : updateSettingsStatusATT");
                    updateSettingsStatusATT(tempCdInfo);

                } else if(AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE) {
                    Log.d("PRESENCE_UI", "MainActivity : updateSettingsStatusVZW");
                    updateSettingsStatusVZW(tempCdInfo);

                } else {
                    AppGlobalState.getCdService().setMyInfo(AppGlobalState.getCdServiceHandle(), tempCdInfo, AppGlobalState.getpUserData());
                    AppGlobalState.SetCdInfo(tempCdInfo);
                }
                break;
            default:
                break;

            }

            if (tempCdInfo != null) {
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : IM_CAPABILITY = " + tempCdInfo.isImSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : FT_CAPABILITY = " + tempCdInfo.isFtSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : IS_CAPABILITY = " + tempCdInfo.isIsSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : VS_CAPABILITY = " + tempCdInfo.isVsSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isCDViaPresenceSupported = " + tempCdInfo.isCdViaPresenceSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isGeoPullSupported= "+tempCdInfo.isGeoPullSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isGeoPushSupported = "+ tempCdInfo.isGeoPushSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isIPVideoSupported = " + tempCdInfo.isIpVideoSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isIPVoiceSupported = " + tempCdInfo.isIpVoiceSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isSMSupported = " + tempCdInfo.isSmSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isSpSupported = " + tempCdInfo.isSpSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isVsDuringCSSupported = " + tempCdInfo.isVsDuringCSSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : isbFullSnFGroupChatSupported = " + tempCdInfo.isFullSnFGroupChatSupported());
                    Log.d(TAG, "IQCDServiceListener_CMDStatus : getCapTimestamp = " + tempCdInfo.getCapTimestamp());
            }
            Message updateUiMesg = uiThreadHandler.obtainMessage(
                    PRESENCE_IMS_CD_CMD_STATUS, Integer.toString(CdCMDStatus.getCmdId().getCmdId()));
            uiThreadHandler.sendMessage(updateUiMesg);

        }
        private void updateSettingsStatusATT(CapInfo myCdInfo) {

     try {
         Log.d("PRESENCE_UI", "MainActivity : updateSettingsStatus : After");
         if(AppGlobalState.getCdService()!= null)
         {
             //CDInfo myCdInfo = new CDInfo();


             myCdInfo.setFtSupported(getFeatureStatus("IPME_KEY"));
             myCdInfo.setImSupported(getFeatureStatus("IPME_KEY"));
             myCdInfo.setSmSupported(getFeatureStatus("IPME_KEY"));

             myCdInfo.setCdViaPresenceSupported(getFeatureStatus("PRESENCE_KEY"));


             if(AppGlobalState.isCustomFTEnabled()) {
                 myCdInfo.setExts(AppGlobalState.GetCustomFT());
             }

             AppGlobalState.getCdService().setMyInfo(AppGlobalState.getCdServiceHandle(), myCdInfo, AppGlobalState.getpUserData());
             AppGlobalState.SetCdInfo(myCdInfo);
         }
         else
         {
             Log.d("PRESENCE_UI", "MainActivity : getCdService : NULL");
         }
      }catch (Exception e) {
        e.printStackTrace();
      }
    }


    private void updateSettingsStatusVZW(CapInfo myCdInfo) {

        try {
            Log.d("PRESENCE_UI", "MainActivity : updateSettingsStatus : After");
            if(AppGlobalState.getCdService()!= null)
            {
                //CDInfo myCdInfo = new CDInfo();
                //AppGlobalState.getCdService().QCDService_GetMyCdInfo(AppGlobalState.getCdServiceHandle(), AppGlobalState.getpUserData());

                myCdInfo.setFtSupported(getFeatureStatus("FT_KEY"));

                myCdInfo.setImSupported(getFeatureStatus("CHAT_KEY"));
                myCdInfo.setFullSnFGroupChatSupported(getFeatureStatus("CHAT_KEY"));


                if(AppGlobalState.isCustomFTEnabled()) {
                    myCdInfo.setExts(AppGlobalState.GetCustomFT());
                }

                AppGlobalState.getCdService().setMyInfo(AppGlobalState.getCdServiceHandle(), myCdInfo, AppGlobalState.getpUserData());
                AppGlobalState.SetCdInfo(myCdInfo);
            }
            else
            {
                Log.d("PRESENCE_UI", "MainActivity : getCdService : NULL");
            }
         }catch (Exception e) {
           e.printStackTrace();
         }
       }




    private boolean getFeatureStatus(String key) {

       SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
       boolean featureSupported = preferences.getBoolean(key, false);
       Log.d("MainActivity", "Key:"+key+":featureSupported:"+featureSupported);
       return featureSupported;
    }

    };

}

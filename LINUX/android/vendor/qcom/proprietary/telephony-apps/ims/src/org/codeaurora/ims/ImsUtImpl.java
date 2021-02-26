/*
 * Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BAIC;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BAICr;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BAOC;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BAOIC;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BAOICxH;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BA_ALL;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BA_MO;
import static com.android.internal.telephony.CommandsInterface.CB_FACILITY_BA_MT;

import com.android.ims.ImsCallForwardInfo;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsSsData;
import com.android.ims.ImsSsInfo;
import com.android.ims.ImsUtInterface;
import com.android.ims.internal.IImsUt;
import com.android.ims.internal.IImsUtListener;
import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.CallForwardInfo;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.DriverCall;
import com.android.internal.telephony.imsphone.ImsPhoneMmiCode;
import com.qualcomm.ims.utils.Log;

import android.Manifest;
import android.app.Service;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.ims.stub.ImsUtImplBase;

import org.codeaurora.ims.ImsCallUtils;
import org.codeaurora.ims.ImsPhoneCommandsInterface;

public class ImsUtImpl extends ImsUtImplBase {

    private static final String LOG_TAG = "ImsUtImpl";
    private static final int MAX_REQUESTS_PENDING = 50; // TODO: Verify and set proper value!

    // Supplementary Service Events
    private static final int EVENT_QUERY_CF    = 1;
    private static final int EVENT_UPDATE_CF   = 2;
    private static final int EVENT_QUERY_CW    = 3;
    private static final int EVENT_UPDATE_CW   = 4;
    private static final int EVENT_QUERY_CLIR  = 5;
    private static final int EVENT_UPDATE_CLIR = 6;
    private static final int EVENT_QUERY_CLIP  = 7;
    private static final int EVENT_UPDATE_CLIP = 8;
    private static final int EVENT_QUERY_COLR  = 9;
    private static final int EVENT_UPDATE_COLR = 10;
    private static final int EVENT_QUERY_COLP  = 11;
    private static final int EVENT_UPDATE_COLP = 12;
    private static final int EVENT_QUERY_CB    = 13;
    private static final int EVENT_UPDATE_CB   = 14;
    private static final int EVENT_UNSOL_ON_SS = 15;

    // Used for various supp. services APIs.
    // See 27.007 +CCFC or +CLCK
    static final int SERVICE_CLASS_NONE       = 0; // no user input
    static final int SERVICE_CLASS_VOICE      = (1 << 0);
    static final int SERVICE_CLASS_DATA       = (1 << 1); //synonym for 16+32+64+128
    static final int SERVICE_CLASS_FAX        = (1 << 2);
    static final int SERVICE_CLASS_SMS        = (1 << 3);
    static final int SERVICE_CLASS_DATA_SYNC  = (1 << 4);
    static final int SERVICE_CLASS_DATA_ASYNC = (1 << 5);
    static final int SERVICE_CLASS_PACKET     = (1 << 6);
    static final int SERVICE_CLASS_PAD        = (1 << 7);
    static final int SERVICE_CLASS_MAX        = (1 << 7); // Max SERVICE_CLASS value

    // Call forwarding 'reason' values.
    static final int CF_REASON_UNCONDITIONAL    = 0;
    static final int CF_REASON_BUSY             = 1;
    static final int CF_REASON_NO_REPLY         = 2;
    static final int CF_REASON_NOT_REACHABLE    = 3;
    static final int CF_REASON_ALL              = 4;
    static final int CF_REASON_ALL_CONDITIONAL  = 5;

    // UT Error strings the lower layers can send as part of response exception.
    static final String UT_ERROR_GENERIC             = "E_GENERIC_FAILURE";
    static final String UT_ERROR_NETWORK_UNSUPPORTED = "E_NETWORK_NOT_SUPPORTED";
    static final String UT_ERROR_FDN_FAILURE = "E_FDN_CHECK_FAILURE";
    static final String UT_ERROR_SS_MODIFIED_TO_DIAL = "E_SS_MODIFIED_TO_DIAL";
    static final String UT_ERROR_SS_MODIFIED_TO_USSD = "E_SS_MODIFIED_TO_USSD";
    static final String UT_ERROR_SS_MODIFIED_TO_SS = "E_SS_MODIFIED_TO_SS";
    static final String UT_ERROR_SS_MODIFIED_TO_DIAL_VIDEO = "E_SS_MODIFIED_TO_DIAL_VIDEO";

    // Permissions used by class.
    private final String MODIFY_PHONE_STATE = Manifest.permission.MODIFY_PHONE_STATE;
    private final String READ_PRIVILEGED_PHONE_STATE = Manifest.permission.READ_PRIVILEGED_PHONE_STATE;
    private final String READ_PHONE_STATE = Manifest.permission.READ_PHONE_STATE;

    private ImsServiceSub mServiceSub;
    private ImsSenderRxr mCi;
    private Context mContext;
    private ImsUtListenerProxy mListenerProxy = new ImsUtListenerProxy();
    private Handler mHandler = new ImsUtImplHandler();
    private static int requestId = -1;

    public ImsUtImpl(ImsServiceSub serviceSub, ImsSenderRxr senderRxr, Context context) {
        mServiceSub = serviceSub;
        mCi = senderRxr;
        mContext = context;
        mCi.registerForSuppServiceIndication(mHandler, EVENT_UNSOL_ON_SS, null);
    }

    private void enforceReadPhoneState(String fn) {
        if (mContext.checkCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE)
                != PackageManager.PERMISSION_GRANTED
                && mContext.checkCallingOrSelfPermission(READ_PHONE_STATE)
                != PackageManager.PERMISSION_GRANTED) {
            mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, fn);
        }
    }

    @VisibleForTesting
    public Handler getHandler() {
        return mHandler;
    }

    /**
     * Closes the object. This object is not usable after being closed.
     */
    public void close() {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "close");
        mCi.unregisterForSuppServiceIndication(mHandler);
        mCi = null;
        mListenerProxy = null;
        mHandler = null;
    }

    /**
     * Retrieves the configuration of the call barring.
     */
    public int queryCallBarring(int cbType) {
        return queryCallBarringForServiceClass(cbType, SERVICE_CLASS_NONE);
    }

    /**
     * Retrieves the configuration of the call barring for specified service class.
     */
    public int queryCallBarringForServiceClass(int cbType, int serviceClass) {
        enforceReadPhoneState("queryCallBarring");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCallBarring.");
            // ImsUt.java treats ids < 0 as an error.
            return -1;
        }

        int facility = getFacilityFromCbType(cbType);
        if (facility == -1) {
            Log.e(this, "Unsupported call barring facility code in queryCallBarring.");
            return -1;
        }

        if (facility == ImsQmiIF.FACILITY_BS_MT) {
            mCi.suppSvcStatus(ImsQmiIF.QUERY,
                    facility,
                    null,
                    SERVICE_CLASS_VOICE,
                    mHandler.obtainMessage(EVENT_QUERY_CB, id, 0, this));
        } else {
            mCi.suppSvcStatus(ImsQmiIF.QUERY,
                    facility, null, serviceClass,
                    mHandler.obtainMessage(EVENT_QUERY_CB, id, 0, this));
        }

        return id;
    }

    private int getFacilityFromCbType(int cbType) {
        // NOTE: Refer to getCBTypeFromFacility in ImsPhone.java. All codes
        //       there are converted to appropriate ImsQmiIF codes.
        if (cbType == ImsUtInterface.CB_BAOC) {
            return ImsQmiIF.FACILITY_BAOC;
        }
        else if (cbType == ImsUtInterface.CB_BOIC) {
            return ImsQmiIF.FACILITY_BAOIC;
        }
        else if (cbType == ImsUtInterface.CB_BOIC_EXHC) {
            return ImsQmiIF.FACILITY_BAOICxH;
        }
        else if (cbType == ImsUtInterface.CB_BAIC) {
            return ImsQmiIF.FACILITY_BAIC;
        }
        else if (cbType == ImsUtInterface.CB_BIC_WR) {
            return ImsQmiIF.FACILITY_BAICr;
        }
        else if (cbType == ImsUtInterface.CB_BA_ALL) {
            return ImsQmiIF.FACILITY_BA_ALL;
        }
        else if (cbType == ImsUtInterface.CB_BA_MO) {
            return ImsQmiIF.FACILITY_BA_MO;
        }
        else if (cbType == ImsUtInterface.CB_BA_MT) {
            return ImsQmiIF.FACILITY_BA_MT;
        }
        else if (cbType == ImsUtInterface.CB_BS_MT) {
            return ImsQmiIF.FACILITY_BS_MT;
        }
        else if (cbType == ImsUtInterface.CB_BIC_ACR) {
            return ImsQmiIF.FACILITY_BAICa;
        }
        else { // Unsupported Call Barring Code
            return -1;
        }
    }

    /**
     * Retrieves the configuration of the call forward.
     */
    public int queryCallForward(int condition, String number) {
        return queryCFForServiceClass(condition, number, SERVICE_CLASS_VOICE);
    }

    /**
     * Retrieves the configuration of the call forward for specified service class.
     */
    public int queryCFForServiceClass(int condition, String number, int serviceClass) {
        enforceReadPhoneState("queryCallForward");
        int reason = -1;

        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCallForward.");
            // ImsUt.java treats ids < 0 as an error.
            return -1;
        }

        if (condition == ImsUtInterface.CDIV_CF_UNCONDITIONAL) {
            reason = CF_REASON_UNCONDITIONAL;
        }
        else if (condition == ImsUtInterface.CDIV_CF_BUSY) {
            reason = CF_REASON_BUSY;
        }
        else if (condition == ImsUtInterface.CDIV_CF_NO_REPLY) {
            reason = CF_REASON_NO_REPLY;
        }
        else if (condition == ImsUtInterface.CDIV_CF_NOT_REACHABLE) {
            reason = CF_REASON_NOT_REACHABLE;
        }
        else if (condition == ImsUtInterface.CDIV_CF_ALL) {
            reason = CF_REASON_ALL;
        }
        else if (condition == ImsUtInterface.CDIV_CF_ALL_CONDITIONAL) {
            reason = CF_REASON_ALL_CONDITIONAL;
        }
        else if (condition == ImsUtInterface.CDIV_CF_NOT_LOGGED_IN) {
            //TODO: NOT SUPPORTED CURRENTLY.
            // It's only supported in the IMS service (CS does not define it).
            // IR.92 recommends that an UE activates both the CFNRc and the CFNL
            // (CDIV using condition not-registered) to the same target.
            reason = -1;
        }
        else {
            Log.e(this, "Invalid condition for queryCallForward.");
            return -1;
        }

        mCi.queryCallForwardStatus(reason,
                                   serviceClass,
                                   number,
                                   mHandler.obtainMessage(EVENT_QUERY_CF, id, 0, this));
        return id;
    }

    /**
     * Retrieves the configuration of the call waiting.
     */
    public int queryCallWaiting() {
        enforceReadPhoneState("queryCallWaiting");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCallWaiting.");
            return -1;
        }
        mCi.queryCallWaiting(SERVICE_CLASS_NONE,
                             mHandler.obtainMessage(EVENT_QUERY_CW, id, 0, this));
        return id;
    }

    /**
     * Retrieves the default CLIR setting.
     */
    public int queryCLIR() {
        enforceReadPhoneState("queryCLIR");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCLIR.");
            return -1;
        }

        mCi.getCLIR(mHandler.obtainMessage(EVENT_QUERY_CLIR, id, 0, this));
        return id;
    }

    /**
     * Retrieves the CLIP call setting.
     */
    public int queryCLIP() {
        enforceReadPhoneState("queryCLIP");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCLIP.");
            return -1;
        }

        mCi.queryCLIP(mHandler.obtainMessage(EVENT_QUERY_CLIP, id, 0, this));
        return id;
    }

    /**
     * Retrieves the COLR call setting.
     */
    public int queryCOLR() {
        enforceReadPhoneState("queryCOLR");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCOLR.");
            return -1;
        }

        mCi.getCOLR(mHandler.obtainMessage(EVENT_QUERY_COLR, id, 0, this));
        return id;
    }

    /**
     * Retrieves the COLP call setting.
     */
    public int queryCOLP() {
        enforceReadPhoneState("queryCOLP");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for queryCLIP.");
            return -1;
        }

        mCi.getSuppSvc("COLP", //TODO: String argument required. Use like this or define somewhere?
                       mHandler.obtainMessage(EVENT_QUERY_COLP, id, 0, this));
        return id;
    }

    /**
     * Updates or retrieves the supplementary service configuration.
     */
    public int transact(Bundle ssInfo) {
        // No permission check since not implemented - otherwise would have to
        // check:
        // But not clear if MODIFY or READ?
        //Not supported. Return -1 for error handling by framework.
        Log.e(this, "Unsupported API transact() called.");
        return -1;
    }

    /**
     * Updates the configuration of the call barring.
     */
    public int updateCallBarring(int cbType, boolean enable,String[] barrList) {
        return 0; //TODO: Remove dummy stub added for compilation
    }

    /*
     * we have reused CF actions because CF and CB actions are used for
     * same purpose.However,We are updating CB actions here as per proto
     * file to be in sync with lower layer.
     */
    public int getIcbAction(int action) {
        if(action == ImsPhoneCommandsInterface.CF_ACTION_DISABLE){
            return ImsQmiIF.DEACTIVATE;
        } else if(action == ImsPhoneCommandsInterface.CF_ACTION_ENABLE) {
            return  ImsQmiIF.ACTIVATE;
        } else if (action == ImsPhoneCommandsInterface.CF_ACTION_ERASURE) {
            return ImsQmiIF.ERASURE;
        } else if(action == ImsPhoneCommandsInterface.CF_ACTION_REGISTRATION) {
            return ImsQmiIF.REGISTER;
        }
        return ImsUtInterface.INVALID;
    }

    /**
     * Updates the configuration of the call barring.
     */
    public int updateCallBarring(int cbType, int action, String[] barrList) {
        return updateCallBarringForServiceClass(cbType, action, SERVICE_CLASS_NONE, barrList);
    }

    /**
     * Updates the configuration of the call barring for specified service class.
     */
    public int updateCallBarringForServiceClass(int cbType, int action, int serviceClass,
            String[] barrList) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCallBarring");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCallBarring.");
            // ImsUt.java treats ids < 0 as an error.
            return -1;
        }

        int facility = getFacilityFromCbType(cbType);
        if (facility == -1) {
            Log.e(this, "Unsupported call barring facility code in updateCallBarring.");
            return -1;
        }

        int cbAction = getIcbAction(action);
        // Check for ICB case.
        if (facility == ImsQmiIF.FACILITY_BS_MT) {
            mCi.suppSvcStatus(cbAction,
                    facility,
                    barrList,
                    SERVICE_CLASS_VOICE,
                    mHandler.obtainMessage(EVENT_UPDATE_CB, id, 0, this));
        } else {
            mCi.suppSvcStatus(cbAction,
                    facility, null, serviceClass,
                    mHandler.obtainMessage(EVENT_UPDATE_CB, id, 0, this));
        }

        return id;
    }

    /**
     * Updates the configuration of the call forward.
     */
    public int updateCallForward(int action, int condition, String number, int serviceClass,
            int timeSeconds) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCallForward");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCallForward.");
            // ImsUt.java treats ids < 0 as an error.
            return -1;
        }
        mCi.setCallForward(action,
                           condition,
                           serviceClass,
                           number,
                           timeSeconds,
                           mHandler.obtainMessage(EVENT_UPDATE_CF, id, 0, this));
        return id;
    }

    /**
     * Updates the configuration of the call forward Unconditional Timer.
     */
    public int updateCallForwardUncondTimer(int startHour, int startMinute, int endHour,
            int endMinute, int action, int condition, String number) {
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCallForwardUncondTimer.");
            // ImsUt.java treats ids < 0 as an error.
            return -1;
        }
        mCi.setCallForwardUncondTimer(startHour, startMinute, endHour, endMinute,
                           action, condition,
                           CommandsInterface.SERVICE_CLASS_VOICE,
                           number,
                           mHandler.obtainMessage(EVENT_UPDATE_CF, id, 0, this));
        return id;
    }

    /**
     * Updates the configuration of the call waiting.
     */
    public int updateCallWaiting(boolean enable, int serviceClass) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCallWaiting");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCallForward.");
            return -1;
        }
        mCi.setCallWaiting(enable,
                           serviceClass,
                           mHandler.obtainMessage(EVENT_UPDATE_CW, id, 0, this));
        return id;
    }

    /**
     * Updates the configuration of the CLIR supplementary service.
     */
    public int updateCLIR(int clirMode) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCLIR");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCLIR.");
            return -1;
        }
        mCi.setCLIR(clirMode,
                    mHandler.obtainMessage(EVENT_UPDATE_CLIR, id, 0, this));
        return id;
    }

    /**
     * Updates the configuration of the CLIP supplementary service.
     */
    public int updateCLIP(boolean enable) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCLIP");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCLIP.");
            return -1;
        }
        mCi.setSuppSvc("CLIP",
                       enable,
                       mHandler.obtainMessage(EVENT_UPDATE_CLIP, id, 0, this));
        return id;
    }

    /**
     * Updates the configuration of the COLR supplementary service.
     */
    public int updateCOLR(int presentation) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCOLR");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCOLR.");
            return -1;
        }
        mCi.setCOLR(presentation,
                    mHandler.obtainMessage(EVENT_UPDATE_COLR, id, 0, this));
        return id;
    }

    /**
     * Updates the configuration of the COLP supplementary service.
     */
    public int updateCOLP(boolean enable) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateCOLP");
        int id = getIdForRequest();
        if (id < 0) {
            Log.e(this, "Invalid request id for updateCOLP.");
            return -1;
        }
        mCi.setSuppSvc("COLP",
                       enable,
                       mHandler.obtainMessage(EVENT_UPDATE_COLP, id, 0, this));
        return id;
    }

    /**
     * Sets the listener.
     */
    public void setListener(IImsUtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setListener");
        mListenerProxy.mListener = listener;
    }

    /**
     * Method to get a request id for a request.
     * @return requestId
     */
    private int getIdForRequest() {
        // Note: This logic is in place to handle multiple UT requests at
        //       the same time. Currently, UI supports only one request.
        requestId++;
        if (requestId >= MAX_REQUESTS_PENDING) {
            requestId = 0;
        }
        return requestId;
    }

    /**
     * Method to parse the exception string within an AsyncResult exception
     * and return the correct IMS reason for failure.
     * @param ar The (AsyncResult) response message from lower layers.
     * @return The ImsReasonInfo with appropriate error code.
     */
    private ImsReasonInfo getImsReasonInfoFromResponseError(AsyncResult ar) {
        if (ar == null) {
            Log.i(this, "getImsReasonInfoFromResponseError :: Null AsyncResult!");
            return null;
        }
        if (ar.exception == null) {
            Log.i(this, "getImsReasonInfoFromResponseError :: "
                    + "No exception in AsyncResult!");
            return null;
        }
        // ImsSenderRxr packs a RuntimeException with the AyncResult.
        // This is the only type we are handling currently.
        if (!(ar.exception instanceof RuntimeException)) {
            Log.i(this, "getImsReasonInfoFromResponseError :: "
                    + "Improper exception type in AsyncResult!");
            return null;
        }

        int code = ImsReasonInfo.CODE_UNSPECIFIED;
        // Create a default ImsReasonInfo to be returned if no specific error
        // message has been propagated by the lower layers.
        ImsReasonInfo genericError = new ImsReasonInfo(code, 0);

        RuntimeException ex = (RuntimeException)ar.exception;
        String error = ex.getMessage();
        if (error == null) {
            Log.i(this, "getImsReasonInfoFromResponseError :: "
                    + "Null message string in exception!");
            return genericError;
        }

        // Check the error string propagated with the exception.
        // Map it to the appropriate ImsReasonInfo code.
        if (error.compareTo(UT_ERROR_GENERIC) == 0) {
            return genericError;
        } else if (error.compareTo(UT_ERROR_NETWORK_UNSUPPORTED) == 0) {
            code = ImsReasonInfo.CODE_UT_SERVICE_UNAVAILABLE;
        } else if (error.compareTo(UT_ERROR_FDN_FAILURE) == 0) {
            code = ImsReasonInfo.CODE_FDN_BLOCKED;
        } else if (error.compareTo(UT_ERROR_SS_MODIFIED_TO_DIAL) == 0) {
            code = ImsReasonInfo.CODE_UT_SS_MODIFIED_TO_DIAL;
        } else if (error.compareTo(UT_ERROR_SS_MODIFIED_TO_USSD) == 0) {
            code = ImsReasonInfo.CODE_UT_SS_MODIFIED_TO_USSD;
        } else if (error.compareTo(UT_ERROR_SS_MODIFIED_TO_SS) == 0) {
            code = ImsReasonInfo.CODE_UT_SS_MODIFIED_TO_SS;
        } else if (error.compareTo(UT_ERROR_SS_MODIFIED_TO_DIAL_VIDEO) == 0) {
            code = ImsReasonInfo.CODE_UT_SS_MODIFIED_TO_DIAL_VIDEO;
        } else {
            Log.i(this, "getImsReasonInfoFromResponseError :: "
                    + "Unrecognized exception message string!");
            return genericError;
        }

        Log.i(this, "getImsReasonInfoFromResponseError :: "
                + "ImsReasonInfo code=" + code);
        return new ImsReasonInfo(code, 0);
    }

    //Handler for tracking requests sent to ImsSenderRxr.
    private class ImsUtImplHandler extends Handler {
        ImsUtImplHandler() {
            super();
        }

        private void handleCFUTResponse(AsyncResult ar, Message msg) {

            ImsCallForwardTimerInfo cfInfoList[] = (ImsCallForwardTimerInfo[]) ar.result;

            if (cfInfoList.length < 1) {
                Log.e(this, "ImsCallForwardTimerInfo[] has no elements!");
                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                        msg.arg1,
                        new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                return;
            }

            boolean badCfResponse = false;
            ImsCallForwardTimerInfo cfInfo;
            ImsCallForwardInfo callForwardInfo;
            ImsCallForwardInfo[] callForwardInfoList
                = new ImsCallForwardInfo[cfInfoList.length];

            for (int i = 0; i < cfInfoList.length; i++) {
                cfInfo = cfInfoList[i];
                callForwardInfo = new ImsCallForwardInfo();

                if (cfInfo.status == 1) {
                    callForwardInfo.mStatus = 1; // Enabled
                }
                else if (cfInfo.status == 0) {
                    callForwardInfo.mStatus = 0; // Disabled
                }
                else {
                    badCfResponse = true;
                    Log.e(this, "Bad status in Query CFUT response.");
                }

                if (cfInfo.reason == CF_REASON_UNCONDITIONAL) {
                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_UNCONDITIONAL;
                }
                else {
                    badCfResponse = true;
                    Log.e(this, "Bad reason in Query CFUT response.");
                }

                if (badCfResponse) {
                    mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                            msg.arg1,
                            new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                    return;
                }

                callForwardInfo.mNumber = new String(cfInfo.number);
                //commented out to sort out atel component dependency.
                //callForwardInfo.mServiceClass = cfInfo.serviceClass;

                callForwardInfoList[i] = callForwardInfo;
            }

            mListenerProxy.utConfigurationCallForwardQueried((IImsUt)ar.userObj,
                    msg.arg1,
                    callForwardInfoList);
        }

        @Override
        public void handleMessage(Message msg) {
            Log.i(this, "Message received: what = " + msg.what);
            AsyncResult ar;

            switch (msg.what) {

                case EVENT_QUERY_CB:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }

                        if (ar.exception != null) {
                            Log.e(this, "Query CB error");

                            if (ar.userObj != null) {
                                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else if (ar.result != null) {
                            ImsQmiIF.SuppSvcResponse response = (ImsQmiIF.SuppSvcResponse) ar.result;
                            if (response.hasErrorDetails()) {
                                Log.e(this, "SuppSvcResponse has failure for CB query.");
                                ImsQmiIF.SipErrorInfo sipError = response.getErrorDetails();
                                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                       msg.arg1, ImsCallUtils.getImsReasonInfo(sipError,
                                       ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                                return;
                            } else if (response.hasFailureCause()) {
                                ImsReasonInfo error =
                                        new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0);
                                error.mExtraMessage = response.getFailureCause();
                                Log.e(LOG_TAG, "CB query failed with error = " + error);
                                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        error);
                                return;
                            }
                            if (!response.hasStatus()) {
                                Log.e(this, "No service status info in response for CB query.");
                                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                            }
                            else {
                                int count = response.getCbNumListTypeCount();

                                if(count > 0) {
                                    ImsSsInfo[] ssInfoArray = null;
                                    for (int i = 0; i < count; i++) {
                                        ImsQmiIF.CbNumListType cbNumListType =
                                                response.getCbNumListType(i);
                                        int size = cbNumListType.getCbNumListCount();
                                        ssInfoArray = new ImsSsInfo[size];
                                        if (cbNumListType.hasServiceClass()) {
                                            ImsQmiIF.ServiceClass serviceClass =
                                                    cbNumListType.getServiceClass();
                                            int service_class = serviceClass.getServiceClass();
                                            if (size != 0) {
                                                for (int j = 0; j < size; j++) {
                                                    ImsQmiIF.CbNumList cbNumList =
                                                            cbNumListType.getCbNumList(j);
                                                    ImsSsInfo ssInfo = new ImsSsInfo();
                                                    ssInfo.mIcbNum = cbNumList.getNumber();
                                                    ssInfo.mStatus = cbNumList.getStatus();
                                                    ssInfoArray[j] = ssInfo;
                                                }
                                            }
                                        }
                                    }
                                    mListenerProxy.utConfigurationCallBarringQueried(
                                            (IImsUt)ar.userObj, msg.arg1, ssInfoArray);
                                } else {
                                    ImsSsInfo[] ssInfoStatus = new ImsSsInfo[1];
                                    ImsSsInfo ssInfo = new ImsSsInfo();
                                    if (response.getStatus() == 0) {
                                        ssInfo.mStatus = ImsSsInfo.DISABLED;
                                    } else if (response.getStatus() == 1) {
                                        ssInfo.mStatus = ImsSsInfo.ENABLED;
                                    }
                                    ssInfoStatus[0] = ssInfo;
                                    Log.i(this, "success callback Query Anonymous CB, status= "
                                            + ssInfo.mStatus);
                                    mListenerProxy.utConfigurationCallBarringQueried(
                                            (IImsUt)ar.userObj, msg.arg1, ssInfoStatus);
                                }
                            }
                        }
                        else {
                            Log.e(this, "Null response received for Query CB!");
                            mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                    msg.arg1,
                                    new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                        }
                    }
                    break;

                case EVENT_UPDATE_CB:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }

                        if (ar.exception != null) {
                            Log.e(this, "Update CB error");

                            if (ar.userObj != null) {
                                mListenerProxy.utConfigurationUpdateFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else if (ar.result != null) {
                            ImsQmiIF.SuppSvcResponse response = (ImsQmiIF.SuppSvcResponse) ar.result;
                            if (response.hasErrorDetails()) {
                                Log.e(this, "SuppSvcResponse has failure for CB update.");
                                ImsQmiIF.SipErrorInfo sipError = response.getErrorDetails();
                                mListenerProxy.utConfigurationUpdateFailed((IImsUt)ar.userObj,
                                        msg.arg1, ImsCallUtils.getImsReasonInfo(sipError,
                                        ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                                return;
                            } else if (response.hasFailureCause()) {
                                ImsReasonInfo error =
                                        new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0);
                                error.mExtraMessage = response.getFailureCause();
                                Log.e(LOG_TAG, "CB update failed with error = " + error);
                                mListenerProxy.utConfigurationUpdateFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        error);
                                return;
                            }
                            mListenerProxy.utConfigurationUpdated((IImsUt)ar.userObj, msg.arg1);
                        }
                        else {
                            // Null response from RIL is a valid success scenario here.
                            mListenerProxy.utConfigurationUpdated((IImsUt)ar.userObj, msg.arg1);
                        }
                    }
                    break;

                case EVENT_UPDATE_CF:
                case EVENT_UPDATE_CW:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            if (msg.what == EVENT_UPDATE_CF) {
                                Log.e(this, "Update CF error");
                            }
                            else if (msg.what == EVENT_UPDATE_CW) {
                                Log.e(this, "Update CW error");
                            }

                            if (ar.result != null) {
                                // Update CF/CW response has failure cause information.
                                // Check for it to determine request's success of failure.
                                ImsQmiIF.SuppSvcResponse response
                                    = (ImsQmiIF.SuppSvcResponse) ar.result;
                                if (response.hasErrorDetails()) {
                                    Log.d(this, "SuppSvcResponse has failure for msg.what= "
                                            + msg.what);
                                    ImsQmiIF.SipErrorInfo sipError = response.getErrorDetails();
                                    mListenerProxy.utConfigurationUpdateFailed((IImsUt) ar.userObj,
                                            msg.arg1, ImsCallUtils.getImsReasonInfo(sipError,
                                            ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                                } else if (response.hasFailureCause()) {
                                    ImsReasonInfo error =
                                            new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR,
                                            0);
                                    error.mExtraMessage = response.getFailureCause();
                                    Log.e(LOG_TAG, "SuppSvc " + msg.what + " failed with error = "
                                            + error);
                                    mListenerProxy.utConfigurationUpdateFailed((IImsUt)ar.userObj,
                                            msg.arg1,
                                            error);
                                } else {
                                    Log.e(LOG_TAG, "SuppSvcResponse failure with neither"
                                            + " errordetails nor failurecause");
                                    mListenerProxy.utConfigurationUpdateFailed(
                                            (IImsUt)ar.userObj, msg.arg1,
                                            getImsReasonInfoFromResponseError(ar));
                                }
                            }
                            else if (ar.userObj != null) {
                                mListenerProxy.utConfigurationUpdateFailed((IImsUt) ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            } else {
                                // Nothing to pass to frameworks for this request's response.
                                Log.e(LOG_TAG, "SuppSvcResponse failure with neither ar.result"
                                         + " nor userObj");
                                mListenerProxy.utConfigurationUpdateFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else {
                            Log.i(this, "Success callback called for msg.what= "
                                    + msg.what);
                            mListenerProxy.utConfigurationUpdated((IImsUt) ar.userObj, msg.arg1);
                        }
                    }
                    break;

                case EVENT_QUERY_CF:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            Log.e(this, "Query CF error");
                            if (ar.userObj != null) {
                                mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else if (ar.result != null) {
                            if (ar.result instanceof ImsCallForwardTimerInfo[]) {
                                Log.i(this, "Handle CFUT response");
                                handleCFUTResponse(ar, msg);
                                return;
                            }
                            CallForwardInfo cfInfoList[] = (CallForwardInfo[]) ar.result;

                            if (cfInfoList.length < 1) {
                                Log.e(this, "CallForwardInfo[] has no elements!");
                                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                      msg.arg1,
                                      new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                              return;
                            }

                            boolean badCfResponse = false;
                            CallForwardInfo cfInfo;
                            ImsCallForwardInfo callForwardInfo;
                            ImsCallForwardInfo[] callForwardInfoList
                                = new ImsCallForwardInfo[cfInfoList.length];

                            for (int i = 0; i < cfInfoList.length; i++) {
                                cfInfo = cfInfoList[i];
                                callForwardInfo = new ImsCallForwardInfo();

                                if (cfInfo.status == 1) {
                                    callForwardInfo.mStatus = 1; // Enabled
                                }
                                else if (cfInfo.status == 0) {
                                    callForwardInfo.mStatus = 0; // Disabled
                                }
                                else {
                                    badCfResponse = true;
                                    Log.e(this, "Bad status in Query CF response.");
                                }

                                if (cfInfo.reason == CF_REASON_UNCONDITIONAL) {
                                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_UNCONDITIONAL;
                                }
                                else if (cfInfo.reason == CF_REASON_BUSY) {
                                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_BUSY;
                                }
                                else if (cfInfo.reason == CF_REASON_NO_REPLY) {
                                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_NO_REPLY;
                                    // Time present only in this case.
                                    callForwardInfo.mTimeSeconds = cfInfo.timeSeconds;
                                }
                                else if (cfInfo.reason == CF_REASON_NOT_REACHABLE) {
                                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_NOT_REACHABLE;
                                }
                                else if (cfInfo.reason == CF_REASON_ALL) {
                                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_ALL;
                                }
                                else if (cfInfo.reason == CF_REASON_ALL_CONDITIONAL) {
                                    callForwardInfo.mCondition = ImsUtInterface.CDIV_CF_ALL_CONDITIONAL;
                                }
                                else {
                                    badCfResponse = true;
                                    Log.e(this, "Bad reason in Query CF response.");
                                }
                                /* TODO: Remove comment added for bringup
                                if ((cfInfo.startHour < 24) && (cfInfo.startMinute < 60) &&
                                        (cfInfo.endHour < 24) && (cfInfo.endMinute) < 60){
                                    callForwardInfo.mStartHour = cfInfo.startHour;
                                    callForwardInfo.mStartMinute = cfInfo.startMinute;
                                    callForwardInfo.mEndHour = cfInfo.endHour;
                                    callForwardInfo.mEndMinute = cfInfo.endMinute;
                                } else {
                                    badCfResponse = true;
                                    Log.e(this, "Bad Timer Values in Query CF response.");
                                }*/

                                if (badCfResponse) {
                                    mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                            msg.arg1,
                                            new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                                    return;
                                }

                                callForwardInfo.mToA = cfInfo.toa;
                                callForwardInfo.mNumber = new String(cfInfo.number);
                                callForwardInfo.mServiceClass = cfInfo.serviceClass;

                                callForwardInfoList[i] = callForwardInfo;
                            }

                            mListenerProxy.utConfigurationCallForwardQueried((IImsUt)ar.userObj,
                                                                             msg.arg1,
                                                                             callForwardInfoList);
                        }
                        else {
                            Log.e(this, "Null response received for Query CF!");
                            mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                    msg.arg1,
                                    new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                        }
                    }
                    break;

                case EVENT_QUERY_CW:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            Log.e(this, "Query CW error");
                            if (ar.userObj != null) {
                                mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else if (ar.result != null) {
                            int[] cwResponse = (int[]) ar.result;

                            ImsSsInfo[] callWaitingInfoList = new ImsSsInfo[1];
                            ImsSsInfo callWaitingInfo = new ImsSsInfo();

                            if (cwResponse[0] == ImsQmiIF.ENABLED) {
                                if ((cwResponse[1] & SERVICE_CLASS_VOICE) == SERVICE_CLASS_VOICE) {
                                    callWaitingInfo.mStatus = ImsSsInfo.ENABLED;
                                } else {
                                    callWaitingInfo.mStatus = ImsSsInfo.DISABLED;
                                }
                            }
                            else if (cwResponse[0] == ImsQmiIF.DISABLED) {
                                callWaitingInfo.mStatus = ImsSsInfo.DISABLED;
                            }
                            else {
                                Log.e(this, "No service status received for CallWaitingInfo.");
                                mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                        msg.arg1,
                                        new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                                return;
                            }

                            //NOTE: Service status is VOICE by default, hence not checked.

                            callWaitingInfoList[0] = callWaitingInfo;

                            mListenerProxy.utConfigurationCallWaitingQueried((IImsUt)ar.userObj,
                                                                             msg.arg1,
                                                                             callWaitingInfoList);
                        }
                        else {
                            Log.e(this, "Null response received for Query CW!");
                            mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                    msg.arg1,
                                    new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0));
                        }
                    }
                    break;

                case EVENT_QUERY_CLIR:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            if (msg.what == EVENT_QUERY_CLIR) {
                                Log.e(this, "Query CLIR error");
                            }

                            if (ar.userObj != null) {
                                mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else if (ar.result != null) {
                            int[] clirResp = (int[]) ar.result;
                            Bundle clirInfo = new Bundle();
                            clirInfo.putIntArray(ImsPhoneMmiCode.UT_BUNDLE_KEY_CLIR, clirResp);
                            Log.i(this, "Calling success callback for Query CLIR.");
                            mListenerProxy.utConfigurationQueried((IImsUt) ar.userObj, msg.arg1,
                                    clirInfo);
                        }
                    }
                    break;

                case EVENT_QUERY_CLIP:
                    ar = (AsyncResult) msg.obj;
                    ImsQmiIF.ClipProvisionStatus clipStatus =
                            (ImsQmiIF.ClipProvisionStatus) ar.result;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            Log.e(this, "Error for Query Event= " + msg.what);
                            if (ar.userObj != null) {
                                ImsQmiIF.SipErrorInfo sipError = clipStatus.getErrorDetails();
                                if (sipError != null) {
                                    mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                            msg.arg1, ImsCallUtils.getImsReasonInfo(sipError,
                                            ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                                } else {
                                    mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                            msg.arg1,
                                            getImsReasonInfoFromResponseError(ar));
                                }
                            }
                        } else if (clipStatus != null) {
                            ImsSsInfo ssInfo = new ImsSsInfo();
                            ssInfo.mStatus = clipStatus.getClipStatus();
                            Bundle clInfo = new Bundle();
                            clInfo.putParcelable(ImsPhoneMmiCode.UT_BUNDLE_KEY_SSINFO,
                                    ssInfo);
                            Log.d(this, "Success callback on Query event= " + msg.what);
                            mListenerProxy.utConfigurationQueried((IImsUt) ar.userObj,
                                    msg.arg1, clInfo);
                        }
                    }
                    break;

                case EVENT_QUERY_COLR:
                    ar = (AsyncResult) msg.obj;
                    ImsQmiIF.Colr colr = (ImsQmiIF.Colr) ar.result;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            Log.e(this, "Error for Query Event= " + msg.what);

                            if (ar.userObj != null) {
                                ImsQmiIF.SipErrorInfo sipError = colr.getErrorDetails();
                                if (sipError != null) {
                                    mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                            msg.arg1, ImsCallUtils.getImsReasonInfo(sipError,
                                            ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                                } else {
                                    mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                            msg.arg1,
                                            getImsReasonInfoFromResponseError(ar));
                                }
                            }
                        } else if (colr != null) {
                            ImsSsInfo ssInfo = new ImsSsInfo();
                            ssInfo.mStatus = colr.getPresentation();
                            Bundle clInfo = new Bundle();
                            clInfo.putParcelable(ImsPhoneMmiCode.UT_BUNDLE_KEY_SSINFO,
                                    ssInfo);
                            Log.d(this, "Success callback on Query event= " + msg.what);
                            mListenerProxy.utConfigurationQueried((IImsUt) ar.userObj,
                                    msg.arg1, clInfo);
                        }
                    }
                    break;

                case EVENT_QUERY_COLP:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            Log.e(this, "Query COLP error");

                            if (ar.userObj != null) {
                                mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        }
                        else if (ar.result != null) {
                            // COLP response has failure cause information.
                            // Check for it to determine request's success of failure.
                            ImsQmiIF.SuppSvcResponse response = (ImsQmiIF.SuppSvcResponse) ar.result;
                            if(response.hasErrorDetails()) {
                                Log.e(this, "SuppSvcResponse has failure for COLP query.");
                                ImsQmiIF.SipErrorInfo sipError = response.getErrorDetails();
                                    mListenerProxy.utConfigurationQueryFailed((IImsUt) ar.userObj,
                                            msg.arg1, ImsCallUtils.getImsReasonInfo(sipError,
                                            ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                            } else if (response.hasFailureCause()) {
                                ImsReasonInfo error =
                                        new ImsReasonInfo(ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0);
                                error.mExtraMessage = response.getFailureCause();
                                Log.e(LOG_TAG, "COLP query failed with error = " + error);
                                mListenerProxy.utConfigurationQueryFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        error);
                            }
                            else {
                                ImsSsInfo ssInfo = new ImsSsInfo();
                                Bundle clInfo = new Bundle();
                                response = (ImsQmiIF.SuppSvcResponse) ar.result;
                                if (response.hasStatus()) {
                                    ssInfo.mStatus = response.getStatus();
                                    Log.i(this, "Service= " + msg.what + " status= "
                                            + ssInfo.mStatus);
                                    clInfo.putParcelable(ImsPhoneMmiCode.UT_BUNDLE_KEY_SSINFO,
                                            ssInfo);

                                    Log.i(this, "Success callback called for Query COLP.");
                                    mListenerProxy.utConfigurationQueried((IImsUt) ar.userObj,
                                            msg.arg1, clInfo);
                                }
                            }
                        }
                    }
                    break;

                case EVENT_UPDATE_CLIR:
                case EVENT_UPDATE_CLIP:
                case EVENT_UPDATE_COLR:
                case EVENT_UPDATE_COLP:

                    ImsReasonInfo error = new ImsReasonInfo(
                            ImsReasonInfo.CODE_UT_NETWORK_ERROR, 0);
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        if (msg.arg1 < 0) {
                            Log.e(this, "Invalid message id received in handleMessage.");
                            return;
                        }
                        if (ar.exception != null) {
                            if (msg.what == EVENT_UPDATE_CLIR) {
                                Log.e(this, "Update CLIR error");
                            }
                            else if (msg.what == EVENT_UPDATE_CLIP) {
                                Log.e(this, "Update CLIP error");
                            }
                            else if (msg.what == EVENT_UPDATE_COLR) {
                                Log.e(this, "Update COLR error");
                            }
                            else if (msg.what == EVENT_UPDATE_COLP) {
                                Log.e(this, "Update COLP error");
                            }
                            if (ar.result != null) {
                                // CLIP and COLP query responses have failure cause information.
                                // Check for it to determine request's success of failure.
                                ImsQmiIF.SuppSvcResponse response =
                                        (ImsQmiIF.SuppSvcResponse) ar.result;
                                if (response.hasErrorDetails()) {
                                    Log.e(this,
                                            "SuppSvcResponse has failure for CLIP/COLP update");
                                    ImsQmiIF.SipErrorInfo sipError = response.getErrorDetails();
                                    mListenerProxy.utConfigurationUpdateFailed(
                                           (IImsUt)ar.userObj, msg.arg1,
                                           ImsCallUtils.getImsReasonInfo(sipError,
                                           ImsReasonInfo.CODE_UT_NETWORK_ERROR));
                                } else if (response.hasFailureCause()) {
                                    error.mExtraMessage = response.getFailureCause();
                                    Log.e(LOG_TAG, "SuppSvc " + msg.what + " failed, error: "
                                            + error);
                                    mListenerProxy.utConfigurationUpdateFailed(
                                            (IImsUt)ar.userObj, msg.arg1, error);
                                } else {
                                    Log.e(LOG_TAG, "SuppSvcResponse failure with neither"
                                            + " errordetails nor failurecause");
                                    mListenerProxy.utConfigurationUpdateFailed(
                                            (IImsUt)ar.userObj, msg.arg1,
                                            getImsReasonInfoFromResponseError(ar));
                                }
                            } else if (ar.userObj != null) {
                                    Log.e(LOG_TAG, "SuppSvcResponse failure with valid userObj");
                                    mListenerProxy.utConfigurationUpdateFailed((IImsUt) ar.userObj,
                                            msg.arg1,
                                            getImsReasonInfoFromResponseError(ar));
                            } else {
                                // Nothing to pass to frameworks for this request's response.
                                Log.e(LOG_TAG, "SuppSvcResponse failure with neither ar.result"
                                         + " nor userObj");
                                mListenerProxy.utConfigurationUpdateFailed((IImsUt)ar.userObj,
                                        msg.arg1,
                                        getImsReasonInfoFromResponseError(ar));
                            }
                        } else {
                            Log.i(this, "Success callback called for msg.what= "
                                    + msg.what);
                            mListenerProxy.utConfigurationUpdated((IImsUt) ar.userObj, msg.arg1);
                        }
                    }
                    break;
                 case EVENT_UNSOL_ON_SS:
                    ar = (AsyncResult) msg.obj;
                    if (ar.exception == null) {
                        ImsSsData ssData = (ImsSsData) ar.result;
                        mListenerProxy.onSupplementaryServiceIndication(ssData);
                    } else {
                        Log.e(this, "exception in handling UNSOL_ON_SS");
                    }
                    break;
            }

        }

    }
}

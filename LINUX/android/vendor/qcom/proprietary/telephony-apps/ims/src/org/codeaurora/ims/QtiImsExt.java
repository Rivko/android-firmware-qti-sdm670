/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package org.codeaurora.ims;

import android.Manifest;
import android.content.Context;
import org.codeaurora.ims.internal.IQtiImsExt;
import org.codeaurora.ims.internal.IQtiImsExtListener;
import org.codeaurora.ims.QtiImsExtBase;
import org.codeaurora.ims.ImsCallUtils;

public class QtiImsExt extends QtiImsExtBase {
    private final String MODIFY_PHONE_STATE = Manifest.permission.MODIFY_PHONE_STATE;
    private final String READ_PHONE_STATE = Manifest.permission.READ_PHONE_STATE;

    private Context mContext;
    private ImsServiceSub mServiceSub[];

    public QtiImsExt(Context context, ImsServiceSub[] serviceSub) {
        mContext = context;
        mServiceSub = serviceSub;
    }

    @Override
    protected void onSetCallForwardUncondTimer(int phoneId, int startHour, int startMinute,
            int endHour, int endMinute, int action, int condition, int serviceClass, String number,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setCallForwardUncondTimer");
        mServiceSub[phoneId].setCallForwardUncondTimer(startHour, startMinute,
                endHour, endMinute, action, condition, serviceClass, number, listener);
    }

    @Override
    protected void onGetCallForwardUncondTimer(int phoneId, int reason, int serviceClass,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getCallForwardUncondTimer");
        mServiceSub[phoneId].getCallForwardUncondTimer(reason,
                serviceClass, listener);
    }

    @Override
    protected void onGetPacketCount(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getPacketCount");
        mServiceSub[phoneId].getPacketCount(listener);
    }

    @Override
    protected void onGetPacketErrorCount(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getPacketErrorCount");
        mServiceSub[phoneId].getPacketErrorCount(listener);
    }

    @Override
    protected void onSendCallDeflectRequest(int phoneId, String deflectNumber,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "sendCallDeflectRequest");
        mServiceSub[phoneId].sendCallDeflectRequest(deflectNumber, listener);
    }

    @Override
    protected void onResumePendingCall(int phoneId, int videoState) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "resumePendingCall");
        mServiceSub[phoneId].resumePendingCall(videoState);
    }

    @Override
    protected void onSendCallTransferRequest(int phoneId, int type, String number,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "sendCallTransferRequest");
        mServiceSub[phoneId].sendCallTransferRequest(type, number, listener);
    }

    @Override
    protected void onSendCancelModifyCall(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "sendCancelModifyCall");
        mServiceSub[phoneId].sendCancelModifyCall(listener);
    }

    @Override
    protected void onQueryVopsStatus(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "queryVopsStatus");
        mServiceSub[phoneId].queryVopsStatus(listener);
    }

    @Override
    protected void onQuerySsacStatus(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "querySsacStatus");
        mServiceSub[phoneId].querySsacStatus(listener);
    }

    @Override
    protected void onRegisterForParticipantStatusInfo(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE,
                "registerForParticipantStatusInfo");
        mServiceSub[phoneId].registerForParticipantStatusInfo(listener);
    }

    @Override
    protected void onUpdateVoltePreference(int phoneId, int preference,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateVoltePreference");
        mServiceSub[phoneId].updateVoltePreference(preference, listener);
    }

    @Override
    protected void onQueryVoltePreference(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "queryVoltePreference");
        mServiceSub[phoneId].queryVoltePreference(listener);
    }

    @Override
    protected void onGetHandoverConfig(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getHandoverConfig");
        mServiceSub[phoneId].getHandoverConfig(listener);
    }

    @Override
    protected void onSetHandoverConfig(int phoneId, int hoConfig, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setHandoverConfig");
        mServiceSub[phoneId].setHandoverConfig(hoConfig, listener);
    }

    @Override
    protected int onGetVvmAppConfig(int phoneId) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getVvmAppConfig");
        return mServiceSub[phoneId].getConfigInterface().getProvisionedValue(
                ImsCallUtils.QTI_CONFIG_VVM_APP);
    }

    @Override
    protected int onSetVvmAppConfig(int phoneId, int defaultVvmApp) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setVvmAppConfig");
        return mServiceSub[phoneId].getConfigInterface().setProvisionedValue(
                ImsCallUtils.QTI_CONFIG_VVM_APP, defaultVvmApp);
    }

    @Override
    protected int onGetRcsAppConfig(int phoneId) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getRcsAppConfig");
        return mServiceSub[phoneId].getConfigInterface().getProvisionedValue(
                ImsCallUtils.QTI_CONFIG_SMS_APP);
    }

    @Override
    protected int onSetRcsAppConfig(int phoneId, int defaultSmsApp) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setRcsAppConfig");
        return mServiceSub[phoneId].getConfigInterface().setProvisionedValue(
                ImsCallUtils.QTI_CONFIG_SMS_APP, defaultSmsApp);
    }
}

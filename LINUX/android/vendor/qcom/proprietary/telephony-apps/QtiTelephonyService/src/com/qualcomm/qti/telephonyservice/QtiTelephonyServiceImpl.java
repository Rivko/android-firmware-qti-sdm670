/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.telephonyservice;

import java.util.Arrays;

import com.google.protobuf.micro.InvalidProtocolBufferMicroException;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import com.qualcomm.qti.telephonyservice.IQtiTelephonyService;

import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.IQcRilHook;

public class QtiTelephonyServiceImpl extends IQtiTelephonyService.Stub {

    private static final String TAG = "QtiTelephonyServiceImpl";

    private static final String VERSION = "1";
    private static final String ACCESS_USER_AUTHENTICATION_APIS_PERMISSION =
            "com.qualcomm.qti.permission.ACCESS_USER_AUTHENTICATION_APIS";
    private static final String ACCESS_USER_AUTHENTICATION_APIS_ERROR =
            "Need " + ACCESS_USER_AUTHENTICATION_APIS_PERMISSION + " to access api";

    private IQcRilHook mQcRilHook;
    private final Context mContext;
    private RilOemMessageBuilder messageBuilder;

    public QtiTelephonyServiceImpl(Context context, IQcRilHook qcRilHook) {
        this.mContext = context;

        // Allow instrumentation to set a test double for IQcRilHook.
        this.mQcRilHook = qcRilHook;

        messageBuilder = new RilOemMessageBuilder();
        Log.d(TAG, "Service created. Version=" + VERSION);
    }

    protected void setQcRilHook(IQcRilHook qcRilHook) {
        mQcRilHook = qcRilHook;
    }

    public String getVersion() {
        return VERSION;
    }

    public KsNafResponse gbaInit(byte[] securityProtocol, String nafFullyQualifiedDomainName,
            int slotId, int applicationType, boolean forceBootStrapping) {
        enforceAccessUserAuthenticationApisPermission();

        KsNafResponse response = null;

        try {
            byte[] request =
                    messageBuilder.buildGbaInitRequest(securityProtocol,
                         nafFullyQualifiedDomainName, slotId, applicationType, forceBootStrapping);

            byte[] rilResponseProto = mQcRilHook.qcRilSendProtocolBufferMessage(request, slotId);

            response = RilOemProtoParser.parseKsNafResponse(rilResponseProto,
                    TelephonyServiceProtos.RIL_OEM_GBA_INIT);
            return response;
        } catch (IllegalArgumentException ex) {
            Log.e(TAG, "IllegalArgumentException: " + ex);
            return response;
        } catch (InvalidProtocolBufferMicroException ex) {
            Log.e(TAG, "InvalidProtocolBufferMicroException: " + ex);
            return response;
        }
    }

    public byte[] getImpi(int slotId, int applicationType, boolean secure) {
        enforceAccessUserAuthenticationApisPermission();
        byte[] response = new byte[] {};

        try {
            byte[] request = messageBuilder.buildImpiRequest(slotId, applicationType, secure);

            byte[] rilResponseProto = mQcRilHook.qcRilSendProtocolBufferMessage(request, slotId);

            byte[] impi = RilOemProtoParser.parseImpi(rilResponseProto,
                    TelephonyServiceProtos.RIL_OEM_IMPI);
            return impi;
        } catch (IllegalArgumentException ex) {
            Log.e(TAG, "IllegalArgumentException: " + ex);
            return response;
        } catch (InvalidProtocolBufferMicroException ex) {
            Log.e(TAG, "InvalidProtocolBufferMicroException: " + ex);
            return response;
        }
    }

    private void enforceAccessUserAuthenticationApisPermission() {
        mContext.enforceCallingOrSelfPermission(ACCESS_USER_AUTHENTICATION_APIS_PERMISSION,
                ACCESS_USER_AUTHENTICATION_APIS_ERROR);
    }

}

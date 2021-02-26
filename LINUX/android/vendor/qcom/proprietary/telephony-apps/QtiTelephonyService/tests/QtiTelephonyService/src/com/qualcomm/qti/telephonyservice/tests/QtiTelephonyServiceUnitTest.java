/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.telephonyservice.tests;

import java.util.Arrays;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.os.IBinder;
import android.test.ServiceTestCase;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import com.google.protobuf.micro.ByteStringMicro;
import com.qualcomm.qti.telephonyservice.QtiTelephonyService;
import com.qualcomm.qti.telephonyservice.KsNafResponse;
import com.qualcomm.qcrilhook.IQcRilHook;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verifyNoMoreInteractions;
import static org.mockito.Mockito.reset;
import static org.mockito.Matchers.anyByte;
import static org.mockito.Matchers.anyObject;

import org.mockito.Mockito;

import com.qualcomm.qti.telephonyservice.IQtiTelephonyService;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RILOEMMessage;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemGbaInitRequestPayload;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemGbaInitResponsePayload;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemImpiRequestPayload;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemImpiResponsePayload;

public class QtiTelephonyServiceUnitTest extends ServiceTestCase<QtiTelephonyService> {

    private IQtiTelephonyService mQtiTelephonyService;
    private IQcRilHook mQcRilHookMock;

    private static final int TOKEN_BASE = 1000;
    private int mToken = TOKEN_BASE;

    private static final int RIL_OEM_MSG_REQUEST = 1;
    private static final int RIL_OEM_MSG_RESPONSE = 2;
    private static final int RIL_OEM_MSG_INDICATION = 3;

    private static final int RIL_OEM_GBA_INIT = 1;
    private static final int RIL_OEM_SECURE_IMPI = 2;

    private static final int KsNAF_TYPE_ENCRYPTED = 1;
    public static final int RIL_OEM_ERR_SUCCESS = 0;

    public static final int RIL_OEM_ERR_GENERIC_FAILURE = 1;
    public static final int RIL_OEM_ERR_NOT_SUPPORTED = 2;
    public static final int RIL_OEM_ERR_INVALID_PARAMETER = 3;

    private static final int INVALID_SLOT_ID = 200;
    private static final int INVALID_APPLICATION_ID = 100;

    public QtiTelephonyServiceUnitTest() {
        super(QtiTelephonyService.class);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        Intent startIntent = new Intent();
        startIntent.setClass(getSystemContext(), QtiTelephonyService.class);
        IBinder service = bindService(startIntent);
        assertNotNull(service);

        mQtiTelephonyService = IQtiTelephonyService.Stub.asInterface(service);
        assertNotNull(mQtiTelephonyService);

        mQcRilHookMock = mock(IQcRilHook.class);
        // QtiTelephonyService to use the mock qcRilhook
        // Use reflection to set mock QcRilHook on the service
        // getService().setQcRilHook(mQcRilHookMock)
        setMockQcRilHookOnService();
    }

    private void setMockQcRilHookOnService() throws IllegalAccessException,
            InvocationTargetException, NoSuchMethodException {
        Class[] parameterTypes = new Class[1];
        parameterTypes[0] = IQcRilHook.class;
        Method setQcRilHook =
                getService().getClass().getDeclaredMethod("setQcRilHook", parameterTypes);
        setQcRilHook.setAccessible(true);
        Object[] parameters = new Object[1];
        parameters[0] = mQcRilHookMock;
        setQcRilHook.invoke(getService(), parameters);
    }

    public void testQtiTelephonyServiceVersion() throws RemoteException {
        String version = mQtiTelephonyService.getVersion();
        assertNotNull(version);
        assertFalse(version.isEmpty());
    }

    public void testSecureImpi() throws RemoteException {
        byte[] encryptedImpi = new byte[]{1,1,1,1,1};
        int slotId = 0;
        int applicationType = 5;

        // 1. Prepare mock and script its behaviour
        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(
                new byte[]{anyByte()}))
                .thenReturn(buildImpiMessageResponse(encryptedImpi).toByteArray());
        // 2. Invoke api to be tested
        byte[] secureImpi = mQtiTelephonyService.getImpi(slotId, applicationType, true);
        // 3. Validate api response
        assertFalse(secureImpi.length == 0);
        assertTrue(Arrays.equals(encryptedImpi, secureImpi));
        // 4. Validate mock saw what it was expecting
        Mockito.verify(mQcRilHookMock).qcRilSendProtocolBufferMessage(
                buildImpiMessageRequest(slotId, applicationType, true).toByteArray());
        Mockito.verifyNoMoreInteractions(mQcRilHookMock);
    }

    public void testSecureImpiInvalidMessageType() throws RemoteException {
        RILOEMMessage impiResponse = buildImpiMessageResponse(new byte[]{1});
        impiResponse.setType(RIL_OEM_MSG_REQUEST);

        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(impiResponse.toByteArray());

        byte[] secureImpi = mQtiTelephonyService.getImpi(0, 1, true);
        assertTrue(secureImpi.length == 0);
    }

    public void testSecureImpiInvalidMessageId() throws RemoteException {
        RILOEMMessage impiResponse = buildImpiMessageResponse(new byte[]{1});
        impiResponse.setId(RIL_OEM_GBA_INIT);

        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(impiResponse.toByteArray());

        byte[] secureImpi = mQtiTelephonyService.getImpi(0, 1, true);
        assertTrue(secureImpi.length == 0);
    }

    public void testSecureImpiInvalidSlotId() throws RemoteException {
        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(buildImpiMessageResponse(new byte[]{1,1,1,1,1}).toByteArray());

        byte[] secureImpi = mQtiTelephonyService.getImpi(INVALID_SLOT_ID, 1, true);
        assertTrue(secureImpi.length == 0);
    }

    public void testSecureImpiInvalidApplicationType() throws RemoteException {
        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(buildImpiMessageResponse(new byte[]{1,1,1,1,1}).toByteArray());

        byte[] secureImpi = mQtiTelephonyService.getImpi(0, INVALID_APPLICATION_ID, true);
        assertTrue(secureImpi.length == 0);
    }

    public void testGbaInit() throws RemoteException {
        byte[] response = new byte[]{1,1,1,1,1};
        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(buildGbaInitMessageResponse(response).toByteArray());
        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(new byte[]{0,1,1,1,1},
                                                              "nafFullyQualifiedDomainName",
                                                              0,
                                                              1,
                                                              true);
        assertNotNull(ksNafresponse);
        assertEquals(KsNAF_TYPE_ENCRYPTED, ksNafresponse.getType());
        assertTrue(Arrays.equals(response, ksNafresponse.getResponse()));
        assertEquals("bootstrapTransactionId", ksNafresponse.getBootstrapTransactionId());
        assertEquals("ksLifetime", ksNafresponse.getLifetime());
    }

    public void testGbaInitInvalidMessageType() throws RemoteException {
        RILOEMMessage invalidMessageType = buildGbaInitMessageResponse(new byte[]{});
        invalidMessageType.setType(RIL_OEM_MSG_REQUEST);

        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(invalidMessageType.toByteArray());

        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(new byte[]{1,1,1,1,1},
                                                                  "nafFullyQualifiedDomainName",
                                                                  0,
                                                                  1,
                                                                  true);
        assertNull(ksNafresponse);
    }

    public void testGbaInitInvalidMessageId() throws RemoteException {
        RILOEMMessage invalidMessageId = buildGbaInitMessageResponse(new byte[]{});
        invalidMessageId.setId(RIL_OEM_SECURE_IMPI);

        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(invalidMessageId.toByteArray());

        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(new byte[]{1,1,1,1,1},
                                                              "nafFullyQualifiedDomainName",
                                                              0,
                                                              1,
                                                              true);
        assertNull(ksNafresponse);
    }

    public void testGbaInitInvalidSlotId() throws RemoteException {
        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(buildGbaInitMessageResponse(new byte[]{}).toByteArray());

        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(new byte[]{1,1,1,1,1},
                                                                    "nafFullyQualifiedDomainName",
                                                                    INVALID_SLOT_ID,
                                                                    5,
                                                                    true);
        assertNull(ksNafresponse);
    }

    public void testGbaInitInvalidApplicationType() throws RemoteException {
        reset(mQcRilHookMock);
        Mockito.when(mQcRilHookMock.qcRilSendProtocolBufferMessage(new byte[]{anyByte()}))
                .thenReturn(buildGbaInitMessageResponse(new byte[]{}).toByteArray());

        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(new byte[]{1,1,1,1,1},
                                                                    "nafFullyQualifiedDomainName",
                                                                    0,
                                                                    INVALID_APPLICATION_ID,
                                                                    true);
        assertNull(ksNafresponse);
    }

    private RILOEMMessage buildImpiMessageResponse(byte[] encryptedImsi) {
        RilOemImpiResponsePayload response = new RilOemImpiResponsePayload();
        response.setImpi(ByteStringMicro.copyFrom(encryptedImsi));

        RILOEMMessage message = new RILOEMMessage();
        message.setToken(100);
        message.setType(RIL_OEM_MSG_RESPONSE);
        message.setId(RIL_OEM_SECURE_IMPI);
        message.setError(RIL_OEM_ERR_SUCCESS);
        message.setPayload(ByteStringMicro.copyFrom(response.toByteArray()));
        return message;
    }

    private RILOEMMessage buildImpiMessageRequest(int slotId,
                                                  int applicationType,
                                                  boolean secure) {
        RilOemImpiRequestPayload request = new RilOemImpiRequestPayload();
        request.setSlotId(slotId);
        request.setApplicationType(applicationType);
        request.setSecure(secure);

        RILOEMMessage message = new RILOEMMessage();
        mToken++;
        message.setToken(mToken);
        message.setType(RIL_OEM_MSG_REQUEST);
        message.setId(RIL_OEM_SECURE_IMPI);
        message.setPayload(ByteStringMicro.copyFrom(request.toByteArray()));
        return message;
    }

    private RILOEMMessage buildGbaInitMessageResponse(byte[] nafResponse) {
        RilOemGbaInitResponsePayload response = new RilOemGbaInitResponsePayload();
        response.setKsNAFType(KsNAF_TYPE_ENCRYPTED);
        response.setKsNAFResponse(ByteStringMicro.copyFrom(nafResponse));
        response.setBootstrapTransactionId("bootstrapTransactionId");
        response.setKsLifetime("ksLifetime");

        RILOEMMessage message = new RILOEMMessage();
        message.setToken(100);
        message.setType(RIL_OEM_MSG_RESPONSE);
        message.setId(RIL_OEM_GBA_INIT);
        message.setError(RIL_OEM_ERR_SUCCESS);
        message.setPayload(ByteStringMicro.copyFrom(response.toByteArray()));
        return message;
    }
}

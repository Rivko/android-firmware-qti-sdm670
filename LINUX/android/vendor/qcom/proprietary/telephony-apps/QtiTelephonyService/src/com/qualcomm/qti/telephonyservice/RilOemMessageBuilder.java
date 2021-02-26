/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.telephonyservice;

import android.util.Log;

import com.google.protobuf.micro.ByteStringMicro;

import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RILOEMMessage;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemGbaInitRequestPayload;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemImpiRequestPayload;

public class RilOemMessageBuilder {

    private static final String TAG = "RilOemMessageBuilder";

    private static final int TOKEN_BASE = 1000;
    private int mToken = TOKEN_BASE;

    public byte[] buildImpiRequest(int slotId, int applicationType, boolean secure) {
        validateSlotId(slotId);
        validateApplicationType(applicationType);
        RilOemImpiRequestPayload impiRequest = new RilOemImpiRequestPayload();
        impiRequest.setSlotId(slotId);
        impiRequest.setApplicationType(applicationType);
        impiRequest.setSecure(secure);

        return buildMessage(TelephonyServiceProtos.RIL_OEM_IMPI, impiRequest.toByteArray());
    }

    public byte[] buildGbaInitRequest(byte[] securityProtocol,
            String nafFullyQualifiedDomainName, int slotId, int applicationType,
            boolean forceBootStrapping) {
        validateSlotId(slotId);
        validateApplicationType(applicationType);
        if (nafFullyQualifiedDomainName == null || nafFullyQualifiedDomainName.isEmpty()) {
            throw new IllegalArgumentException(
                    "nafFullyQualifiedDomainName cannot be null or empty");
        }
        if (securityProtocol.length == 0) {
            throw new IllegalArgumentException("securityProtocol cannot be null or empty");
        }

        RilOemGbaInitRequestPayload gbaInitRequest = new RilOemGbaInitRequestPayload();
        gbaInitRequest.setSecurityProtocol(ByteStringMicro.copyFrom(securityProtocol));
        gbaInitRequest.setNafFullyQualifiedDomainName(nafFullyQualifiedDomainName);
        gbaInitRequest.setSlotId(slotId);
        gbaInitRequest.setApplicationType(applicationType);
        gbaInitRequest.setForceBootstrapping(forceBootStrapping);

        return buildMessage(TelephonyServiceProtos.RIL_OEM_GBA_INIT, gbaInitRequest.toByteArray());
    }

    private byte[] buildMessage(int messageId, byte[] payload) {
        RILOEMMessage messageTag = new RILOEMMessage();
        messageTag.setToken(incrementToken());
        messageTag.setType(TelephonyServiceProtos.RIL_OEM_MSG_REQUEST);
        messageTag.setId(messageId);
        messageTag.setPayload(ByteStringMicro.copyFrom(payload));
        return messageTag.toByteArray();
    }

    private int incrementToken() {
        synchronized (this) {
            return ++mToken;
        }
    }

    private void validateSlotId(int slotId) {
        if (slotId != TelephonyServiceProtos.SLOT_ID_ONE
                && slotId != TelephonyServiceProtos.SLOT_ID_TWO
                && slotId != TelephonyServiceProtos.SLOT_ID_THREE) {
            throw new IllegalArgumentException("Not a valid SlotId");
        }
    }

    private void validateApplicationType(int applicationType) {
        if (applicationType != TelephonyServiceProtos.APPTYPE_UNKNOWN
                && applicationType != TelephonyServiceProtos.APPTYPE_SIM
                && applicationType != TelephonyServiceProtos.APPTYPE_USIM
                && applicationType != TelephonyServiceProtos.APPTYPE_RUIM
                && applicationType != TelephonyServiceProtos.APPTYPE_CSIM
                && applicationType != TelephonyServiceProtos.APPTYPE_ISIM) {
            throw new IllegalArgumentException("Not a valid ApplicationType");
        }
    }
}

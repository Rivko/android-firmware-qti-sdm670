/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.telephonyservice;

import android.util.Log;

import com.google.protobuf.micro.InvalidProtocolBufferMicroException;

import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RILOEMMessage;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemGbaInitResponsePayload;
import com.qualcomm.qti.telephonyservice.TelephonyServiceProtos.RilOemImpiResponsePayload;

public class RilOemProtoParser {

    private static final String TAG = "RilOemProtoParser";

    private static void validateProtoBuf(byte[] protobuf) {
        if (protobuf == null) {
            throw new IllegalArgumentException("protoBuf cannot be null ");
        }
    }

    private static void validateResponse(RILOEMMessage response, int messageId) {
        if (TelephonyServiceProtos.RIL_OEM_MSG_RESPONSE != response.getType()) {
            throw new IllegalArgumentException(
                    "Expected RilOemMessageType " + TelephonyServiceProtos.RIL_OEM_MSG_RESPONSE);
        }
        if (messageId != response.getId()) {
            throw new IllegalArgumentException("Expected RilOemMessageId " + messageId);
        }
        if (response.hasError()
                && TelephonyServiceProtos.RIL_OEM_ERR_SUCCESS != response.getError()) {
            Log.e(TAG, "RilOemError: " + response.getError());
            throw new IllegalArgumentException(
                    "Expected RilOemError " + TelephonyServiceProtos.RIL_OEM_ERR_SUCCESS);
        }
    }

    public static byte[] parseImpi(byte[] protobuf, int messageId)
            throws InvalidProtocolBufferMicroException {
        validateProtoBuf(protobuf);

        RILOEMMessage responseMessage = RILOEMMessage.parseFrom(protobuf);
        validateResponse(responseMessage, messageId);

        RilOemImpiResponsePayload impiResponse =
                RilOemImpiResponsePayload.parseFrom(responseMessage.getPayload().toByteArray());
        return impiResponse.getImpi().toByteArray();
    }

    public static KsNafResponse parseKsNafResponse(byte[] protobuf, int messageId)
            throws InvalidProtocolBufferMicroException {
        validateProtoBuf(protobuf);

        RILOEMMessage responseMessage = RILOEMMessage.parseFrom(protobuf);
        validateResponse(responseMessage, messageId);

        RilOemGbaInitResponsePayload gbaInitResponse =
                RilOemGbaInitResponsePayload.parseFrom(responseMessage.getPayload().toByteArray());
        return new KsNafResponse(
                gbaInitResponse.getKsNAFType(),
                gbaInitResponse.getKsNAFResponse().toByteArray(),
                gbaInitResponse.getBootstrapTransactionId(),
                gbaInitResponse.getKsLifetime());
    }
}

/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qtiTelephonyService.pb.c
  @brief

  DESCRIPTION
    Handles User Authentication message nanopb encode/decode related functions.

******************************************************************************/

#include "qtiTelephonyService.pb.h"



const pb_field_t QtiTelephonyService_RILOEMMessage_fields[6] = {
    PB_FIELD2(  1, FIXED32 , REQUIRED, STATIC  , FIRST, QtiTelephonyService_RILOEMMessage, token, token, 0),
    PB_FIELD2(  2, ENUM    , REQUIRED, STATIC  , OTHER, QtiTelephonyService_RILOEMMessage, type, token, 0),
    PB_FIELD2(  3, ENUM    , REQUIRED, STATIC  , OTHER, QtiTelephonyService_RILOEMMessage, id, type, 0),
    PB_FIELD2(  4, ENUM    , OPTIONAL, STATIC  , OTHER, QtiTelephonyService_RILOEMMessage, error, id, 0),
    PB_FIELD2(  5, BYTES   , OPTIONAL, CALLBACK, OTHER, QtiTelephonyService_RILOEMMessage, payload, error, 0),
    PB_LAST_FIELD
};

const pb_field_t QtiTelephonyService_RilOemGbaInitRequestPayload_fields[6] = {
    PB_FIELD2(  1, BYTES   , REQUIRED, CALLBACK, FIRST, QtiTelephonyService_RilOemGbaInitRequestPayload, securityProtocol, securityProtocol, 0),
    PB_FIELD2(  2, STRING  , REQUIRED, CALLBACK, OTHER, QtiTelephonyService_RilOemGbaInitRequestPayload, nafFullyQualifiedDomainName, securityProtocol, 0),
    PB_FIELD2(  3, ENUM    , REQUIRED, STATIC  , OTHER, QtiTelephonyService_RilOemGbaInitRequestPayload, slotId, nafFullyQualifiedDomainName, 0),
    PB_FIELD2(  4, ENUM    , REQUIRED, STATIC  , OTHER, QtiTelephonyService_RilOemGbaInitRequestPayload, applicationType, slotId, 0),
    PB_FIELD2(  5, BOOL    , OPTIONAL, STATIC  , OTHER, QtiTelephonyService_RilOemGbaInitRequestPayload, forceBootstrapping, applicationType, 0),
    PB_LAST_FIELD
};

const pb_field_t QtiTelephonyService_RilOemGbaInitResponsePayload_fields[6] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, QtiTelephonyService_RilOemGbaInitResponsePayload, ksNAFType, ksNAFType, 0),
    PB_FIELD2(  2, BYTES   , REQUIRED, CALLBACK, OTHER, QtiTelephonyService_RilOemGbaInitResponsePayload, ksNAFResponse, ksNAFType, 0),
    PB_FIELD2(  3, BYTES   , REQUIRED, CALLBACK, OTHER, QtiTelephonyService_RilOemGbaInitResponsePayload, impi, ksNAFResponse, 0),
    PB_FIELD2(  4, STRING  , REQUIRED, CALLBACK, OTHER, QtiTelephonyService_RilOemGbaInitResponsePayload, bootstrapTransactionId, impi, 0),
    PB_FIELD2(  5, STRING  , REQUIRED, CALLBACK, OTHER, QtiTelephonyService_RilOemGbaInitResponsePayload, ksLifetime, bootstrapTransactionId, 0),
    PB_LAST_FIELD
};

const pb_field_t QtiTelephonyService_RilOemImpiRequestPayload_fields[4] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, QtiTelephonyService_RilOemImpiRequestPayload, slotId, slotId, 0),
    PB_FIELD2(  2, ENUM    , REQUIRED, STATIC  , OTHER, QtiTelephonyService_RilOemImpiRequestPayload, applicationType, slotId, 0),
    PB_FIELD2(  3, BOOL    , REQUIRED, STATIC  , OTHER, QtiTelephonyService_RilOemImpiRequestPayload, secure, applicationType, 0),
    PB_LAST_FIELD
};

const pb_field_t QtiTelephonyService_RilOemImpiResponsePayload_fields[2] = {
    PB_FIELD2(  1, BYTES   , REQUIRED, CALLBACK, FIRST, QtiTelephonyService_RilOemImpiResponsePayload, impi, impi, 0),
    PB_LAST_FIELD
};



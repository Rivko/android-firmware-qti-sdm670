/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qtiTelephonyService.pb.h
  @brief

  DESCRIPTION
    Handles User Authentication message nanopb encode/decode related functions.

******************************************************************************/

#ifndef _PB_QTITELEPHONYSERVICE_PB_H_
#define _PB_QTITELEPHONYSERVICE_PB_H_
#include <pb.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _QtiTelephonyService_RILOEMMessageType {
    QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_REQUEST = 1,
    QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE = 2,
    QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_INDICATION = 3
} QtiTelephonyService_RILOEMMessageType;

typedef enum _QtiTelephonyService_RILOEMMessageId {
    QtiTelephonyService_RILOEMMessageId_RIL_OEM_GBA_INIT = 1,
    QtiTelephonyService_RILOEMMessageId_RIL_OEM_IMPI = 2
} QtiTelephonyService_RILOEMMessageId;

typedef enum _QtiTelephonyService_RILOEMError {
    QtiTelephonyService_RILOEMError_RIL_OEM_ERR_SUCCESS = 0,
    QtiTelephonyService_RILOEMError_RIL_OEM_ERR_GENERIC_FAILURE = 1,
    QtiTelephonyService_RILOEMError_RIL_OEM_ERR_NOT_SUPPORTED = 2,
    QtiTelephonyService_RILOEMError_RIL_OEM_ERR_INVALID_PARAMETER = 3
} QtiTelephonyService_RILOEMError;

typedef enum _QtiTelephonyService_RILOEMKsNAFType {
    QtiTelephonyService_RILOEMKsNAFType_KS_NAF_TYPE_PLAIN = 0,
    QtiTelephonyService_RILOEMKsNAFType_KS_NAF_TYPE_ENCRYPTED = 1
} QtiTelephonyService_RILOEMKsNAFType;

typedef enum _QtiTelephonyService_RILOEMApplicationType {
    QtiTelephonyService_RILOEMApplicationType_APPTYPE_UNKNOWN = 0,
    QtiTelephonyService_RILOEMApplicationType_APPTYPE_SIM = 1,
    QtiTelephonyService_RILOEMApplicationType_APPTYPE_USIM = 2,
    QtiTelephonyService_RILOEMApplicationType_APPTYPE_RUIM = 3,
    QtiTelephonyService_RILOEMApplicationType_APPTYPE_CSIM = 4,
    QtiTelephonyService_RILOEMApplicationType_APPTYPE_ISIM = 5
} QtiTelephonyService_RILOEMApplicationType;

typedef enum _QtiTelephonyService_RILOEMSlotIdentifier {
    QtiTelephonyService_RILOEMSlotIdentifier_SLOT_ID_ONE = 0,
    QtiTelephonyService_RILOEMSlotIdentifier_SLOT_ID_TWO = 1,
    QtiTelephonyService_RILOEMSlotIdentifier_SLOT_ID_THREE = 2
} QtiTelephonyService_RILOEMSlotIdentifier;

/* Struct definitions */
typedef struct _QtiTelephonyService_RILOEMMessage {
    uint32_t token;
    QtiTelephonyService_RILOEMMessageType type;
    QtiTelephonyService_RILOEMMessageId id;
    bool has_error;
    QtiTelephonyService_RILOEMError error;
    pb_callback_t payload;
} QtiTelephonyService_RILOEMMessage;

typedef struct _QtiTelephonyService_RilOemGbaInitRequestPayload {
    pb_callback_t securityProtocol;
    pb_callback_t nafFullyQualifiedDomainName;
    QtiTelephonyService_RILOEMSlotIdentifier slotId;
    QtiTelephonyService_RILOEMApplicationType applicationType;
    bool has_forceBootstrapping;
    bool forceBootstrapping;
} QtiTelephonyService_RilOemGbaInitRequestPayload;

typedef struct _QtiTelephonyService_RilOemGbaInitResponsePayload {
    QtiTelephonyService_RILOEMKsNAFType ksNAFType;
    pb_callback_t ksNAFResponse;
    pb_callback_t impi;
    pb_callback_t bootstrapTransactionId;
    pb_callback_t ksLifetime;
} QtiTelephonyService_RilOemGbaInitResponsePayload;

typedef struct _QtiTelephonyService_RilOemImpiRequestPayload {
    QtiTelephonyService_RILOEMSlotIdentifier slotId;
    QtiTelephonyService_RILOEMApplicationType applicationType;
    bool secure;
} QtiTelephonyService_RilOemImpiRequestPayload;

typedef struct _QtiTelephonyService_RilOemImpiResponsePayload {
    pb_callback_t impi;
} QtiTelephonyService_RilOemImpiResponsePayload;

/* Default values for struct fields */

/* Field tags (for use in manual encoding/decoding) */
#define QtiTelephonyService_RILOEMMessage_token_tag 1
#define QtiTelephonyService_RILOEMMessage_type_tag 2
#define QtiTelephonyService_RILOEMMessage_id_tag 3
#define QtiTelephonyService_RILOEMMessage_error_tag 4
#define QtiTelephonyService_RILOEMMessage_payload_tag 5
#define QtiTelephonyService_RilOemGbaInitRequestPayload_securityProtocol_tag 1
#define QtiTelephonyService_RilOemGbaInitRequestPayload_nafFullyQualifiedDomainName_tag 2
#define QtiTelephonyService_RilOemGbaInitRequestPayload_slotId_tag 3
#define QtiTelephonyService_RilOemGbaInitRequestPayload_applicationType_tag 4
#define QtiTelephonyService_RilOemGbaInitRequestPayload_forceBootstrapping_tag 5
#define QtiTelephonyService_RilOemGbaInitResponsePayload_ksNAFType_tag 1
#define QtiTelephonyService_RilOemGbaInitResponsePayload_ksNAFResponse_tag 2
#define QtiTelephonyService_RilOemGbaInitResponsePayload_impi_tag 3
#define QtiTelephonyService_RilOemGbaInitResponsePayload_bootstrapTransactionId_tag 4
#define QtiTelephonyService_RilOemGbaInitResponsePayload_ksLifetime_tag 5
#define QtiTelephonyService_RilOemImpiRequestPayload_slotId_tag 1
#define QtiTelephonyService_RilOemImpiRequestPayload_applicationType_tag 2
#define QtiTelephonyService_RilOemImpiRequestPayload_secure_tag 3
#define QtiTelephonyService_RilOemImpiResponsePayload_impi_tag 1

/* Struct field encoding specification for nanopb */
extern const pb_field_t QtiTelephonyService_RILOEMMessage_fields[6];
extern const pb_field_t QtiTelephonyService_RilOemGbaInitRequestPayload_fields[6];
extern const pb_field_t QtiTelephonyService_RilOemGbaInitResponsePayload_fields[6];
extern const pb_field_t QtiTelephonyService_RilOemImpiRequestPayload_fields[4];
extern const pb_field_t QtiTelephonyService_RilOemImpiResponsePayload_fields[2];

/* Maximum encoded size of messages (where known) */
#define QtiTelephonyService_RilOemImpiRequestPayload_size 14

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

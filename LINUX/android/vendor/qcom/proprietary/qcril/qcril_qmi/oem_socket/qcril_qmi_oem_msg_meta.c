/******************************************************************************
  @file    qcril_qmi_oem_msg_meta.c
  @brief   qcril - OEM HOOK message meta info

  DESCRIPTION
    Handles OEM HOOK message meta info related functions.

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_qmi_oem_msg_meta.h"

typedef struct
{
    QtiTelephonyService_RILOEMMessageId msg_id;
    QtiTelephonyService_RILOEMMessageType msg_type;
    const pb_field_t* msg_pb_fields;
    size_t msg_req_size;
    qcril_evt_e_type msg_event;
    const char* msg_log_str;
} qcril_qmi_oem_proto_msg_meta_type;

const qcril_qmi_oem_proto_msg_meta_type qcril_qmi_oem_proto_msg_meta_data[] =
{
    // Requests and Response
    {QtiTelephonyService_RILOEMMessageId_RIL_OEM_GBA_INIT, QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_REQUEST,  QtiTelephonyService_RilOemGbaInitRequestPayload_fields, sizeof(QtiTelephonyService_RilOemGbaInitRequestPayload), QCRIL_EVT_OEM_MSG_GBA_INIT, "QCRIL_EVT_OEM_MSG_GBA_INIT_REQ"},
    {QtiTelephonyService_RILOEMMessageId_RIL_OEM_GBA_INIT, QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE,  QtiTelephonyService_RilOemGbaInitResponsePayload_fields, sizeof(QtiTelephonyService_RilOemGbaInitResponsePayload), QCRIL_EVT_NONE, "QCRIL_EVT_OEM_MSG_GBA_INIT_RSP"},
    {QtiTelephonyService_RILOEMMessageId_RIL_OEM_IMPI, QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_REQUEST, QtiTelephonyService_RilOemImpiRequestPayload_fields, sizeof(QtiTelephonyService_RilOemImpiRequestPayload), QCRIL_EVT_OEM_MSG_GET_IMPI, "QCRIL_EVT_OEM_MSG_GET_IMPI_REQ"},
    {QtiTelephonyService_RILOEMMessageId_RIL_OEM_IMPI, QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE, QtiTelephonyService_RilOemImpiResponsePayload_fields, sizeof(QtiTelephonyService_RilOemImpiResponsePayload), QCRIL_EVT_NONE, "QCRIL_EVT_OEM_MSG_GET_IMPI_RSP"},
};

size_t qcril_qmi_oem_proto_get_msg_size(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_qmi_oem_proto_msg_meta_data)/sizeof(qcril_qmi_oem_proto_msg_meta_data[0]); i++)
    {
        if ( qcril_qmi_oem_proto_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_oem_proto_msg_meta_data[i].msg_type == msg_type )
        {
            return qcril_qmi_oem_proto_msg_meta_data[i].msg_req_size;
        }
    }
    return -1;
}

const pb_field_t* qcril_qmi_oem_proto_get_msg_pb_fields(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_qmi_oem_proto_msg_meta_data)/sizeof(qcril_qmi_oem_proto_msg_meta_data[0]); i++)
    {
        if ( qcril_qmi_oem_proto_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_oem_proto_msg_meta_data[i].msg_type == msg_type )
        {
            return qcril_qmi_oem_proto_msg_meta_data[i].msg_pb_fields;
        }
    }
    return NULL;
}

const char* qcril_qmi_oem_proto_get_msg_log_str(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_qmi_oem_proto_msg_meta_data)/sizeof(qcril_qmi_oem_proto_msg_meta_data[0]); i++)
    {
        if ( qcril_qmi_oem_proto_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_oem_proto_msg_meta_data[i].msg_type == msg_type )
        {
            if (qcril_qmi_oem_proto_msg_meta_data[i].msg_log_str)
            {
                return qcril_qmi_oem_proto_msg_meta_data[i].msg_log_str;
            }
            else
            {
                break;
            }
        }
    }
    return "Unknown msg";
}

qcril_evt_e_type qcril_qmi_oem_proto_get_msg_event(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_qmi_oem_proto_msg_meta_data)/sizeof(qcril_qmi_oem_proto_msg_meta_data[0]); i++)
    {
        if (qcril_qmi_oem_proto_msg_meta_data[i].msg_id == msg_id)
        {
            return qcril_qmi_oem_proto_msg_meta_data[i].msg_event;
        }
    }
    QCRIL_LOG_ERROR("did not find the corresponding event for %s(msg id %d, type %d)", qcril_qmi_oem_proto_get_msg_log_str(msg_id, msg_type), msg_id, msg_type);
    return QCRIL_EVT_NONE;
}

boolean qcril_qmi_oem_proto_is_msg_valid(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_qmi_oem_proto_msg_meta_data)/sizeof(qcril_qmi_oem_proto_msg_meta_data[0]); i++)
    {
        if (qcril_qmi_oem_proto_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_oem_proto_msg_meta_data[i].msg_type == msg_type)
        {
            return TRUE;
        }
    }
    QCRIL_LOG_ERROR("did not find a valid entry in the meta table for %s(msg id %d, type %d)", qcril_qmi_oem_proto_get_msg_log_str(msg_id, msg_type), msg_id, msg_type);
    return FALSE;
}

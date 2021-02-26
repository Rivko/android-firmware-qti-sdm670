/******************************************************************************
  @file    qcril_qmi_oem_packing.c
  @brief   qcril  - oem msg packing

  DESCRIPTION
    Handles oem hook message packing/unpacking functions.

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <pb_decode.h>
#include <pb_encode.h>
#include "qcril_qmi_oem_packing.h"
#include "qcril_log.h"
#include "qcril_qmi_oem_msg_meta.h"
#include "qcril_qmi_oem_misc.h"
#include "qcril_qmi_npb_encode.h"
#include "qcril_qmi_npb_decode.h"

//===========================================================================
// qcril_qmi_oem_proto_pack_msg_and_tag
//===========================================================================
size_t qcril_qmi_oem_proto_pack_msg_and_tag(const void *msg, uint32_t token,
                                                QtiTelephonyService_RILOEMMessageType type,
                                                QtiTelephonyService_RILOEMMessageId message_id,
                                                bool has_error, QtiTelephonyService_RILOEMError error,
                                                uint8_t *buf, size_t buf_size)
{
    QCRIL_LOG_FUNC_ENTRY();
    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
        return 0;
    }

    size_t packed_size = 0;
    if(qcril_qmi_oem_proto_is_msg_valid(message_id, type))
    {
        QCRIL_LOG_INFO("Message is valid, encoding the message");

        QtiTelephonyService_RILOEMMessage msg_tag;
        memset(&msg_tag, 0x00, sizeof(msg_tag));
        msg_tag.token = token;
        msg_tag.type = type;
        msg_tag.id = message_id;
        msg_tag.has_error = has_error;
        if(has_error)
        {
            msg_tag.error = error;
        }

        pb_ostream_t ostream;
        uint8_t *msg_pack_buf = NULL;
        const pb_field_t* fields = qcril_qmi_oem_proto_get_msg_pb_fields(message_id, type);
        qcril_binary_data_type bin_data;
        if((msg == NULL) && (qcril_qmi_oem_proto_get_msg_size(message_id, type) == 0))
        {
            QCRIL_LOG_INFO("Message payload is NULL, assign NULL to payload");
            msg_tag.payload.arg = NULL;
        }
        else if(fields != NULL)
        {
            QCRIL_LOG_INFO("Message payload exists, encode the payload");
            msg_pack_buf = qcril_malloc(buf_size);
            if (NULL == msg_pack_buf)
            {
                QCRIL_LOG_ERROR("malloc the buffer failed");
                return 0;
            }
            ostream = pb_ostream_from_buffer(msg_pack_buf, buf_size);
            if (qcril_qmi_encode_npb(&ostream, fields, msg))
            {
                bin_data.data = msg_pack_buf;
                bin_data.len = ostream.bytes_written;
                QCRIL_LOG_INFO("bin_data_len: %d", (int)bin_data.len);
                msg_tag.payload.arg = (void*)&bin_data;
            }
        }

        QCRIL_LOG_INFO("encoded the msg, encoding the tag");

        ostream = pb_ostream_from_buffer(buf, buf_size);
        if (qcril_qmi_encode_npb(&ostream, QtiTelephonyService_RILOEMMessage_fields, &msg_tag))
        {
            packed_size = ostream.bytes_written;
            QCRIL_LOG_INFO("encoded the tag");
        }
        else
        {
            QCRIL_LOG_ERROR("pb_encode failed");
        }
        if(msg_pack_buf)
        {
            QCRIL_LOG_INFO("freeing buffer");
            qcril_free(msg_pack_buf);
        }
    }
    else
    {
        QCRIL_LOG_ERROR("Message id and type doesnt co-exist. Cannot encode the msg");
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) packed_size);
    return packed_size;
} // qcril_qmi_oem_proto_pack_msg_and_tag

//===========================================================================
// qcril_qmi_oem_proto_unpack_msg_tag
//===========================================================================
QtiTelephonyService_RILOEMMessage* qcril_qmi_oem_proto_unpack_msg_tag(const uint8_t *buf, size_t buf_size)
{
    QCRIL_LOG_FUNC_ENTRY();

    QtiTelephonyService_RILOEMMessage* msg_tag_ptr = NULL;

    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
    }
    else
    {
        msg_tag_ptr = qcril_malloc(sizeof(*msg_tag_ptr));
        if (msg_tag_ptr)
        {
            pb_istream_t istream;
            istream = pb_istream_from_buffer((uint8_t *)buf, buf_size);
            if(!qcril_qmi_decode_npb(&istream, QtiTelephonyService_RILOEMMessage_fields, msg_tag_ptr))
            {
                QCRIL_LOG_ERROR("pb_decode failed");
                qcril_free(msg_tag_ptr);
                return NULL;
            }
        }
        else
        {
            QCRIL_LOG_ERROR("malloc failed");
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return msg_tag_ptr;
} // qcril_qmi_oem_proto_unpack_msg_tag

//===========================================================================
// qcril_qmi_oem_proto_parse_packed_msg
//===========================================================================
void qcril_qmi_oem_proto_parse_packed_msg(QtiTelephonyService_RILOEMMessageType msg_type,
                                              QtiTelephonyService_RILOEMMessageId msg_id,
                                              const pb_callback_t packed_msg, size_t msg_size,
                                              void** unpacked_msg, size_t*unpacked_msg_size_ptr,
                                              qcril_evt_e_type* event_ptr)
{
    size_t packet_len = 0;
    QCRIL_NOTUSED(msg_size);

    QCRIL_LOG_FUNC_ENTRY();

    *unpacked_msg = NULL;
    *unpacked_msg_size_ptr = 0;
    *event_ptr = qcril_qmi_oem_proto_get_msg_event(msg_id, msg_type);

    if (NULL == packed_msg.arg)
    {
        QCRIL_LOG_ERROR("packed_msg is NULL");
    }
    else
    {
        const pb_field_t* fields = qcril_qmi_oem_proto_get_msg_pb_fields(msg_id, msg_type);
        packet_len = qcril_qmi_oem_proto_get_msg_size(msg_id, msg_type);
        if (fields && packet_len)
        {
            *unpacked_msg = qcril_malloc(packet_len);
            if (*unpacked_msg)
            {
                memset( *unpacked_msg, 0, packet_len );
                pb_istream_t istream;
                qcril_binary_data_type *bin_data_ptr = (qcril_binary_data_type*) packed_msg.arg;
                istream = pb_istream_from_buffer(bin_data_ptr->data, bin_data_ptr->len);
                if(qcril_qmi_decode_npb(&istream, fields, *unpacked_msg))
                {
                    *unpacked_msg_size_ptr = packet_len;
                }
            }
        }
        else
        {
            QCRIL_LOG_INFO("no payload for mesage %s(%d)", qcril_qmi_oem_proto_get_msg_log_str(msg_id, msg_type), msg_id);
        }
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_oem_proto_parse_packed_msg

//===========================================================================
// qcril_qmi_oem_proto_map_oem_error_to_ril_error
//===========================================================================
RIL_Errno qcril_qmi_oem_proto_map_oem_error_to_ril_error(QtiTelephonyService_RILOEMError oem_error)
{
  RIL_Errno ret;

  switch ( oem_error )
  {
    case QtiTelephonyService_RILOEMError_RIL_OEM_ERR_SUCCESS:
      ret = RIL_E_SUCCESS;
      break;

    case QtiTelephonyService_RILOEMError_RIL_OEM_ERR_GENERIC_FAILURE:
      ret = RIL_E_GENERIC_FAILURE;
      break;

    case QtiTelephonyService_RILOEMError_RIL_OEM_ERR_NOT_SUPPORTED:
      ret = RIL_E_REQUEST_NOT_SUPPORTED;
      break;

    default:
      ret = RIL_E_GENERIC_FAILURE;
  }

  QCRIL_LOG_INFO("oem error %d mapped to ril error %d", oem_error, ret);
  return ret;
} // qcril_qmi_oem_proto_map_oem_error_to_ril_error

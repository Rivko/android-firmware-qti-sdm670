/******************************************************************************
  @file    qcril_qmi_ims_packing.c
  @brief   qcril qmi - ims packing

  DESCRIPTION
    Handles ims message packing/unpacking related functions.

  ---------------------------------------------------------------------------

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <pb_decode.h>
#include <pb_encode.h>
#include "qcril_qmi_ims_packing.h"
#include "qcril_log.h"
#include "qcril_qmi_npb_decode.h"
#include "qcril_qmi_npb_encode.h"
#include "qcril_qmi_ims_msg_meta.h"

//===========================================================================
// qcril_qmi_ims_pack_msg_tag
//===========================================================================
size_t qcril_qmi_ims_pack_msg_tag(uint32_t token, ims_MsgType type, ims_MsgId message_id, ims_Error error, uint8_t *buf, size_t buf_size)
{
    QCRIL_LOG_FUNC_ENTRY();
    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
        return 0;
    }

    ims_MsgTag msg_tag;
    memset(&msg_tag, 0x00, sizeof(msg_tag));

    msg_tag.token = token;
    msg_tag.type = type;
    msg_tag.id = message_id;
    msg_tag.error = error;

    pb_ostream_t os;
    os = pb_ostream_from_buffer(&buf[1], buf_size);
    if (!qcril_qmi_encode_npb(&os, ims_MsgTag_fields, &msg_tag))
    {
        QCRIL_LOG_ERROR("pb_encode_failed");
        return 0;
    }
    buf[0] = os.bytes_written;
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) (buf[0]+1));
    return buf[0]+1;
} // qcril_qmi_ims_pack_msg_tag

//===========================================================================
// qcril_qmi_ims_unpack_msg_tag
//===========================================================================
ims_MsgTag* qcril_qmi_ims_unpack_msg_tag(const uint8_t *buf)
{
    QCRIL_LOG_FUNC_ENTRY();
    ims_MsgTag* msg_tag_ptr = NULL;

    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
    }
    else
    {
        msg_tag_ptr = qcril_malloc(sizeof(*msg_tag_ptr));
        if (msg_tag_ptr)
        {
            size_t msg_tag_size = 0;
            msg_tag_size = buf[0];
            pb_istream_t is;
            is = pb_istream_from_buffer((uint8_t *)&buf[1], msg_tag_size);
        if (!qcril_qmi_decode_npb(&is, ims_MsgTag_fields, msg_tag_ptr))
            {
                QCRIL_LOG_ERROR("pb_encode failed");
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
} // qcril_qmi_ims_unpack_msg_tag

//===========================================================================
// qcril_qmi_ims_pack_msg
//===========================================================================
size_t qcril_qmi_ims_pack_msg(const void *msg,
                              ims_MsgType msg_type,
                              ims_MsgId msg_id,
                              uint8_t *buf,
                              size_t max_buf_size)
{
    size_t ret = 0;
    QCRIL_LOG_FUNC_ENTRY();

    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
    }
    else
    {
        QCRIL_LOG_INFO("message id: %d, type: %d", msg_id, msg_type);
        if (NULL == msg)
        {
            QCRIL_LOG_INFO("msg is NULL");
        }
        else
        {
            const pb_field_t* fields = qcril_qmi_ims_get_msg_pb_fields(msg_id, msg_type);
            if (fields)
            {
                pb_ostream_t os;
                os = pb_ostream_from_buffer(buf, max_buf_size);
                if (!qcril_qmi_encode_npb(&os, fields, msg))
                {
                  QCRIL_LOG_ERROR("pb_encode failed");
                  return 0;
                }
                ret = os.bytes_written;
            }
            else
            {
                QCRIL_LOG_INFO("no payload for mesage %s", qcril_qmi_ims_get_msg_log_str(msg_id,
                        msg_type));
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
} // qcril_qmi_ims_pack_msg

//===========================================================================
// qcril_qmi_ims_parse_packed_msg
//===========================================================================
void qcril_qmi_ims_parse_packed_msg(ims_MsgType msg_type,
                                    ims_MsgId msg_id,
                                    const uint8_t *byte_stream,
                                    size_t byte_stream_size,
                                    void** ims_msg_dptr,
                                    size_t* ims_msg_size_ptr,
                                    qcril_evt_e_type* event_ptr)
{
    QCRIL_LOG_FUNC_ENTRY();

    *ims_msg_dptr = NULL;
    *ims_msg_size_ptr = 0;
    *event_ptr = qcril_qmi_ims_get_msg_event(msg_id, msg_type);

    if (NULL == byte_stream)
    {
        QCRIL_LOG_ERROR("packed_msg is NULL");
    }
    else
    {
         const pb_field_t* fields = qcril_qmi_ims_get_msg_pb_fields(msg_id, msg_type);

         if(fields && qcril_qmi_ims_get_msg_size(msg_id, msg_type))
         {

             *ims_msg_dptr = qcril_malloc(qcril_qmi_ims_get_msg_size(msg_id, msg_type));
             if (*ims_msg_dptr)
             {
                 pb_istream_t istream;
                 istream = pb_istream_from_buffer((uint8_t *)byte_stream, byte_stream_size);
                 if (qcril_qmi_decode_npb(&istream, fields, *ims_msg_dptr))
                 {
                     *ims_msg_size_ptr = qcril_qmi_ims_get_msg_size(msg_id, msg_type);
                 }
             }
        }
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_ims_parse_packed_msg

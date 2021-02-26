/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_radio_config_packing.c
  @brief   qcril qmi - radio config nanopb packing helper

  DESCRIPTION
    Helper functions to pack and unpack radio_config messages

******************************************************************************/

#include "qcril_qmi_radio_config_packing.h"
#include "qcril_qmi_radio_config_socket.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include "qcrili.h"



//===========================================================================
// qcril_qmi_radio_config_pack_payload
//===========================================================================
pb_bytes_array_t* qcril_qmi_radio_config_pack_payload
(
   const void* data,
   uint16_t data_len
)
{
    QCRIL_LOG_FUNC_ENTRY();
    pb_bytes_array_t* payload_data = NULL;
    uint16_t payload_len = 0;

    QCRIL_LOG_INFO("copy message payload to pb_bytes_array_t(encoded)");
    payload_len = sizeof(pb_bytes_array_t) + data_len;
    payload_data = (pb_bytes_array_t *) qcril_malloc(payload_len);

    if(payload_data == NULL)
    {
        QCRIL_LOG_ERROR("qcril malloc failed");
        return NULL;
    }

    memset(payload_data, 0, payload_len);
    payload_data->size = (size_t) data_len;
    memcpy(payload_data->bytes, data, data_len);

    QCRIL_LOG_FUNC_RETURN();
    return payload_data;
}// qcril_qmi_radio_config_pack_payload




size_t qcril_qmi_radio_config_pack_msg_and_tag
(
   const void *msg,
   int msg_len,
   uint32_t token,
   com_qualcomm_qti_radioconfiginterface_MessageType type,
   com_qualcomm_qti_radioconfiginterface_MessageId message_id,
   bool has_error,
   com_qualcomm_qti_radioconfiginterface_Error error,
   uint8_t *buf,
   size_t buf_size
)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(msg_len);

  size_t packed_size = 0;
  if(buf == NULL)
  {
    QCRIL_LOG_ERROR("buffer to fill the packed data is null.");
  }
  else
  {
    //need to change when proto expands
    if(message_id == com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG ||
       message_id == com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_GET_CONFIG ||
       message_id == com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_UNSOL_PSK_EXPIRED)
    {
      //pack only valid messages!
      QCRIL_LOG_INFO("Valid message, packing");
      com_qualcomm_qti_radioconfiginterface_MessageTag msg_tag;
      memset(&msg_tag, 0, sizeof(msg_tag));
      
      msg_tag.has_token = true;
      msg_tag.token = token;
      msg_tag.id = message_id;
      msg_tag.type = type;
      msg_tag.has_error = has_error;
      if(has_error)
      {
        msg_tag.error = error;
      }
      msg_tag.payload = NULL;

      QCRIL_LOG_INFO("TAG Details: Token: %d, id: %d, type: %d, has_error: %d", msg_tag.token,
                     msg_tag.id, msg_tag.type, msg_tag.has_error);
      QCRIL_LOG_INFO("packed the tag, packing payload");

      if (msg != NULL)
      {
        QCRIL_LOG_INFO("actually packing payload");
        memset(buf, 0, buf_size);
        pb_ostream_t ostream = pb_ostream_from_buffer(buf, buf_size);
        QCRIL_LOG_INFO("got ostream buffer");

        if(pb_encode(&ostream, com_qualcomm_qti_radioconfiginterface_ConfigMsg_fields, msg))
        {
          QCRIL_LOG_INFO("payload encoded succesfully!");
          msg_tag.payload = qcril_qmi_radio_config_pack_payload(buf, ostream.bytes_written);
        }
        else
        {
          QCRIL_LOG_ERROR("Failed to encode payload! Just pack the tag and send Error response");
        }
      }

      //need ot pack payload here!!
      memset(buf, 0, buf_size);
      pb_ostream_t ostream_tag = pb_ostream_from_buffer(buf, buf_size);
      QCRIL_LOG_INFO("got ostream buffer of tag");
      if(pb_encode(&ostream_tag, com_qualcomm_qti_radioconfiginterface_MessageTag_fields, &msg_tag))
      {
        packed_size = ostream_tag.bytes_written;
        QCRIL_LOG_INFO("Encoded the tag");
      }
      else
      {
        QCRIL_LOG_ERROR("Failed to encode the tag");
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Message id is not supported. Hence cannot encode!");
    }
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET((int)packed_size);
  return packed_size;
}

//===========================================================================
// qcril_qmi_radio_config_unpack_msg_tag
//===========================================================================
com_qualcomm_qti_radioconfiginterface_MessageTag* qcril_qmi_radio_config_unpack_msg_tag
(
	const uint8_t* buf,
	size_t buf_size
)
{
	QCRIL_LOG_FUNC_ENTRY();

	com_qualcomm_qti_radioconfiginterface_MessageTag* msg_tag_ptr = NULL;

	if(buf == NULL)
	{
		QCRIL_LOG_ERROR("buffer is NULL");
	}
	else
	{
		msg_tag_ptr = qcril_malloc(sizeof(*msg_tag_ptr));
		if(msg_tag_ptr)
		{
			pb_istream_t istream;
			istream = pb_istream_from_buffer((uint8_t *)buf, buf_size);
			//to check if we can add this to npbutils
			if(! pb_decode(&istream, com_qualcomm_qti_radioconfiginterface_MessageTag_fields, msg_tag_ptr))
			{
				QCRIL_LOG_ERROR("pb_decode failed!");
				qcril_free(msg_tag_ptr);
				return NULL;
			}
		}
		else
		{
			QCRIL_LOG_ERROR("malloc failed!");
		}
	}
	QCRIL_LOG_FUNC_RETURN();

	return msg_tag_ptr;
}// qcril_qmi_radio_config_unpack_msg_tag

//===========================================================================
// qcril_qmi_radio_config_parse_packed_msg
//===========================================================================
void qcril_qmi_radio_config_parse_packed_msg
(
	com_qualcomm_qti_radioconfiginterface_MessageType msg_type,
	com_qualcomm_qti_radioconfiginterface_MessageId msg_id,
	const pb_bytes_array_t* packed_msg,
	void** unpacked_msg,
	size_t* unpacked_msg_size_ptr,
	qcril_evt_e_type* event_ptr
)
{
    QCRIL_NOTUSED(msg_type);
	QCRIL_LOG_FUNC_ENTRY();

	//safety check
	if(unpacked_msg == NULL || unpacked_msg_size_ptr == NULL || event_ptr == NULL)
	{
		QCRIL_LOG_ERROR("Return fields are sent NULL, either: unpacked_msg/unpacked_msg_size_ptr/event_ptr!");
		return;
	}

	*unpacked_msg = NULL;
	*unpacked_msg_size_ptr = 0;


	if( msg_id == com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_SET_CONFIG)
	{
		*event_ptr = QCRIL_EVT_RADIO_CONFIG_SOCKET_REQ_SET_CONFIG;
	}
	else if( msg_id == com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_GET_CONFIG )
	{
		*event_ptr = QCRIL_EVT_RADIO_CONFIG_SOCKET_REQ_GET_CONFIG;
	}
	else
	{
		*event_ptr = QCRIL_EVT_NONE;
		QCRIL_LOG_ERROR("Message id(%d) is not one of the expected! SET_/GET_ CONFIG!", msg_id);
		//donot process further as the msg fields might be different!!
		return;
	}

	if(packed_msg != NULL)
	{
		*unpacked_msg = qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg));
		if(*unpacked_msg)
		{
			pb_istream_t istream = pb_istream_from_buffer((uint8_t *)packed_msg->bytes, packed_msg->size);

			if( !pb_decode(&istream, com_qualcomm_qti_radioconfiginterface_ConfigMsg_fields, *unpacked_msg) )
			{
				QCRIL_LOG_ERROR("Unable to unpack the payload!");
			}
			else
			{
				*unpacked_msg_size_ptr = sizeof(com_qualcomm_qti_radioconfiginterface_ConfigItemMsg);
			}
		}
		else
		{
			QCRIL_LOG_ERROR("Malloc failed!");
		}
	}
	else
	{
		QCRIL_LOG_ERROR("payload is NULL");
	}

	QCRIL_LOG_FUNC_RETURN();
}// qcril_qmi_radio_config_parse_packed_msg

//===========================================================================
// qcril_qmi_radio_config_unpack_lted_config
//===========================================================================
com_qualcomm_qti_radioconfiginterface_LtedConfig* qcril_qmi_radio_config_unpack_lted_config
(
 const uint8_t* buf,
 size_t buf_size
)
{
  QCRIL_LOG_FUNC_ENTRY();

  com_qualcomm_qti_radioconfiginterface_LtedConfig* msg_ptr = NULL;

  if(buf == NULL)
  {
    QCRIL_LOG_ERROR("buffer is NULL");
  }
  else
  {
    msg_ptr = qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_LtedConfig));
    if(msg_ptr)
    {
      pb_istream_t istream = pb_istream_from_buffer((uint8_t *)buf, buf_size);
      //to check if we can add this to npbutils
      if(!pb_decode(&istream, com_qualcomm_qti_radioconfiginterface_LtedConfig_fields, msg_ptr))
      {
        QCRIL_LOG_ERROR("pb_decode failed!");
        qcril_free(msg_ptr);
        return NULL;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("malloc failed!");
    }
  }
  QCRIL_LOG_FUNC_RETURN();

  return msg_ptr;
}// qcril_qmi_radio_config_unpack_lted_config

//===========================================================================
// qcril_qmi_radio_config_pack_lted_config
//===========================================================================
pb_bytes_array_t* qcril_qmi_radio_config_pack_lted_config
(
 com_qualcomm_qti_radioconfiginterface_LtedConfig* msg
)
{
  QCRIL_LOG_FUNC_ENTRY();

  pb_ostream_t      ostream;
  pb_bytes_array_t *payload_data = NULL;
  int               payload_len  = 0;
  uint8_t           buffer[QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_BUF_SIZE];

  if(msg != NULL)
  {
    QCRIL_LOG_INFO("actually packing payload");
    memset(buffer, 0, sizeof(buffer));
    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    QCRIL_LOG_INFO("got ostream buffer");

    if(pb_encode(&ostream, com_qualcomm_qti_radioconfiginterface_LtedConfig_fields, msg))
    {
      QCRIL_LOG_INFO("payload encoded succesfully!");
      payload_len = sizeof(pb_bytes_array_t) + ostream.bytes_written;
      payload_data = (pb_bytes_array_t *) qcril_malloc(payload_len);
      if (payload_data)
      {
        memset(payload_data, 0, payload_len);
        payload_data->size = (size_t) ostream.bytes_written;
        memcpy(payload_data->bytes, buffer, ostream.bytes_written);
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Failed to encode payload!");
    }
  }

  QCRIL_LOG_FUNC_RETURN();

  return payload_data;
}// qcril_qmi_radio_config_pack_lted_config

//===========================================================================
// qcril_qmi_radio_config_unpack_lted_category
//===========================================================================
com_qualcomm_qti_radioconfiginterface_LtedCategory* qcril_qmi_radio_config_unpack_lted_category
(
 const uint8_t* buf,
 size_t buf_size
)
{
  QCRIL_LOG_FUNC_ENTRY();

  com_qualcomm_qti_radioconfiginterface_LtedCategory* msg_ptr = NULL;

  if(buf == NULL)
  {
    QCRIL_LOG_ERROR("buffer is NULL");
  }
  else
  {
    msg_ptr = qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_LtedCategory));
    if(msg_ptr)
    {
      pb_istream_t istream = pb_istream_from_buffer((uint8_t *)buf, buf_size);
      //to check if we can add this to npbutils
      if(!pb_decode(&istream, com_qualcomm_qti_radioconfiginterface_LtedCategory_fields, msg_ptr))
      {
        QCRIL_LOG_ERROR("pb_decode failed!");
        qcril_free(msg_ptr);
        return NULL;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("malloc failed!");
    }
  }
  QCRIL_LOG_FUNC_RETURN();

  return msg_ptr;
}// qcril_qmi_radio_config_unpack_lted_category

//===========================================================================
// qcril_qmi_radio_config_pack_lted_category
//===========================================================================
pb_bytes_array_t* qcril_qmi_radio_config_pack_lted_category
(
 com_qualcomm_qti_radioconfiginterface_LtedCategory* msg
)
{
  QCRIL_LOG_FUNC_ENTRY();

  pb_ostream_t      ostream;
  pb_bytes_array_t *payload_data = NULL;
  int               payload_len  = 0;
  uint8_t           buffer[QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_BUF_SIZE];

  if(msg != NULL)
  {
    QCRIL_LOG_INFO("actually packing payload");
    memset(buffer, 0, sizeof(buffer));
    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    QCRIL_LOG_INFO("got ostream buffer");

    if(pb_encode(&ostream, com_qualcomm_qti_radioconfiginterface_LtedCategory_fields, msg))
    {
      QCRIL_LOG_INFO("payload encoded succesfully!");
      payload_len = sizeof(pb_bytes_array_t) + ostream.bytes_written;
      payload_data = (pb_bytes_array_t *) qcril_malloc(payload_len);
      if (payload_data)
      {
        memset(payload_data, 0, payload_len);
        payload_data->size = (size_t) ostream.bytes_written;
        memcpy(payload_data->bytes, buffer, ostream.bytes_written);
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Failed to encode payload!");
    }
  }

  QCRIL_LOG_FUNC_RETURN();

  return payload_data;
}// qcril_qmi_radio_config_pack_lted_category

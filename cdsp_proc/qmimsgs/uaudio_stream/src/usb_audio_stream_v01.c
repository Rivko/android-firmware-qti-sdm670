/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        U S B _ A U D I O _ S T R E A M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the uaudio_stream service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //components/rel/qmimsgs.adsp/2.6/uaudio_stream/src/usb_audio_stream_v01.c#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Apr 12 2018 (Spin 0)
   From IDL File: usb_audio_stream_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "usb_audio_stream_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t mem_info_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mem_info_v01, va),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mem_info_v01, pa),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mem_info_v01, size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t apps_mem_info_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_mem_info_v01, evt_ring),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_mem_info_v01, tr_data),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_mem_info_v01, tr_sync),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_mem_info_v01, xfer_buff),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_mem_info_v01, dcba),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t usb_endpoint_descriptor_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bLength),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bDescriptorType),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bEndpointAddress),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bmAttributes),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, wMaxPacketSize),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bInterval),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bRefresh),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_endpoint_descriptor_v01, bSynchAddress),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t usb_interface_descriptor_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bLength),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bDescriptorType),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bInterfaceNumber),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bAlternateSetting),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bNumEndpoints),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bInterfaceClass),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bInterfaceSubClass),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, bInterfaceProtocol),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(usb_interface_descriptor_v01, iInterface),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_uaudio_stream_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, enable),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, usb_token),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, audio_format) - QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, audio_format_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, audio_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, number_of_ch) - QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, number_of_ch_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, number_of_ch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, bit_rate) - QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, bit_rate_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, bit_rate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, xfer_buff_size) - QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, xfer_buff_size_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, xfer_buff_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, service_interval) - QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, service_interval_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_req_msg_v01, service_interval)
};

static const uint8_t qmi_uaudio_stream_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, status) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, internal_status) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, internal_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, internal_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, slot_id) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, slot_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, slot_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_token) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_token_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_token),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_opr_intf_desc) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_opr_intf_desc_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_opr_intf_desc),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_data_ep_desc) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_data_ep_desc_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_data_ep_desc),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_sync_ep_desc) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_sync_ep_desc_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, std_as_sync_ep_desc),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_audio_spec_revision) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_audio_spec_revision_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_audio_spec_revision),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, data_path_delay) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, data_path_delay_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, data_path_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_audio_subslot_size) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_audio_subslot_size_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, usb_audio_subslot_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, xhci_mem_info) - QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, xhci_mem_info_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_resp_msg_v01, xhci_mem_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_uaudio_stream_resp_msg_v01, interrupter_num) - QMI_IDL_OFFSET16RELATIVE(qmi_uaudio_stream_resp_msg_v01, interrupter_num_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_uaudio_stream_resp_msg_v01, interrupter_num),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_uaudio_stream_resp_msg_v01, speed_info) - QMI_IDL_OFFSET16RELATIVE(qmi_uaudio_stream_resp_msg_v01, speed_info_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_uaudio_stream_resp_msg_v01, speed_info),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_uaudio_stream_resp_msg_v01, controller_num) - QMI_IDL_OFFSET16RELATIVE(qmi_uaudio_stream_resp_msg_v01, controller_num_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_uaudio_stream_resp_msg_v01, controller_num)
};

static const uint8_t qmi_uaudio_stream_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, dev_event),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, slot_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_token) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_token_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_token),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_opr_intf_desc) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_opr_intf_desc_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_opr_intf_desc),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_data_ep_desc) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_data_ep_desc_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_data_ep_desc),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_sync_ep_desc) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_sync_ep_desc_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, std_as_sync_ep_desc),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_audio_spec_revision) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_audio_spec_revision_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_audio_spec_revision),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, data_path_delay) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, data_path_delay_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, data_path_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_audio_subslot_size) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_audio_subslot_size_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, usb_audio_subslot_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, xhci_mem_info) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, xhci_mem_info_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, xhci_mem_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, interrupter_num) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, interrupter_num_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, interrupter_num),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, controller_num) - QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, controller_num_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_uaudio_stream_ind_msg_v01, controller_num)
};

/* Type Table */
static const qmi_idl_type_table_entry  uaudio_stream_type_table_v01[] = {
  {sizeof(mem_info_v01), mem_info_data_v01},
  {sizeof(apps_mem_info_v01), apps_mem_info_data_v01},
  {sizeof(usb_endpoint_descriptor_v01), usb_endpoint_descriptor_data_v01},
  {sizeof(usb_interface_descriptor_v01), usb_interface_descriptor_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry uaudio_stream_message_table_v01[] = {
  {sizeof(qmi_uaudio_stream_req_msg_v01), qmi_uaudio_stream_req_msg_data_v01},
  {sizeof(qmi_uaudio_stream_resp_msg_v01), qmi_uaudio_stream_resp_msg_data_v01},
  {sizeof(qmi_uaudio_stream_ind_msg_v01), qmi_uaudio_stream_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object uaudio_stream_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *uaudio_stream_qmi_idl_type_table_object_referenced_tables_v01[] =
{&uaudio_stream_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object uaudio_stream_qmi_idl_type_table_object_v01 = {
  sizeof(uaudio_stream_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(uaudio_stream_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  uaudio_stream_type_table_v01,
  uaudio_stream_message_table_v01,
  uaudio_stream_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry uaudio_stream_service_command_messages_v01[] = {
  {QMI_UAUDIO_STREAM_REQ_V01, QMI_IDL_TYPE16(0, 0), 46}
};

static const qmi_idl_service_message_table_entry uaudio_stream_service_response_messages_v01[] = {
  {QMI_UAUDIO_STREAM_RESP_V01, QMI_IDL_TYPE16(0, 1), 202}
};

static const qmi_idl_service_message_table_entry uaudio_stream_service_indication_messages_v01[] = {
  {QMI_UAUDIO_STREAM_IND_V01, QMI_IDL_TYPE16(0, 2), 181}
};

/*Service Object*/
struct qmi_idl_service_object uaudio_stream_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x41D,
  202,
  { sizeof(uaudio_stream_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(uaudio_stream_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(uaudio_stream_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { uaudio_stream_service_command_messages_v01, uaudio_stream_service_response_messages_v01, uaudio_stream_service_indication_messages_v01},
  &uaudio_stream_qmi_idl_type_table_object_v01,
  0x03,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type uaudio_stream_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( UAUDIO_STREAM_V01_IDL_MAJOR_VERS != idl_maj_version || UAUDIO_STREAM_V01_IDL_MINOR_VERS != idl_min_version
       || UAUDIO_STREAM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&uaudio_stream_qmi_idl_service_object_v01;
}


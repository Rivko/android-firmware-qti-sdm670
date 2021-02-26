/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "DataCommon.h"
#include "wireless_data_service_v01.h"
#include "qmi_client.h"
#include <framework/add_message_id.h>

namespace rildata {

/********************** Request Definitions **********************/
/*typedef struct {
  uint32_t c_struct_sz;
  const uint8_t *p_encoded_type_data;
} qmi_idl_type_table_entry;

typedef struct {
  uint32_t c_struct_sz;
  const uint8_t *p_encoded_tlv_data;
} qmi_idl_message_table_entry;

typedef struct
{
  int8_t (*range_check)(void *val);
}qmi_idl_range_table_entry;


struct qmi_idl_type_table_object {
  uint16_t n_types;
  uint16_t n_messages;
  uint8_t n_referenced_tables;
  const qmi_idl_type_table_entry *p_types;
  const qmi_idl_message_table_entry *p_messages;
  const std::vector<std::vector<qmi_idl_type_table_object>> p_referenced_tables;
  const qmi_idl_range_table_entry *p_ranges;
};

typedef enum {
  QMI_IDL_REQUEST = 0,
  QMI_IDL_RESPONSE,
  QMI_IDL_INDICATION,
  QMI_IDL_NUM_MSG_TYPES
} qmi_idl_type_of_message_type;

typedef struct {
  uint16_t qmi_message_id;
  uint16_t message_table_message_id;
  uint16_t max_msg_len;
} qmi_idl_service_message_table_entry;

typedef struct qmi_idl_service_object {
  uint32_t library_version;
  uint32_t idl_version;
  uint32_t service_id;
  uint32_t max_msg_len;
  uint16_t n_msgs[QMI_IDL_NUM_MSG_TYPES];
  std::vector<qmi_idl_service_message_table_entry> msgid_to_msg;
  const qmi_idl_type_table_object p_type_table;
  uint32_t idl_minor_version;
  struct qmi_idl_service_object* parent_service_obj;
}qmi_idl_service_object_type;

typedef struct qmi_client_struct {
    int service_user_handle;
    qmi_idl_service_object_type p_service;
}qmi_client_type;
*/

//======================================
typedef union
{
  wds_event_report_ind_msg_v01        event_report;
  wds_extended_ip_config_ind_msg_v01  ext_ip_ind;
}WdsIndicationDataType;

typedef struct
{
  qmi_client_type wds_hndl;
  std::shared_ptr<void> user_data;
  int ind_id;
  WdsIndicationDataType ind_data;
  std::shared_ptr<void> self;
} WdsEventData;

/********************** Class Definitions *************************/
class RilEventWDSCallback : public UnSolicitedMessage,public add_message_id<RilEventWDSCallback>{
private:
  WdsEventData data;
protected:
  void deepCopy(WdsEventData const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_WDS_EVENT_CALLBACK,";

  RilEventWDSCallback() = default;
  ~RilEventWDSCallback();

  inline RilEventWDSCallback(WdsEventData const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const WdsEventData& getCallParams();
  string dump();
};
} //namespace

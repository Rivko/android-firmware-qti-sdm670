/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>

namespace rildata {

/********************** Request Definitions **********************/

typedef enum {
  LQE_INVALID_IND_EV,
  LQE_DOWNLINK_THROUGHPUT_IND_EV,
  LQE_UPLINK_THROUGHPUT_IND_EV,
  LQE_MAX_IND_EV
} LqeIndEvents;

typedef struct {
  uint8_t uplink_allowed_rate_valid;
  uint32_t uplink_allowed_rate;

  uint8_t confidence_level_valid;
  uint8_t confidence_level;

  uint8_t is_suspended_valid;
  uint8_t is_suspended;
}lqe_uplink_throughput_info_ind_t;

typedef struct {
  uint8_t downlink_allowed_rate_valid;
  uint32_t downlink_allowed_rate;

  uint8_t confidence_level_valid;
  uint8_t confidence_level;

  uint8_t is_suspended_valid;
  uint8_t is_suspended;
}lqe_downlink_throughput_info_ind_t;

typedef union {
  lqe_downlink_throughput_info_ind_t dl_throughput_info;

  lqe_uplink_throughput_info_ind_t   ul_throughput_info;

} LqeIndEventInfo;

typedef struct
{
  LqeIndEvents      event;
  LqeIndEventInfo  info;
  std::shared_ptr<void>  self;
} LqeEventData;

/********************** Class Definitions *************************/
// FIXME remove template param once framework fixes it...this is a NO-OP
class RilEventLqeCallback : public UnSolicitedMessage,public add_message_id<RilEventLqeCallback> {
private:
  LqeEventData p;
protected:
  void deepCopy(LqeEventData const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_LQE_EVENT_CALLBACK";

  RilEventLqeCallback() = default;
  ~RilEventLqeCallback();

  inline RilEventLqeCallback(LqeEventData const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const LqeEventData& getCallParams();
  string dump();
};
} //namespace

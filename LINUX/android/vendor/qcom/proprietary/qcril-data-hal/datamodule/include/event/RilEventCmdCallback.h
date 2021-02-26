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

typedef enum
{
  QCRIL_DATA_STACK_SWITCH_CMD,
  QCRIL_DATA_MAX_CMD
}CmdId;

/* Modem stack ID */
typedef enum
{
  QCRIL_INVALID_MODEM_STACK_ID = -1,
  QCRIL_DEFAULT_MODEM_STACK_ID = 0,
  QCRIL_MODEM_PRIMARY_STACK_ID = QCRIL_DEFAULT_MODEM_STACK_ID,
  QCRIL_MODEM_SECONDARY_STACK_ID  = 1,
  QCRIL_MODEM_TERTIARY_STACK_ID  = 2,
  QCRIL_MODEM_MAX_STACK_ID
} StackIdType;

typedef struct
{
  CmdId  cmd_id;
  StackIdType old_stack_id;
  StackIdType new_stack_id;
  void              *self;
} CmdData;

/********************** Class Definitions *************************/
// FIXME remove template param once framework fixes it...this is a NO-OP
class RilEventCmdCallback : public UnSolicitedMessage, public add_message_id<RilEventCmdCallback> {

private:
  CmdData p;
protected:
  void deepCopy(CmdData const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_COMMAND_CALLBACK";

  RilEventCmdCallback() = default;
  ~RilEventCmdCallback();

  inline RilEventCmdCallback(CmdData const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const CmdData& getParams();
  string dump();
};

} //namespace

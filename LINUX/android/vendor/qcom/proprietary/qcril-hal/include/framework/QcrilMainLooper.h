/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Looper.h"
#include "framework/QcrilMainMessageQueue.h"

class QcrilMainLooper : public Looper {
 public:
  QcrilMainLooper();
  ~QcrilMainLooper();
  void handleMessage(std::shared_ptr<Message> msg);
  void handleMessageSync(std::shared_ptr<Message> msg);
};

inline QcrilMainLooper::QcrilMainLooper() {
  mMessageQueue = std::unique_ptr<QcrilMainMessageQueue>(new QcrilMainMessageQueue);
}

inline QcrilMainLooper::~QcrilMainLooper() {}

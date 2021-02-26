/******************************************************************************
  @file  FeedbackHandler.cpp
  @brief Feedback module

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "FeedbackHandler.h"

FbcbType FeedbackHandler::mFbcb = NULL;

FeedbackHandler::FeedbackHandler() {
}

FeedbackHandler::FeedbackHandler(FbcbType cb) {
    mFbcb = cb;
}

FeedbackHandler::~FeedbackHandler() {
}

int FeedbackHandler::FbCb(mpctl_msg_t *msg) {
    int ret = -1;
    if (NULL != mFbcb) {
        ret = mFbcb(msg);
    }
    return ret;
}

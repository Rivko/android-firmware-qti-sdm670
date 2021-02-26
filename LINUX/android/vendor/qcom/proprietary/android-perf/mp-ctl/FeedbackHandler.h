/******************************************************************************
  @file  FeedbackHandler.h
  @brief Feedback module

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef _FEED_BACK_HANDLER__H_
#define _FEED_BACK_HANDLER__H_

#include "mp-ctl.h"

typedef int (*FbcbType)(mpctl_msg_t*);

class FeedbackHandler {
private:
    static FbcbType mFbcb;

public:
    FeedbackHandler();
    FeedbackHandler(FbcbType cb);
    ~FeedbackHandler();
    int FbCb(mpctl_msg_t *msg);
};

#endif /*_FEED_BACK_HANDLER__H_*/

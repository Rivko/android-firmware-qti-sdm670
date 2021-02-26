/******************************************************************************
  @file  module1.cpp
  @brief test module to load into hal for hal testing

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "mp-ctl.h"
#include "EventQueue.h"
#include "PerfGlueLayer.h"
#include "ttf.h"
#include <stdio.h>
#include <pthread.h>

#undef QLOGE
#define QLOGE

static pthread_t mod1_thread;
static void *mod1_loop(void *data);

static EventQueue mod1EvQ;
static int mod1Events[] = {
   VENDOR_ACT_TRIGGER_HINT_BEGIN,
   VENDOR_ACT_TRIGGER_HINT_END,
};

static PerfGlueLayer mod1glue = {
   "libqti-tests-mod1.so",
   mod1Events, sizeof(mod1Events)/sizeof(int),
};

static void *Alloccb() {
    void *mem = (void *) new mpctl_msg_t;
    if (NULL ==  mem)
        QLOGE("memory not allocated");
    return mem;
}

static void Dealloccb(void *mem) {
    if (NULL != mem) {
        delete (mpctl_msg_t *)mem;
    }
}

//interface implementation
int perfmodule_init() {
    int rc = 0, ret = SUCCESS;

    mod1EvQ.GetDataPool().SetCBs(Alloccb, Dealloccb);

    rc = pthread_create(&mod1_thread, NULL, mod1_loop, NULL);
    if (rc != 0) {
        ret = FAILED;
    }

    return ret;
}

void perfmodule_exit() {
    pthread_join(mod1_thread, NULL);
}

int perfmodule_submit_request(mpctl_msg_t *msg) {
    EventData *evData;
    int ret = FAILED;

    if (NULL == msg) {
        return ret;
    }

    evData = mod1EvQ.GetDataPool().Get();
    if (NULL == evData) {
        QLOGE("event data pool ran empty");
        return ret;
    }

    mpctl_msg_t *pMsg = (mpctl_msg_t *)evData->mEvData;
    if (pMsg) {
        pMsg->client_pid = msg->client_pid;
        pMsg->client_tid = msg->client_tid;
        pMsg->data = msg->data;
        pMsg->pl_handle = msg->pl_handle;
        pMsg->pl_time = msg->pl_time;
        pMsg->profile = msg->profile;
        pMsg->hint_id = msg->hint_id;
        pMsg->hint_type = msg->hint_type;
        strlcpy(pMsg->usrdata_str, msg->usrdata_str, MAX_MSG_APP_NAME_LEN);
    }
    evData->mEvData = (void *) pMsg;
    mod1EvQ.Wakeup(evData);
    ret = SUCCESS;

    return ret;
}

//support functions
void *mod1_loop(void *) {
    for (;;) {
        //wait for perflock commands
        EventData *evData = mod1EvQ.Wait();

        if (!evData || !evData->mEvData) {
            continue;
        }

        //evData->mEvType;
        // (mpctl_msg_t *)evData->mEvData;
        mod1EvQ.GetDataPool().Return(evData);
    }
    return NULL;
}


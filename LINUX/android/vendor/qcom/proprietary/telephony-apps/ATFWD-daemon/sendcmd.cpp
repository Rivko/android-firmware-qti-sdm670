/*!
  @file
  sendcmd.cpp

  @brief
  Places a Remote Procedure Call (RPC) to Android's AtCmdFwd Service

*/

/*===========================================================================

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0
#define LOG_TAG "Atfwd_sendcmd"

#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>
#include "common_log.h"
#include <vendor/qti/hardware/radio/atcmdfwd/1.0/IAtCmdFwd.h>
#include <vendor/qti/hardware/radio/atcmdfwd/1.0/types.h>
#include "AtCmdFwd.h"

namespace android {

using vendor::qti::hardware::radio::atcmdfwd::V1_0::IAtCmdFwd;

static AtCmdResponse mAtResp;

static void handleResp(const
        ::vendor::qti::hardware::radio::atcmdfwd::V1_0::AtCmdResponse& resp) {
    const char *resp_str = resp.response.c_str();

    LOGI("handlResp AtCmdFwd Result %d", resp.result);
    mAtResp.result = (int) resp.result;
    LOGI("handlResp AtCmdFwd resp string %s", resp_str);

    if (resp.response == "") {
        mAtResp.response = nullptr;
        return;
    }

    int len = resp.response.size() + 1;
    mAtResp.response = (char *)malloc(sizeof(char) * len);
    if (mAtResp.response == NULL) {
        LOGE("malloc error!");
        return;
    }

    strlcpy(mAtResp.response, resp_str, len);

    return;
}

static bool sendAtCmd(const AtCmd *cmd, sp<IAtCmdFwd> atCmdFwd) {
    if (cmd == NULL) {
        LOGE("null cmd\n");
        return false;
    }

    vendor::qti::hardware::radio::atcmdfwd::V1_0::AtCmd atcmd;

    atcmd.opCode = cmd->opcode;

    if (cmd->name == NULL) {
        LOGE("null cmd name\n");
        return false;
    }

    atcmd.name = cmd->name;
    LOGI("AtCmdFwd cmd name %s", cmd->name);
    atcmd.token.num = cmd->ntokens;

    LOGI("AtCmdFwd token num %d", atcmd.token.num);
    for (unsigned int i = 0; i < atcmd.token.num; i++) {
        atcmd.token.items[i] = cmd->tokens[i];
        LOGI("token %s", atcmd.token.items[i].c_str());
    }

    atCmdFwd->processAtCmd(atcmd, handleResp);

    return true;
}

extern "C" AtCmdResponse *sendit(const AtCmd *cmd)
{

    LOGD("hwbinder sendAtCmd");

    if (cmd == nullptr) {
        LOGE("cmd is nullptr!");
        return nullptr;
    }

    sp<IAtCmdFwd> atCmdFwd = IAtCmdFwd::getService("AtCmdFwdService");
    if (atCmdFwd == nullptr) {
        LOGE("AtCmdFwd service not published!");
        return nullptr;
    }

    return (sendAtCmd(cmd, atCmdFwd))? &mAtResp : nullptr;
}

};  /* namespace android */

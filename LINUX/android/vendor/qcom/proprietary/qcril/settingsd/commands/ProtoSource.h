/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef PROTO_SOURCE_H_INCLUDED
#define PROTO_SOURCE_H_INCLUDED

#include "CommandSource.h"
#include "MainLoop.h"
#include "ServiceSocket.h"
#include "Command.h"
#include <string>
#include <memory>
#include <fstream>
#include "vendor/qcom/proprietary/qcril/settingsd/commands/commands.pb.h"

extern "C" {
#include <stdio.h>
}

namespace QcSettingsD {

class ProtoSource: virtual public CommandSource
{
    public:
    ProtoSource(bool service=false);
    ProtoSource(const std::string &tag, bool service=false);
    virtual ~ProtoSource();

    void addEndpoint(std::shared_ptr<EndpointInfo> clientInfo);
    void clearEndPoints();
    void delEndpoint(std::shared_ptr<EndpointInfo> clientInfo);
    void delEndpoint(int fd);
    virtual void addFd(const std::string &tag, int fd);
    virtual void delFd(int fd);

    protected:
    virtual void onReadEvent(int fd);
    virtual void onErrEvent(int fd);

    private:
    std::map<std::string, std::shared_ptr<EndpointInfo>> clients;
    std::map<int, std::shared_ptr<EndpointInfo>> clients_by_fd;
    static ProtoSource &getInstance();
    std::string tag;
    bool service;

    bool notify(int id, CommandResults *results);
    void init();
    void cleanup();
    bool fetch_remote_commands(const std::string *id, const CommandParameters &params, CommandResults &res);
    bool run_remote_commands(const CommandParameters &params, CommandResults &res);
    void handleRequest(const MsgTag &req, MsgTag &rsp);
    void handleResponse(const MsgTag &rsp);
    void handleIndication(const MsgTag &ind);
    std::list<std::shared_ptr<Command>> commands;
    int theFd;
    char buf[1024];
    ActionCmd::action_t remote_action;
};

}

#endif

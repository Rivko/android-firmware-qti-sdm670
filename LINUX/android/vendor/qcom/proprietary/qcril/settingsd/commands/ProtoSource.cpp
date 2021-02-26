/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "commands/EndpointInfo.h"
#include "commands/Command.h"
#include "Logger.h"
#include "commands/ProtoSource.h"
#include "vendor/qcom/proprietary/qcril/settingsd/commands/commands.pb.h"
#include <istream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

extern "C" {
#include <stdio.h>
#include <inttypes.h>
}
#include <google/protobuf/stubs/common.h>

namespace QcSettingsD {

void proto_log(google::protobuf::LogLevel lvl, const char *filename, int line, const std::string &msg)
{
   QCRIL_HAL_LOG_HIGH("[%d] %s(%d): %s", (int)lvl, filename, line, msg.c_str());
}

ProtoSource::ProtoSource(bool service):
        CommandSource(),
        clients(),
        tag("default"),
        service(service),
        theFd(-1)
{
    QCRIL_HAL_LOG_ENTER("tag: %s", tag.c_str());
    init();
    QCRIL_HAL_LOG_LEAVE("");
}
ProtoSource::ProtoSource(const std::string &tag, bool service):
        CommandSource(),
        clients(),
        tag(tag),
        service(service),
        theFd(-1)
{
    QCRIL_HAL_LOG_ENTER("tag: %s", tag.c_str());
    init();
    QCRIL_HAL_LOG_LEAVE("");
}

ProtoSource::~ProtoSource()
{
    QCRIL_HAL_LOG_ENTER("tag: %s", tag.c_str());
    cleanup();
    QCRIL_HAL_LOG_LEAVE("");
}
void ProtoSource::addEndpoint(std::shared_ptr<EndpointInfo> clientInfo)
{
    if (clientInfo)
    {
        QCRIL_HAL_LOG_DEBUG("Adding new endpoint. Name: %s fd: %d", clientInfo->getEndpointName().c_str(), clientInfo->getFd());
        clients[clientInfo->getEndpointName()] = clientInfo;
        clients_by_fd[clientInfo->getFd()] = clientInfo;
        addFd(clientInfo->getEndpointName(), clientInfo->getFd());
    }
}

void ProtoSource::clearEndPoints()
{
    for (std::pair<int,std::shared_ptr<EndpointInfo>>e : clients_by_fd)
    {
        std::shared_ptr<EndpointInfo>clientInfo = e.second;
        if (clientInfo)
        {
            delEndpoint(clientInfo);
        }
    }
}

void ProtoSource::delEndpoint(std::shared_ptr<EndpointInfo> endpoint)
{
    if(endpoint)
    {
        QCRIL_HAL_LOG_DEBUG("deleting Endpoint %s", endpoint->getEndpointName().c_str());
        clients.erase(endpoint->getEndpointName());
        clients_by_fd.erase(endpoint->getFd());
        delFd(endpoint->getFd());
        ::close(endpoint->getFd());
    }
}

void ProtoSource::delEndpoint(int fd)
{
    auto clientInfo = clients_by_fd[fd];
    delEndpoint(clientInfo);
}

ProtoSource &ProtoSource::getInstance()
{
    static ProtoSource instance;

    return instance;
}

void ProtoSource::handleRequest(const MsgTag &req_tag, MsgTag &rsp_tag)
{
    QCRIL_HAL_LOG_ENTER("");
    switch(req_tag.id())
    {
        case MSG_GET_COMMANDS: {
            GetCommandsRsp rsp;
            CommandRegistry::for_each([&rsp](const std::string &name, std::shared_ptr<Command> cmd) -> bool {
                QCRIL_NOTUSED(name);
                QCRIL_HAL_LOG_DEBUG("Appending command %s", cmd->getName().c_str());
                ::Command* protocmd = rsp.add_cmd();
                protocmd->set_cmd_name(cmd->getName());
                protocmd->set_description(cmd->getDescription());
                return true;
            });

            QCRIL_HAL_LOG_DEBUG("Response cmd size: %d", rsp.cmd_size());
            rsp_tag.set_token(req_tag.token());
            rsp_tag.set_type(MSG_TYPE_RESPONSE);
            rsp_tag.set_id(MSG_GET_COMMANDS);
            std::string os;
            rsp.SerializeToString(&os);
            rsp_tag.set_payload(os);
            rsp_tag.PrintDebugString();
            break;
        }
        case MSG_RUN_COMMAND: {
            RunCommand req;
            RunCommand rsp;
            if (req_tag.has_payload())
            {
                if (req.ParseFromString(req_tag.payload()))
                {
                    QCRIL_HAL_LOG_DEBUG("Remote Run command received for command %s", req.cmd_name().c_str());
                    std::shared_ptr<Command> cmd = CommandRegistry::findCommand(req.cmd_name());
                    if (cmd) {
                        CommandParameters pars;
                        CommandResults res;
                        for (int i = 0; i < req.param_size(); i++)
                        {
                            const std::string& parm = req.param(i);
                            pars.addParam(parm);
                        }
                        QCRIL_HAL_LOG_DEBUG("Running command %s", req.cmd_name().c_str());
                        bool rc = cmd->run(pars, res);
                        for (CommandParameter cmdres: res.getParamList())
                        {
                            rsp.add_param(cmdres.getString());
                        }
                        rsp.set_cmd_name(req.cmd_name());
                        rsp_tag.set_token(req_tag.token());
                        rsp_tag.set_type(MSG_TYPE_RESPONSE);
                        rsp_tag.set_id(req_tag.id());
                        rsp_tag.set_error(rc ? ERROR_SUCCESS : ERROR_GENERIC);
                        std::string os;
                        rsp.SerializeToString(&os);
                        rsp_tag.set_payload(os);
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    QCRIL_HAL_LOG_LEAVE("");
}

void ProtoSource::handleResponse(const MsgTag &rsp_tag)
{
     switch(rsp_tag.id())
     {
         case MSG_GET_COMMANDS: {
             GetCommandsRsp rsp;
             if (rsp_tag.has_payload())
             {
                 if (rsp.ParseFromString(rsp_tag.payload()))
                 {
                     QCRIL_HAL_LOG_DEBUG("Received from remote %d commands", rsp.cmd_size());
                     CommandResults res;
                     bool rc = rsp_tag.error() == ERROR_SUCCESS;
                     for (int i = 0; i < rsp.cmd_size(); i++)
                     {
                         QCRIL_HAL_LOG_DEBUG("%d: %s", i, rsp.cmd(i).cmd_name().c_str());
                         res.addParam(rsp.cmd(i).cmd_name());
                         res.addParam(rsp.cmd(i).description());
                         // auto cmd = std::make_shared<ActionCmd>(rsp.cmd(i).cmd_name(),
                         //                              rsp.cmd(i).description(),"",
                         //                              Command::CMD_CAT_EXTERNAL,
                         //                              Command::CMD_EXEC_ASYNC,
                         //                              remote_action);
                         // CommandRegistry::registerCommand("remote", cmd->getName(), cmd);
                     }
                     CommandEvent::process_response(rc, rsp_tag.token(), res);
                 }
             }
             break;
         }
         case MSG_RUN_COMMAND: {
             RunCommand rsp;
             if (rsp_tag.has_payload())
             {
                 QCRIL_HAL_LOG_DEBUG("Received from remote %d results", rsp.param_size());
                 CommandResults res;
                 bool rc = rsp_tag.error() == ERROR_SUCCESS;
                 for (int i = 0; i < rsp.param_size(); i++)
                 {
                     QCRIL_HAL_LOG_DEBUG("%d: %s", i, rsp.param(i).c_str());
                     res.addParam(rsp.param(i).c_str());
                 }
                 CommandEvent::process_response(rc, rsp_tag.token(), res);
             }
             break;
         }
         default:
             break;
     }
}

void ProtoSource::handleIndication(const MsgTag &ind_tag)
{
    CommandResults results;
    bool valid = false;
    switch(ind_tag.id())
    {
        case MSG_PROPERTY_UPDATE_STARTED:
            QCRIL_HAL_LOG_INFO("Property update has started");
            valid = true;
            break;
        case MSG_PROPERTY_UPDATE_COMPLETED: {
            PropertyUpdateCompleted ind;
            QCRIL_HAL_LOG_INFO("Property update has completed");
            if (ind.ParseFromString(ind_tag.payload()))
            {
                for (int i = 0; i < ind.property_name_size(); i++)
                {
                    results.addParam(ind.property_name(i));
                }
                valid = true;
            }
            break;
        }
        default:
            break;
    }
    if (valid)
    {
        CommandEvent::process_indication(ind_tag.id(), MsgId_Name(ind_tag.id()), results);
    }
}

void ProtoSource::onReadEvent(int fd)
{
    int bytes = read(fd, buf, sizeof(buf));
    if (bytes <= 0)
    {
            close(fd);
            delFd(fd);
            cleanup();
    }
    else
    {
        std::string s(buf, bytes);
        std::istringstream is(s);
        MsgTag msgTag;
        bool res;

        QCRIL_HAL_LOG_DEBUG("Read %d bytes from fd %d", bytes, fd);

        while ( (res = msgTag.ParseFromIstream(&is) ) )
        {
            QCRIL_HAL_LOG_DEBUG("Received MsgTag. Token: %s. type: %s. Name: %s. Res: %s",
                       msgTag.token().c_str(),
                       MsgType_Name(msgTag.type()).c_str(),
                       MsgId_Name(msgTag.id()).c_str(),
                       res ? "true" : "false");
            switch(msgTag.type())
            {
                case MSG_TYPE_REQUEST: {
                    MsgTag rsp_tag;
                    ssize_t written = -1;
                    handleRequest(msgTag, rsp_tag);
                    QCRIL_HAL_LOG_DEBUG("After handleRequest");
                    std::string os;
                    rsp_tag.SerializeToString(&os);
                    rsp_tag.PrintDebugString();
                    written = ::write(fd, os.c_str(), os.size());
                    if (written <= 0)
                    {
                        QCRIL_HAL_LOG_DEBUG("Error writing. ret: %d, errno: %d", written, errno);
                    }
                    break;
                }
                case MSG_TYPE_RESPONSE: {
                    handleResponse(msgTag);
                    break;
                }
                case MSG_TYPE_INDICATION: {
                    handleIndication(msgTag);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

bool ProtoSource::fetch_remote_commands(const std::string *id, const CommandParameters &params, CommandResults &results)
{
    bool ret = false;
    ssize_t bytes;
    QCRIL_NOTUSED(params);
    QCRIL_NOTUSED(results);
    QCRIL_HAL_LOG_ENTER("theFd: %d", theFd);

    if (theFd >= 0 )
    {
        MsgTag msgTag;
        msgTag.set_token(*id);
        msgTag.set_type(MSG_TYPE_REQUEST);
        msgTag.set_id(MSG_GET_COMMANDS);

        std::string os;
        msgTag.SerializeToString(&os);
        QCRIL_HAL_LOG_DEBUG("os.size() %d", os.size());
        bytes = write(theFd, os.c_str(), os.size());
        QCRIL_HAL_LOG_DEBUG("bytes written: %d", bytes);
        if (bytes == (ssize_t)os.size())
        {
            ret = true;
        }
    }

    QCRIL_HAL_LOG_LEAVE("ret: %d", ret);
    return ret;
}
bool ProtoSource::notify(int id, CommandResults *results)
{
    bool ret = false;
    MsgTag ind_tag;
    bool valid = false;

    if (MsgId_IsValid(id))
    {
        MsgId theId = (MsgId) id;
        ind_tag.set_token("indication");
        ind_tag.set_type(MSG_TYPE_INDICATION);
        ind_tag.set_id(theId);
        switch (theId)
        {
            case MSG_PROPERTY_UPDATE_STARTED:
                valid = true;
                break;
            case MSG_PROPERTY_UPDATE_COMPLETED:
                if (results)
                {
                    PropertyUpdateCompleted msg;
                    for ( CommandParameter p: results->getParamList())
                    {
                        msg.add_property_name(p.getString());
                    }
                    std::string payload;
                    msg.SerializeToString(&payload);
                    ind_tag.set_payload(payload);
                    valid = true;
                }
                break;
            default:
                break;
        }
        if (valid)
        {
            std::string o;
            if (ind_tag.SerializeToString(&o))
            {
                QCRIL_HAL_LOG_DEBUG("Serialization successful");
                for(const std::unique_ptr<FdEntry> &fde: fds())
                {
                    QCRIL_HAL_LOG_DEBUG("sending %s to %s", MsgId_Name(theId).c_str(), fde->tag.c_str());
                    ssize_t bytes = ::write(fde->fd, o.c_str(), o.size());
                    QCRIL_HAL_LOG_DEBUG("Send bytes: %d", bytes);
                    if (bytes == (ssize_t)o.size())
                    {
                        ret = true;
                    }
                }
            }
            else
            {
                QCRIL_HAL_LOG_DEBUG("Unable to serialize");
            }
        }
    }
    return ret;
}

void ProtoSource::init()
{
    google::protobuf::SetLogHandler(proto_log);
    if (!service)
    {
        commands =
        {
            ActionCmd::make_cmd("help_"+tag, "Show the list of remote commands for " + tag,"",
                Command::CMD_CAT_INTERNAL,
                Command::CMD_EXEC_ASYNC,
                [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                    (void)command;
                    return fetch_remote_commands(id, pars, results);
                }
            ),
        };

        for(std::shared_ptr<Command> cmd: commands)
        {
            CommandRegistry::registerCommand(cmd->getName(), cmd);
        }

        std::shared_ptr<Command> cmd = CommandRegistry::findCommand("help_"+tag);
        if (cmd != nullptr) {
            CommandParameters params;
            cmd->enqueue(params,
                     *new CommandResults(),
                     [this](bool                     rslt,
                        Command::event_type      type,
                        std::shared_ptr<Command> cmd,
                        const CommandParameters &params,
                        CommandResults          &results
                       ) {
                         QCRIL_NOTUSED(params);
                         QCRIL_HAL_LOG_ENTER();
                         if (rslt) {
                             const char *action = type == Command::EVT_TYPE_REQUEST ? "sending" : "executing";
                             QCRIL_HAL_LOG_DEBUG("Success %s command %s", action, cmd->getName().c_str());
                         }
                         switch(type)
                         {
                             case Command::EVT_TYPE_REQUEST:
                                 break;
                             case Command::EVT_TYPE_RESPONSE:
                                 for (unsigned i = 0; i + 1 < results.size() ; i += 2)
                                 {
                                     auto cmd = std::make_shared<ActionCmd>(results[i].getString(),
                                                                  results[i+1].getString(),"",
                                                                  Command::CMD_CAT_EXTERNAL,
                                                                  Command::CMD_EXEC_ASYNC,
                                                                  remote_action);
                                     CommandRegistry::registerCommand("remote", cmd->getName(), cmd);
                                 }
                                 break;
                         }
                         QCRIL_HAL_LOG_LEAVE();
                     });
        }
        remote_action = ActionCmd::action_t([this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &res) -> bool {
                QCRIL_NOTUSED(res);
                bool ret = false;
                if (id && theFd >= 0 && !service && command)
                {
                    ssize_t bytes;
                    MsgTag tag;
                    tag.set_token(*id);
                    tag.set_type(MSG_TYPE_REQUEST);
                    tag.set_id(MSG_RUN_COMMAND);
                    RunCommand cmd;
                    cmd.set_cmd_name(command->getName());
                    for (CommandParameter param: pars.getParamList())
                    {
                        cmd.add_param(param.getString());
                    }
                    std::string payload;
                    cmd.SerializeToString(&payload);
                    tag.set_payload(payload);
                    std::string os;
                    tag.SerializeToString(&os);
                    bytes = ::write(theFd, os.c_str(), os.size());
                    if (bytes == (ssize_t)os.size())
                    {
                        ret = true;
                        QCRIL_HAL_LOG_DEBUG("sent %d bytes", bytes);
                    }

                }
            return ret;
        });
    }
    else
    {
        commands =
        {
            ActionCmd::make_cmd("notify_update_started", "Notify clients that an update of properties has started","",
                Command::CMD_CAT_INTERNAL,
                Command::CMD_EXEC_SYNC,
                [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                    QCRIL_NOTUSED(id);
                    QCRIL_NOTUSED(pars);
                    QCRIL_NOTUSED(results);
                    (void)command;

                    return notify(MSG_PROPERTY_UPDATE_STARTED, NULL);
                }
            ),
            ActionCmd::make_cmd("notify_update_completed", "Notify clients that an update of properties has completed","",
                Command::CMD_CAT_INTERNAL,
                Command::CMD_EXEC_SYNC,
                [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                    QCRIL_NOTUSED(id);
                    QCRIL_NOTUSED(command);
                    QCRIL_NOTUSED(pars);
                    return notify(MSG_PROPERTY_UPDATE_COMPLETED, &results);
                }
            ),
        };

        for(std::shared_ptr<Command> cmd: commands)
        {
            CommandRegistry::registerCommand(cmd->getName(), cmd);
        }
    }
}
void ProtoSource::cleanup()
{
    if (!service)
    {
        for(std::shared_ptr<Command> cmd: commands)
        {
            QCRIL_HAL_LOG_DEBUG("De-registering command %s", cmd->getName().c_str());
            CommandRegistry::deRegisterCommand(cmd->getName());
            cmd.reset();
        }
    }
}
void ProtoSource::onErrEvent(int fd)
{
    QCRIL_HAL_LOG_ENTER("fd: %d. Service: %d", fd, service);
    bool do_cleanup = false;
    if (service)
    {
        std::shared_ptr<EndpointInfo> client = clients_by_fd[fd];
        if (client)
        {
            clients.erase(client->getEndpointName());
            clients_by_fd.erase(fd);
        }
        if (clients.size() < 1)
        {
            do_cleanup = true;
        }
    }
    else
    {
        do_cleanup = true;
    }

    if (do_cleanup) cleanup();
    QCRIL_HAL_LOG_LEAVE("");
}

void ProtoSource::addFd(const std::string &tag, int fd)
{
    if (!service)
    {
        theFd = fd;
    }
    CommandSource::addFd(tag, fd);
}

void ProtoSource::delFd(int fd)
{
    if (!service)
    {
        theFd = -1;
        cleanup();
    }
    CommandSource::delFd(fd);
}


}

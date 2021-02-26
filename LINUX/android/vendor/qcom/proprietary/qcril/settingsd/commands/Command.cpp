/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "Command.h"
#include "Logger.h"

extern "C" {
#include <stdlib.h>
#include <errno.h>
}

namespace QcSettingsD {

thread_local std::map<std::string, std::shared_ptr<CommandEvent>>
        Command::tl_pending_events;

long CommandEvent::seq = 0;

bool CommandEvent::dispatch()
{
    bool result = false;
    if (the_command)
    {
        switch(the_command->execution_type())
        {
            case Command::CMD_EXEC_SYNC:
                result = the_command->run(the_params, the_results);
                QCRIL_HAL_LOG_DEBUG("Invoking callback (type %d) with result=%s", (int) type, result?"success":"failure");
                callback(result, type, the_command, the_params, the_results);
                break;
            case Command::CMD_EXEC_ASYNC:
                QCRIL_HAL_LOG_DEBUG("Executing command asynchronously");
                result = the_command->run_async(shared_from_this());
        }
    }
    return result;
}

bool CommandEvent::process_response(bool result, const std::string &id, CommandResults &results)
{
    bool ret = result;
    std::shared_ptr<CommandEvent> evt = Command::getTlPendingEvents()[id];
    if (evt)
    {
        QCRIL_HAL_LOG_VERBOSE("Found pending request with id %s", id.c_str());
        Command::getTlPendingEvents().erase(id);
        evt->the_results = results;
        evt->callback(ret, Command::EVT_TYPE_RESPONSE, evt->the_command, evt->the_params, evt->the_results);
    }
    else
    {
        QCRIL_HAL_LOG_VERBOSE("Request not found");
        for (std::pair<std::string, std::shared_ptr<CommandEvent>> thepair: Command::getTlPendingEvents())
        {
            QCRIL_HAL_LOG_VERBOSE("Still in queue: %s",thepair.first.c_str());
        }
    }
    return ret;
}

thread_local std::vector<CommandEvent::indication_entry_ptr_t>
                          CommandEvent::tl_indication_callbacks;

void CommandEvent::register_for_indication(CommandEvent::indication_cb_t function)
{
    tl_indication_callbacks.push_back(std::shared_ptr<CommandEvent::indication_entry_t>(new CommandEvent::indication_entry_t(NULL, function)));
}
bool CommandEvent::process_indication(int intid, const std::string &id,const CommandResults &results)
{
    bool ret = false;

    for (auto &func: tl_indication_callbacks)
    {
        if((std::get<0>(*func) != NULL && *std::get<0>(*func) == intid) ||
           (std::get<0>(*func) == NULL))
        std::get<1>(*func)(id, results);
    }
    return ret;
}

bool ActionCmd::run(const CommandParameters &params, CommandResults &results)
{
    bool ret = false;
    if(! ( ret = action(NULL, shared_from_this(), params, results)))
    {
        QCRIL_HAL_LOG_HIGH("Error executing command %s", getName().c_str());
    }
    return ret;
}

bool ActionCmd::do_run_async(std::shared_ptr<CommandEvent> evt)
{
    bool ret = false;
    std::string id(evt->unique_id());
    if (! (ret = action(&id, shared_from_this(), evt->params(), evt->results())))
    {
        QCRIL_HAL_LOG_HIGH("Error executing command %s", getName().c_str());
    }
    return ret;
}
bool Command::run_async(std::shared_ptr<CommandEvent> evt)
{
    bool ret = false;
    QCRIL_HAL_LOG_ENTER("evt: %s", evt ? "true" : "false");
    if (evt)
    {
        const std::string &id = evt->unique_id();
        evt->command()->pending_events[id] = evt;
        evt->command()->do_run_async(evt);
    }
    QCRIL_HAL_LOG_LEAVE("ret: %s", ret ? "true" : "false");
    return ret;
}

bool Command::enqueue(
        const CommandParameters &params,
              CommandResults    &results,
        const callback_t &callback)
{
    bool ret = false;
    QCRIL_HAL_LOG_ENTER("");

    if(theMainLoop)
    {
        QCRIL_HAL_LOG_DEBUG("enqueueing command");
        std::shared_ptr<Event> evt = std::make_shared<CommandEvent>(Command::EVT_TYPE_REQUEST, shared_from_this(), params, results, callback);
        theMainLoop->localQueueEvent(evt);
        ret = true;
    }
    QCRIL_HAL_LOG_LEAVE("ret: %d", ret);
    return ret;
}

long CommandParameter::getLong() const
{
    char *endptr = NULL;
    long ret = strtol(value.c_str(), &endptr, 0);

    if (errno == EINVAL)
    {
        QCRIL_HAL_LOG_DEBUG("Invalid input converting '%s' to long", value.c_str());
    }
    else if (errno == ERANGE)
    {
        QCRIL_HAL_LOG_DEBUG("Out of range converting '%s' to long", value.c_str());
    }
    return ret;
}
bool CommandParameter::getBool() const
{
    bool ret = false;
    std::string tmp = value;

    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

    if (value == "t" || value == "true")
    {
        ret = true;
    }
    else if (value == "f" || value == "false")
    {
        ret = false;
    }
    else
    {
        long l = getLong();
        if (errno == EINVAL)
        {
            QCRIL_HAL_LOG_DEBUG("Invalid input converting '%s' to long", value.c_str());
        }
        else if (errno == ERANGE)
        {
            QCRIL_HAL_LOG_DEBUG("Out of range converting '%s' to long", value.c_str());
        }
        ret = (bool) l;
    }
    return ret;
}

void CommandParameters::addParam(const std::string &param)
{
    paramList.push_back(param);
}

CommandParameters::CommandParameters(std::initializer_list<CommandParameter> init): paramList(init)
{
}
std::map<std::string, std::shared_ptr<CommandRegistry>> &CommandRegistry::registries()
{
    static std::map<std::string, std::shared_ptr<CommandRegistry>> theRegistries;

    return theRegistries;
}
CommandRegistry::CommandRegistry(const std::string &name): name(name)
{
}

std::shared_ptr<CommandRegistry> CommandRegistry::getRegistry()
{
    return getRegistry(DEFAULT_REGISTRY);
}

std::shared_ptr<CommandRegistry> CommandRegistry::getRegistry(const std::string &name)
{
    QCRIL_HAL_LOG_ENTER("%s", name.c_str());
    if (!registries()[name])
    {
        registries()[name] = std::make_shared<CommandRegistry>(name);
    }
    QCRIL_HAL_LOG_LEAVE();
    return registries()[name];
}

CommandRegistry::CommandRegistry(): name(DEFAULT_REGISTRY)
{
}

std::shared_ptr<Command> CommandRegistry::findCommand(std::string name)
{
    return getRegistry()->commands[name];
}

std::shared_ptr<Command> CommandRegistry::findCommand(std::string registry, std::string name)
{
    return getRegistry(registry)->commands[name];
}

void CommandRegistry::doRegisterCommand(const std::string &name,const std::shared_ptr<Command> &command)
{
    QCRIL_HAL_LOG_ENTER("(%s) %s", this->name.c_str(), name.c_str());
    commands[name] = command;
    QCRIL_HAL_LOG_LEAVE();
}

void CommandRegistry::doDeRegisterCommand(const std::string &name)
{
    QCRIL_HAL_LOG_ENTER("%s(%s)", this->name.c_str(), name.c_str());
    commands.erase(name);
    QCRIL_HAL_LOG_LEAVE();
}

void CommandRegistry::deRegisterCommand(const std::string &name)
{
    QCRIL_HAL_LOG_ENTER("%s", name.c_str());
    getRegistry()->doDeRegisterCommand(name);
    QCRIL_HAL_LOG_LEAVE();
}

void CommandRegistry::deRegisterCommand(const std::string &registry, const std::string &name)
{
    QCRIL_HAL_LOG_ENTER("%s -= %s", registry.c_str(), name.c_str());
    getRegistry(registry)->doDeRegisterCommand(name);
    QCRIL_HAL_LOG_LEAVE();
}

void CommandRegistry::registerCommand(const std::string &registry, const std::string &name,const std::shared_ptr<Command> &command)
{
    QCRIL_HAL_LOG_ENTER("%s <- %s", registry.c_str(), name.c_str());
    getRegistry(registry)->doRegisterCommand(name, command);
    QCRIL_HAL_LOG_LEAVE();
}

void CommandRegistry::registerCommand(const std::string &name,const std::shared_ptr<Command> &command)
{
    QCRIL_HAL_LOG_ENTER("%s", name.c_str());
    getRegistry()->doRegisterCommand(name, command);
    QCRIL_HAL_LOG_LEAVE();
}

void CommandRegistry::for_each(const std::function<bool(const std::string &, std::shared_ptr<Command>)> &func)
{
    for_each(DEFAULT_REGISTRY, func);
}

void CommandRegistry::for_each(const std::string &registry, const std::function<bool(const std::string &, std::shared_ptr<Command>)> &func)
{
    for(std::pair<std::string, std::shared_ptr<Command>> pair: getRegistry(registry)->commands)
    {
        func(pair.first, pair.second);
    }
}

}

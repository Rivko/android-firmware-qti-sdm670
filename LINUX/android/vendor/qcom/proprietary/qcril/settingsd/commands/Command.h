/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <map>
#include <vector>
#include <string>
#include <deque>
#include <mutex>
#include <sstream>
#include <utility>
#include "Logger.h"
#include "MainLoop.h"

#define DEFAULT_REGISTRY "default"
#define REMOTE_REGISTRY  "remote"
namespace QcSettingsD
{

class CommandParameter
{
    public:
    const std::string &getString() const
    {
        return value;
    }

    long getLong() const;
    bool getBool() const;
    void setValue(const std::string &value)
    {
        this->value = value;
    }

    CommandParameter(){}
    CommandParameter(const std::string &str)
    {
        this->value = str;
    }
    CommandParameter(const CommandParameter &parm)
    {
        this->value = parm.value;
    }
    CommandParameter(CommandParameter &&parm)
    {
        this->value = std::move(parm.value);
    }
    CommandParameter &operator = (const CommandParameter &parm)
    {
        this->value = parm.value;
        return *this;
    }
    CommandParameter &operator = (const CommandParameter &&parm)
    {
        this->value = std::move(parm.value);
        return *this;
    }
    virtual ~CommandParameter(){}

    private:
    std::string value;
};

class CommandParameters
{
    public:
    void addParam(const std::string &param);
    const std::vector<CommandParameter> &getParamList() const
    {
        return paramList;
    }

    typedef std::vector<CommandParameter>::size_type size_type;

    const CommandParameter &operator[](size_type pos) const
    {
        return paramList[pos];
    }
    size_type size() const
    {
        return paramList.size();
    }

    CommandParameters() {}
    CommandParameters(size_type sz): paramList(sz)
    {}
    CommandParameters(std::initializer_list<CommandParameter> init);
    virtual ~CommandParameters(){}

    using iterator = std::vector<CommandParameter>::iterator;
    using const_iterator = std::vector<CommandParameter>::const_iterator;
    const_iterator begin() const { return paramList.begin(); }
    const_iterator end() const { return paramList.end(); }
    iterator begin() { return paramList.begin(); }
    iterator end() { return paramList.end(); }
    private:
    std::vector<CommandParameter> paramList;
};

class CommandResults: virtual public CommandParameters
{
};

class CommandRegistry;
class CommandEvent;

class Command: public std::enable_shared_from_this<Command>
{
    public:

    enum event_type {
        EVT_TYPE_REQUEST,
        EVT_TYPE_RESPONSE,
    };
    using callback_t =
            std::function<void(      bool,
                                     event_type,
                                     std::shared_ptr<Command> &,
                               const CommandParameters &,
                                     CommandResults &)>;
    enum CommandCategory
    {
        CMD_CAT_EXTERNAL,
        CMD_CAT_INTERNAL,
    };
    enum ExecutionType
    {
        CMD_EXEC_SYNC,
        CMD_EXEC_ASYNC,
    };
    Command(const std::string &name,
            const std::string &description,
            const std::string &long_description,
            CommandCategory    category,
            ExecutionType      exec_type
            ...):
            pending_events(tl_pending_events),
            name(name),
            description(description),
            long_description(long_description),
            category(category),
            theMainLoop(MainLoop::getThreadLoop()),
            the_exec_type(exec_type)
    {
        Logger::log(LVL_DEBUG, name.c_str(), "Command %s", name.c_str());
    }

    Command(std::initializer_list<std::string> init):
            pending_events(tl_pending_events),
            theMainLoop(MainLoop::getThreadLoop())
    {
        auto it = init.begin();

        if (it == init.end()) return;

        name = *it;

        ++it;
        if (it == init.end()) return;
        description = *it;

        ++it;
        if (it == init.end()) return;
        long_description = *it;
    };
    virtual ~Command(){ Logger::log(LVL_DEBUG, "Command", "~Command(%s)", name.c_str());}

    virtual bool run(
                    const CommandParameters &params,
                          CommandResults    &results) = 0;
            bool enqueue(
                    const CommandParameters &params,
                          CommandResults    &results,
                    const callback_t        &callback);

    static std::map<std::string, std::shared_ptr<CommandEvent>>
                                  &getTlPendingEvents()       { return tl_pending_events; }
    const std::string                        &getName() const { return name; }
    const std::string                 &getDescription() const { return description; }
    const std::string             &getLongDescription() const { return long_description; }
          CommandCategory                 getCategory() const { return category; }
    const std::string                      &getSource() const { return source; }
          void        setSource(const std::string &src)       { source = src; }
          ExecutionType                execution_type()       { return the_exec_type; }

    static bool run_async(std::shared_ptr<CommandEvent> evt);

    protected:
    virtual bool do_run_async(std::shared_ptr<CommandEvent> evt) = 0;

    private:
    std::map<std::string, std::shared_ptr<CommandEvent>>
                             &pending_events;
    std::string               name;
    std::string               description;
    std::string               long_description;
    CommandCategory           category;
    std::shared_ptr<MainLoop> theMainLoop;
    std::string               source;
    ExecutionType             the_exec_type;
    static thread_local std::map<std::string, std::shared_ptr<CommandEvent>>
                              tl_pending_events;

};

class CommandEvent: virtual public Event, public std::enable_shared_from_this<CommandEvent>
{
    public:
    bool dispatch();
    CommandEvent(      Command::event_type      type,
                       std::shared_ptr<Command> the_command,
                 const CommandParameters        &the_params,
                       CommandResults           &the_results,
                 const Command::callback_t      &the_callback):
                         callback(the_callback),
                         type(type),
                         the_command(the_command),
                         the_params(the_params),
                         the_results(the_results),
                         the_unique_id((std::ostringstream() << the_command->getName() << (++seq)).str())//,
                         //indication_callbacks(tl_indication_callbacks)
    {
        Logger::log(LVL_DEBUG, "CommandEvent", "");
    }

    Command::event_type       event_type() { return type; }
    std::shared_ptr<Command>    &command() { return the_command; }
    const CommandParameters      &params() { return the_params; }
    CommandResults              &results() { return the_results; }
    const std::string         &unique_id() { return the_unique_id; }

    Command::callback_t       callback;
    // static void add_to_pending_events(
    //                       std::shared_ptr<CommandEvent> evt);
    static bool process_response(
                          bool result,
                          const std::string &id,
                          CommandResults    &results);

    static bool process_indication(
                          int intid,
                          const std::string &id,
                          const CommandResults    &results);

    using indication_cb_t = std::function<void(const std::string&, const CommandResults &)>;
    using indication_entry_t = std::tuple<int *, indication_cb_t &>;
    using indication_entry_ptr_t = std::shared_ptr<indication_entry_t>;
    static void register_for_indication(indication_cb_t function);
    //static void register_for_indication(int id, const std::function<void(int id, CommandResults &)> &function);

    private:
    Command::event_type       type;
    std::shared_ptr<Command>  the_command;
    CommandParameters         the_params;
    CommandResults            the_results;

    std::string               the_unique_id;
    // std::vector<indication_entry_t>
    //                          &indication_callbacks;

    static thread_local std::vector<indication_entry_ptr_t>
                              tl_indication_callbacks;

    static long               seq;
};

class ActionCmd: virtual public Command
{
    public:
    using action_t = std::function<bool(const std::string *, std::shared_ptr<Command>, const CommandParameters &, CommandResults &)>;
    ActionCmd(
        std::string              name,
        std::string              description,
        std::string              long_description,
        Command::CommandCategory category,
        Command::ExecutionType   exec_type,
        const action_t &action):
                Command(name, description, long_description, category, exec_type), action(action)
    {}

    ~ActionCmd() {Logger::log(LVL_DEBUG, "ActionCmd", "~ActionCmd(%s)", getName().c_str());}
    virtual bool run(const CommandParameters &params, CommandResults &results);

    static std::shared_ptr<Command> make_cmd(
            const std::string        name,
            const std::string        description,
            const std::string        long_description,
            Command::CommandCategory category,
            Command::ExecutionType   exec_type,
            const action_t &action)
    {
        return std::make_shared<ActionCmd>(name, description, long_description, category, exec_type, action);
    }
    protected:
    virtual bool do_run_async(std::shared_ptr<CommandEvent> evt);
    private:
    action_t action;
};

class CommandRegistry
{
    friend class Command;
    public:
    static std::shared_ptr<CommandRegistry> getRegistry();
    static std::shared_ptr<CommandRegistry> getRegistry(const std::string &name);
    static void registerCommand(const std::string &name, const std::shared_ptr<Command> &command);
    static void registerCommand(const std::string &registr, const std::string &name, const std::shared_ptr<Command> &command);
    static void deRegisterCommand(const std::string &name);
    static void deRegisterCommand(const std::string &registry, const std::string &name);

    static std::shared_ptr<Command> findCommand(std::string name);
    static std::shared_ptr<Command> findCommand(std::string registry, std::string name);
    static void for_each(const std::function<bool(const std::string &, std::shared_ptr<Command>)> &action);
    static void for_each(const std::string &registry, const std::function<bool(const std::string &, std::shared_ptr<Command>)> &action);
    CommandRegistry();
    CommandRegistry(const std::string &name);
    private:
    std::map<std::string, std::shared_ptr<Command>> commands;
    void doRegisterCommand(const std::string &name,const std::shared_ptr<Command> &command);
    void doDeRegisterCommand(const std::string &name);
    static std::map<std::string, std::shared_ptr<CommandRegistry>> &registries();
    std::string name;

};

}

#endif

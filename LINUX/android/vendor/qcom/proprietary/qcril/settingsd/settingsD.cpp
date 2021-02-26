/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

extern "C" {
#include "properties.h"
};

#include "Logger.h"
#include "settingsD.h"
#include "commands/Command.h"
#include <memory>
#include <functional>

namespace QcSettingsD {

void settingsD::load_prop(const string filename, const string carrier, unordered_set<string> &changed, unsigned instance_id) {
    ifstream myfile(filename, ifstream::in);
    bool res = false;
    if (!myfile)
    {
        QCRIL_HAL_LOG_FATAL("Could not open file %s", filename.c_str());
    }
    else
    {
        QCRIL_HAL_LOG_DEBUG("Opened file '%s' for carrier '%s'", filename.c_str(), carrier.c_str());
        while (!myfile.eof())
        {
            string line;
            getline(myfile, line);
            string key, value;
            if (parse_line(line, key, value))
            {
                process_key(key, instance_id);
                res = property_set(key.c_str(), value.c_str());
                if (res == 0)
                {
                    string default_carrier_name = DEFAULT_PROP;
                    changed.insert(key);
                    if (carrier == default_carrier_name)
                    {
                        property_map[key] = value;
                    }
                    if (carrier != "" && carrier != default_carrier_name)
                    {
                        carrier_key_map[instance_id][key] = value;
                    }
                }
                else
                {
                    QCRIL_HAL_LOG_FATAL("Error: cannot set property %s to %s", key.c_str(), value.c_str());
                }
            }
        }
        myfile.close();
    }
}

void settingsD::load_delta_prop(const string filename, unordered_set<string> &changed, unsigned instance_id)
{
    ifstream myfile(filename, ifstream::in);
    bool res = false;
    if (!myfile)
    {
        QCRIL_HAL_LOG_FATAL("Could not open file %s", filename.c_str());
    }
    else
    {
        QCRIL_HAL_LOG_DEBUG("Opened file '%s'", filename.c_str());
        while (!myfile.eof())
        {
            string line;
            getline(myfile, line);
            string key, value;
            if (parse_line(line, key, value))
            {
                process_key(key, instance_id);
                res = property_set(key.c_str(), value.c_str());
                if (res == 0)
                {
                    carrier_delta_key_map[instance_id][key] = value;
                    changed.insert(key);
                }
                else
                {
                    QCRIL_HAL_LOG_FATAL("Error: cannot set property %s to %s", key.c_str(), value.c_str());
                }

            }
        }
        myfile.close();
    }
}

void settingsD::clear_carrier_prop(unordered_set<string> &changed, unsigned instance_id)
{
    bool res = false;
    for (auto kvpair: carrier_key_map[instance_id])
    {
        string key = kvpair.first;
        // if find key in default map, set to default value
        if (property_map.count(key) > 0)
        {
            res = property_set(key.c_str(), property_map[key].c_str());
        }
        // otherwise, set to empty string
        else
        {
            res = property_set(key.c_str(), "");
        }
        if (res == 0)
        {
            changed.insert(key);
        }
        else
        {
            QCRIL_HAL_LOG_FATAL("Error: cannot reset property %s", key.c_str());
        }
    }
    carrier_key_map[instance_id].clear();
}

void settingsD::clear_delta_prop(unordered_set<string> &changed, unsigned instance_id)
{
    bool res = false;
    for (auto kvpair: carrier_delta_key_map[instance_id])
    {
        string key = kvpair.first;
        changed.insert(key);
        // if find key in carrier_key_map, reset to default value for current carrier
        if (carrier_key_map[instance_id].count(key) > 0)
        {
            res = property_set(key.c_str(), carrier_key_map[instance_id][key].c_str());
        }
        // else if find key in default map, reset to default value
        else if (property_map.count(key) > 0)
        {
            res = property_set(key.c_str(), property_map[key].c_str());
        }
        // otherwise, set to empty string
        else
        {
            res = property_set(key.c_str(), "");
        }
        if (!res) {
            QCRIL_HAL_LOG_FATAL("Error: cannot reset property %s", key.c_str());
        }
    }
    carrier_delta_key_map[instance_id].clear();
}

bool settingsD::parse_line(const string &line, string &key, string &value) {
    if (line.length() == 0 || line[0] == '#') return false;
    auto i = line.find('=');
    if (i != string::npos) {
        key = line.substr(0,i);
        value = line.substr(i+1);
        return true;
    }
    else {
        return false;
    }
}

void settingsD::process_key(string &key, unsigned instance_id) {
    size_t pos = key.find("%(instance)");
    if (pos != string::npos)
    {
        QCRIL_HAL_LOG_DEBUG("key_before_processing %s", key.c_str());
        key.replace(pos, 11, to_string(instance_id));
        QCRIL_HAL_LOG_DEBUG("key_after_processing %s", key.c_str());
    }
}

void settingsD::load_mcc_mnc_map() {
    std::shared_ptr<Command> get_file_path_under_root = CommandRegistry::findCommand("get_file_path_under_root");
    if (get_file_path_under_root) {
        CommandResults results;
        string file_name = MCC_MNC_MAP_FILE;
        get_file_path_under_root->run({file_name}, results);
        if (results.size() >= 1) {
            if (!results[0].getString().empty()) {
                string file_path = results[0].getString();
                QCRIL_HAL_LOG_DEBUG("file_path %s", file_name.c_str());
                ifstream myfile(file_path, ifstream::in);
                QCRIL_HAL_LOG_DEBUG("Opened file '%s' for mcc_mnc_map", MCC_MNC_MAP_FILE);
                while (!myfile.eof()) {
                    string line;
                    getline(myfile, line);
                    string mcc_mnc, carrier;
                    if (parse_line(line, mcc_mnc, carrier)) {
                        QCRIL_HAL_LOG_DEBUG("mcc_mnc %s, carrier %s", mcc_mnc.c_str(), carrier.c_str());
                        mcc_mnc_map[mcc_mnc] = carrier;
                    }
                }
                myfile.close();
            }
            else {
                QCRIL_HAL_LOG_DEBUG("Cannot find mcc_mnc map with name %s", MCC_MNC_MAP_FILE);
            }
        }
        else {
            QCRIL_HAL_LOG_DEBUG("Cannot find mcc_mnc map with name %s", MCC_MNC_MAP_FILE);
        }
    }
    else {
        QCRIL_HAL_LOG_HIGH("Internal error executing command %s", get_file_path_under_root->getName().c_str());
    }
}

void settingsD::load_iccid_map() {
    std::shared_ptr<Command> get_file_path_under_root = CommandRegistry::findCommand("get_file_path_under_root");
    if (get_file_path_under_root) {
        CommandResults results;
        string file_name = ICCID_MAP_FILE;
        get_file_path_under_root->run({file_name}, results);
        if (results.size() >= 1) {
            if (!results[0].getString().empty()) {
                string file_path = results[0].getString();
                QCRIL_HAL_LOG_DEBUG("file_path %s", file_name.c_str());
                ifstream myfile(file_path, ifstream::in);
                QCRIL_HAL_LOG_DEBUG("Opened file '%s' for iccid_map", ICCID_MAP_FILE);
                while (!myfile.eof())
                {
                    string line;
                    getline(myfile, line);
                    string iccid, carrier;
                    if (parse_line(line, iccid, carrier))
                    {
                        QCRIL_HAL_LOG_DEBUG("iccid %s, carrier %s", iccid.c_str(), carrier.c_str());
                        iccid_map[iccid] = carrier;
                    }
                }
                myfile.close();
            }
            else {
                QCRIL_HAL_LOG_DEBUG("Cannot find iccid map with name %s", MCC_MNC_MAP_FILE);
            }
        }
        else {
            QCRIL_HAL_LOG_DEBUG("Cannot find iccid map with name %s", MCC_MNC_MAP_FILE);
        }
    }
    else {
        QCRIL_HAL_LOG_HIGH("Internal error executing command %s", get_file_path_under_root->getName().c_str());
    }
}

settingsD::settingsD() {
    m_carrier_name = vector<string>(MAX_INSTANCE_ID, "");
    m_mcc_mnc_pair = vector<string>(MAX_INSTANCE_ID, "");
    m_carrier_file_name = vector<string>(MAX_INSTANCE_ID, "");
    m_carrier_delta_file_name = vector<string>(MAX_INSTANCE_ID, "");
    //load_default_prop(DEFAULT_PROP);
    init(".");
}

settingsD::~settingsD() {
}

void settingsD::init(const std::string &root)
{
    std::shared_ptr<Command> commands[] =
    {
        ActionCmd::make_cmd("load_prop", "Load properties for a specific carrier",
            "load_prop <path> <carrier-name>\n",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                QCRIL_NOTUSED(id);
                bool ret = false;
                (void)results;
                (void)command;

                if (pars.size() >= 2)
                {
                    const std::string &filename = pars[0].getString();
                    const std::string &carrier = pars[1].getString();
                    const unsigned instance_id = std::stoi(pars[2].getString());
                    QCRIL_HAL_LOG_DEBUG("Calling load_prop with filename %s and carrier %s",
                              filename.c_str(),
                              carrier.c_str());

                    unordered_set<string> changed;
                    notify_start();
                    load_prop(filename, carrier, changed, instance_id);
                    notify_changed(changed);
                    ret = true;
                }
                return ret;
            }
        ),

        ActionCmd::make_cmd("update_carrier", "Set a new carrier",
            "update_carrier <instance_id> <mcc> <mnc> <iccid>\n",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                QCRIL_NOTUSED(id);
                bool ret = false;
                (void)results;
                (void)command;

                if (pars.size() >= 4)
                {
                    unsigned instance_id     = (unsigned)pars[0].getLong();
                    const std::string &mcc   = pars[1].getString();
                    const std::string &mnc   = pars[2].getString();
                    const std::string &iccid = pars[3].getString();
                    QCRIL_HAL_LOG_DEBUG("Calling load_carrier_prop iid %ld mcc %s mnc %s iccid %s",
                              instance_id,
                              mcc.c_str(),
                              mnc.c_str(),
                              iccid.c_str());

                    load_carrier_prop(instance_id, mcc, mnc, iccid);
                    ret = true;
                }
                return ret;
            }
        ),
    };

    for(std::shared_ptr<Command> cmd: commands)
    {
        CommandRegistry::registerCommand(cmd->getName(), cmd);
    }

    std::shared_ptr<Command> set_root_cmd = CommandRegistry::findCommand("set_root");
    if (set_root_cmd)
    {
        CommandResults results;
        set_root_cmd->run({root}, results);
    }

    load_mcc_mnc_map();
    load_iccid_map();

    std::shared_ptr<Command> set_default_name_cmd = CommandRegistry::findCommand("set_default_name");
    if (set_default_name_cmd)
    {
        CommandResults results;
        std::string deflt = DEFAULT_PROP;
        set_default_name_cmd->run({deflt}, results);
    }
    load_carrier_prop(0, "000", "000", "");
}

settingsD::settingsD(const string root) {
    m_carrier_name = vector<string>(MAX_INSTANCE_ID, "");
    m_mcc_mnc_pair = vector<string>(MAX_INSTANCE_ID, "");
    m_carrier_file_name = vector<string>(MAX_INSTANCE_ID, "");
    m_carrier_delta_file_name = vector<string>(MAX_INSTANCE_ID, "");
    init(root);
}

void settingsD::notify_start()
{
    std::shared_ptr<Command> update_started_command = CommandRegistry::findCommand("notify_update_started");
    QCRIL_HAL_LOG_ENTER();
    if (update_started_command)
    {
        QCRIL_HAL_LOG_DEBUG("Sending update started indication");
        CommandParameters params;
        CommandResults results;
        update_started_command->run(params, results);
    }
    QCRIL_HAL_LOG_LEAVE();
}

void settingsD::notify_changed(const unordered_set<string> &changed)
{
    std::shared_ptr<Command> update_completed_command = CommandRegistry::findCommand("notify_update_completed");

    QCRIL_HAL_LOG_ENTER();
    if (update_completed_command)
    {
        QCRIL_HAL_LOG_DEBUG("Sending completed indication");
        CommandParameters params;
        CommandResults results;
        for (const string &s: changed)
        {
            results.addParam(s);
        }
        update_completed_command->run(params, results);
    }
    QCRIL_HAL_LOG_LEAVE();
}
void settingsD::load_default_prop(const string filename){
    property_map.clear();
    unordered_set<string> changed;
    notify_start();
    load_prop(filename, "default", changed, 0);
    notify_changed(changed);
}

void settingsD::load_carrier_prop(unsigned instance_id, const string &mcc, const string &mnc, const string &iccid)
{
    static std::shared_ptr<Command> find_file_command = CommandRegistry::findCommand("find_file");
    string carrier_name = "", mcc_mnc_pair = "";
    string carrier_file_name = "", carrier_delta_file_name = "";
    unordered_set<string> changed_props;

    mcc_mnc_pair = mcc + "-" + mnc;
    if (mcc_mnc_map.count(mcc_mnc_pair) != 0)
    {
        carrier_name = mcc_mnc_map[mcc_mnc_pair];
    }
    else if (iccid_map.count(iccid) != 0)
    {
        carrier_name = iccid_map[iccid];
    }

    if (carrier_name.size() == 0)
    {
        QCRIL_HAL_LOG_INFO("carrier not found for mcc: %s, mnc: %s and iccid: %s\n", mcc.c_str(), mnc.c_str(), iccid.c_str());
        carrier_name = DEFAULT_PROP;
    }

    if (find_file_command)
    {
        CommandResults results;
        bool res = find_file_command->run({carrier_name, mcc, mnc},results);
        int n = results.size();
        if (n >= 1 && res)
        {
            if (!results[0].getString().empty())
            {
                carrier_file_name = results[0].getString();
            }
            if (n >= 2 && !results[1].getString().empty())
            {
                carrier_file_name = results[1].getString();
            }
            if (n == 3 && !results[1].getString().empty())
            {
                carrier_delta_file_name = results[2].getString();
            }
        }
        else
        {
            QCRIL_HAL_LOG_HIGH("Internal error executing command %s", find_file_command->getName().c_str());
        }
    }
    else
    {
        QCRIL_HAL_LOG_INFO("Command find_file not available");
    }

    QCRIL_HAL_LOG_INFO("New carrier: %s", carrier_name.c_str());

    notify_start();

    // same carrier
    if (m_carrier_file_name[instance_id] == carrier_file_name)
    {
        // same delta
        if (m_carrier_delta_file_name[instance_id] == carrier_delta_file_name)
        {
            QCRIL_HAL_LOG_DEBUG("No carrier switch, no need to load");
        }
        // different delta
        else
        {
            QCRIL_HAL_LOG_DEBUG("Same carrier but mcc/mnc pair switched from %s to %s", m_mcc_mnc_pair[instance_id].c_str(), mcc_mnc_pair.c_str());
            // only reset delta
            clear_delta_prop(changed_props, instance_id);
            load_delta_prop(carrier_delta_file_name, changed_props, instance_id);
            m_mcc_mnc_pair[instance_id] = mcc_mnc_pair;
            m_carrier_delta_file_name[instance_id] = carrier_delta_file_name;
        }
    }
    // different carrier
    else
    {
        QCRIL_HAL_LOG_DEBUG("Different carrier");
        if (m_carrier_name[instance_id].size() == 0)
        {
            QCRIL_HAL_LOG_DEBUG("old carrier is empty");
        }
        else {
            QCRIL_HAL_LOG_INFO("carrier switch from %s to %s", m_carrier_name[instance_id].c_str(), carrier_name.c_str());
            // reset old carrier property to default value
            clear_delta_prop(changed_props, instance_id);
            clear_carrier_prop(changed_props, instance_id);
        }

        // load prop for new carrier
        if (carrier_file_name.size() > 0)
        {
            load_prop(carrier_file_name, carrier_name, changed_props, instance_id);
        }
        if (carrier_delta_file_name.size() > 0)
        {
            load_delta_prop(carrier_delta_file_name, changed_props, instance_id);
        }
        m_carrier_name[instance_id] = carrier_name;
        m_mcc_mnc_pair[instance_id] = mcc_mnc_pair;
        m_carrier_file_name[instance_id] = carrier_file_name;
        m_carrier_delta_file_name[instance_id] = carrier_delta_file_name;
    }
    notify_changed(changed_props);
}

settingsD &settingsD::getInstance()
{
    static settingsD theInstance;
    return theInstance;
}

}

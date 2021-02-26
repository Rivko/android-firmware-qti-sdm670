/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "Logger.h"
#include "file_mapper.h"
#include <string>
#include <sstream>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
}

#define NOT_USED(x) (void)(x)
namespace QcSettingsD {
static file_mapper mapper;

file_mapper::file_mapper(): root(".")
{
    init();
};

#define MAX_ERROR_BUF 150

bool file_mapper::is_dir(const std::string &path, std::string &error)
{
    struct stat buf;
    bool ret = false;
    int rc = 0;

    error.clear();
    rc = stat(path.c_str(), &buf);
    if (!rc)
    {
        ret = S_ISDIR(buf.st_mode);
    }
    else
    {
        std::ostringstream o(error);
        char err_buf[MAX_ERROR_BUF];
        strerror_r(errno, err_buf, sizeof(err_buf));
        o << "Unable to stat " << path << ": " << err_buf;
    }

    return ret;
}

bool file_mapper::is_dir_readable(const std::string &path, std::string &error)
{
    bool ret = false;

    ret = !access(path.c_str(), F_OK | R_OK);

    error.clear();
    if (!ret) {
        std::ostringstream o(error);
        char err_buf[MAX_ERROR_BUF];
        strerror_r(errno, err_buf, sizeof(err_buf));
        o << "Unable to read " << path << ": " << err_buf;
    }
    return ret;
}

bool file_mapper::set_search_root(const CommandParameters &pars, CommandResults &results)
{
    bool ret = false;

    if (pars.size() >= 1)
    {
        std::string path = pars[0].getString();
        std::string error;
        if (!is_dir_readable(path, error))
        {
            results.addParam(error);
        }
        else if (!is_dir(path, error))
        {
            results.addParam(error);
        }
        else
        {
            QCRIL_HAL_LOG_INFO("Setting search root to %s", path.c_str());
            root = path;
            ret = true;
        }
    }
    else
    {
        results.addParam("Invalid Number of arguments");
    }
    return ret;
}

bool file_mapper::set_default_name(const CommandParameters &pars, CommandResults &results)
{
    bool ret = false;

   if (pars.size() >= 1)
    {
        std::string name = pars[0].getString();
        std::string error;
        if (name.empty())
        {
            std::ostringstream err;
            err << "Empty default given. Not changing current (\"" << default_name << "\")";
            results.addParam(err.str());
        }
        else
        {
            QCRIL_HAL_LOG_INFO("Setting default name to %s", name.c_str());
            default_name = name;
            ret = true;
        }
    }
    else
    {
        results.addParam("Invalid Number of arguments");
    }
    return ret;
}

bool file_mapper::is_file(const std::string &path, std::string &error)
{
    struct stat buf;
    int rc = 0;
    bool ret = false;

    rc = stat(path.c_str(), &buf);
    error.clear();
    if (!rc)
    {
        ret = S_ISREG(buf.st_mode);
    }
    else
    {
        std::ostringstream o(error);
        char err_buf[MAX_ERROR_BUF];
        strerror_r(errno, err_buf, sizeof(err_buf));
        o << "Unable to stat " << path << ": " << err_buf;
    }

    return ret;
}

bool file_mapper::is_file_readable(const std::string &path, std::string &error)
{
    bool ret = false;

    ret = !access(path.c_str(), F_OK | R_OK);

    error.clear();
    if (!ret) {
        std::ostringstream o(error);
        char err_buf[MAX_ERROR_BUF];
        strerror_r(errno, err_buf, sizeof(err_buf));
        o << "Unable to read " << path << ": " << err_buf;
    }

    return ret;
}
std::string file_mapper::get_default_path()
{
    std::ostringstream default_path;
    default_path << root << "/" << default_name << ".prop";

    return default_path.str();
}
std::string file_mapper::get_carrier_path(const std::string &carrier)
{
    std::ostringstream carrier_path;
    carrier_path << root << "/" << carrier << ".prop";

    return carrier_path.str();
}
std::string file_mapper::get_mcc_mnc_path(
        const std::string &carrier,
        const std::string &mcc,
        const std::string &mnc)
{
    std::ostringstream mcc_mnc_path;
    mcc_mnc_path << root << "/" << carrier << "_" << mcc << "_" << mnc << ".prop";

    return mcc_mnc_path.str();
}
bool file_mapper::find_file(const CommandParameters &pars, CommandResults &results)
{
    bool ret = false;

    if (pars.size() >= 3)
    {
        std::string carrier = pars[0].getString();
        std::string mcc     = pars[1].getString();
        std::string mnc     = pars[2].getString();

        std::string error;
        std::string path;


        // Try the most specific file first
        if ( !(path = get_default_path()).empty() &&
                is_file_readable(path, error) && is_file(path, error))
        {
            results.addParam(path);
            ret = true;
        }
        if ( !(path = get_carrier_path(carrier)).empty() &&
                is_file_readable(path, error) && is_file(path, error))
        {
            results.addParam(path);
            ret = true;
        }
        if ( !(path = get_mcc_mnc_path(carrier, mcc, mnc)).empty() &&
                is_file_readable(path, error) && is_file(path, error))
        {
            results.addParam(path);
            ret = true;
        }
        if ( results.size() == 0 )
        {
            std::ostringstream err;
            path.clear();
            results.addParam(path);
            err << "Unable to find appropriate file for carrier '" <<
                    carrier << "' mcc '" << mcc << "' mnc '" << mnc << "'";
            results.addParam(err.str());
        }
    }

    return ret;
}

bool file_mapper::get_file_path_under_root(const CommandParameters &pars, CommandResults &results)
{
    bool ret = false;
    (void) pars;
    (void) results;

    if (pars.size() >= 1)
    {
        std::string res = root;
        std::string file_name = pars[0].getString();
        res += "/"+file_name;
        results.addParam(res);
        QCRIL_HAL_LOG_DEBUG("file %s path %s: ", file_name.c_str(), res.c_str());
        ret = true;
    }
    else {
        std::ostringstream err;
        err << "Empty file_name given.";
        results.addParam(err.str());
    }
    return ret;
}

void file_mapper::init()
{
    std::shared_ptr<Command> commands[] =
    {
        ActionCmd::make_cmd("set_root", "Set the root folder to search files in",
            "set_root <path> - Use <path> as the root"
            "",
            Command::CMD_CAT_INTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                bool ret = false;

                NOT_USED(id);
                NOT_USED(command);
                ret = set_search_root(pars, results);

                return ret;
            }
        ),

        ActionCmd::make_cmd("find_file", "Find properties file for a specific carrier",
            "find_file <carrier_name> <mcc> <mnc>\n"
            "",
            Command::CMD_CAT_INTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                bool ret = false;

                NOT_USED(id);
                NOT_USED(command);
                ret = find_file(pars, results);

                return ret;
            }
        ),

        ActionCmd::make_cmd("set_default_name", "Set the default property file name",
            "set_default_name <basename> - Provide the base name of the file to use (without the .prop suffix)"
            "",
            Command::CMD_CAT_INTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                bool ret = false;

                NOT_USED(id);
                NOT_USED(command);
                ret = set_default_name(pars, results);

                return ret;
            }
        ),

        ActionCmd::make_cmd("get_file_path_under_root", "Get path of a file under root",
            "get_file_path_under_root <file_name> - get a full path of a file, starting from the settings root (c.f. set_root command)\n"
            "",
            Command::CMD_CAT_INTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {

                bool ret = false;
                NOT_USED(id);
                NOT_USED(command);

                ret = get_file_path_under_root(pars, results);

                return ret;
            }
        )
    };

    for(std::shared_ptr<Command> cmd: commands)
    {
        CommandRegistry::registerCommand(cmd->getName(), cmd);
    }
}
}

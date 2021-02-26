/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __FILE_MAPPER_H_INCLUDED
#define __FILE_MAPPER_H_INCLUDED

#include "commands/Command.h"

namespace QcSettingsD
{

class file_mapper
{
    public:
    file_mapper();
    bool set_search_root(const CommandParameters &params, CommandResults &results);
    bool set_default_name(const CommandParameters &params, CommandResults &results);
    bool find_file(const CommandParameters &params, CommandResults &results);
    bool get_file_path_under_root(const CommandParameters &params, CommandResults &results);

    private:
    void init();
    std::string root;
    std::string default_name;
    bool is_dir(const std::string &path, std::string &error);
    bool is_dir_readable(const std::string &path, std::string &error);
    bool is_file(const std::string &path, std::string &error);
    bool is_file_readable(const std::string &path, std::string &error);
    std::string get_default_path();
    std::string get_carrier_path(const std::string &carrier);
    std::string get_mcc_mnc_path(
            const std::string &carrier,
            const std::string &mcc,
            const std::string &mnc);
};

}

#endif

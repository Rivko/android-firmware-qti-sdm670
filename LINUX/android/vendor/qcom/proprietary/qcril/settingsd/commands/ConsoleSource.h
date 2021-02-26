/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CONSOLE_SOURCE_H_INCLUDED
#define CONSOLE_SOURCE_H_INCLUDED

#include "commands/Command.h"
#include "CommandSource.h"

namespace QcSettingsD {
#define CONSOLE_MAX_CMD_NAME_SIZE 50
class ConsoleSource: virtual public CommandSource
{
    public:
    ConsoleSource();
    ConsoleSource(const std::string &tag);
    bool isCommandAllowed(const Command &cmd);

    protected:
    virtual void onReadEvent(int fd);
    virtual void onErrEvent(int fd);
    void setAllowedCategories(uint32_t cats);

    private:
    uint32_t allowed_categories;
    void init();
    std::string tag;
};

}
#endif

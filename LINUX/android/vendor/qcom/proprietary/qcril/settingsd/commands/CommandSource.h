/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef COMMAND_SOURCE_H_INCLUDED
#define COMMAND_SOURCE_H_INCLUDED

#include "MainLoop.h"
#include <list>
#include <string>

struct FdEntry
{
    FdEntry(const std::string &tag, int fd): tag(tag), fd(fd)
    {
    }

    std::string tag;
    int fd;
};


namespace QcSettingsD {
class CommandSource: virtual public EventSource
{
    public:
    CommandSource();
    virtual int updateFdSet(fd_set &readfds, fd_set &writefds, fd_set &exceptfds);
    virtual int checkEvents(fd_set &readfds, fd_set &writefds, fd_set &exceptfds);
    protected:
    virtual void onReadEvent(int fd) = 0;
    virtual void onErrEvent(int fd) = 0;
    virtual void addFd(const std::string &tag, int fd);
    virtual void delFd(int fd);
    virtual const std::list<std::unique_ptr<FdEntry>> &fds() const { return fd_list; }
    private:
    std::list<std::unique_ptr<FdEntry>>fd_list;
};

}
#endif

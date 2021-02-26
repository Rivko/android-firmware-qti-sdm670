/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <algorithm>
#include <iostream>
#include "CommandSource.h"
#include "Logger.h"

using namespace QcSettingsD;

CommandSource::CommandSource()
{
}

void CommandSource::addFd(const std::string &tag, int fd)
{
    QCRIL_HAL_LOG_DEBUG("tag %s. fd %d", tag.c_str(), fd);
    fd_list.emplace_back(new FdEntry(tag, fd));
}

void CommandSource::delFd(int fd)
{
    (void)fd;

    fd_list.remove_if([fd](const std::unique_ptr<FdEntry> &entry) -> bool {
        return fd == entry->fd;
    });
    // for ( auto it = fd_list.begin();
    //            it != fd_list.end(); it ++ )
    // {
    //     if (fd == (*it)->fd) {
    //         fd_list.erase(it);
    //     }
    // }
}

int CommandSource::updateFdSet(fd_set &readfds, fd_set &writefds, fd_set &exceptfds)
{
    int max_fd = -1;

    QCRIL_HAL_LOG_ENTER("");
    (void) writefds;
    for ( auto &entry: fd_list )
    {
    //for( auto it = fd_list.begin();
    //       it != fd_list.end(); it++ )
    //{
    //  auto &entry = *it;
        QCRIL_HAL_LOG_DEBUG("Adding fd %d (%s)", entry->fd, entry->tag.c_str());
        FD_SET(entry->fd, &readfds);
        FD_SET(entry->fd, &exceptfds);
        if ( entry->fd > max_fd)
        {
            max_fd = entry->fd;
        }
    }
    QCRIL_HAL_LOG_LEAVE("");
    return max_fd;
}

int CommandSource::checkEvents(fd_set &readfds, fd_set &writefds, fd_set &exceptfds)
{
    QCRIL_HAL_LOG_ENTER("");
    for ( auto it = fd_list.begin();
               it != fd_list.end(); it ++ )
    {
        auto &entry = *it;
        if (FD_ISSET(entry->fd, &exceptfds))
        {
            QCRIL_HAL_LOG_VERBOSE("except ready on fd %s(%d)", entry->tag.c_str(), entry->fd);
            onErrEvent(entry->fd);
            return 0;
        }
        if (FD_ISSET(entry->fd, &writefds))
        {
            QCRIL_HAL_LOG_VERBOSE("write ready on fd %s(%d)", entry->tag.c_str(), entry->fd);
        }
        if (FD_ISSET(entry->fd, &readfds))
        {
            QCRIL_HAL_LOG_VERBOSE("read ready on fd %s(%d)", entry->tag.c_str(), entry->fd);
            onReadEvent(entry->fd);
        }
    }
    QCRIL_HAL_LOG_LEAVE("");
    return 0;
}

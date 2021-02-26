/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef ENDPOINT_INFO_H_INCLUDED
#define ENDPOINT_INFO_H_INCLUDED

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/sockets.h>
}

#include "CommandSource.h"
#include "MainLoop.h"
#include <string>
#include <sstream>

class EndpointInfo
{
    public:
    EndpointInfo(const std::string &endpoint_name, int fd, sockaddr_storage *sa, socklen_t sl):
            endpoint_name(endpoint_name),
            fd(fd),
            sa(*sa),
            sl(sl)
    {
    }

    const std::string     &getEndpointName() { return endpoint_name; }
    int                              getFd() { return fd; }
    const sockaddr_storage        &getAddr() { return sa; }
    socklen_t                   getAddrLen() { return sl; }
    private:
    std::string endpoint_name;
    int fd;
    sockaddr_storage sa;
    socklen_t sl;
};

#endif

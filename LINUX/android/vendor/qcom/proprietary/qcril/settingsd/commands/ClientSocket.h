/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CLIENT_SOCKET_H_INCLUDED
#define CLIENT_SOCKET_H_INCLUDED

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cutils/sockets.h>
}

#include "EndpointInfo.h"
#include "CommandSource.h"
#include "MainLoop.h"
#include <string>
#include <sstream>

namespace QcSettingsD {

template <class T>
class ClientSocket: virtual public T
{
    public:

    static std::unique_ptr<ClientSocket<T>> getUnix(const std::string &addr)
    {
        return getUnix(addr, addr);
    }
    static std::unique_ptr<ClientSocket<T>>getUnix(const std::string &addr, const std::string &tag)
    {
        ClientSocket<T> *ret = NULL;
        ret = new ClientSocket<T>(tag, addr, AF_UNIX, SOCK_STREAM, 0);

        return std::unique_ptr<ClientSocket<T>>(ret);
    }

    ClientSocket(const std::string &tag, const std::string &addr, int domain, int type, int protocol):
            T(tag),
            addr(addr),
            tag(tag),
            theFd(socket(domain, type, protocol)),
            domain(domain), type(type), protocol(protocol)
    {
        fcntl(theFd, F_SETFL, O_NONBLOCK);
        Logger::log(LVL_DEBUG, tag.c_str(),"%s ( %s, %s, %d, %d, %d)",
                __func__,
                tag.c_str(),
                addr.c_str(),
                domain,
                type,
                protocol);
    }

    int connect()
    {
        sockaddr_un sa;
        int ret;
        strlcpy(sa.sun_path, addr.c_str(), sizeof(sa.sun_path));
        sa.sun_family = AF_UNIX;
        ret = ::connect(theFd, (struct sockaddr *)(&sa), sizeof(sa));
        Logger::log(LVL_DEBUG, tag.c_str(), "%s: ret: %d. fd: %d. errno: %d",
                __func__,
                ret,
                theFd,
                errno);
        if (ret < 0)
        {
            char err_msg[200] = {0};
            strerror_r(errno, err_msg, sizeof(err_msg));
            Logger::log(LVL_HIGH, tag.c_str(), "%s: connect: %s(%d)", __func__, err_msg, errno);
        }
        else
        {
            Logger::log(LVL_HIGH, tag.c_str(), "%s: adding fd %d", __func__, theFd);
            T::addFd("ClientSocket_" + tag, theFd);
        }
        return ret;
    }

    virtual ~ClientSocket()
    {
        Logger::log(LVL_DEBUG, tag.c_str(), "Closing fd %d", theFd);
        ::close(theFd);
        if (!localaddr.empty())
        {
            ::unlink(localaddr.c_str());
        }
    }

    int getFd()
    {
        return theFd;
    }
    protected:

    private:
    std::string addr;
    std::string tag;
    int theFd;
    int domain;
    int type;
    int protocol;
    std::string localaddr;
};

}
#endif

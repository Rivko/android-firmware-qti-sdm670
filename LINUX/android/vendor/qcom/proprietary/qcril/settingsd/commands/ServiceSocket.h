/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef SERVICE_SOCKET_H_INCLUDED
#define SERVICE_SOCKET_H_INCLUDED

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/sockets.h>
}

#include "Logger.h"
#include "EndpointInfo.h"
#include "CommandSource.h"
#include "MainLoop.h"
#include <string>
#include <sstream>
#include <memory>

namespace QcSettingsD {

template <class T>
class ServiceSocket: virtual public CommandSource
{
    public:

    static std::unique_ptr<ServiceSocket<T>> getAndroid(const std::string socket_name)
    {
        ServiceSocket<T> *ret = NULL;
        ret = new ServiceSocket<T>(socket_name);

        return std::unique_ptr<ServiceSocket<T>>(ret);
    }
    static std::unique_ptr<ServiceSocket<T>> getUnix(const std::string &path)
    {
        return getUnix(path, path);
    }

    static std::unique_ptr<ServiceSocket<T>>getUnix(const std::string &path, const std::string &tag)
    {
        ServiceSocket<T> *ret = NULL;

        ret = new ServiceSocket<T>(tag, AF_UNIX, SOCK_STREAM, 0);

        ret->bindUnix(path);
        return std::unique_ptr<ServiceSocket<T>>(ret);
    }

    int find_android_fd(const std::string &name)
    {
        int fd = -1;

        fd = android_get_control_socket(name.c_str());
        return fd;
    }
    ServiceSocket(const std::string &name):
            CommandSource(),
            client_src(new T(name, true)),
            tag(name),
            theFd(find_android_fd(name))
    {
        //addFd("ServiceSocket_" + name, theFd);
        std::unique_ptr<EventSource> es = std::unique_ptr<EventSource>(std::move(client_src));
        MainLoop::addEventSource(es);
    }
    ServiceSocket(const std::string &tag, int domain, int type, int protocol):
            CommandSource(),
            client_src(new T(tag, true)),
            tag(tag),
            theFd(socket(domain, type, protocol)),
            domain(domain), type(type), protocol(protocol)
    {
        fcntl(theFd, F_SETFL, O_NONBLOCK);
        //addFd("ServiceSocket_" + tag, theFd);
        std::unique_ptr<EventSource> es = std::unique_ptr<EventSource>(std::move(client_src));
        MainLoop::addEventSource(es);
    }

    virtual ~ServiceSocket()
    {
        Logger::log(LVL_DEBUG, tag.c_str(), "Closing fd %d", theFd);
        ::close(theFd);
        if (!path.empty())
        {
            ::unlink(path.c_str());
        }
    }

    int bindUnix(const std::string &addr)
    {
        struct sockaddr_un sa;
        path = addr;
        memcpy(sa.sun_path, addr.c_str(), sizeof(sa.sun_path));
        sa.sun_family = AF_UNIX;
        ::unlink(addr.c_str());

        return bind((sockaddr *)&sa, sizeof(sa));
    }
    int bind(const struct sockaddr *addr, socklen_t addrlen)
    {
        return ::bind(theFd, addr, addrlen);
    }

    int listen(int backlog)
    {
        int rc = ::listen(theFd, backlog);
        if ( rc >= 0 )
        {
            addFd("ServiceSocket_"+tag, theFd);
        }
        return rc;
    }

    protected:
    virtual void onReadEvent(int fd) {
        struct sockaddr_storage sa;
        socklen_t sl = sizeof(sa);
        Logger::log(LVL_DEBUG, tag.c_str(), "accepting at fd %d", fd);
        int newFd = accept(fd, (sockaddr *)&sa, &sl);
        Logger::log(LVL_DEBUG, tag.c_str(), "accepted. New Fd: %d.", fd);
        if (newFd >= 0)
        {
            std::ostringstream client_name;
            client_name << "ServiceSocket_" << tag << "_" << newFd;
            auto clientInfo = std::make_shared<EndpointInfo>(client_name.str(), newFd, &sa, sl);
            client_src->addEndpoint(clientInfo);
        }
        else if(errno != EAGAIN       && errno != ENETDOWN    &&
                errno != EPROTO       && errno != ENOPROTOOPT &&
                errno != EHOSTDOWN    && errno != ENONET      &&
                errno != EHOSTUNREACH && errno != EOPNOTSUPP  &&
                errno != ENETUNREACH)
        {
        }
    }

    virtual void delFd(int fd)
    {
        client_src->delEndpoint(fd);
        CommandSource::delFd(fd);
    }
    virtual void onErrEvent(int fd)
    {
        (void)fd;
    }

    private:
    T *client_src;
    std::string tag;
    std::string path;
    int theFd;
    int domain;
    int type;
    int protocol;
};

}

#endif

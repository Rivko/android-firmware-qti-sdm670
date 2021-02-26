/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "Logger.h"
#include "MainLoop.h"
#include <memory>
#include <utility>

extern "C" {
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
}

using namespace QcSettingsD;
/*
 * Event Dispatcher
 */
void EventDispatcher::dispatchEvent(std::shared_ptr<Event> e)
{
    (void)e;
}


/*
 * MainLoop
 */

#define SP_SENDER 0
#define SP_RECEIVER 1

std::shared_ptr<MainLoop> MainLoop::getThreadLoop()
{
    static thread_local std::shared_ptr<MainLoop> threadLoop = std::make_shared<MainLoop>();
    QCRIL_HAL_LOG_DEBUG("current main loop set: %s", threadLoop ? "true" : "false");
    return threadLoop;
}
MainLoop::MainLoop(): goOn(true)
{
    int rc;
    QCRIL_HAL_LOG_DEBUG("Main Loop created");
    rc = socketpair(AF_UNIX, SOCK_STREAM, 0, queue_link);
    if (rc)
    {
        QCRIL_HAL_LOG_HIGH("Unable to create socketpair");
    }
}

MainLoop::~MainLoop()
{
    sources.clear();
    ::close(queue_link[0]);
    ::close(queue_link[1]);
}
void MainLoop::startLoop()
{
    QCRIL_HAL_LOG_ENTER("");
    std::shared_ptr<MainLoop> loop = getThreadLoop();
    if (loop)
    {
        loop->goOn = true;
        QCRIL_HAL_LOG_DEBUG("calling getThreadLoop().loop()");
        loop->loop();
    }
    QCRIL_HAL_LOG_LEAVE("");
}

void MainLoop::addEventSource( std::unique_ptr<EventSource> &src )
{
    std::shared_ptr<MainLoop> loop = getThreadLoop();
    if (loop)
    {
        loop->doAddEventSource( src );
    }
}

bool MainLoop::queueEvent( std::shared_ptr<Event> &evt)
{
    bool ret = false;
    std::shared_ptr<MainLoop> loop = getThreadLoop();
    if (loop)
    {
        loop->doQueueEvent( std::move(evt) );
        ret = true;
    }
    return ret;
}

void MainLoop::wakeup_thread()
{
    unsigned b = 'w';
    ssize_t written = -1;
    written = write(queue_link[SP_SENDER], &b, 1);

    if (written <= 0)
    {
        QCRIL_HAL_LOG_HIGH("Unexpected return value from write: %d. errno: %d", written, errno);
    }
}

void MainLoop::localQueueEvent( std::shared_ptr<Event> &evt )
{
    QCRIL_HAL_LOG_ENTER();
    std::lock_guard<std::mutex> lock(queue_lock);
    queue.push_back(std::move(evt));
    wakeup_thread();
    QCRIL_HAL_LOG_LEAVE();
}

void MainLoop::endLoop()
{
    std::shared_ptr<MainLoop> loop = getThreadLoop();
    if (loop)
    {
        loop->doEndLoop();
    }
}

void MainLoop::doAddEventSource( std::unique_ptr<EventSource> &src )
{
    sources.push_back(std::move(src));
}

void MainLoop::doQueueEvent( std::shared_ptr<Event> evt)
{
    (void) evt;
}

void MainLoop::doEndLoop()
{
    goOn = false;
}

void MainLoop::loop()
{
    fd_set readfds, writefds, exceptfds;
    int maxfd = queue_link[SP_RECEIVER], tmpfd = -1;
    int rc = 0;
    while(goOn)
    {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        if (queue_link[SP_RECEIVER] != -1)
        {
            FD_SET(queue_link[SP_RECEIVER], &readfds);
            FD_SET(queue_link[SP_RECEIVER], &exceptfds);
        }
        for ( std::unique_ptr<EventSource> &source : sources )
        {
            tmpfd = source->updateFdSet(readfds, writefds, exceptfds);
            if ( tmpfd > maxfd) maxfd = tmpfd;
        }

        QCRIL_HAL_LOG_VERBOSE("Waiting for fds");
        errno = 0;
        rc = pselect(maxfd + 1, &readfds, &writefds, &exceptfds, NULL, NULL);
        QCRIL_HAL_LOG_VERBOSE("Woke up. rc: %d. errno: %d", rc, errno);

        if (FD_ISSET(queue_link[SP_RECEIVER], &readfds))
        {
            int c;
            ssize_t read = -1;
            read = ::read(queue_link[SP_RECEIVER], &c, 1);
            if (read <= 0)
            {
                QCRIL_HAL_LOG_HIGH("Unexpected return %d from read. errno: %d", read, errno);
            }

            std::lock_guard<std::mutex> lock(queue_lock);
            const std::shared_ptr<Event> evt = std::move(queue[0]);
            queue.pop_front();
            if (evt)
            {
                QCRIL_HAL_LOG_VERBOSE("Calling evt->dispatch()");
                evt->dispatch();
            }
        }
        for ( std::unique_ptr<EventSource> &source: sources)
        {
            source->checkEvents(readfds, writefds, exceptfds);
        }
    }
    QCRIL_HAL_LOG_DEBUG("Ending loop");
}

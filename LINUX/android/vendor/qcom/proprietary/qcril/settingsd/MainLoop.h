/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef MAINLOOP_H_INCLUDED
#define MAINLOOP_H_INCLUDED

#include <memory>
#include <deque>
#include <iostream>

extern "C" {
#include <sys/select.h>
};

namespace QcSettingsD {

class Event
{
    public:
    virtual bool dispatch() = 0;

    virtual ~Event(){}
};

class EventDispatcher
{
    protected:
    virtual void dispatchEvent(std::shared_ptr<Event> e);
    virtual ~EventDispatcher() {};
};

class EventSource: virtual public EventDispatcher
{
    public:
    virtual ~EventSource() {};
    friend class MainLoop;
    private:
    /** Update the fd sets this event source is interested in
      * @return The maximum fd provided by the source in any of the fd sets
      */
    virtual int updateFdSet(fd_set &readfds, fd_set &writefds, fd_set &exceptfds) = 0;
    virtual int checkEvents(fd_set &readfds, fd_set &writefds, fd_set &exceptfds) = 0;

    protected:
};

class MainLoop: virtual public EventDispatcher
{
    public:
    MainLoop();
    ~MainLoop();
    void localQueueEvent( std::shared_ptr<Event> &evt );
    static void startLoop();
    static void addEventSource( std::unique_ptr<EventSource> &src );
    static bool queueEvent( std::shared_ptr<Event> &evt );
    static void endLoop();
    static std::shared_ptr<MainLoop> getThreadLoop();

    private:
    void doAddEventSource( std::unique_ptr<EventSource> &src );
    void doQueueEvent(std::shared_ptr<Event> evt);
    void doEndLoop();
    void loop();

    bool goOn;
    std::deque<std::unique_ptr<EventSource>> sources;
    std::deque<std::shared_ptr<Event>> queue;
    std::mutex queue_lock;
    int queue_link[2];
    void wakeup_thread();
};

}
#endif

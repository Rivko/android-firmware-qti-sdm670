/******************************************************************************
  @file    EventQueue.cpp
  @brief   Implementation of event queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "EventQueue.h"
#include "stdlib.h"

#define LOG_TAG "ANDR-PERF-EVENT-QUEUE"
#include <cutils/log.h>

using namespace std;

EventQueue::EventQueue() {
    QLOGI("EventManager::EventManager");
    update_available = false;
    pending_event_updates = 0;
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

EventQueue::~EventQueue() {
    QLOGI("~EventManager");
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

void EventQueue::Wakeup(EventData* data)
{
    lock();
    update_available = true;
    addTriggerUpdateToPriorityQ(data);
    pthread_cond_signal(&mCond);
    unlock();
}

EventData* EventQueue::Wait()
{
    EventData *event_data = NULL;

    lock();

    while(!update_available) {
        QLOGI("EventManagerThread Waiting for update");
        QLOGI("event_queue size = %d", event_queue.getSize());
        pthread_cond_wait(&mCond, &mMutex);
    }

    QLOGI("EventManagerThread processing trigger update.");
    QLOGI("event_queue size = %d", event_queue.getSize());

    event_data = readTriggerUpdatesFromPriorityQ();

    if(!event_queue.isEmpty()) {
        update_available = true;
    } else {
        update_available = false;
    }

    unlock();

    return event_data;
}

void EventQueue::addTriggerUpdateToPriorityQ(EventData* data) {
    event_queue.insert(data,0);
    pending_event_updates++;
}

EventData *EventQueue::readTriggerUpdatesFromPriorityQ() {
    EventData *dataToProcess = NULL;

    if(!event_queue.isEmpty()) {
        dataToProcess = (EventData *)event_queue.getData();
    }
    pending_event_updates--;
    QLOGI("pending_event_updates = %d", pending_event_updates);

    return dataToProcess;
}


/******************************************************************************
 *   @file    Action.h
 *   @brief   Abstract base class for "Action"s
 *
 *   DESCRIPTION
 *   Action class is an abstract class for Actions to derive from. Action
 *   objects contain the data structures related to output like OutputDB
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef ACTION_H
#define ACTION_H

#include "AsyncData.h"

class Action {
public:
    virtual ~Action() {}
    /*
     * Actions should implement allocation, maintenance and destruction of
     * resources for OutputDb
     * Actions should override the respective getter function returning
     * void* to the OutputDb for Feature to pass to Algorithm.
     */
    virtual void* getActionOutputDb() = 0;

    virtual void applyAction(const AsyncTriggerPayload& payload) = 0;

};

#endif /* ACTION_H */

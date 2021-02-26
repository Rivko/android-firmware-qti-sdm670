/******************************************************************************
 *   @file    LMPerfGlue.cpp
 *   @brief   The Glue for exposing and registering LM APIs with perf HAL
 *
 *   DESCRIPTION
 *      LMPerfGlue defines and registers LearningModule API's with perf-hal in
 *   the standard format defined by perf-hal.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include <vector>
#include <algorithm>
#include <utils/Timers.h>
#include "PerfGlueLayer.h"
#include "internal/LearningModule.h"
#include "mp-ctl.h"
#include "DebugLog.h"
#include "AsyncData.h"

#define LOG_TAG_LMPG "LMPerfGlue"

#define ASYNC_RANGE_START_NOT_INTERESTED -1
#define ASYNC_RANGE_END_NOT_INTERESTED -1
#ifndef LM_LIB_NAME
#define LM_LIB_NAME "liblearningmodule.so"
#endif

/*
 * dlopen() calls this function when this library is dynamically loaded
 * This syntax is compiler specific to GCC & CLANG and needs to be
 * revisited if we're using a different compiler.
 *
 * This helps remove static scope defined variables and objects which
 * are only static because they're required to be defined / constructor
 * called before anything else.
 *
 * Use this to create asyncsOfInterest and call PerfGlueLayer()
 * and register our module with perf-hal
 */
void __attribute__ ((constructor)) initialize()
{
    AsyncIdMap asyncMap;
    std::vector<int> asyncsOfInterest;
    /*
     * Construct the Async List to be registered with Perf HAL
     * The list format is either a range or a list of values:
     *  If interested in a range, the range start and end are given as first
     *  and second items in the list.
     *  If interested in particular events, range start and end are
     *  specified as -1 followed by a list of individual Async events that
     *  we're interested in.
     *
     * The Async List is populated dynamically from AsyncIdMap
     * Every element of AsyncIdMap gets automatically added to the
     * asyncsOfInterest and forms an array similar to
     *    {
     *    ASYNC_RANGE_START_NOT_INTERESTED,
     *    ASYNC_RANGE_END_NOT_INTERESTED,
     *    VENDOR_HINT_DISPLAY_OFF,
     *    VENDOR_HINT_DISPLAY_ON,
     *    ... etc.
     *    }
     * This is then used to register with perf HAL using PerfGlueLayer()
     */
    asyncsOfInterest.push_back(ASYNC_RANGE_START_NOT_INTERESTED);
    asyncsOfInterest.push_back(ASYNC_RANGE_END_NOT_INTERESTED);
    DEBUGV(LOG_TAG_LMPG,"Creating asyncsOfInterest");
    std::transform(asyncMap.begin(),
            asyncMap.end(),
            std::back_inserter(asyncsOfInterest),
            [](const auto& keyvaluepair){return keyvaluepair.second;}
            );

    /*
     * Call PerfGlueLayer constructor to register the module for perf HAL.
     *
     * The lmPerfGlue object is used only to call the PerfGlueLayer()
     * constructor which registers the library in it's scope by copying
     * the values passed.
     *
     * Hence, lmPerfGlue need not be present in memory after Constructor
     * exits ==> No need of the object to be static
     */
    DEBUGV(LOG_TAG_LMPG,"Calling PerfGlueLayer()");
    PerfGlueLayer lmPerfGlue {
        LM_LIB_NAME,
        &asyncsOfInterest[0],
        static_cast<int>(asyncsOfInterest.size())
    };
}

/*
 * perfmodule_init() is called during initialization of perf HAL
 * The function name is looked up as a symbol from .so by Perf HAL:
 * DO NOT change the function name and signature!
 */
int perfmodule_init() {
    /*
     * Initialize LM by calling getInstance() for the first time.
     */
    DEBUGV(LOG_TAG_LMPG,"perfmodule_init() calling LM::getInstance()");
    LearningModule::getInstance();
    return 0;
}

/*
 * perfmodule_exit() is called during exit of perf HAL
 * The function name is looked up as a symbol from .so by Perf HAL:
 * DO NOT change the function name and signature!
 */
void perfmodule_exit() {
    /*
     * Do Nothing - LM Instance gets destroyed automatically when the program exits
     * as LM instance has function static scope.
     */
    DEBUGV(LOG_TAG_LMPG,"perfmodule_exit()");
}

/*
 * perfmodule_submit_request() is called by perf HAL to send the requests/triggers
 * it gets from the system. This is equivalent of LM::trigger()
 * The function name is looked up as a symbol from .so by Perf HAL:
 * DO NOT change the function name and signature!
 */
int perfmodule_submit_request(mpctl_msg_t *msg) {
    DEBUGV(LOG_TAG_LMPG,"perfmodule_submit_request()");
    if(msg != 0) {
        nsecs_t now = systemTime();
        AsyncTriggerPayload payload(msg);
        payload.timeStamp = now;
        LearningModule::getInstance().trigger(payload);
        return 0;
    }
    return -1;
}

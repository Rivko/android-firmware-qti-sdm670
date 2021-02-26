/******************************************************************************
 *   @file    AsyncData.h
 *   @brief   AsyncId and AsyncTrigger related helpers
 *
 *   DESCRIPTION
 *      AsyncId is the internal representation of hintID from perf-hal. These
 *   definitions are used to map perf-hal provided hintID's and request to their
 *   internal representations.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef ASYNCDATA_H
#define ASYNCDATA_H
#include <unordered_map>
#include <string>
#include <utils/Timers.h>
#include "mp-ctl.h"

#define LMTESTS_HINTTYPE 99
#define DEFAULT_HINTTYPE -1

/**
 * AsyncTriggerPayload is payload received from perf HAL along with the Async
 * triggers. This is just an alternate internal LM representation of
 * mpctl_msg_t.
 *
 * The mpctl_msg_t type from mp-ctl.h has the following members:
 *  Name            type          Comments
 *  data           uint16_t      PerfLock specific (number of perflock args)
 *  pl_handle      int32_t       PerfLock specific (perflock handle)
 *  req_type       uint8_t       PerfLock specific (perflock type - would be HINT for us)
 *  profile        int32_t       PerfLock specific (profile)
 *  pl_time        int32_t       Duration of Hint
 *  pl_args        int32_t[]     PerfLock specific (perflock args)
 *  client_pid     pid_t         PID of the client
 *  client_tid     pid_t         TID of the client
 *  hint_id        uint32_t      Actual Async Hint
 *  hint_type      int32_t       Async Hint may have a subtype
 *  *userdata      void          Reserved - Not used currently
 *  usrdata_str    char[]        Appname
 *
 * All the members relevant to LM (non-perflock-specific members) are mapped
 * to equivalent internal members in AsyncTriggerPayload.
 */

using type_hintid = uint32_t;
using type_hinttype = int32_t;

struct AsyncTriggerPayload {
    type_hintid hintID;    /* mpctl_msg_t.hint_id */
    type_hinttype hintType;           /* mpctl_msg_t.hint_type */
    std::string appName;    /* mpctl_msg_t.usrdata_str */
    int duration;           /* mpctl_msg_t.pl_time */
    pid_t appPID;           /* mpctl_msg_t.client_pid */
    pid_t appThreadTID;     /* mpctl_msg_t.client_tid */
    nsecs_t timeStamp;      /* Time when the Async arrived */

    /* Default Constructor */
    AsyncTriggerPayload() {
        hintID = 0;
        hintType = DEFAULT_HINTTYPE;
        duration = 0;
        appPID = 0;
        appThreadTID = 0;
        timeStamp = 0;
    }

    /* Constructor from mpctl_msg_t */
    AsyncTriggerPayload(mpctl_msg_t* msg) {
        hintID = msg->hint_id;
        hintType = msg->hint_type;
        appName = msg->usrdata_str;
        duration = msg->pl_time;
        appPID = msg->client_pid;
        appThreadTID = msg->client_tid;
        timeStamp = 0;
    }
};

/**
 * AsyncId / hintID is a unique value for each of mpctl_msg_t->hint_id
 * available from HAL layer. It is taken directly from mp-ctl.h
 * For Example:
 *      VENDOR_HINT_FIRST_LAUNCH_BOOST
 *      VENDOR_HINT_FIRST_DRAW
 *      etc.
 *
 * AsyncIdMap is a map of strings and corresponding enum values for AsyncId.
 * This map is used for two purposes:
 *  1. It is used to convert the string read from the xml file into
 * corresponding AsyncId / hintId.
 *  2. It is used to populate asyncsOfInterest dynamically to register with
 * perf-hal for getting those hints.
 *
 * NOTE: Add a this->operator line for each new hintID used by LM features.
 *
 * AsyncIdMap is a struct instead of class to make all the members public
 */
struct AsyncIdMap : std::unordered_map<std::string, type_hintid>{
    AsyncIdMap() {
        this->operator []("VENDOR_HINT_FIRST_LAUNCH_BOOST") = VENDOR_HINT_FIRST_LAUNCH_BOOST;
        this->operator []("VENDOR_HINT_FIRST_DRAW") = VENDOR_HINT_FIRST_DRAW;
        this->operator []("VENDOR_HINT_DISPLAY_OFF") = VENDOR_HINT_DISPLAY_OFF;
        this->operator []("VENDOR_HINT_DISPLAY_ON") = VENDOR_HINT_DISPLAY_ON;
        this->operator []("VENDOR_HINT_TAP_EVENT") = VENDOR_HINT_TAP_EVENT;
    }
    ~AsyncIdMap() {}
};

#endif /* ASYNCDATA_H */

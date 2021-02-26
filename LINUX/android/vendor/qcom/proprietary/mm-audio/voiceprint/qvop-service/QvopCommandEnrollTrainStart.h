/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_CAPTURE_H__
#define __QVOP_COMMAND_ENROLL_CAPTURE_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint Enrollment data processing calls.
     */
    class QvopCommandEnrollTrainStart : public QvopCommandService {
    public:
        QvopCommandEnrollTrainStart() { }

    protected:
        virtual void execute(IQvopCallback&) {
            //no calls to secure environment at this time
        }

    private:

    };
}
#endif  // __QVOP_COMMAND_ENROLL_CAPTURE_H__

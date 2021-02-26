/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_COMMIT_H__
#define __QVOP_COMMAND_ENROLL_COMMIT_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint Enrollment completion. Temlate should be saved.
     */
    class QvopCommandEnrollCommit : public QvopCommandService {
    public:
        QvopCommandEnrollCommit() { }

    protected:
        virtual void execute(IQvopCallback& callback) {

            QvopStatus enrollStatus = QvopEnrollCommit();
            if (QVOP_SUCCESS != enrollStatus) {
                logfile_print_e("%s: QvopEnrollCommit failed", QVOP_FN);
            }

            logfile_print_i("%s:QvopEnrollCommit success ret=%d", QVOP_FN, enrollStatus);

            QvopStatus res = QvopSessionEnd();
            if (QVOP_SUCCESS != res) {
                logfile_print_v("%s: QvopSessionEnd failed", QVOP_FN);
            }

            logfile_print_i("%s: SessionEnd ret=%d", QVOP_FN, res);
            callback.onEnrollStatus(enrollStatus);
        }


    private:


    };
}
#endif  // __QVOP_COMMAND_ENROLL_COMMIT_H__
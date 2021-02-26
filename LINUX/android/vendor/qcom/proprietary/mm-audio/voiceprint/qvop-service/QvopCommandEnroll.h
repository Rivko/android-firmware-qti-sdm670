/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_START_H__
#define __QVOP_COMMAND_ENROLL_START_H__


#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command forPrint Enrollment data processing calls.
     */

    class QvopCommandEnroll : public QvopCommandService {
    public:
        QvopCommandEnroll(const char* user_id, const char* identifier, const char* keyphrase) {
            //struct qvop_enroll_params enroll_params;
            int length = strlen(keyphrase);
            strlcpy(enroll_params.keyphrase, keyphrase, QVOP_MAX_KEYPHRASE_LEN);
            length = strlen(user_id);
            strlcpy(enroll_params.enrollee_id, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
            length = strlen(identifier);
            strlcpy(enroll_params.keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
            enroll_params.mode = keyphrase_mode;
        }

    protected:
        virtual void execute(IQvopCallback& callback) {
            logfile_print_v("%s: entry", QVOP_FN);

            QvopStatus res = tryInitSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: QVOPQvopSessionInit failed", QVOP_FN);
                callback.onError(res);
                return;
            }

            logfile_print_v("%s: TZ loaded.", QVOP_FN);

            res = QvopEnrollBegin(&enroll_params);

            if (res == QVOP_SUCCESS || res == QVOP_PCM_DATA_REQUEST) {
                logfile_print_i("%s: QvopEnrollBegin success res=%d", QVOP_FN, res);
            }
            else  {
                logfile_print_e("%s: QvopEnrollBegin failed res=%d", QVOP_FN, res);
                callback.onEnrollStatus(res);
            }
        }

    private:

        struct qvop_enroll_params enroll_params;

    };
}
#endif  // __QVOP_COMMAND_ENROLL_START_H__